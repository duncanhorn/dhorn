/* 
 * Duncan Horn
 *
 * net.h
 *
 * 
 */
#pragma once

#include "utility.h"

namespace dhorn
{
    /*
     * Byte Order
     */
#pragma region Byte Order

    namespace details
    {
        template <typename Integer, size_t... Indices>
        inline constexpr Integer host_to_network(Integer value, std::index_sequence<Indices...>)
        {
            union type
            {
                uint8_t bytes[sizeof(Integer)];
                Integer value;
            };

            return type{ get_byte<Indices>(value)... }.value;
        }
    }



    /*
     * host_to_network
     */
    template <typename Integer>
    inline constexpr Integer host_to_network(Integer value)
    {
        // First byte should be the most significant byte, second should be second most significant, etc.
        using seq = make_reverse_index_sequence<sizeof(Integer)>;
        return details::host_to_network(value, seq{});
    }



    /*
     * network_to_host
     */
    template <typename Integer>
    inline constexpr Integer network_to_host(Integer value)
    {
        // First byte is the most significant byte, second byte is the second most significant byte, etc. This is just
        // the reverse of host_to_network, so we can just use that...
        return host_to_network(value);
    }

#pragma endregion
}
