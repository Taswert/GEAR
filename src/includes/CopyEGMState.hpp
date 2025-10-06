#pragma once
#include <Geode/Geode.hpp>
#include "myUtils.hpp"
#include <array>
using namespace geode::prelude;

namespace ErGui {
	class CopyEGMState {
	private:
		std::array<short, 10> groups = {};
		int editorL1 = 0;
		int editorL2 = 0;
		int zOrder = 0;
		ZLayer zLayer = ZLayer::Default;

		//---Extra
		int enterChannel = 0;
		int material = 0;

		//---EGO
		int ego_orderVal = 0;
		int ego_channelVal = 0;
		int ego_controlID = 0;

		//---Cosmetic
		bool NoEffects = 0;
		bool DontFade = 0;
		bool NoGlow = 0;
		bool DontEnter = 0;
		bool NoParticle = 0;
		bool HighDetail = 0;
		bool NoAudioScale = 0;
		bool Hide = 0;

		//---Parent & Physics
		bool GroupParent = 0;
		bool NoTouch = 0;
		bool AreaParent = 0;
		bool Passable = 0;
		bool DontBoostX = 0;
		bool ExtendedCollision = 0;
		bool DontBoostY = 0;

		//---Platformer Only
		bool IceBlock = 0;
		bool GripSlope = 0;
		bool NonStickX = 0;
		bool ExtraSticky = 0;
		bool NonStickY = 0;
		bool ScaleStick = 0;

		//---Effect Game Object
		bool ego_SinglePTouch = 0;
		bool ego_Preview = 0;
		bool ego_CenterEffect = 0;
		bool ego_Reverse = 0;

	public:
		void copyState(GameObject* obj);
		void pasteState(GameObject* obj);
		void pasteState(CCArray* objArr);
		CopyEGMState() = default;
	};
}