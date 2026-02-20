#include <Geode/Enums.hpp>
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
	// So, let's cancel this call for my keybinds. I HOPE this will not cause aaaany bugs.
	// P.S. It indeed caused some bugs, such as Editor Sound not working, but now it works pretty well.
	//auto ret = EditorUI::moveForCommand(command);
	return CCPoint(0.f, 0.f);
}

void GearEditorUI::transformObjectCall(EditCommand command) {
	if (EditCommand::RotateCW == command || EditCommand::RotateCCW == command) return;

	EditorUI::transformObjectCall(command);
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


	// Move Steps
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-up"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			auto posStepToKeybinds = Mod::get()->getSavedValue<bool>("move-step-to-bind", false);
			auto moveStep = Mod::get()->getSavedValue<float>("move-step", 30.f);
			
			if (posStepToKeybinds)
				moveObjectsByKey(0.f, moveStep);
			else
				moveObjectsByKey(0.f, EditorUI::get()->m_gridSize);
		}
	});

	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-down"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			auto posStepToKeybinds = Mod::get()->getSavedValue<bool>("move-step-to-bind", false);
			auto moveStep = Mod::get()->getSavedValue<float>("move-step", 30.f);

			if (posStepToKeybinds) 
				moveObjectsByKey(0.f, -moveStep);
			else
				moveObjectsByKey(0.f, -EditorUI::get()->m_gridSize);
		}
	});
	
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-left"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			auto posStepToKeybinds = Mod::get()->getSavedValue<bool>("move-step-to-bind", false);
			auto moveStep = Mod::get()->getSavedValue<float>("move-step", 30.f);

			if (posStepToKeybinds) 
				moveObjectsByKey(-moveStep, 0.f);
			else
				moveObjectsByKey(-EditorUI::get()->m_gridSize, 0.f);
		}
	});

	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-right"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			auto posStepToKeybinds = Mod::get()->getSavedValue<bool>("move-step-to-bind", false);
			auto moveStep = Mod::get()->getSavedValue<float>("move-step", 30.f);

			if (posStepToKeybinds) 
				moveObjectsByKey(moveStep, 0.f);
			else
				moveObjectsByKey(EditorUI::get()->m_gridSize, 0.f);
		}
	});


	// Small Move Steps
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-up-small"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			moveObjectsByKey(0.f, 2.f);
		}
	});

	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-down-small"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			moveObjectsByKey(0.f, -2.f);
		}
	});
	
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-left-small"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			moveObjectsByKey(-2.f, 0.f);
		}
	});

	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "move-obj-right-small"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			moveObjectsByKey(2.f, 0.f);
		}
	});


	// Rotation
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "rotate-ccw"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			auto rotStepToKeybinds = Mod::get()->getSavedValue<bool>("rot-step-to-bind", false);
			auto rotationStep = Mod::get()->getSavedValue<float>("rot-step", 90.f);
			if (rotStepToKeybinds)
				rotateObjectsByKey(-rotationStep);
			else {
				auto objArr = getSelectedObjects();
				auto obj = static_cast<GameObject*>(getSelectedObjects()->objectAtIndex(0));
				if (objArr->count() == 1) {
					transformObject(obj, EditCommand::RotateCCW, false);
				}
				else {
					rotateObjectsByKey(-90.f);
				}
			}
		}
	});

	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "rotate-cw"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			auto rotStepToKeybinds = Mod::get()->getSavedValue<bool>("rot-step-to-bind", false);
			auto rotationStep = Mod::get()->getSavedValue<float>("rot-step", 90.f);
			if (rotStepToKeybinds)
				rotateObjectsByKey(rotationStep);
			else {
				auto objArr = getSelectedObjects();
				auto obj = static_cast<GameObject*>(getSelectedObjects()->objectAtIndex(0));
				if (objArr->count() == 1) {
					transformObject(obj, EditCommand::RotateCW, false);
				}
				else {
					rotateObjectsByKey(90.f);
				}
			}
		}
	});
	

	// Small Rotate
	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "rotate-ccw-small"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			//rotateObjects(getSelectedObjects(), -45.f, getRotationPivotPoint()); // GeometryDash.exe + 0x6c1aa8 - Pivot Point
			rotateObjectsByKey(-45.f);
		}
	});

	addEventListener(KeybindSettingPressedEventV3(Mod::get(), "rotate-cw-small"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
		if (down) {
			//rotateObjects(getSelectedObjects(), 45.f, getRotationPivotPoint());
			rotateObjectsByKey(45.f);
		}
	});
}
//
//$execute{
//	// Custom keybinds
//	auto bindManager = BindManager::get();
//
//	// 4
//	bindManager->registerBindable({
//		"view-mode"_spr,
//		"View Mode",
//		"Toggle the View Tab",
//		{ Keybind::create(KEY_Four, Modifier::None) },
//		Category::EDITOR_UI,
//		false
//		}, "robtop.geometry-dash/delete-mode");
//
//	// Shift+Q
//	bindManager->registerBindable({
//		"rotate-ccw-small"_spr,
//		"Rotate CCW Small",
//		"Rotate Object(s) 45 degrees (by Default) Counter-Clockwise",
//		{ Keybind::create(KEY_Q, Modifier::Shift) },
//		Category::EDITOR_MODIFY
//		}, "robtop.geometry-dash/rotate-cw");
//
//	// Shift+E
//	bindManager->registerBindable({
//		"rotate-cw-small"_spr,
//		"Rotate CW Small",
//		"Rotate Object(s) 45 degrees (by Default) Clockwise",
//		{ Keybind::create(KEY_E, Modifier::Shift) },
//		Category::EDITOR_MODIFY
//		}, "rotate-ccw-small"_spr);
//
//	// Ctrl+A
//	bindManager->registerBindable({
//		"select-all"_spr,
//		"Select All",
//		"Select all Objects",
//		{ Keybind::create(KEY_A, Modifier::Control) },
//		Category::EDITOR_MODIFY,
//		false
//		});
//
//	// Ctrl+W
//	bindManager->registerBindable({
//		"toggle-warp"_spr,
//		"Toggle Warp",
//		"Toggle Warp Controls",
//		{ Keybind::create(KEY_W, Modifier::Control) },
//		Category::EDITOR_MODIFY,
//		false
//		});
//
//	// Ctrl+B
//	bindManager->registerBindable({
//		"apply-buildhelper"_spr,
//		"Apply Buildhelper",
//		"Apply Buildhelper to selected objects",
//		{ Keybind::create(KEY_B, Modifier::Control) },
//		Category::EDITOR_MODIFY,
//		false
//		});
//	
//	// Half Steps: Up
//	bindManager->registerBindable({
//		"move-obj-up-half"_spr,
//		"Move Object Up Half",
//		"Move Selected Object(s) Up 0.5 Grid Size (15 units by default)",
//		{ },
//		Category::EDITOR_MOVE,
//		true
//		});
//
//	// Half Steps: Down
//	bindManager->registerBindable({
//		"move-obj-down-half"_spr,
//		"Move Object Down Half",
//		"Move Selected Object(s) Down 0.5 Grid Size (15 units by default)",
//		{ },
//		Category::EDITOR_MOVE,
//		true
//		});
//
//	// Half Steps: Left
//	bindManager->registerBindable({
//		"move-obj-left-half"_spr,
//		"Move Object Left Half",
//		"Move Selected Object(s) Left 0.5 Grid Size (15 units by default)",
//		{ },
//		Category::EDITOR_MOVE,
//		true
//		});
//
//	// Half Steps: Right
//	bindManager->registerBindable({
//		"move-obj-right-half"_spr,
//		"Move Object Right Half",
//		"Move Selected Object(s) Right 0.5 Grid Size (15 units by default)",
//		{ },
//		Category::EDITOR_MOVE,
//		true
//		});
//
//
//
//	// Globals
//	// Default Steps (Position)
//	new EventListener(+[](InvokeBindEvent* event) {
//		if (event->isDown() && EditorUI::get()) {
//			if (ErGui::posStepToKeybinds)
//				moveObjectsByKey(0.f, ErGui::moveStep);
//			else
//				moveObjectsByKey(0.f, EditorUI::get()->m_gridSize);
//		}
//		return ListenerResult::Propagate;
//	}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-up"));
//
//	new EventListener(+[](InvokeBindEvent* event) {
//		if (event->isDown() && EditorUI::get()) {
//			if (ErGui::posStepToKeybinds) 
//				moveObjectsByKey(0.f, -ErGui::moveStep);
//			else
//				moveObjectsByKey(0.f, -EditorUI::get()->m_gridSize);
//		}
//		return ListenerResult::Propagate;
//		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-down"));
//
//	new EventListener(+[](InvokeBindEvent* event) {
//		if (event->isDown() && EditorUI::get()) {
//			if (ErGui::posStepToKeybinds)
//				moveObjectsByKey(-ErGui::moveStep, 0.f);
//			else
//				moveObjectsByKey(-EditorUI::get()->m_gridSize, 0.f);
//		}
//		return ListenerResult::Propagate;
//		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-left"));
//
//	new EventListener(+[](InvokeBindEvent* event) {
//		if (event->isDown() && EditorUI::get()) {
//			if (ErGui::posStepToKeybinds)
//				moveObjectsByKey(ErGui::moveStep, 0.f);
//			else
//				moveObjectsByKey(EditorUI::get()->m_gridSize, 0.f);
//		}
//		return ListenerResult::Propagate;
//		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-right"));
//
//
//	// Small Steps (Position)
//	new EventListener(+[](InvokeBindEvent* event) {
//		if (event->isDown() && EditorUI::get()) {
//			moveObjectsByKey(0.f, 2.f);
//		}
//		return ListenerResult::Propagate;
//		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-up-small"));
//
//	new EventListener(+[](InvokeBindEvent* event) {
//		if (event->isDown() && EditorUI::get()) {
//			moveObjectsByKey(0.f, -2.f);
//		}
//		return ListenerResult::Propagate;
//		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-down-small"));
//
//	new EventListener(+[](InvokeBindEvent* event) {
//		if (event->isDown() && EditorUI::get()) {
//			moveObjectsByKey(-2.f, 0.f);
//		}
//		return ListenerResult::Propagate;
//		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-left-small"));
//
//	new EventListener(+[](InvokeBindEvent* event) {
//		if (event->isDown() && EditorUI::get()) {
//			moveObjectsByKey(2.f, 0.f);
//		}
//		return ListenerResult::Propagate;
//		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/move-obj-right-small"));
//
//
//	// Rotation
//	new EventListener(+[](InvokeBindEvent* event) {
//		if (event->isDown() && EditorUI::get()) {
//			if (ErGui::rotStepToKeybinds)
//				rotateObjectsByKey(-ErGui::rotationStep);
//			else
//				rotateObjectsByKey(-90.f);
//		}
//		return ListenerResult::Propagate;
//		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/rotate-ccw"));
//
//	new EventListener(+[](InvokeBindEvent* event) {
//		if (event->isDown() && EditorUI::get()) {
//			if (ErGui::rotStepToKeybinds)
//				rotateObjectsByKey(ErGui::rotationStep);
//			else
//				rotateObjectsByKey(90.f);
//		}
//		return ListenerResult::Propagate;
//		}, InvokeBindFilter(nullptr, "robtop.geometry-dash/rotate-cw"));
//
//}