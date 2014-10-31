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
     * Socket Types
     */
    using socket_error_t = int;

    enum class address_family : int
    {
        inet    = AF_INET,
        inet6   = AF_INET6
    };



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
    }

#pragma endregion



    /*
     * Sockets Traits Types
     */
    namespace garbage
    {
        /*
         * address_family_traits
         */
        template <address_family Family>
        struct address_family_traits;

        template <>
        struct address_family_traits<address_family::inet>
        {
            using ip_addr = in_addr;
            static const address_family family = address_family::inet;
            static const int max_string_len = 16;
        };

        template <>
        struct address_family_traits<address_family::inet6>
        {
            using ip_addr = in6_addr;
            static const address_family family = address_family::inet6;
            static const int max_string_len = 46;
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
            auto result = InetPtonA(static_cast<int>(Family), ip, &this->_addr);
            if (result == 0)
            {
                // Result of an invalid pointer. WSAGetLastError will give us zero, so provide our own error code
                throw socket_exception(WSA_INVALID_PARAMETER);
            }
            else if (result == -1)
            {
                garbage::wsa_throw_last_error();
            }
        }

        void Set(_In_ const wchar_t *ip)
        {
            auto result = InetPtonW(static_cast<int>(Family), ip, &this->_addr);
            if (result == 0)
            {
                // Result of an invalid pointer. WSAGetLastError will give us zero, so provide our own error code
                throw socket_exception(WSA_INVALID_PARAMETER);
            }
            else if (result == -1)
            {
                garbage::wsa_throw_last_error();
            }
        }



        ip_addr _addr;
    };

    using ipv4_address = ip_address<address_family::inet>;
    using ipv6_address = ip_address<address_family::inet6>;

#pragma endregion



    /*
     *
     */
#pragma region



#pragma endregion
}
