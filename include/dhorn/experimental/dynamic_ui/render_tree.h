/*
 * Duncan Horn
 *
 * render_tree.h
 *
 * Represents the entire render tree
 */
#pragma once

#include <memory>

#include "details/render_types.h"

namespace dhorn
{
    namespace dynamic_ui
    {
        // Helpers and common types



#pragma region render_tree

        class render_tree final
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            render_tree(void) :
                _state(std::make_shared<details::render_state>())
            {
            }



        private:

            std::shared_ptr<details::render_state> _state;
        };

#pragma endregion
    }
}
