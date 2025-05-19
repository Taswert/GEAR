#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	class CopyEGMState {
	private:
		std::array<short, 10> groups;
		int editorL1;
		int editorL2;
		int zOrder;
		ZLayer zLayer;

		//---Extra
		int enterChannel;
		int material;

		//---EGO
		int ego_orderVal;
		int ego_channelVal;
		int ego_controlID;

		//---Cosmetic
		bool NoEffects;
		bool DontFade;
		bool NoGlow;
		bool DontEnter;
		bool NoParticle;
		bool HighDetail;
		bool NoAudioScale;
		bool Hide;

		//---Parent & Physics
		bool GroupParent;
		bool NoTouch;
		bool AreaParent;
		bool Passable;
		bool DontBoostX;
		bool ExtendedCollision;
		bool DontBoostY;

		//---Platformer Only
		bool IceBlock;
		bool GripSlope;
		bool NonStickX;
		bool ExtraSticky;
		bool NonStickY;
		bool ScaleStick;

		//---Effect Game Object
		bool ego_SinglePTouch;
		bool ego_Preview;
		bool ego_CenterEffect;
		bool ego_Reverse;

	public:
		void copyState(GameObject* obj);
		void pasteState(GameObject* obj);
		void pasteState(CCArray* objArr);
	};

	static int chosenGroupEGM = 0;
	static int groupOffset = 1;

	inline std::vector<std::pair<int, int>> groupsFromObjArr;
	inline std::vector<int> groupsCommonCount; //This array count groups for common objects. Needed for colored group buttons (array object)

	inline CopyEGMState copyStateObject;

	inline GameObject* firstObj;
	inline EffectGameObject* firstEgObj;

	inline int minEl1;
	inline int maxEl1;
	inline int minEl2;
	inline int maxEl2;
	inline int minZOrder;
	inline int maxZOrder;
	inline int minZLayer;
	inline int maxZLayer;
	inline int minEnterChannel;
	inline int maxEnterChannel;
	inline int minMaterial;
	inline int maxMaterial;


	//WELL FUCK ME!
	//---Cosmetic
	inline bool cb_NoEffects;
	inline bool cb_DontFade;
	inline bool cb_NoGlow;
	inline bool cb_DontEnter;
	inline bool cb_NoParticle;
	inline bool cb_HighDetail;
	inline bool cb_NoAudioScale;
	inline bool cb_Hide;
	
	//---Parent & Physics
	inline bool cb_GroupParent;
	inline bool cb_NoTouch;
	inline bool cb_AreaParent;
	inline bool cb_Passable;
	inline bool cb_DontBoostX;
	inline bool cb_ExtendedCollision;
	inline bool cb_DontBoostY;

	//---Platformer Only
	inline bool cb_IceBlock;
	inline bool cb_GripSlope;
	inline bool cb_NonStickX;
	inline bool cb_ExtraSticky;
	inline bool cb_NonStickY;
	inline bool cb_ScaleStick;

	//---Effect Game Object
	inline bool cb_SinglePTouch;
	inline bool cb_Preview;
	inline bool cb_CenterEffect;
	inline bool cb_Reverse;


	void renderEditGroupModule();
	void groupInfoUpdate();
}