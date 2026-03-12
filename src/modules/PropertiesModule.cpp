#include "PropertiesModule.hpp"
#include "CustomImGuiWidgets.hpp"
#include "../features/SelectedObjectInfo.hpp"
#include "DebugModule.hpp"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include "Geode/cocos/support/zip_support/ZipUtils.h"
#include "Geode/loader/SettingV3.hpp"
#include "Geode/utils/Keyboard.hpp"
#include "TransformObjectModule.hpp"
#include "../classes/GearEditorUI.hpp"
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/LocalLevelManager.hpp>
#include <imgui.h>
#include <minwindef.h>


const ccColor4F default_additiveSelectColor = { 0.f, 1.f, 0.f, 1.f };
const ccColor4F default_subtractiveSelectColor = { 1.f, 0.f, 1.f, 1.f };
const ccColor4F default_intersectiveSelectColor = { 1.f, 0.6f, 0.f, 1.f };
const ccColor3B default_selectedObjectColor = { 0, 255, 0 };
const ccColor3B default_hoveredObjectColor = { 255, 255, 0 };

template<>
struct matjson::Serialize<ccColor4F> {
	static Result<ccColor4F> fromJson(matjson::Value const& value) {
		GEODE_UNWRAP_INTO(float r, value["r"].asDouble());
		GEODE_UNWRAP_INTO(float g, value["g"].asDouble());
		GEODE_UNWRAP_INTO(float b, value["b"].asDouble());
		GEODE_UNWRAP_INTO(float a, value["a"].asDouble());
		return Ok(ccColor4F{ r, g, b, a });
	}

	static matjson::Value toJson(ccColor4F const& value) {
		auto obj = matjson::Value();
		obj["r"] = value.r;
		obj["g"] = value.g;
		obj["b"] = value.b;
		obj["a"] = value.a;
		return obj;
	}
};

void renderConfirmPopup(std::function<void()> const& callback) {
	if (ImGui::BeginPopupModal("Confirm Reset")) {
		ImGui::Text("Are you sure, you want to reset settings in this category to default?");
		ImGui::Text("This will only affect settings added by GEAR.");
		if (ImGui::Button("Reset")) {
			callback();
			LevelEditorLayer::get()->updateOptions();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}


void renderGeneralSettings() {
	auto gm = GameManager::sharedState();
	auto mod = geode::Mod::get();
	auto lel = LevelEditorLayer::get();

	bool isAnyItemClicked = false;

	// General Settings
	bool startOptimisation = gm->getGameVariable("0151");
	bool increaseUndoRedo = gm->getGameVariable("0013");
	bool smallWarpBtns = gm->getGameVariable("0169");
	bool autoBuildhelper = mod->getSavedValue<bool>("auto-buildhelper", false);					// Automatically applies Build helper on duplicated objects
	bool deselectControls = mod->getSavedValue<bool>("deselect-controls", false);				// Deselects GJRotation/Scale/TransformControl, when clicking on empty space in editor
	bool gamewindowStaticRatio = mod->getSavedValue<bool>("gamewindow-static-ratio", false);		// Game window keeps executable window ratio
	
	// Playtest bools
	bool ptMusic = gm->getGameVariable("0002");			// Plays music during playtesting
	bool ptNoGrid = gm->getGameVariable("0079");			// Hides grid during playtesting
	bool ptSmoothFix = gm->getGameVariable("0102");		// Enables smooth fix during playtesting
	bool autoPause = gm->getGameVariable("0150");			// Pauses gameplay when starting playtest from StartPos
	bool ignoreDamage = gm->getGameVariable("0009");    	// 

	// Zoom Settings
	bool zoomEasing = mod->getSavedValue<bool>("zoom-easing", true);
	float zoomDuration = mod->getSavedValue<float>("zoom-duration", 0.15f);
	float easingRate = mod->getSavedValue<float>("zoom-easingRate", 2.5f);
	int easingType = mod->getSavedValue<int>("zoom-easingType", tweenfunc::TweenType::EaseOut);
	float zoomStep = mod->getSavedValue<float>("zoom-step", 0.1f);

	// Steps
	float moveStep = 		mod->getSavedValue<float>("move-step", 30.f);
	float rotStep = 		mod->getSavedValue<float>("rot-step", 90.f);
	float scaleStep = 		mod->getSavedValue<float>("scale-step", 0.5f);
	float skewStep = 		mod->getSavedValue<float>("skew-step", 15.f);
	bool moveStepToBind = 	mod->getSavedValue<bool>("move-step-to-bind", false);
	bool rotStepToBind = 	mod->getSavedValue<bool>("rot-step-to-bind", false);


	ImGui::SeparatorText("General Editor Settings");
	if (ImGui::Checkbox("Start Optimisation", &startOptimisation)) 
		isAnyItemClicked = true;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Tries to speed up StartPos loading. RobTop's experimental option.");

	if (ImGui::Checkbox("Increase Undo/Redo", &increaseUndoRedo)) 
		isAnyItemClicked = true;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Increases maximum Undo/Redo limit from 200 to 1000");

	if (ImGui::Checkbox("Small Warp Buttons", &smallWarpBtns)) 
		isAnyItemClicked = true;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::BeginTooltip();
		ImGui::TextUnformatted("Warp control buttons are 50% smaller (Restart editor to work).");
		ImGui::EndTooltip();
	}

	if (ImGui::Checkbox("Auto Build Helper", &autoBuildhelper)) 
		isAnyItemClicked = true;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Automatically applies build helper on pasted objects.");

	if (ImGui::Checkbox("Deselect Controls", &deselectControls)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Scale/Rotation/Warp controls are deselected, when touched an empty space in editor.");
	}

	if (ImGui::Checkbox("Game Window Static Ratio", &gamewindowStaticRatio)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Enables static ratio for game window.");
	}


	ErGui::SeparatorPlus("Playtest Settings");
	if (ImGui::Checkbox("Playtest Music", &ptMusic)) 
		isAnyItemClicked = true;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Plays music during playtesting.");

	if (ImGui::Checkbox("Playtest No Grid", &ptNoGrid)) 
		isAnyItemClicked = true;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Hides grid during playtesting.");
	
	if (ImGui::Checkbox("Playtest Smooth Fix", &ptSmoothFix)) 
		isAnyItemClicked = true;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Enables smooth fix during playtesting");
	
	if (ImGui::Checkbox("Auto-Pause", &autoPause)) 
		isAnyItemClicked = true;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Pauses gameplay when starting from StartPos.");
	
	if (ImGui::Checkbox("Ignore Damage", &ignoreDamage)) 
		isAnyItemClicked = true;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Disables damage during playtesting.");


	ErGui::SeparatorPlus("Transform Steps");
	auto style = ImGui::GetStyle();
	const float INPUT_FLOAT_WIDTH = 30.f * 7.f + style.ItemSpacing.x * 6.f;
	// position
	ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
	ImGui::InputFloat("MoveStep", &moveStep, 1.f);
	if (ImGui::Button("0.5##MoveStepPreset", ImVec2({ 30.f, 0.f }))) 	{ moveStep = 0.5f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("2##MoveStepPreset", ImVec2({ 30.f, 0.f }))) 		{ moveStep = 2.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("7.5##MoveStepPreset", ImVec2({ 30.f, 0.f }))) 	{ moveStep = 7.5f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("10##MoveStepPreset", ImVec2({ 30.f, 0.f }))) 		{ moveStep = 10.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("15##MoveStepPreset", ImVec2({ 30.f, 0.f }))) 		{ moveStep = 15.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("30##MoveStepPreset", ImVec2({ 30.f, 0.f }))) 		{ moveStep = 30.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("150##MoveStepPreset", ImVec2({ 30.f, 0.f }))) 	{ moveStep = 150.f;	isAnyItemClicked = true; }	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	ImGui::Text("Presets");
	if (ImGui::Checkbox("Apply steps to Keybinds##MOVE", &moveStepToBind)) isAnyItemClicked = true;
	
	ImGui::Dummy({0.f, 5.f});

	// rotation
	ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
	ImGui::InputFloat("RotStep", &rotStep, 1.f);
	if (ImGui::Button("1##RotateStepPreset", ImVec2({ 30.f, 0.f }))) 	{ rotStep = 1.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("10##RotateStepPreset", ImVec2({ 30.f, 0.f }))) 	{ rotStep = 10.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("15##RotateStepPreset", ImVec2({ 30.f, 0.f }))) 	{ rotStep = 15.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("30##RotateStepPreset", ImVec2({ 30.f, 0.f }))) 	{ rotStep = 30.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("45##RotateStepPreset", ImVec2({ 30.f, 0.f }))) 	{ rotStep = 45.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("90##RotateStepPreset", ImVec2({ 30.f, 0.f }))) 	{ rotStep = 90.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("180##RotateStepPreset", ImVec2({ 30.f, 0.f }))) 	{ rotStep = 180.f;	isAnyItemClicked = true; }	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	ImGui::Text("Presets");
	if (ImGui::Checkbox("Apply steps to Keybinds##ROTATE", &rotStepToBind)) isAnyItemClicked = true;

	ImGui::Dummy({ 0.f, 5.f });

	// scale
	ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
	ImGui::InputFloat("ScaleStep", &scaleStep, 0.25f);
	if (ImGui::Button("0.01##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) 	{ scaleStep = 0.01f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("0.05##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) 	{ scaleStep = 0.05f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("0.1##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) 	{ scaleStep = 0.1f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("0.25##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) 	{ scaleStep = 0.25f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("0.5##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) 	{ scaleStep = 0.5f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("1##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) 		{ scaleStep = 1.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("2##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) 		{ scaleStep = 2.f;	isAnyItemClicked = true; }	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	ImGui::Text("Presets");
	
	ImGui::Dummy({ 0.f, 5.f });

	// skew
	ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
	ImGui::InputFloat("SkewStep", &skewStep, 1.f);
	if (ImGui::Button("1##SkewStepPreset", ImVec2({ 30.f, 0.f })))  { skewStep = 1.f;		isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("5##SkewStepPreset", ImVec2({ 30.f, 0.f })))  { skewStep = 5.f;		isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("10##SkewStepPreset", ImVec2({ 30.f, 0.f }))) { skewStep = 10.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("15##SkewStepPreset", ImVec2({ 30.f, 0.f }))) { skewStep = 15.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("30##SkewStepPreset", ImVec2({ 30.f, 0.f }))) { skewStep = 30.f;	isAnyItemClicked = true; }	ImGui::SameLine();
	if (ImGui::Button("45##SkewStepPreset", ImVec2({ 30.f, 0.f }))) { skewStep = 45.f;	isAnyItemClicked = true; }	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	ImGui::Text("Presets");

	ErGui::SeparatorPlus("Zoom Settings");
	if (ImGui::Checkbox("Zoom Easing", &zoomEasing)) 
		isAnyItemClicked = true;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Enables zoom easing.");
	
	ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
	if (ImGui::InputFloat("Duration", &zoomDuration, 0.05f, 0.2f)) isAnyItemClicked = true;
	ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
	if (ImGui::InputFloat("Easing Rate", &easingRate, 0.1f, 1.f)) isAnyItemClicked = true;

	const char* easingTypes[] = {
		"Linear",
		"Sine In",
        "Sine Out",
        "Sine In Out",
        "Quad In",
        "Quad Out",
        "Quad In Out",
        "Cubic In",
        "Cubic Out",
        "Cubic In Out",
        "Quart In",
        "Quart Out",
        "Quart In Out",
        "Quint In",
        "Quint Out",
        "Quint In Out",
        "Expo In",
        "Expo Out",
        "Expo In Out",
        "Circ In",
        "Circ Out",
        "Circ In Out",
        "Elastic In",
        "Elastic Out",
        "Elastic In Out",
        "Back In",
        "Back Out",
        "Back In Out",
        "Bounce In",
        "Bounce Out",
        "Bounce In Out",
		"Ease In",
		"Ease Out",
		"Ease In Out"
	};

	ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
	if (ImGui::Combo("Easing Type", &easingType, easingTypes, IM_ARRAYSIZE(easingTypes))) {
		isAnyItemClicked = true;
	}

	ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
	if (ImGui::InputFloat("Zoom Step", &zoomStep, 0.01f, 0.1f)) {
		isAnyItemClicked = true;
		if (zoomStep > 1.f) zoomStep = 1.f;
		else if (zoomStep < 0.01f) zoomStep = 0.01f;
	}	

	ImGui::Dummy(ImVec2(10.f, 10.f));

	renderConfirmPopup([mod]() {
		mod->setSavedValue<bool>("auto-buildhelper", false);
		mod->setSavedValue<bool>("deselect-controls", false);
		mod->setSavedValue<bool>("gamewindow-static-ratio", false);

		mod->setSavedValue<bool>("zoom-easing", true);
		mod->setSavedValue<float>("zoom-duration", 0.15f);
		mod->setSavedValue<float>("zoom-easingRate", 2.5f);
		mod->setSavedValue<int>("zoom-easingType", static_cast<int>(tweenfunc::TweenType::EaseOut));
		mod->setSavedValue<float>("zoom-step", 0.1f);

		mod->setSavedValue<float>("move-step", 30.f);
		mod->setSavedValue<float>("rot-step", 90.f);
		mod->setSavedValue<float>("scale-step", 0.5f);
		mod->setSavedValue<float>("skew-step", 15.f);
		mod->setSavedValue<bool>("move-step-to-bind", false);
		mod->setSavedValue<bool>("rot-step-to-bind", false);
		});
	if (ImGui::Button("Reset to Default")) {
		ImGui::OpenPopup("Confirm Reset");
	}


	// Saving Values
	if (isAnyItemClicked) {
		gm->setGameVariable("0151", startOptimisation);
		gm->setGameVariable("0013", increaseUndoRedo);
		gm->setGameVariable("0169", smallWarpBtns);
		mod->setSavedValue<bool>("auto-buildhelper", autoBuildhelper);
		mod->setSavedValue<bool>("deselect-controls", deselectControls);
		mod->setSavedValue<bool>("gamewindow-static-ratio", gamewindowStaticRatio);

		gm->setGameVariable("0002", ptMusic);
		gm->setGameVariable("0079", ptNoGrid);
		gm->setGameVariable("0102", ptSmoothFix);
		gm->setGameVariable("0150", autoPause);
		gm->setGameVariable("0009", ignoreDamage);

		mod->setSavedValue<bool>("zoom-easing", zoomEasing);
		mod->setSavedValue<float>("zoom-duration", zoomDuration);
		mod->setSavedValue<float>("zoom-easingRate", easingRate);
		mod->setSavedValue<int>("zoom-easingType", easingType);
		mod->setSavedValue<float>("zoom-step", zoomStep);

		// Steps
		mod->setSavedValue<float>("move-step", moveStep);
		mod->setSavedValue<float>("rot-step", rotStep);
		mod->setSavedValue<float>("scale-step", scaleStep);
		mod->setSavedValue<float>("skew-step", skewStep);
		mod->setSavedValue<bool>("move-step-to-bind", moveStepToBind);
		mod->setSavedValue<bool>("rot-step-to-bind", rotStepToBind);

		lel->updateOptions();
	}
}

void renderInterfaceSettings() {
	auto gm = GameManager::sharedState();
	auto mod = geode::Mod::get();
	auto lel = LevelEditorLayer::get();

	bool isAnyItemClicked = false;

	bool triangleColorWheel = mod->getSavedValue<bool>("triangle-color-wheel", true);			//
	bool rotateColorWheel = mod->getSavedValue<bool>("rotate-color-wheel", false);				//
	bool hideObjectListPopup = mod->getSavedValue<bool>("hide-object-list-popup", true);			//
	bool autoswitchToBuildMode = mod->getSavedValue<bool>("autoswitch-to-build-mode", true);		//
	bool showLinkControls = gm->getGameVariable("0097");														//
	bool showZoomControls = mod->getSavedValue<bool>("show-zoom-controls", true);				// Shows zoom controls buttons on toolbox
	bool showBindsInToolbar = mod->getSavedValue<bool>("show-binds-in-toolbar", true);			// 


	ImGui::SeparatorText("Color Widget Settings");
	if (ImGui::Checkbox("Triangle Color Wheel", &triangleColorWheel)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Sets GEAR color wheel to triangle.");
	}

	ImGui::BeginDisabled(!triangleColorWheel);
	if (ImGui::Checkbox("Rotate Color Wheel", &rotateColorWheel)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_AllowWhenDisabled)) {
		ImGui::SetTooltip("Enables color wheel rotation in GEAR. Only works with triangle color wheel.");
	}
	ImGui::EndDisabled();


	ErGui::SeparatorPlus("Object List Settings");
	if (ImGui::Checkbox("Hide Objects Popup", &hideObjectListPopup)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Hides object folder popup, when selecting an object.");
	}

	if (ImGui::Checkbox("Autoswitch to Build Mode", &autoswitchToBuildMode)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Automatically switches to Build Mode when selecting an object in object list.");
	}


	ErGui::SeparatorPlus("Toolbar Settings");
	if (ImGui::Checkbox("Show Link Controls", &showLinkControls)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Shows and enables link controls in the toolbar.");
	}
	if (ImGui::Checkbox("Show Zoom Controls", &showZoomControls)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Shows zoom controls in the toolbar.");
	}
	if (ImGui::Checkbox("Show Keybinds", &showBindsInToolbar)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Shows keybinds in tooltips in the toolbar.");
	}

	//ImGui::SeparatorText("Styling Settings");

	ImGui::Dummy(ImVec2(10.f, 10.f));

	renderConfirmPopup([mod]() {
		mod->setSavedValue<bool>("triangle-color-wheel", true);
		mod->setSavedValue<bool>("rotate-color-wheel", false);
		mod->setSavedValue<bool>("hide-object-list-popup", true);
		mod->setSavedValue<bool>("autoswitch-to-build-mode", true);
		mod->setSavedValue<bool>("show-zoom-controls", true);
		mod->setSavedValue<bool>("show-binds-in-toolbar", true);
		});
	if (ImGui::Button("Reset to Default")) {
		ImGui::OpenPopup("Confirm Reset");
	}
	

	// Saving Values
	if (isAnyItemClicked) {
		mod->setSavedValue<bool>("triangle-color-wheel",		triangleColorWheel);
		mod->setSavedValue<bool>("rotate-color-wheel",			rotateColorWheel);
		mod->setSavedValue<bool>("hide-object-list-popup",		hideObjectListPopup);
		mod->setSavedValue<bool>("autoswitch-to-build-mode",	autoswitchToBuildMode);
		mod->setSavedValue<bool>("show-zoom-controls",			showZoomControls);
		mod->setSavedValue<bool>("show-binds-in-toolbar",		showBindsInToolbar);
		gm->setGameVariable("0097", showLinkControls);
		

		lel->updateOptions();
	}
}

void renderObjectInfoSettings() {
	auto gm = GameManager::sharedState();
	auto mod = geode::Mod::get();
	auto lel = LevelEditorLayer::get();

	bool isAnyItemClicked = false;

	bool showObjectInfo =	gm->getGameVariable("0041");
	bool soiPosition =		mod->getSavedValue<bool>("soi-position", true);
	bool soiRotation =		mod->getSavedValue<bool>("soi-rotation", true);
	bool soiScale =			mod->getSavedValue<bool>("soi-scale", true);
	bool soiColor =			mod->getSavedValue<bool>("soi-color", true);
	bool soiHSV =			mod->getSavedValue<bool>("soi-hsv", false);
	bool soiGroups =		mod->getSavedValue<bool>("soi-groups", true);
	bool soiZLayer =		mod->getSavedValue<bool>("soi-zlayer", true);
	bool soiZOrder =		mod->getSavedValue<bool>("soi-zorder", true);
	bool soiObjectID =		mod->getSavedValue<bool>("soi-objectid", false);
	bool soiTargetGroup =	mod->getSavedValue<bool>("soi-targetgroup", true);
	bool soiItemID =		mod->getSavedValue<bool>("soi-itemid", false);
	bool soiBlockID =		mod->getSavedValue<bool>("soi-blockid", false);
	bool soiParticles =		mod->getSavedValue<bool>("soi-particles", true);
	bool soiHidden =		mod->getSavedValue<bool>("soi-hidden", true);
	bool soiNoTouch =		mod->getSavedValue<bool>("soi-no-touch", true);
	bool soiHighDetail =	mod->getSavedValue<bool>("soi-high-detail", false);
	bool soiObjectCount =	mod->getSavedValue<bool>("soi-object-count", true);
	bool soiObjectType =	mod->getSavedValue<bool>("soi-object-type", false);

	ImGui::SeparatorText("Global Settings");
	if (ImGui::Checkbox("Show Object Info", &showObjectInfo)) {
		isAnyItemClicked = true;
		
		if (lel) {
			if (auto label = typeinfo_cast<CCLabelBMFont*>(lel->getChildByID("object-info-label"_spr)))
				if (showObjectInfo)
					label->setVisible(true);
				else
					label->setVisible(false);
		}
	}


	ImGui::BeginDisabled();
	float testFloat = 0.f;
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ErGui::DragFloat("Position##XPos", &testFloat, 1, 5, "%.3f", 1.f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255));
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ErGui::DragFloat("##YPos", &testFloat, 1, 5, "%.3f", 1.f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255));

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
	ErGui::DragFloat("Scale##Scale", &testFloat);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
	ErGui::DragFloat("Opacity##Opacity", &testFloat);
	
	ImGui::Text("Color");
	ImGui::EndDisabled();

	ErGui::SeparatorPlus("Show Elements");

	ImGui::BeginGroup();
	if (ImGui::Checkbox("Position",	&soiPosition))	isAnyItemClicked = true;
	if (ImGui::Checkbox("Rotation",	&soiRotation))	isAnyItemClicked = true;
	if (ImGui::Checkbox("Scale",	&soiScale))		isAnyItemClicked = true;
	if (ImGui::Checkbox("Color",	&soiColor))		isAnyItemClicked = true;
	if (ImGui::Checkbox("HSV",		&soiHSV))		isAnyItemClicked = true;
	if (ImGui::Checkbox("Groups",	&soiGroups))	isAnyItemClicked = true;
	ImGui::EndGroup();

	ImGui::SameLine();
	
	ImGui::BeginGroup();
	if (ImGui::Checkbox("ZLayer",		&soiZLayer))		isAnyItemClicked = true;
	if (ImGui::Checkbox("ZOrder",		&soiZOrder))		isAnyItemClicked = true;
	if (ImGui::Checkbox("ObjectID",		&soiObjectID))		isAnyItemClicked = true;
	if (ImGui::Checkbox("TargetGroup",	&soiTargetGroup))	isAnyItemClicked = true;
	if (ImGui::Checkbox("ItemID",		&soiItemID))		isAnyItemClicked = true;
	if (ImGui::Checkbox("BlockID",		&soiBlockID))		isAnyItemClicked = true;
	ImGui::EndGroup();

	ImGui::SameLine();

	ImGui::BeginGroup();
	if (ImGui::Checkbox("Particles",	&soiParticles))		isAnyItemClicked = true;
	if (ImGui::Checkbox("Hidden",		&soiHidden))		isAnyItemClicked = true;
	if (ImGui::Checkbox("NoTouch",		&soiNoTouch))		isAnyItemClicked = true;
	if (ImGui::Checkbox("HighDetail",	&soiHighDetail))	isAnyItemClicked = true;
	if (ImGui::Checkbox("ObjectCount",	&soiObjectCount))	isAnyItemClicked = true;
	if (ImGui::Checkbox("ObjectType",	&soiObjectType))	isAnyItemClicked = true;
	ImGui::EndGroup();

	// ToDo: be able to change elements order


	ImGui::Dummy(ImVec2(10.f, 10.f));

	renderConfirmPopup([mod]() {
		mod->setSavedValue<bool>("soi-position", true);
		mod->setSavedValue<bool>("soi-rotation", true);
		mod->setSavedValue<bool>("soi-scale", true);
		mod->setSavedValue<bool>("soi-color", true);
		mod->setSavedValue<bool>("soi-hsv", false);
		mod->setSavedValue<bool>("soi-groups", true);
		mod->setSavedValue<bool>("soi-zlayer", true);
		mod->setSavedValue<bool>("soi-zorder", true);
		mod->setSavedValue<bool>("soi-objectid", false);
		mod->setSavedValue<bool>("soi-targetgroup", true);
		mod->setSavedValue<bool>("soi-itemid", false);
		mod->setSavedValue<bool>("soi-blockid", false);
		mod->setSavedValue<bool>("soi-particles", true);
		mod->setSavedValue<bool>("soi-hidden", true);
		mod->setSavedValue<bool>("soi-no-touch", true);
		mod->setSavedValue<bool>("soi-high-detail", false);
		mod->setSavedValue<bool>("soi-object-count", true);
		mod->setSavedValue<bool>("soi-object-type", false);
		});
	if (ImGui::Button("Reset to Default")) {
		ImGui::OpenPopup("Confirm Reset");
	}

	// Saving Values
	if (isAnyItemClicked) {
		gm->setGameVariable("0041", showObjectInfo);
		mod->setSavedValue<bool>("soi-position",		soiPosition);
		mod->setSavedValue<bool>("soi-rotation",		soiRotation);
		mod->setSavedValue<bool>("soi-scale",			soiScale);
		mod->setSavedValue<bool>("soi-color",			soiColor);
		mod->setSavedValue<bool>("soi-hsv",				soiHSV);
		mod->setSavedValue<bool>("soi-groups",			soiGroups);
		mod->setSavedValue<bool>("soi-zlayer",			soiZLayer);
		mod->setSavedValue<bool>("soi-zorder",			soiZOrder);
		mod->setSavedValue<bool>("soi-objectid",		soiObjectID);
		mod->setSavedValue<bool>("soi-targetgroup",		soiTargetGroup);
		mod->setSavedValue<bool>("soi-itemid",			soiItemID);
		mod->setSavedValue<bool>("soi-blockid",			soiBlockID);
		mod->setSavedValue<bool>("soi-particles",		soiParticles);
		mod->setSavedValue<bool>("soi-hidden",			soiHidden);
		mod->setSavedValue<bool>("soi-no-touch",		soiNoTouch);
		mod->setSavedValue<bool>("soi-high-detail",		soiHighDetail);
		mod->setSavedValue<bool>("soi-object-count",	soiObjectCount);
		mod->setSavedValue<bool>("soi-object-type",		soiObjectType);
		SelectedObjectInfo::g_soiNeedsUpdate = true;
	}
}

void renderDrawsSettings() {
	auto mod = geode::Mod::get();

	bool isAnyItemClicked = false;

	bool fillSelectionZone =			mod->getSavedValue<bool>("fill-selection-zone", false);    // Fills selection zone with solid color
	bool hoveringSelects =				mod->getSavedValue<bool>("hovering-selects", true);          // Hovers objects in the selection zone
	ErGui::g_additiveSelectColor =		mod->getSavedValue<ccColor4F>("additive-select-color", ErGui::g_additiveSelectColor);
	ErGui::g_subtractiveSelectColor =	mod->getSavedValue<ccColor4F>("subtractive-select-color", ErGui::g_subtractiveSelectColor);
	ErGui::g_intersectiveSelectColor =	mod->getSavedValue<ccColor4F>("intersective-select-color", ErGui::g_intersectiveSelectColor);
	ErGui::g_selectedObjectColor =		mod->getSavedValue<ccColor3B>("selected-object-color", ErGui::g_selectedObjectColor);
	ErGui::g_hoveredObjectColor =		mod->getSavedValue<ccColor3B>("hovered-object-color", ErGui::g_hoveredObjectColor);

	if (ImGui::Checkbox("Fill Selection Zone", &fillSelectionZone)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Fills selection zone with slightly opaque color of selection type.");
	}
	if (ImGui::Checkbox("Hovering Selects", &hoveringSelects)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Objects that are in the selection zone, change their color to yellow.");
	}

	if (ImGui::ColorEdit4("Additive Select", reinterpret_cast<float*>(&ErGui::g_additiveSelectColor))) {
		isAnyItemClicked = true;
	}
	if (ImGui::ColorEdit4("Subtractive Select", reinterpret_cast<float*>(&ErGui::g_subtractiveSelectColor))) {
		isAnyItemClicked = true;
	}
	if (ImGui::ColorEdit4("Intersective Select", reinterpret_cast<float*>(&ErGui::g_intersectiveSelectColor))) {
		isAnyItemClicked = true;
	}

	ImGui::Dummy(ImVec2(0.f, 5.f));

	float selectedColor[3] = {
		ErGui::g_selectedObjectColor.r / 255.f,
		ErGui::g_selectedObjectColor.g / 255.f,
		ErGui::g_selectedObjectColor.b / 255.f
	};
	if (ImGui::ColorEdit3("Selected Object", selectedColor)) {
		isAnyItemClicked = true;
		EditorUI::get()->resetSelectedObjectsColor();
	}
	ErGui::g_selectedObjectColor = {
		static_cast<unsigned char>(selectedColor[0] * 255),
		static_cast<unsigned char>(selectedColor[1] * 255),
		static_cast<unsigned char>(selectedColor[2] * 255)
	};

	float hoveredColor[3] = {
		ErGui::g_hoveredObjectColor.r / 255.f,
		ErGui::g_hoveredObjectColor.g / 255.f,
		ErGui::g_hoveredObjectColor.b / 255.f
	};
	if (ImGui::ColorEdit3("Hovered Object", hoveredColor)) {
		isAnyItemClicked = true;
	}
	ErGui::g_hoveredObjectColor = {
		static_cast<unsigned char>(hoveredColor[0] * 255),
		static_cast<unsigned char>(hoveredColor[1] * 255),
		static_cast<unsigned char>(hoveredColor[2] * 255)
	};
	

	ImGui::Dummy(ImVec2(10.f, 10.f));

	renderConfirmPopup([mod]() {
		mod->setSavedValue<bool>("fill-selection-zone", false);
		mod->setSavedValue<bool>("hovering-selects", true);
		mod->setSavedValue<ccColor4F>("additive-select-color", default_additiveSelectColor);
		mod->setSavedValue<ccColor4F>("subtractive-select-color", default_subtractiveSelectColor);
		mod->setSavedValue<ccColor4F>("intersective-select-color", default_intersectiveSelectColor);
		mod->setSavedValue<ccColor3B>("selected-object-color", default_selectedObjectColor);
		mod->setSavedValue<ccColor3B>("hovered-object-color", default_hoveredObjectColor);
		});
	if (ImGui::Button("Reset to Default")) {
		ImGui::OpenPopup("Confirm Reset");
	}

	// Saving Values
	if (isAnyItemClicked) {
		mod->setSavedValue<bool>("fill-selection-zone", fillSelectionZone);
		mod->setSavedValue<bool>("hovering-selects", hoveringSelects);

		mod->setSavedValue<ccColor4F>("additive-select-color", ErGui::g_additiveSelectColor);
		mod->setSavedValue<ccColor4F>("subtractive-select-color", ErGui::g_subtractiveSelectColor);
		mod->setSavedValue<ccColor4F>("intersective-select-color", ErGui::g_intersectiveSelectColor);

		mod->setSavedValue<ccColor3B>("selected-object-color", ErGui::g_selectedObjectColor);
		mod->setSavedValue<ccColor3B>("hovered-object-color", ErGui::g_hoveredObjectColor);
	}
}

void renderDebugSettings() {
	auto gm = GameManager::sharedState();
	auto mod = geode::Mod::get();
	auto lel = LevelEditorLayer::get();

	bool isAnyItemClicked = false;

	bool debugShowObjectsBoxes = mod->getSavedValue<bool>("debug-show-objects-boxes", false);  // Shows hitboxes of all objects


	ImGui::Checkbox("Show Debug Module", &ErGui::g_showDebugModule); // ToDo saving
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Shows Debug module with some useful info.");
	}

	if (ImGui::Checkbox("Show Custom Object Boxes", &debugShowObjectsBoxes)) {
		isAnyItemClicked = true;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Shows custom objects hitboxes.");
	}

	ImGui::Checkbox("Show Touches", &ErGui::dbgTDN); // ToDo saving
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("Shows positions of begin and end of touch.");
	}

	renderConfirmPopup([mod]() {
		ErGui::g_showDebugModule = false;
		ErGui::dbgTDN = false;
		mod->setSavedValue<bool>("debug-show-objects-boxes", false);
		});
	if (ImGui::Button("Reset to Default")) {
		ImGui::OpenPopup("Confirm Reset");
	}

	// Saving Values
	if (isAnyItemClicked) {
		mod->setSavedValue<bool>("debug-show-objects-boxes", debugShowObjectsBoxes);
	}
}

void renderLevelBrowserSettings() {
	ImGui::SeparatorText("Local Levels");
	auto lel = LevelEditorLayer::get();
	int i = 0;
	for (auto level : CCArrayExt<GJGameLevel*>(LocalLevelManager::get()->m_localLevels)) {
		ImGui::Text("%s", level->m_levelName.c_str());
		ImGui::SameLine(300.f);
		if (ImGui::Button(fmt::format("Load Level##{}", i).c_str())) {
			ErGui::getFakePauseLayer()->saveLevel();
			lel->removeAllObjects();
			
			std::string levelString = ZipUtils::decompressString(level->m_levelString, false, 0);
			// log::info("Loading level: {}", levelString.c_str());
			lel->createObjectsFromSetup(levelString);
			lel->m_level = level;
		}
		i++;
	}
	
	// Buggy af
	ImGui::SeparatorText("Downloaded Levels");
	auto levelsDict = GameLevelManager::get()->m_onlineLevels;

	for (auto key : CCArrayExt<CCString*>(levelsDict->allKeys())) {
		GJGameLevel* level = static_cast<GJGameLevel*>(levelsDict->objectForKey(key->getCString()));
		if (!level->m_levelString.size()) continue;
		ImGui::Text("%s", level->m_levelName.c_str());
		ImGui::SameLine(300.f);
		if (ImGui::Button(fmt::format("Load Level##{}", i).c_str())) {
			ErGui::getFakePauseLayer()->saveLevel();
			lel->removeAllObjects();
			ErGui::resetLastObjects(); // This is needed for fixing getLastObjectX/YFast

			std::string levelString = ZipUtils::decompressString(level->m_levelString, false, 0);
			// log::info("Loading level: {}", levelString.c_str());
			lel->createObjectsFromSetup(levelString);
			lel->m_level = level;
		}
		i++;
		// log::info("Key: {}", key);
	}
	// log::info("{}", levelsDict->objectForKey("93450782"));
	// for (int i = 0; i < levelsDict->count(); i++) {
		
	// }
}

std::unordered_map<ImGuiKey, enumKeyCodes> imguiKeyToCocos = {
    { ImGuiKey::ImGuiKey_Backspace, KEY_Backspace },
    { ImGuiKey::ImGuiKey_Tab, KEY_Tab },
    // { "Clear", KEY_Clear },
    { ImGuiKey::ImGuiKey_Enter, KEY_Enter },
    { ImGuiKey::ImGuiMod_Shift, KEY_Shift },
    { ImGuiKey::ImGuiMod_Ctrl, KEY_Control },
    { ImGuiKey::ImGuiMod_Alt, KEY_Alt },
    { ImGuiKey::ImGuiKey_Pause, KEY_Pause },
    { ImGuiKey::ImGuiKey_CapsLock, KEY_CapsLock },
    { ImGuiKey::ImGuiKey_Escape, KEY_Escape },
    { ImGuiKey::ImGuiKey_Space, KEY_Space },
    { ImGuiKey::ImGuiKey_PageUp, KEY_PageUp },
    { ImGuiKey::ImGuiKey_PageDown, KEY_PageDown },
    { ImGuiKey::ImGuiKey_End, KEY_End },
    { ImGuiKey::ImGuiKey_Home, KEY_Home },
    { ImGuiKey::ImGuiKey_LeftArrow, KEY_Left },
    { ImGuiKey::ImGuiKey_UpArrow, KEY_Up },
    { ImGuiKey::ImGuiKey_RightArrow, KEY_Right },
    { ImGuiKey::ImGuiKey_DownArrow, KEY_Down },
    // { ImGuiKey::ImGuiKey_Select, KEY_Select },
    // { ImGuiKey::ImGuiKey_Print, KEY_Print },
    // { ImGuiKey::ImGuiKey_Execute, KEY_Execute },
    { ImGuiKey::ImGuiKey_PrintScreen, KEY_PrintScreen },
    { ImGuiKey::ImGuiKey_Insert, KEY_Insert },
    { ImGuiKey::ImGuiKey_Delete, KEY_Delete },
    // { ImGuiKey::ImGuiKey_Help, KEY_Help },
    { ImGuiKey::ImGuiKey_0, KEY_Zero },
    { ImGuiKey::ImGuiKey_1, KEY_One },
    { ImGuiKey::ImGuiKey_2, KEY_Two },
    { ImGuiKey::ImGuiKey_3, KEY_Three },
    { ImGuiKey::ImGuiKey_4, KEY_Four },
    { ImGuiKey::ImGuiKey_5, KEY_Five },
    { ImGuiKey::ImGuiKey_6, KEY_Six },
    { ImGuiKey::ImGuiKey_7, KEY_Seven },
    { ImGuiKey::ImGuiKey_8, KEY_Eight },
    { ImGuiKey::ImGuiKey_9, KEY_Nine },
    { ImGuiKey::ImGuiKey_A, KEY_A },
    { ImGuiKey::ImGuiKey_B, KEY_B },
    { ImGuiKey::ImGuiKey_C, KEY_C },
    { ImGuiKey::ImGuiKey_D, KEY_D },
    { ImGuiKey::ImGuiKey_E, KEY_E },
    { ImGuiKey::ImGuiKey_F, KEY_F },
    { ImGuiKey::ImGuiKey_G, KEY_G },
    { ImGuiKey::ImGuiKey_H, KEY_H },
    { ImGuiKey::ImGuiKey_I, KEY_I },
    { ImGuiKey::ImGuiKey_J, KEY_J },
    { ImGuiKey::ImGuiKey_K, KEY_K },
    { ImGuiKey::ImGuiKey_L, KEY_L },
    { ImGuiKey::ImGuiKey_M, KEY_M },
    { ImGuiKey::ImGuiKey_N, KEY_N },
    { ImGuiKey::ImGuiKey_O, KEY_O },
    { ImGuiKey::ImGuiKey_P, KEY_P },
    { ImGuiKey::ImGuiKey_Q, KEY_Q },
    { ImGuiKey::ImGuiKey_R, KEY_R },
    { ImGuiKey::ImGuiKey_S, KEY_S },
    { ImGuiKey::ImGuiKey_T, KEY_T },
    { ImGuiKey::ImGuiKey_U, KEY_U },
    { ImGuiKey::ImGuiKey_V, KEY_V },
    { ImGuiKey::ImGuiKey_W, KEY_W },
    { ImGuiKey::ImGuiKey_X, KEY_X },
    { ImGuiKey::ImGuiKey_Y, KEY_Y },
    { ImGuiKey::ImGuiKey_Z, KEY_Z },
    { ImGuiKey::ImGuiKey_LeftSuper, KEY_LeftWindowsKey },
    { ImGuiKey::ImGuiKey_RightSuper, KEY_RightWindowsKey },
    // { ImGuiKey::ImGuiKey_ApplicationsKey, KEY_ApplicationsKey },
    // { ImGuiKey::ImGuiKey_Sleep, KEY_Sleep },
    { ImGuiKey::ImGuiKey_Keypad0, KEY_NumPad0 },
    { ImGuiKey::ImGuiKey_Keypad1, KEY_NumPad1 },
    { ImGuiKey::ImGuiKey_Keypad2, KEY_NumPad2 },
    { ImGuiKey::ImGuiKey_Keypad3, KEY_NumPad3 },
    { ImGuiKey::ImGuiKey_Keypad4, KEY_NumPad4 },
    { ImGuiKey::ImGuiKey_Keypad5, KEY_NumPad5 },
    { ImGuiKey::ImGuiKey_Keypad6, KEY_NumPad6 },
    { ImGuiKey::ImGuiKey_Keypad7, KEY_NumPad7 },
    { ImGuiKey::ImGuiKey_Keypad8, KEY_NumPad8 },
    { ImGuiKey::ImGuiKey_Keypad9, KEY_NumPad9 },
    { ImGuiKey::ImGuiKey_KeypadMultiply, KEY_Multiply },
    { ImGuiKey::ImGuiKey_KeypadAdd, KEY_Add },
    // { ImGuiKey::ImGuiKey_KeypadEqual, KEY_Seperator }, // what
    { ImGuiKey::ImGuiKey_KeypadSubtract, KEY_Subtract },
    { ImGuiKey::ImGuiKey_KeypadDecimal, KEY_Decimal },
    { ImGuiKey::ImGuiKey_KeypadDivide, KEY_Divide },
    { ImGuiKey::ImGuiKey_F1, KEY_F1 },
    { ImGuiKey::ImGuiKey_F2, KEY_F2 },
    { ImGuiKey::ImGuiKey_F3, KEY_F3 },
    { ImGuiKey::ImGuiKey_F4, KEY_F4 },
    { ImGuiKey::ImGuiKey_F5, KEY_F5 },
    { ImGuiKey::ImGuiKey_F6, KEY_F6 },
    { ImGuiKey::ImGuiKey_F7, KEY_F7 },
    { ImGuiKey::ImGuiKey_F8, KEY_F8 },
    { ImGuiKey::ImGuiKey_F9, KEY_F9 },
    { ImGuiKey::ImGuiKey_F10, KEY_F10 },
    { ImGuiKey::ImGuiKey_F11, KEY_F11 },
    { ImGuiKey::ImGuiKey_F12, KEY_F12 },
    { ImGuiKey::ImGuiKey_F13, KEY_F13 },
    { ImGuiKey::ImGuiKey_F14, KEY_F14 },
    { ImGuiKey::ImGuiKey_F15, KEY_F15 },
    { ImGuiKey::ImGuiKey_F16, KEY_F16 },
    { ImGuiKey::ImGuiKey_F17, KEY_F17 },
    { ImGuiKey::ImGuiKey_F18, KEY_F18 },
    { ImGuiKey::ImGuiKey_F19, KEY_F19 },
    { ImGuiKey::ImGuiKey_F20, KEY_F20 },
    { ImGuiKey::ImGuiKey_F21, KEY_F21 },
    { ImGuiKey::ImGuiKey_F22, KEY_F22 },
    { ImGuiKey::ImGuiKey_F23, KEY_F23 },
    { ImGuiKey::ImGuiKey_F24, KEY_F24 },
    { ImGuiKey::ImGuiKey_NumLock, KEY_Numlock },
    { ImGuiKey::ImGuiKey_ScrollLock, KEY_ScrollLock },
    { ImGuiKey::ImGuiKey_LeftShift, KEY_LeftShift },
    { ImGuiKey::ImGuiKey_RightShift, KEY_RightShift },
    { ImGuiKey::ImGuiKey_LeftCtrl, KEY_LeftControl },
    { ImGuiKey::ImGuiKey_RightCtrl, KEY_RightControl },
    // { ImGuiKey::ImGuiKey_LeftMenu, KEY_LeftMenu },
    // { ImGuiKey::ImGuiKey_RightMenu, KEY_RightMenu },
    { ImGuiKey::ImGuiKey_AppBack, KEY_BrowserBack },
    { ImGuiKey::ImGuiKey_AppForward, KEY_BrowserForward },
    // { ImGuiKey::ImGuiKey_BrowserRefresh, KEY_BrowserRefresh },
    // { ImGuiKey::ImGuiKey_BrowserStop, KEY_BrowserStop },
    // { ImGuiKey::ImGuiKey_BrowserSearch, KEY_BrowserSearch },
    // { ImGuiKey::ImGuiKey_BrowserFavorites, KEY_BrowserFavorites },
    // { ImGuiKey::ImGuiKey_BrowserHome, KEY_BrowserHome },
    // { ImGuiKey::ImGuiKey_VolumeMute, KEY_VolumeMute },
    // { ImGuiKey::ImGuiKey_VolumeDown, KEY_VolumeDown },
    // { ImGuiKey::ImGuiKey_VolumeUp, KEY_VolumeUp },
    // { ImGuiKey::ImGuiKey_NextTrack, KEY_NextTrack },
    // { ImGuiKey::ImGuiKey_PreviousTrack, KEY_PreviousTrack },
    // { ImGuiKey::ImGuiKey_StopMedia, KEY_StopMedia },
    // { ImGuiKey::ImGuiKey_PlayPause, KEY_PlayPause },
    // { ImGuiKey::ImGuiKey_LaunchMail, KEY_LaunchMail },
    // { ImGuiKey::ImGuiKey_SelectMedia, KEY_SelectMedia },
    // { ImGuiKey::ImGuiKey_LaunchApp1, KEY_LaunchApp1 },
    // { ImGuiKey::ImGuiKey_LaunchApp2, KEY_LaunchApp2 },
    // { ImGuiKey::ImGuiKey_OEM1, KEY_OEM1 },
    // { ImGuiKey::ImGuiKey_OEMPlus, KEY_OEMPlus },
    // { ImGuiKey::ImGuiKey_OEMComma, KEY_OEMComma },
    // { ImGuiKey::ImGuiKey_OEMMinus, KEY_OEMMinus },
    // { ImGuiKey::ImGuiKey_OEMPeriod, KEY_OEMPeriod },
    // { ImGuiKey::ImGuiKey_OEM2, KEY_OEM2 },
    // { ImGuiKey::ImGuiKey_OEM3, KEY_OEM3 },
    // { ImGuiKey::ImGuiKey_OEM4, KEY_OEM4 },
    // { ImGuiKey::ImGuiKey_OEM5, KEY_OEM5 },
    // { ImGuiKey::ImGuiKey_OEM6, KEY_OEM6 },
    // { ImGuiKey::ImGuiKey_OEM7, KEY_OEM7 },
    // { ImGuiKey::ImGuiKey_OEM8, KEY_OEM8 },
    // { ImGuiKey::ImGuiKey_OEM102, KEY_OEM102 },
    // { ImGuiKey::ImGuiKey_Process, KEY_Process },
    // { ImGuiKey::ImGuiKey_Packet, KEY_Packet },
    // { ImGuiKey::ImGuiKey_Attn, KEY_Attn },
    // { ImGuiKey::ImGuiKey_CrSel, KEY_CrSel },
    // { ImGuiKey::ImGuiKey_ExSel, KEY_ExSel },
    // { ImGuiKey::ImGuiKey_EraseEOF, KEY_EraseEOF },
    // { ImGuiKey::ImGuiKey_Play, KEY_Play },
    // { ImGuiKey::ImGuiKey_Zoom, KEY_Zoom },
    // { ImGuiKey::ImGuiKey_PA1, KEY_PA1 },
    // { ImGuiKey::ImGuiKey_OEMClear, KEY_OEMClear },
    // { ImGuiKey::ImGuiKey_GamepadDpadUp, KEY_ArrowUp }, WHAAAAAT
    // { ImGuiKey::ImGuiKey_GamepadDpadDown, KEY_ArrowDown },
    // { ImGuiKey::ImGuiKey_GamepadDpadLeft, KEY_ArrowLeft },
    // { ImGuiKey::ImGuiKey_GamepadDpadRight, KEY_ArrowRight },
    { ImGuiKey::ImGuiKey_GamepadFaceDown, CONTROLLER_A },
    { ImGuiKey::ImGuiKey_GamepadFaceRight, CONTROLLER_B },
    { ImGuiKey::ImGuiKey_GamepadFaceUp, CONTROLLER_Y },
    { ImGuiKey::ImGuiKey_GamepadFaceLeft, CONTROLLER_X },
    { ImGuiKey::ImGuiKey_GamepadStart, CONTROLLER_Start },
    { ImGuiKey::ImGuiKey_GamepadBack, CONTROLLER_Back },
    { ImGuiKey::ImGuiKey_GamepadR1, CONTROLLER_RB },
    { ImGuiKey::ImGuiKey_GamepadL1, CONTROLLER_LB },
    { ImGuiKey::ImGuiKey_GamepadR2, CONTROLLER_RT },
    { ImGuiKey::ImGuiKey_GamepadL2, CONTROLLER_LT },
    { ImGuiKey::ImGuiKey_GamepadDpadUp, CONTROLLER_Up },
    { ImGuiKey::ImGuiKey_GamepadDpadDown, CONTROLLER_Down },
    { ImGuiKey::ImGuiKey_GamepadDpadLeft, CONTROLLER_Left },
    { ImGuiKey::ImGuiKey_GamepadDpadRight, CONTROLLER_Right },
    { ImGuiKey::ImGuiKey_GamepadLStickUp, CONTROLLER_LTHUMBSTICK_UP },
    { ImGuiKey::ImGuiKey_GamepadLStickDown, CONTROLLER_LTHUMBSTICK_DOWN },
    { ImGuiKey::ImGuiKey_GamepadLStickLeft, CONTROLLER_LTHUMBSTICK_LEFT },
    { ImGuiKey::ImGuiKey_GamepadLStickRight, CONTROLLER_LTHUMBSTICK_RIGHT },
    { ImGuiKey::ImGuiKey_GamepadRStickUp, CONTROLLER_RTHUMBSTICK_UP },
    { ImGuiKey::ImGuiKey_GamepadRStickDown, CONTROLLER_RTHUMBSTICK_DOWN },
    { ImGuiKey::ImGuiKey_GamepadRStickLeft, CONTROLLER_RTHUMBSTICK_LEFT },
    { ImGuiKey::ImGuiKey_GamepadRStickRight, CONTROLLER_RTHUMBSTICK_RIGHT },
    { ImGuiKey::ImGuiKey_GraveAccent, KEY_GraveAccent },
    // { ImGuiKey::ImGuiKey_OEMEqual, KEY_OEMEqual },
    { ImGuiKey::ImGuiKey_LeftBracket, KEY_LeftBracket },
    { ImGuiKey::ImGuiKey_RightBracket, KEY_RightBracket },
    { ImGuiKey::ImGuiKey_Backslash, KEY_Backslash },
    { ImGuiKey::ImGuiKey_Semicolon, KEY_Semicolon },
    { ImGuiKey::ImGuiKey_Apostrophe, KEY_Apostrophe },
    { ImGuiKey::ImGuiKey_Slash, KEY_Slash },
    { ImGuiKey::ImGuiKey_KeypadEnter, KEY_NumEnter },
    // { ImGuiKey::ImGuiKey_World1, KEY_World1 },
    // { ImGuiKey::ImGuiKey_World2, KEY_World2 },
    // { ImGuiKey::ImGuiKey_Mouse4, MOUSE_4 },
    // { ImGuiKey::ImGuiKey_Mouse5, MOUSE_5 },
    // { ImGuiKey::ImGuiKey_Mouse6, MOUSE_6 },
    // { ImGuiKey::ImGuiKey_Mouse7, MOUSE_7 },
    // { ImGuiKey::ImGuiKey_Mouse8, MOUSE_8 },
    { ImGuiKey::ImGuiKey_None, KEY_None },
    // { ImGuiKey::ImGuiKey_Unknown, KEY_Unknown }
};

void renderKeybindsSettings() {
	auto mod = Mod::get();
	static auto ck = Loader::get()->getLoadedMod("geode.custom-keybinds");

	// I think, I am doing this not by the inteded way...
	bool foundKeybinds = false;
	for (auto str : mod->getSettingKeys()) {
		if (str == "keybinds-title") {
			foundKeybinds = true;
			continue;
		}

		if (!foundKeybinds) continue;


		auto setting = mod->getSetting(str);
		auto keybindName = setting->getDisplayName();
		ImGui::Text(keybindName.c_str());
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip(setting->getDescription()->c_str());
		ImGui::SameLine(200.f);

		std::vector<geode::Keybind> keybindVector = mod->getSettingValue<std::vector<geode::Keybind>>(str);



		for (Keybind keybind : keybindVector) {
			auto removeKeybindPopupName = fmt::format("Remove Keybind##{}", keybind.toString());

			if (ImGui::BeginPopupModal(removeKeybindPopupName.c_str())) {
				ImGui::Text(fmt::format("Are you sure, you want to remove the {} bind?", keybind.toString()).c_str());
				if (ImGui::Button("Remove")) {
					keybindVector.erase(std::remove(keybindVector.begin(), keybindVector.end(), keybind), keybindVector.end());
					mod->setSettingValue<std::vector<geode::Keybind>>(str, keybindVector);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
				ImGui::SetTooltip("Remove bind");
			}
			
			if (ImGui::Button(keybind.toString().c_str())) {
				ImGui::OpenPopup(removeKeybindPopupName.c_str());
			}
			ImGui::SameLine();
		}

		auto setKeybindPopupName = fmt::format("Set Keybind##{}", keybindName);
		if (ImGui::BeginPopupModal(setKeybindPopupName.c_str())) {
			static Keybind newKeybind;
			auto dispatcher = CCDirector::get()->getKeyboardDispatcher();

			auto shift = dispatcher->getShiftKeyPressed() ? KeyboardModifier::Shift : KeyboardModifier::None; 
			auto ctrl = dispatcher->getControlKeyPressed() ? KeyboardModifier::Control : KeyboardModifier::None;
			auto alt = dispatcher->getAltKeyPressed() ? KeyboardModifier::Alt : KeyboardModifier::None;
			auto cmd = dispatcher->getCommandKeyPressed() ? KeyboardModifier::Super : KeyboardModifier::None;
			newKeybind.modifiers = shift + ctrl + alt + cmd;

			for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key = static_cast<ImGuiKey>(key+1)) {
				if (!ImGui::IsKeyDown(key)) continue;
				if (key == ImGuiKey_MouseLeft || key == ImGuiKey_MouseRight || key == ImGuiKey_MouseMiddle) continue;
				newKeybind.key = imguiKeyToCocos[key];
			}

			ImGui::Text("Press any key or combination to set a new keybind...");
			ImGui::Text("New keybind: %s", newKeybind.toString().c_str());
			if (ImGui::Button("Save")) {
				if (std::find(keybindVector.begin(), keybindVector.end(), newKeybind) == keybindVector.end()) {
					keybindVector.push_back(newKeybind);
					mod->setSettingValue<std::vector<geode::Keybind>>(str, keybindVector);
				}
				newKeybind = Keybind();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				newKeybind = Keybind();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		auto plusLabel = fmt::format("+##{}", keybindName);
		if (ImGui::Button(plusLabel.c_str())) {
			ImGui::OpenPopup(setKeybindPopupName.c_str());
		}
	}
}

enum PropertiesTabs {
	P_GENERAL,
	P_INTERFACE,
	P_OBJECT_INFO,
	P_DRAWS,
	P_KEYBINDS,
	P_LEVEL_BROWSER,
	P_DEBUG
};

void ErGui::renderPropertiesModule() {
	if (ErGui::showProperties) {
		ImGui::SetNextWindowSize(ImVec2(800, 550), ImGuiCond_FirstUseEver);
		ImGui::Begin("Properties", &ErGui::showProperties, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
		static PropertiesTabs selectedTab = P_GENERAL;
		ImGui::BeginChild("Tabs", ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, 0), ImGuiChildFlags_Borders);
		if (ImGui::Selectable("General",			selectedTab == P_GENERAL)) selectedTab = P_GENERAL;
		if (ImGui::Selectable("Interface",			selectedTab == P_INTERFACE)) selectedTab = P_INTERFACE;
		if (ImGui::Selectable("Object Info",		selectedTab == P_OBJECT_INFO)) selectedTab = P_OBJECT_INFO;
		if (ImGui::Selectable("Draws",				selectedTab == P_DRAWS)) selectedTab = P_DRAWS;
		if (ImGui::Selectable("Keybinds",			selectedTab == P_KEYBINDS)) selectedTab = P_KEYBINDS;
		#ifdef GEAR_DEBUG
		if (ImGui::Selectable("Level Browser",		selectedTab == P_LEVEL_BROWSER)) selectedTab = P_LEVEL_BROWSER;
		if (ImGui::Selectable("Debug",				selectedTab == P_DEBUG)) selectedTab = P_DEBUG;
		#endif
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Settings", ImVec2(0, 0));
		switch (selectedTab)
		{
		case P_GENERAL:
			renderGeneralSettings();
			break;
		case P_INTERFACE:
			renderInterfaceSettings();
			break;
		case P_OBJECT_INFO:
			renderObjectInfoSettings();
			break;
		case P_DRAWS:
			renderDrawsSettings();
			break;
		case P_KEYBINDS:
			renderKeybindsSettings();
			break;
		case P_LEVEL_BROWSER:
			renderLevelBrowserSettings();
			break;
		case P_DEBUG:
			renderDebugSettings();
			break;
		default:
			break;
		}
		ImGui::EndChild();
		ImGui::End();
	}
}