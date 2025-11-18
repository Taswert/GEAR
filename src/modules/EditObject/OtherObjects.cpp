#include "OtherObjects.hpp"
using namespace ErGui;


void drawShakeSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Strength", &eObj->m_shakeStrength, .05f, 0.f, 5.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Interval", &eObj->m_shakeInterval, .01f, 0.f, .1f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Duration", &eObj->m_duration, .05f, 0.f, 2.f);

	drawTouchSpawnTriggered(eObj);
}

void drawRotateGameplaySettings(GameObject* obj, CCArray* objArr) {
	auto rObj = static_cast<RotateGameplayGameObject*>(obj);

	ImGui::Checkbox("Edit Velocity", &rObj->m_editVelocity);

	if (rObj->m_editVelocity) {
		ImGui::SameLine(150.f);
		ImGui::Checkbox("Override Velocity", &rObj->m_overrideVelocity);

		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("VelMod X", &rObj->m_velocityModX, .05f, -1.f, 1.f);

		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("VelMod Y", &rObj->m_velocityModY, .05f, -1.f, 1.f);
	}

	ImGui::Checkbox("Change Channel", &rObj->m_changeChannel);

	if (rObj->m_changeChannel) {
		ImGui::SameLine(150.f);
		ImGui::Checkbox("Channel Only", &rObj->m_channelOnly);

		ImGui::InputInt("Target Channel", &rObj->m_targetChannelID);
	}

	ImGui::Checkbox("Don't Slide", &rObj->m_dontSlide);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Instant Offset", &rObj->m_instantOffset);

	drawTouchSpawnTriggered(rObj);
}

void gameEventCheckbox(EventLinkTrigger*& eventObject, const char* name, GJGameEvent event) {
	auto intEvent = static_cast<int>(event);

	bool value = eventObject->m_eventIDs.contains(intEvent);
	if (ImGui::Checkbox(name, &value)) {
		if (value) eventObject->m_eventIDs.insert(intEvent);
		else eventObject->m_eventIDs.erase(intEvent);
	}
}

void drawEventSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EventLinkTrigger*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Extra ID", &eObj->m_extraID);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Extra ID2", &eObj->m_extraID2);

	if (ImGui::CollapsingHeader("Landing / Hitting")) {
		gameEventCheckbox(eObj, "Tiny Landing", GJGameEvent::TinyLanding);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Feather Landing", GJGameEvent::FeatherLanding);
		gameEventCheckbox(eObj, "Soft Landing", GJGameEvent::SoftLanding);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Normal Landing", GJGameEvent::NormalLanding);
		gameEventCheckbox(eObj, "Hard Landing", GJGameEvent::HardLanding);

		gameEventCheckbox(eObj, "Hit Head", GJGameEvent::HitHead);
	}

	if (ImGui::CollapsingHeader("Interactions")) {
		gameEventCheckbox(eObj, "Orb Touched", GJGameEvent::OrbTouched);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Orb Activated", GJGameEvent::OrbActivated);

		gameEventCheckbox(eObj, "Dash Start", GJGameEvent::DashStart);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Dash Stop", GJGameEvent::DashStop);

		gameEventCheckbox(eObj, "Pad Activated", GJGameEvent::PadActivated);

		gameEventCheckbox(eObj, "Gravity Inverted", GJGameEvent::GravityInverted);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Gravity Restored", GJGameEvent::GravityRestored);

		gameEventCheckbox(eObj, "Teleported", GJGameEvent::Teleported);

		gameEventCheckbox(eObj, "Normal Jump", GJGameEvent::NormalJump);

		gameEventCheckbox(eObj, "Ship Boost Start", GJGameEvent::ShipBoostStart);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Ship Boost End", GJGameEvent::ShipBoostEnd);

		gameEventCheckbox(eObj, "Ball Switch", GJGameEvent::BallSwitch);

		gameEventCheckbox(eObj, "UFO Jump", GJGameEvent::UFOJump);

		gameEventCheckbox(eObj, "Wave Push", GJGameEvent::WavePush);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Wave Release", GJGameEvent::WaveRelease);

		gameEventCheckbox(eObj, "Robot Boost Start", GJGameEvent::RobotBoostStart);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Robot Boost Stop", GJGameEvent::RobotBoostStop);

		gameEventCheckbox(eObj, "Spider Teleport", GJGameEvent::SpiderTeleport);

		gameEventCheckbox(eObj, "Swing Switch", GJGameEvent::SwingSwitch);
	}

	if (ImGui::CollapsingHeader("Portal Interactions")) {
		gameEventCheckbox(eObj, "Portal: Cube", GJGameEvent::PortalNormal);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Portal: Ship", GJGameEvent::PortalShip);
		gameEventCheckbox(eObj, "Portal: Ball", GJGameEvent::PortalBall);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Portal: UFO", GJGameEvent::PortalUFO);
		gameEventCheckbox(eObj, "Portal: Wave", GJGameEvent::PortalWave);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Portal: Robot", GJGameEvent::PortalRobot);
		gameEventCheckbox(eObj, "Portal: Spider", GJGameEvent::PortalSpider);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Portal: Swing", GJGameEvent::PortalSwing);

		gameEventCheckbox(eObj, "Portal: Gravity Flip", GJGameEvent::PortalGravityFlip);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Portal: Gravity Normal", GJGameEvent::PortalGravityNormal);
		gameEventCheckbox(eObj, "Portal: Gravity Invert", GJGameEvent::PortalGravityInvert);

		gameEventCheckbox(eObj, "Portal: Mirror", GJGameEvent::PortalFlip);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Portal: UnMirror", GJGameEvent::PortalUnFlip);

		gameEventCheckbox(eObj, "Portal: Normal Size", GJGameEvent::PortalNormalScale);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Portal: Mini Size", GJGameEvent::PortalMiniScale);

		gameEventCheckbox(eObj, "Portal: Dual On", GJGameEvent::PortalDualOn);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Portal: Dual Off", GJGameEvent::PortalDualOff);

		gameEventCheckbox(eObj, "Portal: Teleport", GJGameEvent::PortalTeleport);
	}

	if (ImGui::CollapsingHeader("Orbs & Pads")) {
		gameEventCheckbox(eObj, "Yellow Orb", GJGameEvent::YellowOrb);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Pink Orb", GJGameEvent::PinkOrb);
		gameEventCheckbox(eObj, "Red Orb", GJGameEvent::RedOrb);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Gravity Orb", GJGameEvent::GravityOrb);
		gameEventCheckbox(eObj, "Green Orb", GJGameEvent::GreenOrb);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Drop Orb", GJGameEvent::DropOrb);
		gameEventCheckbox(eObj, "Custom Orb", GJGameEvent::CustomOrb);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Dash Orb", GJGameEvent::DashOrb);
		gameEventCheckbox(eObj, "Gravity Dash Orb", GJGameEvent::GravityDashOrb);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Spider Orb", GJGameEvent::SpiderOrb);
		gameEventCheckbox(eObj, "Teleport Orb", GJGameEvent::TeleportOrb);

		gameEventCheckbox(eObj, "Yellow Pad", GJGameEvent::YellowPad);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Pink Pad", GJGameEvent::PinkPad);
		gameEventCheckbox(eObj, "Red Pad", GJGameEvent::RedPad);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Gravity Pad", GJGameEvent::GravityPad);
		gameEventCheckbox(eObj, "Spider Pad", GJGameEvent::SpiderPad);
	}

	if (ImGui::CollapsingHeader("Collectibles")) {
		gameEventCheckbox(eObj, "Checkpoint", GJGameEvent::Checkpoint);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Destroy Block", GJGameEvent::DestroyBlock);
		gameEventCheckbox(eObj, "User Coin", GJGameEvent::UserCoin);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Pickup Item", GJGameEvent::PickupItem);
	}

	if (ImGui::CollapsingHeader("Falling")) {
		gameEventCheckbox(eObj, "Fall Low", GJGameEvent::FallLow);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Fall Medium", GJGameEvent::FallMed);
		gameEventCheckbox(eObj, "Fall High", GJGameEvent::FallHigh);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Fall VHigh", GJGameEvent::FallVHigh);

		gameEventCheckbox(eObj, "Fall Speed Low", GJGameEvent::FallSpeedLow);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Fall Speed Medium", GJGameEvent::FallSpeedMed);
		gameEventCheckbox(eObj, "Fall Speed High", GJGameEvent::FallSpeedHigh);
	}

	if (ImGui::CollapsingHeader("Player Actions")) {
		gameEventCheckbox(eObj, "Jump Push", GJGameEvent::JumpPush);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Jump Release", GJGameEvent::JumpRelease);

		gameEventCheckbox(eObj, "Left Push", GJGameEvent::LeftPush);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Left Release", GJGameEvent::LeftRelease);

		gameEventCheckbox(eObj, "Right Push", GJGameEvent::RightPush);
		ImGui::SameLine(160.f);
		gameEventCheckbox(eObj, "Right Release", GJGameEvent::RightRelease);

		gameEventCheckbox(eObj, "Player Reversed", GJGameEvent::PlayerReversed);

		gameEventCheckbox(eObj, "Checkpoint Respawn", GJGameEvent::CheckpointRespawn);
	}

	ImGui::Separator();

	drawTouchSpawnTriggered(eObj);
}

void drawGravitySettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Gravity", &eObj->m_gravityValue, .05f, .1f, 2.f);

	if (ImGui::Checkbox("P1", &eObj->m_targetPlayer1)) {
		eObj->m_targetPlayer2 = false;
		eObj->m_followCPP = false;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("P2", &eObj->m_targetPlayer2)) {
		eObj->m_targetPlayer1 = false;
		eObj->m_followCPP = false;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("PT", &eObj->m_followCPP)) {
		eObj->m_targetPlayer1 = false;
		eObj->m_targetPlayer2 = false;
	}

	drawTouchSpawnTriggered(eObj);
}

void drawPlayerControlSettings(GameObject* obj, CCArray* objArr) {
	auto pObj = static_cast<PlayerControlGameObject*>(obj);

	ImGui::Checkbox("P1", &pObj->m_targetPlayer1);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("P2", &pObj->m_targetPlayer2);

	ImGui::Checkbox("Stop Jump", &pObj->m_stopJump);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Stop Move", &pObj->m_stopMove);

	ImGui::Checkbox("Stop Rotation", &pObj->m_stopRotation);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Stop Slide", &pObj->m_stopSlide);

	drawTouchSpawnTriggered(pObj);
}

void gameOptionsCheckbox(GameOptionsSetting& setting, const char* label) {
	bool off = static_cast<int>(setting == GameOptionsSetting::Off);
	bool on = static_cast<int>(setting == GameOptionsSetting::On);

	ImGui::Text(label);
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox(std::string("On##" + (std::string)label).c_str(), &on)) {
		off = false;
		if (on) setting = GameOptionsSetting::On;
		else setting = GameOptionsSetting::Disabled;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox(std::string("Off##" + (std::string)label).c_str(), &off)) {
		on = false;
		if (off) setting = GameOptionsSetting::Off;
		else setting = GameOptionsSetting::Disabled;
	}
}

void drawGameOptionsSettings(GameObject* obj, CCArray* objArr) {
	auto gObj = static_cast<GameOptionsTrigger*>(obj);

	gameOptionsCheckbox(gObj->m_streakAdditive, "Streak Additive");
	gameOptionsCheckbox(gObj->m_hideGround, "Hide Ground");
	gameOptionsCheckbox(gObj->m_hideMG, "Hide Middle Ground");
	gameOptionsCheckbox(gObj->m_hideP1, "Hide Player 1");
	gameOptionsCheckbox(gObj->m_hideP2, "Hide Player 2");
	gameOptionsCheckbox(gObj->m_disableP1Controls, "Disable P1 Controls");
	gameOptionsCheckbox(gObj->m_disableP2Controls, "Disable P2 Controls");
	gameOptionsCheckbox(gObj->m_unlinkDualGravity, "Unlink Dual Gravity");
	gameOptionsCheckbox(gObj->m_hideAttempts, "Hide Attempts");
	gameOptionsCheckbox(gObj->m_audioOnDeath, "Audio on Death");
	gameOptionsCheckbox(gObj->m_noDeathSFX, "No Death SFX");
	gameOptionsCheckbox(gObj->m_editRespawnTime, "Edit Respawn Time");
	gameOptionsCheckbox(gObj->m_boostSlide, "Boost Slide");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Respawn Time", &gObj->m_respawnTime, .05f, .1f, 10.f);

	drawTouchSpawnTriggered(gObj);
}

void drawTeleportSettings(GameObject* obj, CCArray* objArr) {
	auto tObj = static_cast<TeleportPortalObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target Group ID", &tObj->m_targetGroupID)) {
		if (tObj->m_targetGroupID < 0) tObj->m_targetGroupID = 0;
		if (tObj->m_targetGroupID > 9999) tObj->m_targetGroupID = 9999;
	}

	bool normal = tObj->m_gravityMode == 1;
	bool flipped = tObj->m_gravityMode == 2;
	bool toggle = tObj->m_gravityMode == 3;

	ImGui::Separator();

	if (ImGui::Checkbox("Normal Gravity", &normal)) {
		flipped = false;
		toggle = false;
		if (normal) tObj->m_gravityMode = 1;
		else tObj->m_gravityMode = 0;
	}
	if (ImGui::Checkbox("Flipped Gravity", &flipped)) {
		normal = false;
		toggle = false;
		if (normal) tObj->m_gravityMode = 2;
		else tObj->m_gravityMode = 0;
	}
	if (ImGui::Checkbox("Toggle Gravity", &toggle)) {
		normal = false;
		flipped = false;
		if (normal) tObj->m_gravityMode = 3;
		else tObj->m_gravityMode = 0;
	}

	ImGui::Separator();

	ImGui::Checkbox("Smooth Ease", &tObj->m_teleportEase);
	ImGui::Checkbox("Save Offset", &tObj->m_saveOffset);

	ImGui::Checkbox("Ignore X", &tObj->m_ignoreX);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Ignore Y", &tObj->m_ignoreY);

	ImGui::Checkbox("Instant Camera", &tObj->m_instantCamera);
	ImGui::Checkbox("Snap Ground", &tObj->m_snapGround);
	ImGui::Checkbox("Redirect Dash", &tObj->m_redirectDash);

	ImGui::Separator();

	if (ImGui::Checkbox("Static Force", &tObj->m_staticForceEnabled))
		tObj->m_redirectForceEnabled = false;
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("Redirect Force", &tObj->m_redirectForceEnabled))
		tObj->m_staticForceEnabled = false;

	if (tObj->m_staticForceEnabled) {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Force", &tObj->m_staticForce, .05f, 0.f, 30.f);

		ImGui::Checkbox("Additive", &tObj->m_staticForceAdditive);
	}
	if (tObj->m_redirectForceEnabled) {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Min", &tObj->m_redirectForceMin, .05f, 0.f, 10.f);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Max", &tObj->m_redirectForceMax, .05f, 0.f, 30.f);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Mod", &tObj->m_redirectForceMod, .05f, 0.f, 2.f);
	}

	if (obj->m_objectID == 747 || obj->m_objectID == 2902 || obj->m_objectID == 3027)
		ImGui::Checkbox("Multi Activate", &tObj->m_isMultiActivate);
	else
		drawTouchSpawnTriggered(tObj);
}

void drawEndTriggerSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EndTriggerGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Spawn ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target Pos", &eObj->m_centerGroupID)) {
		if (eObj->m_centerGroupID < 0) eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999) eObj->m_centerGroupID = 9999;
	}

	ImGui::Checkbox("No Effects", &eObj->m_noEffects);
	ImGui::Checkbox("No SFX", &eObj->m_noSFX);
	ImGui::Checkbox("Instant", &eObj->m_instant);

	drawTouchSpawnTriggered(eObj);
}

void drawTextObjectSettings(GameObject* obj, CCArray* objArr) {
	auto lel = LevelEditorLayer::get();
	auto eObj = static_cast<TextGameObject*>(obj);

	if (ImGui::InputTextMultiline("Text", &eObj->m_text, ImVec2(ErGui::INPUT_ITEM_WIDTH, ImGui::GetFrameHeight() * 2.5f))) {
		eObj->updateTextObject(eObj->m_text, false);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragInt("Kerning", &eObj->m_kerning, 1.f, -10, 20, "%d", 0)) {
		eObj->updateTextKerning(eObj->m_kerning);
		eObj->updateTextObject(eObj->m_text, false);
	}

	if (ImGui::Button("Split")) {
		lel->breakApartTextObject(eObj);
	}
}

void drawCheckpointSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<CheckpointGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Spawn ID", &cObj->m_targetGroupID)) {
		if (cObj->m_targetGroupID < 0) cObj->m_targetGroupID = 0;
		if (cObj->m_targetGroupID > 9999) cObj->m_targetGroupID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target Pos", &cObj->m_centerGroupID)) {
		if (cObj->m_centerGroupID < 0) cObj->m_centerGroupID = 0;
		if (cObj->m_centerGroupID > 9999) cObj->m_centerGroupID = 9999;
	}

	ImGui::Checkbox("Player Pos", &cObj->m_targetPlayer1);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Respawn ID", &cObj->m_respawnID);

	drawTouchSpawnTriggered(cObj);
}

void drawObjectControlSettings(GameObject* obj, CCArray* objArr) { // smol
	auto cObj = static_cast<ObjectControlGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target ID", &cObj->m_targetGroupID)) {
		if (cObj->m_targetGroupID < 0) cObj->m_targetGroupID = 0;
		if (cObj->m_targetGroupID > 9999) cObj->m_targetGroupID = 9999;
	}

	drawTouchSpawnTriggered(cObj);
}

void drawForceBlockSettings(GameObject* obj, CCArray* objArr) {
	auto fObj = static_cast<ForceBlockGameObject*>(obj);

	if (!fObj->m_forceRange) {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Force", &fObj->m_force, .05f, 0.f, 5.f);
	}
	else {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		ImGui::DragFloat("##MINFORCE", &fObj->m_minForce, .05f, 0.f, 5.f);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		ImGui::DragFloat("Min / Max Force", &fObj->m_maxForce, .05f, 0.f, 5.f);
	}

	ImGui::Checkbox("Relative", &fObj->m_relativeForce);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Range", &fObj->m_forceRange);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Force ID", &fObj->m_forceID);
}

void ErGui::mapOtherTriggers() {
	// todo: Ghost/Hide/BGEffect
	triggersMap[1520] = drawShakeSettings;
	triggersMap[2900] = drawRotateGameplaySettings;
	triggersMap[3604] = drawEventSettings;
	triggersMap[2899] = drawGameOptionsSettings;
	triggersMap[1932] = drawPlayerControlSettings;
	triggersMap[2066] = drawGravitySettings;
	triggersMap[3022] = drawTeleportSettings; // teleport trigger
	triggersMap[3600] = drawEndTriggerSettings;
	triggersMap[914] = drawTextObjectSettings;
	triggersMap[2063] = drawCheckpointSettings;
	triggersMap[3655] = drawObjectControlSettings;
}

void ErGui::mapOtherObjects() {
	// Teleport
	triggersMap[747] = drawTeleportSettings; // linked teleport portal
	triggersMap[2902] = drawTeleportSettings; // single teleport portal
	triggersMap[3027] = drawTeleportSettings; // teleport orb

	// Force Blocks
	triggersMap[2069] = drawForceBlockSettings;
	triggersMap[3645] = drawForceBlockSettings;
}