/*
 * Duncan Horn
 */

#include <iostream>

#include <dhorn/experimental/array_reference.h>

#include "console_test.h"

int main(int argc, char **argv)
{
    dhorn::experimental::set_console_title(L"dhorn functional tests");
    dhorn::experimental::array_reference<char *> args(argv + 1, argc - 1);

    for (auto arg : args)
    {
        if (strcmp(arg, "console") == 0)
        {
            console_test()();
        }
        else
        {
            auto redErrors = dhorn::experimental::change_console_foreground(dhorn::experimental::console_color::red);
            std::cerr << "ERROR: Unknown argument \"" << arg << "\"" << std::endl;
        }
    }
}
