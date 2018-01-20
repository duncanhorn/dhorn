/*
 * Duncan Horn
 *
 * FunctionalTests.cpp
 *
 * Tests for the functional.h header file
 */
#include "stdafx.h"

#include <dhorn/functional.h>
#include <functional>
#include <string>

using namespace std::literals;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(FunctionalTests)
    {
        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            object_counter::reset();
        }

        TEST_METHOD_CLEANUP(TestCleanup)
        {
            Assert::AreEqual(static_cast<std::size_t>(0), object_counter::instance_count);
        }

        TEST_METHOD(MakeLambdaSharedInstanceTest)
        {
            object_counter obj;
            auto lambda = make_lambda_shared([obj = std::move(obj)]()
            {
            });

            // Should not have made a copy
            Assert::AreEqual(static_cast<std::size_t>(0), object_counter::copy_count);

            auto moveCnt = object_counter::move_count;
            auto objCnt = object_counter::instance_count;
            auto constructCnt = object_counter::constructed_count;

            // Copying the new lambda should not affect object count
            auto lambdaCopy = lambda;

            Assert::AreEqual(static_cast<std::size_t>(0), object_counter::copy_count);
            Assert::AreEqual(moveCnt, object_counter::move_count);
            Assert::AreEqual(objCnt, object_counter::instance_count);
            Assert::AreEqual(constructCnt, object_counter::constructed_count);
        }

        TEST_METHOD(MakeLambdaSharedCopyableTest)
        {
            // unique_ptr is not copy-able
            auto lambda = make_lambda_shared([ptr = std::make_unique<std::string>("foo")]()
            {
            });

            // This is strictly a "does it compile" test
            std::function<void(void)> fn = lambda;
        }
    };
}
