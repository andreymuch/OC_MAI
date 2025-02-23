#pragma once
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set> // Для хранения уникальных id

class BinaryTree {
private:
    struct Node {
        int id;
        Node* left;
        Node* right;
        std::string ipAdressAndPort;

        Node(int id) : id(id), left(nullptr), right(nullptr) {}
    };

    Node* root;
    std::unordered_set<int> ids; // Множество для хранения уникальных id

    // Вспомогательная функция для поиска родителя узла по id
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

    // Вспомогательная функция для удаления поддерева
    void deleteSubtree(Node* node) {
        if (node == nullptr) {
            return;
        }
        deleteSubtree(node->left);
        deleteSubtree(node->right);
        ids.erase(node->id); // Удаляем id из множества
        delete node;
    }

    // Вспомогательная функция для печати дерева с отступами
    void printTree(Node* node, int level = 0, const std::string& prefix = "") {
        if (node == nullptr) {
            return;
        }

        // Вывод правого поддерева
        printTree(node->right, level + 1, prefix + "    ");

        // Вывод текущего узла
        std::cout << prefix;
        if (level > 0) {
            std::cout << "└── ";
        }
        std::cout << node->id << std::endl;

        // Вывод левого поддерева
        printTree(node->left, level + 1, prefix + "    ");
    }

public:
    BinaryTree() : root(nullptr) {}

    ~BinaryTree() {
        deleteSubtree(root);
    }

    // Функция добавления нового узла на минимальную глубину
    int addNode(int id) {
        // Проверка на уникальность id
        if (ids.find(id) != ids.end()) {
            std::cout << "Node with id " << id << " already exists!" << std::endl;
            return -1; // Узел с таким id уже существует
        }

        if (root == nullptr) {
            root = new Node(id);
            ids.insert(id); // Добавляем id в множество
            return -1; // Корень не имеет родителя
        }

        std::queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            Node* current = q.front();
            q.pop();

            // Если левый ребенок отсутствует, добавляем новый узел туда
            if (current->left == nullptr) {
                current->left = new Node(id);
                ids.insert(id); // Добавляем id в множество
                return current->id;
            }
            // Если правый ребенок отсутствует, добавляем новый узел туда
            if (current->right == nullptr) {
                current->right = new Node(id);
                ids.insert(id); // Добавляем id в множество
                return current->id;
            }

            // Если оба ребенка есть, добавляем их в очередь для дальнейшего обхода
            q.push(current->left);
            q.push(current->right);
        }

        return -1; // На случай, если что-то пошло не так
    }

    // Функция удаления узла и его поддерева по id
    void deleteNode(int id) {
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
            return; // Узел с таким id не найден
        }
        if (parent->left && parent->left->id == id) {
            deleteSubtree(parent->left);
            parent->left = nullptr;
        } else if (parent->right && parent->right->id == id) {
            deleteSubtree(parent->right);
            parent->right = nullptr;
        }
    }

    // Функция для печати дерева
    void print() {
        printTree(root);
    }
};

