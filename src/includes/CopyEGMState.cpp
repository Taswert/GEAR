#include "CopyEGMState.hpp"

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

	if (auto egObj = typeinfo_cast<EffectGameObject*>(obj)) {
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

	if (auto egObj = typeinfo_cast<EffectGameObject*>(obj)) {
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