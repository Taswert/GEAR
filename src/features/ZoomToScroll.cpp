#include <Geode/modify/EditorUI.hpp>
#include <../classes/GearEditorUI.hpp>
#include "../modules/GameWindowModule.hpp"
using namespace geode::prelude;

float m_scrollCurrent = 1.f;
float m_scrollTarget = 1.f;

CCPoint m_positionTarget = { 0.f, 0.f };
CCPoint m_positionCurrent = { 0.f, 0.f };

float m_scrollVelocity = 0.f; // если нужна инерци€
float m_minScroll = 0.f;      // нижн€€ граница
float m_maxScroll = 1000.f;   // верхн€€ граница


void GearEditorUI::scrollWheel(float p0, float p1) {
	//Zoom To Cursor + Expanded constractions
	if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed()) {
		auto winSize = CCDirector::sharedDirector()->getWinSize();
		CCLayer* batchLayer = this->m_editorLayer->m_objectLayer;
		if (batchLayer == nullptr) return GearEditorUI::scrollWheel(p0, p1);

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

		float scaleStep = oldScale * 0.1f;
		float newScale = oldScale + (p0 > 0 ? -scaleStep : scaleStep);
		newScale = std::clamp(newScale, 0.1f, 1000.0f);

		float ratio = newScale / oldScale;

		CCPoint delta = oldPosition - visibleCursor;
		CCPoint newPos = visibleCursor + delta * ratio;

		batchLayer->setScale(newScale);
		batchLayer->setPosition(newPos);
		//m_scrollTarget = newScale;
		//m_positionTarget = newPos;
		

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


// Some works on smooth scroll. Should integrate it in the EditorUI better
void GearEditorUI::myUpdate(float dt) {
	float smooth = 1.f - powf(0.001f, dt * 4.f);
	m_scrollCurrent += (m_scrollTarget - m_scrollCurrent) * smooth;
	m_positionCurrent += (m_positionTarget - m_positionCurrent) * smooth;
	//this->applyScroll();
	//log::info("Hi, from GearEditorUI::update! dt = {}", dt);
}

void GearEditorUI::applyScroll() {
	auto batchLayer = this->m_editorLayer->m_objectLayer;
	batchLayer->setScale(m_scrollCurrent);
	batchLayer->setPosition(m_positionCurrent);
}