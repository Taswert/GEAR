#include "TimeAndKeyframeTriggers.hpp"
using namespace ErGui;

void drawTimeControlSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<TimerTriggerGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Item ID", &eObj->m_itemID)) {
		if (eObj->m_itemID < 0) eObj->m_itemID = 0;
		if (eObj->m_itemID > 9999) eObj->m_itemID = 9999;
	}

	bool start = eObj->m_controlType == 0;
	bool stop = eObj->m_controlType == 1;

	if (ImGui::Checkbox("Start", &start)) {
		stop = false;
		eObj->m_controlType = 0;
	}
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("Stop", &stop)) {
		start = false;
		eObj->m_controlType = 1;
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawKeyframeAnimationSettings(GameObject* obj, CCArray* objArr) {
	auto kObj = static_cast<KeyframeAnimTriggerObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Animation Group ID", &kObj->m_animationID);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target ID", &kObj->m_targetGroupID)) {
		if (kObj->m_targetGroupID < 0) kObj->m_targetGroupID = 0;
		if (kObj->m_targetGroupID > 9999) kObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Parent ID", &kObj->m_centerGroupID)) {
		if (kObj->m_centerGroupID < 0) kObj->m_centerGroupID = 0;
		if (kObj->m_centerGroupID > 9999) kObj->m_centerGroupID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Time Mod", &kObj->m_timeMod, .1f, 0.f, 1.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Position X Mod", &kObj->m_positionXMod, .1f, -2.f, 2.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Position Y Mod", &kObj->m_positionYMod, .1f, -2.f, 2.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Rotation Mod", &kObj->m_rotationMod, .1f, -2.f, 2.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Scale X Mod", &kObj->m_scaleXMod, .1f, -2.f, 2.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Scale Y Mod", &kObj->m_scaleYMod, .1f, -2.f, 2.f);

	drawTouchSpawnTriggered(kObj, objArr);
}

void drawKeyframeSettings(GameObject* obj, CCArray* objArr) {
	auto kObj = static_cast<KeyframeGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	drawComponentGroupID(kObj, objArr, "Group ID");

	ImGui::SeparatorText("Extended Settings");
	
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragInt("Key Group", &kObj->m_keyframeGroup)) {
		// Uh, I guess it's fine just like that, lol
		lel->refreshKeyframeAnims();
	}

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	int oldIndex = kObj->m_keyframeIndex;
	int maxIndex = 0;
	if (ErGui::DragInt("Key Index", &kObj->m_keyframeIndex)) {
		// but this abomination...
		std::unordered_map<int, KeyframeGameObject*> currentGroupKeyframes;
		int currentGroup = kObj->m_keyframeGroup;
		int currentIndex = kObj->m_keyframeIndex;
		for (auto objInArr : CCArrayExt<KeyframeGameObject*>(lel->m_keyframeObjects)) {
			if (currentGroup == objInArr->m_keyframeGroup && kObj != objInArr) {
				currentGroupKeyframes[objInArr->m_keyframeIndex] = objInArr;
				if (objInArr->m_keyframeIndex > maxIndex) 
					maxIndex = objInArr->m_keyframeIndex;

				if (currentIndex == objInArr->m_keyframeIndex) {
					if (oldIndex < currentIndex) {
						objInArr->m_keyframeIndex--;
					}
				}
			}
		}

		if (!(currentIndex > maxIndex || currentIndex < 0)) {

			if (oldIndex < currentIndex) {
				//for (int i = 0; i <= maxIndex; i++) {
				//	log::info("{} / {} - {}", i, currentGroupKeyframes[i] ? currentGroupKeyframes[i]->m_keyframeIndex : -1, currentGroupKeyframes[i]);
				//}
				//log::info("Old Index {}", oldIndex);
				//log::info("Current Index {}", currentIndex);


				for (int i = oldIndex + 1; i <= currentIndex; i++) {
					currentGroupKeyframes[i]->m_keyframeIndex--;
					currentGroupKeyframes[i - 1] = currentGroupKeyframes[i];
				}
				currentGroupKeyframes[currentIndex-1]->m_keyframeIndex++;

				//log::info("--------", currentIndex);
				//for (int i = 0; i <= maxIndex; i++) {
				//	if (currentIndex == i) {
				//		currentGroupKeyframes[i] = kObj;
				//		log::info("{} / {} - {}  <-------", i, currentGroupKeyframes[i] ? currentGroupKeyframes[i]->m_keyframeIndex : -1, currentGroupKeyframes[i]);
				//	}
				//	else
				//		log::info("{} / {} - {}", i, currentGroupKeyframes[i] ? currentGroupKeyframes[i]->m_keyframeIndex : -1, currentGroupKeyframes[i]);
				//}
				//log::info("--------DONE--------\n\n", currentIndex);
			}
			else {
				//for (int i = 0; i <= maxIndex; i++) {
				//	log::info("{} / {} - {}", i, currentGroupKeyframes[i] ? currentGroupKeyframes[i]->m_keyframeIndex : -1, currentGroupKeyframes[i]);
				//}
				//log::info("Old Index {}", oldIndex);
				//log::info("Current Index {}", currentIndex);

				for (int i = oldIndex - 1; i >= currentIndex; i--) {
					currentGroupKeyframes[i]->m_keyframeIndex++;
					currentGroupKeyframes[i + 1] = currentGroupKeyframes[i];
				}

				//log::info("--------", currentIndex);
				//for (int i = 0; i <= maxIndex; i++) {
				//	if (currentIndex == i) {
				//		currentGroupKeyframes[i] = kObj;
				//		log::info("{} / {} - {}  <-------", i, currentGroupKeyframes[i] ? currentGroupKeyframes[i]->m_keyframeIndex : -1, currentGroupKeyframes[i]);
				//	}
				//	else
				//		log::info("{} / {} - {}", i, currentGroupKeyframes[i] ? currentGroupKeyframes[i]->m_keyframeIndex : -1, currentGroupKeyframes[i]);
				//}
				//log::info("--------DONE--------\n\n", currentIndex);
			}

		}

		lel->refreshKeyframeAnims();
	}


	ImGui::SeparatorText("Core Settings");

	drawComponentTime(kObj, objArr, "Time", true);
}

void ErGui::mapTimeAndKeyframeTriggers() {
	// Time
	// Time Event
	triggersMap[3617] = drawTimeControlSettings;
	
	// Setup Keyframe
	triggersMap[3032] = drawKeyframeSettings;
	triggersMap[3033] = drawKeyframeAnimationSettings;
	
}