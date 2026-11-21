#include "netloop/timer_wheel.hpp"
#include "netloop/event_loop.hpp"
#include <cassert>
#include <iostream>

void test_timer_wheel_timing() {
    netloop::HashedTimerWheel<64> wheel;
    int counter = 0;

    wheel.schedule_after(3, [&]() {
        counter += 10;
    });

    wheel.tick(); // tick 1
    assert(counter == 0);
    wheel.tick(); // tick 2
    assert(counter == 0);
    wheel.tick(); // tick 3 -> fire!
    assert(counter == 10);

    std::cout << "[PASS] Hashed Timer Wheel one-shot execution at exact tick\n";
}

void test_event_loop_task_drain() {
    netloop::EventLoop loop;
    int val = 0;

    loop.post([&]() { val += 5; });
    loop.post([&]() { val *= 2; });

    loop.step_once();
    assert(val == 10); // (0 + 5) * 2 = 10
    std::cout << "[PASS] EventLoop FIFO task draining\n";
}

int main() {
    std::cout << "Running Network Event Loop Unit Tests...\n";
    test_timer_wheel_timing();
    test_event_loop_task_drain();
    std::cout << "\nAll Unit Tests Passed Successfully!\n";
    return 0;
}
