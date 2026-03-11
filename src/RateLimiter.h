#pragma once
#include <string>

class RateLimiter {
public:
    virtual bool allowRequest(const std::string& client_id) = 0;
};