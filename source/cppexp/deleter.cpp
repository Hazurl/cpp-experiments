#include <cppexp/deleter.hpp>

namespace cppexp
{

auto make_coroutine_deleter()
    -> pure_deleter_t<std::coroutine_handle<>>
{
    return [](std::coroutine_handle<>& coroutine)
    {
        coroutine.destroy();
    };
}

}