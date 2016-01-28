/*
 * CommandLineTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the command_line type
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/command_line.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(CommandLineTests)
        {
            TEST_METHOD(EmptyTest)
            {
                dhorn::command_line cmdLine;
                Assert::AreEqual(0u, cmdLine.size());

                dhorn::command_line cmdLine2({});
                Assert::AreEqual(0u, cmdLine2.size());
            }

            TEST_METHOD(SingleValueTest)
            {
                dhorn::command_line cmdLine({ "foo" });
                Assert::AreEqual(1u, cmdLine.size());

                Assert::IsTrue("foo" == *cmdLine.begin());
                //TODO: Assert::IsTrue(cmdLine.begin().begin() == cmdLine.begin().end());
            }
        };
    }
}
