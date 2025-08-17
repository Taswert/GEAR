#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"

using namespace geode::prelude;

namespace ErGui {
	static float moveStep = 30.f;
	static float rotationStep = 90.f;
	static float scaleStep = 0.5f;
	static float skewStep = 15.f;

	static bool rotLock = true;
	static bool scaleLock = false;

	static float angleArc = 180.f;
	static float angleStep = 5.f;
	static float angleSquish = 0.f;
	
	static bool rotateObjectsPositionSnap = false;
	static bool scaleObjectsPositionSnap = false;
	static bool skewObjectsPositionSnap = false;

	void renderTransformation();
}