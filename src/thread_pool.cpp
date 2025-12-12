// thread_pool.cpp - Scalable Thread Management Library
// Compile: g++ -std=c++17 -pthread src/thread_pool.cpp -O2 -o thread_pool
// Run: ./thread_pool

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <fstream>
#include <mutex>
#include <deque>
#include <thread>
#include <vector>
#include <chrono>
#include <sstream>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>>;

    void increaseSize(size_t newSize);
    void shutdown(bool waitForTasks = true);
    size_t getActiveWorkerCount();
    size_t getQueuedTaskCount();

private:
    void workerLoop();
    std::vector<std::thread> workers;
    std::deque<std::function<void()>> tasks;
    std::mutex tasksMutex;
    std::condition_variable cv;
    std::atomic<bool> stopping{false};
    std::atomic<size_t> activeWorkers{0};
};

ThreadPool::ThreadPool(size_t numThreads) {
    if (numThreads == 0) numThreads = 1;
    increaseSize(numThreads);
}

ThreadPool::~ThreadPool() {
    shutdown(true);
}

// Task submission using packaged_task and futures for result retrieval
template<typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>> {
    using RetType = typename std::invoke_result_t<F, Args...>;
    auto taskPack = std::make_shared<std::packaged_task<RetType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<RetType> res = taskPack->get_future();
    {
        std::lock_guard<std::mutex> lock(tasksMutex);
        tasks.emplace_back([taskPack]() { (*taskPack)(); });
    }
    cv.notify_one();
    return res;
}

// Worker thread loop with mutex and condition_variable for synchronization
void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(tasksMutex);
            cv.wait(lock, [this]() { return stopping.load() || !tasks.empty(); });
            if (stopping && tasks.empty()) return;
            if (!tasks.empty()) {
                task = std::move(tasks.front());
                tasks.pop_front();
                activeWorkers.fetch_add(1, std::memory_order_relaxed);
            } else {
                continue;
            }
        }
        try {
            task();
        } catch (const std::exception &e) {
            std::cerr << "Task exception: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Task unknown exception" << std::endl;
        }
        activeWorkers.fetch_sub(1, std::memory_order_relaxed);
    }
}

void ThreadPool::increaseSize(size_t newSize) {
    if (newSize <= workers.size()) return;
    size_t add = newSize - workers.size();
    for (size_t i = 0; i < add; ++i) {
        workers.emplace_back([this]() { this->workerLoop(); });
    }
}

// Graceful shutdown: waits for tasks to complete before terminating threads
void ThreadPool::shutdown(bool waitForTasks) {
    {
        std::unique_lock<std::mutex> lock(tasksMutex);
        if (!waitForTasks) tasks.clear();
        stopping.store(true);
    }
    cv.notify_all();
    for (auto &t : workers) if (t.joinable()) t.join();
    workers.clear();
}

size_t ThreadPool::getActiveWorkerCount() {
    return activeWorkers.load(std::memory_order_relaxed);
}

size_t ThreadPool::getQueuedTaskCount() {
    std::lock_guard<std::mutex> lock(tasksMutex);
    return tasks.size();
}

// ---------------- MAIN TEST WITH CSV LOGGING ---------------- //
int main() {
    size_t threads = 8;
    size_t tasksCount = 20000;
    size_t workIterations = 50000;

    std::cout << "Running ThreadPool: " << threads << " threads, "
              << tasksCount << " tasks\n";

    ThreadPool pool(threads);

    std::ofstream log("tests/stats.csv");
    log << "submitted,queued,active\n";

    auto tstart = std::chrono::high_resolution_clock::now();
    std::vector<std::future<uint64_t>> results;
    results.reserve(tasksCount);

    for (size_t i = 0; i < tasksCount; i++) {
        results.emplace_back(pool.submit([i, workIterations](){
            uint64_t acc = 0;
            for (size_t k=0; k<workIterations; k++)
                acc += (k*i) & 0xFF;
            return acc;
        }));

        if (i % 500 == 0) {
            size_t q = pool.getQueuedTaskCount();
            size_t a = pool.getActiveWorkerCount();
            log << i << "," << q << "," << a << "\n";
            std::cout << "Submitted: " << i << " | Queue: " << q << " | Active: " << a << "\n";
        }
    }

    uint64_t sum = 0;
    for (auto &f : results) sum += f.get();

    auto tend = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(tend - tstart).count();

    log.close();

    std::cout << "Completed. Sum = " << sum << "\n";
    std::cout << "Total Time = " << elapsed << " seconds\n";

    pool.shutdown(true);
    return 0;
}
