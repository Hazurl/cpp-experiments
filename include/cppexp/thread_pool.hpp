#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <cassert>

#include <cppexp/job.hpp>
#include <cppexp/stop_token.hpp>

namespace cppexp 
{

template<job_c job_type>
class thread_pool 
{
public:

    explicit thread_pool(std::size_t thread_count = std::thread::hardware_concurrency())
    {
        assert(thread_count > 0 && "A thread pool cannot start without any threads");
        threads.reserve(thread_count);
        for(std::size_t i = 0; i < thread_count; ++i) 
        {
            threads.emplace_back(
                thread_pool::thread_process, 
                std::ref(jobs), 
                std::ref(jobs_mutex), 
                std::ref(jobs_empty_condition), 
                std::ref(token));
        }
    }


    // Wait for all jobs to stop then for all threads to stop
    auto join()
        -> void
    {
        token.request_stop();
        for(auto& thread : threads)
        {
            thread.join();
        }
    }


    template<typename job_type_convertible>
        requires(std::is_convertible_v<job_type_convertible&&, job_type>)
    auto schedule(job_type_convertible&& job)
        -> void
    {
        auto lock = std::scoped_lock(jobs_mutex);
        jobs.emplace_back(std::forward<job_type_convertible>(job));
        jobs_empty_condition.notify_one();
    }

private:

    static auto thread_process(std::vector<job_type>& jobs, std::mutex& jobs_mutex, 
        std::condition_variable& jobs_empty_condition, stop_token& token)
        -> void
    {
        while(true)
        {
            auto lock = std::unique_lock(jobs_mutex);
            jobs_empty_condition.wait(lock, 
                [&jobs, &token] () 
                    -> bool
                {
                    return !jobs.empty() || token.stop_requested();
                });

            if (jobs.empty())
            {
                // then stop was requested
                break;
            }

            auto current_job = std::move(jobs.back());
            jobs.pop_back();
            lock.unlock();

            execute_job_sync(current_job);
        }
    }

    std::vector<std::thread> threads;
    std::vector<job_type> jobs;
    std::mutex jobs_mutex;
    stop_token token;
    std::condition_variable jobs_empty_condition;

};

}