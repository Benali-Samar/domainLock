# 🧵 DomainLock – Custom Mutex (Ticket Lock) in C++

> A minimal C++ implementation of a fair mutex using a **ticket lock** mechanism – for educational purposes only.

## 📌 Overview

This project demonstrates how to implement a basic mutual exclusion (mutex) mechanism in C++ from scratch. (version 0.1--SpinLock)
Inspired by the *ticket lock* algorithm, `DomainLock` ensures **FIFO ordering**: the first thread that tries to acquire the lock will be the first to hold it.

The lock uses two atomic counters:

- `ticket`: tracks the next ticket to assign.
- `serving`: tracks the currently served ticket.

Each thread receives a unique ticket and waits until its number matches the `serving` counter.

---

## 🔧 Features

- ✅ **Fair lock acquisition** (FIFO)
- ✅ `try_lock()` and `lock()` support
- ✅ Uses C++20 `<atomic>` for thread-safe primitives
- ✅ Logs thread activity (acquire/release)
- ✅ Lightweight and dependency-free
- ⚠️ Not suitable for production use – built for educational purposes only

---

## 🚀 Getting Started

### ✅ Prerequisites

- C++20 compatible compiler (`g++`, `clang++`)


### 🧠 Example Output
```bash

[Thread 140184607512320] acquired lock (ticket 0)
[Thread 140184599119616] waiting for turn (ticket 1, serving 0)
[Thread 140184607512320] released the variable
[Thread 140184599119616] acquired lock (ticket 1)
...
Counter: 4

```

### 📚 Resources

- [SpinLock](https://wiki.osdev.org/Spinlock)
- [atomic](https://en.cppreference.com/w/cpp/atomic/atomic.html)
- [memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order.html)
- C++ Concurrency in Action by Anthony Williams




### 🛠️ TODO :

- [X] SpinLock simple atomic lock
- [X] Fair lock and ticket logic  
- [ ] Add `try_lock_for(timeout)` method with time-based locking  
- [ ] Implement `DomainLockGuard` (RAII wrapper like `std::lock_guard`)
- [ ] Optional logging toggle to enable/disable thread logs
- [ ] Benchmark and compare with `std::mutex` under various loads
- [ ] Add reentrant locking (like `recursive_mutex`)
- [ ] Experiment with condition variable support (`wait()` / `notify()` interface)
- [ ] Stress test with 100+ threads and measure fairness/starvation

### ✨ Fancy TODOs ✨
- [ ] Add visuals (e.g., ticket lock flow diagram)
- [ ] Unit test :/
- [ ] Using logging library

