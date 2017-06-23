/*
 * Duncan Horn
 *
 * NetTests.cpp
 *
 * Tests for the net.h header file
 */
#include "stdafx.h"

#include <dhorn/net.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(ByteOrderTests)
    {
        template <typename Ty>
        union combine
        {
            Ty value;
            uint8_t bytes[sizeof(Ty)];
        };

        TEST_METHOD(HostToNetworkUnsigned32BitTest)
        {
            combine<uint32_t> result;
            result.value = host_to_network(static_cast<uint32_t>(0));
            Assert::AreEqual(static_cast<uint8_t>(0), result.bytes[0]);
            Assert::AreEqual(static_cast<uint8_t>(0), result.bytes[1]);
            Assert::AreEqual(static_cast<uint8_t>(0), result.bytes[2]);
            Assert::AreEqual(static_cast<uint8_t>(0), result.bytes[3]);

            result.value = host_to_network(static_cast<uint32_t>(0xFFFFFFFF));
            Assert::AreEqual(static_cast<uint8_t>(0xFF), result.bytes[0]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), result.bytes[1]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), result.bytes[2]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), result.bytes[3]);

            result.value = host_to_network(static_cast<uint32_t>(0xAABBCCDD));
            Assert::AreEqual(static_cast<uint8_t>(0xAA), result.bytes[0]);
            Assert::AreEqual(static_cast<uint8_t>(0xBB), result.bytes[1]);
            Assert::AreEqual(static_cast<uint8_t>(0xCC), result.bytes[2]);
            Assert::AreEqual(static_cast<uint8_t>(0xDD), result.bytes[3]);

            result.value = host_to_network(static_cast<uint32_t>(0x11223344));
            Assert::AreEqual(static_cast<uint8_t>(0x11), result.bytes[0]);
            Assert::AreEqual(static_cast<uint8_t>(0x22), result.bytes[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x33), result.bytes[2]);
            Assert::AreEqual(static_cast<uint8_t>(0x44), result.bytes[3]);

            result.value = host_to_network(static_cast<uint32_t>(0xAABB1122));
            Assert::AreEqual(static_cast<uint8_t>(0xAA), result.bytes[0]);
            Assert::AreEqual(static_cast<uint8_t>(0xBB), result.bytes[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x11), result.bytes[2]);
            Assert::AreEqual(static_cast<uint8_t>(0x22), result.bytes[3]);
        }

        TEST_METHOD(HostToNetworkSigned32BitTest)
        {
            combine<int32_t> result;
            result.value = host_to_network(static_cast<int32_t>(0));
            Assert::AreEqual(static_cast<uint8_t>(0), result.bytes[0]);
            Assert::AreEqual(static_cast<uint8_t>(0), result.bytes[1]);
            Assert::AreEqual(static_cast<uint8_t>(0), result.bytes[2]);
            Assert::AreEqual(static_cast<uint8_t>(0), result.bytes[3]);

            result.value = host_to_network(static_cast<int32_t>(0xFFFFFFFF));
            Assert::AreEqual(static_cast<uint8_t>(0xFF), result.bytes[0]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), result.bytes[1]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), result.bytes[2]);
            Assert::AreEqual(static_cast<uint8_t>(0xFF), result.bytes[3]);

            result.value = host_to_network(static_cast<int32_t>(0xAABBCCDD));
            Assert::AreEqual(static_cast<uint8_t>(0xAA), result.bytes[0]);
            Assert::AreEqual(static_cast<uint8_t>(0xBB), result.bytes[1]);
            Assert::AreEqual(static_cast<uint8_t>(0xCC), result.bytes[2]);
            Assert::AreEqual(static_cast<uint8_t>(0xDD), result.bytes[3]);

            result.value = host_to_network(static_cast<int32_t>(0x11223344));
            Assert::AreEqual(static_cast<uint8_t>(0x11), result.bytes[0]);
            Assert::AreEqual(static_cast<uint8_t>(0x22), result.bytes[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x33), result.bytes[2]);
            Assert::AreEqual(static_cast<uint8_t>(0x44), result.bytes[3]);

            result.value = host_to_network(static_cast<int32_t>(0xAABB1122));
            Assert::AreEqual(static_cast<uint8_t>(0xAA), result.bytes[0]);
            Assert::AreEqual(static_cast<uint8_t>(0xBB), result.bytes[1]);
            Assert::AreEqual(static_cast<uint8_t>(0x11), result.bytes[2]);
            Assert::AreEqual(static_cast<uint8_t>(0x22), result.bytes[3]);
        }

        TEST_METHOD(HostToNetworkUnsigned8BitTest)
        {
            uint8_t result;
            result = host_to_network(static_cast<uint8_t>(0));
            Assert::AreEqual(static_cast<uint8_t>(0), result);

            result = host_to_network(static_cast<uint8_t>(0xFF));
            Assert::AreEqual(static_cast<uint8_t>(0xFF), result);

            result = host_to_network(static_cast<uint8_t>(0xAA));
            Assert::AreEqual(static_cast<uint8_t>(0xAA), result);

            result = host_to_network(static_cast<uint8_t>(0x11));
            Assert::AreEqual(static_cast<uint8_t>(0x11), result);
        }

        TEST_METHOD(HostToNetworkSigned8BitTest)
        {
            int8_t result;
            result = host_to_network(static_cast<int8_t>(0));
            Assert::AreEqual(static_cast<int8_t>(0), result);

            result = host_to_network(static_cast<int8_t>(static_cast<uint8_t>(0xFF)));
            Assert::AreEqual(static_cast<int8_t>(static_cast<uint8_t>(0xFF)), result);

            result = host_to_network(static_cast<int8_t>(static_cast<uint8_t>(0xAA)));
            Assert::AreEqual(static_cast<int8_t>(static_cast<uint8_t>(0xAA)), result);

            result = host_to_network(static_cast<int8_t>(0x11));
            Assert::AreEqual(static_cast<int8_t>(0x11), result);
        }

        TEST_METHOD(NetworkToHostUnsigned32BitTest)
        {
            combine<uint32_t> result;
            auto apply = [&](uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3)
            {
                result.bytes[0] = v0;
                result.bytes[1] = v1;
                result.bytes[2] = v2;
                result.bytes[3] = v3;
            };

            apply(0, 0, 0, 0);
            Assert::AreEqual(static_cast<uint32_t>(0), network_to_host(result.value));

            apply(0xFF, 0xFF, 0xFF, 0xFF);
            Assert::AreEqual(static_cast<uint32_t>(0xFFFFFFFF), network_to_host(result.value));

            apply(0xAA, 0xBB, 0xCC, 0xDD);
            Assert::AreEqual(static_cast<uint32_t>(0xAABBCCDD), network_to_host(result.value));

            apply(0x11, 0x22, 0x33, 0x44);
            Assert::AreEqual(static_cast<uint32_t>(0x11223344), network_to_host(result.value));

            apply(0xAA, 0xBB, 0x11, 0x22);
            Assert::AreEqual(static_cast<uint32_t>(0xAABB1122), network_to_host(result.value));
        }

        TEST_METHOD(NetworkToHostSigned32BitTest)
        {
            combine<int32_t> result;
            auto apply = [&](uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3)
            {
                result.bytes[0] = v0;
                result.bytes[1] = v1;
                result.bytes[2] = v2;
                result.bytes[3] = v3;
            };

            apply(0, 0, 0, 0);
            Assert::AreEqual(static_cast<int32_t>(0), network_to_host(result.value));

            apply(0xFF, 0xFF, 0xFF, 0xFF);
            Assert::AreEqual(static_cast<int32_t>(0xFFFFFFFF), network_to_host(result.value));

            apply(0xAA, 0xBB, 0xCC, 0xDD);
            Assert::AreEqual(static_cast<int32_t>(0xAABBCCDD), network_to_host(result.value));

            apply(0x11, 0x22, 0x33, 0x44);
            Assert::AreEqual(static_cast<int32_t>(0x11223344), network_to_host(result.value));

            apply(0xAA, 0xBB, 0x11, 0x22);
            Assert::AreEqual(static_cast<int32_t>(0xAABB1122), network_to_host(result.value));
        }

        TEST_METHOD(NetworkToHostUnsigned8BitTest)
        {
            uint8_t result;
            result = network_to_host(static_cast<uint8_t>(0));
            Assert::AreEqual(static_cast<uint8_t>(0), result);

            result = network_to_host(static_cast<uint8_t>(0xFF));
            Assert::AreEqual(static_cast<uint8_t>(0xFF), result);

            result = network_to_host(static_cast<uint8_t>(0xAA));
            Assert::AreEqual(static_cast<uint8_t>(0xAA), result);

            result = network_to_host(static_cast<uint8_t>(0x11));
            Assert::AreEqual(static_cast<uint8_t>(0x11), result);
        }

        TEST_METHOD(NetworkToHostSigned8BitTest)
        {
            int8_t result;
            result = network_to_host(static_cast<int8_t>(0));
            Assert::AreEqual(static_cast<int8_t>(0), result);

            result = network_to_host(static_cast<int8_t>(static_cast<uint8_t>(0xFF)));
            Assert::AreEqual(static_cast<int8_t>(static_cast<uint8_t>(0xFF)), result);

            result = network_to_host(static_cast<int8_t>(static_cast<uint8_t>(0xAA)));
            Assert::AreEqual(static_cast<int8_t>(static_cast<uint8_t>(0xAA)), result);

            result = network_to_host(static_cast<int8_t>(0x11));
            Assert::AreEqual(static_cast<int8_t>(0x11), result);
        }
    };
}
