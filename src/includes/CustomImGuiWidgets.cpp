#include "CustomImGuiWidgets.hpp"
#include <imgui.h>

namespace ErGui {
	float deltaInputFloat(const char* label, float step) {
		ImGui::PushID(label);

		float result = 0;

		ImGui::Text(label);
		ImGui::SameLine(70.f);
		if (ImGui::Button("-")) {
			result = step * -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("+")) {
			result = step;
		}

		ImGui::PopID();
		return result;
	}

	int deltaInputIntImproved(const char* label, int max, int min, int step) {
		ImGui::PushID(label);

		int result = 0;
		if (ImGui::Button("-##el1Minus")) {
			result = step * -1;
		}
		std::string textStr = label;
		if (max != min)
			textStr += " (" + std::to_string(min) + ".." + std::to_string(max) + ")";
		else
			textStr += " (" + std::to_string(min) + ")";
		ImGui::SameLine();
		ImGui::Text(textStr.c_str());
		ImGui::SameLine();
		if (ImGui::Button("+##el1Plus")) {
			result = step;
		}

		ImGui::PopID();
		return result;
	}

	bool DragFloat(const char* label, float* v, float step, float step_fast, const char* format, float v_speed, float v_min, float v_max, ImGuiSliderFlags flags) {
		const float button_size = ImGui::GetFrameHeight();
		bool value_changed = false;
		auto style = ImGui::GetStyle();

		auto p_step = step > 0.0f ? &step : NULL;
		auto p_step_fast = step_fast > 0.0f ? &step_fast : NULL;

		ImGui::BeginGroup();
		ImGui::PushID(label);

		const char* labelEnd = ImGui::FindRenderedTextEnd(label);
		if (label != labelEnd) {
			ImGui::RenderText(ImGui::GetCursorScreenPos(), label, labelEnd);
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + ErGui::FIRST_ELEMENT_SAMELINE_SPACING - style.ItemInnerSpacing.x * 2, ImGui::GetCursorScreenPos().y));
		}

		ImGui::SetNextItemWidth(std::max<float>(1.0f, ImGui::CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
		if (ImGui::DragFloat("##DragFloat", v, v_speed, v_min, v_max, format, flags)) {
			value_changed = true;
		}

		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
		if (ImGui::ButtonEx("-", ImVec2(button_size, button_size)))
		{
			ImGui::DataTypeApplyOp(ImGuiDataType_Float, '-', v, v, ImGui::GetIO().KeyCtrl && p_step_fast ? p_step_fast : p_step);
			value_changed = true;
		}
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		if (ImGui::ButtonEx("+", ImVec2(button_size, button_size)))
		{
			ImGui::DataTypeApplyOp(ImGuiDataType_Float, '+', v, v, ImGui::GetIO().KeyCtrl && p_step_fast ? p_step_fast : p_step);
			value_changed = true;
		}
		ImGui::PopItemFlag();

		// ImGui::SameLine(0, style.ItemInnerSpacing.x);
		// if (labelEnd > label) {
		// 	ImVec2 pos = ImGui::GetCursorScreenPos();
		// 	ImVec2 textSize = ImGui::CalcTextSize(label, labelEnd);
		// 	ImGui::ItemSize(textSize);
		// 	ImGui::RenderText(pos, label, labelEnd);
		// }

		ImGui::PopID();
		ImGui::EndGroup();

		return value_changed;
	}

	// Draws DragFloat with a colored side
	bool DragFloat(ImVec4 color, const char* label, float* v, float step, float step_fast, const char* format, float v_speed, float v_min, float v_max, ImGuiSliderFlags flags) {
		const float button_size = ImGui::GetFrameHeight();
		bool value_changed = false;
		auto style = ImGui::GetStyle();

		auto p_step = step > 0.0f ? &step : NULL;
		auto p_step_fast = step_fast > 0.0f ? &step_fast : NULL;

		ImGui::BeginGroup();
		ImGui::PushID(label);

		const char* labelEnd = ImGui::FindRenderedTextEnd(label);
		if (label != labelEnd) {
			ImGui::RenderText(ImGui::GetCursorScreenPos(), label, labelEnd);
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + ErGui::FIRST_ELEMENT_SAMELINE_SPACING - style.ItemInnerSpacing.x * 2, ImGui::GetCursorScreenPos().y));
		}

		ImGui::SetNextItemWidth(std::max<float>(1.0f, ImGui::CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
		if (ImGui::DragFloat("##DragFloat", v, v_speed, v_min, v_max, format, flags)) {
			value_changed = true;
		}

		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		auto* drawList = ImGui::GetWindowDrawList();
		float thickness = 3.f;

		drawList->AddRectFilled(
			min,
			ImVec2(min.x + thickness, max.y),
			IM_COL32(color.x, color.y, color.z, color.w),
			ImGui::GetStyle().FrameRounding,
			ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomLeft
		);

		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
		if (ImGui::ButtonEx("-", ImVec2(button_size, button_size)))
		{
			ImGui::DataTypeApplyOp(ImGuiDataType_Float, '-', v, v, ImGui::GetIO().KeyCtrl && p_step_fast ? p_step_fast : p_step);
			value_changed = true;
		}
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		if (ImGui::ButtonEx("+", ImVec2(button_size, button_size)))
		{
			ImGui::DataTypeApplyOp(ImGuiDataType_Float, '+', v, v, ImGui::GetIO().KeyCtrl && p_step_fast ? p_step_fast : p_step);
			value_changed = true;
		}
		ImGui::PopItemFlag();

		// ImGui::SameLine(0, style.ItemInnerSpacing.x);
		// if (labelEnd > label) {
		// 	ImVec2 pos = ImGui::GetCursorScreenPos();
		// 	ImVec2 textSize = ImGui::CalcTextSize(label, labelEnd);
		// 	ImGui::ItemSize(textSize);
		// 	ImGui::RenderText(pos, label, labelEnd);
		// }

		ImGui::PopID();
		ImGui::EndGroup();

		return value_changed;
	}

	bool DragInt(const char* label, int* v, int step, int step_fast, const char* format, float v_speed, int v_min, int v_max, ImGuiSliderFlags flags) {
		const float button_size = ImGui::GetFrameHeight();
		bool value_changed = false;
		auto style = ImGui::GetStyle();

		auto p_step = step > 0 ? &step : NULL;
		auto p_step_fast = step_fast > 0 ? &step_fast : NULL;

		ImGui::BeginGroup();
		ImGui::PushID(label);
		
		const char* labelEnd = ImGui::FindRenderedTextEnd(label);
		if (label != labelEnd) {
			ImGui::RenderText(ImGui::GetCursorScreenPos(), label, labelEnd);
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + ErGui::FIRST_ELEMENT_SAMELINE_SPACING - style.ItemInnerSpacing.x * 2, ImGui::GetCursorScreenPos().y));
		}

		ImGui::SetNextItemWidth(std::max<float>(1.0f, ImGui::CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
		if (ImGui::DragInt("##DragInt", v, v_speed, v_min, v_max, format, flags)) {
			value_changed = true;
		}

		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
		if (ImGui::ButtonEx("-", ImVec2(button_size, button_size)))
		{
			ImGui::DataTypeApplyOp(ImGuiDataType_S32, '-', v, v, ImGui::GetIO().KeyCtrl && p_step_fast ? p_step_fast : p_step);
			value_changed = true;
		}
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		if (ImGui::ButtonEx("+", ImVec2(button_size, button_size)))
		{
			ImGui::DataTypeApplyOp(ImGuiDataType_S32, '+', v, v, ImGui::GetIO().KeyCtrl && p_step_fast ? p_step_fast : p_step);
			value_changed = true;
		}
		ImGui::PopItemFlag();

		// ImGui::SameLine(0, style.ItemInnerSpacing.x);
		// if (labelEnd > label) {
		// 	ImVec2 pos = ImGui::GetCursorScreenPos();
		// 	ImVec2 textSize = ImGui::CalcTextSize(label, labelEnd);
		// 	ImGui::ItemSize(textSize);
		// 	ImGui::RenderText(pos, label, labelEnd);
		// }

		ImGui::PopID();
		ImGui::EndGroup();

		return value_changed;
	}

	bool DragInt(ImVec4 color, const char* label, int* v, int step, int step_fast, const char* format, float v_speed, int v_min, int v_max, ImGuiSliderFlags flags) {
		const float button_size = ImGui::GetFrameHeight();
		bool value_changed = false;
		auto style = ImGui::GetStyle();

		auto p_step = step > 0 ? &step : NULL;
		auto p_step_fast = step_fast > 0 ? &step_fast : NULL;

		ImGui::BeginGroup();
		ImGui::PushID(label);

		const char* labelEnd = ImGui::FindRenderedTextEnd(label);
		if (label != labelEnd) {
			ImGui::RenderText(ImGui::GetCursorScreenPos(), label, labelEnd);
			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + ErGui::FIRST_ELEMENT_SAMELINE_SPACING - style.ItemInnerSpacing.x * 2, ImGui::GetCursorScreenPos().y));
		}

		ImGui::SetNextItemWidth(std::max<float>(1.0f, ImGui::CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
		if (ImGui::DragInt("##DragInt", v, v_speed, v_min, v_max, format, flags)) {
			value_changed = true;
		}

		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		auto* drawList = ImGui::GetWindowDrawList();
		float thickness = 3.f;

		drawList->AddRectFilled(
			min,
			ImVec2(min.x + thickness, max.y),
			IM_COL32(color.x, color.y, color.z, color.w),
			ImGui::GetStyle().FrameRounding,
			ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomLeft
		);

		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
		if (ImGui::ButtonEx("-", ImVec2(button_size, button_size)))
		{
			ImGui::DataTypeApplyOp(ImGuiDataType_S32, '-', v, v, ImGui::GetIO().KeyCtrl && p_step_fast ? p_step_fast : p_step);
			value_changed = true;
		}
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		if (ImGui::ButtonEx("+", ImVec2(button_size, button_size)))
		{
			ImGui::DataTypeApplyOp(ImGuiDataType_S32, '+', v, v, ImGui::GetIO().KeyCtrl && p_step_fast ? p_step_fast : p_step);
			value_changed = true;
		}
		ImGui::PopItemFlag();

		// ImGui::SameLine(0, style.ItemInnerSpacing.x);
		// if (labelEnd > label) {
		// 	ImVec2 pos = ImGui::GetCursorScreenPos();
		// 	ImVec2 textSize = ImGui::CalcTextSize(label, labelEnd);
		// 	ImGui::ItemSize(textSize);
		// 	ImGui::RenderText(pos, label, labelEnd);
		// }

		ImGui::PopID();
		ImGui::EndGroup();

		return value_changed;
	}

	void SeparatorPlus(const char* txt) {
		//const ImVec2 dummy = ImVec2(5.f, 0.f);
		ImGui::Dummy(ImVec2(0.f, 5.f));
		ImGui::SeparatorText(txt);

		//if (dummy.x < 0.f) dummy.x = 0.f;
		//if (dummy.y < 0.f) dummy.y = 0.f;
	}
}