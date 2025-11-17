#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
#include "../features/Selection.hpp"
using namespace geode::prelude;

namespace ErGui {

	void saveHueValues(ccColor3B* color);
	void setupTriggersSettings();
	void renderEditObjectModule();
}