#include "GearEditorUI.hpp"
#include "GearControls.hpp"

#include "ObjectCategories.hpp"

#include "ObjectListModule.hpp"
#include "EditGroupModule.hpp"
#include "EditObject/EditObjectModule.hpp"
#include "SelectFilterModule.hpp"
#include "ContextMenuModule.hpp"
#include "../features/Hovering.hpp"
#include "../features/Selection.hpp"
#include "PropertiesModule.hpp"

void exitEditor() { // EditorUI is already destroyed here
	// Save Object Config
	auto cfgDir = utils::string::pathToString(Mod::get()->getSettingValue<std::filesystem::path>("object-list-config"));
	matjson::Value j;
	for (const auto& key : ErGui::keyOrder) {
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

GearEditorUI::Fields::~Fields() {
	m_lastUnderCursor->release();
	exitEditor();
}



bool GearEditorUI::onCreate() {
	bool ret = EditorUI::onCreate();
	if (m_selectedObjectIndex) {
		ErGui::addRecentObjectToList(m_selectedObjectIndex);
	}
	return ret;
}

void GearEditorUI::clickOnPosition(CCPoint p0) {
	// dead zone fix
	float tmp = m_toolbarHeight;
	m_toolbarHeight = -999999;
	EditorUI::clickOnPosition(p0);
	m_toolbarHeight = tmp;
}

void GearEditorUI::reloadCustomItems() {
	EditorUI::reloadCustomItems();
	ErGui::reloadCustomObjects();
}

CCArray* GearEditorUI::pasteObjects(gd::string p0, bool p1, bool p2) {
	auto objArr = EditorUI::pasteObjects(p0, p1, p2);
	if (geode::Mod::get()->getSavedValue<bool>("auto-buildhelper", false) && !p1 && !p2) {
		ErGui::getFakePauseLayer()->onBuildHelper(nullptr);
	}
	return objArr;
}



// Colored object building
GameObject* GearEditorUI::createObject(int p0, CCPoint p1) {
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

void GearEditorUI::deselectAll() {
	EditorUI::deselectAll();
	ErGui::g_selectVersion++;
}

void GearEditorUI::deselectObject(GameObject* obj) {
	EditorUI::deselectObject(obj);
	ErGui::g_selectVersion++;
}

void GearEditorUI::selectObject(GameObject * obj, bool p1) {
	if (auto eObj = static_cast<EffectGameObject*>(obj))
		ErGui::saveHueValues(&eObj->m_triggerTargetColor);

	// orig
	EditorUI::selectObject(obj, p1);
	ErGui::g_selectVersion++;

	// Visible Selection
	if (ErGui::isVisibleSelection) {
		obj->m_isSelected = 1;
		GameManager::sharedState()->m_levelEditorLayer->m_editorUI->resetSelectedObjectsColor();
	}
	else {
		obj->m_isSelected = 0;
	}
}

void GearEditorUI::selectObjects(CCArray * objArr, bool p1) {
	auto singleSelectedObject = this->m_selectedObject;

	EditorUI::selectObjects(objArr, p1);
	ErGui::g_selectVersion++;
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

void GearEditorUI::processSelectObjects(CCArray * objArr) { // I hope this would not cause some more bugs, gosh...
	CCArray* sfr = ErGui::selectFilterRealization(objArr);
	EditorUI::processSelectObjects(sfr);
}


void GearEditorUI::onHideUI(CCObject * sender) {
	static_cast<CCMenuItemSpriteExtra*>(sender)->setVisible(false);
	ErGui::hideUI = false;
}

bool GearEditorUI::init(LevelEditorLayer * lel) {
	// Lasso And Swipe
	ErGui::editorUIDrawNode = CCDrawNode::create();
	lel->m_objectParent->addChild(ErGui::editorUIDrawNode);
	ErGui::editorUIDrawNode->setZOrder(2);

	// Debug Draw Node
	ErGui::touchedDN = CCDrawNode::create();
	lel->m_objectParent->addChild(ErGui::touchedDN);
	ErGui::touchedDN->setZOrder(2);

	auto ret = EditorUI::init(lel);

	m_fields->m_lastUnderCursor = CCArray::create();
	m_fields->m_lastUnderCursor->retain();

	this->setVisible(false);

	auto hideBtnSpr = CCSprite::createWithSpriteFrameName("hideBtn_001.png");
	hideBtnSpr->setScale(0.65f);
	hideBtnSpr->setOpacity(100);
	CCMenuItemSpriteExtra* hideUIBtn = CCMenuItemSpriteExtra::create(
		hideBtnSpr, this,
		menu_selector(GearEditorUI::onHideUI)
	);
	hideUIBtn->setID("hideUIBtn"_spr);
	hideUIBtn->setVisible(ErGui::hideUI);

	auto hideUIMenu = CCMenu::create();
	hideUIMenu->addChild(hideUIBtn);
	hideUIMenu->setID("hideUIMenu"_spr);
	hideUIMenu->setPosition({ 13.f, CCDirector::sharedDirector()->getWinSize().height - 10.f });

	lel->addChild(hideUIMenu);

	return ret;
}

void GearEditorUI::updateGridNodeSize() {
	auto actualMode = m_selectedMode;
	m_selectedMode = 2;
	EditorUI::updateGridNodeSize();
	m_selectedMode = actualMode;
}

bool GearEditorUI::ccTouchBegan(CCTouch* touch, CCEvent* event) {
	if (ErGui::rightTouch) 
		return CCLayer::ccTouchBegan(touch, event);

	// Saving Batch Layer Position
	ErGui::beginTouchLocation = touch->getLocation();

	// Fix for touch
	if (m_touchID == 0) m_touchID = -1;

	// --- Debug
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
	if (this->m_editorLayer->objectAtPosition(touchConverted) && this->m_freeMoveEnabled) {
		ErGui::isFreeMoveAndObjectTouching = true;
	}
	else {
		ErGui::isFreeMoveAndObjectTouching = false;

		// --- Lasso And VanillaSwipe ccTouchBegan / void
		if (this->m_selectedMode == 3 && (m_swipeEnabled || CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())) {
			ErGui::ccTouchBegan_LassoAndVanillaSwipe(this, touch);
		}
	}

	return EditorUI::ccTouchBegan(touch, event);
}


void GearEditorUI::ccTouchMoved(CCTouch* touch, CCEvent* event) {
		
	// Maybe leave it here???
	if (ErGui::rightTouch) {
		CCLayer::ccTouchMoved(touch, event);
		return;
	}


	// --- Debug Begin ---
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

	// --- Debug End ---


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
		

	// Getting Controls touch
	bool rotationControlTouched = static_cast<GearRotationControl*>(this->m_rotationControl)->m_fields->m_touchedControl;
	bool scaleControlTouched = static_cast<GearScaleControl*>(this->m_scaleControl)->m_fields->m_touchedControl;
	bool transformControlTouched = static_cast<GearTransformControl*>(this->m_transformControl)->m_fields->m_touchedControl;
	bool someControlTouched = rotationControlTouched || scaleControlTouched || transformControlTouched;


	// Lasso color
	ccColor4F lassoColor = ErGui::g_additiveSelectColor;
	switch (Mod::get()->getSavedValue<int>("select-mode", 1)) {
	case 2:	// Subtractive
		lassoColor = ErGui::g_subtractiveSelectColor;
		break;
	case 3: // Intersective
		lassoColor = ErGui::g_intersectiveSelectColor;
		break;
	case 1: // Additive
	default:
		break;
	}
	

	// Not Free moving object and not touching controls
	if (!ErGui::isFreeMoveAndObjectTouching && !someControlTouched) {
		ccColor4F selectionFillColor = Mod::get()->getSavedValue<bool>("fill-selection-zone", false) ?
			ccColor4F{ lassoColor.r - lassoColor.r / 1.3f, lassoColor.g - lassoColor.g / 1.3f, lassoColor.b - lassoColor.b / 1.3f, 0.1f } :
			ccColor4F{ 0, 0, 0, 0 };

		// --- Lasso ccTouchMoved
		if (this->m_selectedMode == 3 && ErGui::isLassoEnabled && (m_swipeEnabled || CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())) {
			ErGui::ccTouchMoved_Lasso(this, touch, selectionFillColor, lassoColor);
			return;
		}

		// --- Vanilla Swipe ccTouchMoved
		if (this->m_selectedMode == 3 && (m_swipeEnabled || CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed()) && ErGui::editorUISwipePoints.size() > 0) {
			ErGui::ccTouchMoved_VanillaSwipe(this, touch, selectionFillColor, lassoColor);
		}
	}

	EditorUI::ccTouchMoved(touch, event);
}

void GearEditorUI::ccTouchEnded(CCTouch* touch, CCEvent* event) {
	auto lel = LevelEditorLayer::get();

	// --- RMB Begin ---

	if (ErGui::rightTouch) {
		auto cameraPos = lel->m_objectLayer->getPosition();
		auto cameraScale = lel->m_objectLayer->getScale();
		CCPoint touchConverted = CCPoint(
			(touch->getLocation() - cameraPos) / cameraScale
		);
		if (auto obj = lel->objectAtPosition(touchConverted)) {
			ErGui::objectUnderCursor = obj;
		}

		ErGui::shouldOpenContextMenu = !ErGui::shouldOpenContextMenu;
		return CCLayer::ccTouchEnded(touch, event);
	}

	// --- RMB End ---

	ErGui::touchedDN->clear();
	ErGui::editorUIDrawNode->clear();
	ErGui::forEachObject(lel, ErGui::resetHover);

	// Getting controls and ending touching if one of these are touched
	auto rotationControl = this->m_rotationControl;
	bool rotationControlTouched = static_cast<GearRotationControl*>(rotationControl)->m_fields->m_touchedControl;

	auto scaleControl = this->m_scaleControl;
	bool scaleControlTouched = static_cast<GearScaleControl*>(scaleControl)->m_fields->m_touchedControl;

	auto transformControl = this->m_transformControl;
	bool transformControlTouched = static_cast<GearTransformControl*>(transformControl)->m_fields->m_touchedControl;

	if (rotationControlTouched || scaleControlTouched || transformControlTouched) {
		return EditorUI::ccTouchEnded(touch, event);
	}

	
	// Select Mode = Edit
	if (!ErGui::isFreeMoveAndObjectTouching && this->m_selectedMode == 3) {

		if (ErGui::ccTouchEnded_Selection(this, touch)) {
			return EditorUI::ccTouchEnded(touch, event);
		}

		// Disable Rotation/Scale/Transform controls when clicked on empty space
		if (Mod::get()->getSavedValue<bool>("deselect-controls", false)) {
			GameManager::sharedState()->setGameVariable("0007", false);
			rotationControl->setVisible(false);
			scaleControl->setVisible(false);
			transformControl->setVisible(false);
		}

			
		return EditorUI::ccTouchEnded(touch, event);
	}


	EditorUI::ccTouchEnded(touch, event);
}