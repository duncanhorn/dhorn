/*
 * Duncan Horn
 *
 * console_test.h
 *
 * 
 */
#pragma once

#include <dhorn/experimental/console.h>

inline std::ostream& operator<<(std::ostream& stream, dhorn::experimental::console_color color)
{
    using namespace dhorn::experimental;
    switch (color)
    {
    case console_color::black:
        stream << "black";
        break;

    case console_color::dark_gray:
        stream << "dark_gray";
        break;

    case console_color::gray:
        stream << "gray";
        break;

    case console_color::white:
        stream << "white";
        break;

    case console_color::dark_red:
        stream << "dark_red";
        break;

    case console_color::red:
        stream << "red";
        break;

    case console_color::dark_green:
        stream << "dark_green";
        break;

    case console_color::green:
        stream << "green";
        break;

    case console_color::dark_blue:
        stream << "dark_blue";
        break;

    case console_color::blue:
        stream << "blue";
        break;

    case console_color::dark_magenta:
        stream << "dark_magenta";
        break;

    case console_color::magenta:
        stream << "magenta";
        break;

    case console_color::dark_cyan:
        stream << "dark_cyan";
        break;

    case console_color::cyan:
        stream << "cyan";
        break;

    case console_color::dark_yellow:
        stream << "dark_yellow";
        break;

    case console_color::yellow:
        stream << "yellow";
        break;

    default:
        stream << "unknown";
        break;
    }

    return stream;
}

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

        test_console_info();
        std::cout << std::endl;
    }

private:

    void test_foreground_colors()
    {
        using namespace dhorn::experimental;

        auto black = console::set_foreground(console_color::black);
        std::cout << "black" << std::endl;

        auto darkGray = console::set_foreground(console_color::dark_gray);
        std::cout << "dark_gray" << std::endl;

        auto gray = console::set_foreground(console_color::gray);
        std::cout << "gray" << std::endl;

        auto white = console::set_foreground(console_color::white);
        std::cout << "white" << std::endl;

        auto darkRed = console::set_foreground(console_color::dark_red);
        std::cout << "dark_red" << std::endl;

        auto red = console::set_foreground(console_color::red);
        std::cout << "red" << std::endl;

        auto darkGreen = console::set_foreground(console_color::dark_green);
        std::cout << "dark_green" << std::endl;

        auto green = console::set_foreground(console_color::green);
        std::cout << "green" << std::endl;

        auto darkBlue = console::set_foreground(console_color::dark_blue);
        std::cout << "dark_blue" << std::endl;

        auto blue = console::set_foreground(console_color::blue);
        std::cout << "blue" << std::endl;

        auto darkMagenta = console::set_foreground(console_color::dark_magenta);
        std::cout << "dark_magenta" << std::endl;

        auto magenta = console::set_foreground(console_color::magenta);
        std::cout << "magenta" << std::endl;

        auto darkCyan = console::set_foreground(console_color::dark_cyan);
        std::cout << "dark_cyan" << std::endl;

        auto cyan = console::set_foreground(console_color::cyan);
        std::cout << "cyan" << std::endl;

        auto darkYellow = console::set_foreground(console_color::dark_yellow);
        std::cout << "dark_yellow" << std::endl;

        auto yellow = console::set_foreground(console_color::yellow);
        std::cout << "yellow" << std::endl;
    }

    void test_background_colors()
    {
        using namespace dhorn::experimental;

        auto black = console::set_background(console_color::black);
        std::cout << "black" << std::endl;

        auto darkGray = console::set_background(console_color::dark_gray);
        std::cout << "dark_gray" << std::endl;

        auto gray = console::set_background(console_color::gray);
        std::cout << "gray" << std::endl;

        auto white = console::set_background(console_color::white);
        std::cout << "white" << std::endl;

        auto darkRed = console::set_background(console_color::dark_red);
        std::cout << "dark_red" << std::endl;

        auto red = console::set_background(console_color::red);
        std::cout << "red" << std::endl;

        auto darkGreen = console::set_background(console_color::dark_green);
        std::cout << "dark_green" << std::endl;

        auto green = console::set_background(console_color::green);
        std::cout << "green" << std::endl;

        auto darkBlue = console::set_background(console_color::dark_blue);
        std::cout << "dark_blue" << std::endl;

        auto blue = console::set_background(console_color::blue);
        std::cout << "blue" << std::endl;

        auto darkMagenta = console::set_background(console_color::dark_magenta);
        std::cout << "dark_magenta" << std::endl;

        auto magenta = console::set_background(console_color::magenta);
        std::cout << "magenta" << std::endl;

        auto darkCyan = console::set_background(console_color::dark_cyan);
        std::cout << "dark_cyan" << std::endl;

        auto cyan = console::set_background(console_color::cyan);
        std::cout << "cyan" << std::endl;

        auto darkYellow = console::set_background(console_color::dark_yellow);
        std::cout << "dark_yellow" << std::endl;

        auto yellow = console::set_background(console_color::yellow);
        std::cout << "yellow" << std::endl;
    }

    void test_console_info()
    {
        auto bounds = dhorn::experimental::console::bounds();
        std::cout << "Console position: (" << bounds.x << ", " << bounds.y << ")" << std::endl;
        std::cout << "Console size: " << bounds.width << "x" << bounds.height << std::endl;

        auto size = dhorn::experimental::console::buffer_size();
        std::cout << "Console buffer size: " << size.width << "x" << size.height << std::endl;

        auto pos = dhorn::experimental::console::cursor_position();
        std::cout << "Console cursor position: (" << pos.x << ", " << pos.y << ")" << std::endl;

        std::cout << "Console foreground color: " << dhorn::experimental::console::foreground() << std::endl;
        std::cout << "Console background color: " << dhorn::experimental::console::background() << std::endl;
    }
};
