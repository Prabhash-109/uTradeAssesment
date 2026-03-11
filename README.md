# Thread-Safe Rate Limiter (C++)

## Overview

This project implements a **thread-safe rate limiter** that enforces request limits per client.
It simulates an API gateway component responsible for protecting services from excessive requests.

The limiter supports multiple algorithms and processes **concurrent requests from multiple clients** using C++ multithreading.

Each request contains a `client_id`, and the system decides whether the request should be:

* **ALLOWED**
* **RATE_LIMITED**

The project also prints request logs and summary statistics at the end of execution.

---

# Features

* Thread-safe implementation using `std::mutex`
* Supports **multiple rate limiting algorithms**
* Handles **concurrent client requests**
* Per-request logging
* Summary statistics at the end
* Modular and extensible design

---

# Supported Algorithms

## 1. Fixed Window Rate Limiter

### Concept

Time is divided into fixed windows (e.g., 60 seconds).
Each client can only make **N requests per window**.

Example:

```
Max Requests = 10
Window Size = 60 seconds
```

If a client sends more than 10 requests in that 60-second window, additional requests are rejected.

### Data Structure

```
unordered_map<string, WindowData>
```

Where:

```
client_id → {window_start_time, request_count}
```

### Algorithm Steps

1. Get current timestamp
2. Check if client window expired
3. Reset counter if new window started
4. Allow request if `count < max_requests`
5. Otherwise reject

### Pros

* Simple to implement
* Low memory usage
* Fast lookup O(1)

### Cons

* Burst traffic at window boundaries

Example:

```
10 requests at 59s
10 requests at 60s
→ 20 requests in 1 second
```

---

## 2. Token Bucket Rate Limiter

### Concept

Each client has a bucket containing tokens.

* Each request consumes **1 token**
* Tokens refill at a fixed rate
* Requests are allowed only if tokens are available

Example:

```
Bucket Capacity = 10 tokens
Refill Rate = 1 token per second
```

### Data Structure

```
unordered_map<string, Bucket>
```

Where:

```
client_id → {tokens, last_refill_time}
```

### Algorithm Steps

1. Calculate elapsed time since last refill
2. Add new tokens based on refill rate
3. Cap tokens at bucket capacity
4. If tokens ≥ 1 → allow request
5. Otherwise reject request

### Pros

* Smooth request flow
* Handles burst traffic gracefully
* Commonly used in real API gateways

### Cons

* Slightly more complex than fixed window

---

# Thread Safety

Since multiple threads simulate concurrent clients, shared data structures must be protected.

This implementation uses:

```
std::mutex
std::lock_guard
```

These ensure that only one thread modifies shared data at a time.

Protected resources include:

* client request counters
* token buckets
* logging counters

Without locking, race conditions could corrupt state.

---

# System Architecture

```
Client Threads
      │
      ▼
Rate Limiter Interface
      │
 ┌───────────────┐
 │               │
 ▼               ▼
FixedWindow   TokenBucket
Limiter        Limiter
```

Design uses **polymorphism**, allowing easy addition of new algorithms.

---

# Concurrent Request Simulation

The system simulates:

```
5 clients
25 requests each
Total = 125 requests
```

Each client runs in its own thread.

Example client IDs:

```
client_0
client_1
client_2
client_3
client_4
```

---

# Example Output

```
1710152121 | client=client_1 | algorithm=FixedWindow | ALLOWED
1710152121 | client=client_1 | algorithm=FixedWindow | ALLOWED
1710152121 | client=client_1 | algorithm=FixedWindow | RATE_LIMITED
```

Summary statistics:

```
Summary Stats

Total Requests: 125
Allowed: 50
Rejected: 75
```

---

# Data Structures Used

| Data Structure   | Purpose                      |
| ---------------- | ---------------------------- |
| `unordered_map`  | O(1) lookup per client       |
| `mutex`          | thread safety                |
| `vector<thread>` | concurrent client simulation |

---

# Build Instructions

### Compile

```
g++ -std=c++17 -pthread src/main.cpp -o rate_limiter
```

### Run

```
./rate_limiter
```

On Windows:

```
rate_limiter.exe
```

---

# Configuration

Currently the following parameters are configurable in code:

```
max_requests
window_seconds
bucket_capacity
refill_rate
```

These can easily be extended to load from a **JSON configuration file**.

---

# Edge Cases Considered

* New client with no previous requests
* Window expiration
* Token refill calculation
* Concurrent access to shared maps
* Burst request traffic

---

# Trade-offs

### Simplicity vs Feature Set

The implementation prioritizes **clarity and correctness** over advanced optimizations.

Possible improvements:

* Lock-free data structures
* per-client mutex instead of global mutex
* sharded maps for high scalability

---

# Possible Improvements

Future enhancements could include:

* HTTP server interface (`POST /request`)
* runtime configuration updates
* per-client custom limits
* distributed rate limiting using Redis
* sliding window algorithm
* metrics endpoint `/stats`

---

# Why This Design

Key design goals:

* modular architecture
* easy algorithm extension
* thread safety
* clear separation of responsibilities

This mirrors how rate limiting components are designed in **real API gateways and trading systems**.

---

