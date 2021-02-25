#include <iostream>

#include <cppexp/unique_ressource.hpp>
#include <cppexp/deleter.hpp>

int main() {

    auto ressource = cppexp::make_unique_resource_checked<int*>(new int{ 42 }, nullptr, cppexp::make_deleter<int>());

    auto other = std::move(ressource);
    ressource = std::move(other);

    auto ressource2 = cppexp::unique_resource(new int{ 1337 }, cppexp::make_deleter<int>());

}