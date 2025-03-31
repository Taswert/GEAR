#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	inline cocos2d::CCDrawNode* editorUIDrawNode = nullptr;
	inline std::vector<cocos2d::CCPoint> editorUILassoPoints;

	inline bool isLassoEnabled = false;

	void renderToolsModule1();
	void renderToolsModule2();
}