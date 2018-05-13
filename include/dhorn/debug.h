/*
 * Duncan Horn
 *
 * debug.h
 *
 * A collection of operations that are useful for asserting various conditions when in debug mode.
 */
#pragma once

#include <cassert>
#include <mutex>

#ifndef DHORN_DEBUG
#if (defined DEBUG) || (defined _DEBUG) || (defined DBG)
#define DHORN_DEBUG 1
#endif
#endif

namespace dhorn
{
#pragma region Locking Assertions

    /*
     * assert_lock_held
     *
     * Asserts that the lock is held. Note that this does _not_ necessarily assert that the caller is holding the lock,
     * but should give decent confidence since it is unlikely that contention will exist every time this check is called
     * while testing.
     */
    template <typename Lockable>
    inline void assert_lock_held([[maybe_unused]] Lockable& lockable)
    {
#if DHORN_DEBUG
        // We expect try_lock to return false. If it happens to return true, we'll assert fail, but we still want to be
        // continuable if a debugger is attached, so make sure to unlock as well
        assert(!lockable.try_lock() || (lockable.unlock(), false));
#endif
    }



    /*
     * assert_lock_held_exclusive
     *
     * Asserts that the shared lock is held exclusively. I.e. this call will raise an assertion failure if the lock is
     * either not held or held in shared mode. Note that just like assert_lock_held, this function does not necessarily
     * assert that the caller is the one holding the lock.
     */
    template <typename SharedMutex>
    inline void assert_lock_held_exclusive([[maybe_unused]] SharedMutex& mutex)
    {
#if DHORN_DEBUG
        // Exclusive means we can't acquire it shared
        assert(!mutex.try_lock_shared() || (mutex.unlock_shared(), false));
#endif
    }



    /*
     * assert_lock_held_shared
     *
     * Asserts that the shared lock is _at least_ held in shared mode. I.e. asserts that the lock is held either shared
     * _or_ exclusively. Again, just like assert_lock_held and assert_lock_held_exclusive, this does not necessarily
     * assert that the caller is the one holding the lock.
     */
    template <typename SharedMutex>
    inline void assert_lock_held_shared([[maybe_unused]] SharedMutex& mutex)
    {
#if DHORN_DEBUG
        // Shared means we can't acquire it exclusively
        assert_lock_held(mutex);
#endif
    }

#pragma endregion
}
