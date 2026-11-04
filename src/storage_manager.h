
#pragma once

#include "page.h"

#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <array>

class StorageManager {
private:
    std::string filename;
    std::fstream file;

public:
    StorageManager(const std::string& fileName)
        : filename(fileName) {

        file.open(
            filename,
            std::ios::in |
            std::ios::out |
            std::ios::binary
        );

        // Create file if it doesn't exist
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

    // Count existing pages
    uint32_t getPageCount() {
        file.clear();
        file.seekg(0, std::ios::end);

        std::streamoff fileSize = file.tellg();

        if (fileSize < 0) {
            throw std::runtime_error("Unable to determine file size");
        }

        if (fileSize % Page::PAGE_SIZE != 0) {
            throw std::runtime_error("Database file is corrupted");
        }

        return static_cast<uint32_t>(
            fileSize / Page::PAGE_SIZE
        );
    }

    // Allocate a new page
    uint32_t allocatePage() {
        uint32_t pageId = getPageCount();

        // Create an empty page
        Page emptyPage;

        // Write the empty page to disk
        writePage(pageId, emptyPage);

        return pageId;
    }

    // Write a page to disk
    void writePage(uint32_t pageId, const Page& page) {
        std::streamoff offset =
            static_cast<std::streamoff>(pageId) * Page::PAGE_SIZE;

        file.clear();
        file.seekp(offset, std::ios::beg);

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
        if (pageId >= getPageCount()) {
            throw std::runtime_error("Page does not exist");
        }

        std::streamoff offset =
            static_cast<std::streamoff>(pageId) * Page::PAGE_SIZE;

        file.clear();
        file.seekg(offset, std::ios::beg);

        std::array<char, Page::PAGE_SIZE> buffer{};

        file.read(
            buffer.data(),
            static_cast<std::streamsize>(Page::PAGE_SIZE)
        );

        if (file.gcount() != static_cast<std::streamsize>(Page::PAGE_SIZE)) {
            throw std::runtime_error("Failed to read complete page");
        }

        page.setData(buffer);
    }

    ~StorageManager() {
        if (file.is_open()) {
            file.close();
        }
    }
};
