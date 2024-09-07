#include "tracy/Tracy.hpp"
#include <unordered_map>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <vector>
#include <cstdlib>  // For std::atoi

// Mutex to protect the shared map from concurrent access
std::mutex mapMutex;

// Define a constexpr list of plot names for up to 16 threads
constexpr const char* plotNames[] = {
    "Memory Address - Thread 0",  "Memory Address - Thread 1",
    "Memory Address - Thread 2",  "Memory Address - Thread 3",
    "Memory Address - Thread 4",  "Memory Address - Thread 5",
    "Memory Address - Thread 6",  "Memory Address - Thread 7",
    "Memory Address - Thread 8",  "Memory Address - Thread 9",
    "Memory Address - Thread 10", "Memory Address - Thread 11",
    "Memory Address - Thread 12", "Memory Address - Thread 13",
    "Memory Address - Thread 14", "Memory Address - Thread 15"
};

void insertElement(std::unordered_map<int, int>& myMap, int key, int value, int threadId) {
    ZoneScopedN("Insert Element");

    {
        std::lock_guard<std::mutex> lock(mapMutex); // Protect map insertion

        myMap[key] = value;

        // Get memory address of the inserted element
        int64_t addr = reinterpret_cast<int64_t>(&myMap[key]);

        // Print the memory address in hexadecimal and decimal formats
        std::cout << "Inserting key: " << key << ", at address: 0x" 
                  << std::hex << addr << std::dec << " (" << addr << ")" << std::endl;

        // Use a static string array for stream names and ensure threadId is within range
        if (threadId >= 0 && threadId < 16) {
            // Plot the memory address in Tracy (Y-axis = memory address, stream unique to each thread)
            TracyPlot(plotNames[threadId], addr);
        }

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

void processMapInserts(std::unordered_map<int, int>& myMap, int start, int count, int threadId) {
    ZoneScopedN("Process Map Inserts");

    for (int i = start; i < start + count; i++) {
        insertElement(myMap, i, i * 10, threadId);
        simulateWorkload(50);
    }
}

// Thread function that performs insertions and names the thread
void threadFunction(std::unordered_map<int, int>& myMap, int threadId, int start, int count) {
    // Set thread name for Tracy
    std::stringstream ss;
    ss << "Worker Thread " << threadId;
    tracy::SetThreadName(ss.str().c_str());  // Name each thread uniquely

    processMapInserts(myMap, start, count, threadId);
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
    if (numThreads <= 0 || insertsPerThread <= 0 || numThreads > 16) {
        std::cerr << "Error: numThreads must be between 1 and 16, and insertsPerThread must be positive integers." << std::endl;
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