#include "GameWindowModule.hpp"
#include "../helpers/ScreenRenderer.h"

namespace ErGui {
	bool isGameWindowHovered = false;
	CCPoint gameWindowTouchCoordinatesConvertedToWorld;
	CCPoint gameWindowTouchCoordinatesConvertedToWorldForZoom;
	static bool constrainByLastObject = false;
}

void ErGui::renderGameWindow() {

	auto renderedScreen = ScreenRenderer::render();
	auto textureRatio = renderedScreen.size.width / renderedScreen.size.height;

	ImGui::Begin("Game");
	//if (ImGui::Button("Save")) {
	//	auto winSize = CCDirector::sharedDirector()->getWinSize();
	//	auto rt = CCRenderTexture::create(winSize.width, winSize.height, kTexture2DPixelFormat_RGBA8888);
	//	rt->begin();
	//	CCDirector::sharedDirector()->getRunningScene()->visit();
	//	rt->end();

	//	// Собираем полный путь
	//	std::string path = "coolName.png";
	//	bool ok = rt->saveToFile(path.c_str(), cocos2d::kCCImageFormatPNG);
	//	if (ok) {
	//		std::cout << "Done! Saved to %s";
	//	}
	//	else {
	//		std::cout << "Nah...";
	//	}
	//}
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	float objectLayerX = lel->m_objectLayer->getPositionX() / lel->m_objectLayer->getScale() * -1;
	float maxPosX = ErGui::constrainByLastObject ? lel->getLastObjectX() : std::max(lel->getLastObjectX(), 32470.f);
	ImGui::SetNextItemWidth(INPUT_ITEM_WIDTH * 4.f);
	if (ImGui::SliderFloat("##LevelPositionSliderX", &objectLayerX, -30.f, maxPosX))
		lel->m_objectLayer->setPositionX(objectLayerX * -1 * lel->m_objectLayer->getScale());

	ImGui::SameLine();
	ImGui::Checkbox("##Constrain-By-Last-Object", &ErGui::constrainByLastObject);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Constrain By Last Object");

	ImGui::SameLine();
	ImGui::SameLine();
	short step = 1;
	short fastStep = 5;
	ImGui::SetNextItemWidth(INPUT_ITEM_WIDTH);
	if (ImGui::InputScalar("Layer", ImGuiDataType_S16, &lel->m_currentLayer, &step, &fastStep)) {
		if (lel->m_currentLayer < -1) 
			lel->m_currentLayer = -1;
	}

	ImGui::SameLine();
	if (ImGui::Button("Level Settings")) {
		lel->m_editorUI->onSettings(nullptr);
	}

	ImGui::SameLine();
	if (ImGui::Button("Save Game Viewport Screenshot")) {
		ScreenRenderer::getCCRenderTexture()->saveToFile("viewport.png");
	}

	ImVec2 gameWinSize = ImGui::GetContentRegionAvail();
	ImVec2 cursorStart = ImGui::GetCursorPos();

	ImVec2 drawSize;
	ImVec2 drawOffset;
	if (gameWinSize.x > gameWinSize.y * textureRatio) {
		drawSize.y = gameWinSize.y;
		drawSize.x = gameWinSize.y * textureRatio;

		drawOffset.y = 0;
		drawOffset.x = (gameWinSize.x - drawSize.x) / 2;
	}
	else {
		drawSize.x = gameWinSize.x;
		drawSize.y = gameWinSize.x / textureRatio;

		drawOffset.x = 0;
		drawOffset.y = (gameWinSize.y - drawSize.y) / 2;
	}



	ImGui::SetCursorPos(ImVec2(cursorStart.x + drawOffset.x, cursorStart.y + drawOffset.y));
	ImGui::Image(renderedScreen.tex, drawSize, ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::IsItemHovered()) {
		ErGui::isGameWindowHovered = true;
		auto mouse = ImGui::GetMousePos();
		ImVec2 windowStart = ImGui::GetItemRectMin();
		ImVec2 windowMouseCoordinates = ImVec2(mouse.x - windowStart.x, mouse.y - windowStart.y);
		auto winSize = CCDirector::sharedDirector()->getWinSize();
		ErGui::gameWindowTouchCoordinatesConvertedToWorld = CCPoint(windowMouseCoordinates.x / drawSize.x * winSize.width, windowMouseCoordinates.y / drawSize.y * winSize.height);
		ErGui::gameWindowTouchCoordinatesConvertedToWorldForZoom = CCPoint(gameWindowTouchCoordinatesConvertedToWorld.x, winSize.height - gameWindowTouchCoordinatesConvertedToWorld.y);
	}
	else {
		ErGui::isGameWindowHovered = false;
	}

	//Vertical slider lol
	//ImGui::SameLine();
	//float objectLayerY = lel->m_objectLayer->getPositionY() / lel->m_objectLayer->getScale() * -1;
	//if (ImGui::VSliderFloat("##LevelPositionSliderY", ImVec2(20.f, drawSize.y), &objectLayerY, 0.f, 29800.f))
	//	lel->m_objectLayer->setPositionY(objectLayerY * -1 * lel->m_objectLayer->getScale());
	

	ImGui::End();
}