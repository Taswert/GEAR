#pragma once
#include "myUtils.hpp"
#include "Hovering.hpp"
//#include <Geode/modify/GameObject.hpp>
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
			(obj->m_editorLayer == currentLayer || (obj->m_editorLayer2 == currentLayer && obj->m_editorLayer2 != 0) || currentLayer == -1)) {
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


		auto objRect = obj->m_textureRect;

		objRect.origin = cocos2d::CCPoint(
			(objRect.origin.x * cameraScale) + cameraPos.x,
			(objRect.origin.y * cameraScale) + cameraPos.y
		);

		objRect.size *= cameraScale;

		auto currentLayer = LevelEditorLayer::get()->m_currentLayer;
		if (objRect.intersectsRect(normalizeRect(ErGui::selectRect)) && 
			(obj->m_editorLayer == currentLayer || (obj->m_editorLayer2 == currentLayer && obj->m_editorLayer2 != 0) || currentLayer == -1)) {
			static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = true;
		}
		else {
			static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = false;
		}

		// Show Texture Boxes
		// Don't forget to clear drawNode before cycle
		//ErGui::touchedDN->drawRect(objRect, { 1.f - 1.f / 1.33f, 1.f - 1.f / 1.33f, 0, 0.1f }, 0.3f, { 1.f, 1.f, 0, 0.33f });
	}

	void resetHover(GameObject* obj) {
		static_cast<GearGameObject*>(obj)->m_fields->m_isHovered = false;
	}
}
