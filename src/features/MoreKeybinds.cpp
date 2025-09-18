#pragma once
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <imgui.h>
#include "myUtils.hpp"
using namespace geode::prelude;

static std::set<cocos2d::enumKeyCodes> editorUIHoldingKeys;

void releaseEditorUIKeys() {
	if (!editorUIHoldingKeys.empty()) {
		auto copy = editorUIHoldingKeys;
		for (auto key : copy) {
			CCDirector::sharedDirector()->getKeyboardDispatcher()->dispatchKeyboardMSG(key, false, false);
		}
		editorUIHoldingKeys.clear();
	}
}

class $modify(CCEGLView) {
	void pollEvents() {
		auto& io = ImGui::GetIO();
		if (io.WantCaptureMouse && !editorUIHoldingKeys.empty()) {
			releaseEditorUIKeys();
		}
	}
};

class $modify(EditorUI) {
	virtual void keyDown(cocos2d::enumKeyCodes p0) {

		// 4 - View Mode
		if (p0 == cocos2d::enumKeyCodes::KEY_Four) {
			GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 4;
		}

		// 5 - some mode?
		//if (p0 == cocos2d::enumKeyCodes::KEY_Five) {
		//	GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 5;
		//}

		// Shift + A - Select All
		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed() && p0 == cocos2d::enumKeyCodes::KEY_A) {
			ErGui::selectAllObjects();
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