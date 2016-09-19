/*
 * Duncan Horn
 *
 * console_test.h
 *
 * 
 */
#pragma once

#include <dhorn/experimental/console.h>

class console_test
{
public:

    void operator()()
    {
        std::cout << "dhorn::console tests" << std::endl;
        std::cout << "Testing out foreground colors..." << std::endl;
        test_foreground_colors();
        std::cout << std::endl;

        std::cout << "Testing out background colors..." << std::endl;
        test_background_colors();
        std::cout << std::endl;
    }

private:

    void test_foreground_colors()
    {
        using namespace dhorn::experimental;

        auto black = change_console_foreground(console_color::black);
        std::cout << "black" << std::endl;

        auto darkGray = change_console_foreground(console_color::dark_gray);
        std::cout << "dark_gray" << std::endl;

        auto gray = change_console_foreground(console_color::gray);
        std::cout << "gray" << std::endl;

        auto white = change_console_foreground(console_color::white);
        std::cout << "white" << std::endl;

        auto darkRed = change_console_foreground(console_color::dark_red);
        std::cout << "dark_red" << std::endl;

        auto red = change_console_foreground(console_color::red);
        std::cout << "red" << std::endl;

        auto darkGreen = change_console_foreground(console_color::dark_green);
        std::cout << "dark_green" << std::endl;

        auto green = change_console_foreground(console_color::green);
        std::cout << "green" << std::endl;

        auto darkBlue = change_console_foreground(console_color::dark_blue);
        std::cout << "dark_blue" << std::endl;

        auto blue = change_console_foreground(console_color::blue);
        std::cout << "blue" << std::endl;

        auto darkMagenta = change_console_foreground(console_color::dark_magenta);
        std::cout << "dark_magenta" << std::endl;

        auto magenta = change_console_foreground(console_color::magenta);
        std::cout << "magenta" << std::endl;

        auto darkCyan = change_console_foreground(console_color::dark_cyan);
        std::cout << "dark_cyan" << std::endl;

        auto cyan = change_console_foreground(console_color::cyan);
        std::cout << "cyan" << std::endl;

        auto darkYellow = change_console_foreground(console_color::dark_yellow);
        std::cout << "dark_yellow" << std::endl;

        auto yellow = change_console_foreground(console_color::yellow);
        std::cout << "yellow" << std::endl;
    }

    void test_background_colors()
    {
        using namespace dhorn::experimental;

        auto black = change_console_background(console_color::black);
        std::cout << "black" << std::endl;

        auto darkGray = change_console_background(console_color::dark_gray);
        std::cout << "dark_gray" << std::endl;

        auto gray = change_console_background(console_color::gray);
        std::cout << "gray" << std::endl;

        auto white = change_console_background(console_color::white);
        std::cout << "white" << std::endl;

        auto darkRed = change_console_background(console_color::dark_red);
        std::cout << "dark_red" << std::endl;

        auto red = change_console_background(console_color::red);
        std::cout << "red" << std::endl;

        auto darkGreen = change_console_background(console_color::dark_green);
        std::cout << "dark_green" << std::endl;

        auto green = change_console_background(console_color::green);
        std::cout << "green" << std::endl;

        auto darkBlue = change_console_background(console_color::dark_blue);
        std::cout << "dark_blue" << std::endl;

        auto blue = change_console_background(console_color::blue);
        std::cout << "blue" << std::endl;

        auto darkMagenta = change_console_background(console_color::dark_magenta);
        std::cout << "dark_magenta" << std::endl;

        auto magenta = change_console_background(console_color::magenta);
        std::cout << "magenta" << std::endl;

        auto darkCyan = change_console_background(console_color::dark_cyan);
        std::cout << "dark_cyan" << std::endl;

        auto cyan = change_console_background(console_color::cyan);
        std::cout << "cyan" << std::endl;

        auto darkYellow = change_console_background(console_color::dark_yellow);
        std::cout << "dark_yellow" << std::endl;

        auto yellow = change_console_background(console_color::yellow);
        std::cout << "yellow" << std::endl;
    }
};
