#include "FooterModule.hpp"
#include "razoomUtils.hpp"

int getLengthKey(int time, bool isPlat) {
	if (isPlat) return 5;

	if (time < 10) return 0;
	else if (time < 30) return 1;
	else if (time < 60) return 2;
	else if (time < 120) return 3;
	else return 4;
}

void ErGui::renderFooter() {
	ImGui::Begin("Footer");
	ImGui::GetWindowDockNode()->LocalFlags = ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDocking;

	auto lel = LevelEditorLayer::get();
	
	//Time + Level Length
	float tfp = lel->m_drawGridLayer->timeForPos({ getLastObjectXFast() + 340.f, 0 }, 0, 0, 0, 0, 0, 0);
	int minutes = tfp / 60;
	float seconds = std::fmodf(tfp, 60.f);
	std::string timeStr;
	if (minutes < 1)
		timeStr = CCString::createWithFormat("%.2fs", seconds)->m_sString;
	else
		timeStr = CCString::createWithFormat("%dm %.1fs", minutes, seconds)->m_sString;

	//SFXes
	gd::string sfxIDs = lel->getSFXIDs();
	auto countSFXIDs = std::count(sfxIDs.begin(), sfxIDs.end(), ',') + 1;
	if (sfxIDs.empty()) countSFXIDs = 0;

	//Songs
	bool isFound = false;
	gd::string songIDs = lel->getSongIDs(isFound);
	auto countSongIDs = std::count(songIDs.begin(), songIDs.end(), ',') + 1;
	if (songIDs.empty()) countSongIDs = 0;

	//Particles
	int maxParticleCount = 0;
	for (const auto& particle : CCArrayExt<ParticleGameObject*>(lel->m_particleObjects)) {
		std::string particleSize = particle->m_particleData;
		if (particleSize != "") {
			particleSize = particleSize.erase(particleSize.find_first_of('a'));
			maxParticleCount += std::atoi(particleSize.c_str());
		}
	}


	//MAIN FOOTER TEXT
	ImGui::Text("Objs: %d", lel->m_objectCount.value());
	ImGui::SameLine();
	ImGui::Text("|");
	ImGui::SameLine();

	ImGui::Text("%s", timeStr.c_str());
	ImGui::SameLine();
	ImGui::Text("|");
	ImGui::SameLine();

	ImGui::Text("%s", GJGameLevel::lengthKeyToString(getLengthKey(std::floor(tfp), lel->m_isPlatformer)).c_str());
	ImGui::SameLine();
	ImGui::Text("|");
	ImGui::SameLine();

	ImGui::Text("SFX: %d", countSFXIDs);
	ImGui::SameLine();
	ImGui::Text("|");
	ImGui::SameLine();

	if (countSongIDs > 21) {
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.00f, 0.76f, 0.31f, 1.00f });
		ImGui::Text("Songs: %d/21", countSongIDs);
		ImGui::PopStyleColor();
	}
	else
		ImGui::Text("Songs: %d/21", countSongIDs);
	ImGui::SameLine();
	ImGui::Text("|");
	ImGui::SameLine();
	if (maxParticleCount > 100000) {
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.00f, 0.76f, 0.31f, 1.00f });
		ImGui::Text("Max Particles: %d", maxParticleCount);
		ImGui::PopStyleColor();
	}
	else
		ImGui::Text("Max Particles: %d", maxParticleCount);
	ImGui::SameLine();
	ImGui::Text("|");
	ImGui::SameLine();
	ImGui::Text(geode::getMod()->getVersion().toVString().c_str());
	ImGui::SameLine();
	ImGui::Text("|");
	ImGui::SameLine();
	ImGui::Text("FPS: %d", (int) ErGui::getFPS());

	//ImGui::Text("ArraySizes: %d %d %d %d %d %d %d %d %d %d %d", lel->m_unkArr0->count(), lel->m_unkArr1->count(), lel->m_unkArr2->count(),
	//lel->m_unkArr3->count(), lel->m_unkArr4->count(), lel->m_unkArr5->count(), lel->m_unkArr7->count(),
	//lel->m_unkArr8->count(), lel->m_unkArr11->count(), lel->m_unkArr12->count(), lel->m_unkArr13->count());

	ImGui::End();
}




//    
// GEODE_PAD(156);
// EffectGameObject* m_playbackObject;
