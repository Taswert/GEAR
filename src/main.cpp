#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <imgui_internal.h>

#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>

#include <Geode/modify/CustomSongLayer.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/modify/ObjectToolbox.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

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
#include "modules/ContextMenuModule.hpp"

#include "includes/ObjectCategories.hpp"
#include <matjson.hpp>
#include <matjson/reflect.hpp>
#include <matjson/std.hpp>
#include <matjson/stl_serialize.hpp>

#include <Geode/Result.hpp>

using namespace geode::prelude;

template <>
struct matjson::Serialize<ErGui::ObjectConfig> {
	static geode::Result<ErGui::ObjectConfig> fromJson(const matjson::Value& value) {
		GEODE_UNWRAP_INTO(int thumbnailObjectId, value["thumbnailObjectId"].asInt());
		GEODE_UNWRAP_INTO(std::vector<matjson::Value> arr, value["objectIdVector"].asArray());

		// Преобразование каждого элемента в int
		std::vector<int> vec;
		vec.reserve(arr.size());
		for (auto& item : arr) {
			GEODE_UNWRAP_INTO(int id, item.asInt());
			vec.push_back(id);
		}

		return geode::Ok(ErGui::ObjectConfig{
			thumbnailObjectId,
			std::move(vec)
			});
	}

	static matjson::Value toJson(const ErGui::ObjectConfig& obj) {
		// Собираем JSON-массив из вектора int
		std::vector<matjson::Value> arr;
		arr.reserve(obj.objectIdVector.size());
		for (int id : obj.objectIdVector) {
			arr.emplace_back(id);
		}
		// Value(std::vector<matjson::Value>) создаёт JSON-массив :contentReference[oaicite:0]{index=0}

		// Собираем объект
		return matjson::makeObject({
			{ "thumbnailObjectId", obj.thumbnailObjectId },
			{ "objectIdVector", matjson::Value(std::move(arr)) }
			}); // makeObject(...) для создания JSON-объекта :contentReference[oaicite:1]{index=1}
	}
};

// Написано нейронкой, т.к. я просто устану разбираться в этом сейчас. Основа от DevTools'ов.
static GLuint captureScreenToGLTexture() {
	static GLuint captureTexId = 0;    // хранит ID текстуры, куда мы копируем кадр
	static int lastW = 0, lastH = 0;   // чтобы пересоздавать текстуру, если размер изменился

	// 1) Узнаём, сколько пикселей реально сейчас на экране (viewport):
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	int screenW = vp[2];
	int screenH = vp[3];

	// 2) Если размер изменился (например, игрок поменял окно), пересоздаём текстуру:
	if (!captureTexId || lastW != screenW || lastH != screenH) {
		if (captureTexId) {
			glDeleteTextures(1, &captureTexId);
			captureTexId = 0;
		}
		glGenTextures(1, &captureTexId);
		glBindTexture(GL_TEXTURE_2D, captureTexId);

		// Настраиваем фильтрацию — обычно LINEAR, чтобы превью при масштабировании было быстрее «размыто»
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Резервируем память под будущую копию экрана
		glTexImage2D(
			GL_TEXTURE_2D,		// таргет
			0,					// LOD
			GL_RGBA,			// внутренний формат (RGBA8)
			screenW, screenH,	// ширина, высота
			0,					 // border
			GL_RGBA,			// формат поступающих данных
			GL_UNSIGNED_BYTE,	// тип данных
			nullptr				// пока нет данных (будет заполнено glCopyTexSubImage2D)
		);
		glBindTexture(GL_TEXTURE_2D, 0);

		lastW = screenW;
		lastH = screenH;
	}

	// 3) Привязываем нашу текстуру и копируем текущий back-buffer (FBO = 0) в неё:
	glBindTexture(GL_TEXTURE_2D, captureTexId);

	// Копируем целиком экран (вниз/влево от 0,0) в текстуру, начиная с 0,0
	glCopyTexSubImage2D(
		GL_TEXTURE_2D, 0,    // который уровень mipmap (0) и таргет
		0, 0,                // смещение (xOffset, yOffset) в текстуре
		0, 0,                // какая точка back-buffer берётся (x, y)
		screenW, screenH     // сколько пикселей копировать
	);

	// 4) Бинд снова очищаем (не обязательно, но принято)
	glBindTexture(GL_TEXTURE_2D, 0);

	// 5) Возвращаем ID текстуры, куда мы скопировали экран
	return captureTexId;
}


bool filterSingleObj(bool filterBool, int objParameter, std::vector<int> vec) {
	if (!filterBool) return true;
	for (int i = 0; i < vec.size(); i++) {
		if (objParameter == vec[i])
			return true;
	}
	return false;
}

void exitEditor() { // EditorUI is already destroyed here
	//Cfg Save
	auto cfgDir = Mod::get()->getSettingValue<std::filesystem::path>("object-list-config");
	matjson::Value j;
	for (auto key : ErGui::keyOrder) {
		j[key] = ErGui::objectCfg[key];
	}
	std::ofstream oCfgFile = std::ofstream(cfgDir);
	oCfgFile.write(j.dump().c_str(), j.dump().size());
	oCfgFile.close();

	ErGui::lastObjX = nullptr;
	ErGui::lastObjY = nullptr;
	ErGui::lastObjCountX = 0;
	ErGui::lastObjCountY = 0;

	ErGui::clearObjectListCache();
}


class $modify(LevelEditorLayer) {
	void removeSpecial(GameObject * obj) {
		if (obj->m_objectID == 3642) {
			for (auto alObj : this->m_drawGridLayer->m_audioLineObjects) {
				if (alObj.second == obj) {
					this->m_drawGridLayer->m_audioLineObjects.erase(alObj.first);
				}
			}
		}
		LevelEditorLayer::removeSpecial(obj);
	}
};

class $modify(ObjectToolbox) {
	float gridNodeSizeForKey(int id) {
		auto size = Mod::get()->template getSavedValue<float>("grid-size");
		if (size < 1 || roundf(size) == 30) {
			return ObjectToolbox::gridNodeSizeForKey(id);
		}
		return size;
	}
};


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

void manualObjectSelect(GameObject* objInShape) {
	int selectMode = Mod::get()->getSavedValue<int>("select-mode");

	switch (selectMode) {
	case 3: { // Intersective
		EditorUI::get()->deselectAll();
		selectObjectWithFilter(objInShape, false);
		break;
	}
	case 2: { // Subtractive
		if (ErGui::selectFilterRealization(objInShape)) {
			EditorUI::get()->deselectObject(objInShape);
			if (EditorUI::get()->m_selectedObjects->count() == 1) {
				EditorUI::get()->m_selectedObject = static_cast<GameObject*>(EditorUI::get()->m_selectedObjects->objectAtIndex(0));
				EditorUI::get()->m_selectedObjects->removeAllObjects();
			}
		}
		break;
	}
	case 1: // Additive
	default: {
		selectObjectWithFilter(objInShape, false);
		break;
	}
	}
}




class $modify(EditorUI) {

	struct Fields {
		int nothing = 0; // trigger lazy m_fields initialization
		Fields() {}
		~Fields() {
			exitEditor();
		}
	};

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



		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())
			EditorUI::scrollWheel(p1, p0);
		else {
			
			EditorUI::scrollWheel(p0, p1);
			
		}
	}


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


	GameObject* createObject(int p0, CCPoint p1) {
		bool enableColorBuild1 = Mod::get()->getSavedValue<bool>("enable-build-color-1");
		bool enableColorBuild2 = Mod::get()->getSavedValue<bool>("enable-build-color-2");

		int color1 = Mod::get()->template getSavedValue<int>("build-color-1");
		int color2 = Mod::get()->template getSavedValue<int>("build-color-2");
		auto obj = EditorUI::createObject(p0, p1);
		if (obj->m_baseColor && enableColorBuild1) {
			obj->m_baseColor->m_colorID = color1;
			std::cout << "main\n";
		}
		if (obj->m_detailColor && enableColorBuild2) {
			obj->m_detailColor->m_colorID = color2;
			std::cout << "det\n";
		}
		return obj;
	}

	void selectObject(GameObject* obj, bool p1) {
		if (auto eObj = static_cast<EffectGameObject*>(obj))
			ErGui::saveHueValues(&eObj->m_triggerTargetColor);
		EditorUI::selectObject(obj, p1);
	}

	void selectObjects(CCArray* objArr, bool p1) {
		EditorUI::selectObjects(objArr, p1);
		ErGui::groupInfoUpdate();
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
		//ErGui::originalCameraPosition = lel->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();

		//ErGui::og_prevMode = lel->m_previewMode;
		//ErGui::og_prevShad = lel->m_previewShaders;
		//ErGui::og_prevPart = lel->m_previewParticles;
		//ErGui::og_prevAnim = lel->m_previewAnimations;

		this->setVisible(false);
		//this->m_constrainedHeight = 0.f;

		return ret;
	}

	virtual void keyDown(cocos2d::enumKeyCodes p0) {

		if (p0 == cocos2d::enumKeyCodes::KEY_Four) {
			GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 4;
		}

		//if (p0 == cocos2d::enumKeyCodes::KEY_Five) {
		//	GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 5;
		//}

		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed() && p0 == cocos2d::enumKeyCodes::KEY_A) {
			ErGui::selectAllObjects();
		}

		// todo: select all right / select all left

		EditorUI::keyDown(p0);
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
			//LASSO
			if (this->m_selectedMode == 3 && ErGui::isLassoEnabled && (m_swipeEnabled || CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())) {
				CCPoint pt = touch->getLocation();
				ErGui::editorUISwipePoints.push_back(pt);
				ErGui::editorUIDrawNode->clear();

				if (ErGui::editorUISwipePoints.size() > 1) {
					ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUISwipePoints.data(), ErGui::editorUISwipePoints.size(), { 0, 0, 0, 0 }, 0.3f, lassoColor);
				}
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

				if (ErGui::editorUISwipePoints.size() > 1) {
					ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUISwipePoints.data(), ErGui::editorUISwipePoints.size(), { 0, 0, 0, 0 }, 0.3f, lassoColor);
				}
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

			ErGui::shouldOpenContextMenu = true;
			return CCLayer::ccTouchEnded(touch, event);
		}

		ErGui::touchedDN->clear();
		ErGui::editorUIDrawNode->clear();

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
					auto cameraPos = editorLayer->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();
					auto cameraScale = editorLayer->getChildByID("main-node")->getChildByID("batch-layer")->getScale();

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
	void swapBuffers() {
		ErGui::gameFrame = captureScreenToGLTexture();
		CCEGLView::swapBuffers();
	}
	
	void onGLFWMouseCallback(GLFWwindow* window, int button, int action, int mods) {
		CCEGLView::onGLFWMouseCallBack(window, button, action, mods);
		std::cout
			<< "Button: " << button << "\n"
			<< "Action: " << action << "\n"
			<< "Mods: " << mods << "\n";
	}

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
						ErGui::rightTouch = new CCTouch;
						ErGui::rightTouch->setTouchInfo(1, LOWORD(msg.lParam), HIWORD(msg.lParam));

						auto setBegan = CCSet::create();
						setBegan->addObject(ErGui::rightTouch);

						CCDirector::sharedDirector()->getTouchDispatcher()->touchesBegan(setBegan, nullptr);
						break;
					}
					case WM_MOUSEMOVE: {
						if (ErGui::rightTouch) {
							ErGui::rightTouch->setTouchInfo(1, LOWORD(msg.lParam), HIWORD(msg.lParam));

							auto setMoved = CCSet::create();
							setMoved->addObject(ErGui::rightTouch);
							CCDirector::sharedDirector()->getTouchDispatcher()->touchesMoved(setMoved, nullptr);
						}
						break;
					}
					case WM_RBUTTONUP: {
						ErGui::rightTouch->setTouchInfo(1, LOWORD(msg.lParam), HIWORD(msg.lParam));

						auto setEnded = CCSet::create();
						setEnded->addObject(ErGui::rightTouch);
						CCDirector::sharedDirector()->getTouchDispatcher()->touchesEnded(setEnded, nullptr);

						auto setCancelled = CCSet::create();
						setCancelled->addObject(ErGui::rightTouch);
						CCDirector::sharedDirector()->getTouchDispatcher()->touchesCancelled(setCancelled, nullptr);

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

	if (Mod::get()->getSavedValue<float>("grid-size") == 0.f) Mod::get()->setSavedValue("grid-size", 30.f);
	if (Mod::get()->getSavedValue<float>("zoom-multiplier") == 0.f) Mod::get()->setSavedValue("zoom-multiplier", 1.f);
	if (Mod::get()->getSavedValue<int>("select-mode") == 0) Mod::get()->setSavedValue("select-mode", 1);
	//if (Mod::get()->getSavedValue<int>("build-color-1") == 0) Mod::get()->setSavedValue("build-color-1", 1);
	//if (Mod::get()->getSavedValue<int>("build-color-2") == 0) Mod::get()->setSavedValue("build-color-2", 1);
	//if (Mod::get()->getSavedValue<bool>("enable-build-color-1") == 0) Mod::get()->setSavedValue("enable-build-color-1", 1);
	//if (Mod::get()->getSavedValue<bool>("enable-build-color-2") == 0) Mod::get()->setSavedValue("enable-build-color-2", 1);

	auto cfgDir = Mod::get()->getSettingValue<std::filesystem::path>("object-list-config");

	std::ifstream cfgFile = std::ifstream(cfgDir);
	geode::Result parsed = matjson::parse(cfgFile);
	
	if (!parsed) {
		matjson::Value j;
		for (auto key : ErGui::keyOrder) {
			j[key] = ErGui::getDefaultObjectCfg()[key];
		}

		auto someError = parsed.unwrapErr();
		if (parsed.unwrapErr().message == "eof") {
			std::ofstream oCfgFile = std::ofstream(cfgDir);
			oCfgFile.write(j.dump().c_str(), j.dump().size());
			oCfgFile.close();
		}
		std::cout << "Failed to parse json: " << someError.message << " " << typeid(someError).name() << "\n";
		
		ErGui::objectCfg = ErGui::getDefaultObjectCfg();

	}
	else {
		ErGui::objectCfg.clear();
		// ErGui::keyOrder.clear();
		matjson::Value parsedObject = parsed.unwrap();
		std::map<std::string, std::vector<ErGui::ObjectConfig>> newData;

		for (const auto& [key, mapValue] : parsedObject) {
			// ErGui::keyOrder.push_back(key);
			auto myVectorOfObjCfg = mapValue.asArray().unwrap();
			std::vector<ErGui::ObjectConfig> objCfgVector;
			for (const auto& objCfgValue : myVectorOfObjCfg) {
				auto unwrappedObjCfg = objCfgValue.as<ErGui::ObjectConfig>().unwrap();
				objCfgVector.push_back(unwrappedObjCfg);
			}
			
			ErGui::objectCfg[key] = objCfgVector;
		}
	}
	
	cfgFile.close();
	

	ErGui::editorUIbottomConstrainPatch->enable();
	
	// DEBUG - позволяет смотреть оффсеты полей
	//ErGui::objectCfg = data;
	//std::cout
	//	<< "Offset ColorSelectPopup::m_touchTriggered = "
	//	<< offsetof(EffectGameObject, EffectGameObject::m_centerGroupID)
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

		auto fontDir = Mod::get()->getResourcesDir() / "Inter_18pt-Regular.ttf";
		io.Fonts->AddFontFromFileTTF(fontDir.string().c_str(), 15.f);

		static const ImWchar icons_ranges[] = { ICON_MIN_MDI, ICON_MAX_MDI, 0 };
		ImFontConfig iconConfig;
		iconConfig.MergeMode = true;
		fontDir = Mod::get()->getResourcesDir() / "materialdesignicons-webfont.ttf";
		io.Fonts->AddFontFromFileTTF(fontDir.string().c_str(), 24.f, &iconConfig, icons_ranges);


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