#pragma once
#include <Geode/modify/EditorUI.hpp>
#include "../modules/GameWindowModule.hpp"
using namespace geode::prelude;

class $modify(EditorUI) {
	void scrollWheel(float p0, float p1) {
		//Zoom To Cursor + Expanded constractions
		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed()) {
			auto winSize = CCDirector::sharedDirector()->getWinSize();
			CCLayer* batchLayer = this->m_editorLayer->m_objectLayer;
			if (batchLayer == nullptr) return EditorUI::scrollWheel(p0, p1);

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
			newScale = clamp(newScale, 0.1f, 1000.0f);

			float ratio = newScale / oldScale;

			CCPoint delta = oldPosition - visibleCursor;
			CCPoint newPos = visibleCursor + delta * ratio;

			batchLayer->setScale(newScale);
			batchLayer->setPosition(newPos);

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
};