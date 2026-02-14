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
	// todo: sort this shit
    inline const std::unordered_set<int> triggerSet = { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 55, 56, 57, 58, 59, 105, 744, 900, 915, 899, 901, 914, 1616, 1006, 1007, 1049, 1268, 1346, 2067, 1347, 1520, 1585, 1912, 3033, 1814, 1915, 2063, 3016, 3617, 3660, 3661, 3032, 3006, 3007, 3008, 3009, 3010, 3011, 3012, 3013, 3014, 3015, 3024, 3029, 3030, 3031, 1595, 1611, 1811, 1817, 3608, 3614, 3615, 3617, 3619, 3620, 3641, 1912, 2068, 3607, 3608, 3618, 1913, 1914, 1916, 2901, 2015, 2062, 2925, 2016, 1917, 2900, 1934, 3605, 3602, 3603, 3604, 1935, 2999, 3606, 3612, 1615, 3613, 3662, 1815, 3609, 3640, 1816, 3643, 1812, 33, 32, 1613, 1612, 1818, 1819, 3600, 1932, 2899, 3642, 2903, 2066, 3022, 2904, 2905, 2907, 2909, 2910, 2911, 2912, 2913, 2914, 2915, 2916, 2917, 2919, 2920, 2921, 2922, 2923, 2924, 22, 24, 23, 25, 26, 27, 28, 55, 56, 57, 58, 59, 1915, 3017, 3018, 3019, 3020, 3021, 3023, 29, 30, 105, 744, 915, 1931, 3655, 2069, 3645, 747, 2902, 1050, 1051, 1052, 3000, 3001, 3002, 1591, 1593, 1053, 1054, 1592, 1516, 920, 923, 924, 921, 2874, 2034, 2865, 2864, 1936, 1937, 1938, 1939, 1583, 1851, 1852, 1854, 1849, 1850, 1518, 1853, 1855, 1858, 1519, 1618, 2040, 2044, 1697, 1698, 1699, 1857, 1860, 2882, 2883, 2884, 2885, 2886, 2887, 2888, 2889, 2890, 2891, 2892, 2893, 1839, 1841, 1842, 2035, 2036, 2038, 2048, 2049, 2050, 2037, 2039, 2869, 2870, 2871, 2052, 2053, 2868, 2020, 2021, 2041, 2042, 2026, 2027, 2028, 2867, 2046, 2047, 2051, 2055, 2873, 2879, 2876, 2880, 2045, 2029, 2223, 2246, 2629, 2630, 3303, 3304, 3482, 3483, 3484, 2605, 2694, 4211, 3119, 3120, 3121, 3219, 1856, 2022, 2023, 2024, 2025, 2030, 2031, 2032, 2033, 2043, 2054, 2872, 2875, 2877, 2878, 2881, 2894, 3492, 3493, 4300, 1329, 1614,
		// Collectibles
		3601, 1275, 1587, 1589, 1598, 4401, 4402, 4403, 4404, 4405, 4406, 4518, 4410, 4411, 4412, 4421, 4529, 4530, 4531, 4532, 4414, 4415, 4416, 4417, 4418, 4419, 4420, 4413, 4422, 4424, 4425, 4426, 4521, 4526, 4407, 4408, 4409, 4427, 4428, 4429, 4430, 4438, 4439, 4440, 4441, 4442, 4443, 4446, 4447, 4448, 4449, 4519, 4451, 4452, 4453, 4454, 4455, 4456, 4457, 4458, 4459, 4460, 4461, 4462, 4463, 4464, 4522, 4465, 4466, 4467, 4468, 4469, 4470, 4471, 4472, 4473, 4474, 4475, 4476, 4477, 4480, 4481, 4482, 4483, 4479, 4478, 4484, 4485, 4486, 4487, 4488, 4538, 4489, 4490, 4491, 4492, 4493, 4494, 4495, 4496, 4497, 4537, 4527, 4528, 4434, 4435, 4498, 4499, 4500, 4501, 4502, 4503, 4504, 4505, 4506, 4507, 4508, 4509, 4510, 4511, 4512, 4513, 4514, 4533, 4534, 4535, 4431, 4432, 4433, 4436, 4437, 4444, 4445, 4450, 4515, 4516, 4517, 4520, 4523, 4524, 4525, 4536, 4539
	};
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