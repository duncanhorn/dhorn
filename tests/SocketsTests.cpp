/*
 * Duncan Horn
 *
 * SocketsTest.cpp
 *
 * Tests for the dhorn sockets classes, structures, functions, etc.
 */
#include "stdafx.h"

#include <string>
#include <thread>
#include <vector>

#include <dhorn/experimental/sockets.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_COUNT      100

#pragma warning(push)
#pragma warning(disable:6387) // We wish to test parameters with nullptr

bool operator==(const in6_addr &addr1, const in6_addr &addr2)
{
    for (int i = 0; i < 8; i++)
    {
        if (addr1.u.Word[i] != addr2.u.Word[i])
        {
            return false;
        }
    }

    return true;
}

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(Ipv4AddressTests)
        {
        public:

            /*
             * Constructor tests. Note that most consume the operator in_addr function
             */
#pragma region Constructor Tests

            TEST_METHOD(DefaultConstructorTest)
            {
                dhorn::experimental::ipv4_address addr;
                Assert::AreEqual(((in_addr)addr).s_addr, 0ul);
            }

            TEST_METHOD(CreateTypeConstructorTest)
            {
                // Simple test: value is zero
                dhorn::experimental::ipv4_address ip(0);
                Assert::AreEqual(((in_addr)ip).s_addr, 0ul);

                // More complicated case: must reverse byte order
                dhorn::experimental::ipv4_address ip2(0x00c0ffee);
                Assert::AreEqual(((in_addr)ip2).s_addr, 0xeeffc000ul);
            }

            TEST_METHOD(IpAddrConstructorTest)
            {
                in_addr addr;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    addr.s_net = rand() % 256;
                    addr.s_host = rand() % 256;
                    addr.s_lh = rand() % 256;
                    addr.s_impno = rand() % 256;

                    dhorn::experimental::ipv4_address ip(addr);
                    Assert::AreEqual(((in_addr)ip).s_addr, addr.s_addr);
                }

                addr.s_addr = 0xFFFFFFFF;
                dhorn::experimental::ipv4_address ip1(addr);
                Assert::AreEqual(((in_addr)ip1).s_addr, addr.s_addr);

                addr.s_addr = 0;
                dhorn::experimental::ipv4_address ip2(addr);
                Assert::AreEqual(((in_addr)ip2).s_addr, addr.s_addr);
            }

            TEST_METHOD(SockAddrConstructorTest)
            {
                // Can successfully construct with valid sockaddr_in
                sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_addr.s_addr = htonl(0x7F000001ul);

                dhorn::experimental::ipv4_address ip(addr);
                Assert::IsTrue(((in_addr)ip).s_addr == 0x0100007Ful);

                // Cannot construct if family is AF_INET6
                try
                {
                    addr.sin_family = AF_INET6;
                    dhorn::experimental::ipv4_address ip2(addr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &)
                {
                }
            }

            TEST_METHOD(StringConstructorTest)
            {
                std::string ip1 = "0.0.0.0";
                dhorn::experimental::ipv4_address addr1(ip1.c_str());
                Assert::AreEqual(((in_addr)addr1).s_addr, 0ul);

                std::string ip2 = "255.255.255.255";
                dhorn::experimental::ipv4_address addr2(ip2.c_str());
                Assert::AreEqual(((in_addr)addr2).s_addr, 0xFFFFFFFFul);

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse" (network-byte-order)
                std::string ip3 = "127.0.0.1";
                dhorn::experimental::ipv4_address addr3(ip3.c_str());
                Assert::AreEqual(((in_addr)addr3).s_addr, 0x0100007Ful);

                std::string ip4 = "2.0.0.10";
                dhorn::experimental::ipv4_address addr4(ip4.c_str());
                Assert::AreEqual(((in_addr)addr4).s_addr, 0x0A000002ul);

                try
                {
                    std::string ip5 = "foo.bar";
                    dhorn::experimental::ipv4_address addr5(ip5.c_str());
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    dhorn::experimental::ipv4_address addr6((char *)nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSAEFAULT);
                }
            }

            TEST_METHOD(WStringConstructorTest)
            {
                std::wstring ip1 = L"0.0.0.0";
                dhorn::experimental::ipv4_address addr1(ip1.c_str());
                Assert::AreEqual(((in_addr)addr1).s_addr, 0ul);

                std::wstring ip2 = L"255.255.255.255";
                dhorn::experimental::ipv4_address addr2(ip2.c_str());
                Assert::AreEqual(((in_addr)addr2).s_addr, 0xFFFFFFFFul);

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"127.0.0.1";
                dhorn::experimental::ipv4_address addr3(ip3.c_str());
                Assert::AreEqual(((in_addr)addr3).s_addr, 0x0100007Ful);

                std::wstring ip4 = L"2.0.0.10";
                dhorn::experimental::ipv4_address addr4(ip4.c_str());
                Assert::AreEqual(((in_addr)addr4).s_addr, 0x0A000002ul);

                try
                {
                    std::wstring ip5 = L"foo.bar";
                    dhorn::experimental::ipv4_address addr5(ip5.c_str());
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    dhorn::experimental::ipv4_address addr6((wchar_t *)nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSAEFAULT);
                }
            }

            TEST_METHOD(StdStringConstructorTest)
            {
                std::string ip1 = "0.0.0.0";
                dhorn::experimental::ipv4_address addr1(ip1);
                Assert::AreEqual(((in_addr)addr1).s_addr, 0ul);

                std::string ip2 = "255.255.255.255";
                dhorn::experimental::ipv4_address addr2(ip2);
                Assert::AreEqual(((in_addr)addr2).s_addr, 0xFFFFFFFFul);

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "127.0.0.1";
                dhorn::experimental::ipv4_address addr3(ip3);
                Assert::AreEqual(((in_addr)addr3).s_addr, 0x0100007Ful);

                std::string ip4 = "2.0.0.10";
                dhorn::experimental::ipv4_address addr4(ip4);
                Assert::AreEqual(((in_addr)addr4).s_addr, 0x0A000002ul);

                try
                {
                    std::string ip5 = "foo.bar";
                    dhorn::experimental::ipv4_address addr5(ip5);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    std::string ip6;
                    dhorn::experimental::ipv4_address addr6(ip6);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }
            }

            TEST_METHOD(StdWStringConstructorTest)
            {
                std::wstring ip1 = L"0.0.0.0";
                dhorn::experimental::ipv4_address addr1(ip1);
                Assert::AreEqual(((in_addr)addr1).s_addr, 0ul);

                std::wstring ip2 = L"255.255.255.255";
                dhorn::experimental::ipv4_address addr2(ip2);
                Assert::AreEqual(((in_addr)addr2).s_addr, 0xFFFFFFFFul);

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"127.0.0.1";
                dhorn::experimental::ipv4_address addr3(ip3);
                Assert::AreEqual(((in_addr)addr3).s_addr, 0x0100007Ful);

                std::wstring ip4 = L"2.0.0.10";
                dhorn::experimental::ipv4_address addr4(ip4);
                Assert::AreEqual(((in_addr)addr4).s_addr, 0x0A000002ul);

                try
                {
                    std::wstring ip5 = L"foo.bar";
                    dhorn::experimental::ipv4_address addr5(ip5);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    std::wstring ip6;
                    dhorn::experimental::ipv4_address addr6(ip6);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }
            }

#pragma endregion



            /*
             * Assignment Tests
             */
#pragma region Assignment Tests

            TEST_METHOD(CreateTypeAssignmentTest)
            {
                dhorn::experimental::ipv4_address ip;

                ip = 0;
                Assert::AreEqual(((in_addr)ip).s_addr, 0ul);

                ip = 0x00c0ffee;
                Assert::AreEqual(((in_addr)ip).s_addr, 0xeeffc000ul);
            }

            TEST_METHOD(IpAddrAssignmentTest)
            {
                dhorn::experimental::ipv4_address ip;
                in_addr addr;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    addr.s_net = rand() % 256;
                    addr.s_host = rand() % 256;
                    addr.s_lh = rand() % 256;
                    addr.s_impno = rand() % 256;

                    ip = addr;
                    Assert::AreEqual(((in_addr)ip).s_addr, addr.s_addr);
                }

                addr.s_addr = 0xFFFFFFFF;
                ip = addr;
                Assert::AreEqual(((in_addr)ip).s_addr, addr.s_addr);

                addr.s_addr = 0;
                ip = addr;
                Assert::AreEqual(((in_addr)ip).s_addr, addr.s_addr);
            }

            TEST_METHOD(SockAddrAssignmentTest)
            {
                // Can successfully construct with valid sockaddr_in
                dhorn::experimental::ipv4_address ip;
                sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_addr.s_addr = htonl(0x7F000001ul);

                ip = addr;
                Assert::IsTrue(((in_addr)ip).s_addr == 0x0100007Ful);

                // Cannot construct if family is AF_INET6
                try
                {
                    addr.sin_family = AF_INET6;
                    ip = addr;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &)
                {
                }
            }

            TEST_METHOD(StringAssignmentTest)
            {
                dhorn::experimental::ipv4_address addr;
                dhorn::experimental::ipv4_address addr2;

                std::string ip1 = "0.0.0.0";
                addr = ip1.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0ul);

                std::string ip2 = "255.255.255.255";
                addr = ip2.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0xFFFFFFFFul);

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "127.0.0.1";
                addr = ip3.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0100007Ful);

                std::string ip4 = "2.0.0.10";
                addr = ip4.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0A000002ul);

                try
                {
                    std::string ip5 = "foo.bar";
                    addr = ip5.c_str();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    addr2 = (char *)nullptr;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSAEFAULT);
                }
            }

            TEST_METHOD(WStringAssignmentTest)
            {
                dhorn::experimental::ipv4_address addr;
                dhorn::experimental::ipv4_address addr2;

                std::wstring ip1 = L"0.0.0.0";
                addr = ip1.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0ul);

                std::wstring ip2 = L"255.255.255.255";
                addr = ip2.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0xFFFFFFFFul);

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"127.0.0.1";
                addr = ip3.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0100007Ful);

                std::wstring ip4 = L"2.0.0.10";
                addr = ip4.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0A000002ul);

                try
                {
                    std::wstring ip5 = L"foo.bar";
                    addr = ip5.c_str();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    addr2 = (wchar_t *)nullptr;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSAEFAULT);
                }
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                dhorn::experimental::ipv4_address addr;
                dhorn::experimental::ipv4_address addr2;

                std::string ip1 = "0.0.0.0";
                addr = ip1;
                Assert::AreEqual(((in_addr)addr).s_addr, 0ul);

                std::string ip2 = "255.255.255.255";
                addr = ip2;
                Assert::AreEqual(((in_addr)addr).s_addr, 0xFFFFFFFFul);

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "127.0.0.1";
                addr = ip3;
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0100007Ful);

                std::string ip4 = "2.0.0.10";
                addr = ip4;
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0A000002ul);

                try
                {
                    std::string ip5 = "foo.bar";
                    addr = ip5;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    addr2 = std::string();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }
            }

            TEST_METHOD(StdWStringAssignmentTest)
            {
                dhorn::experimental::ipv4_address addr;
                dhorn::experimental::ipv4_address addr2;

                std::wstring ip1 = L"0.0.0.0";
                addr = ip1;
                Assert::AreEqual(((in_addr)addr).s_addr, 0ul);

                std::wstring ip2 = L"255.255.255.255";
                addr = ip2;
                Assert::AreEqual(((in_addr)addr).s_addr, 0xFFFFFFFFul);

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"127.0.0.1";
                addr = ip3;
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0100007Ful);

                std::wstring ip4 = L"2.0.0.10";
                addr = ip4;
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0A000002ul);

                try
                {
                    std::wstring ip5 = L"foo.bar";
                    addr = ip5;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    addr2 = std::wstring();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }
            }

#pragma endregion



            /*
             * Type Conversion Tests
             */
#pragma region Type Conversion Tests

            TEST_METHOD(IpAddrConversionTest)
            {
                dhorn::experimental::ipv4_address addr;
                in_addr in;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    in.s_net = rand() % 256;
                    in.s_host = rand() % 256;
                    in.s_lh = rand() % 256;
                    in.s_impno = rand() % 256;

                    addr = in;
                    Assert::AreEqual(((in_addr)addr).s_addr, in.s_addr);
                }
            }

            TEST_METHOD(StringConversionTest)
            {
                static const std::vector<std::string> test_strings
                {
                    "0.0.0.0", "127.0.0.1", "255.255.255.255", "0.1.2.3", "192.168.0.1",
                    "10.0.0.1", "128.64.32.16"
                };

                for (auto &str : test_strings)
                {
                    dhorn::experimental::ipv4_address addr(str);
                    Assert::AreEqual((std::string)addr, str);
                }
            }

            TEST_METHOD(WStringConversionTest)
            {
                static const std::vector<std::wstring> test_strings
                {
                    L"0.0.0.0", L"127.0.0.1", L"255.255.255.255", L"0.1.2.3", L"192.168.0.1",
                    L"10.0.0.1", L"128.64.32.16"
                };

                for (auto &str : test_strings)
                {
                    dhorn::experimental::ipv4_address addr(str);
                    Assert::AreEqual((std::wstring)addr, str);
                }
            }

#pragma endregion



            /*
             * Public Function Tests
             */
#pragma region Public Function Tests

            TEST_METHOD(IpAddrTest)
            {
                dhorn::experimental::ipv4_address addr;
                in_addr in;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    in.s_net = rand() % 256;
                    in.s_host = rand() % 256;
                    in.s_lh = rand() % 256;
                    in.s_impno = rand() % 256;

                    addr = in;
                    Assert::AreEqual(addr.addr().s_addr, in.s_addr);
                }
            }

            TEST_METHOD(StringTest)
            {
                static const std::vector<std::string> test_strings
                {
                    "0.0.0.0", "127.0.0.1", "255.255.255.255", "0.1.2.3", "192.168.0.1",
                    "10.0.0.1", "128.64.32.16"
                };

                for (auto &str : test_strings)
                {
                    dhorn::experimental::ipv4_address addr(str);
                    Assert::AreEqual(addr.str(), str);
                }
            }

            TEST_METHOD(WSTringTest)
            {
                static const std::vector<std::wstring> test_strings
                {
                    L"0.0.0.0", L"127.0.0.1", L"255.255.255.255", L"0.1.2.3", L"192.168.0.1",
                    L"10.0.0.1", L"128.64.32.16"
                };

                for (auto &str : test_strings)
                {
                    dhorn::experimental::ipv4_address addr(str);
                    Assert::AreEqual(addr.str<wchar_t>(), str);
                }
            }

#pragma endregion
        };



        TEST_CLASS(Ipv6AddressTests)
        {
        private:

            in6_addr make_addr(USHORT w0, USHORT w1, USHORT w2, USHORT w3,
                USHORT w4, USHORT w5, USHORT w6, USHORT w7)
            {
                in6_addr addr;

                addr.u.Word[0] = w0;
                addr.u.Word[1] = w1;
                addr.u.Word[2] = w2;
                addr.u.Word[3] = w3;
                addr.u.Word[4] = w4;
                addr.u.Word[5] = w5;
                addr.u.Word[6] = w6;
                addr.u.Word[7] = w7;

                return addr;
            }

            in6_addr zero(void)
            {
                return in6_addr{};
            }

            in6_addr random_addr(void)
            {
                in6_addr addr;

                for (int i = 0; i < 8; i++)
                {
                    addr.u.Word[i] = rand() % 0xFFFF;
                }

                return addr;
            }

        public:

            /*
             * Constructor tests. Note that most consume the operator in_addr6 function
             */
#pragma region Constructor Tests

            TEST_METHOD(DefaultConstructorTest)
            {
                dhorn::experimental::ipv6_address addr;
                Assert::IsTrue((in_addr6)addr == zero());
            }

            TEST_METHOD(CreateTypeConstructorTest)
            {
                uint8_t val[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                  0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

                dhorn::experimental::ipv6_address ip(val);

                for (size_t i = 0; i < 16; i++)
                {
                    Assert::AreEqual(val[i], ip.addr().s6_addr[i]);
                }
            }

            TEST_METHOD(IpAddrConstructorTest)
            {
                in_addr6 addr;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    addr = random_addr();

                    dhorn::experimental::ipv6_address ip(addr);
                    Assert::IsTrue(addr == ip);
                }

                addr = make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
                dhorn::experimental::ipv6_address ip1(addr);
                Assert::IsTrue(addr == ip1);

                addr = zero();
                dhorn::experimental::ipv6_address ip2(addr);
                Assert::IsTrue(addr == ip2);
            }

            TEST_METHOD(SockAddrConstructorTest)
            {
                // Can successfully construct with valid sockaddr_in6
                sockaddr_in6 addr;
                addr.sin6_family = AF_INET6;
                addr.sin6_addr = make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb);

                dhorn::experimental::ipv6_address ip(addr);
                Assert::IsTrue((in6_addr)ip == addr.sin6_addr);

                // Cannot construct if family is AF_INET
                try
                {
                    addr.sin6_family = AF_INET;
                    dhorn::experimental::ipv6_address ip2(addr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &)
                {
                }
            }

            TEST_METHOD(StringConstructorTest)
            {
                std::string ip1 = "::";
                dhorn::experimental::ipv6_address addr1(ip1.c_str());
                Assert::IsTrue(zero() == addr1);

                std::string ip2 = "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                dhorn::experimental::ipv6_address addr2(ip2.c_str());
                Assert::IsTrue(make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) == addr2);

                // Note: on x86/amd64 machines, the byte order in each word should be reversed
                std::string ip3 = "::1";
                dhorn::experimental::ipv6_address addr3(ip3.c_str());
                Assert::IsTrue(make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100) == addr3);

                std::string ip4 = "0123:4567::aaaa:bbbb";
                dhorn::experimental::ipv6_address addr4(ip4.c_str());
                Assert::IsTrue(make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb) == addr4);

                try
                {
                    std::string ip5 = "foo::bar";
                    dhorn::experimental::ipv6_address addr5(ip5.c_str());
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    dhorn::experimental::ipv6_address addr6((char *)nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSAEFAULT);
                }
            }

            TEST_METHOD(WStringConstructorTest)
            {
                std::wstring ip1 = L"::";
                dhorn::experimental::ipv6_address addr1(ip1.c_str());
                Assert::IsTrue(zero() == addr1);

                std::wstring ip2 = L"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                dhorn::experimental::ipv6_address addr2(ip2.c_str());
                Assert::IsTrue(make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) == addr2);

                // Note: on x86/amd64 machines, the byte order in each word should be reversed
                std::wstring ip3 = L"::1";
                dhorn::experimental::ipv6_address addr3(ip3.c_str());
                Assert::IsTrue(make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100) == addr3);

                std::wstring ip4 = L"0123:4567::aaaa:bbbb";
                dhorn::experimental::ipv6_address addr4(ip4.c_str());
                Assert::IsTrue(make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb) == addr4);

                try
                {
                    std::wstring ip5 = L"foo::bar";
                    dhorn::experimental::ipv6_address addr5(ip5.c_str());
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    dhorn::experimental::ipv6_address addr6((char *)nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSAEFAULT);
                }
            }

            TEST_METHOD(StdStringConstructorTest)
            {
                std::string ip1 = "::";
                dhorn::experimental::ipv6_address addr1(ip1);
                Assert::IsTrue(zero() == addr1);

                std::string ip2 = "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                dhorn::experimental::ipv6_address addr2(ip2);
                Assert::IsTrue(make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) == addr2);

                // Note: on x86/amd64 machines, the byte order in each word should be reversed
                std::string ip3 = "::1";
                dhorn::experimental::ipv6_address addr3(ip3);
                Assert::IsTrue(make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100) == addr3);

                std::string ip4 = "0123:4567::aaaa:bbbb";
                dhorn::experimental::ipv6_address addr4(ip4);
                Assert::IsTrue(make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb) == addr4);

                try
                {
                    std::string ip5 = "foo::bar";
                    dhorn::experimental::ipv6_address addr5(ip5);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    std::string ip6;
                    dhorn::experimental::ipv6_address addr6(ip6);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }
            }

            TEST_METHOD(StdWStringConstructorTest)
            {
                std::wstring ip1 = L"::";
                dhorn::experimental::ipv6_address addr1(ip1);
                Assert::IsTrue(zero() == addr1);

                std::wstring ip2 = L"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                dhorn::experimental::ipv6_address addr2(ip2);
                Assert::IsTrue(make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) == addr2);

                // Note: on x86/amd64 machines, the byte order in each word should be reversed
                std::wstring ip3 = L"::1";
                dhorn::experimental::ipv6_address addr3(ip3);
                Assert::IsTrue(make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100) == addr3);

                std::wstring ip4 = L"0123:4567::aaaa:bbbb";
                dhorn::experimental::ipv6_address addr4(ip4);
                Assert::IsTrue(make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb) == addr4);

                try
                {
                    std::wstring ip5 = L"foo::bar";
                    dhorn::experimental::ipv6_address addr5(ip5);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    std::wstring ip6;
                    dhorn::experimental::ipv6_address addr6(ip6);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }
            }

#pragma endregion



            /*
             * Assignment Tests
             */
#pragma region Assignment Tests

            TEST_METHOD(CreateTypeAssignmentTest)
            {
                uint8_t val[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                  0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

                dhorn::experimental::ipv6_address ip;
                ip = val;

                for (size_t i = 0; i < 16; i++)
                {
                    Assert::AreEqual(val[i], ip.addr().s6_addr[i]);
                }
            }

            TEST_METHOD(IpAddrAssignmentTest)
            {
                dhorn::experimental::ipv6_address ip;
                in6_addr addr;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    addr = random_addr();

                    ip = addr;
                    Assert::IsTrue(ip == addr);
                }

                addr = make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
                ip = addr;
                Assert::IsTrue(ip == addr);

                addr = zero();
                ip = addr;
                Assert::IsTrue(ip == addr);
            }

            TEST_METHOD(SockAddrAssignmentTest)
            {
                // Can successfully construct with valid sockaddr_in6
                dhorn::experimental::ipv6_address ip;
                sockaddr_in6 addr;
                addr.sin6_family = AF_INET6;
                addr.sin6_addr = make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb);

                ip = addr;
                Assert::IsTrue((in6_addr)ip == addr.sin6_addr);

                // Cannot construct if family is AF_INET
                try
                {
                    addr.sin6_family = AF_INET;
                    ip = addr;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &)
                {
                }
            }

            TEST_METHOD(StringAssignmentTest)
            {
                dhorn::experimental::ipv6_address addr;
                dhorn::experimental::ipv6_address addr2;

                std::string ip1 = "::";
                addr = ip1.c_str();
                Assert::IsTrue(addr == zero());

                std::string ip2 = "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                addr = ip2.c_str();
                Assert::IsTrue(addr == make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF));

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "::1";
                addr = ip3.c_str();
                Assert::IsTrue(addr == make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100));

                std::string ip4 = "0123:4567::aaaa:bbbb";
                addr = ip4.c_str();
                Assert::IsTrue(addr == make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb));

                try
                {
                    std::string ip5 = "foo::bar";
                    addr = ip5.c_str();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    addr2 = (char *)nullptr;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSAEFAULT);
                }
            }

            TEST_METHOD(WStringAssignmentTest)
            {
                dhorn::experimental::ipv6_address addr;
                dhorn::experimental::ipv6_address addr2;

                std::wstring ip1 = L"::";
                addr = ip1.c_str();
                Assert::IsTrue(addr == zero());

                std::wstring ip2 = L"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                addr = ip2.c_str();
                Assert::IsTrue(addr == make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF));

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"::1";
                addr = ip3.c_str();
                Assert::IsTrue(addr == make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100));

                std::wstring ip4 = L"0123:4567::aaaa:bbbb";
                addr = ip4.c_str();
                Assert::IsTrue(addr == make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb));

                try
                {
                    std::wstring ip5 = L"foo::bar";
                    addr = ip5.c_str();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    addr2 = (wchar_t *)nullptr;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSAEFAULT);
                }
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                dhorn::experimental::ipv6_address addr;
                dhorn::experimental::ipv6_address addr2;

                std::string ip1 = "::";
                addr = ip1;
                Assert::IsTrue(addr == zero());

                std::string ip2 = "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                addr = ip2;
                Assert::IsTrue(addr == make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF));

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "::1";
                addr = ip3;
                Assert::IsTrue(addr == make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100));

                std::string ip4 = "0123:4567::aaaa:bbbb";
                addr = ip4;
                Assert::IsTrue(addr == make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb));

                try
                {
                    std::string ip5 = "foo::bar";
                    addr = ip5;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    std::string ip6;
                    addr2 = ip6;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }
            }

            TEST_METHOD(StdWStringAssignmentTest)
            {
                dhorn::experimental::ipv6_address addr;
                dhorn::experimental::ipv6_address addr2;

                std::wstring ip1 = L"::";
                addr = ip1;
                Assert::IsTrue(addr == zero());

                std::wstring ip2 = L"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                addr = ip2;
                Assert::IsTrue(addr == make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF));

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"::1";
                addr = ip3;
                Assert::IsTrue(addr == make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100));

                std::wstring ip4 = L"0123:4567::aaaa:bbbb";
                addr = ip4;
                Assert::IsTrue(addr == make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb));

                try
                {
                    std::wstring ip5 = L"foo::bar";
                    addr = ip5;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }

                try
                {
                    std::wstring ip6;
                    addr2 = ip6;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSA_INVALID_PARAMETER);
                }
            }

#pragma endregion



            /*
             * Type Conversion Tests
             */
#pragma region Type Conversion Tests

            TEST_METHOD(IpAddrConversionTest)
            {
                dhorn::experimental::ipv6_address addr;
                in6_addr in;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    in = random_addr();

                    addr = in;
                    Assert::IsTrue((in6_addr)addr == in);
                }
            }

            TEST_METHOD(StringConversionTest)
            {
                static const std::vector<std::string> test_strings
                {
                    "::", "::1", "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                    "0:1111::eeee:ffff", "123:4567:89ab:cdef::",
                    "0:1111:2222:3333:4444:5555:6666:7777", "::128:64:32:16"
                };

                for (auto &str : test_strings)
                {
                    dhorn::experimental::ipv6_address addr(str);
                    Assert::AreEqual((std::string)addr, str);
                }
            }

            TEST_METHOD(WStringConversionTest)
            {
                static const std::vector<std::wstring> test_strings
                {
                    L"::", L"::1", L"ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                    L"0:1111::eeee:ffff", L"123:4567:89ab:cdef::",
                    L"0:1111:2222:3333:4444:5555:6666:7777", L"::128:64:32:16"
                };

                for (auto &str : test_strings)
                {
                    dhorn::experimental::ipv6_address addr(str);
                    Assert::AreEqual((std::wstring)addr, str);
                }
            }

#pragma endregion



            /*
             * Public Function Tests
             */
#pragma region Public Function Tests

            TEST_METHOD(IpAddrTest)
            {
                dhorn::experimental::ipv6_address addr;
                in6_addr in;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    in = random_addr();

                    addr = in;
                    Assert::IsTrue(addr.addr() == in);
                }
            }

            TEST_METHOD(StringTest)
            {
                static const std::vector<std::string> test_strings
                {
                    "::", "::1", "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                    "0:1111::eeee:ffff", "123:4567:89ab:cdef::",
                    "0:1111:2222:3333:4444:5555:6666:7777", "::128:64:32:16"
                };

                for (auto &str : test_strings)
                {
                    dhorn::experimental::ipv6_address addr(str);
                    Assert::AreEqual(addr.str(), str);
                }
            }

            TEST_METHOD(WSTringTest)
            {
                static const std::vector<std::wstring> test_strings
                {
                    L"::", L"::1", L"ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                    L"0:1111::eeee:ffff", L"123:4567:89ab:cdef::",
                    L"0:1111:2222:3333:4444:5555:6666:7777", L"::128:64:32:16"
                };

                for (auto &str : test_strings)
                {
                    dhorn::experimental::ipv6_address addr(str);
                    Assert::AreEqual(addr.str<wchar_t>(), str);
                }
            }

#pragma endregion
        };



        TEST_CLASS(SocketBaseTests)
        {
            template <typename Func>
            void ExecuteSocketTest(const Func &func, bool closeSocket = true)
            {
                // Construct a socket. This is the one that we'll be using for the test
                dhorn::experimental::socket_t rawSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                Assert::IsTrue(rawSocket != dhorn::experimental::invalid_socket);

                func(rawSocket);

                if (closeSocket)
                {
                    // Close the socket; don't care if this fails
                    ::closesocket(rawSocket);
                }
            }

            template <typename ServerFunc, typename ClientFunc>
            void ExecuteServerTest(
                const dhorn::experimental::socket_address &serverAddress,
                const ServerFunc &serverFunc,
                const ClientFunc &clientFunc)
            {
                dhorn::experimental::socket_base server(
                    dhorn::experimental::address_family::internetwork_version_4,
                    dhorn::experimental::socket_type::stream,
                    dhorn::experimental::ip_protocol::transmission_control_protocol);
                server.bind(serverAddress);

                // Make sure the server is listening before we kick off the client thread
                server.listen(5);

                std::thread clientThread([&]()
                {
                    dhorn::experimental::socket_address addr(dhorn::experimental::ipv4_address(dhorn::experimental::local_host), 1337);// TODO: port
                    dhorn::experimental::socket_base client(
                        dhorn::experimental::address_family::internetwork_version_4,
                        dhorn::experimental::socket_type::stream,
                        dhorn::experimental::ip_protocol::transmission_control_protocol);

                    client.connect(addr);
                    clientFunc(client);

                    client.close();
                });

                dhorn::experimental::socket_address clientAddress;
                auto clientSocket = server.accept(clientAddress);

                serverFunc(server, clientSocket);

                clientThread.join();
                clientSocket.close();
                server.close();
            }

            TEST_CLASS_INITIALIZE(Initialize)
            {
                // Make sure ws2_32.dll is loaded before any test runs
                WSADATA data;
                auto result = WSAStartup(MAKEWORD(2, 2), &data);

                // If the dll load fails, we cannot call WSAGetLastError
                if (result)
                {
                    throw dhorn::experimental::socket_exception(result);
                }
            }

            TEST_METHOD(DefaultConstructorTest)
            {
                // Default constructor has socket with value dhorn::experimental::invalid_socket
                dhorn::experimental::socket_base sock;
                Assert::IsTrue(static_cast<SOCKET>(sock) == dhorn::experimental::invalid_socket);
            }

            TEST_METHOD(SocketConstuctorTest)
            {
                ExecuteSocketTest([](dhorn::experimental::socket_t rawSocket)
                {
                    // Constructing with invalid socket should not cause issues
                    dhorn::experimental::socket_base sock(dhorn::experimental::invalid_socket);
                    Assert::IsTrue(static_cast<SOCKET>(sock) == dhorn::experimental::invalid_socket);

                    // Construct with pre-made socket. Since we're not testing the close function right now, we'll get
                    // an exception. We don't necessarily care what the exception is, and that's okay since that's not
                    // the point here. In fact, we don't necessarily care whether or not an exception is thrown
                    try
                    {
                        dhorn::experimental::socket_base sock2(rawSocket);
                        Assert::IsTrue(static_cast<SOCKET>(sock2) == rawSocket);
                    }
                    catch (dhorn::experimental::socket_exception &/*ignored*/)
                    {
                    }
                });
            }

            TEST_METHOD(MoveConstructorTest)
            {
                ExecuteSocketTest([](dhorn::experimental::socket_t rawSocket)
                {
                    dhorn::experimental::socket_base sock(rawSocket);

                    // Move to a new socket. This should throw an exception when the destructor is run. Just like
                    // before, we do not care what the exception is, nor do we really care if one is thrown. Just catch
                    // and ignore the exception if it does occur
                    try
                    {
                        dhorn::experimental::socket_base sock2(std::move(sock));
                        Assert::IsTrue(static_cast<SOCKET>(sock) == dhorn::experimental::invalid_socket);
                        Assert::IsTrue(static_cast<SOCKET>(sock2) == rawSocket);
                    }
                    catch (dhorn::experimental::socket_exception &/*ignored*/)
                    {
                    }

                    // Moving an invalid socket shouldn't give any errors, etc.
                    dhorn::experimental::socket_base sock2(std::move(sock));
                    Assert::IsTrue(static_cast<SOCKET>(sock) == dhorn::experimental::invalid_socket);
                    Assert::IsTrue(static_cast<SOCKET>(sock2) == dhorn::experimental::invalid_socket);
                });
            }

            TEST_METHOD(DestructorTest)
            {
                ExecuteSocketTest([](dhorn::experimental::socket_t rawSocket)
                {
                    // We only care that we get an exception for the destructor test
                    try
                    {
                        {
                            dhorn::experimental::socket_base sock(rawSocket);
                            (void)sock;
                        }
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (dhorn::experimental::socket_exception &e)
                    {
                        Assert::IsTrue(e.get_error() == WSAEFAULT);
                    }
                });
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                ExecuteSocketTest([](dhorn::experimental::socket_t rawSocket)
                {
                    dhorn::experimental::socket_base sock(rawSocket);

                    // Move to a new socket. This should throw an exception when the destructor is run. Just like
                    // before, we do not care what the exception is, nor do we really care if one is thrown. Just catch
                    // and ignore the exception if it does occur
                    try
                    {
                        dhorn::experimental::socket_base sock2;
                        Assert::IsTrue(static_cast<SOCKET>(sock2) == dhorn::experimental::invalid_socket); // don't inline

                        sock2 = std::move(sock);
                        Assert::IsTrue(static_cast<SOCKET>(sock) == dhorn::experimental::invalid_socket);
                        Assert::IsTrue(static_cast<SOCKET>(sock2) == rawSocket);
                    }
                    catch (dhorn::experimental::socket_exception &/*ignored*/)
                    {
                    }

                    // Moving an invalid socket shouldn't give any errors, etc.
                    dhorn::experimental::socket_base sock2(std::move(sock));
                    Assert::IsTrue(static_cast<SOCKET>(sock) == dhorn::experimental::invalid_socket);
                    Assert::IsTrue(static_cast<SOCKET>(sock2) == dhorn::experimental::invalid_socket);
                });
            }

            TEST_METHOD(SocketAssignmentTest)
            {
                ExecuteSocketTest([](dhorn::experimental::socket_t rawSocket)
                {
                    // Yet again, we expect, but do not check for, an exception in the destructor
                    try
                    {
                        dhorn::experimental::socket_base sock;
                        Assert::IsTrue(static_cast<SOCKET>(sock) == dhorn::experimental::invalid_socket);

                        sock = rawSocket;
                        Assert::IsTrue(static_cast<SOCKET>(sock) == rawSocket);
                    }
                    catch (dhorn::experimental::socket_exception &/*ignored*/)
                    {
                    }
                });
            }

            TEST_METHOD(OperatorBoolTest)
            {
                ExecuteSocketTest([](dhorn::experimental::socket_t rawSocket)
                {
                    // Yet again, we expect, but do not check for, an exception during the destructor
                    try
                    {
                        dhorn::experimental::socket_base sock;
                        Assert::IsFalse(sock);

                        sock = rawSocket;
                        Assert::IsTrue(sock);
                    }
                    catch (dhorn::experimental::socket_exception &/*ignored*/)
                    {
                    }
                });
            }

            TEST_METHOD(OpenConstructorTest)
            {
                // Creating a "normal" TCP or UDP socket should not fail
                dhorn::experimental::socket_base tcpSocket(
                    dhorn::experimental::address_family::internetwork_version_4,
                    dhorn::experimental::socket_type::stream,
                    dhorn::experimental::ip_protocol::transmission_control_protocol);
                Assert::AreNotEqual(static_cast<SOCKET>(tcpSocket), dhorn::experimental::invalid_socket);

                dhorn::experimental::socket_base udpSocket(
                    dhorn::experimental::address_family::internetwork_version_4,
                    dhorn::experimental::socket_type::datagram,
                    dhorn::experimental::ip_protocol::user_datagram_protocol);
                Assert::AreNotEqual(static_cast<SOCKET>(udpSocket), dhorn::experimental::invalid_socket);

                // Close the sockets ourselves; We're not testing the close function yet
                ::closesocket(tcpSocket.detach());
                ::closesocket(udpSocket.detach());

                // TCP/UDP mis-match should throw
                try
                {
                    dhorn::experimental::socket_base sock(
                        dhorn::experimental::address_family::internetwork_version_4,
                        dhorn::experimental::socket_type::stream,
                        dhorn::experimental::ip_protocol::user_datagram_protocol);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::AreEqual(static_cast<int>(WSAEPROTONOSUPPORT), e.get_error());
                }

                try
                {
                    dhorn::experimental::socket_base sock(
                        dhorn::experimental::address_family::internetwork_version_4,
                        dhorn::experimental::socket_type::datagram,
                        dhorn::experimental::ip_protocol::transmission_control_protocol);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::AreEqual(static_cast<int>(WSAEPROTONOSUPPORT), e.get_error());
                }
            }

            TEST_METHOD(OpenTest)
            {
                dhorn::experimental::socket_base tcpSocket;
                dhorn::experimental::socket_base udpSocket;

                // Creating a "normal" TCP or UDP socket should not fail
                tcpSocket.open(
                    dhorn::experimental::address_family::internetwork_version_4,
                    dhorn::experimental::socket_type::stream,
                    dhorn::experimental::ip_protocol::transmission_control_protocol);
                Assert::AreNotEqual(static_cast<SOCKET>(tcpSocket), dhorn::experimental::invalid_socket);

                udpSocket.open(
                    dhorn::experimental::address_family::internetwork_version_4,
                    dhorn::experimental::socket_type::datagram,
                    dhorn::experimental::ip_protocol::user_datagram_protocol);
                Assert::AreNotEqual(static_cast<SOCKET>(udpSocket), dhorn::experimental::invalid_socket);

                // Calling open on a socket already in use should throw and leave the socket in a correct state
                try
                {
                    tcpSocket.open(
                        dhorn::experimental::address_family::internetwork_version_4,
                        dhorn::experimental::socket_type::stream,
                        dhorn::experimental::ip_protocol::transmission_control_protocol);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::AreEqual(static_cast<int>(WSAEISCONN), e.get_error());
                }

                // Close the sockets ourselves; We're not testing the close function yet
                ::closesocket(tcpSocket.detach());
                ::closesocket(udpSocket.detach());

                // TCP/UDP mis-match should throw
                try
                {
                    tcpSocket.open(
                        dhorn::experimental::address_family::internetwork_version_4,
                        dhorn::experimental::socket_type::stream,
                        dhorn::experimental::ip_protocol::user_datagram_protocol);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::AreEqual(static_cast<int>(WSAEPROTONOSUPPORT), e.get_error());
                }

                try
                {
                    udpSocket.open(
                        dhorn::experimental::address_family::internetwork_version_4,
                        dhorn::experimental::socket_type::datagram,
                        dhorn::experimental::ip_protocol::transmission_control_protocol);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::AreEqual(static_cast<int>(WSAEPROTONOSUPPORT), e.get_error());
                }
            }

            TEST_METHOD(CloseTest)
            {
                ExecuteSocketTest([](dhorn::experimental::socket_t rawSocket)
                {
                    dhorn::experimental::socket_base sock(rawSocket);
                    Assert::AreEqual(rawSocket, static_cast<SOCKET>(sock));

                    sock.close();
                    Assert::AreEqual(dhorn::experimental::invalid_socket, static_cast<SOCKET>(sock));

                    // Calling closesocket should fail since the socket is already closed
                    Assert::AreNotEqual(0, ::closesocket(rawSocket));
                }, false /*don't close*/);
            }

            TEST_METHOD(BindTest)
            {
                dhorn::experimental::socket_address testAddr(dhorn::experimental::ipv4_address(dhorn::experimental::any_address), 1337);
                dhorn::experimental::socket_address testAddr2(dhorn::experimental::ipv4_address(dhorn::experimental::any_address), 1338);

                // Bind should fail on invalid socket with WSAENOTSOCK
                try
                {
                    dhorn::experimental::socket_base sock;
                    sock.bind(testAddr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSAENOTSOCK);
                }

                // Bind should succeed if the socket is created with IPPROTO_TCP
                auto rawSock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                Assert::IsTrue(rawSock != dhorn::experimental::invalid_socket);

                dhorn::experimental::socket_base sock(rawSock);
                sock.bind(testAddr);

                try
                {
                    // Expect an exception if we call bind twice (with a different address)
                    sock.bind(testAddr2);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::IsTrue(e.get_error() == WSAEINVAL);
                }

                sock.close();

                // ExecuteSocketTest gives us a socket created with IPPROTO_UDP
                ExecuteSocketTest([&](dhorn::experimental::socket_t rawSocket)
                {
                    dhorn::experimental::socket_base sock(rawSocket);
                    sock.bind(testAddr);

                    try
                    {
                        // Expect an exception if we call bind twice (with a different address)
                        sock.bind(testAddr2);
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (dhorn::experimental::socket_exception &e)
                    {
                        Assert::IsTrue(e.get_error() == WSAEINVAL);
                    }

                    sock.detach(); // Let ExecuteSocketTest close the socket
                });
            }

            TEST_METHOD(GetSocketNameTest)
            {
                dhorn::experimental::socket_address testAddr(dhorn::experimental::ipv4_address(dhorn::experimental::any_address), 1337);
                dhorn::experimental::socket_base sock(
                    dhorn::experimental::address_family::internetwork_version_4,
                    dhorn::experimental::socket_type::datagram,
                    dhorn::experimental::ip_protocol::user_datagram_protocol);

                // Calling get_socket_name on a non-bound socket should throw
                try
                {
                    sock.get_socket_name();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::AreEqual(static_cast<int>(WSAEINVAL), e.get_error());
                }

                // Bind against the test socket_address
                sock.bind(testAddr);

                // Make sure that the address is the same
                auto addr = sock.get_socket_name();
                Assert::IsTrue(addr == testAddr);

                sock.close();
            }

            TEST_METHOD(SocketOptionTest)
            {
                dhorn::experimental::socket_address testAddr(dhorn::experimental::ipv4_address(dhorn::experimental::any_address), 1337);
                dhorn::experimental::socket_base sock;

                // Set/get socket option should fail on an invalid socket
                try
                {
                    sock.set_socket_option(dhorn::experimental::socket_level::socket, dhorn::experimental::socket_option::debug_info, true);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::AreEqual(static_cast<int>(WSAENOTSOCK), e.get_error());
                }

                try
                {
                    bool val = sock.get_socket_option<bool>(
                        dhorn::experimental::socket_level::socket,
                        dhorn::experimental::socket_option::debug_info);
                    (void)val;
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::socket_exception &e)
                {
                    Assert::AreEqual(static_cast<int>(WSAENOTSOCK), e.get_error());
                }

                sock.open(
                    dhorn::experimental::address_family::internetwork_version_4,
                    dhorn::experimental::socket_type::datagram,
                    dhorn::experimental::ip_protocol::user_datagram_protocol);

                // It is expected that we bind the socket before getting/setting options on it
                sock.bind(testAddr);

                // Verify that the value is false first (otherwise we're testing nothing...)
                bool val = sock.get_socket_option<bool>(dhorn::experimental::socket_level::socket, dhorn::experimental::socket_option::debug_info);
                Assert::IsFalse(val);

                sock.set_socket_option(dhorn::experimental::socket_level::socket, dhorn::experimental::socket_option::debug_info, true);
                val = sock.get_socket_option<bool>(dhorn::experimental::socket_level::socket, dhorn::experimental::socket_option::debug_info);
                Assert::IsTrue(val);

                sock.close();
            }

            TEST_METHOD(ListenConnectAcceptTest)
            {
                dhorn::experimental::socket_address serverAddress(dhorn::experimental::ipv4_address(dhorn::experimental::local_host), 1337);

                bool succeeded = false;
                ExecuteServerTest(
                    serverAddress,
                    [&](dhorn::experimental::socket_base& /*serverSocket*/, dhorn::experimental::socket_base& /*connectedSocket*/)
                {
                    // No messages need to be sent; we just test connection
                },
                    [&](dhorn::experimental::socket_base& /*clientSocket*/)
                {
                    // If client didn't throw, the connection succeeded
                    succeeded = true;
                });

                Assert::IsTrue(succeeded);
            }

            TEST_METHOD(GetPeerNameTest)
            {
                dhorn::experimental::socket_address serverAddress(dhorn::experimental::ipv4_address(dhorn::experimental::any_address), 1337);

                ExecuteServerTest(
                    serverAddress,
                    [&](dhorn::experimental::socket_base& /*serverSocket*/, dhorn::experimental::socket_base& connectedSocket)
                {
                    auto peerName = connectedSocket.get_peer_name();
                    Assert::IsTrue(peerName.size() > 0);
                },
                    [&](dhorn::experimental::socket_base& clientSocket)
                {
                    // We can't predict the address of what we're connected to, but we can assert that it gave us a
                    // valid value
                    auto peerName = clientSocket.get_peer_name();
                    Assert::IsTrue(peerName.size() > 0);
                });
            }

            TEST_METHOD(SendReceiveTest)
            {
                dhorn::experimental::socket_address serverAddress(dhorn::experimental::ipv4_address(dhorn::experimental::any_address), 1337);

                ExecuteServerTest(
                    serverAddress,
                    [&](dhorn::experimental::socket_base& /*serverSocket*/, dhorn::experimental::socket_base& connectedSocket)
                {
                    // Receive using a pointer to a buffer
                    char buffer[4] = {};
                    auto len = connectedSocket.receive(buffer, 3, dhorn::experimental::message_flags::none);
                    Assert::IsTrue(len == 3);
                    Assert::IsTrue(strcmp(buffer, "foo") == 0);

                    // Send using iterators
                    const char sendBuff[] = "bar";
                    len = connectedSocket.send(std::begin(sendBuff), std::end(sendBuff), dhorn::experimental::message_flags::none);
                    Assert::IsTrue(len == 4);

                    // Receive using an array
                    len = connectedSocket.receive(buffer, dhorn::experimental::message_flags::none);
                    Assert::IsTrue(len == 4);
                    Assert::IsTrue(strcmp(buffer, "car") == 0);
                },
                    [&](dhorn::experimental::socket_base& clientSocket)
                {
                    // Send using a pointer to a buffer
                    auto len = clientSocket.send("foo", 3, dhorn::experimental::message_flags::none);
                    Assert::IsTrue(len == 3);

                    // Receive using iterators
                    char buffer[4] = {};
                    auto itr = clientSocket.receive(std::begin(buffer), std::end(buffer), dhorn::experimental::message_flags::none);
                    Assert::IsTrue(std::distance(std::begin(buffer), itr) == 4);
                    Assert::IsTrue(strcmp(buffer, "bar") == 0);

                    // Send using an array
                    const char sendBuff[] = "car";
                    len = clientSocket.send(sendBuff, dhorn::experimental::message_flags::none);
                });
            }

            TEST_METHOD(SendToReceiveFromTest)
            {
                dhorn::experimental::socket_base sock1(
                    dhorn::experimental::address_family::internetwork_version_4,
                    dhorn::experimental::socket_type::datagram,
                    dhorn::experimental::ip_protocol::user_datagram_protocol);
                dhorn::experimental::socket_base sock2(
                    dhorn::experimental::address_family::internetwork_version_4,
                    dhorn::experimental::socket_type::datagram,
                    dhorn::experimental::ip_protocol::user_datagram_protocol);

                sock1.bind(dhorn::experimental::socket_address(dhorn::experimental::ipv4_address(dhorn::experimental::any_address), 1337));
                sock2.bind(dhorn::experimental::socket_address(dhorn::experimental::ipv4_address(dhorn::experimental::any_address), 1338));

                // Send "foo" using a pointer to a buffer
                dhorn::experimental::socket_address dest2(dhorn::experimental::ipv4_address(dhorn::experimental::local_host), 1338);
                auto len = sock1.send_to("foo", 3, dhorn::experimental::message_flags::none, dest2);
                Assert::IsTrue(len == 3);

                // Receive using a pointer to a buffer
                char buffer[4] = {};
                dhorn::experimental::socket_address dest1; // Should pick up the correct destination address
                len = sock2.receive_from(buffer, 3, dhorn::experimental::message_flags::none, dest1);
                Assert::IsTrue(len == 3);
                Assert::IsTrue(strcmp(buffer, "foo") == 0);


                // Send "bar" using iterators
                const char sendBuff[] = "bar";
                len = sock2.send_to(std::begin(sendBuff), std::end(sendBuff), dhorn::experimental::message_flags::none, dest1);
                Assert::IsTrue(len == 4);

                // Receive using iterators
                auto itr = sock1.receive_from(std::begin(buffer), std::end(buffer), dhorn::experimental::message_flags::none, dest2);
                Assert::IsTrue(std::distance(std::begin(buffer), itr) == 4);
                Assert::IsTrue(strcmp(buffer, "bar") == 0);


                // Send "car" using an array
                const char sendBuff2[] = "car";
                len = sock1.send_to(sendBuff2, dhorn::experimental::message_flags::none, dest2);
                Assert::IsTrue(len == 4);

                // Receive using an array
                sock2.receive_from(buffer, dhorn::experimental::message_flags::none, dest1);
                Assert::IsTrue(len == 4);
                Assert::IsTrue(strcmp(buffer, "car") == 0);

                sock1.close();
                sock2.close();
            }
        };



        TEST_CLASS(UdpPacketTests)
        {
            TEST_METHOD(CapacityTest)
            {
                dhorn::experimental::udp_packet<int> packet(100);
                Assert::AreEqual(static_cast<size_t>(100), packet.capacity());
            }

            TEST_METHOD(InitialSizeTest)
            {
                // Size is tested more completely in the SetDataTest
                dhorn::experimental::udp_packet<int> packet(100);
                Assert::AreEqual(static_cast<size_t>(0), packet.size());
            }

            TEST_METHOD(SetDataTest)
            {
                dhorn::experimental::udp_packet<char> packet(100);

                char vals[101];
                for (char i = 0; i < 101; ++i)
                {
                    vals[i] = i;
                }

                // Test setting with the maximum amount
                packet.set_data(std::begin(vals), std::begin(vals) + 100);
                Assert::AreEqual(static_cast<size_t>(100), packet.size());
                Assert::IsTrue(std::equal(
                    std::begin(vals),
                    std::begin(vals) + 100,
                    packet.buffer().get()));

                // Now set with half the amount
                packet.set_data(std::begin(vals) + 50, std::begin(vals) + 100);
                Assert::AreEqual(static_cast<size_t>(50), packet.size());
                Assert::IsTrue(std::equal(
                    std::begin(vals) + 50,
                    std::begin(vals) + 100,
                    packet.buffer().get()));

                // Setting with more than the maximum amount should throw
                try
                {
                    packet.set_data(std::begin(vals), std::end(vals));
                    Assert::Fail(L"Expected an exception");
                }
                catch (std::out_of_range &)
                {
                }
            }

            TEST_METHOD(SwapTest)
            {
                dhorn::experimental::udp_packet<int> packet1(10);
                dhorn::experimental::udp_packet<int> packet2(20);

                std::vector<int> vals1 = { 0, 1, 2, 3, 4 };
                std::vector<int> vals2 = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

                packet1.set_data(std::begin(vals1), std::end(vals1));
                packet2.set_data(std::begin(vals2), std::end(vals2));

                auto checkFunc = [&](const dhorn::experimental::udp_packet<int> &p1, const dhorn::experimental::udp_packet<int> &p2)
                {
                    Assert::AreEqual(static_cast<size_t>(5), p1.size());
                    Assert::AreEqual(static_cast<size_t>(10), p1.capacity());
                    Assert::IsTrue(std::equal(
                        std::begin(vals1),
                        std::end(vals1),
                        p1.buffer().get()));

                    Assert::AreEqual(static_cast<size_t>(10), p2.size());
                    Assert::AreEqual(static_cast<size_t>(20), p2.capacity());
                    Assert::IsTrue(std::equal(
                        std::begin(vals2),
                        std::end(vals2),
                        p2.buffer().get()));
                };

                checkFunc(packet1, packet2);

                // Check using swap member function
                packet1.swap(packet2);
                checkFunc(packet2, packet1);

                // Check using std::swap
                std::swap(packet1, packet2);
                checkFunc(packet1, packet2);
            }
        };



        TEST_CLASS(UdpSocketTests)
        {
            TEST_CLASS_INITIALIZE(Initialize)
            {
                // Make sure ws2_32.dll is loaded before any test runs
                WSADATA data;
                auto result = WSAStartup(MAKEWORD(2, 2), &data);

                // If the dll load fails, we cannot call WSAGetLastError
                if (result)
                {
                    throw dhorn::experimental::socket_exception(result);
                }
            }

            // Since all other socket functions just forward to the underlying socket_base, we won't re-test them
            TEST_METHOD(SendReceiveWithPacketTest)
            {
                dhorn::experimental::udp_socket sock1;
                dhorn::experimental::udp_socket sock2;

                // Bind at least one socket to a known port
                dhorn::experimental::socket_address addr(dhorn::experimental::ipv4_address(dhorn::experimental::any_address), 1337);
                sock2.bind(addr);

                // Form the udp_packet
                dhorn::experimental::udp_packet<char> packet1(4);
                packet1.set_data("foo", 4);
                packet1.set_addr(dhorn::experimental::socket_address(dhorn::experimental::ipv4_address(dhorn::experimental::local_host), 1337));

                // Send/receive the packet
                sock1.send(packet1);
                dhorn::experimental::udp_packet<char> packet2(4);
                sock2.receive(packet2);
                Assert::AreEqual(static_cast<size_t>(4), packet2.size());
                Assert::AreEqual(0, strcmp("foo", packet2.buffer().get()));

                // Send the next udp_packet re-using the socket_address information from the receive
                packet2.set_data("bar", 4);
                sock2.send(packet2);
                sock1.receive(packet1);
                Assert::AreEqual(static_cast<size_t>(4), packet1.size());
                Assert::AreEqual(0, strcmp("bar", packet1.buffer().get()));

                sock1.close();
                sock2.close();
            }
        };



        TEST_CLASS(TcpSocketsTests)
        {
            TEST_CLASS_INITIALIZE(Initialize)
            {
                // Make sure ws2_32.dll is loaded before any test runs
                WSADATA data;
                auto result = WSAStartup(MAKEWORD(2, 2), &data);

                // If the dll load fails, we cannot call WSAGetLastError
                if (result)
                {
                    throw dhorn::experimental::socket_exception(result);
                }
            }

            TEST_METHOD(SimpleClientServerTest)
            {
                // Create the server
                dhorn::experimental::socket_address serverAddr(dhorn::experimental::ipv4_address(dhorn::experimental::any_address), 1337);
                dhorn::experimental::server_socket server;
                server.bind(serverAddr);
                server.listen(5);

                // This is the data that will be sent and received
                std::vector<int> data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

                // Start the client thread and connect to the server
                std::thread clientThread([&data]()
                {
                    dhorn::experimental::tcp_socket client;
                    dhorn::experimental::socket_address addr(dhorn::experimental::ipv4_address(dhorn::experimental::local_host), 1337);
                    client.connect(addr);

                    // On connection, we let the server send us the data first
                    std::vector<int> recData(data.size() * 2);
                    auto itr = client.receive(std::begin(recData), std::end(recData));
                    recData.erase(itr, std::end(recData));
                    Assert::AreEqual(data.size(), recData.size());
                    Assert::IsTrue(std::equal(std::begin(data), std::end(data), std::begin(recData)));

                    // Send the data back (in reverse order)
                    client.send(data.rbegin(), data.rend());

                    client.shutdown(dhorn::experimental::shutdown_options::send);
                    client.close();
                });

                // Accept the incoming socket and send the data
                dhorn::experimental::socket_address clientAddr;
                auto sock = server.accept(clientAddr);
                sock.send(std::begin(data), std::end(data));

                // Now receive the data back (in reverse order)
                std::vector<int> recData(data.size());
                auto itr = sock.receive(std::begin(recData), std::end(recData));
                recData.erase(itr, std::end(recData));
                Assert::AreEqual(data.size(), recData.size());
                for (size_t i = 0; i < data.size(); ++i)
                {
                    Assert::AreEqual(recData[recData.size() - i - 1], data[i]);
                }

                // Close the connected client
                sock.shutdown(dhorn::experimental::shutdown_options::send);
                sock.close();

                clientThread.join();
                server.close();
            }
        };
    }
}

#pragma warning(pop)
