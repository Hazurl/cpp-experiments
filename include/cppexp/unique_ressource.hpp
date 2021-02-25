#pragma once

#include <concepts>

namespace cppexp
{

template<typename resource_type, typename deleter_type>
class unique_resource
{
public:

    // Initialize to non-owning by default
    unique_resource()
        : handled(false)
    {}

    // Initialize the resource and the destructor
    template<typename resource_type_argument, typename deleter_type_argument>
    unique_resource(resource_type_argument&& resource_argument, deleter_type_argument&& deleter_argument)
        : resource(std::forward<resource_type_argument>(resource_argument))
        , deleter(std::forward<deleter_type_argument>(deleter_argument))
        , handled(true)
    {}

    // Change the owner of the resource
    unique_resource(unique_resource&& other)
        : resource(std::move(other.resource))
        , deleter(std::move(other.deleter))
        , handled(other.handled)
    {
        other.release();
    }

    auto operator=(unique_resource&& other) 
        -> unique_resource&&
    {
        reset();
        resource = std::move(other.resource);
        deleter = std::move(other.deleter);
        handled = other.handled;

        other.release();

        return std::move(*this);
    }

    // Call the deleter only if this object is owning the resource
    ~unique_resource()
    {
        reset();
    }

    // Non copyable
    unique_resource(unique_resource const&) = delete;
    auto operator=(unique_resource const&) -> unique_resource& = delete;


    // Stop owning the resource, the deleter won't be called, neither now, nor in the destructor
    auto release() noexcept
        -> void
    {
        handled = false;
    } 

    // Dispose of the old resource by callign the deleter
    auto reset()
        -> void
    {
        if(handled)
        {
            deleter(resource);
            handled = false;
        }
    }

    auto get() const noexcept
        -> resource_type const&
    {
        return resource;
    }

    auto get_deleter() const noexcept
        -> deleter_type const&
    {
        return deleter;
    }

    auto owns() const noexcept
        -> bool
    {
        return handled;
    }

private:

    resource_type resource;
    deleter_type deleter;
    bool handled;

};

/*
template<typename resource_type, typename deleter_type>
unique_resource(resource_type, deleter_type) -> unique_resource<resource_type, deleter_type>;
*/

// Contruct a unique_resource, only owns it if the resource is not equals to 'invalid_resource'
template<typename resource_type, typename deleter_type, typename invalid_resource_type = std::decay_t<resource_type>>
auto make_unique_resource_checked(resource_type&& resource, invalid_resource_type const& invalid_resource, deleter_type&& deleter)
    -> unique_resource<std::decay_t<resource_type>, std::decay_t<deleter_type>>
{
    auto res = unique_resource<resource_type, deleter_type>(std::forward<resource_type>(resource), std::forward<deleter_type>(deleter));
    if (res.get() == invalid_resource)
    {
        res.release();
    }

    return res;
}

}