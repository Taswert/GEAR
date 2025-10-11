#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	inline int chosenMainColorSFM = 0;
	inline int offsetMainColorSFM = 0;
	inline int chosenDetailColorSFM = 0;
	inline int offsetDetailColorSFM = 0;
	inline int chosenGroupSFM = 0;
	inline int offsetGroupSFM = 0;
	inline int chosenChannelSFM = 0;
	inline int offsetChannelSFM = 0;
	inline int chosenTypeSFM = 0;

	inline std::unordered_set<int> objectsFilterSet;
	inline std::unordered_set<int> mainColorsFilterSet;
	inline std::unordered_set<int> detailColorsFilterSet;
	inline std::unordered_set<int> groupsFilterSet;
	inline std::unordered_set<int> channelFilterSet;
	inline std::unordered_set<int> objectTypeSet;

	inline bool filterByObjects = false;
	inline bool filterByMainColors = false;
	inline bool filterByDetailColors = false;
	inline bool filterByGroups = false;
	inline bool filterByChannel = false;
	inline bool filterByType = false;

	inline int filterModifier = 0;

	void renderSelectFilter();

	bool selectFilterRealization(GameObject* obj);
	CCArray* selectFilterRealization(CCArray* objArrInRect);
}