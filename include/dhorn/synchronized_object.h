/*
 * Duncan Horn
 *
 * synchronized_object.h
 *
 * Represents an object that intrinsically does synchronization for you. This is useful when dealing with objects that
 * are otherwise not thread safe (e.g. std::vector, etc.). The idea is that you declare whatever type you are wanting
 * with synchronized_object<Ty> for any type Ty (e.g. synchronized_object<std::vector<...>>) and then depending on
 * which function is invoked on the synchronized_object instance, user-defined code will run either with or without
 * acquiring the lock via the execute_with_lock and execute_without_lock functions. Note that due to internal
 * restrictions of the data type and the ambiguity surrounding whether copies should be done with or without the lock,
 * synchronized_objects cannot be copied or moved. It is the responsibility of the client for choosing how copies
 * should be made. Similarly, synchronized_objects cannot be swapped.
 *
 * Each synchronized_object contains exactly one mutex (the type of which is customizable) and the value that it is
 * protecting. When you execute a function under lock, the first argument to this function will always be a reference
 * to the object that is protected by the synchronized_object. The second argument is a reference to the lock object.
 * E.g. if you call execute_with_lock<std::unique_lock> on a synchronized_object that uses std::mutex, the second
 * argument would be a std::unique_lock<std::mutex> &. This gives the client an opportunity to use condition_variable,
 * etc. for more advanced synchronization.
 *
 * The synchronized_object class also provides the lock function which allows a LockType (e.g. std::lock_guard, etc.)
 * as well as an optional std::defer_lock_t or std::try_to_lock_t. Note that an overload for std::adopt_lock_t is not
 * provided since such an operation would not exist (you cannot lock in a non-RAII way).
 */
#pragma once

#include <mutex>

namespace dhorn
{
    template <typename Ty, typename MutexType = std::mutex>
    class synchronized_object
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        synchronized_object()
        {
        }

        synchronized_object(_In_ Ty value) :
            _value(std::move(value))
        {
        }

        // Cannot copy
        synchronized_object(_In_ const synchronized_object &) = delete;
        synchronized_object &operator=(_In_ const synchronized_object &) = delete;



        /*
         * Execution
         */
        template <typename LockType = std::unique_lock<MutexType>, typename Func>
        void execute_with_lock(_In_ const Func &fn)
        {
            LockType lock(this->_mutex);
            fn(this->_value, lock);
        }

        template <typename Func>
        void execute_without_lock(_In_ const Func &fn)
        {
            fn(this->_value);
        }

        template <typename Func>
        void execute_without_lock(_In_ const Func &fn) const
        {
            fn(this->_value);
        }



        /*
         * Value get/set
         *
         * TODO: get and get_locked both return a copy. Would it make sense for get (not get_locked) to return a
         * reference since it's already not thread safe?
         */
        Ty copy_unlocked(void)
        {
            return this->_value;
        }

        Ty copy_unlocked(void) const
        {
            return this->_value;
        }

        template <typename LockType = std::unique_lock<MutexType>>
        Ty copy_locked(void)
        {
            LockType lock(this->_mutex);
            return this->_value;
        }

        void set_unlocked(_In_ const Ty &value)
        {
            this->_value = value;
        }

        template <typename LockType = std::unique_lock<MutexType>>
        void set_locked(_In_ const Ty &value)
        {
            LockType lock(this->_mutex);
            this->_value = value;
        }



        /*
         * Locking
         */
        template <typename LockType = std::unique_lock<MutexType>>
        LockType lock(void)
        {
            return LockType(this->_mutex);
        }

        template <typename LockType = std::unique_lock<MutexType>>
        LockType lock(_In_ std::try_to_lock_t t)
        {
            return LockType(this->_mutex, t);
        }

        template <typename LockType = std::unique_lock<MutexType>>
        LockType lock(_In_ std::defer_lock_t t)
        {
            return LockType(this->_mutex, t);
        }



    private:

        Ty _value;
        MutexType _mutex;
    };
}
