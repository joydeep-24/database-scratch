
#pragma once

#include "page.h"

#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdint>

class StorageManager {
private:
    std::string filename;
    std::fstream file;

public:
    // Constructor
    StorageManager(const std::string& fileName)
        : filename(fileName) {

        // Open the file for reading and writing in binary mode
        file.open(
            filename,
            std::ios::in |
            std::ios::out |
            std::ios::binary
        );

        // Create the file if it does not exist
        if (!file.is_open()) {
            std::ofstream createFile(filename, std::ios::binary);
            createFile.close();

            file.clear();

            file.open(
                filename,
                std::ios::in |
                std::ios::out |
                std::ios::binary
            );
        }

        if (!file.is_open()) {
            throw std::runtime_error("Unable to open database file");
        }
    }

    // Write a page to disk
    void writePage(uint32_t pageId, const Page& page) {
        // Calculate the page's position in the file
        std::streamoff offset =
            static_cast<std::streamoff>(pageId) * Page::PAGE_SIZE;

        file.clear();
        file.seekp(offset, std::ios::beg);

        // Write all 4096 bytes
        const auto& data = page.getData();

        file.write(
            data.data(),
            static_cast<std::streamsize>(Page::PAGE_SIZE)
        );

        file.flush();

        if (!file) {
            throw std::runtime_error("Failed to write page");
        }
    }

    // Read a page from disk
    void readPage(uint32_t pageId, Page& page) {
        // Calculate the page's position in the file
        std::streamoff offset =
            static_cast<std::streamoff>(pageId) * Page::PAGE_SIZE;

        file.clear();
        file.seekg(offset, std::ios::beg);

        // Temporary buffer for reading page data
        std::array<char, Page::PAGE_SIZE> buffer{};

        file.read(
            buffer.data(),
            static_cast<std::streamsize>(Page::PAGE_SIZE)
        );

        // Ensure that a complete page was read
        if (file.gcount() != static_cast<std::streamsize>(Page::PAGE_SIZE)) {
            throw std::runtime_error("Failed to read complete page");
        }

        page.setData(buffer);
    }

    // Destructor
    ~StorageManager() {
        if (file.is_open()) {
            file.close();
        }
    }
};
