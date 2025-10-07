#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include "OpenGLStateHelper.h"
using namespace geode::prelude;

// По идее, нам нужна только структура RenderedTexture и всё.
// Стоит обсудить с Райниксом удаление этого и OpenGLStateHelper хеддеров.

//class ScreenRenderer {
//public:
//	struct RenderedTexture {
//		ImTextureID tex;
//		CCSize size;
//	};
//private:
//	static CCRenderTexture* gameRenderer;
//public:
//	static RenderedTexture render() {
//		auto winSize = CCDirector::sharedDirector()->getWinSize();
//		if (!gameRenderer) {
//			gameRenderer = CCRenderTexture::create(winSize.width, winSize.height, kTexture2DPixelFormat_RGBA8888);
//			gameRenderer->retain();
//		}
//
//		OpenGLStateHelper::saveState();
//		gameRenderer->beginWithClear(0, 0, 0, 0);
//		gameRenderer->begin();
//		CCDirector::sharedDirector()->getRunningScene()->visit();
//		gameRenderer->end();
//		OpenGLStateHelper::pushState();
//
//		auto glTextureID = gameRenderer->getSprite()->getTexture()->getName();
//		auto glTextureSize = gameRenderer->getSprite()->getTexture()->getContentSizeInPixels();
//
//
//		ImTextureID imguiTexture = (ImTextureID)(intptr_t)ErGui::gameFrame;
//		return RenderedTexture(imguiTexture, glTextureSize);
//	}
//
//	static CCRenderTexture* getCCRenderTexture() {
//		return gameRenderer;
//	}
//};
//CCRenderTexture* ScreenRenderer::gameRenderer = nullptr;