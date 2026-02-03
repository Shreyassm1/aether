#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <sys/syscall.h>

// Demonstrates: multiple threads (shared PID), context switching (yield),
// atomic operations (CPU LOCK prefix), and register/stack preservation

std::atomic<int> counter(0);

void worker(int id) {
    // Get kernel thread ID (gettid is per-thread; getpid is per-process)
    pid_t tid = static_cast<pid_t>(syscall(SYS_gettid));
    pid_t pid = getpid();

    for (int i = 0; i < 3; ++i) {
        // Atomic increment: CPU executes with LOCK prefix (atomic at hardware level)
        counter.fetch_add(1, std::memory_order_relaxed);

        std::cout << "Thread " << id << " | PID=" << pid 
                  << " TID=" << tid << " | iter=" << i << std::endl;

        // Voluntary context switch: kernel saves RIP + registers to memory,
        // picks next thread to execute
        std::this_thread::yield();
    }
}

int main() {
    const int num_threads = 4;
    std::vector<std::thread> threads;

    std::cout << "=== Context Switch Lab ===" << std::endl;
    std::cout << "All threads share PID; each has unique TID.\n" << std::endl;

    // Create worker threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\nFinal counter: " << counter << std::endl;
    return 0;
}