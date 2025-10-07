#include "Geode/modify/CCEGLView.hpp"
#include "GameWindowModule.hpp"
#include "razoomUtils.hpp"


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

// Íàïèñàíî íåéðîíêîé, ò.ê. ÿ ïðîñòî óñòàíó ðàçáèðàòüñÿ â ýòîì ñåé÷àñ. Îñíîâà îò DevTools'îâ.
static GLuint captureScreenToGLTexture() {
	static GLuint captureTexId = 0;    // õðàíèò ID òåêñòóðû, êóäà ìû êîïèðóåì êàäð
	static int lastW = 0, lastH = 0;   // ÷òîáû ïåðåñîçäàâàòü òåêñòóðó, åñëè ðàçìåð èçìåíèëñÿ

	// 1) Óçíà¸ì, ñêîëüêî ïèêñåëåé ðåàëüíî ñåé÷àñ íà ýêðàíå (viewport):
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	int screenW = vp[2];
	int screenH = vp[3];

	// 2) Åñëè ðàçìåð èçìåíèëñÿ (íàïðèìåð, èãðîê ïîìåíÿë îêíî), ïåðåñîçäà¸ì òåêñòóðó:
	if (!captureTexId || lastW != screenW || lastH != screenH) {
		if (captureTexId) {
			glDeleteTextures(1, &captureTexId);
			captureTexId = 0;
		}
		glGenTextures(1, &captureTexId);
		glBindTexture(GL_TEXTURE_2D, captureTexId);

		// Íàñòðàèâàåì ôèëüòðàöèþ — îáû÷íî LINEAR, ÷òîáû ïðåâüþ ïðè ìàñøòàáèðîâàíèè áûëî áûñòðåå «ðàçìûòî»
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Ðåçåðâèðóåì ïàìÿòü ïîä áóäóùóþ êîïèþ ýêðàíà
		glTexImage2D(
			GL_TEXTURE_2D,		// òàðãåò
			0,					// LOD
			GL_RGBA,			// âíóòðåííèé ôîðìàò (RGBA8)
			screenW, screenH,	// øèðèíà, âûñîòà
			0,					 // border
			GL_RGBA,			// ôîðìàò ïîñòóïàþùèõ äàííûõ
			GL_UNSIGNED_BYTE,	// òèï äàííûõ
			nullptr				// ïîêà íåò äàííûõ (áóäåò çàïîëíåíî glCopyTexSubImage2D)
		);
		glBindTexture(GL_TEXTURE_2D, 0);

		lastW = screenW;
		lastH = screenH;
	}

	// 3) Ïðèâÿçûâàåì íàøó òåêñòóðó è êîïèðóåì òåêóùèé back-buffer (FBO = 0) â íå¸:
	glBindTexture(GL_TEXTURE_2D, captureTexId);

	// Êîïèðóåì öåëèêîì ýêðàí (âíèç/âëåâî îò 0,0) â òåêñòóðó, íà÷èíàÿ ñ 0,0
	glCopyTexSubImage2D(
		GL_TEXTURE_2D, 0,    // êîòîðûé óðîâåíü mipmap (0) è òàðãåò
		0, 0,                // ñìåùåíèå (xOffset, yOffset) â òåêñòóðå
		0, 0,                // êàêàÿ òî÷êà back-buffer áåð¸òñÿ (x, y)
		screenW, screenH     // ñêîëüêî ïèêñåëåé êîïèðîâàòü
	);

	// 4) Áèíä ñíîâà î÷èùàåì (íå îáÿçàòåëüíî, íî ïðèíÿòî)
	glBindTexture(GL_TEXTURE_2D, 0);

	// 5) Âîçâðàùàåì ID òåêñòóðû, êóäà ìû ñêîïèðîâàëè ýêðàí
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
	
	ImGui::Begin("Game", 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	float objectLayerX = lel->m_objectLayer->getPositionX() / lel->m_objectLayer->getScale() * -1;
	float maxPosX = ErGui::constrainByLastObjectX ? getLastObjectXFast() : std::max(getLastObjectXFast(), 32470.f);

	float maxWidth = INPUT_ITEM_WIDTH * 3.5f; // ïðåäåë øèðèíû ñëàéäåðà
	float minWidth = INPUT_ITEM_WIDTH * 0.5f; // ïðåäåë øèðèíû ñëàéäåðà
	// Áåð¸ì äîñòóïíóþ øèðèíó è îãðàíè÷èâàåì å¸ maxWidth
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
	if (ImGui::Button("All")) {
		lel->m_currentLayer = -1;
	}
	ImGui::SameLine();
	if (ImGui::Button("NF")) {
		ErGui::nextFreeLayer();
	}

	ImGui::SetItemTooltip("Next object free Editor Layer");
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

	// DEBUG - Ñîõðàíåíèå âüþïîðòà â ïíã, â êîðíå èãðû.
	//ImGui::SameLine();
	//if (ImGui::Button("Save Game Viewport Screenshot")) {
	//	ScreenRenderer::getCCRenderTexture()->saveToFile("viewport.png");
	//}

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
		if (ErGui::isGameWindowTouching) { // Íóæíî äëÿ âû÷èñëåíèÿ ïîçèöèè ìûøè, êîãäà îíà íå ïåðåêðûâàåò Image, íî åù¸ íå îòïóùåíî
			updateMousePos(drawSize);
		}
	}

	ImGui::End();
}
