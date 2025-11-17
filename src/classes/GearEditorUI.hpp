#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
#include "razoomUtils.hpp"

#include <matjson.hpp>
#include <matjson/reflect.hpp>
#include <matjson/std.hpp>
#include <matjson/stl_serialize.hpp>

#include <Geode/modify/EditorUI.hpp>
using namespace geode::prelude;

struct GearEditorUI : public geode::Modify<GearEditorUI, EditorUI> {
public:
	struct Fields {
		CCArray* m_lastUnderCursor = nullptr;
		int m_lastUnderCursorIndex = 0;
		Fields() {}
		~Fields();
	};

	// MyFuncs
	void onHideUI(CCObject* sender);
	
	// Hooks
	bool onCreate();
	void clickOnPosition(CCPoint p0);
	void reloadCustomItems();
	CCArray* pasteObjects(gd::string p0, bool p1, bool p2);
	GameObject* createObject(int p0, CCPoint p1);
	void selectObject(GameObject* obj, bool p1);
	void selectObjects(CCArray* objArr, bool p1);
	void processSelectObjects(CCArray* objArr);
	bool init(LevelEditorLayer* lel);
	void updateGridNodeSize();
	bool ccTouchBegan(CCTouch* touch, CCEvent* event);
	void ccTouchMoved(CCTouch* touch, CCEvent* event);
	void ccTouchEnded(CCTouch* touch, CCEvent* event);
	void deselectAll();
};