#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "TweenFunctions.hpp"
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
		// Selection Rolling
		CCArray* m_lastUnderCursor = nullptr;
		int m_lastUnderCursorIndex = 0;

		// Smooth Zooming
		bool m_isSmoothZooming = false;

		float m_zoomCurrent = 1.f;
		float m_zoomTarget = 1.f;
		CCPoint m_positionCurrent = { 0.f, 0.f };
		CCPoint m_positionTarget = { 0.f, 0.f };

		Fields() {}
		~Fields();
	};

	// MyFuncs
	void onHideUI(CCObject* sender);
	void myUpdate(float dt);
	void applyZoom(float scale, CCPoint position);
	CCArray* objectsAtPosition(CCPoint touchPoint); // Should be in GearLevelEditorLayer I think
	GameObject* objectAtPosition(CCPoint touchPoint);
	GameObject* objectAtPosition(CCArray* objArrAtPosition);
	void deleteObjectAndRemoveFromSelected(GameObject* obj, bool noUndo);
	//void registerKeybindsEventListeners();
	
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
	void deselectObject(GameObject* obj);
	void scrollWheel(float y, float x);
	void moveObjectCall(EditCommand command);
	void transformObjectCall(EditCommand command);
	void resetSelectedObjectsColor();

	void onDelete(CCObject* obj);
	void onDeleteSelected(CCObject* obj);
	void undoLastAction(CCObject* obj);
    void redoLastAction(CCObject* obj);

	static GearEditorUI* get();

	virtual void keyDown(cocos2d::enumKeyCodes p0, double timestamp);
	virtual void keyUp(cocos2d::enumKeyCodes p0, double timestamp);
};