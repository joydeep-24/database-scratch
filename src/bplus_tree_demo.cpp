#include <iostream>
#include <vector>

using namespace std;

struct BPlusTreeNode {
    bool isLeaf;
    vector<int> keys;
    vector<int> recordIds;

    // Used to connect one leaf to the next.
    BPlusTreeNode* next;

    explicit BPlusTreeNode(bool leaf)
        : isLeaf(leaf), next(nullptr) {}
};

int main() {
    BPlusTreeNode leaf(true);

    leaf.keys = {10, 20, 30, 40};
    leaf.recordIds = {100, 200, 300, 400};

    cout << "B+Tree leaf node\n";

    for (size_t i = 0; i < leaf.keys.size(); ++i) {
        cout << "Key: " << leaf.keys[i]
             << " -> Record ID: " << leaf.recordIds[i]
             << '\n';
    }

    return 0;
}