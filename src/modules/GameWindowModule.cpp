#include "GameWindowModule.hpp"
#include "../helpers/ScreenRenderer.h"

namespace ErGui {
	bool isGameWindowHovered = false;
	CCPoint gameWindowTouchCoordinatesConvertedToWorld;
}

void ErGui::renderGameWindow() {

	auto renderedScreen = ScreenRenderer::render();
	auto textureRatio = renderedScreen.size.width / renderedScreen.size.height;

	ImGui::Begin("Game");
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
	}
	else {
		ErGui::isGameWindowHovered = false;
	}

}