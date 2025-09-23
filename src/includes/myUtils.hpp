#pragma once
#include <imgui.h>
#include <imgui-cocos.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>
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

	inline auto lassoPatch =					geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x122926), { 0x90, 0x90 });
	inline auto editorUIbottomConstrainPatch =	geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x121DD2), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
	inline auto vanillaGridOpaquePatch =		geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x2dbe78), { 0x00 });

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

	class $modify(GearGameObject, GameObject) {
		struct Fields {
			bool m_isHovered = false;
		};
	};

	bool compareCCArrays(CCArray* arr1, CCArray* arr2);
	void selectAllObjects();
	void selectAllObjectWithDirection(bool rightDir);

	void nextFreeLayer();

	void releaseEditorUIKeys();
	CCRect normalizeRect(CCRect rect);

	void forEachObject(GJBaseGameLayer const* game, std::function<void(GameObject*)> const& callback);

	const float INPUT_ITEM_WIDTH = 160.f;
	const float FIRST_ELEMENT_SAMELINE_SPACING = 72.f;
	inline CCDrawNode* touchedDN = nullptr;
	inline CCPoint touchedDNFirstPoint = { 0.f, 0.f };
	inline bool dbgTDN = false;

	inline bool isGameWindowTouching = false;

	inline bool isFreeMoveAndObjectTouching = false;

	inline float gridWidth = 1.f;
	inline int gridColor[4] = { 0, 0, 0, 150 };

	inline bool isVisibleSelection = true;

	inline bool hideUI = false;

	inline std::vector<cocos2d::CCPoint> editorUISwipePoints;
	inline CCRect selectRect;

	inline CCPoint beginBatchLayerPosition;
};