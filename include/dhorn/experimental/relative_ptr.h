/*
 * Duncan Horn
 *
 * relative_ptr.h
 *
 * 
 */
#pragma once

#include <cstddef>
#include <cstdint>

// TODO: Make a decision about alignment

namespace dhorn::experimental
{
    template <typename Ty, typename OffsetTy>
    class relative_ptr
    {
    public:
        /*
         * std::iterator_traits Type Alias Support
         */
        using difference_type = std::ptrdiff_t; // TODO: Relate to OffsetTy?
        using value_type = Ty;
        using pointer = Ty*;
        using reference = Ty&;



    private:

        Ty& get() noexcept
        {
            auto bytePtr = reinterpret_cast<std::uint8_t*>(this);
            bytePtr += this->_offset;
            return *reinterpret_cast<Ty*>(bytePtr);
        }

        const Ty& get() const noexcept
        {
            auto bytePtr = reinterpret_cast<const std::uint8_t*>(this);
            bytePtr += this->_offset;
            return *reinterpret_cast<Ty*>(bytePtr);
        }



        OffsetTy _offset;
    };



    /*
     * Public Aliases
     */
    template <typename Ty>
    using relative_ptr8 = relative_ptr<Ty, std::int8_t>;

    template <typename Ty>
    using relative_ptr16 = relative_ptr<Ty, std::int16_t>;

    template <typename Ty>
    using relative_ptr32 = relative_ptr<Ty, std::int32_t>;

    template <typename Ty>
    using relative_ptr64 = relative_ptr<Ty, std::int64_t>;
}
