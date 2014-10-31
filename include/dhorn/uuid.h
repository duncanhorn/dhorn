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

#ifdef  WIN32
#include <Windows.h>
#endif  /*WIN32*/

// On debug, set uuid to {CCCCCCCC-CCCC-CCCC-CCCC-CCCCCCCCCCCC}
#ifndef _DHORN_DEBUG_UUID
#define _DHORN_DEBUG_UUID       (0xCCCCCCCC)
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
        // Data can be publically accessed. This is defined as a union for easier use as GUID, etc.
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
            data{}      // zero init
        {
        }

        template <size_t N, typename = typename std::enable_if<N == 16>::type>
        uuid(_In_ const uint8_t (&arr)[N])
        {
            const uint32_t *target = reinterpret_cast<const uint32_t *>(arr);

            for (size_t i = 0; i < 4; i++)
            {
                this->data32[i] = target[i];
            }
        }

        template <size_t N, typename = typename std::enable_if<N == 4>::type>
        uuid(_In_ const uint32_t (&arr)[N])
        {
            for (size_t i = 0; i < 4; i++)
            {
                this->data32[i] = arr[i];
            }
        }

        // We can't use std::initializer_list because we accept two separate types that can't easily be differentiated
        // in client code, and we are very strict on the size of the initialization list we accept
        uuid(_In_ uint8_t val0,  _In_ uint8_t val1,  _In_ uint8_t val2,  _In_ uint8_t val3,
             _In_ uint8_t val4,  _In_ uint8_t val5,  _In_ uint8_t val6,  _In_ uint8_t val7,
             _In_ uint8_t val8,  _In_ uint8_t val9,  _In_ uint8_t val10, _In_ uint8_t val11,
             _In_ uint8_t val12, _In_ uint8_t val13, _In_ uint8_t val14, _In_ uint8_t val15)// :
//             data{ val0, val1, val2,  val3,  val4,  val5,  val6,  val7,
//                   val8, val9, val10, val11, val12, val13, val14, val15 }
        {
            // TODO: switch to std::array once C++14 is adopted and the duplicated braces are no longer needed
            this->data[0]  = val0;  this->data[1]  = val1;  this->data[2]  = val2;
            this->data[3]  = val3;  this->data[4]  = val4;  this->data[5]  = val5;
            this->data[6]  = val6;  this->data[7]  = val7;  this->data[8]  = val8;
            this->data[9]  = val9;  this->data[10] = val10; this->data[11] = val11;
            this->data[12] = val12; this->data[13] = val13; this->data[14] = val14;
            this->data[15] = val15;
        }

        uuid(_In_ uint32_t val0, _In_ uint32_t val1, _In_ uint32_t val2, _In_ uint32_t val3)// :
//            data32{ val0, val1, val2, val3 }
        {
            data32[0] = val0;
            data32[1] = val1;
            data32[2] = val2;
            data32[3] = val3;
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

#ifdef  WIN32

        uuid(_In_ const GUID &guid) :
            Data1(guid.Data1),
            Data2(guid.Data2),
            Data3(guid.Data3)
        {
            // Copy Data4 using a 64-bit integer for performance
            auto &to = reinterpret_cast<uint64_t &>(*this->Data4);
            auto &from = reinterpret_cast<const uint64_t &>(*guid.Data4);

            to = from;
        }

#endif  /*WIN32*/

#ifdef _DEBUG
        ~uuid(void)
        {
            // On debug, set uuid to {CCCCCCCC-CCCC-CCCC-CCCC-CCCCCCCCCCCC}
            data32[0] = _DHORN_DEBUG_UUID;
            data32[1] = _DHORN_DEBUG_UUID;
            data32[2] = _DHORN_DEBUG_UUID;
            data32[3] = _DHORN_DEBUG_UUID;
        }
#endif



        /*
         * Operators
         */
        inline bool operator==(_In_ const uuid &other) const
        {
            return this->Compare(other) == 0;
        }

        inline bool operator<(_In_ const uuid &other) const
        {
            return this->Compare(other) < 0;
        }

        inline bool operator>(_In_ const uuid &other) const
        {
            return this->Compare(other) > 0;
        }

        inline bool operator!=(_In_ const uuid &other) const { return !(*this == other); }
        inline bool operator>=(_In_ const uuid &other) const { return !(*this < other); }
        inline bool operator<=(_In_ const uuid &other) const { return !(*this > other); }



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

        inline int Compare(_In_ const uuid &other) const
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

#ifdef  WIN32
static_assert(sizeof(dhorn::uuid) == sizeof(GUID), "dhorn::uuid must be same size as GUID");
#endif



#ifndef _DHORN_NO_STD

namespace std
{
    template <>
    struct hash<dhorn::uuid>
    {
        using argument_type = dhorn::uuid;
        using result_type = size_t;

        size_t operator()(_In_ const dhorn::uuid &obj)
        {
            return obj.hash();
        }
    };
}

#endif  /*_DHORN_NO_STD*/



#ifdef _MSC_VER
#pragma warning(pop)
#endif
