#include "Selection.hpp"
#include "Hovering.hpp"

using namespace geode::prelude;

namespace ErGui {
	CCArray* selectFilterRealization(CCArray* objArr);
	bool selectFilterRealization(GameObject* obj);
}


void selectObjectsWithFilter(CCArray* objArr, bool p1) {
	CCArray* sfr = ErGui::selectFilterRealization(objArr);
	EditorUI::get()->selectObjects(sfr, p1);
}

void selectObjectWithFilter(GameObject* obj, bool p1) {
	if (ErGui::selectFilterRealization(obj)) {
		EditorUI::get()->selectObject(obj, p1);
	}
}

CCArray* selectWithType(CCArray* selectedObjects, CCArray* deltaObjects) {
	int selectMode = Mod::get()->getSavedValue<int>("select-mode", 1);
	auto editorUI = EditorUI::get();
	CCArray* returnArr = CCArray::create();

	switch (selectMode) {
	case 3: { // Intersective
		if (deltaObjects->count() == 0) return returnArr;
		for (const auto& obj : CCArrayExt<GameObject*>(deltaObjects)) {
			if (selectedObjects->containsObject(obj)) {
				returnArr->addObject(obj);
			}
		}
		break;
	}
	case 2: { // Subtractive
		if (deltaObjects->count() == 0) return selectedObjects;
		returnArr->addObjectsFromArray(selectedObjects);
		for (const auto& obj : CCArrayExt<GameObject*>(deltaObjects)) {
			if (returnArr->containsObject(obj)) {
				if (int lGroup = obj->m_linkedGroup) {
					auto linkedArr = editorUI->m_editorLayer->getStickyGroup(lGroup);
					for (auto lObj : CCArrayExt<GameObject*>(linkedArr)) {
						returnArr->fastRemoveObject(lObj);
					}
				}
				else {
					returnArr->fastRemoveObject(obj);
				}
			}
		}
		break;
	}
	case 1: // Additive
	default: {
		if (deltaObjects->count() == 0) return selectedObjects;
		returnArr->addObjectsFromArray(selectedObjects);
		for (auto obj : CCArrayExt<GameObject*>(deltaObjects)) {
			if (!returnArr->containsObject(obj)) {
				returnArr->addObject(obj);
			}
		}
		break;
	}
	}

	return returnArr;
}

void rawDeselectAll() {
	EditorUI* editorUI = EditorUI::get();
	if (!editorUI) return;

	auto selectedObject = editorUI->m_selectedObject;
	auto selectedObjects = editorUI->m_selectedObjects;

	if (selectedObject) {
		selectedObject->deselectObject();
		editorUI->m_selectedObject = nullptr;
	}

	if (selectedObjects && selectedObjects->count() > 0) {
		for (auto obj : CCArrayExt<GameObject*>(selectedObjects)) {
			obj->deselectObject();
			selectedObjects->fastRemoveObject(obj);
		}
	}
}

void manualObjectsSelect(CCArray* objsInShape) {
	int selectMode = Mod::get()->getSavedValue<int>("select-mode", 1);
	auto editorUI = EditorUI::get();

	switch (selectMode) {
	case 3: { // Intersective
		CCArray* objArr = CCArray::create();

		for (const auto& obj : CCArrayExt<GameObject*>(objsInShape)) {
			if (obj->m_isSelected) {
				objArr->addObject(obj);
			}
		}
		editorUI->deselectAll();
		if (objArr->count() > 0) {
			selectObjectsWithFilter(objArr, false);
		}
		break;
	}
	case 2: { // Subtractive
		CCArray* sfr = ErGui::selectFilterRealization(objsInShape);
		for (auto obj : CCArrayExt<GameObject*>(sfr)) {
			editorUI->deselectObject(obj);
		}
		if (editorUI->m_selectedObjects->count() == 1) {
			editorUI->m_selectedObject = static_cast<GameObject*>(editorUI->m_selectedObjects->objectAtIndex(0));
			editorUI->m_selectedObjects->removeAllObjects();
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
void ErGui::ccTouchBegan_LassoAndVanillaSwipe(GearEditorUI* editorUI, CCTouch* touch) {
	ErGui::editorUISwipePoints.clear();
	CCPoint pt = touch->getLocation();
	ErGui::editorUISwipePoints.push_back(pt);
	ErGui::editorUIDrawNode->clear();
}

void ErGui::ccTouchMoved_Lasso(GearEditorUI* editorUI, CCTouch* touch, ccColor4F selectionFillColor, ccColor4F lassoColor) {
	CCPoint pt = touch->getLocation();
	ErGui::editorUISwipePoints.push_back(pt);
	ErGui::editorUIDrawNode->clear();

	if (ErGui::editorUISwipePoints.size() > 1) {
		ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUISwipePoints.data(), ErGui::editorUISwipePoints.size(), selectionFillColor, 0.3f, lassoColor);
	}
	if (geode::Mod::get()->getSavedValue<bool>("hovering-selects", true))
		ErGui::forEachObject(editorUI->m_editorLayer, ErGui::hoverObjectLasso);
}

void ErGui::ccTouchMoved_VanillaSwipe(GearEditorUI* editorUI, CCTouch* touch, ccColor4F selectionFillColor, ccColor4F lassoColor) {
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
	if (geode::Mod::get()->getSavedValue<bool>("hovering-selects", true)) {
		ErGui::forEachObject(editorUI->m_editorLayer, ErGui::hoverObjectSquare);
	}
}


bool ErGui::ccTouchEnded_Selection(GearEditorUI* editorUI, CCTouch* touch) {
	// --- Selection Begin ---
	auto lel = LevelEditorLayer::get();
	auto currentLayer = lel->m_currentLayer;
	bool swiping = editorUI->m_swipeEnabled || GearEditorUI::get()->m_fields->m_wasShifting;

	// --- Lasso ccTouchEnded
	if (ErGui::editorUISwipePoints.size() > 2 && ErGui::isLassoEnabled && swiping) {

		auto selectedObjects = editorUI->m_selectedObjects;
		auto selectedObject = editorUI->m_selectedObject;
		editorUI->createUndoSelectObject(false);

		// Gonna be selected (or deselected... delta is the best word)
		CCArray* deltaObjects = CCArray::create();
		for (auto obj : CCArrayExt<GameObject*>(lel->m_objects)) {
			if (!(obj->m_editorLayer == currentLayer || (obj->m_editorLayer2 == currentLayer && obj->m_editorLayer2 != 0) || currentLayer == -1)
				|| !ErGui::selectFilterRealization(obj)) continue;

			auto editorLayer = GameManager::sharedState()->getEditorLayer();
			auto cameraPos = editorLayer->m_objectLayer->getPosition();
			auto cameraScale = editorLayer->m_objectLayer->getScale();

			auto objPos = obj->getPosition();
			auto newPos = cocos2d::CCPoint(
				(objPos.x * cameraScale) + cameraPos.x,
				(objPos.y * cameraScale) + cameraPos.y
			);

			if (ErGui::isPointInPolygon(newPos, ErGui::editorUISwipePoints)) {
				deltaObjects->addObject(obj);
			}
		}

		// Was selected before
		CCArray* accurateSelectedObjects = CCArray::create();
		if (selectedObject)
			accurateSelectedObjects->addObject(selectedObject);
		if (selectedObjects && selectedObjects->count() > 0)
			accurateSelectedObjects->addObjectsFromArray(selectedObjects);


		if (deltaObjects->count() == 0) {
			lel->m_undoObjects->removeLastObject();
			return true;
		}

		// Basically this would be selected after all calculations
		CCArray* finalSelectionObjects = selectWithType(accurateSelectedObjects, deltaObjects);
		rawDeselectAll();
		editorUI->selectObjects(finalSelectionObjects, false);

		if (ErGui::compareCCArrays(finalSelectionObjects, accurateSelectedObjects)) {
			lel->m_undoObjects->removeLastObject();
		}

		return true;
	}
	// --- Vanilla Swipe ccTouchEnded
	else if (swiping && ErGui::selectRect) {
		auto selectedObjects = editorUI->m_selectedObjects;
		auto selectedObject = editorUI->m_selectedObject;
		editorUI->createUndoSelectObject(false);

		// Gonna be selected (or deselected... delta is the best word)
		CCArray* deltaObjects = CCArray::create();
		for (auto obj : CCArrayExt<GameObject*>(lel->m_objects)) {
			if (ErGui::isObjectGonnaBeSelected(obj)) {
				deltaObjects->addObject(obj);
			}
		}

		// Resetting select rect
		ErGui::selectRect.reset();

		// Was selected before
		CCArray* accurateSelectedObjects = CCArray::create();
		if (selectedObject)
			accurateSelectedObjects->addObject(selectedObject);
		if (selectedObjects && selectedObjects->count() > 0)
			accurateSelectedObjects->addObjectsFromArray(selectedObjects);


		if (deltaObjects->count() == 0) {
			lel->m_undoObjects->removeLastObject();
			return true;
		}

		// Basically this would be selected after all calculations
		CCArray* finalSelectionObjects = selectWithType(accurateSelectedObjects, deltaObjects);
		rawDeselectAll();
		editorUI->selectObjects(finalSelectionObjects, false);

		if (ErGui::compareCCArrays(finalSelectionObjects, accurateSelectedObjects)) {
			lel->m_undoObjects->removeLastObject();
		}

		return true;
	}
	// --- Single Touch ccTouchEnded
	else if (
		ErGui::beginTouchLocation.x >= touch->getLocation().x - 20.f &&
		ErGui::beginTouchLocation.x <= touch->getLocation().x + 20.f &&
		ErGui::beginTouchLocation.y >= touch->getLocation().y - 20.f &&
		ErGui::beginTouchLocation.y <= touch->getLocation().y + 20.f
		) {
		//auto singleSelectedObject = this->m_selectedObject;

		
		//auto obj = lel->objectAtPosition(touchConverted);

		//ErGui::forEachObject(lel, );

		//if (obj && ErGui::selectFilterRealization(obj)) {
		//	EditorUI::get()->selectObject(obj, false);
		//}


		auto objArr = editorUI->objectsAtPosition(touch->getLocation());


		// Rolling selections
		CCArray* lastUnderCursor = editorUI->m_fields->m_lastUnderCursor;
		int* lucIndex = &editorUI->m_fields->m_lastUnderCursorIndex;

		if (int arrCount = objArr->count()) {
			auto objToSelect = editorUI->objectAtPosition(objArr);

			// Clicking with Swipe/Shift or without
			if (swiping) {
				if (!editorUI->m_selectedObjects->containsObject(objToSelect)) {
					editorUI->createUndoSelectObject(false);
					auto objToSelectArr = CCArray::create();
					objToSelectArr->addObject(objToSelect);
					editorUI->selectObjects(objToSelectArr, false);
				}
			}
			else if (editorUI->m_selectedObject != objToSelect || !editorUI->m_selectedObjects->containsObject(objToSelect)) {
				editorUI->createUndoSelectObject(false);
				editorUI->selectObject(objToSelect, false);
			}
		}
		else {
			lastUnderCursor->removeAllObjects();
		}
	}

	return false;
}