/*
 * Duncan Horn
 *
 * CompressedPairTests.cpp
 *
 * Tests for the compressed_pair type
 */
#include "stdafx.h"

#include <dhorn/compressed_pair.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(CompressedPairTests)
    {
        struct empty
        {
        };

        struct non_empty
        {
            int value = 42;
        };

        struct empty_final final
        {
        };

        struct empty_explicit
        {
            explicit empty_explicit() {}
            explicit empty_explicit(const empty_explicit&) {}
            explicit empty_explicit(empty_explicit&&) {}
        };

        struct non_empty_explicit
        {
            explicit non_empty_explicit() {}
            explicit non_empty_explicit(const non_empty_explicit& o) : value(o.value) {}
            explicit non_empty_explicit(non_empty_explicit&& o) : value(o.value) {}

            int value = 42;
        };

        struct empty_final_explicit final
        {
            explicit empty_final_explicit() {}
            explicit empty_final_explicit(const empty_final_explicit&) {}
            explicit empty_final_explicit(empty_final_explicit&&) {}
        };
    };
}
