#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <chrono>

namespace netloop {

using TimerId = uint64_t;
using TimerCallback = std::function<void()>;

struct TimerEntry {
    TimerId id{0};
    uint64_t expire_tick{0};
    uint64_t interval_ticks{0}; // 0 = one-shot, >0 = periodic
    TimerCallback callback;
    bool cancelled{false};
};

template <size_t WheelSize = 1024>
class HashedTimerWheel {
public:
    HashedTimerWheel() : slots_(WheelSize), current_tick_(0), next_timer_id_(1) {}

    TimerId schedule_after(uint64_t ticks_from_now, TimerCallback cb) {
        TimerId id = next_timer_id_++;
        uint64_t expire_tick = current_tick_ + ticks_from_now;
        size_t slot_idx = expire_tick % WheelSize;

        slots_[slot_idx].push_back(TimerEntry{
            .id = id,
            .expire_tick = expire_tick,
            .interval_ticks = 0,
            .callback = std::move(cb),
            .cancelled = false
        });

        return id;
    }

    TimerId schedule_periodic(uint64_t interval_ticks, TimerCallback cb) {
        TimerId id = next_timer_id_++;
        uint64_t expire_tick = current_tick_ + interval_ticks;
        size_t slot_idx = expire_tick % WheelSize;

        slots_[slot_idx].push_back(TimerEntry{
            .id = id,
            .expire_tick = expire_tick,
            .interval_ticks = interval_ticks,
            .callback = std::move(cb),
            .cancelled = false
        });

        return id;
    }

    void tick() {
        size_t slot_idx = current_tick_ % WheelSize;
        auto& bucket = slots_[slot_idx];

        std::vector<TimerEntry> rearm_list;
        for (auto& timer : bucket) {
            if (timer.cancelled) continue;

            if (timer.expire_tick <= current_tick_) {
                if (timer.callback) {
                    timer.callback();
                }

                if (timer.interval_ticks > 0) {
                    timer.expire_tick = current_tick_ + timer.interval_ticks;
                    rearm_list.push_back(timer);
                }
            } else {
                rearm_list.push_back(timer);
            }
        }

        bucket.clear();
        for (auto& timer : rearm_list) {
            size_t new_slot = timer.expire_tick % WheelSize;
            slots_[new_slot].push_back(std::move(timer));
        }

        ++current_tick_;
    }

    uint64_t current_tick() const noexcept { return current_tick_; }

private:
    std::vector<std::vector<TimerEntry>> slots_;
    uint64_t current_tick_;
    TimerId next_timer_id_;
};

} // namespace netloop
