#include <iostream>
#include <vector>
#include <atomic>
#include <cassert>
#include "../Threads.hpp"


uint16_t test(int num) {
    std::atomic<uint16_t> counter(0);
    std::atomic<uint16_t>* counter_ = &counter;
    Threads pool(num);

    std::vector<std::future<void>> futures;

    for (int i=0; i<num; i++) {
        futures.emplace_back(pool.invoke([counter_] {
            std::cout << "in the thread...\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            counter_->fetch_add(1);
            std::cout << "done\n";
        }));
    }

    for (auto& future : futures)
        future.get();

    std::cout << "counter = " << counter.load() << "\n";
    return counter.load();
}



int main() {
    assert(test(1) == 1);
    assert(test(5) == 5);
}
