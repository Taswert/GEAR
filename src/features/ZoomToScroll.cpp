#include <Geode/Enums.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <../classes/GearEditorUI.hpp>
#include <imgui.h>
#include "../modules/GameWindowModule.hpp"
#include "TweenFunctions.hpp"
using namespace geode::prelude;

float zoomStart = 0.f;
CCPoint positionStart = { 0.f, 0.f };

float elapsedTime = 0.f;

float lerp(float current, float target, float t) {
	return current + (target - current) * t;
}

CCPoint lerp(CCPoint current, CCPoint target, float t) {
	return CCPoint{
		current.x + (target.x - current.x) * t,
		current.y + (target.y - current.y) * t
	};
}

namespace ZoomToScroll {
	bool scrollWheel(GearEditorUI* self, float p0, float p1) {
		if (self->m_fields->m_isSmoothZooming) {
				self->applyZoom(self->m_fields->m_zoomTarget, self->m_fields->m_positionTarget);
			}

		//Zoom To Cursor + Expanded constractions
		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed()) {
			auto winSize = CCDirector::sharedDirector()->getWinSize();
			CCLayer* batchLayer = self->m_editorLayer->m_objectLayer;
			if (batchLayer == nullptr) return true;

			if (self->m_fields->m_isSmoothZooming) {
				self->applyZoom(self->m_fields->m_zoomTarget, self->m_fields->m_positionTarget);
				elapsedTime = 0.f;
			}

			self->m_fields->m_zoomCurrent = batchLayer->getScale();
			self->m_fields->m_positionCurrent = batchLayer->getPosition();

			zoomStart = self->m_fields->m_zoomCurrent;
			positionStart = self->m_fields->m_positionCurrent;
			self->m_fields->m_isSmoothZooming = true;

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

			float scaleStep = oldScale * Mod::get()->getSavedValue<float>("zoom-step", 0.1f);
			float newScale = oldScale + (p0 > 0 ? -scaleStep : scaleStep);
			newScale = std::clamp(newScale, 0.1f, 1000.0f);

			float ratio = newScale / oldScale;

			CCPoint delta = oldPosition - visibleCursor;
			CCPoint newPos = visibleCursor + delta * ratio;

			// batchLayer->setScale(newScale);
			// batchLayer->setPosition(newPos);
			self->m_fields->m_zoomTarget = newScale;
			self->m_fields->m_positionTarget = newPos;
			// редактор вылетел когда я зажал контрол и пытался двигать камеру

			if (self->m_rotationControl)
				self->m_rotationControl->setScale(1.f / newScale);

			return false;
		}
		return true;
	}

	void update(GearEditorUI* self, float dt) {
		
		// Smooth Zooming
		if (self->m_fields->m_isSmoothZooming) {
			auto mod = Mod::get();
			bool zoomEasing = mod->getSavedValue<bool>("zoom-easing", true);
			float zoomDuration = mod->getSavedValue<float>("zoom-duration", 0.15f);
			float easingRate = mod->getSavedValue<float>("zoom-easingRate", 2.5f);
			tweenfunc::TweenType easingType = static_cast<tweenfunc::TweenType>(mod->getSavedValue<int>("zoom-easingType", tweenfunc::TweenType::EaseOut));

			// Zoom Easing is disabled
			if (!zoomEasing) {
				self->applyZoom(self->m_fields->m_zoomTarget, self->m_fields->m_positionTarget);
				self->m_fields->m_isSmoothZooming = false;
			}

			// There is elapsed time
			else if (elapsedTime < zoomDuration) {
				float t = elapsedTime / zoomDuration;
				t = tweenfunc::tweenTo(t, easingType, &easingRate);
				self->m_fields->m_zoomCurrent = lerp(zoomStart, self->m_fields->m_zoomTarget, t);
				self->m_fields->m_positionCurrent = lerp(positionStart, self->m_fields->m_positionTarget,t);
				self->applyZoom(self->m_fields->m_zoomCurrent, self->m_fields->m_positionCurrent);
				elapsedTime += dt;
			}
			// Elapsed time is over
			else {
				elapsedTime = 0.f;
				self->applyZoom(self->m_fields->m_zoomTarget, self->m_fields->m_positionTarget);
				self->m_fields->m_isSmoothZooming = false;
			}

			auto tc = self->m_transformControl;
			if (tc && tc->isVisible()) {
				tc->scaleButtons(1.f / self->m_fields->m_zoomTarget);
			}
		}
	}
}

