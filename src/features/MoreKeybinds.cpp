#pragma once
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <imgui.h>
#include "myUtils.hpp"
#include "razoomUtils.hpp"
using namespace geode::prelude;

static std::set<cocos2d::enumKeyCodes> editorUIHoldingKeys;

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
		if (io.WantCaptureMouse && !editorUIHoldingKeys.empty() && lel->m_playbackMode != PlaybackMode::Playing) {
			releaseEditorUIKeys();
		}
	}
};

void moveObjectsByKey(cocos2d::enumKeyCodes p0, cocos2d::enumKeyCodes key, float x, float y) {
	if (p0 == key) {
		EditorUI* editorUI = EditorUI::get();
		if (auto obj = editorUI->m_selectedObject) {
			editorUI->m_rotationControl->setPosition({ editorUI->m_rotationControl->getPositionX() + x, editorUI->m_rotationControl->getPositionY() + y });
			editorUI->moveObject(obj, { x, y });
		}
		if (auto objArr = editorUI->m_selectedObjects) {
			if (objArr->count() > 0) {
				auto rotControl = editorUI->m_rotationControl;
				rotControl->setPosition({ rotControl->getPositionX() + x, rotControl->getPositionY() + y });
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					editorUI->moveObject(obj, { x, y });
				}
			}
		}
	}
}

class $modify(EditorUI) {
	virtual void keyDown(cocos2d::enumKeyCodes p0) {
		//moveObjectsByKey(p0, cocos2d::enumKeyCodes::KEY_W, 0.f, ErGui::moveStep);
		//moveObjectsByKey(p0, cocos2d::enumKeyCodes::KEY_A, -ErGui::moveStep, 0.f);
		//moveObjectsByKey(p0, cocos2d::enumKeyCodes::KEY_S, 0.f, -ErGui::moveStep);
		//moveObjectsByKey(p0, cocos2d::enumKeyCodes::KEY_D, ErGui::moveStep, 0.f);

		// 4 - View Mode
		if (p0 == cocos2d::enumKeyCodes::KEY_Four) {
			GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 4;
		}

		// 5 - some mode?
		//if (p0 == cocos2d::enumKeyCodes::KEY_Five) {
		//	GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 5;
		//}

		// Ctrl + A - Select All
		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed() && p0 == cocos2d::enumKeyCodes::KEY_A) {
			ErGui::selectAllObjects();
		}

		// Ctrl + B - Build Helper
		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed() && p0 == cocos2d::enumKeyCodes::KEY_B) {
			ErGui::getFakePauseLayer()->onBuildHelper(nullptr);
		}

		// Ctrl + W - Warp
		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed() && p0 == cocos2d::enumKeyCodes::KEY_W) {
			this->activateTransformControl(nullptr);
		}

		// todo: select all right / select all left
		editorUIHoldingKeys.insert(p0);
		EditorUI::keyDown(p0);
	}

	virtual void keyUp(cocos2d::enumKeyCodes p0) {
		editorUIHoldingKeys.erase(p0);
		EditorUI::keyUp(p0);
	}
};