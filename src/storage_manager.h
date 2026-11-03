
#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include "page.h"
#include <fstream>
#include <string>
#include <stdexcept>

class StorageManager {
private:
    std::string filename;
    std::fstream file;

public:
    explicit StorageManager(const std::string& name)
        : filename(name) {

        // Open existing database file.
        file.open(
            filename,
            std::ios::in | std::ios::out | std::ios::binary
        );

        // Create file if it doesn't exist.
        if (!file.is_open()) {
            std::ofstream createFile(
                filename,
                std::ios::binary
            );

            if (!createFile) {
                throw std::runtime_error(
                    "Cannot create database file"
                );
            }

            createFile.close();

            file.clear();
            file.open(
                filename,
                std::ios::in | std::ios::out | std::ios::binary
            );
        }

        if (!file.is_open()) {
            throw std::runtime_error(
                "Cannot open database file"
            );
        }
    }

    // Write a page to disk.
    void writePage(size_t pageId, const Page& page) {

        file.clear();

        file.seekp(
            pageId * Page::PAGE_SIZE,
            std::ios::beg
        );

        const auto& data = page.getData();

        file.write(
            data.data(),
            Page::PAGE_SIZE
        );

        file.flush();

        if (!file) {
            throw std::runtime_error("Page write failed");
        }
    }

    // Read a page from disk.
    void readPage(size_t pageId, Page& page) {

        file.clear();

        file.seekg(
            pageId * Page::PAGE_SIZE,
            std::ios::beg
        );

        std::array<char, Page::PAGE_SIZE> buffer{};

        file.read(
            buffer.data(),
            Page::PAGE_SIZE
        );

        if (file.gcount() != Page::PAGE_SIZE) {
            throw std::runtime_error("Page read failed");
        }

        page.setData(buffer);
    }

    ~StorageManager() {
        if (file.is_open()) {
            file.close();
        }
    }
};

#endif
