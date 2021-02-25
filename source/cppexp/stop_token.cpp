#include <cppexp/stop_token.hpp>

namespace cppexp
{

auto stop_token::request_stop() noexcept
    -> void
{
    is_requested.store(true, std::memory_order_relaxed);
}

auto stop_token::stop_requested() const noexcept
    -> bool
{
    return is_requested.load(std::memory_order_relaxed);
}

auto stop_token::reset() noexcept
    -> void
{
    is_requested.store(false, std::memory_order_relaxed);
}


}