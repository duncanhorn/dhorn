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

    namespace details
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

            template <typename... ArgsTy>
            result_type operator()(ArgsTy &&...args)
            {
                return (ptr->*func)(std::forward<ArgsTy>(args)...);
            }

            // Members
            Ty *ptr;
            function_type func;
        };
    }

    template <typename Ty, typename ResultType, typename... Args>
    details::member_function<Ty, ResultType, Args...> bind_member_function(ResultType (Ty::*func)(Args...), Ty &obj)
    {
        return details::member_function<Ty, ResultType, Args...>(&obj, func);
    }

    template <typename Ty, typename ResultType, typename... Args>
    details::member_function<Ty, ResultType, Args...> bind_member_function(ResultType (Ty::*func)(Args...), Ty *ptr)
    {
        return details::member_function<Ty, ResultType, Args...>(ptr, func);
    }

#pragma endregion
}
