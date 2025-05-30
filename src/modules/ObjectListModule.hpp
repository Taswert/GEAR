#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
#include "razoomUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	void renderObjectList();
	void addRecentObjectToList(int objId);
	void clearObjectListCache();
	void reloadCustomObjects();
}