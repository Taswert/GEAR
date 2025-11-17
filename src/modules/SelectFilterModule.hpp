#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
#include "../features/SelectFilter.hpp"
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

	void renderSelectFilter();

	bool selectFilterRealization(GameObject* obj);
	CCArray* selectFilterRealization(CCArray* objArrInRect);
}