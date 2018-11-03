/*
 * Duncan Horn
 *
 * CommandLineTests.cpp
 *
 * Tests for the command_line type
 */
#include "stdafx.h"

#include <dhorn/experimental/command_line.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(CommandLineTests)
        {
            TEST_METHOD(EmptyTest)
            {
                dhorn::experimental::command_line cmdLine;
                ASSERT_EQ(static_cast<std::size_t>(0), cmdLine.size());

                dhorn::experimental::command_line cmdLine2({});
                ASSERT_EQ(static_cast<std::size_t>(0), cmdLine2.size());

                ASSERT_TRUE(cmdLine.begin() == cmdLine.end());
            }

            TEST_METHOD(SingleValueTest)
            {
                dhorn::experimental::command_line cmdLine({ "foo" });
                ASSERT_EQ(static_cast<std::size_t>(1), cmdLine.size());

                ASSERT_TRUE(*cmdLine.begin() == "foo");
                ASSERT_TRUE(cmdLine.begin().begin() == cmdLine.begin().end());
                ASSERT_TRUE(cmdLine.begin() != cmdLine.end());
            }

            TEST_METHOD(TwoValueTest)
            {
                dhorn::experimental::command_line cmdLine({ "foo", "bar" });
                ASSERT_EQ(static_cast<std::size_t>(2), cmdLine.size());

                auto itr = cmdLine.begin();
                ASSERT_TRUE(itr != cmdLine.end());
                ASSERT_TRUE(*itr == "foo");
                ASSERT_TRUE(itr.begin() == itr.end());

                ++itr;
                ASSERT_TRUE(itr != cmdLine.end());
                ASSERT_TRUE(*itr == "bar");
                ASSERT_TRUE(itr.begin() == itr.end());

                ++itr;
                ASSERT_TRUE(itr == cmdLine.end());
            }

            TEST_METHOD(SingleSwitchTest)
            {
                dhorn::experimental::command_line cmdLine({ "/foo" });
                ASSERT_EQ(static_cast<std::size_t>(1), cmdLine.size());

                ASSERT_TRUE(*cmdLine.begin() == "/foo");
                ASSERT_TRUE(cmdLine.begin().begin() == cmdLine.begin().end());
                ASSERT_TRUE(cmdLine.begin() != cmdLine.end());
            }

            TEST_METHOD(TwoSwitchTest)
            {
                dhorn::experimental::command_line cmdLine({ "/foo", "/bar" });
                ASSERT_EQ(static_cast<std::size_t>(2), cmdLine.size());

                auto itr = cmdLine.begin();
                ASSERT_TRUE(itr != cmdLine.end());
                ASSERT_TRUE(*itr == "/foo");

                auto switchItr = itr.begin();
                ASSERT_TRUE(switchItr == itr.end());

                ++itr;
                ASSERT_TRUE(itr != cmdLine.end());
                ASSERT_TRUE(*itr == "/bar");

                switchItr = itr.begin();
                ASSERT_TRUE(switchItr == itr.end());

                ++itr;
                ASSERT_TRUE(itr == cmdLine.end());
            }

            TEST_METHOD(SingleSwitchWithSingleArgTest)
            {
                dhorn::experimental::command_line cmdLine({ "/foo", "bar" });
                ASSERT_EQ(static_cast<std::size_t>(2), cmdLine.size());

                auto itr = cmdLine.begin();
                ASSERT_TRUE(itr != cmdLine.end());
                ASSERT_TRUE(*itr == "/foo");

                auto switchItr = itr.begin();
                ASSERT_TRUE(switchItr != itr.end());
                ASSERT_TRUE(*switchItr == "bar");

                ++switchItr;
                ASSERT_TRUE(switchItr == itr.end());
                ASSERT_TRUE(itr != cmdLine.end());
                ASSERT_TRUE(*itr == "bar");

                ++itr;
                ASSERT_TRUE(itr == cmdLine.end());
            }

            TEST_METHOD(SingleSwitchWithMultiArgTest)
            {
                dhorn::experimental::command_line cmdLine({ "/foo", "bar1", "bar2" });
                ASSERT_EQ(static_cast<std::size_t>(3), cmdLine.size());

                auto itr = cmdLine.begin();
                ASSERT_TRUE(itr != cmdLine.end());
                ASSERT_TRUE(*itr == "/foo");

                auto switchItr = itr.begin();
                ASSERT_TRUE(switchItr != itr.end());
                ASSERT_TRUE(*switchItr == "bar1");

                ++switchItr;
                ASSERT_TRUE(switchItr != itr.end());
                ASSERT_TRUE(*switchItr == "bar2");
                ASSERT_TRUE(*itr == "bar1");

                ++switchItr;
                ASSERT_TRUE(switchItr == itr.end());
                ASSERT_TRUE(itr != cmdLine.end());
                ASSERT_TRUE(*itr == "bar2");

                ++itr;
                ASSERT_TRUE(itr == cmdLine.end());
            }

            void DoComplexIterteTest(
                const std::vector<std::string> &input,
                const std::vector<std::string> &switches,
                const std::vector<std::string> &args)
            {
                std::vector<std::string> output;
                std::vector<std::string> outSwitches;
                std::vector<std::string> outArgs;

                dhorn::experimental::command_line cmdLine(input.begin(), input.end());
                for (auto itr = cmdLine.begin(); itr != cmdLine.end(); ++itr)
                {
                    output.push_back(*itr);

                    if (!dhorn::experimental::is_command_line_switch(*itr))
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

                ASSERT_TRUE(input == output);
                ASSERT_TRUE(switches == outSwitches);
                ASSERT_TRUE(args == outArgs);
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
