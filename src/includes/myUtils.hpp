#pragma once
#include <imgui.h>
#include <imgui-cocos.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>
using namespace cocos2d;
using namespace geode::prelude;


namespace ErGui {
	bool isPointInPolygon(const cocos2d::CCPoint& pt, const std::vector<cocos2d::CCPoint>& polygon);

	void setMaxMin(int& value, int max, int min);
	void setMin(int& value, int min);

	inline auto lassoPatch =					geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x122926), { 0x90, 0x90 });
	inline auto editorUIbottomConstrainPatch =	geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x121DD2), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
	inline auto vanillaGridOpaquePatch =		geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x2dbe78), { 0x00 });
	inline auto disablSelectObjectInEditorUI1 = geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x12337d), { 0x90, 0x90, 0x90, 0x90, 0x90 });
	inline auto disablSelectObjectInEditorUI2 = geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x1235f0), { 0x90, 0x90, 0x90, 0x90, 0x90 });
	inline auto disablSelectObjectInEditorUI3 = geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x123530), { 0x90, 0x90, 0x90, 0x90, 0x90 });
	inline auto disablSelectObjectInEditorUI4 = geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x123518), { 0x90, 0x90, 0x90, 0x90, 0x90 });
	inline auto disablSelectObjectInEditorUI5 = geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x12336f), { 0x90, 0x90, 0x90, 0x90, 0x90 });
	inline auto disablSelectObjectInEditorUI6 = geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x1232bd), { 0x90, 0x90, 0x90, 0x90, 0x90 });


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
			bool m_wasHovered = false;
		};
	};

	bool compareCCArrays(CCArray* arr1, CCArray* arr2);
	void selectAllObjects();
	void selectAllObjectWithDirection(bool rightDir);

	void nextFreeLayer();

	void releaseEditorUIKeys();
	CCRect normalizeRect(CCRect rect);

	void forEachObject(GJBaseGameLayer const* game, std::function<void(GameObject*)> const& callback);

	std::array<CCPoint, 4> getCornersFromRotatedRect(CCPoint center, CCSize size, float rot);
	CCPoint getAxis(const CCPoint& p1, const CCPoint& p2);
	void rectProjection(const std::array<CCPoint, 4>& corners, const CCPoint& axis, float& min, float& max);
	bool overlap(float minA, float maxA, float minB, float maxB);
	bool checkOBBIntersection(CCPoint posA, CCSize sizeA, float rotA, CCPoint posB, CCSize sizeB, float rotB);

	bool isHitboxAtPoint(const CCPoint& touch, const CCRect& hitbox);
	CCRect getObjectHitbox(GameObject* obj);
	bool isObjectGonnaBeSelected(GameObject* obj);
	void selectEveryObjectInSquare(GameObject* obj);

	const float INPUT_ITEM_WIDTH = 160.f;
	const float SHORT_INPUT_ITEM_WIDTH = 120.f;
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
	inline std::optional<CCRect> selectRect;

	inline CCPoint beginTouchLocation;

	inline float moveStep = 30.f;
	inline float rotationStep = 90.f;
	inline float scaleStep = 0.5f;
	inline float skewStep = 15.f;
};