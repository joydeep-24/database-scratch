
#pragma once

#include <array>
#include <cstring>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <cstddef>

class Page {
public:
    static constexpr size_t PAGE_SIZE = 4096;

private:
    std::array<char, PAGE_SIZE> data{};

public:
    // Write a string into the page
    void writeString(const std::string& value) {
        uint32_t length = static_cast<uint32_t>(value.size());

        // Check whether the string fits inside the page
        if (sizeof(length) + length > PAGE_SIZE) {
            throw std::runtime_error("String is too large for the page");
        }

        // Store the string length first
        std::memcpy(data.data(), &length, sizeof(length));

        // Store the actual string after the length
        std::memcpy(
            data.data() + sizeof(length),
            value.data(),
            length
        );
    }

    // Read a string from the page
    std::string readString() const {
        uint32_t length = 0;

        // Read the stored string length
        std::memcpy(&length, data.data(), sizeof(length));

        // Validate the length
        if (sizeof(length) + length > PAGE_SIZE) {
            throw std::runtime_error("Invalid string length in page");
        }

        // Read and return the string
        return std::string(
            data.data() + sizeof(length),
            length
        );
    }

    // Get the raw page data
    const std::array<char, PAGE_SIZE>& getData() const {
        return data;
    }

    // Replace the page data
    void setData(const std::array<char, PAGE_SIZE>& newData) {
        data = newData;
    }
};
