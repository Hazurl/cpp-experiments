#pragma once

#include <coroutine>

#include <cppexp/unique_ressource.hpp>
#include <cppexp/deleter.hpp>

namespace cppexp
{

template<typename promise_type = void>
auto make_unique_coroutine(std::coroutine_handle<promise_type> handle)
    -> unique_resource<std::coroutine_handle<promise_type>, pure_deleter_t<std::coroutine_handle<promise_type>>>
{
    return make_unique_resource_checked(handle, std::coroutine_handle<>{}, make_coroutine_deleter());
}

}