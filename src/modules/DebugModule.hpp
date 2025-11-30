#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	void renderDebugModule();
	
	#ifdef GEAR_DEBUG
	inline bool g_showDebugModule = true;
	#else
	inline bool g_showDebugModule = false;
	#endif
}