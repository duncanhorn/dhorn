/*
 * Duncan Horn
 *
 * visitor.h
 *
 * 
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
