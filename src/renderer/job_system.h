#ifndef __JOB_SYSTEM_H__
#define __JOB_SYSTEM_H__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

// ============================================================
// Job system with start-barrier for fair work distribution.
//
// Before work begins, ALL threads (main + workers) rendezvous
// at a barrier.  This guarantees every thread starts claiming
// jobs at the same instant, eliminating the "main thread eats
// half the work before workers wake" problem.
// ============================================================

class JobSystem
{
public:
    explicit JobSystem(int num_threads = 0);
    ~JobSystem();

    JobSystem(const JobSystem&) = delete;
    JobSystem& operator=(const JobSystem&) = delete;

    int thread_count() const { return (int)threads_.size(); }

    // Invoke fn(i) for i in [0, count) across all threads (incl. caller).
    void parallel_for(int count, const std::function<void(int)>& fn);

private:
    void worker_loop(int worker_id);

    std::vector<std::thread> threads_;

    // --- signalling ---
    std::mutex              mutex_;
    std::condition_variable cv_;
    bool                    running_    = true;

    int                     batch_id_   = 0;   // incremented each call

    // --- work description ---
    std::atomic<int>        next_job_{0};
    int                     job_count_{0};
    std::function<void(int)> job_fn_;

    // --- start barrier ---
    // All participants (workers + main thread) rendezvous here before
    // claiming jobs.  Protected by a separate mutex to avoid deadlock
    // with the signalling cv.
    std::mutex              barrier_mutex_;
    std::condition_variable barrier_cv_;
    int                     barrier_count_   = 0;
    int                     barrier_target_  = 0;

    // --- completion ---
    std::atomic<int>        jobs_done_{0};
};

// --- Implementation ---

inline JobSystem::JobSystem(int num_threads)
{
    if (num_threads <= 0)
        num_threads = (int)std::thread::hardware_concurrency();
    if (num_threads <= 0) num_threads = 4;

    for (int i = 0; i < num_threads; ++i)
        threads_.emplace_back(&JobSystem::worker_loop, this, i);
}

inline JobSystem::~JobSystem()
{
    {
        std::lock_guard<std::mutex> lk(mutex_);
        running_ = false;
        batch_id_++;
    }
    cv_.notify_all();
    for (auto& t : threads_)
        if (t.joinable()) t.join();
}

inline void JobSystem::worker_loop(int worker_id)
{
    int last_batch = 0;
    (void)worker_id;

    while (true)
    {
        // --- wait for new batch or shutdown ---
        {
            std::unique_lock<std::mutex> lk(mutex_);
            cv_.wait(lk, [this, last_batch] {
                return batch_id_ != last_batch || !running_;
            });
            if (!running_) return;
            last_batch = batch_id_;
        }

        // --- start barrier: wait for all threads (incl. main) ---
        {
            std::unique_lock<std::mutex> lk(barrier_mutex_);
            barrier_count_++;
            if (barrier_count_ == barrier_target_) {
                barrier_cv_.notify_all();
            } else {
                barrier_cv_.wait(lk, [this] {
                    return barrier_count_ == barrier_target_;
                });
            }
        }

        // --- claim and execute jobs lock-free ---
        while (true)
        {
            int j = next_job_.fetch_add(1, std::memory_order_relaxed);
            if (j >= job_count_) break;
            job_fn_(j);
        }

        // --- signal completion ---
        jobs_done_.fetch_add(1, std::memory_order_release);
    }
}

inline void JobSystem::parallel_for(int count, const std::function<void(int)>& fn)
{
    if (count <= 0) return;

    int num_participants = (int)threads_.size() + 1;  // workers + main

    // --- set up work ---
    job_count_ = count;
    job_fn_    = fn;
    next_job_.store(0, std::memory_order_relaxed);
    jobs_done_.store(0, std::memory_order_relaxed);

    // --- initialise start barrier ---
    {
        std::lock_guard<std::mutex> lk(barrier_mutex_);
        barrier_count_  = 0;
        barrier_target_ = num_participants;
    }

    // --- wake workers ---
    {
        std::lock_guard<std::mutex> lk(mutex_);
        batch_id_++;
    }
    cv_.notify_all();

    // --- main thread also enters the start barrier ---
    {
        std::unique_lock<std::mutex> lk(barrier_mutex_);
        barrier_count_++;
        if (barrier_count_ == barrier_target_) {
            barrier_cv_.notify_all();
        } else {
            barrier_cv_.wait(lk, [this] {
                return barrier_count_ == barrier_target_;
            });
        }
    }

    // --- all threads now start claiming jobs simultaneously ---
    while (true)
    {
        int j = next_job_.fetch_add(1, std::memory_order_relaxed);
        if (j >= count) break;
        fn(j);
    }
    jobs_done_.fetch_add(1, std::memory_order_release);

    // --- wait for workers to finish ---
    while (jobs_done_.load(std::memory_order_acquire) < num_participants)
        std::this_thread::yield();
}

#endif // __JOB_SYSTEM_H__
