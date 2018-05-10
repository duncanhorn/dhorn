#pragma once

#include "scanner.h"

namespace dhorn::experimental::json
{
    /*
     * value_type
     *
     * Indicates the underlying type of a JSON value
     */
    enum class value_type
    {
        null,
        boolean,
        number,
        string,
        array,
        object,
    };
}
