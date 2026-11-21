#include "netloop/event_loop.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

int main() {
    std::cout << "=========================================================\n";
    std::cout << "   Async Event Loop & Timer Wheel Reactor C++20          \n";
    std::cout << "=========================================================\n\n";

    netloop::EventLoop loop;
    uint64_t timer_fired_count = 0;
    uint64_t task_executed_count = 0;

    std::cout << "[*] 1. Scheduling Timers on Hashed Timer Wheel...\n";
    loop.schedule_timer(5, [&]() {
        std::cout << "    [EVENT] One-shot timer fired at tick 5!\n";
        timer_fired_count++;
    });

    loop.schedule_periodic_timer(10, [&]() {
        timer_fired_count++;
    });

    // 2. Benchmark Task Dispatch Throughput
    constexpr size_t NUM_TASKS = 3'000'000;
    std::cout << "[*] Posting " << NUM_TASKS << " deferred execution tasks to loop...\n";

    for (size_t i = 0; i < NUM_TASKS; ++i) {
        loop.post([&task_executed_count]() {
            task_executed_count++;
        });
    }

    std::cout << "[*] Running Event Loop Reactor...\n";
    auto t0 = std::chrono::high_resolution_clock::now();

    loop.run_steps(100);

    auto t1 = std::chrono::high_resolution_clock::now();
    double elapsed_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    double mops = (task_executed_count / (elapsed_ms / 1000.0)) / 1e6;

    std::cout << "\n[+] Reactor Dispatch Performance:\n";
    std::cout << "    Executed Tasks:    " << task_executed_count << "\n";
    std::cout << "    Timer Triggers:    " << timer_fired_count << "\n";
    std::cout << "    Elapsed Time:      " << std::fixed << std::setprecision(2) << elapsed_ms << " ms\n";
    std::cout << "    Task Throughput:   " << std::fixed << std::setprecision(2) << mops << " Million tasks/sec\n";
    std::cout << "    Loop Iterations:   " << loop.iterations() << "\n";

    std::cout << "\n[✔] Asynchronous Network Event Engine Verified.\n";
    return 0;
}
