
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class BPlusTree {
private:
    static const int MAX_KEYS = 3;

    struct Node {
        bool isLeaf;

        vector<int> keys;
        vector<pair<int, int>> entries;
        vector<Node*> children;

        Node* next;

        Node(bool leaf) {
            isLeaf = leaf;
            next = nullptr;
        }
    };

    struct SplitResult {
        bool split;
        int separator;
        Node* right;

        SplitResult() : split(false), separator(0), right(nullptr) {}

        SplitResult(int key, Node* node)
            : split(true), separator(key), right(node) {}
    };

    Node* root;

    // Recursive insertion
    SplitResult insertRecursive(Node* node, int key, int recordId) {

        // CASE 1: Leaf node
        if (node->isLeaf) {

            auto it = lower_bound(
                node->entries.begin(),
                node->entries.end(),
                key,
                [](const pair<int, int>& entry, int value) {
                    return entry.first < value;
                }
            );

            if (it != node->entries.end() &&
                it->first == key) {
                cout << "Duplicate key: " << key << '\n';
                return SplitResult();
            }

            node->entries.insert(it, {key, recordId});

            // No overflow
            if (node->entries.size() <= MAX_KEYS) {
                return SplitResult();
            }

            // Split leaf
            Node* right = new Node(true);

            int mid = node->entries.size() / 2;

            right->entries.assign(
                node->entries.begin() + mid,
                node->entries.end()
            );

            node->entries.erase(
                node->entries.begin() + mid,
                node->entries.end()
            );

            // Maintain linked leaves
            right->next = node->next;
            node->next = right;

            // First key of right leaf goes to parent
            int separator = right->entries.front().first;

            return SplitResult(separator, right);
        }

        // CASE 2: Internal node

        // Select child using separator keys
        int idx = upper_bound(
            node->keys.begin(),
            node->keys.end(),
            key
        ) - node->keys.begin();

        SplitResult result =
            insertRecursive(node->children[idx], key, recordId);

        if (!result.split) {
            return SplitResult();
        }

        // Insert promoted separator and new child
        node->keys.insert(
            node->keys.begin() + idx,
            result.separator
        );

        node->children.insert(
            node->children.begin() + idx + 1,
            result.right
        );

        // No internal overflow
        if (node->keys.size() <= MAX_KEYS) {
            return SplitResult();
        }

        // Split internal node
        Node* right = new Node(false);

        int mid = node->keys.size() / 2;

        int promotedKey = node->keys[mid];

        // Move keys after the promoted key
        right->keys.assign(
            node->keys.begin() + mid + 1,
            node->keys.end()
        );

        // Move corresponding child pointers
        right->children.assign(
            node->children.begin() + mid + 1,
            node->children.end()
        );

        // Keep left half in original node
        node->keys.resize(mid);
        node->children.resize(mid + 1);

        return SplitResult(promotedKey, right);
    }

    // Find the appropriate leaf
    Node* findLeaf(int key) const {
        Node* current = root;

        while (!current->isLeaf) {
            int idx = upper_bound(
                current->keys.begin(),
                current->keys.end(),
                key
            ) - current->keys.begin();

            current = current->children[idx];
        }

        return current;
    }

    void destroy(Node* node) {
        if (node == nullptr) return;

        for (Node* child : node->children) {
            destroy(child);
        }

        delete node;
    }

public:

    BPlusTree() {
        root = new Node(true);
    }

    void insert(int key, int recordId) {

        SplitResult result =
            insertRecursive(root, key, recordId);

        // Root overflow: create a new root
        if (result.split) {

            Node* newRoot = new Node(false);

            newRoot->keys.push_back(result.separator);

            newRoot->children.push_back(root);
            newRoot->children.push_back(result.right);

            root = newRoot;
        }
    }

    int search(int key) const {

        Node* leaf = findLeaf(key);

        auto it = lower_bound(
            leaf->entries.begin(),
            leaf->entries.end(),
            key,
            [](const pair<int, int>& entry, int value) {
                return entry.first < value;
            }
        );

        if (it != leaf->entries.end() &&
            it->first == key) {
            return it->second;
        }

        return -1;
    }

    void displayLeaves() const {

        Node* current = root;

        // Reach leftmost leaf
        while (!current->isLeaf) {
            current = current->children[0];
        }

        cout << "\nLeaf nodes:\n";

        while (current != nullptr) {

            cout << "[ ";

            for (const auto& entry : current->entries) {
                cout << entry.first << " ";
            }

            cout << "] -> ";

            current = current->next;
        }

        cout << "NULL\n";
    }

    void displayRoot() const {

        cout << "\nRoot keys: ";

        for (int key : root->keys) {
            cout << key << " ";
        }

        cout << '\n';
    }

    ~BPlusTree() {
        destroy(root);
    }
};

int main() {

    BPlusTree tree;

    int keys[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};

    for (int key : keys) {
        tree.insert(key, key * 10);
    }

    tree.displayRoot();
    tree.displayLeaves();

    cout << "\nSearch results:\n";

    cout << "40 -> " << tree.search(40) << '\n';
    cout << "70 -> " << tree.search(70) << '\n';
    cout << "90 -> " << tree.search(90) << '\n';
    cout << "99 -> " << tree.search(99) << '\n';

    return 0;
}
