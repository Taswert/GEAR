#pragma once
#include <Geode/Geode.hpp>
using namespace cocos2d;
using namespace geode::prelude;

namespace ErGui {
	// Maybe this thing should be somewhere else...
	inline const char* objectTypes[] = {
		"Solid", "Basic (Unused)", "Hazard", "Inverse Gravity Portal", "Normal Gravity Portal", "Ship Portal", "Cube Portal", "Decoration", "Yellow Jump Pad", "Pink Jump Pad", "Gravity Pad", "Yellow Jump Orb", "Pink Jump Orb", "Gravity Orb", "Inverse Mirror Portal", "Normal Mirror Portal", "Ball Portal", "Regular Size Portal", "Mini Size Portal", "UFO Portal", "Modifier", "Breakable", "Secret Coin", "Dual Portal", "Solo Portal", "Slope", "Wave Portal", "Robot Portal", "Teleport Portal", "Green Orb", "Collectible", "User Coin", "Drop Orb", "Spider Portal", "Red Jump Pad", "Red Jump Orb", "Custom Orb", "Dash Orb", "Gravity Dash Orb", "Collision Object", "Special", "Swing Portal", "Gravity Toggle Portal", "Spider Orb", "Spider Pad", "Enter Effect Object", "Teleport Orb", "Animated Hazard",
	};

	inline bool filterByObjects = false;
	inline bool filterByMainColors = false;
	inline bool filterByDetailColors = false;
	inline bool filterByGroups = false;
	inline bool filterByChannel = false;
	inline bool filterByType = false;

	inline int filterModifier = 0;

	inline std::unordered_set<int> objectsFilterSet;
	inline std::unordered_set<int> mainColorsFilterSet;
	inline std::unordered_set<int> detailColorsFilterSet;
	inline std::unordered_set<int> groupsFilterSet;
	inline std::unordered_set<int> channelFilterSet;
	inline std::unordered_set<int> objectTypeSet;


	bool selectFilterRealization(GameObject* obj);
	CCArray* selectFilterRealization(CCArray* objArrInRect);
}