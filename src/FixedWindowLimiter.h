#pragma once

#include <unordered_map>
#include <mutex>
#include <chrono>
#include <string>

class FixedWindowLimiter {

private:
    int maxRequests;
    int windowSize;

    std::unordered_map<std::string, int> requestCount;
    std::unordered_map<std::string, long> windowStart;

    std::mutex mtx;

public:

    FixedWindowLimiter(int maxReq, int window)
        : maxRequests(maxReq), windowSize(window) {}

    bool allowRequest(const std::string& clientId) {

        long now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        std::lock_guard<std::mutex> lock(mtx);

        if (windowStart[clientId] == 0)
            windowStart[clientId] = now;

        if (now - windowStart[clientId] >= windowSize) {

            windowStart[clientId] = now;
            requestCount[clientId] = 0;
        }

        if (requestCount[clientId] < maxRequests) {

            requestCount[clientId]++;
            return true;
        }

        return false;
    }
};