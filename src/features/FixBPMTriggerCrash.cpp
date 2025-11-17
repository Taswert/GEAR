#pragma once
#include <Geode/modify/LevelEditorLayer.hpp>
using namespace geode::prelude;

class $modify(LevelEditorLayer) {
	void removeSpecial(GameObject * obj) {
		if (obj->m_objectID == 3642) {
			for (auto alObj : this->m_drawGridLayer->m_audioLineObjects) {
				if (alObj.second == obj) {
					this->m_drawGridLayer->m_audioLineObjects.erase(alObj.first);
				}
			}
		}
		LevelEditorLayer::removeSpecial(obj);
	}
};