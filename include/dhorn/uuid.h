/*
 * Duncan Horn
 *
 * uuid.h
 *
 * A simple UUID implementation. It is important to note that this IS different than Windows' GUID implementation in
 * terms of byte order; uuids are in network-byte-order. The total number of bits is the same however, so the two types
 * CAN be converted; they'll just represent different uuids, though the internal data will remain the same.
 */
#pragma once

#include <array>
#include <cstdint>
#include <type_traits>

#ifdef WIN32
#include <Windows.h>
#endif

// On debug, set uuid to {CCCCCCCC-CCCC-CCCC-CCCC-CCCCCCCCCCCC}
#ifndef DHORN_DEBUG_UUID
#define DHORN_DEBUG_UUID        (0xCCCCCCCC)
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4351)       // Default array initialization
#pragma warning(disable:4201)       // Nameless struct/union
#endif

namespace dhorn
{
    class uuid
    {
    public:
        // Data can be publicly accessed. This is defined as a union for easier use as GUID, etc.
        union
        {
            uint8_t  data[16];
            uint32_t data32[4];
            struct
            {
                // Same definition as Windows' GUID
                uint32_t Data1;
                uint16_t Data2;
                uint16_t Data3;
                uint8_t  Data4[8];
            };
        };



        /*
         * Constructor(s)/Destructor
         */
        uuid(void) :
            data{}
        {
        }

        template <size_t N, typename = std::enable_if_t<N == 16>>
        uuid(const uint8_t (&arr)[N])
        {
            const uint32_t *target = reinterpret_cast<const uint32_t *>(arr);

            for (size_t i = 0; i < 4; i++)
            {
                this->data32[i] = target[i];
            }
        }

        template <size_t N, typename = std::enable_if_t<N == 4>>
        uuid(const uint32_t (&arr)[N])
        {
            for (size_t i = 0; i < 4; i++)
            {
                this->data32[i] = arr[i];
            }
        }

        // We can't use std::initializer_list because we accept two separate types that can't easily be differentiated
        // in client code, and we are very strict on the size of the initialization list we accept
        uuid(uint8_t val0,  uint8_t val1,  uint8_t val2,  uint8_t val3,
             uint8_t val4,  uint8_t val5,  uint8_t val6,  uint8_t val7,
             uint8_t val8,  uint8_t val9,  uint8_t val10, uint8_t val11,
             uint8_t val12, uint8_t val13, uint8_t val14, uint8_t val15) :
             data{ val0, val1, val2,  val3,  val4,  val5,  val6,  val7,
                   val8, val9, val10, val11, val12, val13, val14, val15 }
        {
        }

        uuid(uint32_t val0, uint32_t val1, uint32_t val2, uint32_t val3) :
            data32{ val0, val1, val2, val3 }
        {
        }

        //template <int N, typename = std::enable_if<N == 8>::type>
        uuid(uint32_t val0, uint16_t val1, uint16_t val2, const std::array<uint8_t, 8> &val3) :
            Data1(val0),
            Data2(val1),
            Data3(val2)
        {
            for (size_t i = 0; i < 8; i++)
            {
                this->Data4[i] = val3[i];
            }
        }

#ifdef WIN32

        uuid(const GUID &guid) :
            Data1(guid.Data1),
            Data2(guid.Data2),
            Data3(guid.Data3)
        {
            // Copy Data4 using a 64-bit integer for performance
            auto &to = reinterpret_cast<uint64_t &>(*this->Data4);
            auto &from = reinterpret_cast<const uint64_t &>(*guid.Data4);

            to = from;
        }

#endif

#ifdef _DEBUG
        ~uuid(void)
        {
            // On debug, set uuid to {CCCCCCCC-CCCC-CCCC-CCCC-CCCCCCCCCCCC}
            data32[0] = DHORN_DEBUG_UUID;
            data32[1] = DHORN_DEBUG_UUID;
            data32[2] = DHORN_DEBUG_UUID;
            data32[3] = DHORN_DEBUG_UUID;
        }
#endif



        /*
         * Operators
         */
        inline bool operator==(const uuid &other) const
        {
            return this->Compare(other) == 0;
        }

        inline bool operator<(const uuid &other) const
        {
            return this->Compare(other) < 0;
        }

        inline bool operator>(const uuid &other) const
        {
            return this->Compare(other) > 0;
        }

        inline bool operator!=(const uuid &other) const { return !(*this == other); }
        inline bool operator>=(const uuid &other) const { return !(*this < other); }
        inline bool operator<=(const uuid &other) const { return !(*this > other); }



        /*
         * Provide an easy to use hash function for use with hash tables, etc.
         */
        size_t hash(void) const
        {
            size_t hash = 0;

            for (size_t i = 0; i < 4; i++)
            {
                // Mulitply previous value by 31 (a prime number)
                hash = (hash << 5) - hash;
                hash += this->data32[i];
            }

            return hash;
        }



    private:

        inline int Compare(const uuid &other) const
        {
            for (size_t i = 0; i < 4; i++)
            {
                int diff = this->data32[i] - other.data32[i];
                if (diff)
                {
                    return diff;
                }
            }

            return 0;
        }
    };
}

#ifdef WIN32
static_assert(sizeof(dhorn::uuid) == sizeof(GUID), "dhorn::uuid must be same size as GUID");
#endif



#ifndef DHORN_NO_STD

namespace std
{
    template <>
    struct hash<dhorn::uuid>
    {
        using argument_type = dhorn::uuid;
        using result_type = size_t;

        size_t operator()(const dhorn::uuid &obj)
        {
            return obj.hash();
        }
    };
}

#endif



#ifdef _MSC_VER
#pragma warning(pop)
#endif
