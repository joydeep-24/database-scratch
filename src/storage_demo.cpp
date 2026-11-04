
#include <iostream>
#include <string>
#include <stdexcept>

#include "page.h"
#include "storage_manager.h"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cout << "Available commands:\n";
        std::cout << "./storage_demo.exe allocate\n";
        std::cout << "./storage_demo.exe write <page_id> <text>\n";
        std::cout << "./storage_demo.exe read <page_id>\n";
        std::cout << "./storage_demo.exe count\n";
        return 1;
    }

    try {
        StorageManager storage("database.db");

        std::string operation = argv[1];

        // Allocate a new page
        if (operation == "allocate") {

            uint32_t pageId = storage.allocatePage();

            std::cout << "New page allocated!\n";
            std::cout << "Page ID: " << pageId << "\n";
        }

        // Write data into an existing page
        else if (operation == "write") {

            if (argc < 4) {
                std::cout << "Usage: write <page_id> <text>\n";
                return 1;
            }

            uint32_t pageId = std::stoul(argv[2]);

            std::string text = argv[3];

            Page page;

            page.writeString(text);

            storage.writePage(pageId, page);

            std::cout << "Data written to page "
                      << pageId << "\n";
        }

        // Read data from a page
        else if (operation == "read") {

            if (argc < 3) {
                std::cout << "Usage: read <page_id>\n";
                return 1;
            }

            uint32_t pageId = std::stoul(argv[2]);

            Page page;

            storage.readPage(pageId, page);

            std::cout << "Data retrieved: "
                      << page.readString() << "\n";
        }

        // Display total page count
        else if (operation == "count") {

            std::cout << "Total pages: "
                      << storage.getPageCount() << "\n";
        }

        else {
            std::cout << "Invalid command!\n";
            return 1;
        }
    }

    catch (const std::exception& error) {
        std::cerr << "Error: "
                  << error.what() << "\n";

        return 1;
    }

    return 0;
}
