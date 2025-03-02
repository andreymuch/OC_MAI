#include <iostream>
#include <thread>
#include <set>
#include <string>
#include <algorithm>
#include <sstream>
#include <zmq.hpp>
#include <windows.h>
#include <mutex>
#include "binary_tree.h"


BinaryTree tree;
std::mutex tree_mutex;
std::mutex set_mutex;
std::set<int> ping_list;

void server_worker(zmq::context_t& context) {
    zmq::socket_t router_socket(context, zmq::socket_type::router);
    router_socket.set(zmq::sockopt::router_mandatory, 1);
    router_socket.bind("tcp://*:5555");

    zmq::socket_t pull_socket(context, zmq::socket_type::pull);
    pull_socket.connect("inproc://server_pull");

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

            if (clients.insert(client_str).second) {
                std::cout << "New client connected: ID = " << client_str << std::endl;
            }

            std::cout << "Received from client: "  << message << std::endl;
            std::istringstream iss(message);
            std::string command, id;
            iss >> command >> id;

            if (command=="pong"){
                std::lock_guard<std::mutex> lock(set_mutex);
                ping_list.insert(std::stoi(id));

            }
        }

        if (items[1].revents & ZMQ_POLLIN) {
            zmq::message_t command_msg;
            pull_socket.recv(command_msg, zmq::recv_flags::none);

            std::string command(static_cast<char*>(command_msg.data()), command_msg.size());
            
            for (auto it = clients.begin();  it != clients.end();) {
                zmq::message_t forward_msg(command.begin(), command.end());
                try{
                    router_socket.send(zmq::message_t(it->begin(), it->end()), zmq::send_flags::sndmore);
                    router_socket.send(forward_msg, zmq::send_flags::none);
                    ++it;
                } catch (const zmq::error_t&){
                    std::cerr << "Failed to forward message to client " << *it << std::endl;
                    it = clients.erase(it);
                }
            }
        }
    }
}

void startWorkerProcess(const std::string& server_address, const std::string& child_port, int id) {
    std::string command = "worker.exe " + server_address + " " + child_port + " " + std::to_string(id);
    STARTUPINFOA si = { sizeof(si) };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW; //консоль
    PROCESS_INFORMATION pi;
    if (CreateProcessA(NULL, &command[0], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::cerr << "Failed to start worker process for node " << id << std::endl;
    }
}


void  startPingAllNodesThred(){
    Sleep(3000);
    std::set<int> deadNodes;

    {
        std::lock_guard<std::mutex> lock(set_mutex);
        for (const auto& id : ping_list) {
            std::string message = "client ID: "+ std::to_string(id)+ " отвечает";
            std::cout << message << std::endl;
        }
        std::lock_guard<std::mutex> lock1(tree_mutex);
        std::set_difference(tree.getIds().begin(), tree.getIds().end(), ping_list.begin(), ping_list.end(), std::inserter(deadNodes, deadNodes.begin()));
    }

    
    {std::lock_guard<std::mutex> lock(tree_mutex);
        for (const auto& id : deadNodes) {
            tree.deleteNode(id);
            std::cout << "Узел " << id << " не отвечает и будет удален из дерева" << std::endl;
        }
    }
    
    
}

int main() {
    zmq::context_t context(1);

    zmq::socket_t push_socket(context, zmq::socket_type::push);
    push_socket.bind("inproc://server_pull");

    std::thread worker(server_worker, std::ref(context));

    while (true) {
        std::string input;
        std::getline(std::cin, input);

        if (!input.empty()) {
            std::istringstream iss(input);
            std::string command;
            int id;
            iss >> command >> id;
            
            if (command == "create" && !iss.fail()) {
                std::lock_guard<std::mutex> lock(tree_mutex);
                if (tree.isEmpty()) {
                    if (tree.createRoot(id)) {
                        std::string child_port = "tcp://127.0.0.1:" + std::to_string(5555 + id);
                        tree.addIpAndPort(id, "tcp://127.0.0.1:5555", child_port);
                        std::cout << "Root node " << id << " created, connects to server on port 5555"
                                << " and listens on " << child_port << std::endl;
                        startWorkerProcess("tcp://127.0.0.1:5555", child_port, id);
                    }
                } else {
                    int parent_id = tree.addNode(id);
                    if (parent_id != -1) {
                        std::string parent_port = "tcp://127.0.0.1:" + std::to_string(5555 + parent_id);
                        std::string child_port = "tcp://127.0.0.1:" + std::to_string(5555 + id);
                        tree.addIpAndPort(id, parent_port, child_port);
                        std::cout << "Node " << id << " created with parent " << parent_id 
                                << ", connects to " << parent_port 
                                << " and listens on " << child_port << std::endl;
                        startWorkerProcess(parent_port, child_port, id);
                    } else {
                        std::cerr << "Failed to add node " << id << " to the tree!" << std::endl;
                    }
                    
                }
            
            }else if (command == "print"){
                tree.print();
            }else if(command == "pingall"){
                {std::lock_guard<std::mutex> lock(set_mutex);
                    ping_list.clear();
                }
                zmq::message_t command_msg(input.data(), input.size());
                push_socket.send(command_msg, zmq::send_flags::none);
                std::thread thred(startPingAllNodesThred);
                thred.detach();
            } 
            else {
                zmq::message_t command_msg(input.data(), input.size());
                push_socket.send(command_msg, zmq::send_flags::none);
            }
        }
    }

    worker.join();
    return 0;
}
