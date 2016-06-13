#include "Threads.hpp"

Threads::Threads(size_t num) {
    for (size_t t = 0; t < num; t++) {
        threads.emplace_back([this] {
            while (!this->terminated) {
                std::function<void()> job;

                {
                    std::unique_lock<std::mutex> lock(this->mutex);
                    this->condition.wait(lock, [this] {
                        return this->terminated || !this->jobs.empty();
                    });

                    if(this->terminated && this->jobs.empty())
                        return;

                    job = std::move(this->jobs.front());
                    this->jobs.pop();
                }

                job();
            }
        });
    }
}

Threads::~Threads() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        terminated = true;
    }
    condition.notify_all();
    for (auto& thread : threads)
        thread.join();
}

std::future<void> Threads::invoke(std::function<void()> job) {
    std::shared_ptr<std::packaged_task<void()>> packaged_job = std::make_shared<std::packaged_task<void()>>(job);
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        jobs.emplace([packaged_job] () { (*packaged_job)(); } );
    }
    condition.notify_one();

    return packaged_job->get_future();
}


