#include "Geode/modify/LevelEditorLayer.hpp"
#include "myUtils.hpp"
using namespace geode::prelude;

void drawObjectBoxes(GameObject* obj) {
	auto objLayer = EditorUI::get()->m_editorLayer->m_objectLayer;
	auto cameraScale = objLayer->getScale();

	auto rot = -CC_DEGREES_TO_RADIANS(obj->getRotation());

	if (!obj || !obj->getParent())
		return;

	auto hitbox = ErGui::getObjectHitbox(obj);

	auto corners = ErGui::getCornersFromRotatedRect(hitbox.origin, hitbox.size, rot);

	ErGui::touchedDN->drawPolygon(corners.data(), 4, {1.f - 1.f / 1.33f, 1.f - 1.f / 1.33f, 0, 0.1f}, 0.3f, {1.f, 1.f, 0, 0.33f});
	//ErGui::touchedDN->drawRect(texRect, { 1.f - 1.f / 1.33f, 1.f - 1.f / 1.33f, 0, 0.1f }, 0.3f, { 1.f, 1.f, 0, 0.33f });
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