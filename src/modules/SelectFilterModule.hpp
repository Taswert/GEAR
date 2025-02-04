#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	static int chosenColorSFM = 0;
	static int chosenGroupSFM = 0;

	static std::vector<int> objectsFilterArr;
	static std::vector<int> mainColorsFilterArr;
	static std::vector<int> groupsFilterArr;

	static bool filterByObjects = false;
	static bool filterByMainColors = false;
	static bool filterByGroups = false;
	static bool filterOr = false;

	void renderSelectFilter();
}