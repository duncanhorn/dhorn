/*
 * Duncan Horn
 *
 * functional.h
 *
 * A set of helpers/utilities for dealing with function objects.
 */
#pragma once

#include <memory>

namespace dhorn
{
    /*
     * make_lambda_shared
     *
     * Wraps a lambda (or any function object, really) into a shared pointer so that it can be copied or passed to a
     * function/type that requires a copy-able function object (such as std::function).
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * auto foo = std::make_unique<Foo>();
     * std::function<void(void)> func = make_lambda_shared([foo = std::move(foo)]()
     * {
     *     // Use foo
     * });
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    template <typename Func>
    inline auto make_lambda_shared(Func&& func)
    {
        return [sharedFunc = std::make_shared<std::decay_t<Func>>(std::forward<Func>(func))](auto&&... args)
        {
            (*sharedFunc)(std::forward<decltype(args)>(args)...);
        };
    }
}
