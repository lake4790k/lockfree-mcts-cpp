#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class Threads {
public:

    Threads(size_t);
    ~Threads();
    std::future<void> invoke(std::function<void()>);

private:

    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;

    std::mutex mutex;
    std::condition_variable condition;
    bool terminated{false};
};

