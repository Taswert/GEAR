#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	static float moveStep = 30.f;
	static float rotationStep = 90.f;
	static float scaleStep = 0.5f;
	static float skewStep = 15.f;
	
	static bool rotateObjectsPositionSnap = false;
	static bool scaleObjectsPositionSnap = false;
	static bool skewObjectsPositionSnap = false;

	void renderTransformation();
}