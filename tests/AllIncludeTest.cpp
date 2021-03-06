/*
 * Duncan Horn
 *
 * AllIncludeTest.h
 *
 * Makes sure that there aren't any conflicting types/etc.
 */

// "Normal" Includes
#include <dhorn/algorithm.h>
#include <dhorn/bitmask.h>
#include <dhorn/compressed_base.h>
#include <dhorn/compressed_pair.h>
#include <dhorn/console.h>
#include <dhorn/crtp_base.h>
#include <dhorn/debug.h>
#include <dhorn/functional.h>
#include <dhorn/inplace_function.h>
#include <dhorn/iterator.h>
#include <dhorn/scope_guard.h>
#include <dhorn/string.h>
#include <dhorn/thread_pool.h>
#include <dhorn/type_traits.h>
#include <dhorn/utility.h>
#include <dhorn/visitor.h>

// COM Includes
#include <dhorn/com/com_ptr.h>
#include <dhorn/com/com_traits.h>
#include <dhorn/com/hresult_error.h>

// Unicode Includes
#include <dhorn/unicode/encoding.h>
#include <dhorn/unicode/iterator.h>

// Windows Includes
#include <dhorn/windows/file_time_clock.h>
#include <dhorn/windows/guid.h>
#include <dhorn/windows/tick_count_clock.h>

// WinRT Includes
#include <dhorn/winrt/weak_ref.h>
#include <dhorn/winrt/winrt_traits.h>
