#pragma once
#include <imgui_internal.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	float deltaInputFloat(const char* label, float step);
	int deltaInputIntImproved(const char* label, int max, int min, int step);
	bool DragFloat(const char* label, float* v, float step = 1, float step_fast = 5, const char* format = "%.3f", float v_speed = 1.f, float v_min = 0.f, float v_max = 0.f, ImGuiSliderFlags flags = 0);
	bool DragFloat(ImVec4 color, const char* label, float* v, float step = 1, float step_fast = 5, const char* format = "%.3f", float v_speed = 1.f, float v_min = 0.f, float v_max = 0.f, ImGuiSliderFlags flags = 0);
	bool DragInt(const char* label, int* v, int step = 1, int step_fast = 5, const char* format = "%d", float v_speed = 1.f, int v_min = 0, int v_max = 0, ImGuiSliderFlags flags = 0);
	bool DragInt(ImVec4 color, const char* label, int* v, int step = 1, int step_fast = 5, const char* format = "%d", float v_speed = 1.f, int v_min = 0, int v_max = 0, ImGuiSliderFlags flags = 0);
	void SeparatorPlus(const char* txt);
}