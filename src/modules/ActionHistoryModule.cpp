#pragma once
#include "ActionHistoryModule.hpp"
#include <Geode/GeneratedPredeclare.hpp>
#include <cocos2d.h>

void ErGui::renderActionHistoryModule() {
	ImGui::Begin("ActionHistory-Module");
	auto undoArray = GameManager::sharedState()->m_levelEditorLayer->m_undoObjects;
	int i = 0;
	for (auto undoObject : CCArrayExt<UndoObject*>(undoArray)) {
		std::string myText = "[";
		myText += std::to_string(i);
		myText += "] COMMAND: ";
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
			break;
		}
		myText += "(" + std::to_string(static_cast<int>(undoObject->m_command)) + "): ";
		ImGui::Text(myText.c_str());
		i++;
	}
	ImGui::End();
}