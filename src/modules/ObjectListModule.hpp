#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
#include "razoomUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	CCSprite* getObjectSprite(int id);
	void renderObjectList();
	void addRecentObjectToList(int objId);
	void clearObjectListCache();
	void reloadCustomObjects();
}