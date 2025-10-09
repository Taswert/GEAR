#include "Geode/modify/LevelEditorLayer.hpp"
#include "myUtils.hpp"
using namespace geode::prelude;

void drawObjectBoxes(GameObject* obj) {
	auto objLayer = EditorUI::get()->m_editorLayer->m_objectLayer;
	auto cameraPos = objLayer->getPosition();
	auto cameraScale = objLayer->getScale();

	auto texRect = obj->m_textureRect;

	texRect.origin = cocos2d::CCPoint(
		(texRect.origin.x * cameraScale) + cameraPos.x,
		(texRect.origin.y * cameraScale) + cameraPos.y
	);

	texRect.size *= cameraScale;

	ErGui::touchedDN->drawRect(texRect, { 1.f - 1.f / 1.33f, 1.f - 1.f / 1.33f, 0, 0.1f }, 0.3f, { 1.f, 1.f, 0, 0.33f });
}

class $modify(LevelEditorLayer) {
	void draw() {

		// Show Texture Boxes
		static bool s_drawedObjectBoxes = false;
		if (s_drawedObjectBoxes) {
			ErGui::touchedDN->clear();
		}

		if (bool debugShowObjectsBoxes = geode::Mod::get()->getSavedValue<bool>("debug-show-objects-boxes", false)) {
			ErGui::forEachObject(this, drawObjectBoxes);
			s_drawedObjectBoxes = true;
		}
		
		draw();
	}
};