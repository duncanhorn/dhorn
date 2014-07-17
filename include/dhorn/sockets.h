/*
 * sockets.h
 *
 * Duncan Horn
 *
 * 
 */
#pragma once

#include <cassert>
#include <iostream>
#include <string>

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
     * Socket Types
     */
    using socket_error_t = int;

    enum address_family
    {
        af_inet     = AF_INET,
        af_inet6    = AF_INET6
    };



    /*
     * Sockets Traits Types
     */
    namespace garbage
    {
        /*
         * address_family_traits
         */
        template <address_family _Af>
        struct address_family_traits {};

        template <>
        struct address_family_traits<af_inet>
        {
            using ip_addr = in_addr;
            static const int max_string_len = 16;
        };

        template <>
        struct address_family_traits<af_inet6>
        {
            using ip_addr = in6_addr;
            static const int max_string_len = 46;
        };
    }



    /*
     * dhorn::ip_address
     */
#pragma region ip_address

    template <address_family _Af>
    class ip_address
    {
    public:

        /*
         * Type/Value Definitions
         */
        using ip_addr = typename garbage::address_family_traits<_Af>::ip_addr;
        static const address_family family = _Af;



        /*
         * Constructor(s)/Destructor
         */

        ip_address(void) :
            _addr{},
            _error(0),
            _fail(false)
        {
        }

        ip_address(_In_ const ip_addr &ip) :
            _addr(ip),
            _error(0),
            _fail(false)
        {
        }

        template <typename _CharT>
        ip_address(_In_ const _CharT *ip) :
            _error(0),
            _fail(false)
        {
            this->_Set(ip);
        }

        template <typename _CharT>
        ip_address(_In_ const std::basic_string<_CharT> &ip) :
            _error(0),
            _fail(false)
        {
            this->_Set(ip.c_str());
        }



        /*
         * Assignment Operators
         */

        ip_address &operator=(_In_ const ip_addr &ip)
        {
            this->_addr = ip;
            return *this;
        }

        template <typename _CharT>
        ip_address &operator=(_In_ const _CharT *ip)
        {
            this->_Set(ip);
            return *this;
        }

        template <typename _CharT>
        ip_address &operator=(_In_ const std::basic_string<_CharT> &ip)
        {
            this->_Set(ip.c_str());
            return *this;
        }



        /*
         * Type Conversion Operators
         */

        operator const ip_addr &(void) const
        {
            return this->_addr;
        }

        template <typename _CharT>
        operator std::basic_string<_CharT>(void) const
        {
            return this->str<_CharT>();
        }



        /*
         * Public Functions
         */

        const ip_addr &addr(void) const
        {
            return this->_addr;
        }

        template <typename _CharT = char>
        std::basic_string<_CharT> str(void) const
        {
            return string_converter<_CharT>::to_string(this->_addr);
        }

        bool fail(void) const
        {
            return this->_fail;
        }

        socket_error_t error(void) const
        {
            return this->_error;
        }



    private:

        void _Set(_In_ const char *ip)
        {
            auto result = InetPtonA(_Af, ip, &this->_addr);
            if (result != 1)
            {
                this->_error = WSAGetLastError();
                this->_fail = true;
            }
        }

        void _Set(_In_ const wchar_t *ip)
        {
            auto result = InetPtonW(_Af, ip, &this->_addr);
            if (result != 1)
            {
                this->_error = WSAGetLastError();
                this->_fail = true;
            }
        }



        /*
         * ip_addr to string conversion
         */
        template <typename _CharT>
        struct string_converter {};

        template <>
        struct string_converter<char>
        {
            static std::string to_string(_In_ const ip_addr &addr)
            {
                char buff[garbage::address_family_traits<_Af>::max_string_len + 1];

                auto result = InetNtopA(_Af, (PVOID)&addr, buff, sizeof(buff));
                assert(result);

                return buff;
            }
        };

        template <>
        struct string_converter<wchar_t>
        {
            static std::wstring to_string(_In_ const ip_addr &addr)
            {
                wchar_t buff[garbage::address_family_traits<_Af>::max_string_len + 1];

                auto result = InetNtopW(_Af, (PVOID)&addr, buff, sizeof(buff) / 2);
                assert(result);

                return buff;
            }
        };



        ip_addr        _addr;
        socket_error_t _error;
        bool           _fail;
    };

    using ipv4_address = ip_address<af_inet>;
    using ipv6_address = ip_address<af_inet6>;

#pragma endregion



    /*
     * dhorn::udp_packet
     */
    template <typename _CharT>
    class basic_udp_packet
    {
    public:

    };

    using upd_packet = basic_udp_packet<char>;
    using wudp_packet = basic_udp_packet<wchar_t>;

}
