#include <iostream>
#include <thread>
#include <chrono>

#include <cppexp/unique_ressource.hpp>
#include <cppexp/deleter.hpp>
#include <cppexp/coroutine.hpp>
#include <cppexp/stop_token.hpp>

int main() {

    auto ressource = cppexp::make_unique_resource_checked<int*>(new int{ 42 }, nullptr, cppexp::make_deleter<int>());

    auto other = std::move(ressource);
    ressource = std::move(other);

    auto ressource2 = cppexp::unique_resource(new int{ 1337 }, cppexp::make_deleter<int>());

    auto cor = cppexp::make_unique_coroutine({});

    auto stop_token = cppexp::stop_token{};
    auto thread = std::thread([&stop_token]
    {
        while(not stop_token.stop_requested())
        {
            std::cout << "Working...\n";
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(750ms);
        }
        std::cout << "Stop!\n";
    });

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(2s);

    std::cout << "Request stop!\n"; // Yeah I know std::cout is not thread-safe, but I don't care
    stop_token.request_stop();

    thread.join();

}