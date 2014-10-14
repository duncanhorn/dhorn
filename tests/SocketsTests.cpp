/*
 * SocketsTest.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn sockets classes, structures, functions, etc.
 */

#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include <vector>

#include "dhorn/sockets.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_COUNT      100

bool operator==(_In_ const in6_addr &addr1, _In_ const in6_addr &addr2)
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
                dhorn::ipv4_address addr;
                Assert::AreEqual(((in_addr)addr).s_addr, 0ul);
                Assert::IsFalse(addr.fail());
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

                    dhorn::ipv4_address ip(addr);
                    Assert::AreEqual(((in_addr)ip).s_addr, addr.s_addr);
                    Assert::IsFalse(ip.fail());
                }

                addr.s_addr = 0xFFFFFFFF;
                dhorn::ipv4_address ip1(addr);
                Assert::AreEqual(((in_addr)ip1).s_addr, addr.s_addr);
                Assert::IsFalse(ip1.fail());

                addr.s_addr = 0;
                dhorn::ipv4_address ip2(addr);
                Assert::AreEqual(((in_addr)ip2).s_addr, addr.s_addr);
                Assert::IsFalse(ip2.fail());
            }

            TEST_METHOD(StringConstructorTest)
            {
                std::string ip1 = "0.0.0.0";
                dhorn::ipv4_address addr1(ip1.c_str());
                Assert::AreEqual(((in_addr)addr1).s_addr, 0ul);
                Assert::IsFalse(addr1.fail());

                std::string ip2 = "255.255.255.255";
                dhorn::ipv4_address addr2(ip2.c_str());
                Assert::AreEqual(((in_addr)addr2).s_addr, 0xFFFFFFFFul);
                Assert::IsFalse(addr2.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "127.0.0.1";
                dhorn::ipv4_address addr3(ip3.c_str());
                Assert::AreEqual(((in_addr)addr3).s_addr, 0x0100007Ful);
                Assert::IsFalse(addr3.fail());

                std::string ip4 = "2.0.0.10";
                dhorn::ipv4_address addr4(ip4.c_str());
                Assert::AreEqual(((in_addr)addr4).s_addr, 0x0A000002ul);
                Assert::IsFalse(addr4.fail());

                std::string ip5 = "foo.bar";
                dhorn::ipv4_address addr5(ip5.c_str());
                Assert::IsTrue(addr5.fail());

#pragma warning(push)
#pragma warning(disable:6387)
                dhorn::ipv4_address addr6((char *)nullptr);
#pragma warning(pop)
                Assert::IsTrue(addr6.fail());
            }

            TEST_METHOD(WStringConstructorTest)
            {
                std::wstring ip1 = L"0.0.0.0";
                dhorn::ipv4_address addr1(ip1.c_str());
                Assert::AreEqual(((in_addr)addr1).s_addr, 0ul);
                Assert::IsFalse(addr1.fail());

                std::wstring ip2 = L"255.255.255.255";
                dhorn::ipv4_address addr2(ip2.c_str());
                Assert::AreEqual(((in_addr)addr2).s_addr, 0xFFFFFFFFul);
                Assert::IsFalse(addr2.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"127.0.0.1";
                dhorn::ipv4_address addr3(ip3.c_str());
                Assert::AreEqual(((in_addr)addr3).s_addr, 0x0100007Ful);
                Assert::IsFalse(addr3.fail());

                std::wstring ip4 = L"2.0.0.10";
                dhorn::ipv4_address addr4(ip4.c_str());
                Assert::AreEqual(((in_addr)addr4).s_addr, 0x0A000002ul);
                Assert::IsFalse(addr4.fail());

                std::wstring ip5 = L"foo.bar";
                dhorn::ipv4_address addr5(ip5.c_str());
                Assert::IsTrue(addr5.fail());

#pragma warning(push)
#pragma warning(disable:6387)
                dhorn::ipv4_address addr6((wchar_t *)nullptr);
#pragma warning(pop)
                Assert::IsTrue(addr6.fail());
            }

            TEST_METHOD(StdStringConstructorTest)
            {
                std::string ip1 = "0.0.0.0";
                dhorn::ipv4_address addr1(ip1);
                Assert::AreEqual(((in_addr)addr1).s_addr, 0ul);
                Assert::IsFalse(addr1.fail());

                std::string ip2 = "255.255.255.255";
                dhorn::ipv4_address addr2(ip2);
                Assert::AreEqual(((in_addr)addr2).s_addr, 0xFFFFFFFFul);
                Assert::IsFalse(addr2.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "127.0.0.1";
                dhorn::ipv4_address addr3(ip3);
                Assert::AreEqual(((in_addr)addr3).s_addr, 0x0100007Ful);
                Assert::IsFalse(addr3.fail());

                std::string ip4 = "2.0.0.10";
                dhorn::ipv4_address addr4(ip4);
                Assert::AreEqual(((in_addr)addr4).s_addr, 0x0A000002ul);
                Assert::IsFalse(addr4.fail());

                std::string ip5 = "foo.bar";
                dhorn::ipv4_address addr5(ip5);
                Assert::IsTrue(addr5.fail());

                std::string ip6;
                dhorn::ipv4_address addr6(ip6);
                Assert::IsTrue(addr6.fail());
            }

            TEST_METHOD(StdWStringConstructorTest)
            {
                std::wstring ip1 = L"0.0.0.0";
                dhorn::ipv4_address addr1(ip1);
                Assert::AreEqual(((in_addr)addr1).s_addr, 0ul);
                Assert::IsFalse(addr1.fail());

                std::wstring ip2 = L"255.255.255.255";
                dhorn::ipv4_address addr2(ip2);
                Assert::AreEqual(((in_addr)addr2).s_addr, 0xFFFFFFFFul);
                Assert::IsFalse(addr2.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"127.0.0.1";
                dhorn::ipv4_address addr3(ip3);
                Assert::AreEqual(((in_addr)addr3).s_addr, 0x0100007Ful);
                Assert::IsFalse(addr3.fail());

                std::wstring ip4 = L"2.0.0.10";
                dhorn::ipv4_address addr4(ip4);
                Assert::AreEqual(((in_addr)addr4).s_addr, 0x0A000002ul);
                Assert::IsFalse(addr4.fail());

                std::wstring ip5 = L"foo.bar";
                dhorn::ipv4_address addr5(ip5);
                Assert::IsTrue(addr5.fail());

                std::wstring ip6;
                dhorn::ipv4_address addr6(ip6);
                Assert::IsTrue(addr6.fail());
            }

#pragma endregion



            /*
             * Assignment Tests
             */
#pragma region Assignment Tests

            TEST_METHOD(IpAddrAssignmentTest)
            {
                dhorn::ipv4_address ip;
                in_addr addr;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    addr.s_net = rand() % 256;
                    addr.s_host = rand() % 256;
                    addr.s_lh = rand() % 256;
                    addr.s_impno = rand() % 256;

                    ip = addr;
                    Assert::AreEqual(((in_addr)ip).s_addr, addr.s_addr);
                    Assert::IsFalse(ip.fail());
                }

                addr.s_addr = 0xFFFFFFFF;
                ip = addr;
                Assert::AreEqual(((in_addr)ip).s_addr, addr.s_addr);
                Assert::IsFalse(ip.fail());

                addr.s_addr = 0;
                ip = addr;
                Assert::AreEqual(((in_addr)ip).s_addr, addr.s_addr);
                Assert::IsFalse(ip.fail());
            }

            TEST_METHOD(StringAssignmentTest)
            {
                dhorn::ipv4_address addr;
                dhorn::ipv4_address addr2;

                std::string ip1 = "0.0.0.0";
                addr = ip1.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0ul);
                Assert::IsFalse(addr.fail());

                std::string ip2 = "255.255.255.255";
                addr = ip2.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0xFFFFFFFFul);
                Assert::IsFalse(addr.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "127.0.0.1";
                addr = ip3.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0100007Ful);
                Assert::IsFalse(addr.fail());

                std::string ip4 = "2.0.0.10";
                addr = ip4.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0A000002ul);
                Assert::IsFalse(addr.fail());

                std::string ip5 = "foo.bar";
                addr = ip5.c_str();
                Assert::IsTrue(addr.fail());

#pragma warning(push)
#pragma warning(disable:6387)
                addr2 = (char *)nullptr;
#pragma warning(pop)
                Assert::IsTrue(addr2.fail());
            }

            TEST_METHOD(WStringAssignmentTest)
            {
                dhorn::ipv4_address addr;
                dhorn::ipv4_address addr2;

                std::wstring ip1 = L"0.0.0.0";
                addr = ip1.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0ul);
                Assert::IsFalse(addr.fail());

                std::wstring ip2 = L"255.255.255.255";
                addr = ip2.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0xFFFFFFFFul);
                Assert::IsFalse(addr.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"127.0.0.1";
                addr = ip3.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0100007Ful);
                Assert::IsFalse(addr.fail());

                std::wstring ip4 = L"2.0.0.10";
                addr = ip4.c_str();
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0A000002ul);
                Assert::IsFalse(addr.fail());

                std::wstring ip5 = L"foo.bar";
                addr = ip5.c_str();
                Assert::IsTrue(addr.fail());

#pragma warning(push)
#pragma warning(disable:6387)
                addr2 = (wchar_t *)nullptr;
#pragma warning(pop)
                Assert::IsTrue(addr2.fail());
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                dhorn::ipv4_address addr;
                dhorn::ipv4_address addr2;

                std::string ip1 = "0.0.0.0";
                addr = ip1;
                Assert::AreEqual(((in_addr)addr).s_addr, 0ul);
                Assert::IsFalse(addr.fail());

                std::string ip2 = "255.255.255.255";
                addr = ip2;
                Assert::AreEqual(((in_addr)addr).s_addr, 0xFFFFFFFFul);
                Assert::IsFalse(addr.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "127.0.0.1";
                addr = ip3;
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0100007Ful);
                Assert::IsFalse(addr.fail());

                std::string ip4 = "2.0.0.10";
                addr = ip4;
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0A000002ul);
                Assert::IsFalse(addr.fail());

                std::string ip5 = "foo.bar";
                addr = ip5;
                Assert::IsTrue(addr.fail());

                addr2 = std::string();
                Assert::IsTrue(addr2.fail());
            }

            TEST_METHOD(StdWStringAssignmentTest)
            {
                dhorn::ipv4_address addr;
                dhorn::ipv4_address addr2;

                std::wstring ip1 = L"0.0.0.0";
                addr = ip1;
                Assert::AreEqual(((in_addr)addr).s_addr, 0ul);
                Assert::IsFalse(addr.fail());

                std::wstring ip2 = L"255.255.255.255";
                addr = ip2;
                Assert::AreEqual(((in_addr)addr).s_addr, 0xFFFFFFFFul);
                Assert::IsFalse(addr.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"127.0.0.1";
                addr = ip3;
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0100007Ful);
                Assert::IsFalse(addr.fail());

                std::wstring ip4 = L"2.0.0.10";
                addr = ip4;
                Assert::AreEqual(((in_addr)addr).s_addr, 0x0A000002ul);
                Assert::IsFalse(addr.fail());

                std::wstring ip5 = L"foo.bar";
                addr = ip5;
                Assert::IsTrue(addr.fail());

                addr2 = std::wstring();
                Assert::IsTrue(addr2.fail());
            }

#pragma endregion



            /*
             * Type Conversion Tests
             */
#pragma region Type Conversion Tests

            TEST_METHOD(IpAddrConversionTest)
            {
                dhorn::ipv4_address addr;
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
                    dhorn::ipv4_address addr(str);
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
                    dhorn::ipv4_address addr(str);
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
                dhorn::ipv4_address addr;
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
                    dhorn::ipv4_address addr(str);
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
                    dhorn::ipv4_address addr(str);
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
                dhorn::ipv6_address addr;
                Assert::IsTrue((in_addr6)addr == zero());
                Assert::IsFalse(addr.fail());
            }

            TEST_METHOD(IpAddrConstructorTest)
            {
                in_addr6 addr;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    addr = random_addr();

                    dhorn::ipv6_address ip(addr);
                    Assert::IsTrue(addr == ip);
                    Assert::IsFalse(ip.fail());
                }

                addr = make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
                dhorn::ipv6_address ip1(addr);
                Assert::IsTrue(addr == ip1);
                Assert::IsFalse(ip1.fail());

                addr = zero();
                dhorn::ipv6_address ip2(addr);
                Assert::IsTrue(addr == ip2);
                Assert::IsFalse(ip2.fail());
            }

            TEST_METHOD(StringConstructorTest)
            {
                std::string ip1 = "::";
                dhorn::ipv6_address addr1(ip1.c_str());
                Assert::IsTrue(zero() == addr1);
                Assert::IsFalse(addr1.fail());

                std::string ip2 = "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                dhorn::ipv6_address addr2(ip2.c_str());
                Assert::IsTrue(make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) == addr2);
                Assert::IsFalse(addr2.fail());

                // Note: on x86/amd64 machines, the byte order in each word should be reversed
                std::string ip3 = "::1";
                dhorn::ipv6_address addr3(ip3.c_str());
                Assert::IsTrue(make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100) == addr3);
                Assert::IsFalse(addr3.fail());

                std::string ip4 = "0123:4567::aaaa:bbbb";
                dhorn::ipv6_address addr4(ip4.c_str());
                Assert::IsTrue(make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb) == addr4);
                Assert::IsFalse(addr4.fail());

                std::string ip5 = "foo::bar";
                dhorn::ipv6_address addr5(ip5.c_str());
                Assert::IsTrue(addr5.fail());

#pragma warning(push)
#pragma warning(disable:6387)
                dhorn::ipv6_address addr6((char *)nullptr);
#pragma warning(pop)
                Assert::IsTrue(addr6.fail());
            }

            TEST_METHOD(WStringConstructorTest)
            {
                std::wstring ip1 = L"::";
                dhorn::ipv6_address addr1(ip1.c_str());
                Assert::IsTrue(zero() == addr1);
                Assert::IsFalse(addr1.fail());

                std::wstring ip2 = L"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                dhorn::ipv6_address addr2(ip2.c_str());
                Assert::IsTrue(make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) == addr2);
                Assert::IsFalse(addr2.fail());

                // Note: on x86/amd64 machines, the byte order in each word should be reversed
                std::wstring ip3 = L"::1";
                dhorn::ipv6_address addr3(ip3.c_str());
                Assert::IsTrue(make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100) == addr3);
                Assert::IsFalse(addr3.fail());

                std::wstring ip4 = L"0123:4567::aaaa:bbbb";
                dhorn::ipv6_address addr4(ip4.c_str());
                Assert::IsTrue(make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb) == addr4);
                Assert::IsFalse(addr4.fail());

                std::wstring ip5 = L"foo::bar";
                dhorn::ipv6_address addr5(ip5.c_str());
                Assert::IsTrue(addr5.fail());

#pragma warning(push)
#pragma warning(disable:6387)
                dhorn::ipv6_address addr6((char *)nullptr);
#pragma warning(pop)
                Assert::IsTrue(addr6.fail());
            }

            TEST_METHOD(StdStringConstructorTest)
            {
                std::string ip1 = "::";
                dhorn::ipv6_address addr1(ip1);
                Assert::IsTrue(zero() == addr1);
                Assert::IsFalse(addr1.fail());

                std::string ip2 = "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                dhorn::ipv6_address addr2(ip2);
                Assert::IsTrue(make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) == addr2);
                Assert::IsFalse(addr2.fail());

                // Note: on x86/amd64 machines, the byte order in each word should be reversed
                std::string ip3 = "::1";
                dhorn::ipv6_address addr3(ip3);
                Assert::IsTrue(make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100) == addr3);
                Assert::IsFalse(addr3.fail());

                std::string ip4 = "0123:4567::aaaa:bbbb";
                dhorn::ipv6_address addr4(ip4);
                Assert::IsTrue(make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb) == addr4);
                Assert::IsFalse(addr4.fail());

                std::string ip5 = "foo::bar";
                dhorn::ipv6_address addr5(ip5);
                Assert::IsTrue(addr5.fail());

                std::string ip6;
                dhorn::ipv6_address addr6(ip6);
                Assert::IsTrue(addr6.fail());
            }

            TEST_METHOD(StdWStringConstructorTest)
            {
                std::wstring ip1 = L"::";
                dhorn::ipv6_address addr1(ip1);
                Assert::IsTrue(zero() == addr1);
                Assert::IsFalse(addr1.fail());

                std::wstring ip2 = L"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                dhorn::ipv6_address addr2(ip2);
                Assert::IsTrue(make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) == addr2);
                Assert::IsFalse(addr2.fail());

                // Note: on x86/amd64 machines, the byte order in each word should be reversed
                std::wstring ip3 = L"::1";
                dhorn::ipv6_address addr3(ip3);
                Assert::IsTrue(make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100) == addr3);
                Assert::IsFalse(addr3.fail());

                std::wstring ip4 = L"0123:4567::aaaa:bbbb";
                dhorn::ipv6_address addr4(ip4);
                Assert::IsTrue(make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb) == addr4);
                Assert::IsFalse(addr4.fail());

                std::wstring ip5 = L"foo::bar";
                dhorn::ipv6_address addr5(ip5);
                Assert::IsTrue(addr5.fail());

                std::wstring ip6;
                dhorn::ipv6_address addr6(ip6);
                Assert::IsTrue(addr6.fail());
            }

#pragma endregion



            /*
             * Assignment Tests
             */
#pragma region Assignment Tests

            TEST_METHOD(IpAddrAssignmentTest)
            {
                dhorn::ipv6_address ip;
                in6_addr addr;

                for (int i = 0; i < TEST_COUNT; i++)
                {
                    addr = random_addr();

                    ip = addr;
                    Assert::IsTrue(ip == addr);
                    Assert::IsFalse(ip.fail());
                }

                addr = make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
                ip = addr;
                Assert::IsTrue(ip == addr);
                Assert::IsFalse(ip.fail());

                addr = zero();
                ip = addr;
                Assert::IsTrue(ip == addr);
                Assert::IsFalse(ip.fail());
            }

            TEST_METHOD(StringAssignmentTest)
            {
                dhorn::ipv6_address addr;
                dhorn::ipv6_address addr2;

                std::string ip1 = "::";
                addr = ip1.c_str();
                Assert::IsTrue(addr == zero());
                Assert::IsFalse(addr.fail());

                std::string ip2 = "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                addr = ip2.c_str();
                Assert::IsTrue(addr == make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF));
                Assert::IsFalse(addr.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "::1";
                addr = ip3.c_str();
                Assert::IsTrue(addr == make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100));
                Assert::IsFalse(addr.fail());

                std::string ip4 = "0123:4567::aaaa:bbbb";
                addr = ip4.c_str();
                Assert::IsTrue(addr == make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb));
                Assert::IsFalse(addr.fail());

                std::string ip5 = "foo::bar";
                addr = ip5.c_str();
                Assert::IsTrue(addr.fail());

#pragma warning(push)
#pragma warning(disable:6387)
                addr2 = (char *)nullptr;
#pragma warning(pop)
                Assert::IsTrue(addr2.fail());
            }

            TEST_METHOD(WStringAssignmentTest)
            {
                dhorn::ipv6_address addr;
                dhorn::ipv6_address addr2;

                std::wstring ip1 = L"::";
                addr = ip1.c_str();
                Assert::IsTrue(addr == zero());
                Assert::IsFalse(addr.fail());

                std::wstring ip2 = L"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                addr = ip2.c_str();
                Assert::IsTrue(addr == make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF));
                Assert::IsFalse(addr.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"::1";
                addr = ip3.c_str();
                Assert::IsTrue(addr == make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100));
                Assert::IsFalse(addr.fail());

                std::wstring ip4 = L"0123:4567::aaaa:bbbb";
                addr = ip4.c_str();
                Assert::IsTrue(addr == make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb));
                Assert::IsFalse(addr.fail());

                std::wstring ip5 = L"foo::bar";
                addr = ip5.c_str();
                Assert::IsTrue(addr.fail());

#pragma warning(push)
#pragma warning(disable:6387)
                addr2 = (wchar_t *)nullptr;
#pragma warning(pop)
                Assert::IsTrue(addr2.fail());
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                dhorn::ipv6_address addr;
                dhorn::ipv6_address addr2;

                std::string ip1 = "::";
                addr = ip1;
                Assert::IsTrue(addr == zero());
                Assert::IsFalse(addr.fail());

                std::string ip2 = "FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                addr = ip2;
                Assert::IsTrue(addr == make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF));
                Assert::IsFalse(addr.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::string ip3 = "::1";
                addr = ip3;
                Assert::IsTrue(addr == make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100));
                Assert::IsFalse(addr.fail());

                std::string ip4 = "0123:4567::aaaa:bbbb";
                addr = ip4;
                Assert::IsTrue(addr == make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb));
                Assert::IsFalse(addr.fail());

                std::string ip5 = "foo::bar";
                addr = ip5;
                Assert::IsTrue(addr.fail());

                std::string ip6;
                addr2 = ip6;
                Assert::IsTrue(addr2.fail());
            }

            TEST_METHOD(StdWStringAssignmentTest)
            {
                dhorn::ipv6_address addr;
                dhorn::ipv6_address addr2;

                std::wstring ip1 = L"::";
                addr = ip1;
                Assert::IsTrue(addr == zero());
                Assert::IsFalse(addr.fail());

                std::wstring ip2 = L"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF";
                addr = ip2;
                Assert::IsTrue(addr == make_addr(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF));
                Assert::IsFalse(addr.fail());

                // Note: on x86/amd64 machines, the ip address should be stored in "reverse"
                std::wstring ip3 = L"::1";
                addr = ip3;
                Assert::IsTrue(addr == make_addr(0, 0, 0, 0, 0, 0, 0, 0x0100));
                Assert::IsFalse(addr.fail());

                std::wstring ip4 = L"0123:4567::aaaa:bbbb";
                addr = ip4;
                Assert::IsTrue(addr == make_addr(0x2301, 0x6745, 0, 0, 0, 0, 0xaaaa, 0xbbbb));
                Assert::IsFalse(addr.fail());

                std::wstring ip5 = L"foo::bar";
                addr = ip5;
                Assert::IsTrue(addr.fail());

                std::wstring ip6;
                addr2 = ip6;
                Assert::IsTrue(addr2.fail());
            }

#pragma endregion



            /*
             * Type Conversion Tests
             */
#pragma region Type Conversion Tests

            TEST_METHOD(IpAddrConversionTest)
            {
                dhorn::ipv6_address addr;
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
                    dhorn::ipv6_address addr(str);
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
                    dhorn::ipv6_address addr(str);
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
                dhorn::ipv6_address addr;
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
                    dhorn::ipv6_address addr(str);
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
                    dhorn::ipv6_address addr(str);
                    Assert::AreEqual(addr.str<wchar_t>(), str);
                }
            }

#pragma endregion
        };
    }
}
