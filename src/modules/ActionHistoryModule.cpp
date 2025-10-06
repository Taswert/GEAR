#pragma once
#include "ActionHistoryModule.hpp"
#include <Geode/GeneratedPredeclare.hpp>
#include <cocos2d.h>

void ErGui::renderActionHistoryModule() {
	if (!ImGui::Begin("Action History")) {
		ImGui::End();
		return;
	}

	auto editorUI = GameManager::sharedState()->m_levelEditorLayer->m_editorUI;
	//auto dgl = GameManager::sharedState()->m_levelEditorLayer->m_drawGridLayer;
	//int aloSize = dgl->m_audioLineObjects.size();
	//std::string aloStr = "m_audioLineObjects size = " + std::to_string(aloSize);
	//ImGui::Text(aloStr.c_str());

	//int j = 0;
	//for (auto aloObj : dgl->m_audioLineObjects) {
	//	ImGui::Text("[%d] Object Address: %p / Key: %d", j, aloObj.second, aloObj.first);
	//	j++;
	//}
	bool undoObjectsCount = editorUI->m_editorLayer->m_undoObjects->count();
	bool redoObjectsCount = editorUI->m_editorLayer->m_redoObjects->count();
	
	if (!undoObjectsCount) {
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	if (ImGui::Button("Undo")) {
		editorUI->undoLastAction(nullptr);
	}
	if (!undoObjectsCount) {
		ImGui::PopStyleVar();
	}


	ImGui::SameLine();

	if (!redoObjectsCount) {
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	if (ImGui::Button("Redo")) {
		editorUI->redoLastAction(nullptr);
	}
	if (!redoObjectsCount) {
		ImGui::PopStyleVar();
	}


	if (ImGui::CollapsingHeader("Undo List")) {
		auto undoArray = GameManager::sharedState()->m_levelEditorLayer->m_undoObjects;
		int i = 0;
		for (auto undoObject : CCArrayExt<UndoObject*>(undoArray)) {
			std::string myText = "[";
			myText += std::to_string(i);
			myText += "]:";
			switch (undoObject->m_command)
			{
			case UndoCommand::Delete:
				myText += "Deleted single object ";
				break;
			case UndoCommand::DeleteMulti:
				myText += "Deleted multiple objects ";
				break;
			case UndoCommand::New:
				myText += "Created new object ";
				break;
			case UndoCommand::Paste:
				myText += "Pasted object(s) ";
				break;
			case UndoCommand::Transform:
				myText += "Transformed object(s) ";
				break;
			case UndoCommand::Select:
				myText += "Selected object(s) ";
				break;
			default:
				myText += "Something done to object(s) ";
				break;
			}
			myText += std::format("[{}]", static_cast<void*>(undoObject)) + "(" + std::to_string(undoObject->m_objects ? static_cast<int>(undoObject->m_objects->count()) : 1) + ")";
			ImGui::Text(myText.c_str());
			i++;
		}
	}

	if (ImGui::CollapsingHeader("Redo List")) {
		auto redoArray = GameManager::sharedState()->m_levelEditorLayer->m_redoObjects;
		int i = 0;
		for (auto redoObject : CCArrayExt<UndoObject*>(redoArray)) {
			std::string myText = "[";
			myText += std::to_string(i);
			myText += "]: ";
			switch (redoObject->m_command)
			{
			case UndoCommand::Delete:
				myText += "Deleted single object ";
				break;
			case UndoCommand::DeleteMulti:
				myText += "Deleted multiple objects ";
				break;
			case UndoCommand::New:
				myText += "Created new object ";
				break;
			case UndoCommand::Paste:
				myText += "Pasted object(s) ";
				break;
			case UndoCommand::Transform:
				myText += "Transformed object(s) ";
				break;
			case UndoCommand::Select:
				myText += "Selected object(s) ";
				break;
			default:
				myText += "Something done to object(s) ";
				break;
			}
			myText += std::format("[{}]", static_cast<void*>(redoObject)) + "(" + std::to_string(redoObject->m_objects ? static_cast<int>(redoObject->m_objects->count()) : 1) + ")";
			ImGui::Text(myText.c_str());
			i++;
		}
	}
	ImGui::End();
}