#pragma once
#include <imgui_internal.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	inline cocos2d::CCDrawNode* editorUIDrawNode = nullptr;

	inline bool isLassoEnabled = false;

	void renderToolsModule1();
	void renderToolsModule2();
}