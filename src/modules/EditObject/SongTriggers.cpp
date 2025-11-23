#include "SongTriggers.hpp"
using namespace ErGui;

void drawSongSettings(GameObject* obj, CCArray* objArr) {
	auto sObj = static_cast<SongTriggerGameObject*>(obj);

	auto mdm = MusicDownloadManager::sharedState();
	auto fmodAudioEngine = FMODAudioEngine::sharedEngine();

	ImGui::Checkbox("Prep", &sObj->m_prep);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Load Prep", &sObj->m_loadPrep);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Song ID", &sObj->m_soundID)) {
		if (sObj->m_soundID < 0) sObj->m_soundID = 0;
	}

	auto songObject = mdm->getSongInfoObject(sObj->m_soundID);
	log::debug("{}", songObject);

	if (songObject) {
		ImGui::Text("Name: %s", songObject->m_songName.c_str());
		ImGui::Text("Artist: %s", songObject->m_artistName.c_str());
		
		if (ImGui::Button("Play")) {
			std::string songPath = mdm->pathForSong(sObj->m_soundID);
			
			if (!fmodAudioEngine->isMusicPlaying(sObj->m_soundID))
				fmodAudioEngine->playMusic(songPath, sObj->m_loop, 0.f, 0);
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop")) {
			fmodAudioEngine->stopAllMusic(true);
		}
	}
}

void drawBPMSettings(GameObject* obj, CCArray* objArr) {
	auto aObj = static_cast<AudioLineGuideGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("BPM", &aObj->m_beatsPerMinute)) {
		if (aObj->m_beatsPerMinute < 1) aObj->m_beatsPerMinute = 1;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("BPB", &aObj->m_beatsPerBar)) {
		if (aObj->m_beatsPerBar < 1) aObj->m_beatsPerBar = 1;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Duration", &aObj->m_duration, 1.f, 1.f, 120.f, "%.0f");

	int speedIndex = 0;
	for (int i = 0; i < IM_ARRAYSIZE(speedValues); i++) {
		if ((int)aObj->m_speed == speedValues[i]) {
			speedIndex = i;
			break;
		}
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::Combo("Speed", &speedIndex, speedItems, IM_ARRAYSIZE(speedItems))) {
		(int&)aObj->m_speed = speedValues[speedIndex];
	}

	ImGui::Checkbox("Disable", &aObj->m_disabled);
} 

void ErGui::mapSongTriggers() {
	//triggersMap[1934] = drawSongSettings;
	//Edit Song
	//SFX
	//Edit SFX
	triggersMap[3642] = drawBPMSettings;
}