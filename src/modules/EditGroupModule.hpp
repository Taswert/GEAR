#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
#include "CopyEGMState.hpp"

#include "SettingsModule.hpp"
using namespace geode::prelude;

namespace ErGui {
	static int chosenGroupEGM = 0;
	static int groupOffset = 1;

	inline std::vector<std::pair<int, int>> groupsFromObjArr; // First - Key (GroupID), Second - Object Count
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

	inline int minOrderValue;
	inline int maxOrderValue;
	inline int minChannel;
	inline int maxChannel;
	inline int minControlID;
	inline int maxControlID;


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