#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	inline cocos2d::CCPoint originalCameraPosition;

	inline bool og_prevAnim = false;
	inline bool og_prevMode = false;
	inline bool og_prevPart = false;
	inline bool og_prevShad = false;

	inline float cam2_offsetX = 0.f;
	inline float cam2_offsetY = 0.f;

	inline bool cam2_prevAnim = false;
	inline bool cam2_prevMode = false;
	inline bool cam2_prevPart = false;
	inline bool cam2_prevShad = false;

	void renderCameraSettings();
}