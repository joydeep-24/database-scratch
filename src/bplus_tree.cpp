
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class BPlusTree {
private:
    static const int MAX_KEYS = 3;

    struct Node {
        vector<pair<int, int>> entries;
        Node* next;

        Node() : next(nullptr) {}
    };

    Node* head;

public:
    BPlusTree() {
        head = new Node();
    }

    // Find the appropriate leaf and insert a key.
    void insert(int key, int recordId) {
        Node* current = head;

        // Find the leaf where this key belongs.
        while (current->next != nullptr &&
               current->entries.back().first < key) {
            current = current->next;
        }

        // Find sorted insertion position.
        auto it = lower_bound(
            current->entries.begin(),
            current->entries.end(),
            key,
            [](const pair<int, int>& entry, int value) {
                return entry.first < value;
            }
        );

        // Avoid duplicate keys.
        if (it != current->entries.end() &&
            it->first == key) {
            cout << "Duplicate key: " << key << '\n';
            return;
        }

        current->entries.insert(it, {key, recordId});

        // Check for overflow.
        if (current->entries.size() > MAX_KEYS) {
            splitLeaf(current);
        }
    }

    // Split an overflowing leaf into two leaves.
    void splitLeaf(Node* current) {
        Node* newLeaf = new Node();

        int mid = current->entries.size() / 2;

        // Move the second half of entries.
        newLeaf->entries.assign(
            current->entries.begin() + mid,
            current->entries.end()
        );

        current->entries.erase(
            current->entries.begin() + mid,
            current->entries.end()
        );

        // Maintain linked leaf structure.
        newLeaf->next = current->next;
        current->next = newLeaf;

        cout << "Leaf split occurred!\n";
    }

    // Search for a key and return its record ID.
    int search(int key) const {
        Node* current = head;

        while (current != nullptr) {
            if (!current->entries.empty() &&
                key <= current->entries.back().first) {

                auto it = lower_bound(
                    current->entries.begin(),
                    current->entries.end(),
                    key,
                    [](const pair<int, int>& entry, int value) {
                        return entry.first < value;
                    }
                );

                if (it != current->entries.end() &&
                    it->first == key) {
                    return it->second;
                }

                return -1;
            }

            current = current->next;
        }

        return -1;
    }

    // Display all linked leaf nodes.
    void display() const {
        Node* current = head;
        int count = 1;

        while (current != nullptr) {
            cout << "Leaf " << count++ << ": ";

            for (const auto& entry : current->entries) {
                cout << "(" << entry.first
                     << ", " << entry.second << ") ";
            }

            cout << " -> ";
            current = current->next;
        }

        cout << "NULL\n";
    }

    ~BPlusTree() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

int main() {
    BPlusTree tree;

    tree.insert(10, 100);
    tree.insert(20, 200);
    tree.insert(30, 300);
    tree.insert(40, 400);
    tree.insert(50, 500);
    tree.insert(60, 600);
    tree.insert(70, 700);

    cout << "\nB+Tree Leaf Structure:\n";
    tree.display();

    cout << "\nSearching:\n";

    cout << "Key 40: " << tree.search(40) << '\n';
    cout << "Key 70: " << tree.search(70) << '\n';
    cout << "Key 99: " << tree.search(99) << '\n';

    return 0;
}
