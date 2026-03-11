#pragma once

#include <unordered_map>
#include <mutex>
#include <chrono>
#include <string>
#include <algorithm>

class TokenBucketLimiter {

private:

    int capacity;
    double refillRate;

    struct Bucket {

        double tokens;
        long lastRefill;
    };

    std::unordered_map<std::string, Bucket> buckets;

    std::mutex mtx;

public:

    TokenBucketLimiter(int cap, double rate)
        : capacity(cap), refillRate(rate) {}

    bool allowRequest(const std::string& clientId) {

        long now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        std::lock_guard<std::mutex> lock(mtx);

        auto &bucket = buckets[clientId];

        if (bucket.lastRefill == 0) {

            bucket.tokens = capacity;
            bucket.lastRefill = now;
        }

        double elapsed = now - bucket.lastRefill;

        bucket.tokens = std::min((double)capacity,
                                 bucket.tokens + elapsed * refillRate);

        bucket.lastRefill = now;

        if (bucket.tokens >= 1) {

            bucket.tokens -= 1;
            return true;
        }

        return false;
    }
};