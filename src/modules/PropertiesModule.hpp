#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>

using namespace geode::prelude;

namespace ErGui {
	void renderPropertiesModule();

	inline bool showProperties = false;
	inline ccColor4F g_additiveSelectColor = { 0.f, 1.f, 0.f, 1.f };
	inline ccColor4F g_subtractiveSelectColor = { 1.f, 0.f, 1.f, 1.f };
	inline ccColor4F g_intersectiveSelectColor = { 1.f, 0.6f, 0.f, 1.f };

	inline ccColor3B g_selectedObjectColor = { 0, 255, 0 };
	inline ccColor3B g_hoveredObjectColor = { 255, 255, 0 };
}