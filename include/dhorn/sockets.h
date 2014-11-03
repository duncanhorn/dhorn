/*
 * sockets.h
 *
 * Duncan Horn
 *
 * 
 */
#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>

#ifdef  WIN32

/* Force the project to link against the Windows Sockets library */
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>

#else

#error "Your compiler is not yet supported by the dhorn sockets API"

#endif

namespace dhorn
{
    /*
     * Socket Types and Constants
     */
#pragma region Socket Types and Constants

#ifdef  WIN32
    using socket_t = SOCKET;
    using socket_error_t = int;

    static const socket_t invalid_socket = INVALID_SOCKET;
    static const socket_error_t socket_error = SOCKET_ERROR;

    // Not defined using htonl since all values exposed to client code should be in host-byte-order. We do the
    // translation to network-byte-order when these values are used by the implementation.
    static const uint32_t any_address = INADDR_ANY;
    static const uint32_t loopback_address = INADDR_LOOPBACK;
    static const uint32_t broadcast_address = INADDR_BROADCAST;
    static const uint32_t invalid_address = INADDR_NONE;
#endif  /*WIN32*/

    enum class address_family : uint16_t
    {
        unspecified                     = AF_UNSPEC,
        unix                            = AF_UNIX,
        internetwork_version_4          = AF_INET,
        arpanet_imp                     = AF_IMPLINK,
        pup_protocols                   = AF_PUP,
        mit_chaos_protocols             = AF_CHAOS,
        xerox_ns_protocols              = AF_NS,
        ipx_protocols                   = AF_IPX,
        iso_protocols                   = AF_ISO,
        osi_protocols                   = AF_OSI,
        european_computer_manufacturers = AF_ECMA,
        data_kit_protocols              = AF_DATAKIT,
        ccitt_protocols                 = AF_CCITT,
        ibm_sna                         = AF_SNA,
        dec_net                         = AF_DECnet,
        direct_data_link_interface      = AF_DLI,
        lat                             = AF_LAT,
        nsc_hyperchannel                = AF_HYLINK,
        apple_talk                      = AF_APPLETALK,
        net_bios                        = AF_NETBIOS,
        voice_view                      = AF_VOICEVIEW,
        fire_fox_protocols              = AF_FIREFOX,
        unknown_1                       = AF_UNKNOWN1,
        banyan                          = AF_BAN,
        native_atm_services             = AF_ATM,
        internetwork_version_6          = AF_INET6,
        microsoft_wolfpack              = AF_CLUSTER,
        ieee_12844_4_wg_af              = AF_12844,
        irda                            = AF_IRDA,
        net_designers_osi_gateway       = AF_NETDES,
    };

    enum class socket_type : int
    {
        stream                      = SOCK_STREAM,
        datagram                    = SOCK_DGRAM,
        raw_protocol                = SOCK_RAW,
        reliably_delivered_message  = SOCK_RDM,
        sequenced_packet_stream     = SOCK_SEQPACKET,
    };

    enum class ip_protocol
    {
        ipv6_hop_by_hop_options                 = IPPROTO_HOPOPTS,
        internet_control_message_protocol       = IPPROTO_ICMP,
        internet_group_management_protocol      = IPPROTO_IGMP,
        gateway_to_gateway_protocol             = IPPROTO_GGP,
        version_4_header                        = IPPROTO_IPV4,
        stream_protocol                         = IPPROTO_ST,
        transmission_control_protocol           = IPPROTO_TCP,
        core_based_trees_protocol               = IPPROTO_CBT,
        exterior_gateway_protocol               = IPPROTO_EGP,
        private_interior_gateway_protocol       = IPPROTO_IGP,
        parc_universal_packet_protocol          = IPPROTO_PUP,
        user_datagram_protocol                  = IPPROTO_UDP,
        internet_datagram_protocol              = IPPROTO_IDP,
        reliable_data_protocol                  = IPPROTO_RDP,
        version_6_header                        = IPPROTO_IPV6,
        ipv6_routing_header                     = IPPROTO_ROUTING,
        ipv6_fragmentation_header               = IPPROTO_FRAGMENT,
        encapsulating_security_payload          = IPPROTO_ESP,
        authentication_header                   = IPPROTO_AH,
        ipv6_internet_control_message_protocol  = IPPROTO_ICMPV6,
        ipv6_no_next_header                     = IPPROTO_NONE,
        ipv6_destination_options                = IPPROTO_DSTOPTS,
        net_disk_protocol                       = IPPROTO_ND,
        wideband_monitoring                     = IPPROTO_ICLFXBM,
        protocol_independent_multicast          = IPPROTO_PIM,
        programatic_general_multicast           = IPPROTO_PGM,
        level_2_tunneling_protocol              = IPPROTO_L2TP,
        stream_control_transmission_protocol    = IPPROTO_SCTP,
        raw_ip_packets                          = IPPROTO_RAW,
    };

#pragma endregion



    /*
     * socket_exception
     */
#pragma region socket_exception

    class socket_exception :
        public std::exception
    {
    public:
        socket_exception(_In_ socket_error_t error) :
            _error(error)
        {
        }

        virtual char *what(void)
        {
            return "socket_exception";
        }

        socket_error_t get_error(void)
        {
            return this->_error;
        }

    private:

        socket_error_t _error;
    };

    namespace garbage
    {
        inline void wsa_throw_last_error(void)
        {
            auto error = WSAGetLastError();
            if (error)
            {
                throw socket_exception(error);
            }
        }

        template <typename Ty>
        inline void wsa_throw_if_null(_In_ Ty *ptr)
        {
            if (!ptr)
            {
                wsa_throw_last_error();
            }
        }

        inline socket_error_t wsa_throw_if_error(_In_ socket_error_t val)
        {
            if (val == socket_error)
            {
                wsa_throw_last_error();
            }

            return val;
        }

        inline void wsa_throw_if_false(_In_ bool expr, _In_ socket_error_t error)
        {
            if (!expr)
            {
                throw socket_exception(error);
            }
        }

        inline void wsa_throw_if_true(_In_ bool expr, _In_ socket_error_t error)
        {
            if (expr)
            {
                throw socket_exception(error);
            }
        }
    }

#pragma endregion



    /*
     * Sockets Traits
     */
    namespace garbage
    {
        /*
         * address_family_traits
         */
        template <address_family Family>
        struct address_family_traits;

        template <>
        struct address_family_traits<address_family::internetwork_version_4>
        {
            using ip_addr = in_addr;
            using create_type = unsigned long;
            static const address_family family = address_family::internetwork_version_4;
            static const int max_string_len = 16;

            static inline ip_addr create(_In_ create_type val)
            {
                ip_addr result;
                result.s_addr = htonl(val);
                return result;
            }
        };

        template <>
        struct address_family_traits<address_family::internetwork_version_6>
        {
            using ip_addr = in6_addr;
            using create_type = uint8_t[16];
            static const address_family family = address_family::internetwork_version_6;
            static const int max_string_len = 46;

            static inline ip_addr create(_In_ const create_type &val)
            {
                // create_type is already in network-byte-order, so we can just copy as-is
                ip_addr result;
                memcpy(result.s6_addr, val, sizeof(result.s6_addr));
                return result;
            }
        };



        /*
         * sockets_string_traits
         */
        template <typename CharType, address_family Family>
        struct sockets_string_traits;

        template <address_family Family>
        struct sockets_string_traits<char, Family>
        {
            using MyTraits = address_family_traits<Family>;
            using ip_addr = typename MyTraits::ip_addr;

            inline static std::string n_to_p(_In_ const ip_addr &addr)
            {
                char buff[MyTraits::max_string_len + 1];

                auto result = InetNtopA(static_cast<int>(Family), (PVOID)&addr, buff, sizeof(buff));
                garbage::wsa_throw_if_null(result);

                return buff;
            }
        };

        template <address_family Family>
        struct sockets_string_traits<wchar_t, Family>
        {
            using MyTraits = address_family_traits<Family>;
            using ip_addr = typename MyTraits::ip_addr;

            inline static std::wstring n_to_p(_In_ const ip_addr &addr)
            {
                wchar_t buff[MyTraits::max_string_len + 1];

                auto result = InetNtopW(static_cast<int>(Family), (PVOID)&addr, buff, sizeof(buff) / 2);
                garbage::wsa_throw_if_null(result);

                return buff;
            }
        };
    }



    /*
     * dhorn::ip_address
     */
#pragma region ip_address

    template <address_family Family>
    class ip_address
    {
        using MyTraits = garbage::address_family_traits<Family>;

    public:

        /*
         * Type/Value Definitions
         */
        using ip_addr = typename MyTraits::ip_addr;
        static const address_family family = Family;



        /*
         * Constructor(s)/Destructor
         */
        ip_address(void) :
            _addr{}
        {
        }

        ip_address(_In_ const typename MyTraits::create_type &val) :
            _addr(MyTraits::create(val))
        {
        }

        ip_address(_In_ const ip_addr &ip) :
            _addr(ip)
        {
        }

        template <typename CharType>
        ip_address(_In_ const CharType *ip)
        {
            this->Set(ip);
        }

        template <typename CharType>
        ip_address(_In_ const std::basic_string<CharType> &ip)
        {
            this->Set(ip.c_str());
        }



        /*
         * Assignment Operators
         */
        ip_address &operator=(_In_ const typename MyTraits::create_type &val)
        {
            this->_addr = MyTraits::create(val);
            return *this;
        }

        ip_address &operator=(_In_ const ip_addr &ip)
        {
            this->_addr = ip;
            return *this;
        }

        template <typename CharType>
        ip_address &operator=(_In_ const CharType *ip)
        {
            this->Set(ip);
            return *this;
        }

        template <typename CharType>
        ip_address &operator=(_In_ const std::basic_string<CharType> &ip)
        {
            this->Set(ip.c_str());
            return *this;
        }



        /*
         * Type Conversion Operators
         */
        operator const ip_addr &(void) const
        {
            return this->_addr;
        }

        template <typename CharType>
        operator std::basic_string<CharType>(void) const
        {
            return this->str<CharType>();
        }



        /*
         * Public Functions
         */
        const ip_addr &addr(void) const
        {
            return this->_addr;
        }

        template <typename CharType = char>
        std::basic_string<CharType> str(void) const
        {
            return garbage::sockets_string_traits<CharType, Family>::n_to_p(this->_addr);
        }



    private:

        void Set(_In_ const char *ip)
        {
            auto result = garbage::wsa_throw_if_error(InetPtonA(static_cast<int>(Family), ip, &this->_addr));
            if (result == 0)
            {
                // Result of an invalid string. WSAGetLastError will give us zero, so provide our own error code
                throw socket_exception(WSA_INVALID_PARAMETER);
            }
        }

        void Set(_In_ const wchar_t *ip)
        {
            auto result = garbage::wsa_throw_if_error(InetPtonW(static_cast<int>(Family), ip, &this->_addr));
            if (result == 0)
            {
                // Result of an invalid string. WSAGetLastError will give us zero, so provide our own error code
                throw socket_exception(WSA_INVALID_PARAMETER);
            }
        }



        ip_addr _addr;
    };

    using ipv4_address = ip_address<address_family::internetwork_version_4>;
    using ipv6_address = ip_address<address_family::internetwork_version_6>;

#pragma endregion



    /*
     * socket_address
     */
    class socket_address final
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        socket_address(_In_ const ipv4_address &addr, _In_ uint16_t port)
        {
            this->_ipv4Addr =
            {
                static_cast<int>(ipv4_address::family),     // sin_family
                htons(port),                                // sin_port
                addr                                        // sin_addr
            };

            this->_size = sizeof(this->_ipv4Addr);
        }

        socket_address(
            _In_ const ipv6_address &addr,
            _In_ uint16_t port,
            _In_ uint32_t flowInfo,
            _In_ uint32_t scopeId)
        {
            this->_ipv6Addr =
            {
                static_cast<int>(ipv6_address::family),     // sin6_family
                htons(port),                                // sin6_port
                flowInfo,                                   // sin6_flowinfo
                addr,                                       // sin6_addr
                { scopeId }                                 // sin6_scope_id
            };

            this->_size = sizeof(this->_ipv6Addr);
        }



        /*
         * Operators
         */
        operator const sockaddr *(void) const
        {
            return &this->_addr;
        }

        operator sockaddr &(void)
        {
            return this->_addr;
        }

        operator const sockaddr &(void) const
        {
            return this->_addr;
        }

        operator sockaddr_in &(void)
        {
            return this->_ipv4Addr;
        }

        operator const sockaddr_in &(void) const
        {
            return this->_ipv4Addr;
        }

        operator sockaddr_in6 &(void)
        {
            return this->_ipv6Addr;
        }

        operator const sockaddr_in6 &(void) const
        {
            return this->_ipv6Addr;
        }



        /*
         * Public Functions
         */
        void assign(_In_ const ipv4_address &addr, _In_ uint16_t port)
        {
            this->_ipv4Addr =
            {
                static_cast<int>(ipv4_address::family),     // sin_family
                htons(port),                                // sin_port
                addr                                        // sin_addr
            };

            this->_size = sizeof(this->_ipv4Addr);
        }

        void assign(_In_ const ipv6_address &addr, _In_ uint16_t port, _In_ uint32_t flowInfo, _In_ uint32_t scopeId)
        {
            this->_ipv6Addr =
            {
                static_cast<int>(ipv6_address::family),     // sin6_family
                htons(port),                                // sin6_port
                flowInfo,                                   // sin6_flowinfo
                addr,                                       // sin6_addr
                { scopeId }                                 // sin6_scope_id
            };

            this->_size = sizeof(this->_ipv6Addr);
        }

        size_t size(void) const
        {
            return this->_size;
        }



    private:

        union
        {
            sockaddr _addr;
            sockaddr_in _ipv4Addr;
            sockaddr_in6 _ipv6Addr;
        };

        size_t _size;
    };



    /*
     * socket_base
     */
    namespace garbage
    {
        /*
         * Only to be used by socket_base constructor to call WSAStartup to load WinSock.dll
         */
        struct socket_initializer
        {
            socket_initializer(void)
            {
                WSADATA data;
                auto result = WSAStartup(MAKEWORD(2, 2), &data);

                // If the dll load fails, we cannot call WSAGetLastError
                if (result)
                {
                    throw socket_exception(result);
                }
            }

            ~socket_initializer(void)
            {
                wsa_throw_if_error(WSACleanup());
            }
        };



        /*
         * Base socket class
         */
        class socket_base
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            socket_base(void) :
                _socket(invalid_socket)
            {
                // Under the C++ standard, the construction of static function variables is not thread safe, so the
                // constructor technically can get called more than once here. In the unlikely event that this does
                // happen, we'll just end up calling WSAStartup more than once while only making one call to
                // WSACleanup. Since the system will re-claim our resources, we just ignore the possibility
                static const socket_initializer _init;
            }



            /*
             * Socket functions
             */
            void open(_In_ address_family family, _In_ socket_type type, _In_ ip_protocol protocol)
            {
                wsa_throw_if_false(this->_socket == invalid_socket, WSAEISCONN);

                this->_socket = socket(static_cast<int>(family), static_cast<int>(type), static_cast<int>(protocol));
                if (this->_socket == invalid_socket)
                {
                    wsa_throw_last_error();
                }
            }

            void bind(_In_ const socket_address &addr)
            {
                wsa_throw_if_error(::bind(this->_socket, addr, addr.size()));
            }



        protected:

            //socket_address _addr;
            socket_t _socket;
        };
    }



    /*
     * udp_socket
     */
    class udp_socket final
    {
    public:
        /*
         * Constructor(s)/Destructor
         */


    private:

    };
}
