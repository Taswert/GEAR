#pragma once
#include <imgui_internal.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	void renderContextMenu();

	inline CCTouch* rightTouch = nullptr;
	inline bool shouldOpenContextMenu = false;
	inline GameObject* objectUnderCursor = nullptr;
	inline bool isContextMenuOpen = false;
}