#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#include "SelectedObjectInfo.hpp"
#include "SelectFilter.hpp"

using namespace geode::prelude;
using namespace SelectedObjectInfo;

std::unordered_map<ZLayer, std::string> zLayerMap = {
	{ZLayer::B5, "Bottom 5"}, 
	{ZLayer::B4, "Bottom 4"}, 
	{ZLayer::B3, "Bottom 3"}, 
	{ZLayer::B2, "Bottom 2"}, 
	{ZLayer::B1, "Bottom 1"},
	{ZLayer::T1, "Top 1"}, 
	{ZLayer::T2, "Top 2"}, 
	{ZLayer::T3, "Top 3"}, 
	{ZLayer::T4, "Top 4"},
	{ZLayer::Default, "Default"}
};

std::unordered_set<int> objsWithItemIDs = { 
	1275, 1276, 1587, 1588, 1589, 1590, 1598, 1599, 1614, 3601, 4401, 4402, 4403, 
	4404, 4405, 4406, 4407, 4408, 4409, 4410, 4411, 4412, 4413, 4414, 4415, 4416, 4417,
	4418, 4419, 4420, 4421, 4422, 4423, 4424, 4425, 4426, 4427, 4428, 4429, 4430, 4431, 
	4432, 4433, 4434, 4435, 4436, 4437, 4438, 4439, 4440, 4441, 4442, 4443, 4444, 4445, 
	4446, 4447, 4448, 4449, 4450, 4451, 4452, 4453, 4454, 4455, 4456, 4457, 4458, 4459,
	4460, 4461, 4462, 4463, 4464, 4465, 4466, 4467, 4468, 4469, 4470, 4471, 4472, 4473,
	4474, 4475, 4476, 4477, 4478, 4479, 4480, 4481, 4482, 4483, 4484, 4485, 4486, 4487, 
	4488, 4538, 4489, 4490, 4491, 4492, 4493, 4494, 4495, 4496, 4497, 4537, 4498, 4499,
	4500, 4501, 4502, 4503, 4504, 4505, 4506, 4507, 4508, 4509, 4510, 4511, 4512, 4513,
	4514, 4515, 4516, 4517, 4518, 4519, 4520, 4521, 4522, 4523, 4524, 4525, 4526, 4527, 
	4528, 4529, 4530, 4531, 4532, 4533, 4534, 4535, 4536, 4539, 
	
	1611, 1811, 1817, 1615, 3641, 3620, 3619,
};

std::unordered_set<int> objsWithBlockIDs = {
	1816, 1815, 3609,
};

//std::string objectTypes[] = {
	//"Solid", "Basic (Unused)", "Hazard", "Inverse Gravity Portal", "Normal Gravity Portal", "Ship Portal", "Cube Portal", "Decoration", "Yellow Jump Pad", "Pink Jump Pad", "Gravity Pad", "Yellow Jump Orb", "Pink Jump Orb", "Gravity Orb", "Inverse Mirror Portal", "Normal Mirror Portal", "Ball Portal", "Regular Size Portal", "Mini Size Portal", "UFO Portal", "Modifier", "Breakable", "Secret Coin", "Dual Portal", "Solo Portal", "Slope", "Wave Portal", "Robot Portal", "Teleport Portal", "Green Orb", "Collectible", "User Coin", "Drop Orb", "Spider Portal", "Red Jump Pad", "Red Jump Orb", "Custom Orb", "Dash Orb", "Gravity Dash Orb", "Collision Object", "Special", "Swing Portal", "Gravity Toggle Portal", "Spider Orb", "Spider Pad", "Enter Effect Object", "Teleport Orb", "Animated Hazard",
//};

void makeLabelFromSet(std::string& selectedInfoString, std::set<int> mySet, const char* nameSingle, const char* nameMult) {
	if (mySet.size() > 4) {
		selectedInfoString += nameMult + std::string(": ")
			+ std::to_string(*mySet.begin()) + ", "
			+ std::to_string(*std::next(mySet.begin())) + ", "
			+ ".. "
			+ std::to_string(*std::prev(std::prev(mySet.end()))) + ", "
			+ std::to_string(*std::prev(mySet.end())) + "\n";
	}
	else if (mySet.size() != 0) {
		if (mySet.size() == 1)
			selectedInfoString += nameSingle + std::string(": ");
		else
			selectedInfoString += nameMult + std::string(": ");
		for (const auto& id : mySet) {
			selectedInfoString += std::to_string(id);
			if (id != *std::prev(mySet.end()))
				selectedInfoString += ", ";
		}
		selectedInfoString += "\n";
	}
}


namespace SelectedObjectInfo {
	static bool s_isSelecting = false;
	
	void update() {
		if (!g_soiNeedsUpdate) return;
		g_soiNeedsUpdate = false;

		auto editorUI = EditorUI::get();
		if (!editorUI) return;

		auto mod = geode::Mod::get();

		auto obj = editorUI->m_selectedObject;
		auto objArr = editorUI->m_selectedObjects;

		// Updating my object info label
		if (auto label = typeinfo_cast<CCLabelBMFont*>(LevelEditorLayer::get()->getChildByID("object-info-label"_spr))) {
			//---- Single Selected
			// Pos: //
			// Rot: //
			// Scale: //
			// Color 1 //
			// Color 2 //
			// Color 1 HSV // -
			// Color 2 HSV // -
			// Groups // 
			// Z Layer // 
			// Z Order // 
			// Object ID //  
			// Target Group // 
			// Item ID // -
			// Block ID // -
			// Particle Count // 
			// isHidden // 
			// isNoTouch //
			// isHighDetail // -

			std::string selectedInfoString = "";

			bool soiPosition =		mod->getSavedValue<bool>("soi-position", true);
			bool soiRotation =		mod->getSavedValue<bool>("soi-rotation", true);
			bool soiScale =			mod->getSavedValue<bool>("soi-scale", true);
			bool soiColor =			mod->getSavedValue<bool>("soi-color", true);
			bool soiHSV =			mod->getSavedValue<bool>("soi-hsv", false);
			bool soiGroups =		mod->getSavedValue<bool>("soi-groups", true);
			bool soiZLayer =		mod->getSavedValue<bool>("soi-zlayer", true);
			bool soiZOrder =		mod->getSavedValue<bool>("soi-zorder", true);
			bool soiObjectID =		mod->getSavedValue<bool>("soi-objectid", false);
			bool soiTargetGroup =	mod->getSavedValue<bool>("soi-targetgroup", true);
			bool soiItemID =		mod->getSavedValue<bool>("soi-itemid", false);
			bool soiBlockID =		mod->getSavedValue<bool>("soi-blockid", false);
			bool soiParticles =		mod->getSavedValue<bool>("soi-particles", true);
			bool soiHidden =		mod->getSavedValue<bool>("soi-hidden", true);
			bool soiNoTouch =		mod->getSavedValue<bool>("soi-no-touch", true);
			bool soiHighDetail =	mod->getSavedValue<bool>("soi-high-detail", false);
			bool soiObjectCount =	mod->getSavedValue<bool>("soi-object-count", true);
			bool soiObjectType =	mod->getSavedValue<bool>("soi-object-type", false);

			if (obj) {
				auto eObj = typeinfo_cast<EffectGameObject*>(obj);

				// Pos
				if (soiPosition) {
					selectedInfoString += "X: " + std::format("{:.2f}", obj->getPositionX()) + "\n";
					selectedInfoString += "Y: " + std::format("{:.2f}", obj->getPositionY()) + "\n";
				}

				// Rot
				if (soiRotation) {
					selectedInfoString += "Rot: " + std::format("{:.2f}", obj->getRotation()) + "\n";
				}

				// Scale
				if (soiScale) {
					selectedInfoString += "Scale: " + std::format("{:.2f}", obj->m_scaleX) + "\n";
				}

				// Colors
				if (soiColor) {
					auto gjBaseColor = obj->m_baseColor;
					auto gjDetailColor = obj->m_detailColor;

					if (gjDetailColor) {

						// Color1 & Color2 ids
						int baseColorID = gjBaseColor->m_colorID;
						int detailColorID = gjDetailColor->m_colorID;

						selectedInfoString += "Color 1: " + std::to_string(baseColorID) + "\n";
						if (soiHSV) selectedInfoString += std::format("HSV: {:.2f} ", gjBaseColor->m_hsv.h) + std::format("{:.2f} ", gjBaseColor->m_hsv.s) + std::format("{:.2f}\n", gjBaseColor->m_hsv.v);

						selectedInfoString += "Color 2: " + std::to_string(detailColorID) + "\n";
						if (soiHSV) selectedInfoString += std::format("HSV: {:.2f} ", gjDetailColor->m_hsv.h) + std::format("{:.2f} ", gjDetailColor->m_hsv.s) + std::format("{:.2f}\n", gjDetailColor->m_hsv.v);
					}
					else {
						// Color1 & Color2 ids
						int baseColorID = gjBaseColor->m_colorID;
						selectedInfoString += "Color: " + std::to_string(baseColorID) + "\n";

						if (soiHSV) selectedInfoString += std::format("HSV: {:.2f} ", gjBaseColor->m_hsv.h) + std::format("{:.2f} ", gjBaseColor->m_hsv.s) + std::format("{:.2f}\n", gjBaseColor->m_hsv.v);
					}
				}

				// Groups
				if (obj->m_groups && soiGroups) {
					selectedInfoString += "Groups: ";
					for (int i = 0; i < obj->m_groups->size(); i++) {
						selectedInfoString += std::to_string(obj->m_groups->at(i));
						if (i + 1 < obj->m_groups->size()) {
							if (obj->m_groups->at(i + 1) == 0) break;
							else selectedInfoString += ", ";
						}
					}
					selectedInfoString += "\n";
				}

				// Z Layer
				if (soiZLayer) {
					if (!zLayerMap.contains(obj->m_zLayer)) {
						selectedInfoString += "Z Layer: What?\n";
					}
					else {
						selectedInfoString += "Z Layer: " + zLayerMap[obj->m_zLayer] + "\n";
					}
				}

				// Z Order
				if (soiZOrder) selectedInfoString += "Z Order: " + std::to_string(obj->m_zOrder) + "\n";

				// Object ID
				if (soiObjectID) selectedInfoString += "Object ID: " + std::to_string(obj->m_objectID) + "\n";

				// Object Type
				std::string typeStr = ErGui::objectTypes[(int)obj->m_objectType];
				if (soiObjectType) selectedInfoString += "Type: " + typeStr + "\n";

				// Target Group ID
				if (eObj && soiTargetGroup) {
					selectedInfoString += "Target Group: " + std::to_string(eObj->m_targetGroupID) + "\n";
				}

				// Is Hidden
				if (soiHidden) {
					if (obj->m_isHide)
						selectedInfoString += "Hidden\n";
					else
						selectedInfoString += "Shown\n";
				}

				// Is Touchable
				if (soiNoTouch) {
					if (obj->m_isNoTouch)
						selectedInfoString += "No Touch\n";
					else
						selectedInfoString += "Touchable\n";
				}

				// Is High Detail
				if (soiHighDetail) {
					if (obj->m_isHighDetail)
						selectedInfoString += "High Detail\n";
					else
						selectedInfoString += "Low Detail\n";
				}

				// Particle Count
				if (soiParticles) {
					int particleCount = 0;
					if (auto pobj = typeinfo_cast<ParticleGameObject*>(obj)) {
						std::string particleSize = pobj->m_particleData;
						if (particleSize != "") {
							particleSize = particleSize.erase(particleSize.find_first_of('a'));
							particleCount += std::atoi(particleSize.c_str());
						}

						selectedInfoString += std::to_string(particleCount) + " Particles\n";
					}
				}

				// Item ID
				if (objsWithItemIDs.contains(obj->m_objectID) && soiItemID) {
					if (eObj) {
						selectedInfoString += "Item ID: " + std::to_string(eObj->m_itemID);
					}
				}

				// Block ID
				if (objsWithBlockIDs.contains(obj->m_objectID) && soiBlockID) {
					if (eObj) {
						selectedInfoString += "Block ID: " + std::to_string(eObj->m_itemID);
					}
				}
			}
			else if (objArr->count() > 0) {
				//---- Multiple Selected
				// Object Count //
				// Groups (Range) //
				// Z Layer (Range) //
				// Z Order (Range) //
				// Target Group (Range) //
				// Item ID (Range) // -
				// Block ID (Range) // -
				// Particle Count //
				//

				// Object Count 
				if (soiObjectCount) selectedInfoString += "Objects: " + std::to_string(objArr->count()) + "\n";
				auto randomObj = typeinfo_cast<GameObject*>(objArr->objectAtIndex(0));

				int minBaseColor = randomObj->m_baseColor->m_colorID;
				int maxBaseColor = randomObj->m_baseColor->m_colorID;

				std::optional<int> minDetailColor;
				std::optional<int> maxDetailColor;
				
				std::set<int> groupIDs;

				ZLayer minZLayer = randomObj->m_zLayer;
				ZLayer maxZLayer = randomObj->m_zLayer;

				int minZOrder = randomObj->m_zOrder;
				int maxZOrder = randomObj->m_zOrder;

				std::set<int> targetGroups;

				std::set<int> itemIDs;

				std::set<int> blockIDs;

				int particleCount = 0;	
				for (auto objInArr : CCArrayExt<GameObject*>(objArr)) {

					auto eObj = typeinfo_cast<EffectGameObject*>(objInArr);

					// Base Color
					if (objInArr->m_baseColor->m_colorID < minBaseColor) minBaseColor = objInArr->m_baseColor->m_colorID;
					if (objInArr->m_baseColor->m_colorID > maxBaseColor) maxBaseColor = objInArr->m_baseColor->m_colorID;

					// Detail Color
					if (objInArr->m_detailColor) {
						if (objInArr->m_detailColor->m_colorID < minDetailColor) minDetailColor = objInArr->m_detailColor->m_colorID;
						if (objInArr->m_detailColor->m_colorID > maxDetailColor) maxDetailColor = objInArr->m_detailColor->m_colorID;
					}

					// Groups
					if (objInArr->m_groups) {
						for (const auto& id : *objInArr->m_groups) {
							if (id == 0) break;
							groupIDs.insert(id);
						}
					}

					// Z Layer
					if (objInArr->m_zLayer < minZLayer) minZLayer = objInArr->m_zLayer;
					if (objInArr->m_zLayer > maxZLayer) maxZLayer = objInArr->m_zLayer;

					// Z Order
					if (objInArr->m_zOrder < minZOrder) minZOrder = objInArr->m_zOrder;
					if (objInArr->m_zOrder > maxZOrder) maxZOrder = objInArr->m_zOrder;


					if (eObj) {
						// Target Group
						if (eObj->m_targetGroupID != 0)
							targetGroups.insert(eObj->m_targetGroupID);


						// Item ID
						if (objsWithItemIDs.contains(eObj->m_objectID)) {
							itemIDs.insert(eObj->m_itemID);
						}

						// Block ID
						if (objsWithBlockIDs.contains(eObj->m_objectID)) {
							blockIDs.insert(eObj->m_itemID);
						}
					}

					// Particle Count
					if (auto pobj = typeinfo_cast<ParticleGameObject*>(objInArr)) {
						std::string particleSize = pobj->m_particleData;
						if (particleSize != "") {
							particleSize = particleSize.erase(particleSize.find_first_of('a'));
							particleCount += std::atoi(particleSize.c_str());
						}
					}
				}

				// Groups
				if (!groupIDs.empty() && soiGroups) {
					makeLabelFromSet(selectedInfoString, groupIDs, "Group", "Groups");
				}

				// ZLayer
				if (soiZLayer) {
					if (minZLayer == maxZLayer) {
						selectedInfoString += "Z Layer: " + zLayerMap[minZLayer] + "\n";
					}
					else {
						selectedInfoString += "Z Layer: " + zLayerMap[minZLayer] + " - " + zLayerMap[maxZLayer] + "\n";
					}
				}

				// ZOrder
				if (soiZOrder) {
					if (minZOrder == maxZOrder) {
						selectedInfoString += "Z Order: " + std::to_string(minZOrder) + "\n";
					}
					else {
						selectedInfoString += "Z Order: " + std::to_string(minZOrder) + " - " + std::to_string(maxZOrder) + "\n";
					}
				}

				// Target Group
				if (soiTargetGroup) makeLabelFromSet(selectedInfoString, targetGroups, "Target Group", "Target Groups");

				// ItemID
				if (soiItemID) makeLabelFromSet(selectedInfoString, itemIDs, "ItemID", "ItemIDs");

				// BlockID
				if (soiBlockID) makeLabelFromSet(selectedInfoString, blockIDs, "BlockID", "BlockIDs");

				// Particle Count
				if (particleCount > 0 && soiParticles) {
					selectedInfoString += std::to_string(particleCount) + " Particles\n";
				}
			}

			

			label->setString(selectedInfoString.c_str());
		}
	}

	bool init(LevelEditorLayer* lel) {
		// Orig
		auto selectedObjectInfo = CCLabelBMFont::create("", "chatFont.fnt");
		selectedObjectInfo->setAnchorPoint({ 0.f, 1.f });
		selectedObjectInfo->setPosition({ 20.f, CCDirector::sharedDirector()->getScreenTop() - 20.f });
		selectedObjectInfo->setScale(0.66f);
		selectedObjectInfo->setID("object-info-label"_spr);
		selectedObjectInfo->setZOrder(1);
		lel->addChild(selectedObjectInfo);

		if (!GameManager::sharedState()->getGameVariable("0041"))
			selectedObjectInfo->setVisible(false);

		return true;
	}

	void selectObjectsBefore() {
		s_isSelecting = true;
		g_soiNeedsUpdate = true;
		// Orig
	}
	void selectObjectsAfter() {
		// Orig
		s_isSelecting = false;
	}

	void selectObjectBefore() {
		s_isSelecting = true;
		g_soiNeedsUpdate = true;
		// Orig
	}
	void selectObjectAfter() {
		// Orig
		s_isSelecting = false;
	}

	void deselectAll() {
		if (!s_isSelecting) {
			g_soiNeedsUpdate = true;
		}
		// Orig
	}

	void onDelete() {
		// Orig
		g_soiNeedsUpdate = true;
	}

	void onDeleteSelected() {
		// Orig
		g_soiNeedsUpdate = true;
	}

	void undoLastAction() {
		// Orig
		g_soiNeedsUpdate = true;
	}

	void redoLastAction() {
		// Orig
		g_soiNeedsUpdate = true;
	}
}

class $modify(LevelEditorLayer) {
	void removeObject(GameObject * p0, bool p1) {
		LevelEditorLayer::removeObject(p0, p1);
		g_soiNeedsUpdate = true;
	}
};