#include "ContextMenuModule.hpp"
#include "EditGroupModule.hpp"


void renderContextForSingleObject() {
	auto editorUI = EditorUI::get();
	auto lel = LevelEditorLayer::get();
	auto selectedObject = editorUI->m_selectedObject;
	auto selectedObjects = editorUI->m_selectedObjects;

	if ((!selectedObject && !selectedObjects->count()) ||
		(ErGui::objectUnderCursor != selectedObject && !selectedObjects->count())) {
		editorUI->createUndoSelectObject(false);
		editorUI->selectObject(ErGui::objectUnderCursor, false);
	}

	if (ErGui::objectUnderCursor && (ErGui::objectUnderCursor == selectedObject ||
		selectedObjects->containsObject(ErGui::objectUnderCursor))) {

		if (!selectedObjects || selectedObjects->count() == 0) {

			if (ImGui::Selectable("Edit Object")) {
				editorUI->editObject(nullptr);
			}
			if (ImGui::Selectable("Edit Group")) {
				editorUI->editGroup(nullptr);
			}
			if (ImGui::Selectable("Edit Special")) {
				editorUI->editObjectSpecial(0);
			}
			ImGui::Dummy({ 5.f, 5.f });

		}

		if (ImGui::Selectable("De-Select##Single")) {
			editorUI->createUndoSelectObject(false);
			editorUI->deselectObject(ErGui::objectUnderCursor);
			ImGui::CloseCurrentPopup();
		}
	}
	else {
		if (ImGui::Selectable("Edit Object")) {
			editorUI->editObject(nullptr);
		}
		if (ImGui::Selectable("Edit Group")) {
			editorUI->editGroup(nullptr);
		}
		if (ImGui::Selectable("Edit Special")) {
			editorUI->editObjectSpecial(0);
		}
		ImGui::Dummy({ 5.f, 5.f });

		if (ImGui::Selectable("Select##Single")) {
			CCArray* toSelect = CCArray::create();
			if (selectedObject) toSelect->addObject(selectedObject);
			if (selectedObjects) toSelect->addObjectsFromArray(selectedObjects);
			toSelect->addObject(ErGui::objectUnderCursor);

			editorUI->createUndoSelectObject(false);
			editorUI->selectObjects(toSelect, false);
			ImGui::CloseCurrentPopup();
		}
	}


	if (ImGui::Selectable("Delete Object##Single")) {
		editorUI->deleteObject(ErGui::objectUnderCursor, false);
		ImGui::CloseCurrentPopup();
	}

	ImGui::Dummy({ 5.f, 5.f });
	if (ImGui::Selectable("Duplicate Object##Single")) {
		editorUI->deselectAll();
		editorUI->selectObject(ErGui::objectUnderCursor, false);

		editorUI->onDuplicate(nullptr);

		//editorUI->selectObjects(toSelect, false);
	}
	if (ImGui::Selectable("Copy Object##Single")) {
		CCArray* toSelect = CCArray::create();
		if (selectedObject) toSelect->addObject(selectedObject);
		if (selectedObjects) toSelect->addObjectsFromArray(selectedObjects);

		editorUI->deselectAll();
		editorUI->selectObject(ErGui::objectUnderCursor, false);

		editorUI->doCopyObjects(false);
		editorUI->selectObjects(toSelect, false);
	}
	if (ImGui::Selectable("Paste Objects##Single")) {
		editorUI->doPasteObjects(false);
	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("Copy Color##Single")) {
		lel->copyObjectState(ErGui::objectUnderCursor);
	}
	auto cso = lel->m_copyStateObject;
	if (ImGui::Selectable("Paste Color##Single") && cso) {
		auto copyStateObj = cso;
		auto gjBaseColor = ErGui::objectUnderCursor->m_baseColor;
		auto gjDetailColor = ErGui::objectUnderCursor->m_detailColor;

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

		ErGui::objectUnderCursor->m_customColorType = copyStateObj->m_customColorType;
	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("Copy State##Single")) {
		ErGui::copyStateObject.copyState(ErGui::objectUnderCursor);
	}
	if (ImGui::Selectable("Paste State##Single")) {
		ErGui::copyStateObject.pasteState(ErGui::objectUnderCursor);
	}

	ImGui::Dummy({ 5.f,5.f });

	if (ImGui::Selectable("Go To Layer##Single")) {
		lel->m_currentLayer = ErGui::objectUnderCursor->m_editorLayer;
	}
}


void renderContextForMultipleObjects() {
	auto editorUI = EditorUI::get();
	auto selectedObject = editorUI->m_selectedObject;
	auto selectedObjects = editorUI->m_selectedObjects;


	if (ImGui::Selectable("Edit Object")) {
		editorUI->editObject(nullptr);
	}
	if (ImGui::Selectable("Edit Group")) {
		editorUI->editGroup(nullptr);
	}
	if (ImGui::Selectable("Edit Special")) {
		editorUI->editObjectSpecial(0);
	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("De-Select All")) {
		editorUI->createUndoSelectObject(false);
		editorUI->deselectAll();
	}
	if (ImGui::Selectable("Delete Selected")) {
		if (editorUI->m_selectedObject) editorUI->deleteObject(editorUI->m_selectedObject, false);
		else if (editorUI->m_selectedObjects->count() > 0) editorUI->onDeleteSelected(nullptr);
	}
	if (ImGui::Selectable("Build Helper")) {
		ErGui::getFakePauseLayer()->onBuildHelper(nullptr);
	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("Duplicate Selected")) {
		editorUI->onDuplicate(nullptr);
	}
	if (ImGui::Selectable("Copy Selected")) {
		editorUI->doCopyObjects(false);
	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("Paste Color")) {
		for (auto obj : CCArrayExt<GameObject*>(selectedObjects)) {
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
	if (ImGui::Selectable("Paste State")) {
		ErGui::copyStateObject.pasteState(selectedObjects);
	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("Align X")) {
		ErGui::getFakePauseLayer()->onAlignX(nullptr);
	}
	if (ImGui::Selectable("Align Y")) {
		ErGui::getFakePauseLayer()->onAlignY(nullptr);
	}
}

void renderContextForNoObjects() {
	auto editorUI = EditorUI::get();
	auto lel = LevelEditorLayer::get();

	if (ImGui::Selectable("Select All")) {
		ErGui::selectAllObjects();
	}
	if (ImGui::Selectable("Select All Left")) {
		ErGui::selectAllObjectWithDirection(false);
	}
	if (ImGui::Selectable("Select All Right")) {
		ErGui::selectAllObjectWithDirection(true);
	}
	if (ImGui::Selectable("De-Select All")) {
		editorUI->createUndoSelectObject(false);
		editorUI->deselectAll();
	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("Paste Objects")) {
		editorUI->doPasteObjects(false);
	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("All Layer")) {
		lel->m_currentLayer = -1;
	}
	if (ImGui::Selectable("Next Free Layer")) {
		ErGui::nextFreeLayer();
	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("Level Settings")) {
		editorUI->onSettings(nullptr);
	}
}


void ErGui::renderContextMenu() {
	std::string contextMenuName = "ContextMenu";
	static bool wasOpen = false;

	if (ErGui::shouldOpenContextMenu) {
		ImGui::OpenPopup(contextMenuName.c_str());
		ErGui::shouldOpenContextMenu = false;
	}
	
	isContextMenuOpen = ImGui::IsPopupOpen(contextMenuName.c_str(), ImGuiPopupFlags_None);

	if (wasOpen && !isContextMenuOpen) {
		objectUnderCursor = nullptr;
	}

	wasOpen = isContextMenuOpen;


	if (ImGui::BeginPopup(contextMenuName.c_str())) {
		auto editorUI = EditorUI::get();
		auto selectedObject = editorUI->m_selectedObject;
		auto selectedObjects = editorUI->m_selectedObjects;


		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.f, 0.5f });
		ImGui::PushStyleColor(ImGuiCol_Separator, { 0.33f, 0.33f, 0.33f, 1.f });

		if (objectUnderCursor && selectedObjects && selectedObjects->containsObject(objectUnderCursor)) {
			ImGui::SeparatorText("Multiple");
			renderContextForMultipleObjects();
		}

		if (objectUnderCursor) {
			ImGui::SeparatorText("Single");
			renderContextForSingleObject();
		}

		if (!objectUnderCursor) {
			ImGui::SeparatorText("Editor");
			renderContextForNoObjects();
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		ImGui::EndPopup();
	}
}