#pragma once

#include "timer_wheel.hpp"
#include <deque>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>
#include <iostream>

namespace netloop {

using Task = std::function<void()>;

class EventLoop {
public:
    EventLoop() : running_(false), loop_iterations_(0) {}

    void post(Task task) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            task_queue_.push_back(std::move(task));
        }
    }

    TimerId schedule_timer(uint64_t ticks_from_now, TimerCallback cb) {
        return timer_wheel_.schedule_after(ticks_from_now, std::move(cb));
    }

    TimerId schedule_periodic_timer(uint64_t interval_ticks, TimerCallback cb) {
        return timer_wheel_.schedule_periodic(interval_ticks, std::move(cb));
    }

    // Run event loop until stop requested or for specified step count
    void run_steps(size_t max_steps) {
        running_.store(true);
        for (size_t step = 0; step < max_steps && running_.load(); ++step) {
            step_once();
        }
        running_.store(false);
    }

    void step_once() {
        // 1. Process pending tasks
        std::deque<Task> current_tasks;
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            current_tasks.swap(task_queue_);
        }

        for (auto& task : current_tasks) {
            if (task) task();
        }

        // 2. Advance timer wheel
        timer_wheel_.tick();
        ++loop_iterations_;
    }

    void stop() noexcept {
        running_.store(false);
    }

    uint64_t iterations() const noexcept {
        return loop_iterations_;
    }

private:
    std::mutex queue_mutex_;
    std::deque<Task> task_queue_;
    HashedTimerWheel<1024> timer_wheel_;
    std::atomic<bool> running_;
    uint64_t loop_iterations_;
};

} // namespace netloop
