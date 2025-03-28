#pragma once
#include <imgui.h>
#include <imgui-cocos.hpp>
#include <Geode/Geode.hpp>

//class LevelEditorLayer {
//	gd::string getSFXIDs(LevelEditorLayer* lel);
//	gd::string getSongIDs(LevelEditorLayer* lel, bool* idk);
//};

namespace ErGui {
	bool isPointInPolygon(const cocos2d::CCPoint& pt, const std::vector<cocos2d::CCPoint>& polygon);

	void createNewKeyframeAnim(EditorUI* self);

	void setMaxMin(int& value, int max, int min);
	void setMin(int& value, int min);

	float deltaInputFloat(const char* label, float step);

	int deltaInputIntImproved(const char* label, int max, int min, int step);

	inline auto lassoPatch = geode::Patch::create(reinterpret_cast<void*>(geode::base::get() + 0x122926), { 0x90, 0x90 });

	void enableClicks();
};