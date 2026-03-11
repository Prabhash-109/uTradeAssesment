#pragma once

#include "RateLimiter.h"
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <algorithm>

struct Bucket {
    double tokens = 0;
    long last_refill = 0;
};

class TokenBucketLimiter : public RateLimiter {

private:

    int capacity;
    double refill_rate;

    std::unordered_map<std::string, Bucket> buckets;
    std::mutex mtx;

public:

    TokenBucketLimiter(int cap, double rate) {
        capacity = cap;
        refill_rate = rate;
    }

    bool allowRequest(const std::string& client_id) override {

        std::lock_guard<std::mutex> lock(mtx);

        long now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        auto &bucket = buckets[client_id];

        double elapsed = now - bucket.last_refill;

        bucket.tokens = std::min((double)capacity,
                                 bucket.tokens + elapsed * refill_rate);

        bucket.last_refill = now;

        if (bucket.tokens >= 1) {
            bucket.tokens -= 1;
            return true;
        }

        return false;
    }
};