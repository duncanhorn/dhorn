/*
 * Duncan Horn
 *
 * functional.h
 *
 * Things that I feel are missing from the C++ standard's functional (or similar)  header
 */
#pragma once

namespace dhorn
{
    /*
     * bind_member_function
     */
#pragma region bind_member_function

    namespace garbage
    {
        template <typename Ty, typename ResultType, typename... Args>
        struct member_function
        {
            using result_type = ResultType;
            using function_type = result_type(Ty::*)(Args...);

            member_function(Ty *ptr, function_type &func) :
                ptr(ptr),
                func(func)
            {
            }

            result_type operator()(_In_ Args&&... args)
            {
                return (ptr->*func)(std::forward<Args>(args)...);
            }

            // Members
            Ty *ptr;
            function_type func;
        };
    }

    template <typename Ty, typename ResultType, typename... Args>
    garbage::member_function<Ty, ResultType, Args...> bind_member_function(
        _In_ ResultType(Ty::*func)(Args...),
        _In_ Ty &obj)
    {
        return garbage::member_function<Ty, ResultType, Args...>(&obj, func);
    }

    template <typename Ty, typename ResultType, typename... Args>
    garbage::member_function<Ty, ResultType, Args...> bind_member_function(
        _In_ ResultType(Ty::*func)(Args...),
        _In_ Ty *ptr)
    {
        return garbage::member_function<Ty, ResultType, Args...>(ptr, func);
    }

#pragma endregion
}
