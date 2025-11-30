#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
#include "../features/Selection.hpp"
#include "CustomImGuiWidgets.hpp"
#include "misc/cpp/imgui_stdlib.h"
#include "EditObjectUIComponents.hpp"
using namespace geode::prelude;

#define APPLY_FIELDS_TO_OTHER_TRIGGERS(m_field, valueToSet, TriggerClass)	\
for (auto objInArr : CCArrayExt<TriggerClass*>(objArr)) {					\
	objInArr->m_field = valueToSet;											\
}

// LevelEditorLayer should be defined as lel at this point pls
#define APPLY_FIELDS_TO_OTHER_TRIGGERS_AND_UPDATE(m_field, valueToSet, TriggerClass)	\
for (auto objInArr : CCArrayExt<TriggerClass*>(objArr)) {								\
	objInArr->m_field = valueToSet;														\
	lel->updateObjectLabel(objInArr);													\
}

namespace ErGui {

	inline const std::unordered_set<int> triggerSet = { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 55, 56, 57, 58, 59, 105, 744, 900, 915, 899, 901, 914, 1616, 1006, 1007, 1049, 1268, 1346, 2067, 1347, 1520, 1585, 1912, 3033, 1814, 1915, 2063, 3016, 3617, 3660, 3661, 3032, 3006, 3007, 3008, 3009, 3010, 3011, 3012, 3013, 3014, 3015, 3024, 3029, 3030, 3031, 1595, 1611, 1811, 1817, 3608, 3614, 3615, 3617, 3619, 3620, 3641, 1912, 2068, 3607, 3608, 3618, 1913, 1914, 1916, 2901, 2015, 2062, 2925, 2016, 1917, 2900, 1934, 3605, 3602, 3603, 3604, 1935, 2999, 3606, 3612, 1615, 3613, 3662, 1815, 3609, 3640, 1816, 3643, 1812, 33, 32, 1613, 1612, 1818, 1819, 3600, 1932, 2899, 3642, 2903, 2066, 3022, 2904, 2905, 2907, 2909, 2910, 2911, 2912, 2913, 2914, 2915, 2916, 2917, 2919, 2920, 2921, 2922, 2923, 2924, 22, 24, 23, 25, 26, 27, 28, 55, 56, 57, 58, 59, 1915, 3017, 3018, 3019, 3020, 3021, 3023, 29, 30, 105, 744, 915, 1931, 3655, 2069, 3645, 747, 2902 };
	using ErGuiSettingsDrawer = void (*)(GameObject*, CCArray*);
	inline std::unordered_map<int, ErGuiSettingsDrawer> triggersMap;

	inline std::set<int> oldColorTriggers = { 29, 30, 105, 744, 900, 915 };

	inline const char* moveTargetModeItems[] = {
		"Both", "X Only", "Y Only"
	};

	inline const char* pulseModeItems[] = {
		"Color Mode", "HSV Mode"
	};

	inline const char* pulseTargetTypeItems[] = {
		"Channel", "Group"
	};

	inline const char* gameModesItems[] = {
		"Cube", "Ship", "Ball", "UFO", "Wave", "Robot", "Spider", "Swing"
	};

	inline const char* speedItems[] = {
		"Slow", "Normal", "Fast", "Faster", "Very Fast"
	};

	inline const int speedValues[] = {
		1, 0, 2, 3, 4,
	};

	inline const char* enterType[] = {
		"None", "Enter Only", "Exit Only"
	};

	inline const char* edgeDirection[] = {
		"None", "Left", "Right", "Up", "Down"
	};

	inline int spawnOldGroupID = 0;
	inline int spawnNewGroupID = 0;

	inline float savedHueEO, savedSaturationEO, savedValueEO;


	void saveHueValues(ccColor3B* color);
	bool isOldColorTrigger(GameObject* obj);

	void setupTriggersSettings();
	void renderEditObjectModule();

	void SeparatorPlus(const char* txt);
}