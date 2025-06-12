#include "ContextMenuModule.hpp"
#include "EditGroupModule.hpp"


void renderContextForSingleObject() {
	auto editorUI = EditorUI::get();
	auto selectedObject = editorUI->m_selectedObject;
	auto selectedObjects = editorUI->m_selectedObjects;

	if (ErGui::objectUnderCursor && (ErGui::objectUnderCursor == selectedObject ||
		selectedObjects->containsObject(ErGui::objectUnderCursor))) {

		if (ImGui::Selectable("De-Select##Single")) {
			editorUI->createUndoSelectObject(false);
			editorUI->deselectObject(ErGui::objectUnderCursor);
			ImGui::CloseCurrentPopup();
		}
	}
	else {

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
		LevelEditorLayer::get()->copyObjectState(ErGui::objectUnderCursor);
	}
	if (ImGui::Selectable("Paste Color##Single")) {

	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("Copy State##Single")) {
		ErGui::copyStateObject.copyState(ErGui::objectUnderCursor);
	}
	if (ImGui::Selectable("Paste State##Single")) {
		ErGui::copyStateObject.pasteState(ErGui::objectUnderCursor);
	}
}


void renderContextForMultipleObjects() {
	auto editorUI = EditorUI::get();
	auto selectedObject = editorUI->m_selectedObject;
	auto selectedObjects = editorUI->m_selectedObjects;

	if (ImGui::Selectable("De-Select All")) {
		editorUI->createUndoSelectObject(false);
		editorUI->deselectAll();
	}
	if (ImGui::Selectable("Delete Selected")) {
		if (editorUI->m_selectedObject) editorUI->deleteObject(editorUI->m_selectedObject, false);
		else if (editorUI->m_selectedObjects->count() > 0) editorUI->onDeleteSelected(nullptr);
	}

	ImGui::Dummy({ 5.f, 5.f });

	if (ImGui::Selectable("Duplicate Selected")) {
		editorUI->onDuplicate(nullptr);
	}
	if (ImGui::Selectable("Copy Selected")) {
		editorUI->doCopyObjects(false);
	}

	ImGui::Dummy({ 5.f, 5.f });

	ImGui::Selectable("Paste Color");
	ImGui::Selectable("Paste State");

	ImGui::Dummy({ 5.f, 5.f });

	ImGui::Selectable("Align X");
	ImGui::Selectable("Align Y");

	ImGui::Dummy({ 5.f, 5.f });

	ImGui::Selectable("Build Helper");
	ImGui::Selectable("Re-Group");
	ImGui::Selectable("New Group X");
	ImGui::Selectable("New Group Y");

	ImGui::Dummy({ 5.f, 5.f });

	ImGui::Selectable("Create Loop");
	ImGui::Selectable("Create Extras");

}

void renderContextForNoObjects() {
	auto editorUI = EditorUI::get();

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

	ImGui::Selectable("Next Free Layer");
	ImGui::Selectable("All Layer");

	ImGui::Dummy({ 5.f, 5.f });

	ImGui::Selectable("Uncheck All Portals");
	ImGui::Selectable("Reset Unused Colors");

	ImGui::Dummy({ 5.f, 5.f });

	ImGui::Selectable("Level Settings");
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

		if (objectUnderCursor) {
			ImGui::SeparatorText("Single");
			renderContextForSingleObject();
		}

		if (objectUnderCursor && selectedObjects && selectedObjects->containsObject(objectUnderCursor)) {
			ImGui::SeparatorText("Multiple");
			renderContextForMultipleObjects();
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