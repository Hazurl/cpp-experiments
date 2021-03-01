#pragma once

#include <coroutine>

namespace cppexp
{


template<typename type>
using pure_deleter_t = auto(*)(type&) -> void;


// Use the operator delete
template<typename type>
auto make_deleter()
    -> pure_deleter_t<type*>
{
    return [](type*& value) 
    {
        delete value;
    };
}


// Call destroy on the coroutine
template<typename promise_type = void>
auto make_coroutine_deleter()
    -> pure_deleter_t<std::coroutine_handle<promise_type>>
{
    return [](std::coroutine_handle<promise_type>& coroutine)
    {
        coroutine.destroy();
    };
}

}