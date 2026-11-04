
#include <iostream>
#include <string>

#include "page.h"
#include "storage_manager.h"

int main(int argc, char* argv[]) {

    // Check command-line arguments
    if (argc != 2) {
        std::cout << "Usage:\n";
        std::cout << "./storage_demo.exe write\n";
        std::cout << "./storage_demo.exe read\n";
        return 1;
    }

    std::string operation = argv[1];

    try {
        // Create storage manager
        StorageManager storage("database.db");

        if (operation == "write") {

            Page page;

            // Store data inside the page
            page.writeString("Hello Database!");

            // Write page 0 to disk
            storage.writePage(0, page);

            std::cout << "Data saved to disk!\n";
        }

        else if (operation == "read") {

            Page page;

            // Read page 0 from disk
            storage.readPage(0, page);

            // Display retrieved data
            std::cout << "Data retrieved: "
                      << page.readString()
                      << std::endl;
        }

        else {
            std::cout << "Invalid operation!\n";
            return 1;
        }
    }

    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}
