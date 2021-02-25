#pragma once

#include <atomic>

namespace cppexp 
{
    
class stop_token
{
public:

    auto request_stop() noexcept
        -> void;

    auto stop_requested() const noexcept
        -> bool;

    auto reset() noexcept
        -> void;

private:

    std::atomic_bool is_requested{ false };

};

}