#pragma once

#include <coroutine>

#include <cppexp/unique_ressource.hpp>
#include <cppexp/deleter.hpp>

namespace cppexp
{


template<typename promise_type = void>
using unique_coroutine = unique_resource<
    std::coroutine_handle<promise_type>, 
    pure_deleter_t<std::coroutine_handle<promise_type>>>;


template<typename promise_type = void>
auto make_unique_coroutine(std::coroutine_handle<promise_type> handle)
    -> unique_coroutine<promise_type>
{
    return make_unique_resource_checked(handle, std::coroutine_handle<promise_type>{}, make_coroutine_deleter<promise_type>());
}

}