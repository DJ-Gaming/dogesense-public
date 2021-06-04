#pragma once
#include "../../dependencies/utilities/csgo.hpp"
#include "framework.hpp"
#include "variables.hpp"
#include "zgui.hpp"

namespace menu {
	inline int current_tab{0};

	void render();
	void toggle();
	void initialize();;
};
