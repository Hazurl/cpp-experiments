#pragma once

#include <thread>
#include <vector>
#include <functional>

namespace cppexp
{

// Holds multiple threads with a common stop_source
class multi_jthread
{
public:

    multi_jthread() = default;

    multi_jthread(multi_jthread&& other);
    multi_jthread& operator=(multi_jthread&& other);

    // request_stop and join all threads (block the thread)
    ~multi_jthread();

    // non-copyable
    multi_jthread(multi_jthread const&) = delete;
    multi_jthread& operator=(multi_jthread const&) = delete;

    // Join all threads, request_stop is not called, clear the threads list afterward
    auto join()
        -> void;

    // Toggle the stop source, notifying all threads to stop
    auto request_stop() noexcept
        -> bool;
    
    // Detach all threads and clear the list afterward
    auto detach()
        -> void;

    // Add a new thread, if the thread's function accept a std::stop_token as it's first argument
    // the token is then passed in conjunction with the arguments
    template<typename function_type, typename...arg_types>
    auto emplace_back(function_type&& function, arg_types&&... args)
        -> std::thread&
    {
        if constexpr (requires {
            { std::invoke(std::forward<function_type>(function), stop_source.get_token(), std::forward<arg_types>(args)...) };
        })
        {
            return threads.emplace_back(std::forward<function_type>(function), stop_source.get_token(), std::forward<arg_types>(args)...);
        } 
        else
        {
            return threads.emplace_back(std::forward<function_type>(function), std::forward<arg_types>(args)...);
        } 
    }

    auto size() const noexcept
        -> std::size_t;

    auto empty() const noexcept
        -> bool;

    auto capacity() const noexcept
        -> std::size_t;

    auto reserve(std::size_t new_capacity)
        -> void;

private:

    std::vector<std::thread> threads;
    std::stop_source stop_source;

};

}