
class Page {
public:
    static constexpr size_t PAGE_SIZE = 4096;

private:
    array<char, PAGE_SIZE> data{};

public:
    // Write a string into the page.
    void writeString(const string& value) {

        uint32_t length = static_cast<uint32_t>(value.size());

        if (sizeof(length) + length > PAGE_SIZE) {
            throw runtime_error("Page capacity exceeded");
        }

        // Store string length first.
        memcpy(data.data(), &length, sizeof(length));

        // Store actual string.
        memcpy(
            data.data() + sizeof(length),
            value.data(),
            length
        );
    }

    // Read a string from the page.
    string readString() const {

        uint32_t length = 0;

        memcpy(
            &length,
            data.data(),
            sizeof(length)
        );

        if (sizeof(length) + length > PAGE_SIZE) {
            throw runtime_error("Invalid page data");
        }

        return string(
            data.data() + sizeof(length),
            length
        );
    }

    // Get raw page data.
    const array<char, PAGE_SIZE>& getData() const {
        return data;
    }

    // Load raw page data.
    void setData(const array<char, PAGE_SIZE>& input) {
        data = input;
    }
};
