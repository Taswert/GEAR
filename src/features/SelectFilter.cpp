#include "SelectFilter.hpp"


bool ErGui::selectFilterRealization(GameObject* obj) {
	bool isFilterOn = false;
	bool shouldSelect = false;

	if (!ErGui::objectsFilterSet.empty() && ErGui::filterByObjects) {
		isFilterOn = true;
		if (ErGui::objectsFilterSet.contains(obj->m_objectID)) {
			shouldSelect = true;
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}

	if (!ErGui::mainColorsFilterSet.empty() && ErGui::filterByMainColors) {
		isFilterOn = true;
		if (obj->m_baseColor) {
			if (ErGui::mainColorsFilterSet.contains(obj->m_baseColor->m_colorID)) {
				shouldSelect = true;
			}
			else if (ErGui::filterModifier == 1) {
				return false;
			}
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}

	if (!ErGui::detailColorsFilterSet.empty() && ErGui::filterByDetailColors) {
		isFilterOn = true;
		if (obj->m_detailColor) {
			if (ErGui::detailColorsFilterSet.contains(obj->m_detailColor->m_colorID)) {
				shouldSelect = true;
			}
			else if (ErGui::filterModifier == 1) {
				return false;
			}
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}

	if (!ErGui::groupsFilterSet.empty() && ErGui::filterByGroups) {
		isFilterOn = true;
		if (obj->m_groups) {
			bool foundGroup = false;
			for (int i = 0; i < obj->m_groups->size(); i++) {
				if (ErGui::groupsFilterSet.contains(obj->m_groups->at(i))) {
					shouldSelect = true;
					foundGroup = true;
				}
			}
			if (!foundGroup && ErGui::filterModifier == 1) {
				return false;
			}
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}

	if (!ErGui::channelFilterSet.empty() && ErGui::filterByChannel) {
		isFilterOn = true;
		if (auto egObj = typeinfo_cast<EffectGameObject*>(obj)) {
			if (ErGui::channelFilterSet.contains(egObj->m_channelValue)) {
				shouldSelect = true;
			}
			else if (ErGui::filterModifier == 1) {
				return false;
			}
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}

	if (!ErGui::objectTypeSet.empty() && ErGui::filterByType) {
		isFilterOn = true;
		if (ErGui::objectTypeSet.contains((int)obj->m_objectType)) {
			shouldSelect = true;
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}

	if (!isFilterOn)
		return true; // EditorUI::selectObject(obj, p1);
	else if (shouldSelect)
		return true; // EditorUI::selectObject(obj, p1);
	else
		return false;
}

CCArray* ErGui::selectFilterRealization(CCArray* objArrInRect) {

	CCArray* shouldSelectArr = CCArray::create();
	for (auto obj : CCArrayExt<GameObject*>(objArrInRect)) {

		bool isFilterOn = false;
		bool shouldSelect = false;

		if (!ErGui::objectsFilterSet.empty() && ErGui::filterByObjects) {
			isFilterOn = true;
			if (ErGui::objectsFilterSet.contains(obj->m_objectID)) {
				shouldSelect = true;
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}

		if (!ErGui::mainColorsFilterSet.empty() && ErGui::filterByMainColors) {
			isFilterOn = true;
			if (obj->m_baseColor) {
				if (ErGui::mainColorsFilterSet.contains(obj->m_baseColor->m_colorID)) {
					shouldSelect = true;
				}
				else if (ErGui::filterModifier == 1) {
					continue;
				}
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}

		if (!ErGui::detailColorsFilterSet.empty() && ErGui::filterByDetailColors) {
			isFilterOn = true;
			if (obj->m_detailColor) {
				if (ErGui::detailColorsFilterSet.contains(obj->m_detailColor->m_colorID)) {
					shouldSelect = true;
				}
				else if (ErGui::filterModifier == 1) {
					continue;
				}
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}

		if (!ErGui::groupsFilterSet.empty() && ErGui::filterByGroups) {
			isFilterOn = true;
			if (obj->m_groups) {
				bool foundGroup = false;
				for (int i = 0; i < obj->m_groups->size(); i++) {
					if (ErGui::groupsFilterSet.contains(obj->m_groups->at(i))) {
						shouldSelect = true;
						foundGroup = true;
					}
				}
				if (!foundGroup && ErGui::filterModifier == 1) {
					continue;
				}
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}

		if (!ErGui::channelFilterSet.empty() && ErGui::filterByChannel) {
			isFilterOn = true;
			if (auto egObj = typeinfo_cast<EffectGameObject*>(obj)) {
				if (ErGui::channelFilterSet.contains(egObj->m_channelValue)) {
					shouldSelect = true;
				}
				else if (ErGui::filterModifier == 1) {
					continue;
				}
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}

		if (!ErGui::objectTypeSet.empty() && ErGui::filterByType) {
			isFilterOn = true;
			if (ErGui::objectTypeSet.contains((int)obj->m_objectType)) {
				shouldSelect = true;
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}


		if (!isFilterOn)
			return objArrInRect;
		else if (shouldSelect)
			shouldSelectArr->addObject(obj);
		else
			continue;
	}

	return shouldSelectArr;
}