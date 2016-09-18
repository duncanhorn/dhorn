/*
 * Duncan Horn
 *
 * nullable_value.h
 *
 * Represents a "nullable" value that is stored inside of the object as opposed to on the heap. I.e. this implies that
 * the type has two member variables: one of the target type, and a pointer that can point to either null or to this
 * value. In general, this type behaves like a pointer with the exception of assignment (since it can only point to one
 * of two values).
 */
#pragma once

#include <cstdint>

namespace dhorn
{
    namespace experimental
    {
        template <typename Ty>
        class nullable_value final
        {
        public:
            nullable_value(void) = default;

            nullable_value(const nullable_value &other)
            {
                if (other)
                {
                    Set(other.get());
                }
            }

            nullable_value(nullable_value &&other)
            {
                swap(other);
            }

            ~nullable_value()
            {
                Reset();
            }



            /*
             * Operators
             */
            nullable_value &operator=(const nullable_value &other)
            {
                if (&other == this)
                {
                    return *this;
                }

                Reset();
                if (other)
                {
                    Set(other.get());
                }

                return *this;
            }

            nullable_value &operator=(nullable_value &&other)
            {
                swap(other);
                return *this;
            }

            Ty &operator*(void)
            {
                return *this->_ptr;
            }

            const Ty &operator*(void) const
            {
                return *this->_ptr;
            }

            Ty *operator->(void) const
            {
                return this->_ptr;
            }

            operator bool(void) const
            {
                return has_value();
            }



            /*
             * Functions
             */
            const Ty &get(void) const
            {
                return *this->_ptr;
            }

            bool has_value(void) const
            {
                return this->_ptr != nullptr;
            }

            void reset(void)
            {
                Reset();
            }

            void reset(const Ty &value)
            {
                Set(value);
            }

            void reset(Ty &&value)
            {
                Set(std::move(value));
            }

            void set(const Ty &value)
            {
                Set(value);
            }

            void set(Ty &&value)
            {
                Set(std::move(value));
            }

            void swap(nullable_value &other)
            {
                if (other && has_value())
                {
                    // Both are good; swap is okay
                    std::swap(*this->_ptr, *other._ptr);
                }
                else if (other)
                {
                    // Only other is good
                    Set(std::move(*other._ptr));
                    other.Reset();
                }
                else if (has_value())
                {
                    // Only this is good
                    other.Set(std::move(*this->_ptr));
                    Reset();
                }
            }



        private:

            void SetPointer()
            {
                this->_ptr = reinterpret_cast<Ty *>(&this->_data);
            }

            void Set(const Ty &value)
            {
                if (!this->_ptr)
                {
                    // First creation
                    SetPointer();
                    new (this->_ptr) Ty(value);
                }
                else
                {
                    // Already created
                    *this->_ptr = value;
                }
            }

            void Set(Ty &&value)
            {
                if (!this->_ptr)
                {
                    // First creation
                    SetPointer();
                    new (this->_ptr) Ty(std::move(value));
                }
                else
                {
                    // Already created
                    *this->_ptr = std::move(value);
                }
            }

            void Reset()
            {
                if (this->_ptr)
                {
                    this->_ptr->~Ty();
                    this->_ptr = nullptr;
                }
            }

            Ty *_ptr = nullptr;
            uint8_t _data[sizeof(Ty)];
        };
    }
}
