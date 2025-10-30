#pragma once
#include "myUtils.hpp"
#include "../modules/ToolsModule.hpp"
#include <Geode/modify/GameObject.hpp>
#include "SelectFilterModule.hpp"

namespace ErGui {
	bool isPointInPolygon(const cocos2d::CCPoint& pt, const std::vector<cocos2d::CCPoint>& polygon) {
		bool inside = false;
		int count = static_cast<int>(polygon.size());
		for (int i = 0, j = count - 1; i < count; j = i++)
		{
			const cocos2d::CCPoint& pi = polygon[i];
			const cocos2d::CCPoint& pj = polygon[j];

			// Проверяем пересечение луча, идущего вправо от pt, со стороной многоугольника
			if (((pi.y > pt.y) != (pj.y > pt.y)) &&
				(pt.x < (pj.x - pi.x) * (pt.y - pi.y) / (pj.y - pi.y) + pi.x))
			{
				inside = !inside;
			}
		}
		return inside;
	}

	void setMaxMin(int& value, int max, int min) {
		if (value > max) value = max;
		else if (value < min) value = min;
	}

	void setMin(int& value, int min) {
		if (value < min) value = min;
	}

	// To Do: Отказаться от этой функции.
	void drawSpriteInImGui(CCSprite* tempSprite) {
		auto quad = tempSprite->getQuad();

		float u_min = std::min({ quad.bl.texCoords.u, quad.br.texCoords.u, quad.tl.texCoords.u, quad.tr.texCoords.u });
		float u_max = std::max({ quad.bl.texCoords.u, quad.br.texCoords.u, quad.tl.texCoords.u, quad.tr.texCoords.u });
		float v_min = std::min({ quad.bl.texCoords.v, quad.br.texCoords.v, quad.tl.texCoords.v, quad.tr.texCoords.v });
		float v_max = std::max({ quad.bl.texCoords.v, quad.br.texCoords.v, quad.tl.texCoords.v, quad.tr.texCoords.v });

		auto texture = tempSprite->getTexture();
		GLuint textureID = texture->getName();
		ImTextureID imguiTexture = (ImTextureID)(intptr_t)textureID;


		auto drawList = ImGui::GetWindowDrawList();

		if (tempSprite->getContentWidth() > tempSprite->getContentHeight()) {
			float ratio = tempSprite->getContentWidth() / (ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x);
			float newY = tempSprite->getContentHeight() / ratio;
			float centerY = (ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y) / 2.f;

			drawList->AddImageQuad(
				textureID,
				ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y + (newY / 2) - centerY), ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y + (newY / 2) - centerY),
				ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y - (newY / 2) - centerY), ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y - (newY / 2) - centerY),
				ImVec2(quad.bl.texCoords.u, quad.bl.texCoords.v), ImVec2(quad.br.texCoords.u, quad.br.texCoords.v),
				ImVec2(quad.tr.texCoords.u, quad.tr.texCoords.v), ImVec2(quad.tl.texCoords.u, quad.tl.texCoords.v)
			);
		}
		else {
			float ratio = tempSprite->getContentHeight() / (ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y);
			float newX = tempSprite->getContentWidth() / ratio;
			float centerX = (ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x) / 2.f;

			drawList->AddImageQuad(
				textureID,
				ImVec2(ImGui::GetItemRectMin().x - (newX / 2) + centerX, ImGui::GetItemRectMax().y), ImVec2(ImGui::GetItemRectMin().x + (newX / 2) + centerX, ImGui::GetItemRectMax().y),
				ImVec2(ImGui::GetItemRectMin().x + (newX / 2) + centerX, ImGui::GetItemRectMin().y), ImVec2(ImGui::GetItemRectMin().x - (newX / 2) + centerX, ImGui::GetItemRectMin().y),
				ImVec2(quad.bl.texCoords.u, quad.bl.texCoords.v), ImVec2(quad.br.texCoords.u, quad.br.texCoords.v),
				ImVec2(quad.tr.texCoords.u, quad.tr.texCoords.v), ImVec2(quad.tl.texCoords.u, quad.tl.texCoords.v)
			);
		}
	}

	void drawFrameInImGui(CCSpriteFrame* frame) {
		CCSprite* tempSprite = CCSprite::createWithSpriteFrame(frame);
		drawSpriteInImGui(tempSprite);
	}
	
	void initImGuiStyling() {
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

		style.WindowPadding = ImVec2(8.f, 8.f);
		style.WindowRounding = 4.f;
		style.WindowBorderSize = 0.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ChildRounding = 4.f;
		style.ChildBorderSize = 1.f;
		style.PopupRounding = 4.f;
		style.PopupBorderSize = 1.f;
		style.FramePadding = ImVec2(6.f, 3.f);
		style.FrameRounding = 2.f;
		style.FrameBorderSize = 0.f;
		style.ItemSpacing = ImVec2(10.f, 4.f);
		style.ItemInnerSpacing = ImVec2(4.f, 4.f);
		style.CellPadding = ImVec2(4.f, 2.f);
		style.TouchExtraPadding = ImVec2(0.f, 0.f);
		style.IndentSpacing = 24.f;
		style.ScrollbarSize = 14.f;
		style.ScrollbarRounding = 6.f;
		style.GrabMinSize = 12.f;
		style.GrabRounding = 4.f;
		style.LogSliderDeadzone = 4.f;
		style.TabRounding = 4.f;
		style.TabBorderSize = 0.f;
		style.TabBarBorderSize = 2.f;
		style.TabBarOverlineSize = 0.f;
		style.TableAngledHeadersAngle = 35.f;
		style.TableAngledHeadersTextAlign = ImVec2(0.5f, 0.f);
		style.ColorButtonPosition = ImGuiDir_Left;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.5f, 0.65f);
		style.SeparatorTextBorderSize = 4.f;
		style.SeparatorTextAlign = ImVec2(0.f, 0.f);
		style.SeparatorTextPadding = ImVec2(40.f, 2.f);
		style.DockingSeparatorSize = 4.f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.79f, 0.79f, 0.79f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 0.71f, 0.28f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.84f, 0.52f, 0.04f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.71f, 0.28f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.84f, 0.56f, 0.14f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.71f, 0.28f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.78f, 0.78f, 0.78f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.85f, 0.54f, 0.07f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.75f, 0.75f, 0.75f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.85f, 0.54f, 0.07f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.50f, 0.08f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.65f, 0.11f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.65f, 0.11f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.65f, 0.11f, 0.95f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.97f, 0.68f, 0.25f, 0.80f);
		colors[ImGuiCol_Tab] = ImVec4(0.53f, 0.53f, 0.53f, 0.86f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.82f, 0.54f, 0.11f, 1.00f);
		colors[ImGuiCol_TabSelectedOverline] = ImVec4(1.00f, 0.65f, 0.11f, 0.00f);
		colors[ImGuiCol_TabDimmed] = ImVec4(0.20f, 0.20f, 0.20f, 0.97f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
		colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(1.00f, 0.65f, 0.11f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.f, 0.f, 0.f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextLink] = ImVec4(1.00f, 0.65f, 0.11f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.65f, 0.11f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavCursor] = ImVec4(1.00f, 0.65f, 0.11f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

	// Change to advanced undo/redo later.
	void addObjectToUndoList(GameObject* obj, UndoCommand command) {
		GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, command));
		GameManager::sharedState()->getEditorLayer()->m_redoObjects->removeAllObjects();
	}

	void addObjectsToUndoList(CCArray* objArr, UndoCommand command) {
		CCArray* copyObjArr = CCArray::create();
		for (const auto& obj : CCArrayExt<GameObject*>(objArr)) {
			auto objCopy = GameObjectCopy::create(obj);
			copyObjArr->addObject(objCopy);
			//objCopy->resetObject();
		}
		GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::createWithArray(copyObjArr, command));
		GameManager::sharedState()->getEditorLayer()->m_redoObjects->removeAllObjects();
	}

	bool compareCCArrays(CCArray* arr1, CCArray* arr2) {
		if (!arr1 || !arr2) return false;
		if (arr1->count() != arr2->count()) return false;
		for (int i = 0; i < arr1->count(); i++) {
			if (!arr1->objectAtIndex(i)->isEqual(arr2->objectAtIndex(i))) 
				return false;
		}
		return true;
	}

	void selectAllObjects() {
		auto editorUI = EditorUI::get();
		if (editorUI) {
			CCArray* objsInCurrentLayer = CCArray::create();
			for (auto obj : CCArrayExt<GameObject*>(editorUI->m_editorLayer->m_objects)) {
				if (obj->m_editorLayer == editorUI->m_editorLayer->m_currentLayer || (obj->m_editorLayer2 == editorUI->m_editorLayer->m_currentLayer && obj->m_editorLayer2 != 0) || editorUI->m_editorLayer->m_currentLayer == -1) {
					objsInCurrentLayer->addObject(obj);
				}
			}
			editorUI->processSelectObjects(objsInCurrentLayer);
			editorUI->updateButtons();
			//EditorUI::deactivateRotationControl();
			editorUI->deactivateScaleControl();
			editorUI->deactivateTransformControl();
			editorUI->updateObjectInfoLabel();
		}
	}

	void selectAllObjectWithDirection(bool rightDir) {
		auto editorUI = EditorUI::get();
		if (editorUI) {
			auto cameraPos = editorUI->m_editorLayer->m_objectLayer->getPosition();
			auto cameraScale = editorUI->m_editorLayer->m_objectLayer->getScale();
			int centerX = (CCDirector::sharedDirector()->getWinSize().width / 2 - cameraPos.x) / cameraScale;

			CCArray* objsInCurrentLayer = CCArray::create();
			for (auto obj : CCArrayExt<GameObject*>(editorUI->m_editorLayer->m_objects)) {
				if (obj->m_editorLayer == editorUI->m_editorLayer->m_currentLayer || (obj->m_editorLayer2 == editorUI->m_editorLayer->m_currentLayer && obj->m_editorLayer2 != 0) || editorUI->m_editorLayer->m_currentLayer == -1) {
					if ((rightDir && obj->getPositionX() >= centerX) || 
						(!rightDir && obj->getPositionX() <= centerX))
					objsInCurrentLayer->addObject(obj);
				}
			}
			editorUI->processSelectObjects(objsInCurrentLayer);
			editorUI->updateButtons();
			//EditorUI::deactivateRotationControl();
			editorUI->deactivateScaleControl();
			editorUI->deactivateTransformControl();
			editorUI->updateObjectInfoLabel();
		}
	}

	void nextFreeLayer() {
		std::set<int> layersSet;
		auto lel = LevelEditorLayer::get();
		for (auto obj : CCArrayExt<GameObject*>(lel->m_objects)) {
			if (obj->m_editorLayer >= 0)
				layersSet.insert(obj->m_editorLayer);
			if (obj->m_editorLayer2 > 0)
				layersSet.insert(obj->m_editorLayer2);
		}

		int result = *std::prev(layersSet.end()) + 1;
		for (int i = 0; i < *std::prev(layersSet.end()); i++) {
			if (!layersSet.contains(i)) {
				result = i;
				break;
			}
		}

		lel->m_currentLayer = result;
	}

	CCRect normalizeRect(CCRect rect) {
		CCRect ret(rect.origin, rect.size);

		if (ret.size.height < 0) {
			ret.origin.y += ret.size.height;
			ret.size.height = -ret.size.height;
		}

		if (ret.size.width < 0) {
			ret.origin.x += ret.size.width;
			ret.size.width = -ret.size.width;
		}

		return ret;
	}


	// Alphalaneous func, written by prevter. Thank you guys!
	void forEachObject(GJBaseGameLayer const* game, std::function<void(GameObject*)> const& callback) {
		int count = game->m_sections.empty() ? -1 : game->m_sections.size();
		for (int i = game->m_leftSectionIndex; i <= game->m_rightSectionIndex && i < count; ++i) {
			auto leftSection = game->m_sections[i];
			if (!leftSection) continue;

			auto leftSectionSize = leftSection->size();
			for (int j = game->m_bottomSectionIndex; j <= game->m_topSectionIndex && j < leftSectionSize; ++j) {
				auto section = leftSection->at(j);
				if (!section) continue;

				auto sectionSize = game->m_sectionSizes[i]->at(j);
				for (int k = 0; k < sectionSize; ++k) {
					auto obj = section->at(k);
					if (!obj) continue;

					callback(obj);
				}
			}
		}
	}


	std::array<CCPoint, 4> getCornersFromRotatedRect(CCPoint center, CCSize size, float rot) {
		float cosR = cosf(rot);
		float sinR = sinf(rot);
		float hw = size.width / 2;
		float hh = size.height / 2;

		std::array<CCPoint, 4> corners = {
			CCPoint(hw,  hh),
			CCPoint(hw, -hh),
			CCPoint(-hw, -hh),
			CCPoint(-hw,  hh)
		};

		for (auto& p : corners) {
			float x = p.x;
			float y = p.y;
			p.x = center.x + x * cosR - y * sinR;
			p.y = center.y + x * sinR + y * cosR;
		}
		return corners;
	}

	CCPoint getAxis(const CCPoint& p1, const CCPoint& p2) {
		CCPoint edge = ccpSub(p2, p1);
		float len = sqrtf(edge.x * edge.x + edge.y * edge.y);
		return CCPoint(edge.x / len, edge.y / len);
	}

	void rectProjection(const std::array<CCPoint, 4>& corners, const CCPoint& axis, float& min, float& max) {
		min = max = corners[0].x * axis.x + corners[0].y * axis.y;
		for (int i = 1; i < 4; ++i) {
			float proj = corners[i].x * axis.x + corners[i].y * axis.y;
			if (proj < min) min = proj;
			if (proj > max) max = proj;
		}
	}

	bool overlap(float minA, float maxA, float minB, float maxB) {
		return !(maxA < minB || maxB < minA);
	}

	bool checkOBBIntersection(CCPoint posA, CCSize sizeA, float rotA,
		CCPoint posB, CCSize sizeB, float rotB) {
		auto cornersA = getCornersFromRotatedRect(posA, sizeA, rotA);
		auto cornersB = getCornersFromRotatedRect(posB, sizeB, rotB);

		CCPoint axes[4] = {
			getAxis(cornersA[0], cornersA[1]),
			getAxis(cornersA[1], cornersA[2]),
			getAxis(cornersB[0], cornersB[1]),
			getAxis(cornersB[1], cornersB[2]),
		};

		for (auto& axis : axes) {
			float minA, maxA, minB, maxB;
			rectProjection(cornersA, axis, minA, maxA);
			rectProjection(cornersB, axis, minB, maxB);

			if (!overlap(minA, maxA, minB, maxB))
				return false;
		}

		return true;
	}

	CCRect getObjectHitbox(GameObject* obj) {
		auto editorUI = EditorUI::get();
		if (!editorUI) return CCRect();
		if (!obj || !obj->getParent()) return CCRect();
		auto objLayer = editorUI->m_editorLayer->m_objectLayer;
		auto cameraPos = objLayer->getPosition();
		auto cameraScale = objLayer->getScale();

		auto box = obj->boundingBox();
		auto centerPoint = obj->getParent()->convertToWorldSpace({
			box.getMidX(),
			box.getMidY()
			});

		auto contentSize = obj->getScaledContentSize();

		if (auto txtObj = typeinfo_cast<TextGameObject*>(obj)) {
			auto txtChildren = txtObj->getChildren();
			if (txtChildren && txtChildren->count() > 0) {
				auto firstLetter = static_cast<CCFontSprite*>(txtChildren->firstObject());
				const auto firstBox = firstLetter->boundingBox();

				auto lastLetter = static_cast<CCFontSprite*>(txtChildren->lastObject());
				const auto lastBox = lastLetter->boundingBox();

				auto firstCenterPoint = firstLetter->getParent()->convertToWorldSpace({
					firstBox.getMidX(),
					firstBox.getMidY()
					});

				auto lastCenterPoint = lastLetter->getParent()->convertToWorldSpace({
					lastBox.getMidX(),
					lastBox.getMidY()
					});

				centerPoint = CCPoint(
					(firstCenterPoint.x + lastCenterPoint.x) / 2.f,
					(firstCenterPoint.y + lastCenterPoint.y) / 2.f
				);

				auto firstPosLocal = firstLetter->getPosition();
				auto lastPosLocal = lastLetter->getPosition();

				float dx = (lastPosLocal.x - firstPosLocal.x) * obj->m_scaleX;
				float dy = (lastPosLocal.y - firstPosLocal.y) * obj->m_scaleY;
				float centerDistLocal = sqrtf(dx * dx + dy * dy);

				auto firstSize = firstLetter->getScaledContentSize() * CCSize(obj->m_scaleX, obj->m_scaleY);
				auto lastSize = lastLetter->getScaledContentSize() * CCSize(obj->m_scaleX, obj->m_scaleY);

				float widthLocal = centerDistLocal + (firstSize.width + lastSize.width) * 0.5f;
				float heightLocal = std::max(firstSize.height, lastSize.height);

				contentSize = CCSize(widthLocal, heightLocal);

			}
		}
		contentSize *= cameraScale;

		return CCRect(centerPoint, contentSize);
	}

	bool isHitboxAtPoint(const CCPoint& touch, const CCRect& hitbox) {

		auto center = hitbox.origin;
		auto size = hitbox.size;
		
		float xMin = center.x - size.width / 2.f;
		float xMax = xMin + size.width;

		float yMin = center.y - size.height / 2.f;
		float yMax = yMin + size.height;

		if (touch.x >= xMin && touch.x <= xMax &&
			touch.y >= yMin && touch.y <= yMax)
			return true;

		return false;
	}

	bool isObjectGonnaBeSelected(GameObject* obj) {
		
		if (!obj || !obj->getParent())
			return false;

		auto hitbox = getObjectHitbox(obj);

		CCPoint selectRectCenter = {
			ErGui::selectRect.origin.x + ErGui::selectRect.size.width / 2,
			ErGui::selectRect.origin.y + ErGui::selectRect.size.height / 2
		};

		auto isIntersecting = ErGui::checkOBBIntersection(hitbox.origin, hitbox.size, -CC_DEGREES_TO_RADIANS(obj->getRotation()), selectRectCenter, ErGui::selectRect.size, 0.f);

		auto currentLayer = LevelEditorLayer::get()->m_currentLayer;
		if (isIntersecting &&
			(obj->m_editorLayer == currentLayer || (obj->m_editorLayer2 == currentLayer && obj->m_editorLayer2 != 0) || currentLayer == -1)
			&& ErGui::selectFilterRealization(obj)) {
			return true;
		}
		else {
			return false;
		}
	}

	void selectEveryObjectInSquare(GameObject* obj) {
		if (isObjectGonnaBeSelected(obj)) {
			auto editorUI = EditorUI::get();
			auto selectedObject = editorUI->m_selectedObject;
			auto selectedObjects = editorUI->m_selectedObjects;
			
			if (!selectedObject && !selectedObjects && !selectedObjects->count()) {
				editorUI->selectObject(obj, false);
			}
			else if (!selectedObjects) {
				CCArray* newArr = CCArray::create();
				obj->selectObject({ 0, 255, 0 });
				obj->m_isSelected = true;
				editorUI->m_selectedObjects = newArr;
				editorUI->m_selectedObjects->addObject(obj);
				editorUI->m_selectedObject = nullptr;

				if (obj->m_linkedGroup) {
					auto objArr = editorUI->m_editorLayer->getStickyGroup(obj->m_linkedGroup);
					editorUI->selectObjects(objArr, false);
				}
			}
			else {
				obj->selectObject({ 0, 255, 0 });
				obj->m_isSelected = true;
				editorUI->m_selectedObjects->addObject(obj);
				editorUI->m_selectedObject = nullptr;

				if (obj->m_linkedGroup) {
					auto objArr = editorUI->m_editorLayer->getStickyGroup(obj->m_linkedGroup);
					editorUI->selectObjects(objArr, false);
				}
			}
		}
	}
}