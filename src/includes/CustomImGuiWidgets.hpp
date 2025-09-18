#pragma once
#include <imgui_internal.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	float deltaInputFloat(const char* label, float step);
	int deltaInputIntImproved(const char* label, int max, int min, int step);
	bool BetterDragFloat(const char* label, float* v, float step = 1, float step_fast = 5, const char* format = "%.3f", float v_speed = 1.f, float v_min = 0.f, float v_max = 0.f, ImGuiSliderFlags flags = 0);
}