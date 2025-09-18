#pragma once
#include "myUtils.hpp"
#include "Hovering.hpp"
//#include <Geode/modify/GameObject.hpp>
using namespace geode::prelude;

namespace ErGui {
	void hoverObjectLasso(GameObject* obj) {
		auto cameraPos = EditorUI::get()->m_editorLayer->m_objectLayer->getPosition();
		auto cameraScale = EditorUI::get()->m_editorLayer->m_objectLayer->getScale();

		auto objPos = obj->getPosition();
		auto newPos = cocos2d::CCPoint(
			(objPos.x * cameraScale) + cameraPos.x,
			(objPos.y * cameraScale) + cameraPos.y
		);

		if (ErGui::isPointInPolygon(newPos, ErGui::editorUISwipePoints)) {
			static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = true;
		}
		else {
			static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = false;
		}
	}

	void hoverObjectSquare(GameObject* obj) {
		auto objLayer = EditorUI::get()->m_editorLayer->m_objectLayer;
		auto cameraPos = objLayer->getPosition();
		auto cameraScale = objLayer->getScale();

		auto objRect = obj->getObjectRect();

		objRect.origin = cocos2d::CCPoint(
			(objRect.origin.x * cameraScale) + cameraPos.x,
			(objRect.origin.y * cameraScale) + cameraPos.y
		);

		objRect.size *= cameraScale;

		if (objRect.intersectsRect(normalizeRect(ErGui::selectRect))) {
			static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = true;
		}
		else {
			static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = false;
		}

		//if (static_cast<GearGameObject*>(obj)->m_fields->m_isHovered)
			//ErGui::editorUIDrawNode->drawRect(objRect, { 1.f - 1.f / 1.33f, 1.f - 1.f / 1.33f, 0, 0.1f }, 0.3f, { 1.f, 1.f, 0, 1.f });
	}

	void resetHover(GameObject* obj) {
		static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = false;
	}
}
