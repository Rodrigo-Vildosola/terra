#pragma once

#include <glm/glm.hpp>

#include "terra/core/base.h"
#include "terra/input/key_codes.h"
#include "terra/input/mouse_codes.h"

namespace terra {

	class Input
	{
	public:
		static bool is_key_pressed(KeyCode key);

		static bool is_mouse_button_pressed(MouseCode button);
		static glm::vec2 get_mouse_pos();
		static float get_mouse_x();
		static float get_mouse_y();
	};

}
