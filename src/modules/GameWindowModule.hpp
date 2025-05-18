#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	extern bool isGameWindowHovered;
	extern CCPoint gameWindowTouchCoordinatesConvertedToWorld;
	void renderGameWindow();
}