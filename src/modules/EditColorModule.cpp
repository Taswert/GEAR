	#pragma once
#include "EditColorModule.hpp"
#include <Geode/GeneratedPredeclare.hpp>
#include <cocos2d.h>

int nextFreeOffset = 1;
const char* colorTypes[] = {
	"Default", "Base", "Detail"
};
GameObject* refObject = nullptr;
int objArrOldCount = 0;

float savedHueEC = 0;
float savedSaturationEC = 0;
float savedValueEC = 0;

void ErGui::clampHSV(ccHSVValue* hsvValue) {
	if (!hsvValue->absoluteSaturation && hsvValue->s < 0.f)
		 hsvValue->s = 0.f;
	if (!hsvValue->absoluteBrightness && hsvValue->v < 0.f)
		 hsvValue->v = 0.f;

	if (hsvValue->absoluteSaturation && hsvValue->s > 1.f)
		hsvValue->s = 1.f;
	if (hsvValue->absoluteBrightness && hsvValue->v > 1.f)
		hsvValue->v = 1.f;
}


std::string colorPickerPopup(std::string btnStr, ColorAction* ccMyColor) {
	auto colEditPopupStr = btnStr + "-EDIT-POPUP";
	
	if (ImGui::BeginPopup(colEditPopupStr.c_str())) {
		float r, g, b;
		float hsv[4] = { savedHueEC, savedSaturationEC, savedValueEC, ccMyColor->m_fromOpacity };
		ImGuiColorEditFlags flags =
			ImGuiColorEditFlags_InputHSV  | ImGuiColorEditFlags_AlphaBar;
		if (geode::Mod::get()->getSavedValue<bool>("triangle-color-wheel"))
			flags |= ImGuiColorEditFlags_PickerHueWheel;
		if (!geode::Mod::get()->getSavedValue<bool>("rotate-color-wheel"))
			flags |= ImGuiColorEditFlags_DisableWheelRot;

		if (ccMyColor->m_copyID == 0) {
			ImGui::ColorPicker4((btnStr + "-EDIT").c_str(), hsv, flags);

			savedHueEC = hsv[0];
			savedSaturationEC = hsv[1];
			savedValueEC = hsv[2];
			ccMyColor->m_fromOpacity = hsv[3];

			ImGui::ColorConvertHSVtoRGB(savedHueEC, savedSaturationEC, savedValueEC, r, g, b);
			ccMyColor->m_fromColor = ccColor3B(int(r * 255), int(g * 255), int(b * 255));
		}
		else {
			ImGui::Text("Hue");
			ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			ImGui::DragFloat("##Hue", &ccMyColor->m_copyHSV.h, 1.f, -180.f, 180.f);

			ImGui::Text("Saturation");
			ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			ImGui::DragFloat("##Saturation", &ccMyColor->m_copyHSV.s, 0.05f, -1.f, 2.f);
			ImGui::SameLine();
			ImGui::Checkbox("##AbsoluteSat", &ccMyColor->m_copyHSV.absoluteSaturation);

			ImGui::Text("Value");
			ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			ImGui::DragFloat("##Value", &ccMyColor->m_copyHSV.v, 0.05f, -1.f, 2.f);
			ImGui::SameLine();
			ImGui::Checkbox("##AbsoluteVal", &ccMyColor->m_copyHSV.absoluteBrightness);
			//ImGui::Checkbox("Legacy HSV##what", &ccMyColor->m_legacyHSV); TRIGGER FUNCTION

			ErGui::clampHSV(&ccMyColor->m_copyHSV);

			if (ImGui::Button("HSV Reset##HSV-RESET")) {
				ccMyColor->m_copyHSV.absoluteBrightness = false;
				ccMyColor->m_copyHSV.absoluteSaturation = false;
				ccMyColor->m_copyHSV.h = 0.f;
				ccMyColor->m_copyHSV.s = 1.f;
				ccMyColor->m_copyHSV.v = 1.f;
			}

			ImGui::Checkbox("Copy Opacity", &ccMyColor->m_copyOpacity);
			if (!ccMyColor->m_copyOpacity)
				ImGui::DragFloat("Opacity", &ccMyColor->m_fromOpacity, 0.05f, 0.f, 1.f);
			
		}

		ImGui::Checkbox("Blending", &ccMyColor->m_blending);

		auto ccColorAction = GameManager::sharedState()->m_levelEditorLayer->m_levelSettings->m_effectManager->getColorAction(ccMyColor->m_copyID);
		if (ImGui::ColorButton(btnStr.c_str(), ImVec4(ccColorAction->m_fromColor.r / 255.f, ccColorAction->m_fromColor.g / 255.f, ccColorAction->m_fromColor.b / 255.f, 1.f))) {
			ImGui::OpenPopup("CSP##COPY");
		}

		ErGui::colorSelectImGuiPopup(&ccMyColor->m_copyID, "CSP##COPY", true);
		if (ccMyColor->m_copyID == ccMyColor->m_colorID) ccMyColor->m_copyID = 0;

		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.f);
		int copyID = ccMyColor->m_copyID;
		if (ImGui::InputInt(("##INPUT-COPYCOLOR"), &copyID)) {
			if (ccMyColor->m_colorID == copyID && ccMyColor->m_copyID < copyID) copyID++;
			if (ccMyColor->m_colorID == copyID && ccMyColor->m_copyID > copyID) copyID--;

			if (copyID > 1101)					copyID = 1101;
			if (copyID == ccMyColor->m_colorID)	copyID = 1100;
			if (copyID < 0)						copyID = 0;

			ccMyColor->m_copyID = copyID;
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset")) {
			ccMyColor->m_copyID = 0;
		}
		ImGui::SameLine();
		ImGui::Text("Copy Color");

		ImGui::EndPopup();
	}
	return colEditPopupStr;
}

bool ErGui::colorSelectImGuiPopup(int* colorId, std::string popupStr, bool colorEditRestriction) {
	bool result = false;
	ImGui::SetNextWindowSizeConstraints(ImVec2(100, 0), ImVec2(FLT_MAX, 350));
	if (ImGui::BeginPopup(popupStr.c_str())) {
		for (int i = colorEditRestriction ? 0 : 1; i < 1102; i++) {
			auto effectManager = GameManager::sharedState()->m_levelEditorLayer->m_levelSettings->m_effectManager;
			auto ccMyColor = effectManager->getColorAction(i);

			std::string suffix = "";

			switch (i) {
			default:
				break;
			case 1000:
				suffix = "-BG";
				break;
			case 1001:
				suffix = "-G1";
				break;
			case 1002:
				suffix = "-LINE";
				break;
			case 1003:
				suffix = "-3DL";
				break;
			case 1004:
				suffix = "-OBJECT";
				break;
			case 1005:
				suffix = "-P1";
				break;
			case 1006:
				suffix = "-P2";
				break;
			case 1007:
				suffix = "-LBG";
				break;
			case 1009:
				suffix = "-G2";
				break;
			case 1010:
				suffix = "-BLACK";
				break;
			case 1011:
				suffix = "-WHITE";
				break;
			case 1012:
				suffix = "-LIGHTER";
				break;
			case 1013:
				suffix = "-MG";
				break;
			case 1014:
				suffix = "-MG2";
				break;
			}

			std::string btnStr = std::string("color") + std::to_string(i) + suffix + std::string("##COLOR-BUTTON-POPUP");

			if (ImGui::ColorButton(btnStr.c_str(), ImVec4(ccMyColor->m_fromColor.r / 255.f, ccMyColor->m_fromColor.g / 255.f, ccMyColor->m_fromColor.b / 255.f, 1.f))) {
				*colorId = i;
				ImGui::CloseCurrentPopup();
				result = true;
			}

			auto colEditPopupStr = colorPickerPopup(btnStr, ccMyColor);

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && i != 1005 && i != 1006 && i != 1010 && i != 1011 && !colorEditRestriction) {
				//saving hsv beacuse this stupid game adjusting hue otherwise for whatever reason
				float r = ccMyColor->m_fromColor.r / 255.f;
				float g = ccMyColor->m_fromColor.g / 255.f;
				float b = ccMyColor->m_fromColor.b / 255.f;
				ImGui::ColorConvertRGBtoHSV(r, g, b, savedHueEC, savedSaturationEC, savedValueEC);
				ImGui::OpenPopup(colEditPopupStr.c_str());
			}

			ImGui::SameLine();
			if (!suffix.empty()) {
				suffix.erase(suffix.begin());
				ImGui::Text(suffix.c_str());
			}
			else {
				ImGui::Text(std::to_string(i).c_str());
			}

			if (i % 6 != 0) ImGui::SameLine(70 * (i % 6));
		}
		ImGui::EndPopup();
	}
	return result;
}

void colorTypeFilter(GJSpriteColor* spriteColor, bool isDetail, CCArray* objArr) {
	if (objArr) {
		for (auto obj : CCArrayExt<GameObject*>(objArr)) {
			if (!isDetail) {
				if (!obj->m_detailColor && obj->m_customColorType != 2 && ((obj->m_customColorType == 0 && !obj->m_isDecoration) || obj->m_customColorType == 1)) {
					obj->m_baseColor->m_colorID = spriteColor->m_colorID;
				}
				else if (obj->m_detailColor) {
					obj->m_baseColor->m_colorID = spriteColor->m_colorID;
				}
			}

			else if (isDetail) {
				if (!obj->m_detailColor && obj->m_customColorType != 1 && ((obj->m_customColorType == 0 && obj->m_isDecoration) || obj->m_customColorType == 2)) {
					obj->m_baseColor->m_colorID = spriteColor->m_colorID;
				}
				else if (obj->m_detailColor) {
					obj->m_detailColor->m_colorID = spriteColor->m_colorID;
				}
			}
		}
	}
}

void colorButtonSelect(GJSpriteColor* spriteColor, bool isDetail, CCArray* objArr) {
	if (spriteColor) {
		auto ccColorAction = GameManager::sharedState()->m_levelEditorLayer->m_levelSettings->m_effectManager->getColorAction(spriteColor->m_colorID);
		std::string itemName = isDetail ? "Detail" : "Base";
		std::string btnStr = "colorButton##" + itemName;
		std::string cspStr = "CSP##" + itemName;
		std::string inputIntStr = "##" + itemName;
		std::string nextFreeStr = "Next Free##" + itemName;

		if (ImGui::ColorButton(btnStr.c_str(), ImVec4(ccColorAction->m_fromColor.r / 255.f, ccColorAction->m_fromColor.g / 255.f, ccColorAction->m_fromColor.b / 255.f, 1.f))) {
			ImGui::OpenPopup(cspStr.c_str());
		}

		auto colEditPopupStr = colorPickerPopup(btnStr, ccColorAction);
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
			spriteColor->m_colorID != 1005 && spriteColor->m_colorID != 1006 &&
			spriteColor->m_colorID != 1010 && spriteColor->m_colorID != 1011) {

			//saving hsv beacuse this stupid game adjusting hue otherwise for whatever reason
			float r = ccColorAction->m_fromColor.r / 255.f;
			float g = ccColorAction->m_fromColor.g / 255.f;
			float b = ccColorAction->m_fromColor.b / 255.f;
			ImGui::ColorConvertRGBtoHSV(r, g, b, savedHueEC, savedSaturationEC, savedValueEC);

			ImGui::OpenPopup(colEditPopupStr.c_str());
		}

		if (ErGui::colorSelectImGuiPopup(&spriteColor->m_colorID, cspStr.c_str(), false)) {
			colorTypeFilter(spriteColor, isDetail, objArr);
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.f);
		if (ImGui::InputInt(inputIntStr.c_str(), &spriteColor->m_colorID)) {
			if (spriteColor->m_colorID > 1101) spriteColor->m_colorID = 1101;
			if (spriteColor->m_colorID < 1) spriteColor->m_colorID = 1;

			colorTypeFilter(spriteColor, isDetail, objArr);
		}
		
		ImGui::SameLine();
		if (ImGui::Button(nextFreeStr.c_str())) {
			for (int i = nextFreeOffset; i < 1102; i++) {
				auto iColorAction = GameManager::sharedState()->m_levelEditorLayer->m_levelSettings->m_effectManager->getColorAction(i);
				if (iColorAction->m_fromColor.r == 255 && iColorAction->m_fromColor.g == 255 && iColorAction->m_fromColor.b == 255) {
					spriteColor->m_colorID = i;

					colorTypeFilter(spriteColor, isDetail, objArr);
					break;
				}
			}
		}
		
		ImGui::SameLine();
		ImGui::Text(itemName.c_str());
	}
}


void renderForObjectEC(GameObject* obj) {
	auto gjBaseColor = obj->m_baseColor;
	auto gjDetailColor = obj->m_detailColor;

	if (ImGui::Button("Copy")) {
		GameManager::sharedState()->m_levelEditorLayer->copyObjectState(obj);
	}
	ImGui::SameLine();
	if (ImGui::Button("Paste") && GameManager::sharedState()->m_levelEditorLayer->m_copyStateObject) {
		auto copyStateObj = GameManager::sharedState()->m_levelEditorLayer->m_copyStateObject;
		gjBaseColor->m_colorID = copyStateObj->m_baseColor->m_colorID;
		gjBaseColor->m_usesHSV = copyStateObj->m_baseColor->m_usesHSV;
		gjBaseColor->m_hsv = copyStateObj->m_baseColor->m_hsv;
		gjBaseColor->m_opacity = copyStateObj->m_baseColor->m_opacity;
		if (gjDetailColor) {
			gjDetailColor->m_colorID = copyStateObj->m_detailColor->m_colorID;
			gjDetailColor->m_usesHSV = copyStateObj->m_detailColor->m_usesHSV;
			gjDetailColor->m_hsv = copyStateObj->m_detailColor->m_hsv;
			gjDetailColor->m_opacity = copyStateObj->m_detailColor->m_opacity;
		}

		obj->m_customColorType = copyStateObj->m_customColorType;
	}

	colorButtonSelect(gjBaseColor, false, nullptr);
	colorButtonSelect(gjDetailColor, true, nullptr);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Offset", &nextFreeOffset)) {
		if (nextFreeOffset > 1101)	nextFreeOffset = 1101;
		if (nextFreeOffset < 1)		nextFreeOffset = 1;
	}

	if (ImGui::Checkbox("Visible Selection", &obj->m_isSelected)) {
		if (obj->m_isSelected) {
			reinterpret_cast<void(__thiscall*)(EditorUI*)>(geode::base::get() + 0x11dd50)(GameManager::sharedState()->m_levelEditorLayer->m_editorUI); //->resetSelectedObjectsColor();
		}
	}

	if (obj->m_detailColor) {
		ImGui::Text("Hue");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		ImGui::DragFloat("##Hue1", &gjBaseColor->m_hsv.h, 1.f, -180.f, 180.f);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		ImGui::DragFloat("##Hue2", &gjDetailColor->m_hsv.h, 1.f, -180.f, 180.f);

		ImGui::Text("Saturation");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		ImGui::DragFloat("##Saturation1", &gjBaseColor->m_hsv.s, 0.05f, -1.f, 2.f);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		ImGui::DragFloat("##Saturation2", &gjDetailColor->m_hsv.s, 0.05f, -1.f, 2.f);
		ImGui::SameLine();
		ImGui::Checkbox("##AbsoluteSat1", &gjBaseColor->m_hsv.absoluteSaturation);
		ImGui::SameLine();
		ImGui::Checkbox("##AbsoluteSat2", &gjDetailColor->m_hsv.absoluteSaturation);

		ImGui::Text("Value");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		ImGui::DragFloat("##Value1", &gjBaseColor->m_hsv.v, 0.05f, -1.f, 2.f);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		ImGui::DragFloat("##Value2", &gjDetailColor->m_hsv.v, 0.05f, -1.f, 2.f);
		ImGui::SameLine();
		ImGui::Checkbox("##AbsoluteVal1", &gjBaseColor->m_hsv.absoluteBrightness);
		ImGui::SameLine();
		ImGui::Checkbox("##AbsoluteVal2", &gjDetailColor->m_hsv.absoluteBrightness);

		ErGui::clampHSV(&gjBaseColor->m_hsv);
		ErGui::clampHSV(&gjDetailColor->m_hsv);

		ImGui::Text("HSV Reset");
		if (ImGui::Button("Base##HSV-RESET")) {
			gjBaseColor->m_hsv.absoluteBrightness = false;
			gjBaseColor->m_hsv.absoluteSaturation = false;
			gjBaseColor->m_hsv.h = 0.f;
			gjBaseColor->m_hsv.s = 1.f;
			gjBaseColor->m_hsv.v = 1.f;
		}
		ImGui::SameLine();
		if (ImGui::Button("Detail##HSV-RESET")) {
			gjDetailColor->m_hsv.absoluteBrightness = false;
			gjDetailColor->m_hsv.absoluteSaturation = false;
			gjDetailColor->m_hsv.h = 0.f;
			gjDetailColor->m_hsv.s = 1.f;
			gjDetailColor->m_hsv.v = 1.f;
		}
	}
	else {
		ImGui::Text("Hue");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("##Hue", &gjBaseColor->m_hsv.h, 1.f, -180.f, 180.f);

		ImGui::Text("Saturation");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("##Saturation", &gjBaseColor->m_hsv.s, 0.05f, -1.f, 2.f);
		ImGui::SameLine();
		ImGui::Checkbox("##AbsoluteSat", &gjBaseColor->m_hsv.absoluteSaturation);

		ImGui::Text("Value");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("##Value", &gjBaseColor->m_hsv.v, 0.05f, -1.f, 2.f);
		ImGui::SameLine();
		ImGui::Checkbox("##AbsoluteVal", &gjBaseColor->m_hsv.absoluteBrightness);

		ErGui::clampHSV(&gjBaseColor->m_hsv);

		if (ImGui::Button("HSV Reset##HSV-RESET")) {
			gjBaseColor->m_hsv.absoluteBrightness = false;
			gjBaseColor->m_hsv.absoluteSaturation = false;
			gjBaseColor->m_hsv.h = 0.f;
			gjBaseColor->m_hsv.s = 1.f;
			gjBaseColor->m_hsv.v = 1.f;
		}

		ImGui::Combo("Color Type", reinterpret_cast<int*>(&obj->m_customColorType), colorTypes, IM_ARRAYSIZE(colorTypes));
	}

	objArrOldCount = 1;
}


void renderForArrayEC(CCArray* objArr) {
	if (ImGui::Button("Paste") && GameManager::sharedState()->m_levelEditorLayer->m_copyStateObject) {
		for (auto obj : CCArrayExt<GameObject*>(objArr)) {
			auto gjBaseColor = obj->m_baseColor;
			auto gjDetailColor = obj->m_detailColor;
			auto copyStateObj = GameManager::sharedState()->m_levelEditorLayer->m_copyStateObject;

			gjBaseColor->m_colorID = copyStateObj->m_baseColor->m_colorID;
			gjBaseColor->m_usesHSV = copyStateObj->m_baseColor->m_usesHSV;
			gjBaseColor->m_hsv = copyStateObj->m_baseColor->m_hsv;
			gjBaseColor->m_opacity = copyStateObj->m_baseColor->m_opacity;
			if (gjDetailColor) {
				gjDetailColor->m_colorID = copyStateObj->m_detailColor->m_colorID;
				gjDetailColor->m_usesHSV = copyStateObj->m_detailColor->m_usesHSV;
				gjDetailColor->m_hsv = copyStateObj->m_detailColor->m_hsv;
				gjDetailColor->m_opacity = copyStateObj->m_detailColor->m_opacity;
			}

			obj->m_customColorType = copyStateObj->m_customColorType;
		}
	}


	if (!refObject || objArrOldCount != objArr->count()) {
		bool foundEtalon = false;
		for (auto obj : CCArrayExt<GameObject*>(objArr)) {
			if (obj->m_detailColor) {
				foundEtalon = true;
				refObject = obj;
				break;
			}
			if (obj->m_customColorType == 2) {
				foundEtalon = true;
				refObject = obj;
			}
		}
		if (!foundEtalon)
			refObject = static_cast<GameObject*>(objArr->objectAtIndex(0));
		objArrOldCount = objArr->count();
	}

	auto gjBaseColor = refObject->m_baseColor;
	auto gjDetailColor = refObject->m_detailColor;


	colorButtonSelect(gjBaseColor, false, objArr);
	colorButtonSelect(gjDetailColor, true, objArr);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Offset", &nextFreeOffset)) {
		if (nextFreeOffset > 1101)	nextFreeOffset = 1101;
		if (nextFreeOffset < 1)		nextFreeOffset = 1;
	}


	if (ImGui::Checkbox("Visible Selection", &static_cast<GameObject*>(objArr->objectAtIndex(0))->m_isSelected)) {
		if (static_cast<GameObject*>(objArr->objectAtIndex(0))->m_isSelected) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isSelected = 1;
			}
			reinterpret_cast<void(__thiscall*)(EditorUI*)>(geode::base::get() + 0x11dd50)(GameManager::sharedState()->m_levelEditorLayer->m_editorUI); //->resetSelectedObjectsColor();
		}
		else {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isSelected = 0;
			}
		}
	}

	//OKAY THIS SHOULD BE REFINED LATER
	if (gjDetailColor) {
		ImGui::Text("Hue");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		if (ImGui::DragFloat("##Hue1", &gjBaseColor->m_hsv.h, 1.f, -180.f, 180.f)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.h = gjBaseColor->m_hsv.h;
			}
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		if (ImGui::DragFloat("##Hue2", &gjDetailColor->m_hsv.h, 1.f, -180.f, 180.f)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				if (obj->m_detailColor) {
					obj->m_detailColor->m_hsv.h = gjDetailColor->m_hsv.h;
				}
			}
		}

		ImGui::Text("Saturation");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		if (ImGui::DragFloat("##Saturation1", &gjBaseColor->m_hsv.s, 0.05f, -1.f, 2.f)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.s = gjBaseColor->m_hsv.s;
				ErGui::clampHSV(&obj->m_baseColor->m_hsv);
			}
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		if (ImGui::DragFloat("##Saturation2", &gjDetailColor->m_hsv.s, 0.05f, -1.f, 2.f)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				if (obj->m_detailColor) {
					obj->m_detailColor->m_hsv.s = gjDetailColor->m_hsv.s;
					ErGui::clampHSV(&obj->m_detailColor->m_hsv);
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("##AbsoluteSat1", &gjBaseColor->m_hsv.absoluteSaturation)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.absoluteSaturation = gjBaseColor->m_hsv.absoluteSaturation;
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("##AbsoluteSat2", &gjDetailColor->m_hsv.absoluteSaturation)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				if (obj->m_detailColor)
					obj->m_detailColor->m_hsv.absoluteSaturation = gjDetailColor->m_hsv.absoluteSaturation;
			}
		}

		ImGui::Text("Value");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		if (ImGui::DragFloat("##Value1", &gjBaseColor->m_hsv.v, 0.05f, -1.f, 2.f)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.v = gjBaseColor->m_hsv.v;
				ErGui::clampHSV(&obj->m_baseColor->m_hsv);
			}
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		if (ImGui::DragFloat("##Value2", &gjDetailColor->m_hsv.v, 0.05f, -1.f, 2.f)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				if (obj->m_detailColor) {
					obj->m_detailColor->m_hsv.v = gjDetailColor->m_hsv.v;
					ErGui::clampHSV(&obj->m_detailColor->m_hsv);
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("##AbsoluteVal1", &gjBaseColor->m_hsv.absoluteBrightness)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.absoluteBrightness = gjBaseColor->m_hsv.absoluteBrightness;
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("##AbsoluteVal2", &gjDetailColor->m_hsv.absoluteBrightness)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				if (obj->m_detailColor)
					obj->m_detailColor->m_hsv.absoluteBrightness = gjDetailColor->m_hsv.absoluteBrightness;
			}
		}


		ImGui::Text("HSV Reset");
		if (ImGui::Button("Base##HSV-RESET")) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.absoluteBrightness = false;
				obj->m_baseColor->m_hsv.absoluteSaturation = false;
				obj->m_baseColor->m_hsv.h = 0.f;
				obj->m_baseColor->m_hsv.s = 1.f;
				obj->m_baseColor->m_hsv.v = 1.f;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Detail##HSV-RESET")) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				if (obj->m_detailColor) {
					obj->m_detailColor->m_hsv.absoluteBrightness = false;
					obj->m_detailColor->m_hsv.absoluteSaturation = false;
					obj->m_detailColor->m_hsv.h = 0.f;
					obj->m_detailColor->m_hsv.s = 1.f;
					obj->m_detailColor->m_hsv.v = 1.f;
				}
			}
		}

		if (ImGui::Combo("Color Type", reinterpret_cast<int*>(&refObject->m_customColorType), colorTypes, IM_ARRAYSIZE(colorTypes))) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_customColorType = refObject->m_customColorType;
			}
		}
	}
	else {
		ImGui::Text("Hue");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::DragFloat("##Hue", &gjBaseColor->m_hsv.h, 1.f, -180.f, 180.f)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.h = gjBaseColor->m_hsv.h;
			}
		}
		
		ImGui::Text("Saturation");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::DragFloat("##Saturation", &gjBaseColor->m_hsv.s, 0.05f, -1.f, 2.f)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.s = gjBaseColor->m_hsv.s;
				ErGui::clampHSV(&obj->m_baseColor->m_hsv);
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("##AbsoluteSaturation", &gjBaseColor->m_hsv.absoluteSaturation)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.absoluteSaturation = gjBaseColor->m_hsv.absoluteSaturation;
			}
		}

		ImGui::Text("Value");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::DragFloat("##Value", &gjBaseColor->m_hsv.v, 0.05f, -1.f, 2.f)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.v = gjBaseColor->m_hsv.v;
				ErGui::clampHSV(&obj->m_baseColor->m_hsv);
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("##AbsoluteValue", &gjBaseColor->m_hsv.absoluteBrightness)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.absoluteBrightness = gjBaseColor->m_hsv.absoluteBrightness;
			}
		}


		ErGui::clampHSV(&gjBaseColor->m_hsv);

		
		if (ImGui::Button("HSV Reset##HSV-RESET")) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_baseColor->m_hsv.absoluteBrightness = false;
				obj->m_baseColor->m_hsv.absoluteSaturation = false;
				obj->m_baseColor->m_hsv.h = 0.f;
				obj->m_baseColor->m_hsv.s = 1.f;
				obj->m_baseColor->m_hsv.v = 1.f;
			}
		}

		if (ImGui::Combo("Color Type", reinterpret_cast<int*>(&refObject->m_customColorType), colorTypes, IM_ARRAYSIZE(colorTypes))) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_customColorType = refObject->m_customColorType;
			}
		}
	}
}


void ErGui::renderEditColor() {
	if (!ImGui::Begin("Color")) {
		ImGui::End();
		return;
	}
	auto gameManager = GameManager::sharedState();
	auto effectManager = gameManager->m_levelEditorLayer->m_levelSettings->m_effectManager;

	if (auto obj = gameManager->getEditorLayer()->m_editorUI->m_selectedObject) {
		renderForObjectEC(obj);
	}
	else if (gameManager->getEditorLayer()->m_editorUI->m_selectedObjects->count()) {
		auto objArr = gameManager->getEditorLayer()->m_editorUI->m_selectedObjects;
		renderForArrayEC(objArr);
	}
	else {
		ImGui::Text("Object is not selected...");
		refObject = nullptr;
	}
	ImGui::End();
}