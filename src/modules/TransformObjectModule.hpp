#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"

using namespace geode::prelude;

namespace ErGui {
	static bool rotLock = true;
	static bool scaleLock = false;

	static float angleArc = 180.f;
	static float angleStep = 5.f;
	static float angleSquish = 0.f;

	static int stackCopies = 1;
	static float stackXOffset = 0.f;
	static float stackYOffset = 0.f;
	static float stackRotate = 0.f;
	
	static bool rotateObjectsPositionSnap = false;
	static bool scaleObjectsPositionSnap = false;
	static bool skewObjectsPositionSnap = false;

	void renderTransformation();
}