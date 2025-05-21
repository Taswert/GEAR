#include "ToolsModule.hpp"
#include "EditColorModule.hpp"
#include "IconsMaterialDesignIcons.h"

void parseObjects(const char* tabName, EditorUI* editorUI) {
	auto bsl = static_cast<BoomScrollLayer*>(editorUI->getChildByID(tabName)->getChildren()->objectAtIndex(0));
	auto el = static_cast<ExtendedLayer*>(bsl->getChildren()->objectAtIndex(0));
	for (ButtonPage* bp : CCArrayExt<ButtonPage*>(el->getChildren())) {
		auto menu = static_cast<CCMenu*>(bp->getChildren()->objectAtIndex(0));
		for (CreateMenuItem* cmi : CCArrayExt<CreateMenuItem*>(menu->getChildren())) {
			auto bs = static_cast<ButtonSprite*>(cmi->getChildren()->objectAtIndex(0));
			for (CCNode* gameObject : CCArrayExt<CCNode*>(bs->getChildren())) {
				if (dynamic_cast<GameObject*>(gameObject)) {
					std::cout << dynamic_cast<GameObject*>(gameObject)->m_objectID << ", ";
				}
			}
		}
	}
}

void EditorUI_toggleSpecialEditButtons(EditorUI* eui) {
	return reinterpret_cast<void(__thiscall*)(EditorUI*)>(geode::base::get() + 0x119010)(eui);
}

void ErGui::renderToolsModule1() {
	ImGui::Begin("Tools-Module1");
	ErGui::enableClicks();
		
	auto gameManager = GameManager::sharedState();
	auto editorUI = gameManager->getEditorLayer()->m_editorUI;

	
	ImGui::RadioButton("Build", &editorUI->m_selectedMode, 2);	//ICON_MDI_BRUSH
	ImGui::RadioButton("Edit", &editorUI->m_selectedMode, 3);	//ICON_MDI_SELECTION
	ImGui::RadioButton("Delete##Mode", &editorUI->m_selectedMode, 1);	//ICON_MDI_DELETE
	ImGui::RadioButton("Zoom", &editorUI->m_selectedMode, 4);	//ICON_MDI_MAGNIFY
	//ImGui::RadioButton("Image##RADIO",	&editorUI->m_selectedMode, 5);


	ImGui::Text("-");

	//reinterpret_cast<const char*>(u8"\uf1127")
	bool swipeBool = editorUI->m_swipeEnabled;
	if (ImGui::Checkbox("Swipe", &swipeBool)) { //ICON_MDI_GESTURE_TAP_HOLD
		editorUI->m_swipeEnabled = swipeBool;

		if (isLassoEnabled && editorUI->m_swipeEnabled) {
			lassoPatch->enable();
		}
		else {
			lassoPatch->disable();
		}
	}

	bool rotationBool = gameManager->getGameVariable("0007");
	if (ImGui::Checkbox("Rotate", &rotationBool)) { //ICON_MDI_ROTATE_RIGHT
		gameManager->setGameVariable("0007", rotationBool);
		if (rotationBool) {
			EditorUI_toggleSpecialEditButtons(editorUI);
		}
		else {
			editorUI->m_rotationControl->setVisible(false);
		}
	}

	bool freeMoveBool = editorUI->m_freeMoveEnabled;
	if (ImGui::Checkbox("Free Move", &freeMoveBool)) { //ICON_MDI_CURSOR_MOVE
		editorUI->m_freeMoveEnabled = freeMoveBool;
	}

	bool snapBool = gameManager->getGameVariable("0008");
	if (ImGui::Checkbox("Snap", &snapBool)) {	//ICON_MDI_AXIS_ARROW
		gameManager->setGameVariable("0008", snapBool);
	}
		
	ImGui::Text("-");

	if (ImGui::Button("Delete##Perm")) {	//ICON_MDI_DELETE_FOREVER
		if (editorUI->m_selectedObject) editorUI->deleteObject(editorUI->m_selectedObject, false);
		else if (editorUI->m_selectedObjects->count() > 0) editorUI->onDeleteSelected(nullptr);
	}

	if (ImGui::Button("Warp")) {	//ICON_MDI_VECTOR_SQUARE
		editorUI->activateTransformControl(nullptr);
	}

	if (ImGui::Button("Deselect")) { //ICON_MDI_SELECTION_OFF
		editorUI->deselectAll();
	}

	ImGui::Text("-");

	auto mode = *reinterpret_cast<int*>(reinterpret_cast<long long>(editorUI->m_editorLayer) + 0x878);
	std::string playtestStr = "Playtest"; //ICON_MDI_PLAY
	if (mode == 1) playtestStr = "Pause"; //ICON_MDI_PAUSE

	if (ImGui::Button(playtestStr.c_str())) {
		editorUI->onPlaytest(nullptr);
	}

	if (ImGui::Button("Playback")) { //ICON_MDI_PLAY_OUTLINE
		editorUI->onPlayback(nullptr);
	}

	if (mode == 1 || mode == 2) {
		if (ImGui::Button("Stop")) { //ICON_MDI_STOP
			editorUI->onStopPlaytest(nullptr);
		}
	}

	ImGui::Text("-");
	short step = 1;
	short fastStep = 5;
	if (ImGui::InputScalar("Layer", ImGuiDataType_S16, &editorUI->m_editorLayer->m_currentLayer, &step, &fastStep)) {
		if (editorUI->m_editorLayer->m_currentLayer < -1) 
			editorUI->m_editorLayer->m_currentLayer = -1;
	}


	if (ImGui::Button("Link")) { //ICON_MDI_LINK
		editorUI->onGroupSticky(nullptr);
	}
	
	if (ImGui::Button("Unlink")) { //ICON_MDI_LINK_OFF
		editorUI->onUngroupSticky(nullptr);
	}

	ImGui::Checkbox("DbgTDN", &ErGui::dbgTDN);

	ImGui::End();
}



void ErGui::renderToolsModule2() {
	ImGui::Begin("Tools-Module2");

	auto gameManager = GameManager::sharedState();
	auto editorUI = gameManager->getEditorLayer()->m_editorUI;

	switch (editorUI->m_selectedMode) {
		case 1: //Delete
		{
			int deleteFilterMode = GameManager::sharedState()->getIntGameVariable("0005");
			GameManager::sharedState()->setIntGameVariable("0006", editorUI->m_selectedObjectIndex);
			std::string customObjectStr = "Custom-" + std::to_string(GameManager::sharedState()->getIntGameVariable("0006")) + std::string("##RADIO");

			ImGui::RadioButton("None##RADIO", &deleteFilterMode, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Details##RADIO", &deleteFilterMode, 1);
			ImGui::SameLine();
			ImGui::RadioButton("Static##RADIO", &deleteFilterMode, 2);
			ImGui::SameLine();
			ImGui::RadioButton(customObjectStr.c_str(), &deleteFilterMode, 3);

			GameManager::sharedState()->setIntGameVariable("0005", deleteFilterMode);


			if (ImGui::Button("DeleteAllSP")) {
				editorUI->onDeleteStartPos(nullptr);
			}

			if (auto obj = editorUI->m_selectedObject) {
				std::string buttonStr = "DeleteAll-" + std::to_string(obj->m_objectID);
				ImGui::SameLine();
				if (ImGui::Button(buttonStr.c_str())) {
					editorUI->onDeleteSelectedType(nullptr);
				}
			}

			ImGui::SameLine();
			ImGui::Button("##OBJECT", { 30.f, 30.f });

			std::string newFrameName = ObjectToolbox::sharedState()->intKeyToFrame(editorUI->m_selectedObjectIndex);
			if (!newFrameName.empty()) {
				CCSpriteFrame* frame = CCSpriteFrameCache::get()->spriteFrameByName(newFrameName.c_str());
				if (frame)
					ErGui::drawFrameInImGui(frame);
			}

			
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
			ImGui::SetNextItemWidth(80.f);
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

			ImGui::Button("##SELECTED-OBJECT", { 30.f, 30.f });

			std::string newFrameName = ObjectToolbox::sharedState()->intKeyToFrame(editorUI->m_selectedObjectIndex);
			if (!newFrameName.empty()) {
				CCSpriteFrame* frame = CCSpriteFrameCache::get()->spriteFrameByName(newFrameName.c_str());
				if (frame)
					ErGui::drawFrameInImGui(frame);
			}

			auto ccMyColor2 = effectManager->getColorSprite(color2);
			if (ImGui::ColorButton("color2##COLOR-BUTTON", ImVec4(ccMyColor2->m_color.r / 255.f, ccMyColor2->m_color.g / 255.f, ccMyColor2->m_color.b / 255.f, 1.f))) {
				ImGui::OpenPopup("CSP##COLOR2");
			}
			ErGui::colorSelectImGuiPopup(&color2, "CSP##COLOR2", false);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.f);
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
			if (ImGui::Checkbox("Lasso", &isLassoEnabled)) {
				if (isLassoEnabled && editorUI->m_swipeEnabled) {
					//GameManager::sharedState()->setGameVariable("0003", true);
					lassoPatch->enable();
				}
				else {
					//GameManager::sharedState()->setGameVariable("0003", false);
					lassoPatch->disable();
				}
			}
			ImGui::SameLine();


			int selectMode = Mod::get()->getSavedValue<int>("select-mode");

			ImGui::RadioButton("Additive##RADIO",		&selectMode, 1);
			ImGui::SameLine();
			ImGui::RadioButton("Subtractive##RADIO",	&selectMode, 2);
			ImGui::SameLine();
			ImGui::RadioButton("Intersective##RADIO",	&selectMode, 3);
			

			Mod::get()->setSavedValue("select-mode", selectMode);
			break;
		}
		case 4: //Zoom
		{
			if (ImGui::Button("Zoom+")) editorUI->zoomIn(nullptr);
			ImGui::SameLine();
			if (ImGui::Button("Zoom-")) editorUI->zoomOut(nullptr);

			ImGui::SameLine();
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			float zoomMul = Mod::get()->template getSavedValue<float>("zoom-multiplier");
			if (ImGui::DragFloat("Zoom Mul", &zoomMul, 0.1f) && zoomMul >= 0.1f && zoomMul <= 10.f) {
				Mod::get()->setSavedValue("zoom-multiplier", zoomMul);
				editorUI->updateGridNodeSize();
			}

			ImGui::SameLine();
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			float gridSize = Mod::get()->template getSavedValue<float>("grid-size");
			if (ImGui::DragFloat("Grid Size", &gridSize, 0.5f) && gridSize >= 0.1f) {
				Mod::get()->setSavedValue("grid-size", gridSize);
				editorUI->updateGridNodeSize();
			}
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
