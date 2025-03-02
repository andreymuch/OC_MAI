#pragma once
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set> 
#include <vector>
#include<set>
class BinaryTree {
private:
    struct Node {
        int id;
        Node* left;
        Node* right;
        std::vector<std::string> ipAdressAndPort;

        Node(int id) : id(id), left(nullptr), right(nullptr) {}
    };

    Node* root;
    std::unordered_set<int> ids; 
    
    Node* findParent(Node* current, int id) {
        if (current == nullptr) {
            return nullptr;
        }
        if ((current->left && current->left->id == id) || (current->right && current->right->id == id)) {
            return current;
        }
        Node* parent = findParent(current->left, id);
        if (parent == nullptr) {
            parent = findParent(current->right, id);
        }
        return parent;
    }


    void deleteSubtree(Node* node) {
        if (node == nullptr) {
            return;
        }
        deleteSubtree(node->left);
        deleteSubtree(node->right);
        ids.erase(node->id);
        delete node;
    }

    void printTree(Node* node, int level = 0, const std::string& prefix = "") {
        if (node == nullptr) {
            return;
        }
        printTree(node->right, level + 1, prefix + "    ");
        std::cout << prefix;
        if (level > 0) {
            std::cout << "└── ";
        }
        std::cout << node->id << std::endl;
        printTree(node->left, level + 1, prefix + "    ");
    }

public:

    BinaryTree() : root(nullptr) {}

    ~BinaryTree() {
        deleteSubtree(root);
    }
    const std::unordered_set<int>& getIds() const {
        return ids;
    }
    void addIpAndPort(int id, const std::string& parentPort, const std::string& childPort) {
        std::queue<Node*> q;
        if (root == nullptr) return;
        q.push(root);
    
        while (!q.empty()) {
            Node* current = q.front();
            q.pop();
    
            if (current->id == id) {
                current->ipAdressAndPort.push_back(parentPort);
                current->ipAdressAndPort.push_back(childPort);
                return;
            }
    
            if (current->left) q.push(current->left);
            if (current->right) q.push(current->right);
        }
    
        std::cerr << "Node with ID " << id << " not found in tree!" << std::endl;
    }

    int addNode(int id) {
        if (ids.find(id) != ids.end()) {
            std::cout << "Node with id " << id << " already exists!" << std::endl;
            return -1;
        }

        if (root == nullptr) {
            root = new Node(id);
            ids.insert(id);
            return -1;
        }

        std::queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            Node* current = q.front();
            q.pop();
            if (current->left == nullptr) {
                current->left = new Node(id);
                ids.insert(id);
                return current->id;
            }
            if (current->right == nullptr) {
                current->right = new Node(id);
                ids.insert(id);
                return current->id;
            }
            q.push(current->left);
            q.push(current->right);
        }

        return -1;
    }
    bool createRoot(int id) {
        if (root != nullptr) {
            std::cout << "Root already exists!" << std::endl;
            return false;
        }
        if (ids.find(id) != ids.end()) {
            std::cout << "Node with id " << id << " already exists!" << std::endl;
            return false;
        }
        root = new Node(id);
        ids.insert(id);
        return true;
    }

    void deleteNode(int id) {
        if (ids.find(id) == ids.end()) {
            return;
        }
        if (root == nullptr) {
            return;
        }
        if (root->id == id) {
            deleteSubtree(root);
            root = nullptr;
            return;
        }
        Node* parent = findParent(root, id);
        if (parent == nullptr) {
            std::cout << "Node with id " << id << " not found!" << std::endl;
            return;
        }
        if (parent->left && parent->left->id == id) {
            deleteSubtree(parent->left);
            parent->left = nullptr;
        } else if (parent->right && parent->right->id == id) {
            deleteSubtree(parent->right);
            parent->right = nullptr;
        }
    }
    bool isEmpty() const {
        return root == nullptr;
    }

    void print() {
        std::cout<<"Tree: "<< std::endl;
        printTree(root);
    }
};

