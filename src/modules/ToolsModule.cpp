#include "ToolsModule.hpp"
#include "EditColorModule.hpp"
#include "IconsMaterialDesignIcons.h"
#include "GearIcons.hpp"
#include "ObjectListModule.hpp"
#include "EditGroupModule.hpp"
#include "../classes/GearEditorUI.hpp"

#include "../features/Hovering.hpp"


#include <geode.custom-keybinds/include/Keybinds.hpp>
using namespace keybinds;

void SameLineInWindow(float nextButtonSize, ImVec2 DummyPad) {
	float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	float lastButtonX2 = ImGui::GetItemRectMax().x;
	float nextButtonX2 = lastButtonX2 + ImGui::GetItemRectSize().x;

	if (nextButtonX2 < windowVisibleX2)
		ImGui::SameLine();
	else
		ImGui::Dummy(DummyPad);
}

void SetTooltipWithBind(const std::string& name, const keybinds::ActionID& actionID) {
	std::string bindsStr;
	for (auto what : BindManager::get()->getBindsFor(actionID)) {
		keybinds::Bind* bind = what.data();
		bindsStr += bind->toString();
	}
	ImGui::SetTooltip(fmt::format("{} ({})", name, bindsStr).c_str());
}

void ErGui::renderToolsModule1() {
	ImGui::Begin("Tools-Module1", nullptr, ImGuiWindowFlags_NoScrollbar);
	ImGui::GetWindowDockNode()->LocalFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDocking;

	auto gameManager = GameManager::sharedState();
	auto editorUI = EditorUI::get();

	const ImVec2 BTN_SIZE = ImVec2(24.f, 30.f);
	const ImVec2 DUMMY_PAD = ImVec2(0.f, 2.f);
	const float selectableRounding = ImGui::GetStyle().FrameRounding;

	ImGui::PushStyleColor(ImGuiCol_Separator, { 0.33f, 0.33f, 0.33f, 1.f });
	

	if (ImGui::Selectable(ICON_MDI_BRUSH, editorUI->m_selectedMode == 2, 0, BTN_SIZE, selectableRounding)) {
		editorUI->m_selectedMode = 2;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		SetTooltipWithBind("Build Mode", "robtop.geometry-dash/build-mode");
	}
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	if (ImGui::Selectable(ICON_MDI_SELECTION, editorUI->m_selectedMode == 3, 0, BTN_SIZE, selectableRounding))
		editorUI->m_selectedMode = 3;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		SetTooltipWithBind("Edit Mode", "robtop.geometry-dash/edit-mode");
	}
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	if (ImGui::Selectable(ICON_MDI_DELETE, editorUI->m_selectedMode == 1, 0, BTN_SIZE, selectableRounding))
		editorUI->m_selectedMode = 1;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Delete Mode (3)");
		SetTooltipWithBind("Delete Mode", "robtop.geometry-dash/delete-mode");
	}
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	if (ImGui::Selectable(ICON_MDI_EYE, editorUI->m_selectedMode == 4, 0, BTN_SIZE, selectableRounding))
		editorUI->m_selectedMode = 4;
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Observer Mode (4)");
		SetTooltipWithBind("View Mode", "view-mode"_spr);
	}
		
	ImGui::Separator();
	
	bool swipeBool = editorUI->m_swipeEnabled;
	if (ImGui::Selectable(ICON_MDI_GESTURE_TAP_HOLD, &swipeBool, 0, BTN_SIZE, selectableRounding)) {
		editorUI->m_swipeEnabled = swipeBool;

		if (isLassoEnabled && editorUI->m_swipeEnabled) {
			lassoPatch->enable();
		}
		else {
			lassoPatch->disable();
		}
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Swipe (T)");
		SetTooltipWithBind("Swipe", "robtop.geometry-dash/toggle-swipe");
	}
		
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	bool rotationBool = gameManager->getGameVariable("0007");
	if (ImGui::Selectable(ICON_MDI_ROTATE_RIGHT, &rotationBool, 0, BTN_SIZE, selectableRounding)) {
		gameManager->setGameVariable("0007", rotationBool);
		if (rotationBool) {
			editorUI->toggleSpecialEditButtons();
		}
		else {
			editorUI->m_rotationControl->setVisible(false);
		}
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Rotation (R)");
		SetTooltipWithBind("Rotate", "robtop.geometry-dash/toggle-rotate");
	}	
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	bool freeMoveBool = editorUI->m_freeMoveEnabled;
	if (ImGui::Selectable(ICON_MDI_CURSOR_MOVE, &freeMoveBool, 0, BTN_SIZE, selectableRounding)) {
		editorUI->m_freeMoveEnabled = freeMoveBool;
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Free Move (F)");
		SetTooltipWithBind("Free Move", "robtop.geometry-dash/toggle-free-move");
	}
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	bool snapBool = gameManager->getGameVariable("0008");
	if (ImGui::Selectable(ICON_MDI_AXIS_ARROW, &snapBool, 0, BTN_SIZE, selectableRounding)) {
		gameManager->setGameVariable("0008", snapBool);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Snap (G)");
		SetTooltipWithBind("Snap", "robtop.geometry-dash/toggle-snap");
	}
		
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);
	
	bool autoBuildHelperBool = Mod::get()->getSavedValue<bool>("auto-buildhelper", false);
	if (ImGui::Selectable(ICON_MDI_WRENCH, &autoBuildHelperBool, 0, BTN_SIZE, selectableRounding)) {
		Mod::get()->setSavedValue<bool>("auto-buildhelper", autoBuildHelperBool);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		ImGui::SetTooltip("AutoBuild Helper");
	}
		

	ImGui::Separator();


	auto selectedObject = editorUI->m_selectedObject;
	auto selectedObjects = editorUI->m_selectedObjects;
	auto copyStateObj = LevelEditorLayer::get()->m_copyStateObject;

	ImGui::BeginDisabled(selectedObject == nullptr);
	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.70f, 0.75f });
	
	if (ImGui::Selectable(ICON_GEAR_COPY_VALUES, false, 0, BTN_SIZE, selectableRounding)) {
		editorUI->onCopyState(nullptr);
		//LevelEditorLayer::get()->copyObjectState(selectedObject);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip("Copy Values");
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	ImGui::EndDisabled();


	ImGui::BeginDisabled((selectedObject == nullptr && selectedObjects->count() == 0) || copyStateObj == nullptr);

	if (ImGui::Selectable(ICON_GEAR_PASTE_VALUES, false, 0, BTN_SIZE, selectableRounding)) {
		editorUI->onPasteState(nullptr);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip("Paste State");
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	if (ImGui::Selectable(ICON_GEAR_PASTE_COLOR, false, 0, BTN_SIZE, selectableRounding)) {
		editorUI->onPasteColor(nullptr);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip("Paste Color");

	ImGui::PopStyleVar();
	ImGui::EndDisabled();


	ImGui::Separator();
	if (ImGui::Selectable(ICON_MDI_CONTENT_DUPLICATE, false, 0, BTN_SIZE, selectableRounding)) {
		editorUI->onDuplicate(nullptr);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Duplicate (Ctrl+D)");
		SetTooltipWithBind("Duplicate", "robtop.geometry-dash/copy-paste");
	}
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	if (ImGui::Selectable(ICON_MDI_DELETE_FOREVER, false, 0, BTN_SIZE, selectableRounding)) {
		if (editorUI->m_selectedObject) {
			editorUI->deleteObject(editorUI->m_selectedObject, false);
			editorUI->deselectAll();
		}
		else if (editorUI->m_selectedObjects->count() > 0) editorUI->onDeleteSelected(nullptr);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Delete Selected (Del)");
		SetTooltipWithBind("Delete Selected", "robtop.geometry-dash/delete");	
	}
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	if (ImGui::Selectable(ICON_MDI_VECTOR_SQUARE, false, 0, BTN_SIZE, selectableRounding)) {
		editorUI->activateTransformControl(nullptr);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Warp Selected (Ctrl+W)");
		SetTooltipWithBind("Warp Selected", "toggle-warp"_spr);
	}
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	if (ImGui::Selectable(ICON_MDI_RESIZE, false, 0, BTN_SIZE, selectableRounding)) {
		if (editorUI->m_scaleControl->isVisible()) {
			editorUI->deactivateScaleControl();
		}
		else if ((editorUI->m_selectedObject || editorUI->m_selectedObjects->count() > 0))
			editorUI->activateScaleControl(editorUI->getChildren()->firstObject());

	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Scale Transform");
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	if (ImGui::Selectable(ICON_MDI_SELECTION_OFF, false, 0, BTN_SIZE, selectableRounding)) {
		editorUI->createUndoSelectObject(false);
		editorUI->deselectAll();
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Deselect Objects (Alt+D)");
		SetTooltipWithBind("Deselect Objects", "robtop.geometry-dash/deselect-all");
	}


	ImGui::Separator();

	auto mode = editorUI->m_editorLayer->m_playbackMode;
	std::string playtestStr = ICON_MDI_PLAY; //ICON_MDI_PLAY
	if (mode == PlaybackMode::Playing) playtestStr = ICON_MDI_PAUSE; //ICON_MDI_PAUSE


	if (ImGui::Selectable(ICON_MDI_MUSIC_NOTE, false, 0, BTN_SIZE, selectableRounding)) {
		editorUI->onPlayback(nullptr);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip("Playback");
		SetTooltipWithBind("Playback", "robtop.geometry-dash/playback-music");
	}
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);
	if (ImGui::Selectable(playtestStr.c_str(), false, 0, BTN_SIZE, selectableRounding)) {
		editorUI->onPlaytest(nullptr);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
		//ImGui::SetTooltip(mode == PlaybackMode::Playing ? "Pause" : "Playtest");
		if (mode == PlaybackMode::Playing) {
			ImGui::SetTooltip("Pause");
		}
		else {
			SetTooltipWithBind("Playtest", "robtop.geometry-dash/playtest");
		}
	}

	if (mode == PlaybackMode::Playing || mode == PlaybackMode::Paused) {
		SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);
		if (ImGui::Selectable(ICON_MDI_STOP, false, 0, BTN_SIZE, selectableRounding)) {
			editorUI->onStopPlaytest(nullptr);
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
			//ImGui::SetTooltip("Stop");
			SetTooltipWithBind("Stop", "robtop.geometry-dash/playtest");
		}
	}

	ImGui::Separator();

	bool showLinkControls = gameManager->getGameVariable("0097");
	if (ImGui::Selectable(ICON_MDI_LINK_LOCK, &showLinkControls, 0, BTN_SIZE, selectableRounding)) {
		gameManager->setGameVariable("0097", showLinkControls);
		LevelEditorLayer::get()->updateOptions();
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Enable Link");
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);

	ImGui::BeginDisabled(!showLinkControls);

	if (ImGui::Selectable(ICON_MDI_LINK, false, 0, BTN_SIZE, selectableRounding)) {
		editorUI->onGroupSticky(nullptr);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip("Link Selected");
	SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);
	if (ImGui::Selectable(ICON_MDI_LINK_OFF, false, 0, BTN_SIZE, selectableRounding)) {
		editorUI->onUngroupSticky(nullptr);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip("Unlink Selected");

	ImGui::EndDisabled();

	//ImGui::Separator();
	//ImGui::Selectable(ICON_MDI_ALL_INCLUSIVE, &ErGui::dbgTDN, 0, BTN_SIZE);
	//if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
	//	ImGui::SetTooltip("DEBUG: Shows touched position");
	//ImGui::Checkbox("DbgTDN", &ErGui::dbgTDN);
	if (geode::Mod::get()->getSavedValue<bool>("show-zoom-controls", true)) {
		ImGui::Separator();

		if (ImGui::Selectable(ICON_MDI_MAGNIFY_PLUS, false, 0, BTN_SIZE, selectableRounding))
			editorUI->zoomIn(nullptr);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
			//ImGui::SetTooltip("Zoom In");
			SetTooltipWithBind("Zoom In", "robtop.geometry-dash/zoom-in");
		}
		SameLineInWindow(BTN_SIZE.x, DUMMY_PAD);
		if (ImGui::Selectable(ICON_MDI_MAGNIFY_MINUS, false, 0, BTN_SIZE, selectableRounding))
			editorUI->zoomOut(nullptr);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
			//ImGui::SetTooltip("Zoom Out");
			SetTooltipWithBind("Zoom Out", "robtop.geometry-dash/zoom-out");
		}
	}


	

	ImGui::PopStyleColor();

	ImGui::End();
}



void ErGui::renderToolsModule2() {
	if (!ImGui::Begin("Tools-Module2")) {
		ImGui::End();
		return;
	}

	const ImVec2 BTN_SIZE = ImVec2(24.f, 30.f);

	if (auto dockNode = ImGui::GetWindowDockNode()) {
		if (dockNode->Windows.size() == 1)
			dockNode->LocalFlags = ImGuiDockNodeFlags_NoTabBar;
		else
			dockNode->LocalFlags = ImGuiDockNodeFlags_None;
	}

	auto gameManager = GameManager::sharedState();
	auto editorUI = EditorUI::get();

	switch (editorUI->m_selectedMode) {
		case 1: //Delete
		{
			int deleteFilterMode = GameManager::sharedState()->getIntGameVariable("0005");
			GameManager::sharedState()->setIntGameVariable("0006", editorUI->m_selectedObjectIndex);
			std::string customObjectStr = fmt::format("Custom-{}##RADIO", GameManager::sharedState()->getIntGameVariable("0006"));

			ImGui::RadioButton("None##RADIO", &deleteFilterMode, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Details##RADIO", &deleteFilterMode, 1);
			ImGui::SameLine();
			ImGui::RadioButton("Static##RADIO", &deleteFilterMode, 2);
			ImGui::SameLine();
			ImGui::RadioButton(customObjectStr.c_str(), &deleteFilterMode, 3);

			GameManager::sharedState()->setIntGameVariable("0005", deleteFilterMode);


			//if (ImGui::Button("DeleteAllSP")) {				// 31
			//	editorUI->onDeleteStartPos(nullptr);
			//}

			int objId = 0;
			if (auto obj = editorUI->m_selectedObject) {
				objId = obj->m_objectID;
				//std::string buttonStr = "DeleteAll-" + std::to_string(obj->m_objectID);
				//ImGui::SameLine();
				//if (ImGui::Button(buttonStr.c_str())) {
				//	editorUI->onDeleteSelectedType(nullptr);
				//}
			}

			std::string newFrameName;
			auto lel = LevelEditorLayer::get();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));


			ImTextureID startposObjectTexture = (ImTextureID)(intptr_t)getObjectSprite(31)->getTexture()->getName();
			if (ImGui::ImageButton("##STARTPOS-OBJECT", startposObjectTexture, { 26.f, 26.f }, ImVec2(0, 1), ImVec2(1, 0))) {
				editorUI->deselectAll();
				lel->removeAllObjectsOfType(31);
				if (static_cast<UndoObject*>(lel->m_undoObjects->lastObject())->m_objects->count() == 0)
					lel->m_undoObjects->removeLastObject();
				editorUI->updateButtons();
			}
			ImGui::SameLine();


			// Texture
			if (objId == 0) {
				ImGui::Button("##SELECTED-OBJECT", { 30.f, 30.f });
			}
			else {
				ImTextureID selectedObjectTexture = (ImTextureID)(intptr_t)getObjectSprite(objId)->getTexture()->getName();
				if (ImGui::ImageButton("##SELECTED-OBJECT", selectedObjectTexture, { 26.f, 26.f }, ImVec2(0, 1), ImVec2(1, 0))) {
					editorUI->onDeleteSelectedType(nullptr);
				}
			}
			ImGui::SameLine();


			// Texture
			if (editorUI->m_selectedObjectIndex == 0) {
				ImGui::Button("##LIST-OBJECT", { 30.f, 30.f });
			}
			else {
				ImTextureID listObjectTexture = (ImTextureID)(intptr_t)getObjectSprite(editorUI->m_selectedObjectIndex)->getTexture()->getName();
				if (ImGui::ImageButton("##LIST-OBJECT", listObjectTexture, { 26.f, 26.f }, ImVec2(0, 1), ImVec2(1, 0))) {
					editorUI->deselectAll();
					lel->removeAllObjectsOfType(editorUI->m_selectedObjectIndex);
					if (static_cast<UndoObject*>(lel->m_undoObjects->lastObject())->m_objects->count() == 0)
						lel->m_undoObjects->removeLastObject();
					editorUI->updateButtons();
				}
			} 

			ImGui::PopStyleVar();
			
			break;
		}
		case 2: //Build
		{
			int color1 = Mod::get()->template getSavedValue<int>("build-color-1");
			int color2 = Mod::get()->template getSavedValue<int>("build-color-2");

			auto effectManager = gameManager->m_levelEditorLayer->m_effectManager;
			auto ccMyColor1 = effectManager->getColorSprite(color1);
			if (ImGui::ColorButton("color1##COLOR-BUTTON", ImVec4(ccMyColor1->m_color.r / 255.f, ccMyColor1->m_color.g / 255.f, ccMyColor1->m_color.b / 255.f, 1.f))) {
				ImGui::OpenPopup("CSP##COLOR1");
			}
			ErGui::colorSelectImGuiPopup(&color1, "CSP##COLOR1", false);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			if (ImGui::InputInt("##BUILD-COLOR-1", &color1)) {
				if (color1 > 1101) color1 = 1101;
				if (color1 < 0) color1 = 0;
			}

			ImGui::SameLine();
			bool enableColorBuild1 = Mod::get()->getSavedValue<bool>("enable-build-color-1");
			if (ImGui::Checkbox("##ENABLE-COLOR-BUILD-1", &enableColorBuild1)) {
				Mod::get()->setSavedValue("enable-build-color-1", enableColorBuild1);
			}

			ImGui::SameLine();

			//ImGui::Button("##SELECTED-OBJECT", { 30.f, 30.f });

			//std::string newFrameName = ObjectToolbox::sharedState()->intKeyToFrame(editorUI->m_selectedObjectIndex);
			//if (!newFrameName.empty()) {
			//	CCSpriteFrame* frame = CCSpriteFrameCache::get()->spriteFrameByName(newFrameName.c_str());
			//	if (frame)
			//		ErGui::drawFrameInImGui(frame);
			//}
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			if (editorUI->m_selectedObjectIndex) {
				ImTextureID texture = (ImTextureID)(intptr_t)getObjectSprite(editorUI->m_selectedObjectIndex)->getTexture()->getName();
				ImGui::ImageButton("##SELECTED-OBJECT", texture, { 27.f, 27.f }, ImVec2(0, 1), ImVec2(1, 0));
			}
			else {
				ImGui::Button("##SELECTED-OBJECT", { 30.f, 30.f });
			}
			ImGui::PopStyleVar();

			auto ccMyColor2 = effectManager->getColorSprite(color2);
			if (ImGui::ColorButton("color2##COLOR-BUTTON", ImVec4(ccMyColor2->m_color.r / 255.f, ccMyColor2->m_color.g / 255.f, ccMyColor2->m_color.b / 255.f, 1.f))) {
				ImGui::OpenPopup("CSP##COLOR2");
			}
			ErGui::colorSelectImGuiPopup(&color2, "CSP##COLOR2", false);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			if (ImGui::InputInt("##BUILD-COLOR-2", &color2)) {
				if (color2 > 1101) color2 = 1101;
				if (color2 < 0) color2 = 0;
			}

			ImGui::SameLine();
			bool enableColorBuild2 = Mod::get()->getSavedValue<bool>("enable-build-color-2");
			if (ImGui::Checkbox("##ENABLE-COLOR-BUILD-2", &enableColorBuild2)) {
				Mod::get()->setSavedValue("enable-build-color-2", enableColorBuild2);
			}

			Mod::get()->setSavedValue("build-color-1", color1);
			Mod::get()->setSavedValue("build-color-2", color2);

			

			break;
		}
		case 3: //Edit
		{
			const float selectableRounding = ImGui::GetStyle().FrameRounding;

			if (ImGui::Selectable(ICON_MDI_LASSO, isLassoEnabled, 0, BTN_SIZE, selectableRounding)) {
				if (isLassoEnabled && editorUI->m_swipeEnabled) {
					//GameManager::sharedState()->setGameVariable("0003", true);
					lassoPatch->enable();
				}
				else {
					//GameManager::sharedState()->setGameVariable("0003", false);
					lassoPatch->disable();
				}
				isLassoEnabled = !isLassoEnabled;
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
				ImGui::SetTooltip("Lasso");

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(5.f, 0.f));
			ImGui::SameLine();


			int selectMode = Mod::get()->getSavedValue<int>("select-mode", 1);

			if (ImGui::Selectable(ICON_GEAR_ADDITIVE_MODE, selectMode == 1, 0, BTN_SIZE, selectableRounding))
				selectMode = 1;
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
				ImGui::SetTooltip("Additive Mode");
			ImGui::SameLine();
			if (ImGui::Selectable(ICON_GEAR_SUBTRACTIVE_MODE, selectMode == 2, 0, BTN_SIZE, selectableRounding))
				selectMode = 2;
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
				ImGui::SetTooltip("Subtractive Mode");
			ImGui::SameLine();
			if (ImGui::Selectable(ICON_GEAR_INTERSECTIVE_MODE, selectMode == 3, 0, BTN_SIZE, selectableRounding))
				selectMode = 3;
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
				ImGui::SetTooltip("Intersective Mode");
			

			Mod::get()->setSavedValue("select-mode", selectMode);
			break;
		}
		case 4: //Zoom
		{
			if (ImGui::Selectable(ICON_MDI_MAGNIFY_PLUS, false, 0, BTN_SIZE)) editorUI->zoomIn(nullptr);
			ImGui::SameLine();
			if (ImGui::Selectable(ICON_MDI_MAGNIFY_MINUS, false, 0, BTN_SIZE)) editorUI->zoomOut(nullptr);

			//ImGui::SameLine();
			//ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			//float zoomMul = Mod::get()->template getSavedValue<float>("zoom-multiplier", 1.f);
			//if (ImGui::DragFloat("Zoom Mul", &zoomMul, 0.1f) && zoomMul >= 0.1f && zoomMul <= 10.f) {
			//	Mod::get()->setSavedValue("zoom-multiplier", zoomMul);
			//	editorUI->updateGridNodeSize();
			//}

			ImGui::SameLine();

			
			float gridSize = Mod::get()->template getSavedValue<float>("grid-size", 30.f);
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			if (ImGui::DragFloat("Grid Size", &gridSize, 0.5f) && gridSize >= 0.1f) {
				Mod::get()->setSavedValue("grid-size", gridSize);
				editorUI->updateGridNodeSize();
			}
			
			if (ImGui::Button("1##GridSizePreset"))		{ gridSize = 1.f; Mod::get()->setSavedValue("grid-size", gridSize); editorUI->updateGridNodeSize(); } ImGui::SameLine();
			if (ImGui::Button("5##GridSizePreset"))		{ gridSize = 5.f; Mod::get()->setSavedValue("grid-size", gridSize); editorUI->updateGridNodeSize(); } ImGui::SameLine();
			if (ImGui::Button("7.5##GridSizePreset"))	{ gridSize = 7.5f; Mod::get()->setSavedValue("grid-size", gridSize); editorUI->updateGridNodeSize(); } ImGui::SameLine(); // 0.25 of block
			if (ImGui::Button("10##GridSizePreset"))	{ gridSize = 10.f; Mod::get()->setSavedValue("grid-size", gridSize); editorUI->updateGridNodeSize(); } ImGui::SameLine(); // 0.33 of block
			if (ImGui::Button("15##GridSizePreset"))	{ gridSize = 15.f; Mod::get()->setSavedValue("grid-size", gridSize); editorUI->updateGridNodeSize(); } ImGui::SameLine(); // 0.5 of block
			if (ImGui::Button("30##GridSizePreset"))	{ gridSize = 30.f; Mod::get()->setSavedValue("grid-size", gridSize); editorUI->updateGridNodeSize(); } ImGui::SameLine(); // 1 block
			if (ImGui::Button("90##GridSizePreset"))	{ gridSize = 90.f; Mod::get()->setSavedValue("grid-size", gridSize); editorUI->updateGridNodeSize(); } ImGui::SameLine(); // 3 blocks
			if (ImGui::Button("150##GridSizePreset"))	{ gridSize = 150.f; Mod::get()->setSavedValue("grid-size", gridSize); editorUI->updateGridNodeSize(); } ImGui::SameLine(); // 5 blocks
			ImGui::Text("Grid Presets");
			break;
		}
		case 5: //Image
		{
			ImGui::Text("-----| Observer Mode |-----");

			break;
		}
		default:
			break;
	}

	ImGui::End();
}
