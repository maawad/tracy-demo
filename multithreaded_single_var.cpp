#include "tracy/Tracy.hpp"
#include <unordered_map>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <vector>
#include <cstdlib>  

// Mutex to protect the shared map from concurrent access
std::mutex mapMutex;

void insertElement(std::unordered_map<int, int>& myMap, int key, int value) {
    ZoneScopedN("Insert Element");

    {
        std::lock_guard<std::mutex> lock(mapMutex); // Protect map insertion

        myMap[key] = value;

        // Get memory address of the inserted element
        int64_t addr = reinterpret_cast<int64_t>(&myMap[key]);

        // Print the memory address in hexadecimal and decimal formats
        std::cout << "Inserting key: " << key << ", at address: 0x" 
                  << std::hex << addr << std::dec << " (" << addr << ")" << std::endl;

        // Plot the memory address in Tracy (Y-axis = memory address)
        TracyPlot("Memory Address", addr);

        // Optionally stream memory range as message
        std::stringstream ss;
        ss << "Memory Range: 0x" << std::hex << addr << " - 0x" << (addr + sizeof(int));
        TracyMessage(ss.str().c_str(), ss.str().size());
    }
}

void simulateWorkload(int delay_ms) {
    ZoneScopedN("Simulate Workload");
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
}

void processMapInserts(std::unordered_map<int, int>& myMap, int start, int count) {
    ZoneScopedN("Process Map Inserts");

    for (int i = start; i < start + count; i++) {
        insertElement(myMap, i, i * 10);
        simulateWorkload(50);
    }
}

// Thread function that performs insertions and names the thread
void threadFunction(std::unordered_map<int, int>& myMap, int threadId, int start, int count) {
    // Set thread name for Tracy
    std::stringstream ss;
    ss << "Worker Thread " << threadId;
    tracy::SetThreadName(ss.str().c_str());  // Name each thread uniquely

    processMapInserts(myMap, start, count);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <numThreads> <insertsPerThread>" << std::endl;
        return 1;
    }

    // Parse the number of threads and number of inserts per thread from the command line
    int numThreads = std::atoi(argv[1]);
    int insertsPerThread = std::atoi(argv[2]);

    // Validate the input values
    if (numThreads <= 0 || insertsPerThread <= 0) {
        std::cerr << "Error: numThreads and insertsPerThread must be positive integers." << std::endl;
        return 1;
    }

    std::unordered_map<int, int> myMap;
    std::vector<std::thread> threads;

    // Start threads, assigning a unique range and name to each
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(threadFunction, std::ref(myMap), i, i * insertsPerThread, insertsPerThread));
    }

    // Join all threads
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}