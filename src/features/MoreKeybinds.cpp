#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <imgui.h>
#include "myUtils.hpp"
#include "razoomUtils.hpp"
#include "../classes/GearEditorUI.hpp"
#include "../modules/TransformObjectModule.hpp"
#include "../modules/PropertiesModule.hpp"
using namespace geode::prelude;

static std::set<cocos2d::enumKeyCodes> editorUIHoldingKeys;

#include <geode.custom-keybinds/include/Keybinds.hpp>
using namespace keybinds;

void releaseEditorUIKeys() {
	if (!editorUIHoldingKeys.empty()) {
		auto copy = editorUIHoldingKeys;
		for (const auto& key : copy) {
			CCDirector::sharedDirector()->getKeyboardDispatcher()->dispatchKeyboardMSG(key, false, false);
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
	}
}

void GearEditorUI::keyDown(cocos2d::enumKeyCodes p0) {
		
	// 4 - View Mode
	//if (p0 == cocos2d::enumKeyCodes::KEY_Four) {
	//	GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 4;
	//	return EditorUI::keyDown(p0);
	//}

	// 5 - some mode?
	//if (p0 == cocos2d::enumKeyCodes::KEY_Five) {
	//	GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 5;
	//}

	// Ctrl + A - Select All
	//if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed() && p0 == cocos2d::enumKeyCodes::KEY_A) {
	//	ErGui::selectAllObjects();
	//	return EditorUI::keyDown(p0);
	//}

	// Ctrl + B - Build Helper
	//if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed() && p0 == cocos2d::enumKeyCodes::KEY_B) {
	//	ErGui::getFakePauseLayer()->onBuildHelper(nullptr);
	//	return EditorUI::keyDown(p0);
	//}

	// Ctrl + W - Warp
	//if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed() && p0 == cocos2d::enumKeyCodes::KEY_W) {
	//	this->activateTransformControl(nullptr);
	//	log::info("Warped");
	//	return EditorUI::keyDown(p0);
	//}

	

	// todo: select all right / select all left
	editorUIHoldingKeys.insert(p0);
	EditorUI::keyDown(p0);
}

void GearEditorUI::keyUp(cocos2d::enumKeyCodes p0) {
	editorUIHoldingKeys.erase(p0);
	EditorUI::keyUp(p0);
}

void GearEditorUI::moveObjectCall(EditCommand command) {
	// So, let's cancel this call for my keybinds. I HOPE this will not cause aaaany bugs.
	//EditorUI::moveObjectCall(EditCommand command);
	return;
}

void GearEditorUI::transformObjectCall(EditCommand command) {
	if (EditCommand::RotateCW == command || EditCommand::RotateCCW == command) return;

	EditorUI::transformObjectCall(command);
}


void GearEditorUI::registerKeybindsEventListeners() {
	// Small Rotate
	this->addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
		if (event->isDown()) {
			rotateObjectsByKey(-45.f);
		}
		return ListenerResult::Propagate;
		}, "rotate-ccw-small"_spr);

	this->addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
		if (event->isDown()) {
			rotateObjectsByKey(45.f);
		}
		return ListenerResult::Propagate;
		}, "rotate-cw-small"_spr);


	// 4
	this->addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
		if (event->isDown()) {
			//log::info("HIHIHI");
			GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 4;
			//log::info("BYEBYE\n");
		}
		return ListenerResult::Propagate;
		}, "view-mode"_spr);

	// Ctrl + A
	this->addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
		if (event->isDown()) {
			ErGui::selectAllObjects();
		}
		return ListenerResult::Propagate;
		}, "select-all"_spr);

	// Ctrl + W
	this->addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
		if (event->isDown()) {
			this->activateTransformControl(nullptr);
		}
		return ListenerResult::Propagate;
		}, "toggle-warp"_spr);

	// Ctrl + B
	this->addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
		if (event->isDown()) {
			ErGui::getFakePauseLayer()->onBuildHelper(nullptr);
		}
		return ListenerResult::Propagate;
		}, "apply-buildhelper"_spr);
}

$execute{
	// Custom keybinds
	auto bindManager = BindManager::get();

	// 4
	bindManager->registerBindable({
		"view-mode"_spr,
		"View Mode",
		"Toggle the View Tab",
		{ Keybind::create(KEY_Four, Modifier::None) },
		Category::EDITOR_UI,
		false
		}, "robtop.geometry-dash/delete-mode");

	// Shift+Q
	bindManager->registerBindable({
		"rotate-ccw-small"_spr,
		"Rotate CCW Small",
		"Rotate Object(s) 45 degrees (by Default) Counter-Clockwise",
		{ Keybind::create(KEY_Q, Modifier::Shift) },
		Category::EDITOR_MODIFY
		}, "robtop.geometry-dash/rotate-cw");

	// Shift+E
	bindManager->registerBindable({
		"rotate-cw-small"_spr,
		"Rotate CW Small",
		"Rotate Object(s) 45 degrees (by Default) Clockwise",
		{ Keybind::create(KEY_E, Modifier::Shift) },
		Category::EDITOR_MODIFY
		}, "rotate-ccw-small"_spr);

	// Ctrl+A
	bindManager->registerBindable({
		"select-all"_spr,
		"Select All",
		"Select all Objects",
		{ Keybind::create(KEY_A, Modifier::Control) },
		Category::EDITOR_MODIFY,
		false
		});

	// Ctrl+W
	bindManager->registerBindable({
		"toggle-warp"_spr,
		"Toggle Warp",
		"Toggle Warp Controls",
		{ Keybind::create(KEY_W, Modifier::Control) },
		Category::EDITOR_MODIFY,
		false
		});

	// Ctrl+B
	bindManager->registerBindable({
		"apply-buildhelper"_spr,
		"Apply Buildhelper",
		"Apply Buildhelper to selected objects",
		{ Keybind::create(KEY_B, Modifier::Control) },
		Category::EDITOR_MODIFY,
		false
		});



	// Globals
	// Default Steps (Position)
	new EventListener(+[](InvokeBindEvent* event) {
		if (event->isDown()) {
			if (ErGui::posStepToKeybinds)
				moveObjectsByKey(0.f, ErGui::moveStep);
			else
				moveObjectsByKey(0.f, EditorUI::get()->m_gridSize);
		}
		return ListenerResult::Stop;
	}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-up"));

	new EventListener(+[](InvokeBindEvent* event) {
		if (event->isDown()) {
			if (ErGui::posStepToKeybinds) 
				moveObjectsByKey(0.f, -ErGui::moveStep);
			else
				moveObjectsByKey(0.f, -EditorUI::get()->m_gridSize);
		}
		return ListenerResult::Propagate;
		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-down"));

	new EventListener(+[](InvokeBindEvent* event) {
		if (event->isDown()) {
			if (ErGui::posStepToKeybinds)
				moveObjectsByKey(-ErGui::moveStep, 0.f);
			else
				moveObjectsByKey(-EditorUI::get()->m_gridSize, 0.f);
		}
		return ListenerResult::Propagate;
		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-left"));

	new EventListener(+[](InvokeBindEvent* event) {
		if (event->isDown()) {
			if (ErGui::posStepToKeybinds)
				moveObjectsByKey(ErGui::moveStep, 0.f);
			else
				moveObjectsByKey(EditorUI::get()->m_gridSize, 0.f);
		}
		return ListenerResult::Propagate;
		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-right"));


	// Small Steps (Position)
	new EventListener(+[](InvokeBindEvent* event) {
		if (event->isDown()) {
			moveObjectsByKey(0.f, 2.f);
		}
		return ListenerResult::Propagate;
		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-up-small"));

	new EventListener(+[](InvokeBindEvent* event) {
		if (event->isDown()) {
			moveObjectsByKey(0.f, -2.f);
		}
		return ListenerResult::Propagate;
		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-down-small"));

	new EventListener(+[](InvokeBindEvent* event) {
		if (event->isDown()) {
			moveObjectsByKey(-2.f, 0.f);
		}
		return ListenerResult::Propagate;
		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-left-small"));

	new EventListener(+[](InvokeBindEvent* event) {
		if (event->isDown()) {
			moveObjectsByKey(2.f, 0.f);
		}
		return ListenerResult::Propagate;
		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-right-small"));


	// Rotation
	new EventListener(+[](InvokeBindEvent* event) {
		if (event->isDown()) {
			if (ErGui::rotStepToKeybinds)
				rotateObjectsByKey(-ErGui::rotationStep);
			else
				rotateObjectsByKey(-90.f);
		}
		return ListenerResult::Propagate;
		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/rotate-ccw"));

	new EventListener(+[](InvokeBindEvent* event) {
		if (event->isDown()) {
			if (ErGui::rotStepToKeybinds)
				rotateObjectsByKey(ErGui::rotationStep);
			else
				rotateObjectsByKey(90.f);
		}
		return ListenerResult::Propagate;
		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/rotate-cw"));

}