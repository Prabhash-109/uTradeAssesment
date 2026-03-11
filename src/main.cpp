#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

#include "FixedWindowLimiter.h"
#include "TokenBucketLimiter.h"

FixedWindowLimiter fixedLimiter(10, 60);

std::mutex printMutex;

int total_requests = 0;
int allowed = 0;
int rejected = 0;

void simulateClient(std::string client_id, int requests) {

    for (int i = 0; i < requests; i++) {

        bool result = fixedLimiter.allowRequest(client_id);

        auto now = std::chrono::system_clock::now();

        long timestamp =
        std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();

        {
            std::lock_guard<std::mutex> lock(printMutex);

            total_requests++;

            if (result)
                allowed++;
            else
                rejected++;

            std::cout << timestamp
                      << " | client=" << client_id
                      << " | algorithm=FixedWindow"
                      << " | result=" << (result ? "ALLOWED" : "RATE_LIMITED")
                      << std::endl;
        }
    }
}

int main() {

    std::vector<std::thread> threads;

    for (int i = 0; i < 5; i++) {
        threads.push_back(
            std::thread(simulateClient, "client_" + std::to_string(i), 25)
        );
    }

    for (auto &t : threads)
        t.join();

    std::cout << "\nSummary Stats\n";

    std::cout << "Total Requests: " << total_requests << std::endl;
    std::cout << "Allowed: " << allowed << std::endl;
    std::cout << "Rejected: " << rejected << std::endl;

    return 0;
}