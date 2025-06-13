#include "EditGroupModule.hpp"
using namespace ErGui;

const char* layerTypeItems[] = {
	"Bottom 5", "Bottom 4", "Bottom 3", "Bottom 2", "Bottom 1",
	"Top 1", "Top 2", "Top 3", "Top 4",
	"Default"
};

const int layerIntItems[] = {
	-5, -3, -1, 1, 3,
	5, 7, 9, 11,
	0
};


void ErGui::CopyEGMState::copyState(GameObject* obj) {
	if (obj->m_groups) {
		for (int i = 0; i < 10; i++) {
			groups[i] = obj->m_groups->at(i);
		}
	}

	editorL1 = obj->m_editorLayer;
	editorL2 = obj->m_editorLayer2;
	zOrder = obj->m_zOrder;
	zLayer = obj->m_zLayer;

	enterChannel = obj->m_enterChannel;
	material = obj->m_objectMaterial;

	NoEffects = obj->m_hasNoEffects;
	DontFade = obj->m_isDontFade;
	NoGlow = obj->m_hasNoGlow;
	DontEnter = obj->m_isDontEnter;
	NoParticle = obj->m_hasNoParticles;
	HighDetail = obj->m_isHighDetail;
	NoAudioScale = obj->m_hasNoAudioScale;
	Hide = obj->m_isHide;

	GroupParent = obj->m_hasGroupParent;
	NoTouch = obj->m_isNoTouch;
	AreaParent = obj->m_hasAreaParent;
	Passable = obj->m_isPassable;
	DontBoostX = obj->m_isDontBoostX;
	ExtendedCollision = obj->m_hasExtendedCollision;
	DontBoostY = obj->m_isDontBoostY;

	IceBlock = obj->m_isIceBlock;
	GripSlope = obj->m_isGripSlope;
	NonStickX = obj->m_isNonStickX;
	ExtraSticky = obj->m_isExtraSticky;
	NonStickY = obj->m_isNonStickY;
	ScaleStick = obj->m_isScaleStick;

	if (auto egObj = dynamic_cast<EffectGameObject*>(obj)) {
		ego_orderVal = egObj->m_ordValue;
		ego_channelVal = egObj->m_channelValue;
		ego_controlID = egObj->m_controlID;

		ego_SinglePTouch = egObj->m_isSinglePTouch;
		ego_Preview = egObj->m_shouldPreview;
		ego_CenterEffect = egObj->m_hasCenterEffect;
		ego_Reverse = egObj->m_isReverse;
	}
}

void ErGui::CopyEGMState::pasteState(GameObject* obj) {
	if (obj->m_groups) {
		for (int i = 0; obj->m_groups->at(0) != 0; i++) {
			std::cout << obj->m_groups->at(0) << " ";
			obj->removeFromGroup(obj->m_groups->at(0));
		}
	}
	
	for (int i = 0; i < 10; i++) {
		obj->addToGroup(groups[i]);
	}

	obj->m_editorLayer = editorL1;
	obj->m_editorLayer2 = editorL2;
	obj->m_zOrder = zOrder;
	obj->m_zLayer = zLayer;

	obj->m_enterChannel = enterChannel;
	obj->m_objectMaterial = material;

	obj->m_hasNoEffects = NoEffects;
	obj->m_isDontFade = DontFade;
	obj->m_hasNoGlow = NoGlow;
	obj->m_isDontEnter = DontEnter;
	obj->m_hasNoParticles = NoParticle;
	obj->m_isHighDetail = HighDetail;
	obj->m_hasNoAudioScale = NoAudioScale;
	obj->m_isHide = Hide;

	obj->m_hasGroupParent = GroupParent;
	obj->m_isNoTouch = NoTouch;
	obj->m_hasAreaParent = AreaParent;
	obj->m_isPassable = Passable;
	obj->m_isDontBoostX = DontBoostX;
	obj->m_hasExtendedCollision = ExtendedCollision;
	obj->m_isDontBoostY = DontBoostY;

	obj->m_isIceBlock = IceBlock;
	obj->m_isGripSlope = GripSlope;
	obj->m_isNonStickX = NonStickX;
	obj->m_isExtraSticky = ExtraSticky;
	obj->m_isNonStickY = NonStickY;
	obj->m_isScaleStick = ScaleStick;

	if (auto egObj = dynamic_cast<EffectGameObject*>(obj)) {
		egObj->m_ordValue = ego_orderVal;
		egObj->m_channelValue = ego_channelVal;
		egObj->m_controlID = ego_controlID;

		egObj->m_isSinglePTouch = ego_SinglePTouch;
		egObj->m_shouldPreview = ego_Preview;
		egObj->m_hasCenterEffect = ego_CenterEffect;
		egObj->m_isReverse = ego_Reverse;
	}
}

void ErGui::CopyEGMState::pasteState(CCArray* objArr) {
	for (auto obj : CCArrayExt<GameObject*>(objArr)) {
		this->pasteState(obj);
	}
}

void renderForObject(GameObject* obj, LevelEditorLayer* lel) {
	if (ImGui::Button("Copy##COPYSTATE")) {
		copyStateObject.copyState(obj);
	}
	ImGui::SameLine();
	if (ImGui::Button("Paste##PASTESTATE")) {
		copyStateObject.pasteState(obj);
	}
	ImGui::SameLine();
	if (ImGui::Button("Go To Layer")) {
		LevelEditorLayer::get()->m_currentLayer = obj->m_editorLayer;
	}

	if (ImGui::CollapsingHeader("-----| Object Info |-----")) {
		ImGui::Text("Object Address: %p", obj);
		ImGui::SameLine();
		if (ImGui::Button("Copy")) {
			clipboard::write(CCString::createWithFormat("%p", obj)->getCString());
		}

		ImGui::Text("Object ID: %d", obj->m_objectID);

		//ImGui::Text(typeid(obj).name());	
	}
	if (ImGui::CollapsingHeader("-----| Groups Settings |-----")) {
		ImGui::PushItemWidth(150.0f);
		ImGui::InputInt("ID Offset", &groupOffset);
		setMaxMin(groupOffset, 9999, 1);
		ImGui::PushItemWidth(150.0f);
		ImGui::InputInt("GroupID", &chosenGroupEGM);
		setMaxMin(chosenGroupEGM, 9999, 1);


		bool addGroupCheck = true;
		if (obj->m_groups) {
			addGroupCheck = std::find(obj->m_groups->begin(), obj->m_groups->end(), 0) != obj->m_groups->end() && 
				(!static_cast<CCArray*>(lel->m_groups[chosenGroupEGM]) || !static_cast<CCArray*>(lel->m_groups[chosenGroupEGM])->containsObject(obj));
		}

		if (ImGui::Button("Add##EGM-ADDGROUP")) {
			if (addGroupCheck) {
				if (!lel->m_groups[chosenGroupEGM]) {
					CCArray* arr = CCArray::create();
					arr->retain();								//I hope this didn't make some memory leaks...
					lel->m_groups[chosenGroupEGM] = arr;
				}
				static_cast<CCArray*>(lel->m_groups[chosenGroupEGM])->addObject(obj);
				obj->addToGroup(chosenGroupEGM);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("AddP##EGM-ADDGROUPPARENT")) {
			if (addGroupCheck) {
				if (!lel->m_groups[chosenGroupEGM]) {
					CCArray* arr = CCArray::create();
					arr->retain();
					lel->m_groups[chosenGroupEGM] = arr;
				}
				static_cast<CCArray*>(lel->m_groups[chosenGroupEGM])->addObject(obj);
				obj->addToGroup(chosenGroupEGM);
				lel->setGroupParent(obj, chosenGroupEGM);
			}
			else if (std::find(obj->m_groups->begin(), obj->m_groups->end(), 0) != obj->m_groups->end() &&
				static_cast<CCArray*>(lel->m_groups[chosenGroupEGM])->containsObject(obj)) {
				lel->setGroupParent(obj, chosenGroupEGM);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Next Free##EGM-NEXTFREEGROUP")) {
			auto groups = lel->m_groups;
			if (groupOffset < 1) groupOffset = 1;
			for (int i = groupOffset; i < groups.size(); i++) {
				int count = groups[i] ? groups[i]->count() : 0;
				if (count == 0) {
					chosenGroupEGM = i;
					break;
				}
			}
		}

		ImGui::PushStyleColor(ImGuiCol_Separator, { 0.33f, 0.33f, 0.33f, 1.f });
		ImGui::SeparatorText("Groups");
		ImGui::PopStyleColor();

		//ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
		for (int i = 0; i < obj->m_groupCount; i++) {
			int groupInt = obj->m_groups->at(i);
			std::string btnStr = std::to_string(groupInt);
			btnStr += "##RMVGROUP";

			if (lel->m_parentGroupsDict->objectForKey(groupInt) == obj) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.48f, 0.12f, 0.46f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.73f, 0.25f, 0.71f, 1.f));
				if (ImGui::Button(btnStr.c_str())) {
					lel->m_parentGroupsDict->removeObjectForKey(groupInt);
				}
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}
			else {
				if (ImGui::Button(btnStr.c_str())) {
					obj->removeFromGroup(groupInt);
					static_cast<CCArray*>(lel->m_groups[groupInt])->removeObject(obj, false);
				}
			}
			if (i + 1 < obj->m_groupCount)
				ImGui::SameLine();

		}
		//ImGui::PopStyleVar();
	}

	if (ImGui::CollapsingHeader("-----| Layer & Z-Order |-----")) {
		int el1 = obj->m_editorLayer;
		ImGui::PushItemWidth(150.0f);
		ImGui::InputInt("##EditorL1", &el1);
		setMin(el1, 0);
		ImGui::SameLine();
		if (ImGui::Button("CL##EditorL1")) el1 = LevelEditorLayer::get()->m_currentLayer;
		ImGui::SetItemTooltip("Set to current Editor Layer");
		ImGui::SameLine();
		ImGui::Text("EditorL1");
		obj->m_editorLayer = el1;
		

		int el2 = obj->m_editorLayer2;
		ImGui::PushItemWidth(150.0f);
		ImGui::InputInt("##EditorL2", &el2);
		setMin(el2, 0);
		ImGui::SameLine();
		if (ImGui::Button("CL##EditorL2")) el2 = LevelEditorLayer::get()->m_currentLayer;
		ImGui::SetItemTooltip("Set to current Editor Layer");
		ImGui::SameLine();
		ImGui::Text("EditorL2");
		obj->m_editorLayer2 = el2;

		
		std::string zOrderStr = "Z-Order";
		int zord = obj->m_zOrder;
		if (zord == 0) zOrderStr = zOrderStr + " (" + std::to_string(obj->m_defaultZOrder) + ")";
		ImGui::PushItemWidth(150.0f);
		ImGui::InputInt(zOrderStr.c_str(), &zord);
		obj->m_zOrder = zord;
		obj->m_shouldUpdateColorSprite = 1;


		int* zLayer = reinterpret_cast<int*>(&obj->m_zLayer);
		ImGui::PushItemWidth(150.0f);

		int layerIndex = 0;
		for (int i = 0; i < IM_ARRAYSIZE(layerIntItems); i++) {
			if (static_cast<int>(obj->m_zLayer) == layerIntItems[i]) {
				layerIndex = i;
				break;
			}
		}

		if (ImGui::Combo("Z-Layer", &layerIndex, layerTypeItems, IM_ARRAYSIZE(layerTypeItems))) {
			int* objLayer = reinterpret_cast<int*>(&obj->m_zLayer); 
			*objLayer = layerIntItems[layerIndex];
		}

		//ImGui::InputInt("Z-Layer", zLayer);
		////setMaxMin(*zLayer, 11, -5);
		//ImGui::SameLine();
		//std::string zLayerString = "(UNKNOWN)";
		//float zLayerStringFloat = (std::abs(*zLayer - 4) + 1) / 2.f;
		//int zLayerStringInt = std::floor(zLayerStringFloat);
		//std::string postfix = zLayerStringFloat > std::floor(zLayerStringFloat) ? "/2)" : ")";
		//if (*zLayer == 0) zLayerString = "(Default)";
		//else if (*zLayer < 4) zLayerString = "(B" + std::to_string(zLayerStringInt) + postfix;
		//else if (*zLayer > 4) zLayerString = "(T" + std::to_string(zLayerStringInt) + postfix;
		//else if (*zLayer == 4) zLayerString = "(M)";
		//ImGui::Text(zLayerString.c_str());

		
		//if (ImGui::Button("Copy##EGM-COPY")) {
			//lel->
		//}

		//if (ImGui::Button("Paste##EGM-Paste")) {

		//}
	}
	if (ImGui::CollapsingHeader("-----| Extra |-----")) {
		int enterChnl = obj->m_enterChannel;
		ImGui::InputInt("Enter Channel", &enterChnl);
		obj->m_enterChannel = enterChnl;

		int material = obj->m_objectMaterial;
		ImGui::InputInt("Material", &material);
		obj->m_objectMaterial = material;

		//if (dynamic_cast<EffectGameObject*>(obj)) ImGui::Text("ControlID: %d", static_cast<EffectGameObject*>(obj)->m_controlID);
		if (dynamic_cast<EffectGameObject*>(obj)) {
			auto egObj = static_cast<EffectGameObject*>(obj);

			int ordVal = egObj->m_ordValue;
			ImGui::InputInt("Order Value", &ordVal);
			setMin(ordVal, 0);
			egObj->m_ordValue = ordVal;

			int chnlVal = egObj->m_channelValue;
			ImGui::InputInt("Channel", &chnlVal);
			setMin(chnlVal, 0);
			egObj->m_channelValue = chnlVal;

			int ctrlId = egObj->m_controlID;
			ImGui::InputInt("ControlID", &ctrlId);
			egObj->m_controlID = ctrlId;
		}
		ImGui::Text("MainColorID: %d", obj->m_activeMainColorID);
		ImGui::Text("DetailColorID: %d", obj->m_activeDetailColorID);
		if (ImGui::Button("Anim")) {
			
			lel->m_editorUI->createNewKeyframeAnim();
		}
	}
	if (ImGui::CollapsingHeader("-----| Cosmetic |-----")) {

		ImGui::Checkbox("No Effects", &obj->m_hasNoEffects);
		ImGui::SameLine(160);
		ImGui::Checkbox("Dont Fade", &obj->m_isDontFade);

		ImGui::Checkbox("No Glow", &obj->m_hasNoGlow);
		ImGui::SameLine(160);
		ImGui::Checkbox("Dont Enter", &obj->m_isDontEnter);

		ImGui::Checkbox("No Particle", &obj->m_hasNoParticles);
		ImGui::SameLine(160);
		ImGui::Checkbox("High Detail", &obj->m_isHighDetail);

		ImGui::Checkbox("No Audio Scale", &obj->m_hasNoAudioScale);
		ImGui::SameLine(160);
		ImGui::Checkbox("Hide", &obj->m_isHide);

	}
	if (ImGui::CollapsingHeader("-----| Parent & Physics |-----")) {
		ImGui::Checkbox("Group Parent", &obj->m_hasGroupParent);
		ImGui::SameLine(160);
		ImGui::Checkbox("No Touch", &obj->m_isNoTouch);

		ImGui::Checkbox("Area Parent", &obj->m_hasAreaParent);
		ImGui::SameLine(160);
		ImGui::Checkbox("Passable", &obj->m_isPassable);

		ImGui::Checkbox("Dont Boost X", &obj->m_isDontBoostX);
		ImGui::SameLine(160);
		ImGui::Checkbox("Extended Collision", &obj->m_hasExtendedCollision);

		ImGui::Checkbox("Dont Boost Y", &obj->m_isDontBoostY);

	}

	if (ImGui::CollapsingHeader("-----| Platformer Only |-----")) {
		ImGui::Checkbox("Ice Block", &obj->m_isIceBlock);
		ImGui::SameLine(160);
		ImGui::Checkbox("Grip Slope", &obj->m_isGripSlope);

		ImGui::Checkbox("Non Stick X", &obj->m_isNonStickX);
		ImGui::SameLine(160);
		ImGui::Checkbox("Extra Sticky", &obj->m_isExtraSticky);

		ImGui::Checkbox("Non Stick Y", &obj->m_isNonStickY);
		ImGui::SameLine(160);
		ImGui::Checkbox("Scale Stick", &obj->m_isScaleStick);
	}

	if (dynamic_cast<EffectGameObject*>(obj)) {
		if (ImGui::CollapsingHeader("-----| EffectGameObject |-----")) {
			auto egObj = static_cast<EffectGameObject*>(obj);
			ImGui::Checkbox("Single PTouch", &egObj->m_isSinglePTouch);
			ImGui::SameLine(160);
			if (ImGui::Checkbox("Preview", &egObj->m_shouldPreview)) {
				lel->tryUpdateSpeedObject(egObj, false);
			}
			ImGui::Checkbox("Center Effect", &egObj->m_hasCenterEffect);
			ImGui::SameLine(160);
			bool isPlayBackObject = false;
			EffectGameObject** playBackObject = reinterpret_cast<EffectGameObject**>(reinterpret_cast<uintptr_t>(lel->get()) + 0x39A0);
			if (*playBackObject == egObj)
				isPlayBackObject = true;
			if (ImGui::Checkbox("Playback", &isPlayBackObject)) {
				if (!isPlayBackObject)
					*playBackObject = nullptr;
				else
					*playBackObject = egObj;
			}

			if (ImGui::Checkbox("Reverse", &egObj->m_isReverse)) {
				lel->tryUpdateSpeedObject(egObj, false);
			}
		}
	}

	// in an imgui window somewhere...
	//ImGui::PushFont(iconFont);
	//ImGui::Text(ICON_MDI_BLACK_MESA "  Paint"); // use string literal concatenation
	// outputs a paint brush icon and 'Paint' as a string.


	//ImGui::Text(ICON_MDI_BLACK_MESA);
	//ImGui::PopFont();



	//ImGui::Checkbox("")
}

void renderForArray(CCArray* objArr, LevelEditorLayer* lel) {
	if (ImGui::Button("Paste##PASTESTATE")) {
		copyStateObject.pasteState(objArr);
	}

	if (ImGui::CollapsingHeader("-----| Advanced ReGroup |-----")) {
		static int regroupStart = 1;	// Initial group (where it starts before regroup)
		static int regroupEnd = 9999;	// Initial group (where it ends before regroup)
		static int regroupFrom = 1;		// Regrouped Offset (where it would start after regroup)
		static bool regroupOnlyFree = false;

		ImGui::PushItemWidth(150.0f);
		ImGui::InputInt("Regroup Start", &regroupStart);
		setMaxMin(regroupStart, 9999, 1);

		ImGui::PushItemWidth(150.0f);
		ImGui::InputInt("Regroup End", &regroupEnd);
		setMaxMin(regroupEnd, 9999, 1);

		ImGui::PushItemWidth(150.0f);
		ImGui::InputInt("Regroup From", &regroupFrom);
		setMaxMin(regroupFrom, 9999, 1);

		ImGui::Checkbox("Only Free Groups", &regroupOnlyFree);

		if (ImGui::Button("Regroup")) {
			std::map<int, int> regroupedMap;
			int regroupedID = regroupFrom;
			bool offlimit = false;

			std::cout << "Map:\n";
			for (int i = 0; i < groupsFromObjArr.size(); i++) {
				if (groupsFromObjArr[i].first >= regroupStart && groupsFromObjArr[i].first <= regroupEnd) {
					
					if (regroupOnlyFree) {
						//std::cout << regroupedID << " " << lel->m_groups[regroupedID] << "\n";
						while (regroupedID <= 9999 && lel->m_groups[regroupedID] && lel->m_groups[regroupedID]->count() > 0) {
							//std::cout << regroupedID << " - Not Free, adding...\n";
							regroupedID++;
						}
					}
					
					if (regroupedID > 9999) {
						offlimit = true;
						std::cout << "OFFLIMIT! BREAKING REGROUP!\n";
						break;
					}

					std::cout << groupsFromObjArr[i].first << " " << regroupedID << "\n";
					regroupedMap.emplace(groupsFromObjArr[i].first, regroupedID);
					regroupedID++;

				}
			}
			std::cout << "\n";
			
			if (!offlimit) {
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					std::unordered_set<int> idsToAdd;
					int idToRetarget = 0;
					int idToRecenter = 0;

					// Group Removing and Buffering
					for (auto pair : regroupedMap) {
						if (static_cast<CCArray*>(lel->m_groups[pair.first]) && static_cast<CCArray*>(lel->m_groups[pair.first])->containsObject(obj)) {
							obj->removeFromGroup(pair.first);
							static_cast<CCArray*>(lel->m_groups[pair.first])->removeObject(obj, false);

							idsToAdd.emplace(pair.second);
						}

						if (auto eObj = dynamic_cast<EffectGameObject*>(obj)) {
							if (eObj->m_objectID != 1006 || eObj->m_pulseTargetType == 1) {
								if (idToRecenter == 0 && eObj->m_centerGroupID == pair.first)
									idToRecenter = pair.second;
								if (idToRetarget == 0 && eObj->m_targetGroupID == pair.first)
									idToRetarget = pair.second;
							}

							lel->updateObjectLabel(eObj);
						}
					}

					// Actuall Group Adding
					for (int i : idsToAdd) {
						if (!lel->m_groups[i]) {
							CCArray* arr = CCArray::create();
							arr->retain();
							lel->m_groups[i] = arr;
						}
						static_cast<CCArray*>(lel->m_groups[i])->addObject(obj);
						obj->addToGroup(i);
					}

					if (idToRetarget != 0 && dynamic_cast<EffectGameObject*>(obj)) dynamic_cast<EffectGameObject*>(obj)->m_targetGroupID = idToRetarget;
					if (idToRecenter != 0 && dynamic_cast<EffectGameObject*>(obj)) dynamic_cast<EffectGameObject*>(obj)->m_centerGroupID = idToRecenter;
				}
				groupInfoUpdate();
			}
		}
	}

	if (ImGui::CollapsingHeader("-----| Groups Settings |-----")) {
		ImGui::PushItemWidth(150.0f);
		ImGui::InputInt("ID Offset", &groupOffset);
		setMaxMin(groupOffset, 9999, 1);
		ImGui::PushItemWidth(150.0f);
		ImGui::InputInt("GroupID", &chosenGroupEGM);
		setMaxMin(chosenGroupEGM, 9999, 1);


		if (ImGui::Button("Add##EGM-ADDGROUP")) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				bool addGroupCheck = true;
				if (obj->m_groups) {
					addGroupCheck = std::find(obj->m_groups->begin(), obj->m_groups->end(), 0) != obj->m_groups->end() && (!static_cast<CCArray*>(lel->m_groups[chosenGroupEGM]) || !static_cast<CCArray*>(lel->m_groups[chosenGroupEGM])->containsObject(obj));
				}

				if (addGroupCheck) {
					if (!lel->m_groups[chosenGroupEGM]) {
						CCArray* arr = CCArray::create();
						arr->retain();
						lel->m_groups[chosenGroupEGM] = arr;
					}
					static_cast<CCArray*>(lel->m_groups[chosenGroupEGM])->addObject(obj);
					obj->addToGroup(chosenGroupEGM);
					
					groupInfoUpdate();
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Next Free##EGM-NEXTFREEGROUP")) {
			auto groups = lel->m_groups;
			if (groupOffset < 1) groupOffset = 1;
			for (int i = groupOffset; i < groups.size(); i++) {
				int count = groups[i] ? groups[i]->count() : 0;
				if (count == 0) {
					chosenGroupEGM = i;
					break;
				}
			}
		}
		
		ImGui::PushStyleColor(ImGuiCol_Separator, { 0.33f, 0.33f, 0.33f, 1.f });
		ImGui::SeparatorText("Groups");
		ImGui::PopStyleColor();

		int groupsSize = groupsFromObjArr.size();
		for (int i = 0; i < groupsSize; i++) {
			int groupInt = groupsFromObjArr[i].first;
			std::string btnStr = std::to_string(groupInt);
			btnStr += "##RMVGROUP";

			if (groupsFromObjArr[i].second != objArr->count()) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.27f, 0.f, 0.52f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.40f, 0.f, 0.67f, 1.f));
			}

			if (ImGui::Button(btnStr.c_str())) {
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					obj->removeFromGroup(groupInt);
					static_cast<CCArray*>(lel->m_groups[groupInt])->removeObject(obj, false);
				}
				groupsFromObjArr.erase(std::find(groupsFromObjArr.begin(), groupsFromObjArr.end(), groupsFromObjArr[i]));
				groupsSize--;
			}

			if (groupsFromObjArr[i].second != objArr->count()) {
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}

			if ((i + 1) % 10 != 0 && i != groupsSize - 1)
				ImGui::SameLine();
		}

		//ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
		//for (int i = 0; i < obj->m_groupCount; i++) {
		//	int groupInt = obj->m_groups->at(i);
		//	std::string btnStr = std::to_string(groupInt);
		//	btnStr += "##RMVGROUP";
		//
		//	if (lel->m_parentGroupsDict->objectForKey(groupInt) == obj) {
		//		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.48f, 0.12f, 0.46f, 1.f));
		//		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.73f, 0.25f, 0.71f, 1.f));
		//		if (ImGui::Button(btnStr.c_str())) {
		//			lel->m_parentGroupsDict->removeObjectForKey(groupInt);
		//		}
		//		ImGui::PopStyleColor();
		//		ImGui::PopStyleColor();
		//	}
		//	else {
		//		if (ImGui::Button(btnStr.c_str())) {
		//			obj->removeFromGroup(groupInt);
		//			static_cast<CCArray*>(lel->m_groups[groupInt])->removeObject(obj, false);
		//		}
		//	}
		//	if (i + 1 < obj->m_groupCount)
		//		ImGui::SameLine();
		//
		//}
		//ImGui::PopStyleVar();
	}
	

	if (ImGui::CollapsingHeader("-----| Layer & Z-Order |-----")) {

		if (int delta = deltaInputIntImproved("EditorL1", maxEl1, minEl1, 1)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_editorLayer += delta;
				if (obj->m_editorLayer < 0) obj->m_editorLayer = 0;
			}
			groupInfoUpdate();
		}
		ImGui::SameLine();
		if (ImGui::Button("CL##EditorL1")) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_editorLayer = LevelEditorLayer::get()->m_currentLayer;
			}
			groupInfoUpdate();
		}
		ImGui::SetItemTooltip("Set to current Editor Layer");

		if (int delta = deltaInputIntImproved("EditorL2", maxEl2, minEl2, 1)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_editorLayer2 += delta;
				if (obj->m_editorLayer2 < 0) obj->m_editorLayer2 = 0;
			}
			groupInfoUpdate();
		}
		ImGui::SameLine();
		if (ImGui::Button("CL##EditorL2")) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_editorLayer2 = LevelEditorLayer::get()->m_currentLayer;
			}
			groupInfoUpdate();
		}
		ImGui::SetItemTooltip("Set to current Editor Layer");


		if (int delta = deltaInputIntImproved("Z-Order", maxZOrder, minZOrder, 1)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				int oldOrder = obj->m_zOrder;
				obj->m_zOrder += delta;
				//stupid default order check
				if (oldOrder == 1 && delta == -1) obj->m_zOrder = -1;
				if (oldOrder == -1 && delta == 1) obj->m_zOrder = 1;

				obj->m_shouldUpdateColorSprite = 1;
			}
			groupInfoUpdate();
		}

		int layerIndex = 0;
		for (int i = 0; i < IM_ARRAYSIZE(layerIntItems); i++) {
			if (minZLayer == layerIntItems[i]) {
				layerIndex = i;
				break;
			}
		}
		if (ImGui::Combo("Z-Layer", &layerIndex, layerTypeItems, IM_ARRAYSIZE(layerTypeItems))) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				int* objLayer = reinterpret_cast<int*>(&obj->m_zLayer);
				*objLayer = layerIntItems[layerIndex];
				obj->m_shouldUpdateColorSprite = 1;
			}
			groupInfoUpdate();
		}


		//if (int delta = deltaInputIntImproved("Z-Layer", maxZLayer, minZLayer, 1)) {
		//	for (auto obj : CCArrayExt<GameObject*>(objArr)) {
		//		int* zLayer = reinterpret_cast<int*>(&obj->m_zLayer);
		//		*zLayer += delta;
		//		setMaxMin(*zLayer, 11, -5);
		//	}
		//	groupInfoUpdate();
		//}
	}

	if (ImGui::CollapsingHeader("-----| Extra |-----")) {

		if (int delta = deltaInputIntImproved("Enter Channel", maxEnterChannel, minEnterChannel, 1)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_enterChannel += delta;
			}
			groupInfoUpdate();
		}

		if (int delta = deltaInputIntImproved("Material", maxMaterial, minMaterial, 1)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_objectMaterial += delta;
			}
			groupInfoUpdate();
		}

		if (firstEgObj) {
			minOrderValue = firstEgObj->m_ordValue;
			maxOrderValue = firstEgObj->m_ordValue;
			minChannel = firstEgObj->m_channelValue;
			maxChannel = firstEgObj->m_channelValue;
			minControlID = firstEgObj->m_controlID;
			maxControlID = firstEgObj->m_controlID;

			if (int delta = deltaInputIntImproved("Order Value", maxOrderValue, minOrderValue, 1)) {
				for (auto egObj : CCArrayExt<EffectGameObject*>(objArr)) {
					if (egObj) {
						egObj->m_ordValue += delta;
						if (egObj->m_ordValue < 0) egObj->m_ordValue = 0;
					}
				}
				groupInfoUpdate();
			}
			if (int delta = deltaInputIntImproved("Channel Value", maxChannel, minChannel, 1)) {
				for (auto egObj : CCArrayExt<EffectGameObject*>(objArr)) {
					if (egObj) {
						egObj->m_channelValue += delta;
						if (egObj->m_channelValue < 0) egObj->m_channelValue = 0;
					}
				}
				groupInfoUpdate();
			}
			if (int delta = deltaInputIntImproved("ControlID", maxControlID, minControlID, 1)) {
				for (auto egObj : CCArrayExt<EffectGameObject*>(objArr)) {
					if (egObj) {
						egObj->m_controlID += delta;
					}
				}
				groupInfoUpdate();
			}
		}
		if (ImGui::Button("Anim")) {
			lel->m_editorUI->createNewKeyframeAnim();
		}
	}
	

	if (ImGui::CollapsingHeader("-----| Cosmetic |-----")) {

		if (ImGui::Checkbox("No Effects", &cb_NoEffects)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_hasNoEffects = cb_NoEffects;
			}
		}
		ImGui::SameLine(160);
		if (ImGui::Checkbox("Dont Fade", &cb_DontFade)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isDontFade = cb_DontFade;
			}
		}
	
		if (ImGui::Checkbox("No Glow", &cb_NoGlow)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_hasNoGlow = cb_NoGlow;
			}
		}
		ImGui::SameLine(160);
		if (ImGui::Checkbox("Dont Enter", &cb_DontEnter)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isDontEnter = cb_DontEnter;
			}
		}
	
		if (ImGui::Checkbox("No Particle", &cb_NoParticle)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_hasNoParticles = cb_NoParticle;
			}
		}
		ImGui::SameLine(160);
		if (ImGui::Checkbox("High Detail", &cb_HighDetail)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isHighDetail = cb_HighDetail;
			}
		}
	
		if (ImGui::Checkbox("No Audio Scale", &cb_NoAudioScale)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_hasNoAudioScale = cb_NoAudioScale;
			}
		}
		ImGui::SameLine(160);
		if (ImGui::Checkbox("Hide", &cb_Hide)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isHide = cb_Hide;
			}
		}

	}


	if (ImGui::CollapsingHeader("-----| Parent & Physics |-----")) {
		if (ImGui::Checkbox("Group Parent", &cb_GroupParent)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_hasGroupParent = cb_GroupParent;
			}
		}
		ImGui::SameLine(160);
		if (ImGui::Checkbox("No Touch", &cb_NoTouch)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isNoTouch = cb_NoTouch;
			}
		}
	
		if (ImGui::Checkbox("Area Parent", &cb_AreaParent)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_hasAreaParent = cb_AreaParent;
			}
		}
		ImGui::SameLine(160);
		if (ImGui::Checkbox("Passable", &cb_Passable)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isPassable = cb_Passable;
			}
		}
	
		if (ImGui::Checkbox("Dont Boost X", &cb_DontBoostX)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isDontBoostX = cb_DontBoostX;
			}
		}
		ImGui::SameLine(160);
		if (ImGui::Checkbox("Extended Collision", &cb_ExtendedCollision)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_hasExtendedCollision = cb_ExtendedCollision;
			}
		}
	
		if (ImGui::Checkbox("Dont Boost Y", &cb_DontBoostY)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isDontBoostY = cb_DontBoostY;
			}
		}

	}

	if (ImGui::CollapsingHeader("-----| Platformer Only |-----")) {
		if (ImGui::Checkbox("Ice Block", &cb_IceBlock)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isIceBlock = cb_IceBlock;
			}
		}
		ImGui::SameLine(160);
		if (ImGui::Checkbox("Grip Slope", &cb_GripSlope)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isGripSlope = cb_GripSlope;
			}
		}
	
		if (ImGui::Checkbox("Non Stick X", &cb_NonStickX)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isNonStickX = cb_NonStickX;
			}
		}
		ImGui::SameLine(160);
		if (ImGui::Checkbox("Extra Sticky", &cb_ExtraSticky)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isExtraSticky = cb_ExtraSticky;
			}
		}
	
		if (ImGui::Checkbox("Non Stick Y", &cb_NonStickY)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isNonStickY = cb_NonStickY;
			}
		}
		ImGui::SameLine(160);
		if (ImGui::Checkbox("Scale Stick", &cb_ScaleStick)) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->m_isScaleStick = cb_ScaleStick;
			}
		}
	}

	if (firstEgObj) {
		if (ImGui::CollapsingHeader("-----| EffectGameObject |-----")) {
			if (ImGui::Checkbox("Single PTouch", &cb_SinglePTouch)) {
				for (auto obj : CCArrayExt<EffectGameObject*>(objArr)) {
					if (auto egObj = dynamic_cast<EffectGameObject*>(obj)) {
						egObj->m_isSinglePTouch = cb_SinglePTouch;
					}
				}
			}
			ImGui::SameLine(160);
			if (ImGui::Checkbox("Preview", &cb_Preview)) {
				for (auto obj : CCArrayExt<EffectGameObject*>(objArr)) {
					if (auto egObj = dynamic_cast<EffectGameObject*>(obj)) {
						egObj->m_shouldPreview = cb_Preview;
						lel->tryUpdateSpeedObject(egObj, false);
					}
				}
			}

			if (ImGui::Checkbox("Center Effect", &cb_CenterEffect)) {
				for (auto obj : CCArrayExt<EffectGameObject*>(objArr)) {
					if (auto egObj = dynamic_cast<EffectGameObject*>(obj)) {
						egObj->m_hasCenterEffect = cb_CenterEffect;
					}
				}
			}
			ImGui::SameLine(160);
			if (ImGui::Checkbox("Reverse", &cb_Reverse)) {
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					if (auto egObj = dynamic_cast<EffectGameObject*>(obj)) {
						egObj->m_isReverse = cb_Reverse;
						lel->tryUpdateSpeedObject(egObj, false);
					}
				}
			}
		}
	}
}

void ErGui::renderEditGroupModule() {
	ImGui::Begin("Group");

	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto obj = lel->m_editorUI->m_selectedObject;
	auto objArr = lel->m_editorUI->m_selectedObjects;
	if (obj) {
		renderForObject(obj, lel);
	}
	else if (objArr->count() > 0) {
		renderForArray(objArr, lel);
	}
	else {
		ImGui::Text("Object is not selected...");
	}

	ImGui::End();
}

void ErGui::groupInfoUpdate() {
	auto objArr = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObjects;
	if (objArr->count() == 0) return;

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


	// Стоит переработать эту систему.
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

	std::map<int, int> groupSet;
	std::set<int> oldGroupSet;

	for (auto obj : CCArrayExt<GameObject*>(objArr)) {
		if (obj->m_groups) {
			for (int i = 0; i < obj->m_groups->size(); i++) {
				oldGroupSet.insert(obj->m_groups->at(i));
				groupSet[obj->m_groups->at(i)]++;
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


		if (obj->m_hasNoEffects)			ErGui::cb_NoEffects = true;
		if (obj->m_isDontFade)				ErGui::cb_DontFade = true;
		if (obj->m_hasNoGlow)				ErGui::cb_NoGlow = true;
		if (obj->m_isDontEnter)				ErGui::cb_DontEnter = true;
		if (obj->m_hasNoParticles)			ErGui::cb_NoParticle = true;
		if (obj->m_isHighDetail)			ErGui::cb_HighDetail = true;
		if (obj->m_hasNoAudioScale)			ErGui::cb_NoAudioScale = true;
		if (obj->m_isHide)					ErGui::cb_Hide = true;

		if (obj->m_hasGroupParent)			ErGui::cb_GroupParent = true;
		if (obj->m_isNoTouch)				ErGui::cb_NoTouch = true;
		if (obj->m_hasAreaParent)			ErGui::cb_AreaParent = true;
		if (obj->m_isPassable)				ErGui::cb_Passable = true;
		if (obj->m_isDontBoostX)			ErGui::cb_DontBoostX = true;
		if (obj->m_hasExtendedCollision)	ErGui::cb_ExtendedCollision = true;
		if (obj->m_isDontBoostY)			ErGui::cb_DontBoostY = true;

		if (obj->m_isIceBlock)				ErGui::cb_IceBlock = true;
		if (obj->m_isGripSlope)				ErGui::cb_GripSlope = true;
		if (obj->m_isNonStickX)				ErGui::cb_NonStickX = true;
		if (obj->m_isExtraSticky)			ErGui::cb_ExtraSticky = true;
		if (obj->m_isNonStickY)				ErGui::cb_NonStickY = true;
		if (obj->m_isScaleStick)			ErGui::cb_ScaleStick = true;

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
				oldGroupSet.insert(obj->m_groups->at(i));
				groupSet[obj->m_groups->at(i)]++;
			}
		}
	}

	groupSet.erase(0);
	for (auto obj : groupSet) {
		ErGui::groupsFromObjArr.push_back(obj);
	}

	std::sort(ErGui::groupsFromObjArr.begin(), ErGui::groupsFromObjArr.end());
}