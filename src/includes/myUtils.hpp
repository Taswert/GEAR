#pragma once
#include <imgui.h>
#include <imgui-cocos.hpp>
#include <Geode/Geode.hpp>
using namespace cocos2d;
using namespace geode::prelude;
//class LevelEditorLayer {
//	gd::string getSFXIDs(LevelEditorLayer* lel);
//	gd::string getSongIDs(LevelEditorLayer* lel, bool* idk);
//};

namespace ErGui {
	bool isPointInPolygon(const cocos2d::CCPoint& pt, const std::vector<cocos2d::CCPoint>& polygon);

	void setMaxMin(int& value, int max, int min);
	void setMin(int& value, int min);

	float deltaInputFloat(const char* label, float step);
	int deltaInputIntImproved(const char* label, int max, int min, int step);

	inline auto lassoPatch = geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x122926), { 0x90, 0x90 });
	inline auto editorUIbottomConstrainPatch = geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x121DD2), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

	void drawSpriteInImGui(CCSprite* tempSprite);
	void drawFrameInImGui(CCSpriteFrame* frame);

	void initImGuiStyling();

	void addObjectToUndoList(GameObject* obj, UndoCommand command);
	void addObjectsToUndoList(CCArray* objArr, UndoCommand command);

	template<typename T> bool foundInVector(std::vector<T> vec, T valueToFind) {
		bool found = false;
		for (T value : vec) {
			if (value == valueToFind) {
				found = true;
				break;
			}
		}

		return found;
	}

	const float INPUT_ITEM_WIDTH = 160.f;
	const float FIRST_ELEMENT_SAMELINE_SPACING = 70.f;
	inline CCDrawNode* touchedDN = nullptr;
	inline CCPoint touchedDNFirstPoint = { 0.f, 0.f };
	inline bool dbgTDN = false;
	inline GLuint gameFrame;
};