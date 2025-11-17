#pragma once
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <imgui_internal.h>
#include "myUtils.hpp"
#include <../classes/GearEditorUI.hpp>

namespace ErGui {
	inline int g_selectVersion = 0;

	void ccTouchBegan_LassoAndVanillaSwipe(GearEditorUI* editorUI, CCTouch* touch);
	void ccTouchMoved_Lasso(GearEditorUI* editorUI, CCTouch* touch, ccColor4F selectionFillColor, ccColor4F lassoColor);
	void ccTouchMoved_VanillaSwipe(GearEditorUI* editorUI, CCTouch* touch, ccColor4F selectionFillColor, ccColor4F lassoColor);
	bool ccTouchEnded_Selection(GearEditorUI* editorUI, CCTouch* touch);
}