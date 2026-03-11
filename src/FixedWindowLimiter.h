#pragma once
#include "RateLimiter.h"
#include <unordered_map>
#include <mutex>
#include <chrono>

struct WindowData {
    long window_start = 0;
    int count = 0;
};

class FixedWindowLimiter : public RateLimiter {

private:
    int max_requests;
    int window_seconds;

    std::unordered_map<std::string, WindowData> clients;
    std::mutex mtx;

public:

    FixedWindowLimiter(int maxReq, int windowSec) {
        max_requests = maxReq;
        window_seconds = windowSec;
    }

    bool allowRequest(const std::string& client_id) override {

        std::lock_guard<std::mutex> lock(mtx);

        long now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        auto &data = clients[client_id];

        if (now - data.window_start >= window_seconds) {
            data.window_start = now;
            data.count = 0;
        }

        if (data.count < max_requests) {
            data.count++;
            return true;
        }

        return false;
    }
};