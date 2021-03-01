#include <iostream>
#include <thread>
#include <chrono>

#include <cppexp/unique_ressource.hpp>
#include <cppexp/deleter.hpp>
#include <cppexp/coroutine.hpp>
#include <cppexp/thread_pool.hpp>
#include <cppexp/generator.hpp>

template<typename...arg_types>
auto print_to_cout(arg_types&&... args)
    -> void
{
    static std::mutex cout_mutex;
    auto lk = std::scoped_lock(cout_mutex);

    (std::cout << ... << std::forward<arg_types>(args));
}

cppexp::generator<std::size_t> range(std::size_t from, std::size_t to, std::size_t step = 1)
{
    for(; from < to; from += step)
    {
        co_yield from;
    }
}

int main() {

    auto ressource = cppexp::make_unique_resource_checked<int*>(new int{ 42 }, nullptr, cppexp::make_deleter<int>());

    auto other = std::move(ressource);
    ressource = std::move(other);

    auto ressource2 = cppexp::unique_resource(new int{ 1337 }, cppexp::make_deleter<int>());

    auto cor = cppexp::make_unique_coroutine({});

    cppexp::thread_pool<std::function<void()>> thread_pool(4);
    std::mutex cout_mutex;

    for(int i = 0; i < 10; ++i)
    {
        thread_pool.schedule(
            [i] ()
            {
                print_to_cout("+ Starting execution of #", i, " on thread #", std::this_thread::get_id(), "...\n");
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(250ms * ((i * 3) % 5 + 1));
                print_to_cout("- Stoping execution of #", i, " on thread #", std::this_thread::get_id(), "...\n");
            });
    }

    thread_pool.join();

    for(auto value : range(0, 10, 2))
    {
        std::cout << "Yielded: " << value << '\n';
    }

}