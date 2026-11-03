
#include <iostream>
#include "storage_manager.h"

using namespace std;

int main() {

    try {

        // Create a database storage manager.
        StorageManager storage("database.db");

        // Create a page.
        Page page;

        // Store information.
        page.writeString("Hello Database!");

        // Write page 0 to disk.
        storage.writePage(0, page);

        cout << "Page written successfully!\n";

        // Read page 0 from disk.
        Page loadedPage;

        storage.readPage(0, loadedPage);

        // Retrieve stored information.
        string result = loadedPage.readString();

        cout << "Data retrieved: " << result << '\n';

        cout << "Page size: "
             << Page::PAGE_SIZE << " bytes\n";

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
