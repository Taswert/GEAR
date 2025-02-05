#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	static int chosenMainColorSFM = 0;
	static int offsetMainColorSFM = 0;
	static int chosenDetailColorSFM = 0;
	static int offsetDetailColorSFM = 0;
	static int chosenGroupSFM = 0;
	static int offsetGroupSFM = 0;
	static int chosenChannelSFM = 0;
	static int offsetChannelSFM = 0;

	static std::unordered_set<int> objectsFilterSet;
	static std::unordered_set<int> mainColorsFilterSet;
	static std::unordered_set<int> detailColorsFilterSet;
	static std::unordered_set<int> groupsFilterSet;
	static std::unordered_set<int> channelFilterSet;

	static bool filterByObjects = false;
	static bool filterByMainColors = false;
	static bool filterByDetailColors = false;
	static bool filterByGroups = false;
	static bool filterByChannel = false;
	static bool filterOr = false;

	void renderSelectFilter();
}