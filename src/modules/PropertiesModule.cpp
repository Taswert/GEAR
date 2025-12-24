#include "PropertiesModule.hpp"
#include "CustomImGuiWidgets.hpp"
#include "../features/SelectedObjectInfo.hpp"
#include "DebugModule.hpp"
#include "TransformObjectModule.hpp"


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
	bool gamewindowStaticRatio = mod->getSavedValue<bool>("gamewindow-static-ratio", false);	// Game window keeps executable window ratio

	// Playtest bools
	bool ptMusic = gm->getGameVariable("0002");			// Plays music during playtesting
	bool ptNoGrid = gm->getGameVariable("0079");		// Hides grid during playtesting
	bool ptSmoothFix = gm->getGameVariable("0102");		// Enables smooth fix during playtesting
	bool autoPause = gm->getGameVariable("0150");		// Pauses gameplay when starting playtest from StartPos
	bool ignoreDamage = gm->getGameVariable("0009");    // 



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
	float inputFloatWidth = 30.f * 7.f + style.ItemSpacing.x * 6.f;
	// position
	ImGui::SetNextItemWidth(inputFloatWidth);
	ImGui::InputFloat("MoveStep", &ErGui::moveStep, 1.f);
	if (ImGui::Button("0.5##MoveStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::moveStep = 0.5f;	ImGui::SameLine();
	if (ImGui::Button("2##MoveStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::moveStep = 2.f;		ImGui::SameLine();
	if (ImGui::Button("7.5##MoveStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::moveStep = 7.5f;	ImGui::SameLine();
	if (ImGui::Button("10##MoveStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::moveStep = 10.f;		ImGui::SameLine();
	if (ImGui::Button("15##MoveStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::moveStep = 15.f;		ImGui::SameLine();
	if (ImGui::Button("30##MoveStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::moveStep = 30.f;		ImGui::SameLine();
	if (ImGui::Button("150##MoveStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::moveStep = 150.f;	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	ImGui::Text("Presets");
	ImGui::Checkbox("Apply steps to Keybinds##MOVE", &ErGui::posStepToKeybinds);
	
	ImGui::Dummy({0.f, 5.f});

	// rotation
	ImGui::SetNextItemWidth(inputFloatWidth);
	ImGui::InputFloat("RotStep", &ErGui::rotationStep, 1.f);
	if (ImGui::Button("1##RotateStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::rotationStep = 1.f;		ImGui::SameLine();
	if (ImGui::Button("10##RotateStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::rotationStep = 10.f;	ImGui::SameLine();
	if (ImGui::Button("15##RotateStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::rotationStep = 15.f;	ImGui::SameLine();
	if (ImGui::Button("30##RotateStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::rotationStep = 30.f;	ImGui::SameLine();
	if (ImGui::Button("45##RotateStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::rotationStep = 45.f;	ImGui::SameLine();
	if (ImGui::Button("90##RotateStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::rotationStep = 90.f;	ImGui::SameLine();
	if (ImGui::Button("180##RotateStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::rotationStep = 180.f;	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	ImGui::Text("Presets");
	ImGui::Checkbox("Apply steps to Keybinds##ROTATE", &ErGui::rotStepToKeybinds);

	ImGui::Dummy({ 0.f, 5.f });

	// scale
	ImGui::SetNextItemWidth(inputFloatWidth);
	ImGui::InputFloat("ScaleStep", &ErGui::scaleStep, 0.25f);
	if (ImGui::Button("0.01##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::scaleStep = 0.01f;	ImGui::SameLine();
	if (ImGui::Button("0.05##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::scaleStep = 0.05f;	ImGui::SameLine();
	if (ImGui::Button("0.1##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::scaleStep = 0.1f;		ImGui::SameLine();
	if (ImGui::Button("0.25##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::scaleStep = 0.25f;	ImGui::SameLine();
	if (ImGui::Button("0.5##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::scaleStep = 0.5f;		ImGui::SameLine();
	if (ImGui::Button("1##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::scaleStep = 1.f;			ImGui::SameLine();
	if (ImGui::Button("2##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::scaleStep = 2.f;			ImGui::SameLine(0, style.ItemInnerSpacing.x);
	ImGui::Text("Presets");
	
	ImGui::Dummy({ 0.f, 5.f });

	// skew
	ImGui::SetNextItemWidth(inputFloatWidth);
	ImGui::InputFloat("SkewStep", &ErGui::skewStep, 1.f);
	if (ImGui::Button("1##SkewStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::skewStep = 1.f;		ImGui::SameLine();
	if (ImGui::Button("5##SkewStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::skewStep = 5.f;		ImGui::SameLine();
	if (ImGui::Button("10##SkewStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::skewStep = 10.f;		ImGui::SameLine();
	if (ImGui::Button("15##SkewStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::skewStep = 15.f;		ImGui::SameLine();
	if (ImGui::Button("30##SkewStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::skewStep = 30.f;		ImGui::SameLine();
	if (ImGui::Button("45##SkewStepPreset", ImVec2({ 30.f, 0.f }))) ErGui::skewStep = 45.f;		ImGui::SameLine(0, style.ItemInnerSpacing.x);
	ImGui::Text("Presets");



	ImGui::Dummy(ImVec2(10.f, 10.f));

	renderConfirmPopup([mod]() {
		mod->setSavedValue<bool>("auto-buildhelper", false);
		mod->setSavedValue<bool>("deselect-controls", false);
		mod->setSavedValue<bool>("gamewindow-static-ratio", false);
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
	bool hideObjectListPopup = mod->getSavedValue<bool>("hide-object-list-popup", true);		//
	bool autoswitchToBuildMode = mod->getSavedValue<bool>("autoswitch-to-build-mode", true);	//
	bool showLinkControls = gm->getGameVariable("0097");                                        // 
	bool showZoomControls = mod->getSavedValue<bool>("show-zoom-controls", true);				// Shows zoom controls buttons on toolbox



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

	//ImGui::SeparatorText("Styling Settings");

	ImGui::Dummy(ImVec2(10.f, 10.f));

	renderConfirmPopup([mod]() {
		mod->setSavedValue<bool>("triangle-color-wheel", true);
		mod->setSavedValue<bool>("rotate-color-wheel", false);
		mod->setSavedValue<bool>("hide-object-list-popup", true);
		mod->setSavedValue<bool>("autoswitch-to-build-mode", true);
		mod->setSavedValue<bool>("show-zoom-controls", true);
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
	ImGui::Text("Position");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ErGui::BetterDragFloat(ImVec4(255, 66, 66, 255), "##XPos", &testFloat);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ErGui::BetterDragFloat(ImVec4(66, 66, 255, 255), "##YPos", &testFloat);

	ImGui::Text("Scale");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
	ErGui::BetterDragFloat("##Scale", &testFloat);

	ImGui::Text("Opacity");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
	ErGui::BetterDragFloat("##Opacity", &testFloat);
	
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
		g_soiNeedsUpdate = true;
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


void ErGui::renderPropertiesModule() {
	if (ErGui::showProperties) {
		ImGui::SetNextWindowSize(ImVec2(800, 550), ImGuiCond_FirstUseEver);
		ImGui::Begin("Properties", &ErGui::showProperties, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
		static int selectedTab = 1;
		ImGui::BeginChild("Tabs", ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, 0), ImGuiChildFlags_Borders);
		if (ImGui::Selectable("General",		selectedTab == 1)) selectedTab = 1;
		if (ImGui::Selectable("Interface",		selectedTab == 2)) selectedTab = 2;
		if (ImGui::Selectable("Object Info",	selectedTab == 3)) selectedTab = 3;
		if (ImGui::Selectable("Draws",			selectedTab == 4)) selectedTab = 4;
		if (ImGui::Selectable("Debug",			selectedTab == 5)) selectedTab = 5;
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Settings", ImVec2(0, 0));
		switch (selectedTab)
		{
		case 1:
			renderGeneralSettings();
			break;
		case 2:
			renderInterfaceSettings();
			break;
		case 3:
			renderObjectInfoSettings();
			break;
		case 4:
			renderDrawsSettings();
			break;
		case 5:
			renderDebugSettings();
			break;
		default:
			break;
		}
		ImGui::EndChild();
		ImGui::End();
	}
}