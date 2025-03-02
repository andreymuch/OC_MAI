
#include <zmq.hpp>
#include <iostream>
#include <thread>
#include <unordered_set>
#include <sstream>
#include <map>


void process_command(std::string name, std::string value, zmq::context_t& context, std::string& client_id) {
    static std::map <std::string, std::string> data;
    std::string response;

    if (value.empty() && !name.empty()){
        if (data.find(name) != data.end()){
            response = " значение по ключу: " + name +" у worker: "+ client_id + " " + data[name];

        }else{
            response = "значение не найдено по ключу: " + name + " у worker: "+ client_id;
        }
    }else if(!name.empty()&& !value.empty()) {
        data[name] = value;
        response = "значение по ключу: " + name + " у worker: "+ client_id + " установлено"; 
        
    }
    else{
        response = "неправильные данные";
    }




    zmq::socket_t push_socket(context, zmq::socket_type::push);
    push_socket.connect("inproc://worker");
    
    
    zmq::message_t msg(response.begin(),response.end());
    push_socket.send(msg, zmq::send_flags::none);
}
int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: client <server_address> <router_bind_address> <client_id>" << std::endl;
        return 1;
    }
   
    std::string server_address = argv[1];
    std::string router_bind_address = argv[2];
    std::string client_id = argv[3];

    std::cout<< "Cleient iD: " + client_id<< std::endl;
    
    
    zmq::context_t context;
    
    zmq::socket_t dealer_socket(context, zmq::socket_type::dealer);
    dealer_socket.set(zmq::sockopt::routing_id, client_id);
    dealer_socket.connect(server_address);

    zmq::socket_t router_socket(context, zmq::socket_type::router);
    router_socket.set(zmq::sockopt::router_mandatory, 1);
    router_socket.bind(router_bind_address);
    

    zmq::socket_t pull_socket(context, zmq::socket_type::pull);
    pull_socket.bind("inproc://worker");
    
    zmq::socket_t monitor_socket(context, zmq::socket_type::pair);
    zmq_socket_monitor(dealer_socket, "inproc://monitor", ZMQ_EVENT_DISCONNECTED);
    monitor_socket.connect("inproc://monitor");
    
    std::unordered_set<std::string> connected_clients;
    zmq::pollitem_t items[] = {
        { static_cast<void*>(dealer_socket), 0, ZMQ_POLLIN, 0 },
        { static_cast<void*>(router_socket), 0, ZMQ_POLLIN, 0 },
        { static_cast<void*>(pull_socket), 0, ZMQ_POLLIN, 0 },
        { static_cast<void*>(monitor_socket), 0, ZMQ_POLLIN, 0 }
    };

    dealer_socket.send(zmq::buffer("Hello from client"+ client_id), zmq::send_flags::none);
    
    while (true) {
        zmq::poll(items, 4);
        
        if (items[0].revents & ZMQ_POLLIN) {
            zmq::message_t msg;
            dealer_socket.recv(msg);
            std::string received_command(static_cast<char*>(msg.data()), msg.size());

            std::cout <<"server command: " + received_command << std::endl;
            std::istringstream iss(received_command);
            std::string id, command, name, value;
            iss >> command >> id >> name >> value;
            
            if (id == client_id && command == "exec" ) {
                std::thread(process_command, name, value, std::ref(context),client_id).detach();
                continue;
            } 

            if (command == "pingall") {
                std::string message = "pong " + client_id;
                std::cout << message << std::endl;
                zmq::message_t msg(message.begin(), message.end());
                dealer_socket.send(msg, zmq::send_flags::none);
            }
              
            for (auto it = connected_clients.begin();  it != connected_clients.end();) {
                zmq::message_t forward_msg(received_command.begin(), received_command.end());
                try{
                    router_socket.send(zmq::message_t(it->begin(), it->end()), zmq::send_flags::sndmore);
                    router_socket.send(forward_msg, zmq::send_flags::none);
                    ++it;
                }catch (const zmq::error_t&){
                    std::cerr << "Failed to forward message to client " << *it << std::endl;
                    it = connected_clients.erase(it);
                }
            }
            
        }
        
        if (items[1].revents & ZMQ_POLLIN) {
            zmq::message_t client_id_msg, msg;
            router_socket.recv(client_id_msg);
            router_socket.recv(msg);
            
            std::string connected_client_id(static_cast<char*>(client_id_msg.data()), client_id_msg.size());
            if (connected_clients.insert(connected_client_id).second) {
                std::cout<<"client connected ID: " + connected_client_id<<std::endl;
            }
            
            dealer_socket.send(msg, zmq::send_flags::none);
        }

        if (items[2].revents & ZMQ_POLLIN) {
            zmq::message_t msg;
            pull_socket.recv(msg);
            std::cout<< "result for srver massage:"+ std::string(static_cast<char*>(msg.data()), msg.size())<<std::endl;
            dealer_socket.send(msg, zmq::send_flags::none);
        }
        
        if (items[3].revents & ZMQ_POLLIN) {
            std::cerr << "Disconnected from server. Exiting..." << std::endl;
            break;
        }
    }
    
    return 0;
}
