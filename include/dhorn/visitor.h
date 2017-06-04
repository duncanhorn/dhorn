/*
 * Duncan Horn
 *
 * visitor.h
 *
 * Creating a visitor for use with `std::visit` (or the visitor pattern in general) is a pain without multiple features
 * added in C++17, namely class template deduction, variadic pack expansion of in-class using-declarations, and
 * aggregate initialization of base classes. Unfortunately we have none of those, so we need to employ a C++14 solution
 * which is much less elegant.
 */
#pragma once

#include <algorithm>
#include <type_traits>

namespace dhorn
{
    template <typename... InvocableTypes>
    struct visitor;

    template <typename Invocable>
    struct visitor<Invocable> :
        public Invocable
    {
        template <typename Arg>
        visitor(Arg&& arg) :
            Invocable(std::forward<Arg>(arg))
        {
        }

        using Invocable::operator();
    };

    template <typename Invocable, typename... InvocableTypes>
    struct visitor<Invocable, InvocableTypes...> :
        public Invocable,
        public visitor<InvocableTypes...>
    {
        template <typename Arg, typename... Args>
        visitor(Arg&& arg, Args&&... args) :
            Invocable(std::forward<Arg>(arg)),
            visitor<InvocableTypes...>(std::forward<Args>(args)...)
        {
        }

        using Invocable::operator();
        using visitor<InvocableTypes...>::operator();
    };

    template <typename... InvocableTypes>
    inline auto make_visitor(InvocableTypes&&... args)
    {
        return visitor<std::decay_t<InvocableTypes>...>(std::forward<InvocableTypes>(args)...);
    }
}
