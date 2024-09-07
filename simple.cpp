#include "tracy/Tracy.hpp"
#include <unordered_map>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>  // For std::hex

int main() {
    // Make sure Tracy is enabled
    #ifdef TRACY_ENABLE

    // Initialize a hash map
    std::unordered_map<int, int> myMap;

    // Example loop for inserting into the hash map
    for (int i = 0; i < 100; i++) {
        // Insert a key-value pair into the hash map
        myMap[i] = i * 10;

        // Get the memory address of the inserted element
        auto addr = reinterpret_cast<int64_t>(&myMap[i]);

        // Print the memory address in hexadecimal format
        std::cout << "Inserting key: " << i << ", at address: 0x" << std::hex << addr << std::dec << "( "<< addr << ")" << std::endl;

        // Stream the key to Tracy
        TracyPlot("Key", static_cast<int64_t>(i));

        // Stream the memory address to Tracy
        TracyPlot("Memory Address", addr);

        // Simulate some workload (e.g., sleep for a short time)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    #endif // TRACY_ENABLE

    return 0;
}