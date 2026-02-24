#include "AdvancedUndoRedo.hpp"
#include "CopyEGMState.hpp"
#include "Geode/modify/EditorUI.hpp"

// List
//
// Edit Object Values (colors, state, trigger settings)
// Edit Level Values
// Editor Values // View states, layer change, anything else?
//


// ����� ���� �� ������� ���� ������ ����� �� ���� ��������� �������� � ��� ���������� ������������ �������� ��������. 
// ���, ���� �����-�� ������ ���������, �� ������ ����� ���� �������, �� ��� ��������
//
// ������, ������� � ����� ������ ����� �� ������ ����� ����� �������� ���� ��������. 
// ����� � ��� ������ �� ��������� ���� ����� � ������ ����� ��� �����.
// 
// �����, ��������� ���� ����� ������, �������� ��������� ���� � �������.
//

// �� ������� �� ������.

ErGui::EnhancedUndoObject* ErGui::EnhancedUndoObject::create(GameObject* obj, EnhancedUndoCommand command) {
	EnhancedUndoObject* ret = new EnhancedUndoObject;
	if (ret->init(obj, command)) ret->autorelease();
	else CC_SAFE_DELETE(ret);
	return ret;
}

ErGui::EnhancedUndoObject* ErGui::EnhancedUndoObject::create(GameObject* obj, EnhancedUndoCommand command, int intValue) {
	EnhancedUndoObject* ret = new EnhancedUndoObject;
	if (ret->init(obj, command, intValue)) ret->autorelease();
	else CC_SAFE_DELETE(ret);
	return ret;
}

ErGui::EnhancedUndoObject* ErGui::EnhancedUndoObject::create(GameObject* obj, EnhancedUndoCommand command, bool boolValue) {
	EnhancedUndoObject* ret = new EnhancedUndoObject;
	if (ret->init(obj, command, boolValue)) ret->autorelease();
	else CC_SAFE_DELETE(ret);
	return ret;
}

bool ErGui::EnhancedUndoObject::init(GameObject* obj, EnhancedUndoCommand command) {
	if (!UndoObject::init(obj, static_cast<UndoCommand>(command)))
		return false;

	CopyEGMState copyStateObject;

	this->m_copyState = copyStateObject;
	this->m_copyState.copyState(obj);
	return true;
}

bool ErGui::EnhancedUndoObject::init(GameObject* obj, EnhancedUndoCommand command, int intValue) {
	if (!UndoObject::init(obj, static_cast<UndoCommand>(command)))
		return false;
	this->m_intValue = intValue;
	return true;
}

bool ErGui::EnhancedUndoObject::init(GameObject* obj, EnhancedUndoCommand command, bool boolValue) {
	if (!UndoObject::init(obj, static_cast<UndoCommand>(command)))
		return false;
	this->m_boolValue = boolValue;
	return true;
}


void enhancedUndoActionForObject(ErGui::EnhancedUndoObject* eUndoObject) {
	int commandNumber = static_cast<int>(eUndoObject->m_command);
	auto obj = eUndoObject->m_objectCopy->m_object;

	switch (static_cast<ErGui::EnhancedUndoCommand>(commandNumber))
	{
	case ErGui::EnhancedUndoCommand::ObjectState: {
		ErGui::CopyEGMState newState;
		newState.copyState(obj);
		eUndoObject->m_copyState.pasteState(obj);
		eUndoObject->m_copyState = newState;
		break;
	}
	case ErGui::EnhancedUndoCommand::GroupAdd:

		//obj->removeFromGroup(groupInt);
		//static_cast<CCArray*>(LevelEditorLayer::get()->m_groups[groupInt])->removeObject(obj, false);
		break;
	case ErGui::EnhancedUndoCommand::GroupRemove:
		// todo
		break;
	case ErGui::EnhancedUndoCommand::ObjectColor:
		// todo
		break;
	case ErGui::EnhancedUndoCommand::LevelValues:
		// todo
		break;
	case ErGui::EnhancedUndoCommand::EditorValues:
		// todo
		break;
	default:
		break;
	}
}

void enhancedUndoActionForArray(ErGui::EnhancedUndoObject* eUndoObject) {
	int commandNumber = static_cast<int>(eUndoObject->m_command);
	auto objArr = eUndoObject->m_objects;

	switch (static_cast<ErGui::EnhancedUndoCommand>(commandNumber))
	{
	case ErGui::EnhancedUndoCommand::ObjectState: {
		ErGui::CopyEGMState newState;
		newState.copyState(eUndoObject->m_objectCopy->m_object);
		eUndoObject->m_copyState.pasteState(eUndoObject->m_objectCopy->m_object);
		eUndoObject->m_copyState = newState;
		break;
	}
	case ErGui::EnhancedUndoCommand::GroupAdd:

		break;
	case ErGui::EnhancedUndoCommand::GroupRemove:
		// todo
		break;
	case ErGui::EnhancedUndoCommand::ObjectColor:
		// todo
		break;
	case ErGui::EnhancedUndoCommand::LevelValues:
		// todo
		break;
	case ErGui::EnhancedUndoCommand::EditorValues:
		// todo
		break;
	default:
		break;
	}
}

// class $modify(EditorUI) {
// 	void undoLastAction(CCObject * p0) {
// 		if (this->m_editorLayer->m_undoObjects->count() == 0) return;

// 		auto undoObject = static_cast<UndoObject*>(this->m_editorLayer->m_undoObjects->lastObject());
// 		int commandNumber = static_cast<int>(undoObject->m_command);
// 		if (commandNumber < 7)
// 			EditorUI::undoLastAction(p0);
// 		else {
// 			auto eUndoObject = static_cast<ErGui::EnhancedUndoObject*>(undoObject);
// 			auto obj = eUndoObject->m_objectCopy->m_object;
// 			auto objArr = eUndoObject->m_objects;

// 			if (obj) {
// 				enhancedUndoActionForObject(eUndoObject);
// 				this->m_editorLayer->m_redoObjects->addObject(eUndoObject);
// 				eUndoObject->m_redo = true;
// 				this->m_editorLayer->m_undoObjects->removeLastObject();
// 			}
// 			else if (objArr && objArr->count() > 0) {
// 				enhancedUndoActionForArray(eUndoObject);
// 				this->m_editorLayer->m_redoObjects->addObject(eUndoObject);
// 				eUndoObject->m_redo = true;
// 				this->m_editorLayer->m_undoObjects->removeLastObject();
// 			}
// 		}
// 	}

// 	void redoLastAction(CCObject * p0) {
// 		if (this->m_editorLayer->m_redoObjects->count() == 0) return;

// 		auto undoObject = static_cast<UndoObject*>(this->m_editorLayer->m_redoObjects->lastObject());
// 		int commandNumber = static_cast<int>(undoObject->m_command);
// 		if (commandNumber < 7)
// 			EditorUI::redoLastAction(p0);
// 		else {
// 			auto eUndoObject = static_cast<ErGui::EnhancedUndoObject*>(undoObject);
// 			switch (static_cast<ErGui::EnhancedUndoCommand>(commandNumber))
// 			{
// 			case ErGui::EnhancedUndoCommand::ObjectState: {
// 				ErGui::CopyEGMState newState;
// 				newState.copyState(eUndoObject->m_objectCopy->m_object);
// 				eUndoObject->m_copyState.pasteState(eUndoObject->m_objectCopy->m_object);
// 				eUndoObject->m_copyState = newState;
// 				break;
// 			}
// 			case ErGui::EnhancedUndoCommand::LevelValues:
// 				// todo
// 				break;
// 			case ErGui::EnhancedUndoCommand::EditorValues:
// 				// todo
// 				break;
// 			default:
// 				break;
// 			}

// 			this->m_editorLayer->m_undoObjects->addObject(eUndoObject);
// 			eUndoObject->m_redo = false;
// 			this->m_editorLayer->m_redoObjects->removeLastObject();
// 		}
// 	}
// };