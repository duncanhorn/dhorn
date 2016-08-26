/*
 * Duncan Horn
 *
 * sockets.h
 *
 * 
 */
#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <set>
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

    static const size_t max_message_size = SO_MAX_MSG_SIZE;

    // Not defined using htonl since all values exposed to client code should be in host-byte-order. We do the
    // translation to network-byte-order when these values are used by the implementation.
    static const uint32_t any_address = INADDR_ANY;
    static const uint32_t loopback_address = INADDR_LOOPBACK;
    static const uint32_t local_host = INADDR_LOOPBACK;
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

    enum class message_flags : int
    {
        none            = 0,
        dont_route      = MSG_DONTROUTE,
        interrupt       = MSG_INTERRUPT,
        out_of_band     = MSG_OOB,
        partial         = MSG_PARTIAL,
        peek            = MSG_PEEK,
        push_immediate  = MSG_PUSH_IMMEDIATE,
        wait_all        = MSG_WAITALL,
    };

    inline message_flags operator|(message_flags lhs, message_flags rhs)
    {
        return static_cast<message_flags>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline message_flags operator&(message_flags lhs, message_flags rhs)
    {
        return static_cast<message_flags>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    enum class shutdown_options : int
    {
        send    = SD_SEND,
        receive = SD_RECEIVE,
        both    = SD_BOTH,
    };

    enum class socket_level : int
    {
        socket  = SOL_SOCKET,
    };

    enum class socket_option : int
    {
        debug_info                      = SO_DEBUG,
        listening                       = SO_ACCEPTCONN,
        reuse_address                   = SO_REUSEADDR,
        keep_alive                      = SO_KEEPALIVE,
        dont_route                      = SO_DONTROUTE,
        broadcast                       = SO_BROADCAST,
        use_loopback                    = SO_USELOOPBACK,
        linger_on_close                 = SO_LINGER,
        oob_inline                      = SO_OOBINLINE,
        dont_linger_on_close            = SO_DONTLINGER,
        exclusive_address_use           = SO_EXCLUSIVEADDRUSE,

        send_buffer_size                = SO_SNDBUF,
        receive_buffer_size             = SO_RCVBUF,
        send_low_water_mark             = SO_SNDLOWAT,
        receive_low_water_mark          = SO_RCVLOWAT,
        send_timeout                    = SO_SNDTIMEO,
        receive_timeout                 = SO_RCVTIMEO,
        error_status                    = SO_ERROR,
        socket_type                     = SO_TYPE,

        group_id                        = SO_GROUP_ID,
        group_priority                  = SO_GROUP_PRIORITY,
        max_message_size                = SO_MAX_MSG_SIZE,
        protocol_info_ansi              = SO_PROTOCOL_INFOA,
        protocol_info_wide              = SO_PROTOCOL_INFOW,
        protocol_info                   = SO_PROTOCOL_INFO,
        service_provider_config_info    = PVD_CONFIG,
        conditional_accept              = SO_CONDITIONAL_ACCEPT,
    };

    inline socket_option operator|(socket_option lhs, socket_option rhs)
    {
        return static_cast<socket_option>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline socket_option operator&(socket_option lhs, socket_option rhs)
    {
        return static_cast<socket_option>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    enum class io_control_command : unsigned long
    {
        available_bytes     = FIONREAD,
        blocking            = FIONBIO,
        asynchronous        = FIOASYNC,
        set_high_watermark  = SIOCSHIWAT,
        get_high_watermark  = SIOCGHIWAT,
        set_low_watermark   = SIOCSLOWAT,
        get_low_watermark   = SIOCGLOWAT,
        at_oob_mark         = SIOCATMARK,
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
        socket_exception(socket_error_t error) :
            _error(error)
        {
        }

        virtual const char *what(void)
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
        inline void wsa_throw_if_null(Ty *ptr)
        {
            if (!ptr)
            {
                wsa_throw_last_error();
            }
        }

        inline socket_error_t wsa_throw_if_error(socket_error_t val)
        {
            if (val == socket_error)
            {
                wsa_throw_last_error();
            }

            return val;
        }

        inline void wsa_throw_if_false(bool expr, socket_error_t error)
        {
            if (!expr)
            {
                throw socket_exception(error);
            }
        }

        inline void wsa_throw_if_true(bool expr, socket_error_t error)
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
            using sock_addr = sockaddr_in;
            using create_type = unsigned long;
            static const address_family family = address_family::internetwork_version_4;
            static const int max_string_len = 16;

            static inline const ip_addr &create(const sock_addr &addr)
            {
                garbage::wsa_throw_if_false(addr.sin_family == static_cast<int>(family), WSAEINVAL);
                return addr.sin_addr;
            }

            static inline ip_addr create(create_type val)
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
            using sock_addr = sockaddr_in6;
            using create_type = uint8_t[16];
            static const address_family family = address_family::internetwork_version_6;
            static const int max_string_len = 46;

            static inline const ip_addr &create(const sock_addr &addr)
            {
                garbage::wsa_throw_if_false(addr.sin6_family == static_cast<int>(family), WSAEINVAL);
                return addr.sin6_addr;
            }

            static inline ip_addr create(const create_type &val)
            {
                // create_type is already in network-byte-order, so we can just copy as-is
                ip_addr result;
                memcpy(result.s6_addr, val, sizeof(result.s6_addr));
                return result;
            }
        };



        /*
         * socket_string_traits
         */
        template <typename CharType, address_family Family>
        struct socket_string_traits;

        template <address_family Family>
        struct socket_string_traits<char, Family>
        {
            using MyTraits = address_family_traits<Family>;
            using ip_addr = typename MyTraits::ip_addr;

            inline static std::string n_to_p(const ip_addr &addr)
            {
                char buff[MyTraits::max_string_len + 1];

                auto result = InetNtopA(
                    static_cast<int>(Family),
                    reinterpret_cast<PVOID>(const_cast<ip_addr *>(&addr)),
                    buff,
                    sizeof(buff));
                garbage::wsa_throw_if_null(result);

                return buff;
            }
        };

        template <address_family Family>
        struct socket_string_traits<wchar_t, Family>
        {
            using MyTraits = address_family_traits<Family>;
            using ip_addr = typename MyTraits::ip_addr;

            inline static std::wstring n_to_p(const ip_addr &addr)
            {
                wchar_t buff[MyTraits::max_string_len + 1];

                auto result = InetNtopW(
                    static_cast<int>(Family),
                    reinterpret_cast<PVOID>(const_cast<ip_addr *>(&addr)),
                    buff,
                    sizeof(buff) / 2);
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
        using sock_addr = typename MyTraits::sock_addr;
        static const address_family family = Family;



        /*
         * Constructor(s)/Destructor
         */
        ip_address(void) :
            _addr{}
        {
        }

        ip_address(const typename MyTraits::create_type &val) :
            _addr(MyTraits::create(val))
        {
        }

        ip_address(const ip_addr &ip) :
            _addr(ip)
        {
        }

        ip_address(const sock_addr &addr) :
            _addr(MyTraits::create(addr))
        {
        }

        template <typename CharType>
        ip_address(const CharType *ip)
        {
            this->Set(ip);
        }

        template <typename CharType>
        ip_address(const std::basic_string<CharType> &ip)
        {
            this->Set(ip.c_str());
        }



        /*
         * Assignment Operators
         */
        ip_address &operator=(const typename MyTraits::create_type &val)
        {
            this->_addr = MyTraits::create(val);
            return *this;
        }

        ip_address &operator=(const ip_addr &ip)
        {
            this->_addr = ip;
            return *this;
        }

        ip_address &operator=(const typename MyTraits::sock_addr &addr)
        {
            this->_addr = MyTraits::create(addr);
            return *this;
        }

        template <typename CharType>
        ip_address &operator=(const CharType *ip)
        {
            this->Set(ip);
            return *this;
        }

        template <typename CharType>
        ip_address &operator=(const std::basic_string<CharType> &ip)
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
            return garbage::socket_string_traits<CharType, Family>::n_to_p(this->_addr);
        }



    private:

        void Set(const char *ip)
        {
            auto result = garbage::wsa_throw_if_error(InetPtonA(static_cast<int>(Family), ip, &this->_addr));
            if (result == 0)
            {
                // Result of an invalid string. WSAGetLastError will give us zero, so provide our own error code
                throw socket_exception(WSA_INVALID_PARAMETER);
            }
        }

        void Set(const wchar_t *ip)
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
#pragma region socket_address

    class socket_address final
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        socket_address(void) :
            _ipv6Addr{}         // Zero initialize the largest struct in the union
        {
        }

        socket_address(const ipv4_address &addr, uint16_t port)
        {
            this->assign(addr, port);
        }

        socket_address(const ipv6_address &addr, uint16_t port, uint32_t flowInfo, uint32_t scopeId)
        {
            this->assign(addr, port, flowInfo, scopeId);
        }



        /*
         * Operators
         */
        operator sockaddr *(void)
        {
            return &this->_addr;
        }

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

        bool operator==(const socket_address &other) const
        {
            if (this->_size != other._size)
            {
                return false;
            }

            return std::memcmp(&this->_addr, &other._addr, this->_size) == 0;
        }

        bool operator!=(const socket_address &other) const
        {
            return !(*this == other);
        }



        /*
         * Public Functions
         */
        void assign(const ipv4_address &addr, uint16_t port)
        {
            this->_ipv4Addr =
            {
                static_cast<int>(ipv4_address::family),     // sin_family
                htons(port),                                // sin_port
                addr                                        // sin_addr
            };

            this->_size = sizeof(this->_ipv4Addr);
        }

        void assign(const ipv6_address &addr, uint16_t port, uint32_t flowInfo, uint32_t scopeId)
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

        size_t reset_size(void)
        {
            switch (static_cast<address_family>(this->_ipv4Addr.sin_family))
            {
            case address_family::internetwork_version_4:
                this->_size = sizeof(this->_ipv4Addr);
                break;

            case address_family::internetwork_version_6:
                this->_size = sizeof(this->_ipv6Addr);
                break;

            default:
                // Unknown
                this->_size = 0;
                break;
            }

            return this->_size;
        }

        address_family family(void) const
        {
            return static_cast<address_family>(this->_addr.sa_family);
        }

        ipv4_address as_ipv4(void) const
        {
            // Throws if sin_famliy does not equal AF_INET
            return this->_ipv4Addr;
        }

        ipv6_address as_ipv6(void) const
        {
            // Throws if sin6_family does not equal AF_INET6
            return this->_ipv6Addr;
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

#pragma endregion



    /*
     * socket_base
     */
#pragma region socket_base

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

            ~socket_initializer(void) noexcept(false)
            {
                wsa_throw_if_error(WSACleanup());
            }
        };

    }


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
            // Under the C++ standard, the construction of static function variables is not guaranteed to be thread
            // safe, so the constructor technically can get called more than once here. In the unlikely event that
            // this does happen, we'll just end up calling WSAStartup more than once while only making one call to
            // WSACleanup. Since the system will re-claim our resources, we just ignore the possibility
            static const garbage::socket_initializer _init;
        }

        socket_base(address_family family, socket_type type, ip_protocol protocol) :
            socket_base()
        {
            this->open(family, type, protocol);
        }

        socket_base(socket_t sock) :
            socket_base()
        {
            this->_socket = sock;
        }

        socket_base(socket_base &&other) :
            socket_base()
        {
            this->swap(other);
        }

        ~socket_base(void) noexcept(false)
        {
            this->Destroy();
        }

        // Cannot copy
        socket_base(const socket_base &other) = delete;
        socket_base &operator=(const socket_base &other) = delete;



        /*
         * Operators
         */
        socket_base &operator=(socket_base &&other)
        {
            this->swap(other);
            return *this;
        }

        socket_base &operator=(socket_t sock)
        {
            this->Destroy(); // Throws if the socket is still open. This is desired
            this->_socket = sock;

            return *this;
        }

        operator socket_t(void) const
        {
            return this->_socket;
        }

        operator bool(void) const
        {
            return this->_socket != invalid_socket;
        }



        /*
         * Socket functions
         */
        socket_base accept(socket_address &addr)
        {
            // We assume the size of sockaddr_in6 since that's the largest member of socket_address
            int size = sizeof(sockaddr_in6);

            auto result = ::accept(this->_socket, addr, &size);
            if (result == invalid_socket)
            {
                garbage::wsa_throw_last_error();
            }

            addr.reset_size();
            return result;
        }

        void bind(const socket_address &addr)
        {
            this->InvokeThrowOnError(::bind, this->_socket, addr, addr.size());
        }

        void close(void)
        {
            this->InvokeThrowOnError(::closesocket, this->_socket);
            this->_socket = invalid_socket;
        }

        void connect(const socket_address &addr)
        {
            this->InvokeThrowOnError(::connect, this->_socket, addr, addr.size());
        }

        socket_address get_peer_name(void)
        {
            // We assume the size of sockaddr_in6 since that's the largest member of socket_address
            socket_address result;
            int size = sizeof(sockaddr_in6);

            this->InvokeThrowOnError(::getpeername, this->_socket, result, &size);

            // Reset the socket_address size and assert that its size is the expected size
            result.reset_size();
            assert(result.size() == static_cast<size_t>(size));

            return result;
        }

        socket_address get_socket_name(void)
        {
            // We assume the size of sockaddr_in6 since that's the largest member of socket_address
            socket_address result;
            int size = sizeof(sockaddr_in6);

            this->InvokeThrowOnError(::getsockname, this->_socket, result, &size);

            // Reset the socket_address size and assert that its size is the expected size
            result.reset_size();
            assert(result.size() == static_cast<size_t>(size));

            return result;
        }

        template <typename Ty>
        Ty get_socket_option(socket_level level, socket_option opt)
        {
            Ty value;
            int len = static_cast<int>(sizeof(value));

            this->InvokeThrowOnError(
                ::getsockopt,
                this->_socket,
                static_cast<int>(level),
                static_cast<int>(opt),
                reinterpret_cast<char *>(&value),
                &len);

            return value;
        }

        unsigned long io_control(io_control_command cmd, unsigned long value = 0)
        {
            this->InvokeThrowOnError(::ioctlsocket, this->_socket, static_cast<long>(cmd), &value);
            return value;
        }

        void listen(int backlog)
        {
            this->InvokeThrowOnError(::listen, this->_socket, backlog);
        }

        void open(address_family family, socket_type type, ip_protocol protocol)
        {
            garbage::wsa_throw_if_false(this->_socket == invalid_socket, WSAEISCONN);

            this->_socket = socket(static_cast<int>(family), static_cast<int>(type), static_cast<int>(protocol));
            if (this->_socket == invalid_socket)
            {
                garbage::wsa_throw_last_error();
            }
        }

        socket_error_t receive(void *buffer, int length, message_flags flags = message_flags::none)
        {
            auto buff = static_cast<char *>(buffer);
            return this->InvokeThrowOnError(::recv, this->_socket, buff, length, static_cast<int>(flags));
        }

        template <typename Itr>
        Itr receive(Itr front, Itr back, message_flags flags = message_flags::none)
        {
            // We cannot guarantee that the collection [front, back) exists in contiguous memory locations, even if
            // they are random access iterators. Hence, we use std::vector for the call to ::recv
            using value_type = typename std::iterator_traits<Itr>::value_type;
            std::vector<value_type> buffer(std::distance(front, back));

            auto len = this->receive(static_cast<void *>(buffer.data()), buffer.size() * sizeof(value_type), flags);
            assert(len % sizeof(value_type) == 0);
            len /= sizeof(value_type);

            auto itr = front;
            for (int i = 0; i < len; ++i)
            {
                *itr = buffer[i];
                ++itr;
            }

            return itr;
        }

        template <typename Ty, size_t len>
        socket_error_t receive(Ty (&buffer)[len], message_flags flags = message_flags::none)
        {
            return this->receive(static_cast<void *>(buffer), len * sizeof(Ty), flags);
        }

        socket_error_t receive_from(void *buffer, int length, message_flags flags, socket_address &addr)
        {
            // We assume the size of sockaddr_in6 since that's the largest member of socket_address
            int size = sizeof(sockaddr_in6);

            auto result = this->InvokeThrowOnError(
                ::recvfrom,
                this->_socket,
                static_cast<char *>(buffer),
                length,
                static_cast<int>(flags),
                addr,
                &size);

            // Reset the socket_address size and assert that its size is the expected size
            addr.reset_size();
            assert(addr.size() == static_cast<size_t>(size));

            return result;
        }

        template <typename Itr>
        Itr receive_from(Itr front, Itr back, message_flags flags, socket_address &addr)
        {
            // We cannot guarantee that the collection [front, back) exists in contiguous memory locations, even if
            // they are random access iterators. Hence, we use std::vector for the call to ::recvfrom
            using value_type = typename std::iterator_traits<Itr>::value_type;
            std::vector<value_type> buffer(std::distance(front, back));

            auto len = this->receive_from(
                static_cast<void *>(buffer.data()),
                buffer.size() * sizeof(value_type),
                flags,
                addr);

            auto itr = front;
            for (int i = 0; i < len; ++i)
            {
                *itr = buffer[i];
                ++itr;
            }

            return itr;
        }

        template <typename Ty, size_t len>
        socket_error_t receive_from(Ty (&buffer)[len], message_flags flags, socket_address &addr)
        {
            return this->receive_from(static_cast<void *>(buffer), len * sizeof(Ty), flags, addr);
        }

        socket_error_t send(const void *buffer, size_t length, message_flags flags = message_flags::none)
        {
            auto buff = static_cast<const char *>(buffer);
            return this->InvokeThrowOnError(::send, this->_socket, buff, length, static_cast<int>(flags));
        }

        template <typename Itr>
        socket_error_t send(Itr front, Itr back, message_flags flags = message_flags::none)
        {
            // We cannot guarantee that the collection [front, back) exists in contiguous memory locations, even if
            // they are random access iterators. Hence, we transfer the contents to std::vector
            using value_type = typename std::iterator_traits<Itr>::value_type;
            std::vector<value_type> buffer(front, back);

            return this->send(static_cast<const void *>(&buffer[0]), buffer.size() * sizeof(value_type), flags);
        }

        template <typename Ty, size_t len>
        socket_error_t send(const Ty (&buffer)[len], message_flags flags = message_flags::none)
        {
            return this->send(static_cast<const void *>(buffer), len * sizeof(Ty), flags);
        }

        socket_error_t send_to(const void *buffer, size_t length, message_flags flags, const socket_address &addr)
        {
            return this->InvokeThrowOnError(
                ::sendto,
                this->_socket,
                static_cast<const char *>(buffer),
                length,
                static_cast<int>(flags),
                addr,
                addr.size());
        }

        template <typename Itr>
        socket_error_t send_to(Itr front, Itr back, message_flags flags, const socket_address &addr)
        {
            // We cannot guarantee that the collection [front, back) exists in contiguous memory locations, even if
            // they are random access iterators. Hence, we transfer the contents to std::vector
            using value_type = typename std::iterator_traits<Itr>::value_type;
            std::vector<value_type> buffer(front, back);

            return this->send_to(
                static_cast<const void *>(&buffer[0]),
                buffer.size() * sizeof(value_type),
                flags,
                addr);
        }

        template <typename Ty, size_t len>
        socket_error_t send_to(const Ty(&buffer)[len], message_flags flags, const socket_address &addr)
        {
            return this->send_to(static_cast<const void *>(buffer), len * sizeof(Ty), flags, addr);
        }

        template <typename Ty>
        void set_socket_option(socket_level level, socket_option opt, const Ty &val)
        {
            this->InvokeThrowOnError(
                ::setsockopt,
                this->_socket,
                static_cast<int>(level),
                static_cast<int>(opt),
                reinterpret_cast<const char *>(&val),
                sizeof(val));
        }

        void shutdown(shutdown_options options = shutdown_options::both)
        {
            this->InvokeThrowOnError(::shutdown, this->_socket, static_cast<int>(options));
        }



        /*
         * Other functions
         */
        void swap(socket_base &other)
        {
            auto temp = this->_socket;
            this->_socket = other._socket;
            other._socket = temp;
        }

        socket_t detach(void)
        {
            auto value = this->_socket;
            this->_socket = invalid_socket;
            return value;
        }



    protected:

        // Invokes a function and closes the socket (if it's open) on failure. The destructor would otherwise take
        // care of closing the socket for us, but it would also throw another exception, which we don't want
        template <typename Func, typename... Args>
        inline socket_error_t InvokeThrowOnError(const Func &func, Args &&...args)
        {
            auto result = func(std::forward<Args>(args)...);
            if (result == socket_error)
            {
                garbage::wsa_throw_last_error();
            }

            return result;
        }

        void Destroy(void)
        {
            // If the socket isn't already closed, we throw an exception (similar to std::thread if join or detach
            // has not been called). However, in order to leave the program in a good state, we go ahead and close
            // the socket for the user. Note that this function is called by the destructor, so we cannot rely on
            // the destructor doing the cleanup in such situations.
            if (this->_socket != invalid_socket)
            {
                this->close();
                throw socket_exception(WSAEFAULT);
            }
        }

        socket_t _socket;
    };

#pragma endregion



    /*
     * udp_socket
     */
#pragma region udp_socket

    template <typename Ty = char>
    struct udp_packet final
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        udp_packet(size_t capacity)
        {
            this->reset(capacity);
        }

        // Cannot copy
        udp_packet(const udp_packet &) = delete;
        udp_packet &operator=(const udp_packet &) = delete;

        // Default move
        udp_packet(udp_packet &&) = default;
        udp_packet &operator=(udp_packet &&) = default;



        /*
         * Public Functions
         */
        void reset(size_t capacity)
        {
            this->_dataLength = 0;
            this->_bufferLength = capacity;
            this->_buffer.reset(new Ty[this->_bufferLength]);
        }

        size_t capacity(void) const
        {
            return this->_bufferLength;
        }

        size_t size(void) const
        {
            return this->_dataLength;
        }

        const std::unique_ptr<Ty[]> &buffer(void) const
        {
            return this->_buffer;
        }

        const socket_address &addr(void) const
        {
            return this->_addr;
        }

        void set_addr(const socket_address &addr)
        {
            this->_addr = addr;
        }

        void set_data(Ty *buffer, size_t size)
        {
            this->set_data(buffer, buffer + size);
        }

        template <typename Itr>
        void set_data(Itr front, Itr back)
        {
            this->_dataLength = 0; // Expect the worst
            size_t size = std::distance(front, back);
            if (size > this->_bufferLength)
            {
                throw std::out_of_range("Cannot assign data to udp_socket with a length longer than the buffer");
            }

            for (size_t i = 0; front != back; ++front, ++i)
            {
                this->_buffer[i] = *front;
            }

            this->_dataLength = size;
        }

        void swap(udp_packet &other)
        {
            // Swap the buffers
            this->_buffer.swap(other._buffer);

            // Swap the remaining data
            std::swap(this->_bufferLength, other._bufferLength);
            std::swap(this->_dataLength, other._dataLength);
            std::swap(this->_addr, other._addr);
        }



    private:

        std::unique_ptr<Ty[]> _buffer;
        size_t _bufferLength;
        size_t _dataLength;
        socket_address _addr;

        friend class udp_socket;
    };

    class udp_socket
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        udp_socket() :
            _baseSocket(
                address_family::internetwork_version_4,
                socket_type::datagram,
                ip_protocol::user_datagram_protocol)
        {
        }

        udp_socket(socket_t sock) :
            _baseSocket(sock)
        {
        }

        udp_socket(udp_socket &&other) :
            _baseSocket(std::move(other._baseSocket))
        {
        }

        // Cannot copy
        udp_socket(const udp_socket &) = delete;
        udp_socket &operator=(const udp_socket &) = delete;



        /*
         * Operators
         */
        udp_socket &operator=(udp_socket &&other)
        {
            this->swap(other);
            return *this;
        }



        /*
         * Socket Functions
         */
        void bind(const socket_address &addr)
        {
            this->_baseSocket.bind(addr);
        }

        void close(void)
        {
            this->_baseSocket.close();
        }

        // TODO: get_peer_name/get_socket_name??

        template <typename Ty>
        Ty get_socket_option(socket_level level, socket_option opt)
        {
            return this->_baseSocket.get_socket_name<Ty>(level, opt);
        }

        unsigned long io_control(io_control_command cmd, unsigned long value = 0)
        {
            return this->_baseSocket.io_control(cmd, value);
        }

        socket_error_t receive(void *buffer, int length, message_flags flags, socket_address &addr)
        {
            return this->_baseSocket.receive_from(buffer, length, flags, addr);
        }

        template <typename Itr>
        Itr receive(Itr front, Itr back, message_flags flags, socket_address &addr)
        {
            return this->_baseSocket.receive_from<Itr>(front, back, flags, addr);
        }

        template <typename Ty, size_t len>
        socket_error_t receive(Ty(&buffer)[len], message_flags flags, socket_address &addr)
        {
            return this->_baseSocket.receive_from<Ty, len>(buffer, flags, addr);
        }

        template <typename Ty>
        void receive(udp_packet<Ty> &packet, message_flags flags = message_flags::none)
        {
            auto length = this->_baseSocket.receive_from(
                reinterpret_cast<void *>(packet._buffer.get()),
                packet._bufferLength * sizeof(Ty),
                flags,
                packet._addr);

            assert(length % sizeof(Ty) == 0);
            packet._dataLength = length / sizeof(Ty);
        }

        socket_error_t send(const void *buffer, size_t length, message_flags flags, const socket_address &addr)
        {
            return this->_baseSocket.send_to(buffer, length, flags, addr);
        }

        template <typename Itr>
        socket_error_t send(Itr front, Itr back, message_flags flags, const socket_address &addr)
        {
            return this->_baseSocket.send_to<Itr>(front, back, flags, addr);
        }

        template <typename Ty, size_t len>
        socket_error_t send(const Ty(&buffer)[len], message_flags flags, const socket_address &addr)
        {
            return this->_baseSocket.send_to<Ty, len>(buffer, flags, addr);
        }

        template <typename Ty>
        socket_error_t send(const udp_packet<Ty> &packet, message_flags flags = message_flags::none)
        {
            // TODO: Fail if we don't send all bytes? The design is that most callers shouldn't need to check this...
            return this->_baseSocket.send_to(
                reinterpret_cast<void *>(packet._buffer.get()),
                packet._dataLength,
                flags,
                packet._addr);
        }

        template <typename Ty>
        void set_socket_option(socket_level level, socket_option opt, const Ty &val)
        {
            this->_baseSocket.set_socket_option<Ty>(level, opt, val);
        }



        /*
         * Other Functions
         */
        void swap(udp_socket &other)
        {
            this->_baseSocket.swap(other._baseSocket);
        }



    private:

        socket_base _baseSocket;
    };

#pragma endregion



    /*
     * tcp_socket
     */
#pragma region tcp_socket

    class tcp_socket final
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        tcp_socket(void) :
            _baseSocket(
                address_family::internetwork_version_4,
                socket_type::stream,
                ip_protocol::transmission_control_protocol)
        {
        }

        tcp_socket(socket_t sock) :
            _baseSocket(sock)
        {
        }

        tcp_socket(tcp_socket &&other) :
            _baseSocket(std::move(other._baseSocket))
        {
        }

        // Cannot copy
        tcp_socket(const tcp_socket &other) = delete;
        tcp_socket&operator=(const tcp_socket &other) = delete;



        /*
         * Socket Functions
         */
        void bind(const socket_address &addr)
        {
            // Needed...?
            this->_baseSocket.bind(addr);
        }

        void close(void)
        {
            this->_baseSocket.close();
        }

        void connect(const socket_address &addr)
        {
            this->_baseSocket.connect(addr);
        }

        socket_address get_peer_name(void)
        {
            return this->_baseSocket.get_peer_name();
        }

        socket_address get_socket_name(void)
        {
            return this->_baseSocket.get_socket_name();
        }

        template <typename Ty>
        Ty get_socket_option(socket_level level, socket_option opt)
        {
            return this->_baseSocket.get_socket_option<Ty>(level, opt);
        }

        unsigned long io_control(io_control_command cmd, unsigned long value = 0)
        {
            return this->_baseSocket.io_control(cmd, value);
        }

        socket_error_t receive(void *buffer, int length, message_flags flags = message_flags::none)
        {
            return this->_baseSocket.receive(buffer, length, flags);
        }

        template <typename Itr>
        Itr receive(Itr front, Itr back, message_flags flags = message_flags::none)
        {
            return this->_baseSocket.receive<Itr>(front, back, flags);
        }

        template <typename Ty, size_t len>
        socket_error_t receive(Ty (&buffer)[len], message_flags flags = message_flags::none)
        {
            return this->_baseSocket.receive<Ty, len>(buffer, flags);
        }

        socket_error_t send(const void *buffer, size_t length, message_flags flags = message_flags::none)
        {
            return this->_baseSocket.send(buffer, length, flags);
        }

        template <typename Itr>
        socket_error_t send(Itr front, Itr back, message_flags flags = message_flags::none)
        {
            return this->_baseSocket.send<Itr>(front, back, flags);
        }

        template <typename Ty, size_t len>
        socket_error_t send(const Ty (&buffer)[len], message_flags flags = message_flags::none)
        {
            return this->_baseSocket.send<Ty, len>(buffer, flags);
        }

        template <typename Ty>
        void set_socket_option(socket_level level, socket_option opt, const Ty &val)
        {
            this->_baseSocket.set_socket_option<Ty>(level, opt, val);
        }

        void shutdown(shutdown_options options = shutdown_options::both)
        {
            this->_baseSocket.shutdown(options);
        }



        /*
         * Other Functions
         */
        void swap(tcp_socket &other)
        {
            this->_baseSocket.swap(other._baseSocket);
        }



    private:

        socket_base _baseSocket;
    };

#pragma endregion



    /*
     * server_socket
     */
#pragma region server_socket

    class server_socket final
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        server_socket(void) :
            _baseSocket(
            address_family::internetwork_version_4,
            socket_type::stream,
            ip_protocol::transmission_control_protocol)
        {
        }

        server_socket(socket_t sock) :
            _baseSocket(sock)
        {
        }

        server_socket(server_socket &&other) :
            _baseSocket(std::move(other._baseSocket))
        {
        }

        // Cannot copy
        server_socket(const server_socket &other) = delete;
        server_socket &operator=(const server_socket &other) = delete;



        /*
         * Operators
         */
        server_socket &operator=(server_socket &&other)
        {
            this->swap(other);
            return *this;
        }



        /*
         * Socket Functions
         */
        tcp_socket accept(socket_address &addr)
        {
            auto socket = this->_baseSocket.accept(addr);
            return tcp_socket(socket.detach()); // "Promote" as tcp_socket
        }

        void bind(const socket_address &addr)
        {
            this->_baseSocket.bind(addr);
        }

        void close(void)
        {
            this->_baseSocket.close();
        }

        // TODO: get_peer_name/get_socket_name

        template <typename Ty>
        Ty get_socket_option(socket_level level, socket_option opt)
        {
            // TODO: needed...?
            return this->_baseSocket.get_socket_option<Ty>(level, opt);
        }

        unsigned long io_control(io_control_command cmd, unsigned long value = 0)
        {
            // TODO: needed...?
            return this->_baseSocket.io_control(cmd, value);
        }

        void listen(int backlog)
        {
            this->_baseSocket.listen(backlog);
        }

        template <typename Ty>
        void set_socket_option(socket_level level, socket_option opt, const Ty &val)
        {
            // TODO: needed...?
            this->_baseSocket.set_socket_option<Ty>(level, opt, val);
        }



        /*
         * Other Functions
         */
        void swap(server_socket &other)
        {
            this->_baseSocket.swap(other._baseSocket);
        }



    private:

        socket_base _baseSocket;
    };

#pragma endregion



    /*
     * Other Socket Functions
     */
#pragma region Other Functions

    namespace garbage
    {
        inline void create_fd_set(const std::set<socket_t> &sockets, fd_set &fdSet)
        {
            FD_ZERO(&fdSet);

#ifdef WIN32
            wsa_throw_if_false(sockets.size() <= FD_SETSIZE, WSAEINVAL);

            size_t i = 0;
            fdSet.fd_count = sockets.size();
#else
            static_assert(false, "Don't know how to get the size of fd_set with your compiler");
#endif

            for (auto sock : sockets)
            {
#ifdef WIN32
                // For WinSock, we know that fd_set is simply just an array of sockets. Thus, we can optimize this
                // operation since there are no duplicates in std::set
                fdSet.fd_array[i++] = sock;
#else
                FD_SET(sock, &fdSet);
#endif
            }
        }
    }

#pragma endregion
}



#ifndef _DHORN_NO_STD

namespace std
{
    // Overload of std::swap for the socket types
    inline void swap(dhorn::socket_base &lhs, dhorn::socket_base &rhs)
    {
        lhs.swap(rhs);
    }

    template <typename Ty>
    inline void swap(dhorn::udp_packet<Ty> &lhs, dhorn::udp_packet<Ty> &rhs)
    {
        lhs.swap(rhs);
    }

    inline void swap(dhorn::udp_socket &lhs, dhorn::udp_socket &rhs)
    {
        lhs.swap(rhs);
    }

    inline void swap(dhorn::tcp_socket &lhs, dhorn::tcp_socket &rhs)
    {
        lhs.swap(rhs);
    }

    inline void swap(dhorn::server_socket &lhs, dhorn::server_socket &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif
