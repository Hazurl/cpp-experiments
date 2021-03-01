#include <cppexp/multi_jthread.hpp>


namespace cppexp
{

multi_jthread::multi_jthread(multi_jthread&& other)
    : threads(std::move(other.threads))
    , stop_source(std::move(other.stop_source))
{}

multi_jthread& multi_jthread::operator=(multi_jthread&& other)
{
    std::swap(*this, other);
    return *this;
}

multi_jthread::~multi_jthread()
{
    request_stop();
    join();
}

auto multi_jthread::join()
    -> void
{
    for(auto& thread : threads)
    {
        thread.join();
    }

    threads.clear();
}

auto multi_jthread::request_stop() noexcept
    -> bool
{
    return stop_source.request_stop();
}

auto multi_jthread::detach()
    -> void
{
    for(auto& thread : threads)
    {
        thread.detach();
    }

    threads.clear();
}

auto multi_jthread::size() const noexcept
    -> std::size_t
{
    return threads.size();
}

auto multi_jthread::empty() const noexcept
    -> bool
{
    return threads.empty();
}

auto multi_jthread::capacity() const noexcept
    -> std::size_t
{
    return threads.capacity();
}

auto multi_jthread::reserve(std::size_t new_capacity)
    -> void
{
    return threads.reserve(new_capacity);
}

}