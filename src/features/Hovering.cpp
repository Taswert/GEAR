#include "myUtils.hpp"
#include "Hovering.hpp"
//#include <Geode/modify/GameObject.hpp>
#include "SelectFilterModule.hpp"
using namespace geode::prelude;

namespace ErGui {
	void hoverObjectLasso(GameObject* obj) {
		auto batchLayer = EditorUI::get()->m_editorLayer->m_objectLayer;
		auto cameraPos = batchLayer->getPosition();
		auto cameraScale = batchLayer->getScale();

		auto objPos = obj->getPosition();
		auto newPos = cocos2d::CCPoint(
			(objPos.x * cameraScale) + cameraPos.x,
			(objPos.y * cameraScale) + cameraPos.y
		);

		auto currentLayer = LevelEditorLayer::get()->m_currentLayer;
		if (ErGui::isPointInPolygon(newPos, ErGui::editorUISwipePoints) &&
			(obj->m_editorLayer == currentLayer || (obj->m_editorLayer2 == currentLayer && obj->m_editorLayer2 != 0) || currentLayer == -1)
			&& selectFilterRealization(obj)) {
			static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = true;
		}
		else {
			static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = false;
		}
	}

	void hoverObjectSquare(GameObject* obj) {
		if (isObjectGonnaBeSelected(obj)) {
			static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = true;
		}
		else {
			static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = false;
		}
	}

	void resetHover(GameObject* obj) {
		static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = false;
	}
}
