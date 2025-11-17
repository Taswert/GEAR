#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	void hoverObjectLasso(GameObject* obj);
	void hoverObjectSquare(GameObject* obj);
	void resetHover(GameObject* obj);
}