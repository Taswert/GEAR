#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	inline const char* easingItems[] = { "None",
		"Ease In Out",			"Ease In",			"Ease Out",
		"Elastic In Out",		"Elastic In",		"Elastic Out",
		"Bounce In Out",		"Bounce In",		"Bounce Out",
		"Exponential In Out",	"Exponential In",	"Exponential Out",
		"Sine In Out",			"Sine In",			"Sine Out",
		"Back In Out",			"Back In",			"Back Out"
	};

	void drawComponentGroupID(EffectGameObject* eObj, CCArray* objArr, const char* targetGroupLabel, const char* centerGroupLabel);
	void drawComponentGroupID(EffectGameObject* eObj, CCArray* objArr, const char* targetGroupLabel);
	void drawComponentTime(EffectGameObject* eObj, CCArray* objArr, const char* label, bool enableEasing);

	void drawEasingSettings(EffectGameObject* eObj, CCArray* objArr);
	void drawTouchSpawnTriggered(EffectGameObject* eObj, CCArray* objArr);
}