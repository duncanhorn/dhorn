/*
 * SingletonTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn singleton class
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/singleton.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        class test_class
        {
        public:

            static size_t instance_count;
        };
        size_t test_class::instance_count = 0;



        TEST_CLASS(SingletonTests)
        {
            TEST_METHOD_INITIALIZE(TestInit)
            {
                // Reset state
                test_class::instance_count = 0;
            }

            TEST_METHOD(BasicSharedPtrGetInstanceTest)
            {
                //dhorn::singleton<test_class> obj;
            }
        };
    }
}
