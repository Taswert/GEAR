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

	// Stack Tool thing

	static int stackCopies = 1;

	static float stackXOffset = 0.f;
	static float stackYOffset = 0.f;

	static float stackRotate = 0.f;
	static float stackXRotate = 0.f;
	static float stackYRotate = 0.f;

	static float stackScale = 0.f;
	static float stackXScale = 0.f;
	static float stackYScale = 0.f;

	static int stackZOrder = 0;

	//
	
	inline bool rotateObjectsPositionSnap = false;
	inline bool scaleObjectsPositionSnap = false;
	inline bool skewObjectsPositionSnap = false;
	inline bool flipObjectsPositionSnap = false;

	void renderTransformation();
}