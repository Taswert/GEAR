#include <Geode/Enums.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <imgui.h>
#include "myUtils.hpp"
#include "razoomUtils.hpp"
#include "../classes/GearEditorUI.hpp"
#include "../modules/TransformObjectModule.hpp"
#include "../modules/PropertiesModule.hpp"
#include <Geode/loader/SettingV3.hpp>
using namespace geode::prelude;

static std::set<cocos2d::enumKeyCodes> editorUIHoldingKeys;

void releaseEditorUIKeys() {
	if (!editorUIHoldingKeys.empty()) {
		auto copy = editorUIHoldingKeys;
		for (const auto& key : copy) {
			CCDirector::sharedDirector()->getKeyboardDispatcher()->dispatchKeyboardMSG(key, false, false, 0.0);
		}
		editorUIHoldingKeys.clear();
	}
}

class $modify(CCEGLView) {
	void pollEvents() {
		auto& io = ImGui::GetIO();
		auto lel = LevelEditorLayer::get();
		if (io.WantCaptureMouse && !editorUIHoldingKeys.empty() && lel && lel->m_playbackMode != PlaybackMode::Playing) {
			releaseEditorUIKeys();
		}
	}
};

void moveObjectsByKey(float x, float y) {
	EditorUI* editorUI = EditorUI::get();
	auto objArr = editorUI->getSelectedObjects();
	if (objArr && objArr->count() > 0) {
		ErGui::addObjectsToUndoList(objArr, UndoCommand::Transform);

		auto rotControl = editorUI->m_rotationControl;
		rotControl->setPosition({ rotControl->getPositionX() + x, rotControl->getPositionY() + y });
		for (auto obj : CCArrayExt<GameObject*>(objArr)) {
			editorUI->moveObject(obj, { x, y });
		}

		auto tc = editorUI->m_transformControl;
		tc->setPosition({tc->getPositionX() + x, tc->getPositionY() + y});
	}
}

void rotateObjectsByKey(float degrees) {
	EditorUI* editorUI = EditorUI::get();
	auto objArr = editorUI->getSelectedObjects();
	if (objArr && objArr->count() > 0) {
		ErGui::addObjectsToUndoList(objArr, UndoCommand::Transform);
		
		if (!ErGui::rotateObjectsPositionSnap)
			editorUI->rotateObjects(objArr, degrees, { 0, 0 });
		else {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				float rotX = obj->getRotationX();
				float rotY = obj->getRotationY();

				rotX = std::fmod(rotX + degrees, 360);
				rotY = std::fmod(rotY + degrees, 360);

				obj->setRotationX(rotX);
				obj->setRotationY(rotY);
			}
		}

		auto tc = editorUI->m_transformControl;
		tc->setRotation(tc->getRotation() + degrees);
		tc->updateButtons(false, false);
	}
}

void GearEditorUI::keyDown(cocos2d::enumKeyCodes p0, double timestamp) {
	editorUIHoldingKeys.insert(p0);
	EditorUI::keyDown(p0, timestamp);
}

void GearEditorUI::keyUp(cocos2d::enumKeyCodes p0, double timestamp) {
	editorUIHoldingKeys.erase(p0);
	EditorUI::keyUp(p0, timestamp);
}
 
CCPoint GearEditorUI::moveForCommand(EditCommand command) {
	CCPoint returnPoint;
	bool posStepToKeybinds = Mod::get()->getSavedValue<bool>("move-step-to-bind", false);
	float moveStep = posStepToKeybinds ? Mod::get()->getSavedValue<float>("move-step", 30.f) : this->m_gridSize;

	bool customSwitchTriggered = false;

	switch(command) {
		case EditCommand::Up: {
			returnPoint = CCPoint(0.f, moveStep);
			customSwitchTriggered = true;
			break;
		}
		case EditCommand::Down: {
			returnPoint = CCPoint(0.f, -moveStep);
			customSwitchTriggered = true;
			break;
		}
		case EditCommand::Left: {
			returnPoint = CCPoint(-moveStep, 0.f);
			customSwitchTriggered = true;
			break;
		}
		case EditCommand::Right: {
			returnPoint = CCPoint(moveStep, 0.f);
			customSwitchTriggered = true;
			break;
		}
	}

	if (!customSwitchTriggered) 
		return EditorUI::moveForCommand(command);
	
	return returnPoint;
}

void GearEditorUI::transformObjectCall(EditCommand command) {
	auto rotStepToKeybinds = Mod::get()->getSavedValue<bool>("rot-step-to-bind", false);
	auto rotationStep = Mod::get()->getSavedValue<float>("rot-step", 90.f);
	auto objArr = getSelectedObjects();
	auto obj = static_cast<GameObject*>(getSelectedObjects()->objectAtIndex(0));

	if (command == EditCommand::RotateCW) {
		if (rotStepToKeybinds)
			rotateObjectsByKey(rotationStep);
		else {
			if (objArr->count() == 1) {
				ErGui::addObjectsToUndoList(objArr, UndoCommand::Transform);
				transformObject(obj, EditCommand::RotateCW, false);
			}
			else {
				rotateObjectsByKey(90.f);
			}
		}
	}
	else if (command == EditCommand::RotateCCW) {
		if (rotStepToKeybinds)
			rotateObjectsByKey(-rotationStep);
		else {
			if (objArr->count() == 1) {
				ErGui::addObjectsToUndoList(objArr, UndoCommand::Transform);
				transformObject(obj, EditCommand::RotateCCW, false);
			}
			else {
				rotateObjectsByKey(-90.f);
			}
		}
	}
	else EditorUI::transformObjectCall(command);
}


void GearEditorUI::registerKeybindsEventListeners() {
	// todo: select all right / select all left

	// 4
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "view-mode"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down && !repeat) {
			GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 4;
		}
	});

	// Ctrl + A
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "select-all"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down && !repeat) {
			ErGui::selectAllObjects();
		}
	});

	// Ctrl + B
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "apply-buildhelper"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down && !repeat) {
			ErGui::getFakePauseLayer()->onBuildHelper(nullptr);
		}
	});


	// Half Move Steps
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-up-half"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			moveObjectsByKey(0.f, EditorUI::get()->m_gridSize / 2.f);
		}
	});

	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-down-half"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			moveObjectsByKey(0.f, -EditorUI::get()->m_gridSize / 2.f);
		}
	});
	
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-left-half"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			moveObjectsByKey(-EditorUI::get()->m_gridSize / 2.f, 0.f);
		}
	});

	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-right-half"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			moveObjectsByKey(EditorUI::get()->m_gridSize / 2.f, 0.f);
		}
	});
	

	// Small Rotate
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "rotate-ccw-small"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			rotateObjectsByKey(-45.f);
		}
	});

	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "rotate-cw-small"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			rotateObjectsByKey(45.f);
		}
	});
}