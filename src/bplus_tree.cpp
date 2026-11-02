
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


    // Collect leaf nodes using the tree hierarchy.
    void collectLeaves(Node* node, vector<Node*>& leaves) const {
        if (node->isLeaf) {
            leaves.push_back(node);
            return;
        }

        for (Node* child : node->children) {
            collectLeaves(child, leaves);
        }
    }

    // Validate a node recursively.
    bool validateNode(
        Node* node,
        int depth,
        int& leafDepth
    ) const {

        if (node == nullptr) {
            return false;
        }

        // Check node capacity.
        if (node->isLeaf) {

            if (node->entries.size() > MAX_KEYS) {
                return false;
            }

            // Check sorted leaf entries.
            for (size_t i = 1; i < node->entries.size(); i++) {
                if (node->entries[i - 1].first >=
                    node->entries[i].first) {
                    return false;
                }
            }

            // Every leaf must have the same depth.
            if (leafDepth == -1) {
                leafDepth = depth;
            } else if (leafDepth != depth) {
                return false;
            }

            return true;
        }

        // Check internal node capacity.
        if (node->keys.size() > MAX_KEYS) {
            return false;
        }

        // Internal nodes need keys.size() + 1 children.
        if (node->children.size() != node->keys.size() + 1) {
            return false;
        }

        // Check sorted separator keys.
        for (size_t i = 1; i < node->keys.size(); i++) {
            if (node->keys[i - 1] >= node->keys[i]) {
                return false;
            }
        }

        // Validate every child.
        for (Node* child : node->children) {
            if (!validateNode(child, depth + 1, leafDepth)) {
                return false;
            }
        }

        return true;
    }

    // Check whether linked leaves match tree traversal.
    bool validateLeafLinks() const {

        vector<Node*> leaves;
        collectLeaves(root, leaves);

        if (leaves.empty()) {
            return false;
        }

        for (size_t i = 0; i < leaves.size(); i++) {

            Node* expectedNext =
                (i + 1 < leaves.size()) ? leaves[i + 1] : nullptr;

            if (leaves[i]->next != expectedNext) {
                return false;
            }
        }

        return true;
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


    // Return all records within an inclusive range.
    vector<pair<int, int>> rangeSearch(int low, int high) const {

        vector<pair<int, int>> result;

        if (low > high) {
            return result;
        }

        // Find the leaf containing the lower bound.
        Node* current = findLeaf(low);

        // Locate the first key >= low.
        auto it = lower_bound(
            current->entries.begin(),
            current->entries.end(),
            low,
            [](const pair<int, int>& entry, int value) {
                return entry.first < value;
            }
        );

        // Traverse the linked leaves.
        while (current != nullptr) {

            while (it != current->entries.end()) {

                if (it->first > high) {
                    return result;
                }

                result.push_back(*it);
                ++it;
            }

            // Move to the next leaf.
            current = current->next;

            if (current != nullptr) {
                it = current->entries.begin();
            }
        }

        return result;
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


    bool validate() const {

        int leafDepth = -1;

        bool structureValid =
            validateNode(root, 0, leafDepth);

        if (!structureValid) {
            cout << "Tree structure validation failed!\n";
            return false;
        }

        if (!validateLeafLinks()) {
            cout << "Leaf linkage validation failed!\n";
            return false;
        }

        cout << "B+Tree validation successful!\n";
        return true;
    }

};

int main() {

    BPlusTree tree;

    // Test 1: Unsorted insertion
    cout << "\nTEST 1: Insertion\n";

    int keys[] = {
        50, 20, 80, 10, 40,
        70, 30, 90, 60, 100,
        110, 120, 130, 140, 150,
        160, 170, 180, 190, 200
    };

    for (int key : keys) {
        tree.insert(key, key * 10);

        if (!tree.validate()) {
            cout << "Insertion test failed at key "
                 << key << '\n';
            return 1;
        }
    }

    cout << "All insertions passed!\n";

    // Test 2: Search
    cout << "\nTEST 2: Searching\n";

    bool searchPassed = true;

    for (int key : keys) {
        if (tree.search(key) != key * 10) {
            cout << "Search failed for key " << key << '\n';
            searchPassed = false;
        }
    }

    if (tree.search(999) != -1) {
        cout << "Missing-key search failed!\n";
        searchPassed = false;
    }

    cout << (searchPassed ? "Search tests passed!\n"
                          : "Search tests failed!\n");

    // Test 3: Duplicate insertion
    cout << "\nTEST 3: Duplicate insertion\n";

    tree.insert(50, 9999);

    if (tree.search(50) == 500) {
        cout << "Duplicate protection passed!\n";
    } else {
        cout << "Duplicate protection failed!\n";
    }

    // Test 4: Range query
    cout << "\nTEST 4: Range query (50 - 100)\n";

    vector<pair<int, int>> records =
        tree.rangeSearch(50, 100);

    for (const auto& record : records) {
        cout << record.first << " -> "
             << record.second << '\n';
    }

    // Test 5: Final validation
    cout << "\nTEST 5: Final validation\n";

    if (tree.validate()) {
        cout << "ALL STRUCTURAL TESTS PASSED!\n";
    }

    // Display final structure
    tree.displayRoot();
    tree.displayLeaves();

    return 0;
}
