#include "Geode/modify/CCEGLView.hpp"
#include "GameWindowModule.hpp"
#include "razoomUtils.hpp"
#include "IconsMaterialDesignIcons.h"
#include <fmt/format.h>


namespace ErGui {
	bool isGameWindowHovered = false;
	CCPoint gameWindowTouchCoordinatesConvertedToWorld;
	CCPoint gameWindowTouchCoordinatesConvertedToWorldForZoom;
	static bool constrainByLastObjectX = false;
	static bool constrainByLastObjectY = false;
	static GLuint gameFrame;
}

struct RenderedTexture {
	ImTextureID tex;
	CCSize size;
};


void updateMousePos(ImVec2 drawSize) {
	auto mouse = ImGui::GetMousePos();
	ImVec2 windowStart = ImGui::GetItemRectMin();
	ImVec2 windowMouseCoordinates = ImVec2(mouse.x - windowStart.x, mouse.y - windowStart.y);
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	ErGui::gameWindowTouchCoordinatesConvertedToWorld = CCPoint(windowMouseCoordinates.x / drawSize.x * winSize.width, windowMouseCoordinates.y / drawSize.y * winSize.height);
	ErGui::gameWindowTouchCoordinatesConvertedToWorldForZoom = CCPoint(ErGui::gameWindowTouchCoordinatesConvertedToWorld.x, winSize.height - ErGui::gameWindowTouchCoordinatesConvertedToWorld.y);
}

static GLuint captureScreenToGLTexture() {
	static GLuint captureTexId = 0;
	static int lastW = 0, lastH = 0;

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	int screenW = vp[2];
	int screenH = vp[3];

	if (!captureTexId || lastW != screenW || lastH != screenH) {
		if (captureTexId) {
			glDeleteTextures(1, &captureTexId);
			captureTexId = 0;
		}
		glGenTextures(1, &captureTexId);
		glBindTexture(GL_TEXTURE_2D, captureTexId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			screenW, screenH,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			nullptr
		);
		glBindTexture(GL_TEXTURE_2D, 0);

		lastW = screenW;
		lastH = screenH;
	}

	glBindTexture(GL_TEXTURE_2D, captureTexId);
	glCopyTexSubImage2D(
		GL_TEXTURE_2D, 0,
		0, 0,
		0, 0,
		screenW, screenH
	);

	glBindTexture(GL_TEXTURE_2D, 0);

	return captureTexId;
}

// Capturing the game frame from CCEGLView
class $modify(CCEGLView) {
	static void onModify(auto & self) {
		if (!self.setHookPriorityPre("cocos2d::CCEGLView::swapBuffers", Priority::Early)) {
			geode::log::warn("Failed to set hook priority for swapBuffers");
		}
	}
	void swapBuffers() {
		ErGui::gameFrame = captureScreenToGLTexture();
		CCEGLView::swapBuffers();
	}
};


void ErGui::renderGameWindow() {

	auto renderedScreen = RenderedTexture((ImTextureID)(intptr_t)ErGui::gameFrame, CCDirector::sharedDirector()->getWinSize());
	auto textureRatio = renderedScreen.size.width / renderedScreen.size.height;
	auto levelName = LevelEditorLayer::get()->m_level->m_levelName;

	ImGui::Begin(fmt::format("{}###Game", levelName).c_str(), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	float objectLayerX = lel->m_objectLayer->getPositionX() / lel->m_objectLayer->getScale() * -1;
	float maxPosX = ErGui::constrainByLastObjectX ? getLastObjectXFast() : std::max(getLastObjectXFast(), 32470.f);

	float maxWidth = INPUT_ITEM_WIDTH * 3.5f;
	float minWidth = INPUT_ITEM_WIDTH * 0.5f;

	float avail = ImGui::GetContentRegionAvail().x - maxWidth * 0.55f;
	float sliderWidth = (avail > maxWidth) ? maxWidth : avail;
	if (sliderWidth < minWidth)
		sliderWidth = minWidth;

	ImGui::SetNextItemWidth(sliderWidth);
	if (ImGui::SliderFloat("##LevelPositionSliderX", &objectLayerX, -30.f, maxPosX))
		lel->m_objectLayer->setPositionX(objectLayerX * -1 * lel->m_objectLayer->getScale());

	ImGui::SameLine();
	ImGui::Checkbox("##Constrain-By-Last-ObjectX", &ErGui::constrainByLastObjectX);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Constrain By Last Object X");

	ImGui::SameLine();
	ImGui::SameLine();
	short step = 1;
	short fastStep = 5;
	ImGui::SetNextItemWidth(INPUT_ITEM_WIDTH * 0.66f);
	if (ImGui::InputScalar("##Layer", ImGuiDataType_S16, &lel->m_currentLayer, &step, &fastStep)) {
		if (lel->m_currentLayer < -1) 
			lel->m_currentLayer = -1;
	}
	ImGui::SameLine();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(-1, 5));
	if (ImGui::Button(ICON_MDI_MENU_LEFT_OUTLINE, ImVec2(21, 21))) {
		lel->m_currentLayer = -1;
	}
	ImGui::SetItemTooltip("All Layer");
	ImGui::SameLine();
	if (ImGui::Button(ICON_MDI_MENU_RIGHT_OUTLINE, ImVec2(21, 21))) {
		ErGui::nextFreeLayer();
	}
	ImGui::SetItemTooltip("Next Free Layer");
	ImGui::PopStyleVar();
	
	ImGui::SameLine();
	ImGui::Text("Layer");


	// Vertical slider
	//ImGui::SameLine();
	float objectLayerY = lel->m_objectLayer->getPositionY() / lel->m_objectLayer->getScale() * -1;
	float maxPosY = ErGui::constrainByLastObjectY ? getLastObjectYFast() : std::max(getLastObjectYFast(), 29800.f);

	ImGui::SetNextItemWidth(sliderWidth);
	if (ImGui::SliderFloat("##LevelPositionSliderY", &objectLayerY, 0.f, maxPosY))
		lel->m_objectLayer->setPositionY(objectLayerY * -1 * lel->m_objectLayer->getScale());

	ImGui::SameLine();
	ImGui::Checkbox("##Constrain-By-Last-ObjectY", &ErGui::constrainByLastObjectY);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		ImGui::SetTooltip("Constrain By Last Object Y");

	ImVec2 drawSize;
	if (Mod::get()->getSavedValue<bool>("gamewindow-static-ratio", false)) {
		ImVec2 gameWinSize = ImGui::GetContentRegionAvail();
		ImVec2 cursorStart = ImGui::GetCursorPos();

		
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
	}
	// --- AI cause lazy
	else {
		ImVec2 winPos = ImGui::GetWindowPos();
		ImVec2 stylePad = ImGui::GetStyle().WindowPadding;

		// local content region in window-local coords
		ImVec2 contentMinLocal = ImGui::GetWindowContentRegionMin(); // typically equals {WindowPadding.x, WindowPadding.y}
		ImVec2 contentMaxLocal = ImGui::GetWindowContentRegionMax();

		// convert to screen coords (absolute)
		ImVec2 absContentMin = ImVec2(winPos.x + contentMinLocal.x, winPos.y + contentMinLocal.y);
		ImVec2 absContentMax = ImVec2(winPos.x + contentMaxLocal.x, winPos.y + contentMaxLocal.y);

		// top boundary: cursor is located just *after* the toolbar/widgets you've already drawn
		ImVec2 cursorScreen = ImGui::GetCursorScreenPos(); // screen coords (x,y) where next item would be placed

		// build available rect for the image:
		// - left/right are based on content region (we will subtract side padding manually)
		// - top is current cursor Y (so image won't overlap toolbar above)
		// - bottom is content bottom (absContentMax.y)
		ImVec2 availMin = absContentMin;
		availMin.y = cursorScreen.y;              // start below toolbar
		// remove left padding (push image to window left edge of content region minus padding)
		availMin.x -= stylePad.x;

		ImVec2 availMax = absContentMax;
		// expand to remove right/bottom padding (so image reaches edges)
		availMax.x += stylePad.x;
		availMax.y += stylePad.y;

		ImVec2 availSize = ImVec2(availMax.x - availMin.x, availMax.y - availMin.y);
		if (availSize.x <= 0.0f || availSize.y <= 0.0f) {
			// nothing to draw or invalid region
			ImGui::Dummy(ImVec2(0, 0));
			return;
		}

		// --- cover scaling: minimal draw size that still covers avail area, preserving aspect ratio
		float texRatio = textureRatio; // should equal (texWidth / texHeight)
		float drawW = std::max(availSize.x, availSize.y * texRatio);
		float drawH = drawW / texRatio;

		// center the image inside available rect
		ImVec2 drawPos = ImVec2(availMin.x + (availSize.x - drawW) * 0.5f,
			availMin.y + (availSize.y - drawH) * 0.5f);
		drawSize = ImVec2(drawW, drawH);

		// reserve layout space (so following widgets are placed below the image)
		ImGui::Dummy(availSize);

		// clip to available rect (so image is not visible outside desired area)
		ImGui::PushClipRect(availMin, availMax, true);
		// draw image using absolute screen coords
		ImGui::SetCursorScreenPos(drawPos);
		ImGui::Image(renderedScreen.tex, drawSize, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::PopClipRect();
	}
	// --- AI end


	if (ImGui::IsItemHovered()) {
		ErGui::isGameWindowHovered = true;
		updateMousePos(drawSize);
	}
	else {
		ErGui::isGameWindowHovered = false;
		if (ErGui::isGameWindowTouching) {
			updateMousePos(drawSize);
		}
	}

	ImGui::End();
}
