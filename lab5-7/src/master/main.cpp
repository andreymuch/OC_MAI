#include <iostream>
#include <thread>
#include <set>
#include <string>
#include <zmq.hpp>

void server_worker(zmq::context_t& context) {
    zmq::socket_t router_socket(context, zmq::socket_type::router);
    router_socket.set(zmq::sockopt::router_mandatory, 1);
    router_socket.bind("tcp://*:5555");

    zmq::socket_t pull_socket(context, zmq::socket_type::pull);
    pull_socket.bind("inproc://server_pull");

    std::set <std::string> clients;

    zmq::pollitem_t items[] = {
        { static_cast<void*>(router_socket), 0, ZMQ_POLLIN, 0 },
        { static_cast<void*>(pull_socket), 0, ZMQ_POLLIN, 0 }
    };

    while (true) {
        zmq::poll(items, 2, std::chrono::milliseconds(-1));

        if (items[0].revents & ZMQ_POLLIN) {
            zmq::message_t client_id, request;
            router_socket.recv(client_id, zmq::recv_flags::none);
            router_socket.recv(request, zmq::recv_flags::none);

            std::string client_str(static_cast<char*>(client_id.data()), client_id.size());
            std::string message(static_cast<char*>(request.data()), request.size());



            if (clients.insert(client_str).second == true) {
                std::cout << "New client connected: ID = " << client_str << std::endl;
            }

            std::cout << "Received from client: "  << message << std::endl;
        }

        if (items[1].revents & ZMQ_POLLIN) {
            zmq::message_t command_msg;
            pull_socket.recv(command_msg, zmq::recv_flags::none);

            std::string command(static_cast<char*>(command_msg.data()), command_msg.size());
            
            for (const auto& client : clients) {
                zmq::message_t forward_msg(command.begin(), command.end());
                try{
                    router_socket.send(zmq::message_t(client.begin(), client.end()), zmq::send_flags::sndmore);
                    router_socket.send(forward_msg, zmq::send_flags::none);
                }catch (const zmq::error_t&){
                    std::cout << "Failed to forward message to client " << client << std::endl;
                    clients.erase(client);
                    break;
                }
            }
        }
    }
}

int main() {
    zmq::context_t context(1);

    zmq::socket_t push_socket(context, zmq::socket_type::push);
    push_socket.connect("inproc://server_pull");

    std::thread worker(server_worker, std::ref(context));

    while (true) {
        std::string input;
        std::getline(std::cin, input);

        if (!input.empty()) {
            zmq::message_t command_msg(input.data(), input.size());
            push_socket.send(command_msg, zmq::send_flags::none);
        }
    }

    worker.join();
    return 0;
}
