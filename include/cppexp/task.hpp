#pragma once

#include <coroutine>

#include <cppexp/coroutine.hpp>

namespace cppexp
{

template<typename return_type = void>
class task;

namespace details
{

struct final_task_awaiter 
{
    auto await_ready() noexcept
        -> bool 
    {
        return false;
    }

    auto await_suspend(std::coroutine_handle<>) noexcept
        -> std::coroutine_handle<>
    {
        if (continuation)
        {
            return continuation;
        } 
        else
        {
            return std::noop_coroutine();
        }
    }

    auto await_resume() noexcept
        -> void
    {}

    std::coroutine_handle<> continuation;
};


template<typename return_type>
class task_promise
{
public:

    auto return_value(return_type value) noexcept
        -> void
    {
        value_to_return = std::move(value);
    }

    auto initial_suspend() const noexcept
    {
        return std::suspend_always{};
    }

    auto final_suspend() const noexcept
    {
        return final_task_awaiter{ continuation };
    }

    auto rethrow_if_exception() const
        -> void
    {
        if (exception_thrown)
        {
            std::rethrow_exception(exception_thrown);
        }
    }

    auto unhandled_exception() noexcept
        -> void
    {
        exception_thrown = std::current_exception();
    }

    auto get_return_object() noexcept
        -> task<return_type>;

    auto value() const& noexcept
        -> return_type const&
    {
        return value_to_return;
    }

    auto value() & noexcept
        -> return_type&
    {
        return value_to_return;
    }

    auto value() && noexcept
        -> return_type&&
    {
        return std::move(value_to_return);
    }

    auto set_continuation(std::coroutine_handle<> coroutine) noexcept
        -> void
    {
        continuation = coroutine;
    }

private:

    return_type value_to_return;
    std::exception_ptr exception_thrown;
    std::coroutine_handle<> continuation;

};


struct task_awaiter_base
{

    task_awaiter_base(std::coroutine_handle<> continuation) noexcept
        : continuation{ continuation }
    {}

    task_awaiter_base() noexcept = default;

    auto await_ready() noexcept
        -> bool 
    {
        return !continuation || continuation.done();
    }

    auto await_suspend(std::coroutine_handle<> coroutine) noexcept
        -> std::coroutine_handle<>
    {
        continuation.promise().set_continuation(coroutine);
        return continuation;
    }

    std::coroutine_handle<> continuation;

};

} // namespace details 



template<typename return_type>
class task 
{
public:

    using promise_type = task_promise<return_type>;

    task(unique_coroutine<promise_type> coroutine)
        : coroutine(std::move(coroutine))
    {}

    task() = default;

    auto operator co_await() const& noexcept
    {
        struct task_awaiter : details::task_awaiter_base
        {
            auto await_resume()
                -> return_type&
            {
                auto& promise = this->continuation.promise();
                return promise.value();
            }
        };

        return task_awaiter{ coroutine.get() };
    }

    auto operator co_await() const&& noexcept
    {
        struct task_awaiter : details::task_awaiter_base
        {
            auto await_resume()
                -> return_type&&
            {
                auto& promise = this->continuation.promise();
                return std::move(promise).value();
            }
        };

        return task_awaiter{ coroutine.get() };
    }

private:

    unique_coroutine<promise_type> coroutine;

};


template<typename return_type>
auto details::task_promise<return_type>::get_return_object() noexcept
    -> task<return_type>
{
    using promise_type = details::task_promise<return_type>;
    auto coroutine_handle = std::coroutine_handle<promise_type>::from_promise(*this);
    return task<return_type>(make_unique_coroutine(coroutine_handle));
}


}