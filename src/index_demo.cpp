#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

class Index {
private:
    // Each entry stores a key and its associated record ID.
    vector<pair<int, int>> entries;

public:
    void insert(int key, int recordId) {
        auto it = lower_bound(
            entries.begin(),
            entries.end(),
            key,
            [](const pair<int, int>& entry, int value) {
                return entry.first < value;
            }
        );

        if (it != entries.end() && it->first == key) {
            cout << "Duplicate key: " << key << '\n';
            return;
        }

        entries.insert(it, {key, recordId});
    }

    int search(int key) const {
        auto it = lower_bound(
            entries.begin(),
            entries.end(),
            key,
            [](const pair<int, int>& entry, int value) {
                return entry.first < value;
            }
        );

        if (it != entries.end() && it->first == key) {
            return it->second;
        }

        return -1;
    }

    void display() const {
        for (const auto& entry : entries) {
            cout << "Key: " << entry.first
                 << " -> Record ID: " << entry.second << '\n';
        }
    }
};

int main() {
    Index index;

    index.insert(30, 300);
    index.insert(10, 100);
    index.insert(20, 200);
    index.insert(40, 400);

    cout << "Sorted index:\n";
    index.display();

    cout << "\nSearch for key 20: "
         << index.search(20) << '\n';

    cout << "Search for key 99: "
         << index.search(99) << '\n';

    return 0;
}