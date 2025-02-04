#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>

#include <Geode/modify/CustomSongLayer.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <IconsMaterialDesignIcons.h>
#include "modules/EditGroupModule.hpp"
#include "modules/SelectFilterModule.hpp"
#include "modules/TransformObjectModule.hpp"
#include "modules/FooterModule.hpp"
#include "modules/SelectModule.hpp"

using namespace geode::prelude;

template <class R, class T>
R& from(T base, intptr_t offset) {
	return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
}

bool filterSingleObj(bool filterBool, int objParameter, std::vector<int> vec) {
	if (!filterBool) return true;
	for (int i = 0; i < vec.size(); i++) {
		if (objParameter == vec[i])
			return true;
	}
	return false;
}

class $modify(EditorUI) {
	void selectObject(GameObject* obj, bool p1) {
		if ((!ErGui::objectsFilterArr.empty() && ErGui::filterByObjects)) {
			for (int i = 0; i < ErGui::objectsFilterArr.size(); i++) {
				if (obj->m_objectID == ErGui::objectsFilterArr[i]) {
					EditorUI::selectObject(obj, p1);
					return;
				}
			}
			if (!ErGui::filterOr) return;
		}

		if ((!ErGui::mainColorsFilterArr.empty() && ErGui::filterByMainColors)) {
			for (int i = 0; i < ErGui::mainColorsFilterArr.size(); i++) {
				if (obj->m_activeMainColorID == ErGui::mainColorsFilterArr[i]) {
					EditorUI::selectObject(obj, p1);
					return;
				}
			}
			return;
		}

		if ((!ErGui::groupsFilterArr.empty() && ErGui::filterByGroups)) {
			if (!obj->m_groups) return;
			for (int i = 0; i < ErGui::groupsFilterArr.size(); i++) {
				for (int j = 0; j < obj->m_groups->size(); j++) {
					if (obj->m_groups->at(j) == ErGui::groupsFilterArr[i]) {
						EditorUI::selectObject(obj, p1);
						return;
					}
				}
			}
			return;
		}


		//if ((!objectsFilterArr.empty() && filterByObjects) ||
		//	(!mainColorsFilterArr.empty() && filterByMainColors)) {
		//	if (!filterOr) {
		//		if (filterSingleObj(filterByObjects, obj->m_objectID, objectsFilterArr) &&
		//			filterSingleObj(filterByMainColors, obj->m_activeMainColorID, mainColorsFilterArr))
		//			EditorUI::selectObject(obj, p1);
		//	}
		//	else {
		//		if (filterSingleObj(filterByObjects, obj->m_objectID, objectsFilterArr) ||
		//			filterSingleObj(filterByMainColors, obj->m_activeMainColorID, mainColorsFilterArr))
		//			EditorUI::selectObject(obj, p1);
		//	}
		//	return;
		//}
		EditorUI::selectObject(obj, p1);
	}

	void selectObjects(CCArray* objArrInRect, bool p1) {
		
		
		//orig
		EditorUI::selectObjects(objArrInRect, p1);

		auto objArr = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObjects;

		ErGui::groupsFromObjArr.clear();

		ErGui::firstObj = static_cast<GameObject*>(objArr->objectAtIndex(0));
		ErGui::firstEgObj = nullptr;

		ErGui::minEl1 = ErGui::firstObj->m_editorLayer;
		ErGui::maxEl1 = ErGui::firstObj->m_editorLayer;
		ErGui::minEl2 = ErGui::firstObj->m_editorLayer2;
		ErGui::maxEl2 = ErGui::firstObj->m_editorLayer2;
		ErGui::minZOrder = ErGui::firstObj->m_zOrder;
		ErGui::maxZOrder = ErGui::firstObj->m_zOrder;
		ErGui::minZLayer = *reinterpret_cast<int*>(&ErGui::firstObj->m_zLayer);
		ErGui::maxZLayer = *reinterpret_cast<int*>(&ErGui::firstObj->m_zLayer);
		ErGui::minEnterChannel = ErGui::firstObj->m_enterChannel;
		ErGui::maxEnterChannel = ErGui::firstObj->m_enterChannel;
		ErGui::minMaterial = ErGui::firstObj->m_objectMaterial;
		ErGui::maxMaterial = ErGui::firstObj->m_objectMaterial;


		//WELL FUCK ME!
		//---Cosmetic
		ErGui::cb_NoEffects = false;
		ErGui::cb_DontFade = false;
		ErGui::cb_NoGlow = false;
		ErGui::cb_DontEnter = false;
		ErGui::cb_NoParticle = false;
		ErGui::cb_HighDetail = false;
		ErGui::cb_NoAudioScale = false;
		ErGui::cb_Hide = false;

		//---Parent & Physics
		ErGui::cb_GroupParent = false;
		ErGui::cb_NoTouch = false;
		ErGui::cb_AreaParent = false;
		ErGui::cb_Passable = false;
		ErGui::cb_DontBoostX = false;
		ErGui::cb_ExtendedCollision = false;
		ErGui::cb_DontBoostY = false;

		//---Platformer Only
		ErGui::cb_IceBlock = false;
		ErGui::cb_GripSlope = false;
		ErGui::cb_NonStickX = false;
		ErGui::cb_ExtraSticky = false;
		ErGui::cb_NonStickY = false;
		ErGui::cb_ScaleStick = false;

		//---Effect Game Object
		ErGui::cb_SinglePTouch = false;
		ErGui::cb_Preview = false;
		ErGui::cb_CenterEffect = false;
		ErGui::cb_Reverse = false;

		std::unordered_set<int> groupSet;

		for (auto obj : CCArrayExt<GameObject*>(objArr)) {
			if (obj->m_groups) {		
				for (int i = 0; i < obj->m_groups->size(); i++) {
					groupSet.insert(obj->m_groups->at(i));
				}
			}

			int el1 = obj->m_editorLayer;
			if (el1 < ErGui::minEl1) ErGui::minEl1 = el1;
			if (el1 > ErGui::maxEl1) ErGui::maxEl1 = el1;

			int el2 = obj->m_editorLayer2;
			if (el2 < ErGui::minEl2) ErGui::minEl2 = el2;
			if (el2 > ErGui::maxEl2) ErGui::maxEl2 = el2;

			int zOrder = obj->m_zOrder;
			if (zOrder < ErGui::minZOrder) ErGui::minZOrder = zOrder;
			if (zOrder > ErGui::maxZOrder) ErGui::maxZOrder = zOrder;

			int* zLayer = reinterpret_cast<int*>(&obj->m_zLayer);
			if (*zLayer < ErGui::minZLayer) ErGui::minZLayer = *zLayer;
			if (*zLayer > ErGui::maxZLayer) ErGui::maxZLayer = *zLayer;

			int enterChannel = obj->m_enterChannel;
			if (enterChannel < ErGui::minEnterChannel) ErGui::minEnterChannel = enterChannel;
			if (enterChannel > ErGui::maxEnterChannel) ErGui::maxEnterChannel = enterChannel;

			int objectMaterial = obj->m_objectMaterial;
			if (objectMaterial < ErGui::minMaterial) ErGui::minMaterial = objectMaterial;
			if (objectMaterial > ErGui::maxMaterial) ErGui::maxMaterial = objectMaterial;


			if (obj->m_hasNoEffects)		ErGui::cb_NoEffects = true;
			if (obj->m_isDontFade)			ErGui::cb_DontFade = true;
			if (obj->m_hasNoGlow)			ErGui::cb_NoGlow = true;
			if (obj->m_isDontEnter)			ErGui::cb_DontEnter = true;
			if (obj->m_hasNoParticles)		ErGui::cb_NoParticle = true;
			if (obj->m_isHighDetail)		ErGui::cb_HighDetail = true;
			if (obj->m_hasNoAudioScale)		ErGui::cb_NoAudioScale = true;
			if (obj->m_isHide)				ErGui::cb_Hide = true;

			if (obj->m_hasGroupParent)			ErGui::cb_GroupParent = true;
			if (obj->m_isNoTouch)				ErGui::cb_NoTouch = true;
			if (obj->m_hasAreaParent)			ErGui::cb_AreaParent = true;
			if (obj->m_isPassable)				ErGui::cb_Passable = true;
			if (obj->m_isDontBoostX)			ErGui::cb_DontBoostX = true;
			if (obj->m_hasExtendedCollision)	ErGui::cb_ExtendedCollision = true;
			if (obj->m_isDontBoostY)			ErGui::cb_DontBoostY = true;

			if (obj->m_isIceBlock)		ErGui::cb_IceBlock = true;
			if (obj->m_isGripSlope)		ErGui::cb_GripSlope = true;
			if (obj->m_isNonStickX)		ErGui::cb_NonStickX = true;
			if (obj->m_isExtraSticky)	ErGui::cb_ExtraSticky = true;
			if (obj->m_isNonStickY)		ErGui::cb_NonStickY = true;
			if (obj->m_isScaleStick)	ErGui::cb_ScaleStick = true;

			if (dynamic_cast<EffectGameObject*>(obj)) {
				ErGui::firstEgObj = dynamic_cast<EffectGameObject*>(obj);
				if (ErGui::firstEgObj->m_isSinglePTouch)	ErGui::cb_SinglePTouch = true;
				if (ErGui::firstEgObj->m_shouldPreview)		ErGui::cb_Preview = true;
				if (ErGui::firstEgObj->m_hasCenterEffect)	ErGui::cb_CenterEffect = true;
				if (ErGui::firstEgObj->m_isReverse)			ErGui::cb_Reverse = true;
			}
		}

		if (auto obj = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObject) {
			if (obj->m_groups) {
				for (int i = 0; i < obj->m_groups->size(); i++) {
					groupSet.insert(obj->m_groups->at(i));
				}
			}
		}

		ErGui::groupsFromObjArr.assign(groupSet.begin(), groupSet.end());
		std::sort(ErGui::groupsFromObjArr.begin(), ErGui::groupsFromObjArr.end());
		if (ErGui::groupsFromObjArr.size() > 0)
			ErGui::groupsFromObjArr.erase(ErGui::groupsFromObjArr.begin());
		//for (int i = 0; i < ErGui::groupsFromObjArr.size(); i++) {
		//	std::cout << ErGui::groupsFromObjArr[i] << " ";
		//}
		//std::cout << "\n";
		

		//if (ErGui::filterByObjects && !ErGui::objectsFilterArr.empty()) {
		//	CCArray* newArr = CCArray::create();
		//	for (int i = 0; i < objArr->count(); i++) {
		//		for (int j = 0; j < ErGui::objectsFilterArr.size(); j++) {
		//			if (static_cast<GameObject*>(objArr->objectAtIndex(i))->m_objectID == ErGui::objectsFilterArr[j])
		//				newArr->addObject(objArr->objectAtIndex(i));
		//		}
		//	}
		//	EditorUI::selectObjects(newArr, p1);
		//}
		//else
		//	EditorUI::selectObjects(objArr, p1);
	}

	bool init(LevelEditorLayer* lel) {
		ErGui::editorUIDrawNode = CCDrawNode::create();
		this->addChild(ErGui::editorUIDrawNode);

		return EditorUI::init(lel);
	}

	bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
		if (this->m_selectedMode == 3 && ErGui::isLassoEnabled) {
			ErGui::editorUILassoPoints.clear();
			CCPoint pt = touch->getLocation();
			ErGui::editorUILassoPoints.push_back(pt);
			ErGui::editorUIDrawNode->clear();

			//this->m_unk244 = true;
		}
		return EditorUI::ccTouchBegan(touch, event);
	}

	void ccTouchMoved(CCTouch* touch, CCEvent* event) {
		if (this->m_selectedMode == 3 && ErGui::isLassoEnabled) {
			CCPoint pt = touch->getLocation();
			ErGui::editorUILassoPoints.push_back(pt);
			ErGui::editorUIDrawNode->clear();

			if (ErGui::editorUILassoPoints.size() > 1) {
				ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUILassoPoints.data(), ErGui::editorUILassoPoints.size(), { 0, 0, 0, 0 }, 0.2f, { 0, 0.7f, 0, 1.f });
			}
		}
		EditorUI::ccTouchMoved(touch, event);
	}

	void ccTouchEnded(CCTouch* touch, CCEvent* event) {
		if (ErGui::editorUILassoPoints.size() > 2 && this->m_selectedMode == 3 && ErGui::isLassoEnabled) {
			ErGui::editorUIDrawNode->clear();
			ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUILassoPoints.data(), ErGui::editorUILassoPoints.size(), { 0, 0, 0, 0 }, 0.2f, { 0, 1.f, 0, 1.f });

			CCArray* objArr = CCArray::create();
			for (auto obj : CCArrayExt<GameObject*>(GameManager::sharedState()->getEditorLayer()->m_objects)) {

				auto editorLayer = GameManager::sharedState()->getEditorLayer();
				auto cameraPos = editorLayer->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();
				auto cameraScale = editorLayer->getChildByID("main-node")->getChildByID("batch-layer")->getScale();

				auto objPos = obj->getPosition();
				auto newPos = cocos2d::CCPoint(
					(objPos.x * cameraScale) + cameraPos.x,
					(objPos.y * cameraScale) + cameraPos.y
				);

				//std::cout << objPos.x << " " << objPos.y << "\n";
				//std::cout << newPos.x << " " << newPos.y << "\n\n";

				if (ErGui::isPointInPolygon(newPos, ErGui::editorUILassoPoints)) {
					objArr->addObject(obj);
				}
			}
			//std::cout << "\n";
			if (objArr->count() > 0)
				this->selectObjects(objArr, false);
			CC_SAFE_RELEASE(objArr);
			ErGui::editorUIDrawNode->clear();
		}

		EditorUI::ccTouchEnded(touch, event);
	}

	void destructor() {
		CC_SAFE_RELEASE(ErGui::editorUIDrawNode);
		ErGui::editorUIDrawNode = nullptr;
		EditorUI::~EditorUI();
	}
};

//class $modify(CustomizeObjectLayer) {
//	void onSelectColor(CCObject * sender) {
//		CustomizeObjectLayer::onSelectColor(sender);
//		if (GameObject* obj = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObject) {
//			if (from<int>(this, 0x2d0) == 1) {
//				obj->m_activeMainColorID = from<int>(this, 0x2d4);
//			}
//			else if (from<int>(this, 0x2d0) == 2) {
//				obj->m_activeDetailColorID = from<int>(this, 0x2d4);
//			}
//		}
//	}
//};




$on_mod(Loaded) {
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

	ImGuiCocos::get().setup([] {
		ImGuiIO& io = ImGui::GetIO();

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF("Karla-Regular.ttf", 16.f);
		io.Fonts->Build();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}).draw([] {

			if (GameManager::sharedState()->getEditorLayer()) {
				ErGui::renderEditGroupModule();
				ErGui::renderSelectFilter();
				ErGui::renderTransformation();
				ErGui::renderFooter();
				ErGui::renderSelectModule();
			}
			});
}