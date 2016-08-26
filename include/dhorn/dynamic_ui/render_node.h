/*
 * Duncan Horn
 *
 * render_node.h
 *
 * Represents a single node in a render_tree
 */
#pragma once

#include <memory>

#include "garbage/render_types.h"

namespace dhorn
{
    namespace dynamic_ui
    {
#pragma region render_node

        class render_node
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            render_node(std::shared_ptr<const garbage::render_state> state) :
                _state(std::move(state))
            {
            }

            virtual ~render_node(void)
            {
            }



        protected:

            std::shared_ptr<const garbage::render_state> _state;



        private:


        };

#pragma endregion
    }
}
