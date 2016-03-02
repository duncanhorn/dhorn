/*
 * Duncan Horn
 *
 * CommandLineTests.cpp
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

                Assert::IsTrue(cmdLine.begin() == cmdLine.end());
            }

            TEST_METHOD(SingleValueTest)
            {
                dhorn::command_line cmdLine({ "foo" });
                Assert::AreEqual(1u, cmdLine.size());

                Assert::IsTrue(*cmdLine.begin() == "foo");
                Assert::IsTrue(cmdLine.begin().begin() == cmdLine.begin().end());
                Assert::IsTrue(cmdLine.begin() != cmdLine.end());
            }

            TEST_METHOD(TwoValueTest)
            {
                dhorn::command_line cmdLine({ "foo", "bar" });
                Assert::AreEqual(2u, cmdLine.size());

                auto itr = cmdLine.begin();
                Assert::IsTrue(itr != cmdLine.end());
                Assert::IsTrue(*itr == "foo");
                Assert::IsTrue(itr.begin() == itr.end());

                ++itr;
                Assert::IsTrue(itr != cmdLine.end());
                Assert::IsTrue(*itr == "bar");
                Assert::IsTrue(itr.begin() == itr.end());

                ++itr;
                Assert::IsTrue(itr == cmdLine.end());
            }

            TEST_METHOD(SingleSwitchTest)
            {
                dhorn::command_line cmdLine({ "/foo" });
                Assert::AreEqual(1u, cmdLine.size());

                Assert::IsTrue(*cmdLine.begin() == "/foo");
                Assert::IsTrue(cmdLine.begin().begin() == cmdLine.begin().end());
                Assert::IsTrue(cmdLine.begin() != cmdLine.end());
            }

            TEST_METHOD(TwoSwitchTest)
            {
                dhorn::command_line cmdLine({ "/foo", "/bar" });
                Assert::AreEqual(2u, cmdLine.size());

                auto itr = cmdLine.begin();
                Assert::IsTrue(itr != cmdLine.end());
                Assert::IsTrue(*itr == "/foo");

                auto switchItr = itr.begin();
                Assert::IsTrue(switchItr == itr.end());

                ++itr;
                Assert::IsTrue(itr != cmdLine.end());
                Assert::IsTrue(*itr == "/bar");

                switchItr = itr.begin();
                Assert::IsTrue(switchItr == itr.end());

                ++itr;
                Assert::IsTrue(itr == cmdLine.end());
            }

            TEST_METHOD(SingleSwitchWithSingleArgTest)
            {
                dhorn::command_line cmdLine({ "/foo", "bar" });
                Assert::AreEqual(2u, cmdLine.size());

                auto itr = cmdLine.begin();
                Assert::IsTrue(itr != cmdLine.end());
                Assert::IsTrue(*itr == "/foo");

                auto switchItr = itr.begin();
                Assert::IsTrue(switchItr != itr.end());
                Assert::IsTrue(*switchItr == "bar");

                ++switchItr;
                Assert::IsTrue(switchItr == itr.end());
                Assert::IsTrue(itr != cmdLine.end());
                Assert::IsTrue(*itr == "bar");

                ++itr;
                Assert::IsTrue(itr == cmdLine.end());
            }

            TEST_METHOD(SingleSwitchWithMultiArgTest)
            {
                dhorn::command_line cmdLine({ "/foo", "bar1", "bar2" });
                Assert::AreEqual(3u, cmdLine.size());

                auto itr = cmdLine.begin();
                Assert::IsTrue(itr != cmdLine.end());
                Assert::IsTrue(*itr == "/foo");

                auto switchItr = itr.begin();
                Assert::IsTrue(switchItr != itr.end());
                Assert::IsTrue(*switchItr == "bar1");

                ++switchItr;
                Assert::IsTrue(switchItr != itr.end());
                Assert::IsTrue(*switchItr == "bar2");
                Assert::IsTrue(*itr == "bar1");

                ++switchItr;
                Assert::IsTrue(switchItr == itr.end());
                Assert::IsTrue(itr != cmdLine.end());
                Assert::IsTrue(*itr == "bar2");

                ++itr;
                Assert::IsTrue(itr == cmdLine.end());
            }

            void DoComplexIterteTest(
                _In_ const std::vector<std::string> &input,
                _In_ const std::vector<std::string> &switches,
                _In_ const std::vector<std::string> &args)
            {
                std::vector<std::string> output;
                std::vector<std::string> outSwitches;
                std::vector<std::string> outArgs;

                dhorn::command_line cmdLine(std::begin(input), std::end(input));
                for (auto itr = cmdLine.begin(); itr != cmdLine.end(); ++itr)
                {
                    output.push_back(*itr);

                    if (!dhorn::is_command_line_switch(*itr))
                    {
                        outArgs.push_back(*itr);
                    }
                    else
                    {
                        outSwitches.push_back(*itr);
                        for (auto &str : itr)
                        {
                            output.push_back(str);
                            outArgs.push_back(str);
                        }
                    }
                }

                Assert::IsTrue(input == output);
                Assert::IsTrue(switches == outSwitches);
                Assert::IsTrue(args == outArgs);
            }

            TEST_METHOD(ComplexIterateTest1)
            {
                std::vector<std::string> input = { "/switch1", "arg1", "arg2", "/switch2" };
                std::vector<std::string> switches = { "/switch1", "/switch2" };
                std::vector<std::string> args = { "arg1", "arg2" };

                DoComplexIterteTest(input, switches, args);
            }

            TEST_METHOD(ComplexIterateTest2)
            {
                std::vector<std::string> input = { "/switch1", "arg1", "arg2", "/switch2", "arg3" };
                std::vector<std::string> switches = { "/switch1", "/switch2" };
                std::vector<std::string> args = { "arg1", "arg2", "arg3" };

                DoComplexIterteTest(input, switches, args);
            }

            TEST_METHOD(ComplexIterateTest3)
            {
                std::vector<std::string> input = { "arg1", "arg2", "/switch1", "/switch2", "arg3" };
                std::vector<std::string> switches = { "/switch1", "/switch2" };
                std::vector<std::string> args = { "arg1", "arg2", "arg3" };

                DoComplexIterteTest(input, switches, args);
            }
        };
    }
}
