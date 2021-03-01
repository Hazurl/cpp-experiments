#pragma once

#include <functional>

namespace cppexp 
{

template<typename return_type>
auto execute_job_sync(std::function<return_type()> const& job) 
    -> void
{
    job();
}

template<typename return_type>
auto execute_job_sync(return_type (*job) ()) 
    -> void
{
    job();
}

template<typename job_type>
concept job_c = requires(job_type j) 
{
    { ::cppexp::execute_job_sync(j) };
};


}