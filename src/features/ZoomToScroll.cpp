#include <Geode/Enums.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <../classes/GearEditorUI.hpp>
#include <cstdlib>
#include "../modules/GameWindowModule.hpp"
#include "TweenFunctions.hpp"
using namespace geode::prelude;

float zoomStart = 0.f;
CCPoint positionStart = { 0.f, 0.f };

float elapsedTime = 0.f;


void GearEditorUI::scrollWheel(float p0, float p1) {
	if (m_fields->m_isSmoothZooming) {
			applyZoom(m_fields->m_zoomTarget, m_fields->m_positionTarget);
		}

	//Zoom To Cursor + Expanded constractions
	if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed()) {
		auto winSize = CCDirector::sharedDirector()->getWinSize();
		CCLayer* batchLayer = this->m_editorLayer->m_objectLayer;
		if (batchLayer == nullptr) return GearEditorUI::scrollWheel(p0, p1);

		if (m_fields->m_isSmoothZooming) {
			applyZoom(m_fields->m_zoomTarget, m_fields->m_positionTarget);
			elapsedTime = 0.f;
		}

		m_fields->m_zoomCurrent = batchLayer->getScale();
		m_fields->m_positionCurrent = batchLayer->getPosition();

		zoomStart = m_fields->m_zoomCurrent;
		positionStart = m_fields->m_positionCurrent;
		m_fields->m_isSmoothZooming = true;

		float oldScale = batchLayer->getScale();
		CCPoint oldPosition = batchLayer->getPosition();
		CCPoint visibleCursor;

		if (ErGui::isGameWindowHovered)
			visibleCursor = CCPoint{ ErGui::gameWindowTouchCoordinatesConvertedToWorldForZoom.x, ErGui::gameWindowTouchCoordinatesConvertedToWorldForZoom.y };
		else {
			CCPoint glPos = CCDirector::sharedDirector()->getOpenGLView()->getMousePosition();

			CCSize frameSize = CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();
			CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
			CCPoint visibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();

			float vx = (glPos.x / frameSize.width) * visibleSize.width + visibleOrigin.x;
			float vy = (glPos.y / frameSize.height) * visibleSize.height + visibleOrigin.y;
			vy = visibleSize.height - vy;
			visibleCursor = CCPoint{ vx, vy };
		}

		float scaleStep = oldScale * m_fields->m_zoomStep;
		float newScale = oldScale + (p0 > 0 ? -scaleStep : scaleStep);
		newScale = std::clamp(newScale, 0.1f, 1000.0f);

		float ratio = newScale / oldScale;

		CCPoint delta = oldPosition - visibleCursor;
		CCPoint newPos = visibleCursor + delta * ratio;

		// batchLayer->setScale(newScale);
		// batchLayer->setPosition(newPos);
		m_fields->m_zoomTarget = newScale;
		m_fields->m_positionTarget = newPos;
		// редактор вылетел когда я зажал контрол и пытался двигать камеру

		if (this->m_rotationControl)
			this->m_rotationControl->setScale(1.f / newScale);

		return;
	}


	// Horizontal Scroll by holding Shift
	if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())
		EditorUI::scrollWheel(p1, p0);
	else {
		EditorUI::scrollWheel(p0, p1);
	}
}

float lerp(float current, float target, float t) {
	return current + (target - current) * t;
}

CCPoint lerp(CCPoint current, CCPoint target, float t) {
	return CCPoint{
		current.x + (target.x - current.x) * t,
		current.y + (target.y - current.y) * t
	};
}

void GearEditorUI::myUpdate(float dt) {
	// Smooth Zooming
	if (m_fields->m_isSmoothZooming) {
		// Zoom Easing is disabled
		if (!Mod::get()->getSavedValue<bool>("zoom-easing", false)) {
			applyZoom(m_fields->m_zoomTarget, m_fields->m_positionTarget);
			m_fields->m_isSmoothZooming = false;
		}

		// There is elapsed time
		else if (elapsedTime < m_fields->m_zoomDuration) {
			float t = elapsedTime / m_fields->m_zoomDuration;
			t = tweenfunc::tweenTo(t, m_fields->m_easingType, &m_fields->m_easingRate);
			m_fields->m_zoomCurrent = lerp(zoomStart, m_fields->m_zoomTarget, t);
			m_fields->m_positionCurrent = lerp(positionStart, m_fields->m_positionTarget,t);
			applyZoom(m_fields->m_zoomCurrent, m_fields->m_positionCurrent);
			elapsedTime += dt;
		}
		// Elapsed time is over
		else {
			elapsedTime = 0.f;
			applyZoom(m_fields->m_zoomTarget, m_fields->m_positionTarget);
			m_fields->m_isSmoothZooming = false;
		}
	}
}

void GearEditorUI::applyZoom(float scale, CCPoint position) {
	auto batchLayer = this->m_editorLayer->m_objectLayer;
	m_fields->m_zoomCurrent = scale;
	m_fields->m_positionCurrent = position;
	batchLayer->setScale(scale);
	batchLayer->setPosition(position);
}