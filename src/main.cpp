#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <imgui_internal.h>

#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>

#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/DrawGridLayer.hpp>

#include <IconsMaterialDesignIcons.h>
#include "modules/EditGroupModule.hpp"
#include "modules/SelectFilterModule.hpp"
#include "modules/TransformObjectModule.hpp"
#include "modules/FooterModule.hpp"
#include "modules/ToolsModule.hpp"
#include "modules/ObjectListModule.hpp"
#include "modules/LayerModule.hpp"
#include "modules/EditObjectModule.hpp"
#include "modules/ActionHistoryModule.hpp"
#include "modules/EditColorModule.hpp"
#include "modules/GameWindowModule.hpp"
#include "modules/GlobalDockingView.hpp"
#include "modules/SettingsModule.hpp"
#include "modules/ContextMenuModule.hpp"
#include "modules/EditorUtilsModule.hpp"

#include "includes/ObjectCategories.hpp"
#include <matjson.hpp>
#include <matjson/reflect.hpp>
#include <matjson/std.hpp>
#include <matjson/stl_serialize.hpp>

#include <Geode/Result.hpp>

#include "features/Hovering.hpp"

using namespace geode::prelude;

//bool filterSingleObj(bool filterBool, int objParameter, std::vector<int> vec) {
//	if (!filterBool) return true;
//	for (int i = 0; i < vec.size(); i++) {
//		if (objParameter == vec[i])
//			return true;
//	}
//	return false;
//}


void selectObjectsWithFilter(CCArray* objArr, bool p1) {
	CCArray* sfr = ErGui::selectFilterRealization(objArr);
	EditorUI::get()->selectObjects(sfr, p1);
}

void selectObjectWithFilter(GameObject* obj, bool p1) {
	if (ErGui::selectFilterRealization(obj)) {
		EditorUI::get()->selectObject(obj, p1);
	}
}

void manualObjectsSelect(CCArray* objsInShape) {
	int selectMode = Mod::get()->getSavedValue<int>("select-mode");

	switch (selectMode) {
		case 3: { // Intersective
			CCArray* objArr = CCArray::create();

			for (auto obj : CCArrayExt<GameObject*>(objsInShape)) {
				if (obj->m_isSelected) {
					objArr->addObject(obj);
				}
			}
			EditorUI::get()->deselectAll();
			if (objArr->count() > 0) {
				selectObjectsWithFilter(objArr, false);
			}
			break;
		}
		case 2: { // Subtractive
			CCArray* sfr = ErGui::selectFilterRealization(objsInShape);
			for (auto obj : CCArrayExt<GameObject*>(sfr)) {
				EditorUI::get()->deselectObject(obj);
			}
			if (EditorUI::get()->m_selectedObjects->count() == 1) {
				EditorUI::get()->m_selectedObject = static_cast<GameObject*>(EditorUI::get()->m_selectedObjects->objectAtIndex(0));
				EditorUI::get()->m_selectedObjects->removeAllObjects();
			}
			break;
		}
		case 1: // Additive
		default: {
			selectObjectsWithFilter(objsInShape, false);
			break;
		}
	}
}

//void manualObjectSelect(GameObject* objInShape) {
//	int selectMode = Mod::get()->getSavedValue<int>("select-mode");
//
//	switch (selectMode) {
//	case 3: { // Intersective
//		EditorUI::get()->deselectAll();
//		selectObjectWithFilter(objInShape, false);
//		break;
//	}
//	case 2: { // Subtractive
//		if (ErGui::selectFilterRealization(objInShape)) {
//			EditorUI::get()->deselectObject(objInShape);
//			if (EditorUI::get()->m_selectedObjects->count() == 1) {
//				EditorUI::get()->m_selectedObject = static_cast<GameObject*>(EditorUI::get()->m_selectedObjects->objectAtIndex(0));
//				EditorUI::get()->m_selectedObjects->removeAllObjects();
//			}
//		}
//		break;
//	}
//	case 1: // Additive
//	default: {
//		selectObjectWithFilter(objInShape, false);
//		break;
//	}
//	}
//}


void exitEditor() { // EditorUI is already destroyed here
	// Save Object Config
	auto cfgDir = Mod::get()->getSettingValue<std::filesystem::path>("object-list-config");
	matjson::Value j;
	for (auto key : ErGui::keyOrder) {
		j[key] = ErGui::objectCfg[key];
	}
	std::ofstream oCfgFile = std::ofstream(cfgDir);
	oCfgFile.write(j.dump().c_str(), j.dump().size());
	oCfgFile.close();

	// Nullify last Objects by X/Y
	ErGui::lastObjX = nullptr;
	ErGui::lastObjY = nullptr;
	ErGui::lastObjCountX = 0;
	ErGui::lastObjCountY = 0;

	// Clear Object List Cache
	ErGui::clearObjectListCache();
}

class $modify(EditorUI) {

	struct Fields {
		int nothing = 0; // trigger lazy m_fields initialization
		Fields() {}
		~Fields() {
			exitEditor();
		}
	};

	bool onCreate() {
		bool ret = EditorUI::onCreate();
		if (m_selectedObjectIndex) {
			ErGui::addRecentObjectToList(m_selectedObjectIndex);
		}
		return ret;
	}

	void clickOnPosition(CCPoint p0) {
		// dead zone fix
		float tmp = m_toolbarHeight;
		m_toolbarHeight = -999999;
		EditorUI::clickOnPosition(p0);
		m_toolbarHeight = tmp;
	}


	void reloadCustomItems() {
		EditorUI::reloadCustomItems();
		ErGui::reloadCustomObjects();
	}


	// Colored object building
	GameObject* createObject(int p0, CCPoint p1) {
		bool enableColorBuild1 = Mod::get()->getSavedValue<bool>("enable-build-color-1");
		bool enableColorBuild2 = Mod::get()->getSavedValue<bool>("enable-build-color-2");

		int color1 = Mod::get()->template getSavedValue<int>("build-color-1");
		int color2 = Mod::get()->template getSavedValue<int>("build-color-2");
		auto obj = EditorUI::createObject(p0, p1);
		if (obj->m_baseColor && enableColorBuild1) {
			obj->m_baseColor->m_colorID = color1;
		}
		if (obj->m_detailColor && enableColorBuild2) {
			obj->m_detailColor->m_colorID = color2;
		}
		return obj;
	}

	void selectObject(GameObject* obj, bool p1) {
		if (auto eObj = static_cast<EffectGameObject*>(obj))
			ErGui::saveHueValues(&eObj->m_triggerTargetColor);

		// orig
		EditorUI::selectObject(obj, p1);

		// Visible Selection
		if (ErGui::isVisibleSelection) {
			obj->m_isSelected = 1;
			GameManager::sharedState()->m_levelEditorLayer->m_editorUI->resetSelectedObjectsColor();
		}
		else {
			obj->m_isSelected = 0;
		}

		//std::cout << obj->m_colorSprite << "\n";
	}

	void selectObjects(CCArray* objArr, bool p1) {
		auto singleSelectedObject = EditorUI::get()->m_selectedObject;
		
		EditorUI::selectObjects(objArr, p1);
		ErGui::groupInfoUpdate();

		// Visible Selection
		if (ErGui::isVisibleSelection) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isSelected = 1;
			}
			GameManager::sharedState()->m_levelEditorLayer->m_editorUI->resetSelectedObjectsColor();
		}
		else {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isSelected = 0;
			}

			// Preventing one stupid bug, which causes that single selected object to turn green after this func call
			if (singleSelectedObject) {
				singleSelectedObject->m_isSelected = 0;
			}
		}
	}

	void processSelectObjects(CCArray* objArr) { // I hope this would not cause some more bugs, gosh...
		CCArray* sfr = ErGui::selectFilterRealization(objArr);
		EditorUI::processSelectObjects(sfr);
	}

	

	bool init(LevelEditorLayer* lel) {
		ErGui::editorUIDrawNode = CCDrawNode::create();
		lel->m_objectParent->addChild(ErGui::editorUIDrawNode);
		ErGui::editorUIDrawNode->setZOrder(2);

		ErGui::touchedDN = CCDrawNode::create();
		lel->m_objectParent->addChild(ErGui::touchedDN);
		ErGui::touchedDN->setZOrder(2);

		auto ret = EditorUI::init(lel);

		m_fields->nothing = 42; // trigger m_fields lazy initialization

		this->setVisible(false);

		return ret;
	}

	void updateGridNodeSize() {
		auto actualMode = m_selectedMode;
		m_selectedMode = 2;
		EditorUI::updateGridNodeSize();
		m_selectedMode = actualMode;
	}

	bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
		if (ErGui::rightTouch) return CCLayer::ccTouchBegan (touch, event);

		// Fix for touch
		if (m_touchID == 0) m_touchID = -1;

		//DEBUG TOUCH POS
		if (ErGui::dbgTDN) {
			ErGui::touchedDNFirstPoint = touch->getLocation();
			ErGui::touchedDN->drawDot(ErGui::touchedDNFirstPoint, 2.5f, { 0.f, 0.f, 0.f, 1.f });
			ErGui::touchedDN->drawDot(ErGui::touchedDNFirstPoint, 1.5f, { 0.82f, 0.25f, 0.82f, 1.f });
		}

		auto cameraPos = this->m_editorLayer->m_objectLayer->getPosition();
		auto cameraScale = this->m_editorLayer->m_objectLayer->getScale();
		CCPoint touchConverted = CCPoint(
			(touch->getLocation() - cameraPos) / cameraScale
		);

		//FREE MOVE CONDITION
		if (!(this->m_editorLayer->objectAtPosition(touchConverted) && this->m_freeMoveEnabled)) {
			ErGui::isFreeMoveAndObjectTouching = false;
			//LASSO + SWIPE
			if (this->m_selectedMode == 3 && (m_swipeEnabled || CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())) {
				ErGui::editorUISwipePoints.clear();
				CCPoint pt = touch->getLocation();
				ErGui::editorUISwipePoints.push_back(pt);
				ErGui::editorUIDrawNode->clear();
				//return true;
			}
		}
		else {
			ErGui::isFreeMoveAndObjectTouching = true;
		}

		return EditorUI::ccTouchBegan(touch, event);
	}


	void ccTouchMoved(CCTouch* touch, CCEvent* event) {
		if (ErGui::rightTouch) {
			CCLayer::ccTouchMoved(touch, event);
			return;
		}

		//if (this->m_transformControl->active)

		//DEBUG TOUCH POS
		if (ErGui::dbgTDN) {
			ErGui::touchedDN->clear();
			std::vector<cocos2d::CCPoint> touchedDNVerts = { ErGui::touchedDNFirstPoint, touch->getLocation() };
			ErGui::touchedDN->drawLines(touchedDNVerts.data(), touchedDNVerts.size(), 1.5f, { 0.f, 0.f, 0.f, 1.f });
			ErGui::touchedDN->drawLines(touchedDNVerts.data(), touchedDNVerts.size(), 0.5f, { 0.82f, 0.82f, 0.25f, 1.f });

			ErGui::touchedDN->drawDot(touchedDNVerts[0], 2.5f, { 0.f, 0.f, 0.f, 1.f });
			ErGui::touchedDN->drawDot(touchedDNVerts[0], 1.5f, { 0.82f, 0.25f, 0.82f, 1.f });

			ErGui::touchedDN->drawDot(touchedDNVerts[1], 2.5f, { 0.f, 0.f, 0.f, 1.f });
			ErGui::touchedDN->drawDot(touchedDNVerts[1], 1.5f, { 0.82f, 0.82f, 0.25f, 1.f });
		}


		//ZOOM MODE - Нужно переделать
		//if (this->m_selectedMode == 4) {
		//	auto editorUI = GameManager::sharedState()->getEditorLayer()->m_editorUI;
		//	float zoomMul = Mod::get()->template getSavedValue<float>("zoom-multiplier");
		//	editorUI->m_editorZoom += (touch->getLocation().x - touch->getPreviousLocation().x) * 0.01f * zoomMul;
		//	if (editorUI->m_editorZoom > 4.f) editorUI->m_editorZoom = 4.f;
		//	if (editorUI->m_editorZoom < 0.1f) editorUI->m_editorZoom = 0.1f;
		//	editorUI->updateZoom(editorUI->m_editorZoom);
		//	//GameManager::sharedState()->getEditorLayer()->m_editorUI->m_editorZoom;
		//	//EditorUI::get()->m_editorZoom += touch->getLocation().x - touch->getPreviousLocation().x;
		//	return;
		//}

		cocos2d::ccColor4F lassoColor = { 0.f, 1.f, 0.f, 1.f };
		switch (Mod::get()->getSavedValue<int>("select-mode")) {
		case 2:	// Subtractive
			lassoColor = { 1.f, 0.f, 1.f, 1.f };
			break;
		case 3: // Intersective
			lassoColor = { 1.f, 0.6f, 0.f, 1.f };
			break;
		case 1: // Additive
		default:
			break;
		}

		if (!ErGui::isFreeMoveAndObjectTouching) {
			ccColor4F selectionFillColor = Mod::get()->getSavedValue<bool>("fill-selection-zone") ?
				ccColor4F{ lassoColor.r - lassoColor.r / 1.3f, lassoColor.g - lassoColor.g / 1.3f, lassoColor.b - lassoColor.b / 1.3f, 0.1f } :
				ccColor4F{ 0, 0, 0, 0 };

			//LASSO
			if (this->m_selectedMode == 3 && ErGui::isLassoEnabled && (m_swipeEnabled || CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())) {
				CCPoint pt = touch->getLocation();
				ErGui::editorUISwipePoints.push_back(pt);
				ErGui::editorUIDrawNode->clear();

				if (ErGui::editorUISwipePoints.size() > 1) {
					ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUISwipePoints.data(), ErGui::editorUISwipePoints.size(), selectionFillColor, 0.3f, lassoColor);
				}
				if (geode::Mod::get()->getSavedValue<bool>("hovering-selects")) 
					ErGui::forEachObject(this->m_editorLayer, ErGui::hoverObjectLasso);
				return;
			}

			//SWIPE
			if (this->m_selectedMode == 3 && (m_swipeEnabled || CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())) {
				CCPoint ptStart = ErGui::editorUISwipePoints.at(0);
				CCPoint ptEnd = touch->getLocation();
				ErGui::editorUIDrawNode->clear();
				ErGui::editorUISwipePoints.clear();
				ErGui::editorUISwipePoints.push_back(ptStart);
				ErGui::editorUISwipePoints.push_back({ ptStart.x, ptEnd.y });
				ErGui::editorUISwipePoints.push_back(ptEnd);
				ErGui::editorUISwipePoints.push_back({ ptEnd.x, ptStart.y });

				ErGui::selectRect = CCRect(ptStart, { ptEnd.x - ptStart.x, ptEnd.y - ptStart.y });
				//CCRect drawingRect = CCRect((ptEnd.x + ptStart.x) / 2, (ptEnd.y + ptStart.y) / 2, ptEnd.x - ptStart.x, ptEnd.y + ptStart.y);

				if (ErGui::editorUISwipePoints.size() > 1) {
					//ErGui::editorUIDrawNode->drawRect(ErGui::selectRect, { 1.f - 1.f / 1.33f, 0, 0, 0.1f }, 0.3f, {1.f, 0, 0, 1.f});
					ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUISwipePoints.data(), ErGui::editorUISwipePoints.size(), selectionFillColor, 0.3f, lassoColor);
				}
				if (geode::Mod::get()->getSavedValue<bool>("hovering-selects"))
					ErGui::forEachObject(this->m_editorLayer, ErGui::hoverObjectSquare); // Replace with hoverObjectSquare when done properly
			}
		}

		EditorUI::ccTouchMoved(touch, event);
	}

	void ccTouchEnded(CCTouch* touch, CCEvent* event) {
		if (ErGui::rightTouch) {
			auto cameraPos = this->m_editorLayer->m_objectLayer->getPosition();
			auto cameraScale = this->m_editorLayer->m_objectLayer->getScale();
			CCPoint touchConverted = CCPoint(
				(touch->getLocation() - cameraPos) / cameraScale
			);
			if (auto obj = this->m_editorLayer->objectAtPosition(touchConverted)) {
				ErGui::objectUnderCursor = obj;
				//obj->setColor({ 255, 50, 190 });
				//obj->setChildColor({ 190, 40, 140 });
			}

			ErGui::shouldOpenContextMenu = !ErGui::shouldOpenContextMenu;
			return CCLayer::ccTouchEnded(touch, event);
		}

		ErGui::touchedDN->clear();
		ErGui::editorUIDrawNode->clear();
		ErGui::forEachObject(this->m_editorLayer, ErGui::resetHover);
		//SWIPE
		//if (!ErGui::isLassoEnabled && this->m_selectedMode == 3 && (m_swipeEnabled || CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())) {

		//	// тот самый способ получше, но я просто задолбался уже, а objectsInRect надо искать.
		//	//this->createUndoSelectObject(false);
		//	//
		//	//CCArray* selectedObjsDelta = CCArray::create();
		//	//selectedObjsDelta->addObjectsFromArray(this->m_editorLayer->objectsInRect(CCRect(ErGui::editorUISwipePoints.at(0), ErGui::editorUISwipePoints.at(3)), false));
		//	//if (this->m_selectedObject) selectedObjsDelta->addObject(this->m_selectedObject);
		//	//manualObjectsSelect(selectedObjsDelta);
		//	//
		//	//// saving undo list
		//	//CCArray* undoObjects = CCArray::create();
		//	//undoObjects->addObjectsFromArray(LevelEditorLayer::get()->m_undoObjects);
		//	//
		//	//EditorUI::ccTouchEnded(touch, event);
		//	//
		//	//// loading undo list
		//	//LevelEditorLayer::get()->m_undoObjects->removeAllObjects();
		//	//LevelEditorLayer::get()->m_undoObjects->addObjectsFromArray(undoObjects);
		//	//
		//	//return;

		//	return;
		//}

		if (!ErGui::isFreeMoveAndObjectTouching) {
			//LASSO
			if (ErGui::editorUISwipePoints.size() > 2 && ErGui::isLassoEnabled &&
				this->m_selectedMode == 3 && (m_swipeEnabled || CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())) {
				ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUISwipePoints.data(), ErGui::editorUISwipePoints.size(), { 0, 0, 0, 0 }, 0.3f, { 0, 1.f, 0, 1.f });

				CCArray* objArr = CCArray::create();
				for (auto obj : CCArrayExt<GameObject*>(GameManager::sharedState()->getEditorLayer()->m_objects)) {

					if (!(obj->m_editorLayer == LevelEditorLayer::get()->m_currentLayer || (obj->m_editorLayer2 == LevelEditorLayer::get()->m_currentLayer && obj->m_editorLayer2 != 0) || this->m_editorLayer->m_currentLayer == -1)) continue;

					auto editorLayer = GameManager::sharedState()->getEditorLayer();
					auto cameraPos = editorLayer->m_objectLayer->getPosition();
					auto cameraScale = editorLayer->m_objectLayer->getScale();

					auto objPos = obj->getPosition();
					auto newPos = cocos2d::CCPoint(
						(objPos.x * cameraScale) + cameraPos.x,
						(objPos.y * cameraScale) + cameraPos.y
					);

					if (ErGui::isPointInPolygon(newPos, ErGui::editorUISwipePoints)) {
						objArr->addObject(obj);
					}
				}

				// saving selected objects
				CCArray* selectedObjs = CCArray::create();
				selectedObjs->addObjectsFromArray(this->m_selectedObjects);
				GameObject* selectedObj = this->m_selectedObject; // could be nullptr

				CCArray* undoObjects = CCArray::create();
				undoObjects->addObjectsFromArray(LevelEditorLayer::get()->m_undoObjects);
				EditorUI::ccTouchEnded(touch, event);
				LevelEditorLayer::get()->m_undoObjects->removeAllObjects();
				LevelEditorLayer::get()->m_undoObjects->addObjectsFromArray(undoObjects);
				this->deselectAll();

				this->selectObjects(selectedObjs, false);
				if (selectedObj) this->selectObject(selectedObj, false);

				this->createUndoSelectObject(false);
				manualObjectsSelect(objArr);
				ErGui::editorUIDrawNode->clear();

				if (ErGui::compareCCArrays(this->m_selectedObjects, selectedObjs) && this->m_selectedObject == selectedObj) {
					LevelEditorLayer::get()->m_undoObjects->removeLastObject();
				}

				return;
			}
			// SWIPE + Swiping Single Touches
			else if (this->m_selectedMode == 3 && (m_swipeEnabled || CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())) {
				// saving selected objects
				CCArray* selectedObjs = CCArray::create();
				selectedObjs->addObjectsFromArray(this->m_selectedObjects);
				GameObject* selectedObj = this->m_selectedObject; // could be nullptr

				this->deselectAll();

				// saving undo list
				CCArray* undoObjects = CCArray::create();
				undoObjects->addObjectsFromArray(LevelEditorLayer::get()->m_undoObjects);


				EditorUI::ccTouchEnded(touch, event);

				// saving delta objs
				CCArray* selectedObjsDelta = CCArray::create();
				selectedObjsDelta->addObjectsFromArray(this->m_selectedObjects);
				if (this->m_selectedObject) selectedObjsDelta->addObject(this->m_selectedObject);
				this->deselectAll();

				// loading selected objects
				this->selectObjects(selectedObjs, false);
				if (selectedObj) this->selectObject(selectedObj, false);

				// loading undo list
				LevelEditorLayer::get()->m_undoObjects->removeAllObjects();
				LevelEditorLayer::get()->m_undoObjects->addObjectsFromArray(undoObjects);

				this->createUndoSelectObject(false);
				manualObjectsSelect(selectedObjsDelta);
				if (ErGui::compareCCArrays(this->m_selectedObjects, selectedObjs) && this->m_selectedObject == selectedObj) {
					LevelEditorLayer::get()->m_undoObjects->removeLastObject();
				}

				return;
			}
			// Single Touches without swiping
			else if (this->m_selectedMode == 3) {
				EditorUI::ccTouchEnded(touch, event);
				if (auto singleSelectedObject = this->m_selectedObject) {
					if (ErGui::selectFilterRealization(singleSelectedObject)) {
						EditorUI::get()->selectObject(singleSelectedObject, false);
					}
				}
			}
		}
		

		EditorUI::ccTouchEnded(touch, event);
	}

};


//class $modify(DrawGridLayer) {
//	void draw() {
//		int colOffset = 0;
//		int rowOffset = 0;
//		float step = this->m_gridSize;
//		auto lel = LevelEditorLayer::get();
//
//
//		float botY = lel->m_objectLayer->getPositionY() / lel->m_objectLayer->getScale() * -1;
//		float botYRounded = std::round(botY / step) * step;
//		float topY = CCDirector::sharedDirector()->getWinSize().height / lel->m_objectLayer->getScale() + botY;
//
//		do {
//			botYRounded += step;
//			rowOffset += 2;
//		} while (botYRounded < topY);
//
//		//std::cout << rowOffset << "\n";
//
//
//		float leftX = lel->m_objectLayer->getPositionX() / lel->m_objectLayer->getScale();
//		float leftXRounded = std::round(leftX / step) * step;
//		float rightX = CCDirector::sharedDirector()->getWinSize().width / lel->m_objectLayer->getScale() + leftX;
//
//		do {
//			leftXRounded += step;
//			colOffset += 2;
//		} while (leftXRounded < rightX);
//
//		//std::cout << colOffset << "\n";
//		
//		//Never ask man his salary, woman her age and Taswert, why is there +8
//		unsigned int result = colOffset + rowOffset + 8;
//		if (result > 398) result = 398;
//
//		//std::cout << result << "\n\n";
//
//		//std::cout << "Size: " << m_timeMarkers->count() << "\n";
//		//std::cout << "String: " << m_timeMarkerString << "\n";
//		//if (m_timeMarkers->count()) std::cout << static_cast<CCString*>(m_timeMarkers->objectAtIndex(1))->m_sString << "\n";
//		//std::cout << "\n";
//
//
//		if (GameManager::sharedState()->getGameVariable("0038")) {
//			cocos2d::ccDrawColor4B(ErGui::gridColor[0], ErGui::gridColor[1], ErGui::gridColor[2], ErGui::gridColor[3]);		// Color (DefaultRGBA - 0, 0, 0, 150)
//			glLineWidth(ErGui::gridWidth);																					// Width (Default - 1.f)
//			cocos2d::ccDrawLines(reinterpret_cast<CCPoint*>(this->m_pointArray1), result);									// Point & Lines Number
//			glLineWidth(1.f);
//		}
//
//		DrawGridLayer::draw();
//	}
//};



//class $modify(CCDirector) {
//	void drawScene() {
//		CCDirector::drawScene();
//		//if (ImGuiCocos::get().isInitialized())
//		//	ImGuiCocos::get().drawFrame();
//		ErGui::gameFrame = captureScreenToGLTexture();
//		
//	}
//};

class $modify(CCEGLView) {

	void pollEvents() {
		auto& io = ImGui::GetIO();

		if (ErGui::isGameWindowHovered) io.WantCaptureMouse = false;

		bool blockInput = false;
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);

			if (msg.message == WM_MOUSEHWHEEL) {
				CCDirector::sharedDirector()->getMouseDispatcher()->dispatchScrollMSG(0, GET_WHEEL_DELTA_WPARAM(msg.wParam) * 0.4f * -1);
				continue;
			}

			if (EditorUI::get() && !EditorUI::get()->m_isPaused && !io.WantCaptureMouse) {
				switch (msg.message) {
					case WM_RBUTTONDOWN: {

						if (ErGui::rightTouch) {
							auto setCancel = CCSet::create();
							setCancel->addObject(ErGui::rightTouch);
							CCDirector::sharedDirector()->getTouchDispatcher()->touchesCancelled(setCancel, nullptr);

							ErGui::rightTouch->release();
							ErGui::rightTouch = nullptr;
						}

						ErGui::rightTouch = new CCTouch;
						ErGui::rightTouch->retain();
						ErGui::rightTouch->setTouchInfo(1, LOWORD(msg.lParam), HIWORD(msg.lParam));

						auto setBegan = CCSet::create();
						setBegan->addObject(ErGui::rightTouch);

						CCDirector::sharedDirector()->getTouchDispatcher()->touchesBegan(setBegan, nullptr);
						break;
					}
					case WM_MOUSEMOVE: {
						if (!ErGui::rightTouch) break;

						ErGui::rightTouch->setTouchInfo(1, LOWORD(msg.lParam), HIWORD(msg.lParam));

						auto setMoved = CCSet::create();
						setMoved->addObject(ErGui::rightTouch);
						CCDirector::sharedDirector()->getTouchDispatcher()->touchesMoved(setMoved, nullptr);

						break;
					}
					case WM_RBUTTONUP: {
						if (!ErGui::rightTouch) break;

						ErGui::rightTouch->setTouchInfo(1, LOWORD(msg.lParam), HIWORD(msg.lParam));

						auto setEnded = CCSet::create();
						setEnded->addObject(ErGui::rightTouch);
						CCDirector::sharedDirector()->getTouchDispatcher()->touchesEnded(setEnded, nullptr);

						ErGui::rightTouch->release();
						ErGui::rightTouch = nullptr;
						break;
					}
				}
			}

			if (io.WantCaptureMouse) {
				switch (msg.message) {
				case WM_RBUTTONDOWN:
					io.AddMouseButtonEvent(1, true);
					break;
				case WM_RBUTTONUP:
					io.AddMouseButtonEvent(1, false);
					break;
				case WM_MBUTTONDOWN:
					io.AddMouseButtonEvent(2, true);
					break;
				case WM_MBUTTONUP:
					io.AddMouseButtonEvent(2, false);
				}
			}

			if (!blockInput)
				DispatchMessage(&msg);

		}

		// Гарантированно почистить, если Windows не прислала UP
		if (ErGui::rightTouch && !GetAsyncKeyState(VK_RBUTTON)) {
			auto setCancelled = CCSet::create();
			setCancelled->addObject(ErGui::rightTouch);
			CCDirector::sharedDirector()->getTouchDispatcher()->touchesCancelled(setCancelled, nullptr);

			ErGui::rightTouch->release();
			ErGui::rightTouch = nullptr;
		}

		CCEGLView::pollEvents();
	}
};


class $modify(CCTouchDispatcher) {
	void touches(CCSet * touches, CCEvent * event, unsigned int type) {
		auto& io = ImGui::GetIO();
		auto* touch = static_cast<CCTouch*>(touches->anyObject());
		if (!touch) return;

		

		if (type == CCTOUCHBEGAN && ErGui::isGameWindowHovered) {
			ErGui::isGameWindowTouching = true;
		}

		if (ErGui::isGameWindowTouching) {
			ImGui::GetIO().WantCaptureMouse = false;
			touch->setTouchInfo(touch->getID(), ErGui::gameWindowTouchCoordinatesConvertedToWorld.x, ErGui::gameWindowTouchCoordinatesConvertedToWorld.y);
			ImGui::ClearActiveID();
		}

		if (type == CCTOUCHENDED || type == CCTOUCHCANCELLED) {
			ErGui::isGameWindowTouching = false;
		}

		
		CCTouchDispatcher::touches(touches, event, type);
	}
};

$on_mod(Loaded) {
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

	if (!Mod::get()->hasSavedValue("grid-size"))					Mod::get()->setSavedValue("grid-size", 30.f);
	if (!Mod::get()->hasSavedValue("zoom-multiplier"))				Mod::get()->setSavedValue("zoom-multiplier", 1.f);
	if (!Mod::get()->hasSavedValue("select-mode"))					Mod::get()->setSavedValue("select-mode", 1);
	if (!Mod::get()->hasSavedValue("hide-object-list-popup"))		Mod::get()->setSavedValue("hide-object-list-popup", true);
	if (!Mod::get()->hasSavedValue("autoswitch-to-build-mode"))		Mod::get()->setSavedValue("autoswitch-to-build-mode", true);
	if (!Mod::get()->hasSavedValue("show-zoom-controls"))			Mod::get()->setSavedValue("show-zoom-controls", true);

	if (!Mod::get()->hasSavedValue("soi-position"))					Mod::get()->setSavedValue("soi-position", true);
	if (!Mod::get()->hasSavedValue("soi-rotation"))					Mod::get()->setSavedValue("soi-rotation", true);
	if (!Mod::get()->hasSavedValue("soi-scale"))					Mod::get()->setSavedValue("soi-scale", true);
	if (!Mod::get()->hasSavedValue("soi-color"))					Mod::get()->setSavedValue("soi-color", true);
	if (!Mod::get()->hasSavedValue("soi-hsv"))						Mod::get()->setSavedValue("soi-hsv", false);
	if (!Mod::get()->hasSavedValue("soi-groups"))					Mod::get()->setSavedValue("soi-groups", true);
	if (!Mod::get()->hasSavedValue("soi-zlayer"))					Mod::get()->setSavedValue("soi-zlayer", true);
	if (!Mod::get()->hasSavedValue("soi-zorder"))					Mod::get()->setSavedValue("soi-zorder", true);
	if (!Mod::get()->hasSavedValue("soi-objectid"))					Mod::get()->setSavedValue("soi-objectid", false);
	if (!Mod::get()->hasSavedValue("soi-targetgroup"))				Mod::get()->setSavedValue("soi-targetgroup", true);
	if (!Mod::get()->hasSavedValue("soi-itemid"))					Mod::get()->setSavedValue("soi-itemid", false);
	if (!Mod::get()->hasSavedValue("soi-blockid"))					Mod::get()->setSavedValue("soi-blockid", false);
	if (!Mod::get()->hasSavedValue("soi-particles"))				Mod::get()->setSavedValue("soi-particles", true);
	if (!Mod::get()->hasSavedValue("soi-hidden"))					Mod::get()->setSavedValue("soi-hidden", true);
	if (!Mod::get()->hasSavedValue("soi-no-touch"))					Mod::get()->setSavedValue("soi-no-touch", true);
	if (!Mod::get()->hasSavedValue("soi-high-detail"))				Mod::get()->setSavedValue("soi-high-detail", false);
	if (!Mod::get()->hasSavedValue("soi-object-count"))				Mod::get()->setSavedValue("soi-object-count", true);
	
	if (!Mod::get()->hasSavedValue("fill-selection-zone"))			Mod::get()->setSavedValue("fill-selection-zone", false);
	if (!Mod::get()->hasSavedValue("hovering-selects"))				Mod::get()->setSavedValue("hovering-selects", true);
	//if (Mod::get()->getSavedValue<int>("build-color-1") == 0) Mod::get()->setSavedValue("build-color-1", 1);
	//if (Mod::get()->getSavedValue<int>("build-color-2") == 0) Mod::get()->setSavedValue("build-color-2", 1);
	//if (Mod::get()->getSavedValue<bool>("enable-build-color-1") == 0) Mod::get()->setSavedValue("enable-build-color-1", 1);
	//if (Mod::get()->getSavedValue<bool>("enable-build-color-2") == 0) Mod::get()->setSavedValue("enable-build-color-2", 1);
	
	ErGui::editorUIbottomConstrainPatch->enable();
	//ErGui::vanillaGridOpaquePatch->enable();
	
	// DEBUG - позволяет смотреть оффсеты полей
	//ErGui::objectCfg = data;
	//std::cout
	//	<< "Offset DrawGridLayer::m_gridSize = "
	//	<< offsetof(DrawGridLayer, DrawGridLayer::m_gridSize)
	//	<< " bytes\n";

	ImGuiCocos::get().setup([] {
		ImGuiIO& io = ImGui::GetIO();
		
		if (!std::filesystem::exists(io.IniFilename)) {
			const char* prev_ini = io.IniFilename;
			io.IniFilename = nullptr;
			auto defaultCfgDir = Mod::get()->getResourcesDir() / "default_imgui.ini";
			ImGui::LoadIniSettingsFromDisk(defaultCfgDir.string().c_str());
			io.IniFilename = prev_ini;
		}


		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		auto interDir = Mod::get()->getResourcesDir() / "Inter_18pt-Regular.ttf";
		auto mdiDir = Mod::get()->getResourcesDir() / "materialdesignicons-webfont.ttf";

		io.Fonts->AddFontFromFileTTF(interDir.string().c_str(), 15.f);

		static const ImWchar icons_ranges[] = { ICON_MIN_MDI, ICON_MAX_MDI, 0 };
		ImFontConfig iconConfig;
		iconConfig.MergeMode = true;
		
		io.Fonts->AddFontFromFileTTF(mdiDir.string().c_str(), 24.f, &iconConfig, icons_ranges);

		ErGui::initImGuiStyling();
	

		ErGui::setupTriggersSettings();
		}).draw([] {
			if (auto lel = GameManager::sharedState()->getEditorLayer()) {
				ErGui::renderGlobalDockingView();
				ErGui::renderEditGroupModule();
				ErGui::renderSelectFilter();
				ErGui::renderTransformation();
				ErGui::renderFooter();
				ErGui::renderToolsModule1();
				ErGui::renderToolsModule2();
				ErGui::renderObjectList();
				ErGui::renderEditObjectModule();
				ErGui::renderActionHistoryModule();
				ErGui::renderEditColor();
				ErGui::renderSettingsModule();
				ErGui::renderLayerModule();
				ErGui::renderGameWindow();
				ErGui::renderContextMenu();

				//ImGui::ShowStyleEditor();

			}
			else {
				ErGui::isGameWindowHovered = false;
			}
			});
}
// Mod Developers:
// 
// Taswert
// Rainix
// RaZooM
//
// ----------------------------------
// Thanks to:
//
// Coders:
// Emiya		/ Code Help
// HJFod		/ Main Idea, DevTools
// RaZooM		/ Object Categories
// Mat			/ ImGui integration + Circle Tool
// MgostiH		/ First Supported! 
// Partur		/ UI
// 
// Creators:
// CHRAPIVA		/ Ideas + Feedback
// Kisss		/ Ideas + Feedback
// Vernam		/ Creator Contacts + MOTIVATION
// Shlyapa		/ Ideas + Feedback
// Pes11		/ Ideas
// 
// Others:
// Herowhither	/ Moral Support
// Dbrxvich		/ Moral Support
// Pololak		/ Moral Support
// 
//

//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!