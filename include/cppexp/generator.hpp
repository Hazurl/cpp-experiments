#pragma once

#include <cppexp/coroutine.hpp>

namespace cppexp 
{

template<typename return_type>
class generator;

namespace details
{

template<typename return_type>
class generator_promise
{
public:

    auto yield_value(return_type&& value) noexcept
        -> std::suspend_always
    {
        value_to_return = std::addressof(value);
        return std::suspend_always{};
    }

    auto yield_value(return_type& value) noexcept
        -> std::suspend_always
    {
        value_to_return = std::addressof(value);
        return std::suspend_always{};
    }

    auto initial_suspend() const noexcept
    {
        return std::suspend_always{};
    }

    auto final_suspend() const noexcept
    {
        return std::suspend_always{};
    }

    auto return_void() const noexcept
    {}

    auto unhandled_exception() noexcept
        -> void
    {
        exception_thrown = std::current_exception();
    }

    auto get_return_object() noexcept
        -> generator<return_type>;

    
    auto rethrow_if_exception() const
        -> void
    {
        if (exception_thrown)
        {
            std::rethrow_exception(exception_thrown);
        }
    }

    auto value() const noexcept
        -> return_type*
    {
        return value_to_return;
    }

private:

    return_type* value_to_return;
    std::exception_ptr exception_thrown;
};

} // namespace details


struct generator_iterator_end_sentinel {};

template<typename return_type>
class generator_iterator;


template<typename return_type>
class generator
{
public:

    using promise_type = details::generator_promise<return_type>;

    generator(unique_coroutine<promise_type> coroutine)
        : coroutine(std::move(coroutine))
    {}

    generator() = default;

    auto is_done() const
        -> bool
    {
        return !*coroutine || coroutine->done();
    }

    // Resume the coroutine
    // Returns true if the coroutine yielded a value
    auto next()
        -> bool
    {
        coroutine->resume();
        if (coroutine->done())
        {
            coroutine->promise().rethrow_if_exception();
            return false;
        }

        return true;
    }

    // UB if the coroutine is done
    auto value() && noexcept
        -> return_type&&
    {
        return std::move(*coroutine->promise().value());
    }

    auto value() const& noexcept
        -> return_type&
    {
        return *coroutine->promise().value();
    }

    auto begin() noexcept
        -> generator_iterator<return_type>;

    auto end() noexcept
        -> generator_iterator_end_sentinel;

private:

    unique_coroutine<promise_type> coroutine;

};

template<typename return_type>
class generator_iterator
{
public:

    generator_iterator(generator<return_type>& gen)
        : gen{ std::addressof(gen) }
    {}

    generator_iterator() = default;

    auto operator++() noexcept
        -> generator_iterator<return_type>&
    {
        gen->next();
        return *this;
    }

    auto operator*() const& noexcept
        -> return_type&
    {
        return gen->value();
    }

    auto operator->() const& noexcept
        -> return_type*
    {
        return std::addressof(operator*());
    }

    friend auto operator==(generator_iterator const& that, generator_iterator_end_sentinel) noexcept
        -> bool
    {
        return that.gen->is_done();
    }

    friend auto operator==(generator_iterator_end_sentinel end, generator_iterator const& that) noexcept
        -> bool
    {
        return that == end;
    }

    friend auto operator!=(generator_iterator const& that, generator_iterator_end_sentinel end) noexcept
        -> bool
    {
        return !(that == end);
    }

    friend auto operator!=(generator_iterator_end_sentinel end, generator_iterator const& that) noexcept
        -> bool
    {
        return !(that == end);
    }

private:

    generator<return_type>* gen;

};


template<typename return_type>
auto details::generator_promise<return_type>::get_return_object() noexcept
    -> generator<return_type>
{
    using promise_type = details::generator_promise<return_type>;
    auto coroutine_handle = std::coroutine_handle<promise_type>::from_promise(*this);
    return generator<return_type>(make_unique_coroutine(coroutine_handle));
}

template<typename return_type>
auto generator<return_type>::begin() noexcept
    -> generator_iterator<return_type>
{
    next();
    return generator_iterator(*this);
}

template<typename return_type>
auto generator<return_type>::end() noexcept
    -> generator_iterator_end_sentinel
{
    return {};
}


}