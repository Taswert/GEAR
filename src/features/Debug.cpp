#include "Geode/modify/LevelEditorLayer.hpp"
#include "myUtils.hpp"
using namespace geode::prelude;

void drawObjectBoxes(GameObject* obj) {
	auto objLayer = EditorUI::get()->m_editorLayer->m_objectLayer;
	auto cameraScale = objLayer->getScale();

	auto rot = -CC_DEGREES_TO_RADIANS(obj->getRotation());

	if (!obj || !obj->getParent())
		return;

	auto box = obj->boundingBox();
	auto centerPoint = obj->getParent()->convertToWorldSpace({
		box.getMidX(),
		box.getMidY()
		});

	auto contentSize = obj->getScaledContentSize();

	if (auto txtObj = typeinfo_cast<TextGameObject*>(obj)) {
		auto txtChildren = txtObj->getChildren();
		if (txtChildren && txtChildren->count() > 0) {
			auto firstLetter = static_cast<CCFontSprite*>(txtChildren->firstObject());
			const auto firstBox = firstLetter->boundingBox();

			auto lastLetter = static_cast<CCFontSprite*>(txtChildren->lastObject());
			const auto lastBox = lastLetter->boundingBox();

			auto firstCenterPoint = firstLetter->getParent()->convertToWorldSpace({
				firstBox.getMidX(),
				firstBox.getMidY()
			});

			auto lastCenterPoint = lastLetter->getParent()->convertToWorldSpace({
				lastBox.getMidX(),
				lastBox.getMidY()
			});

			centerPoint = CCPoint(
				(firstCenterPoint.x + lastCenterPoint.x) / 2.f,
				(firstCenterPoint.y + lastCenterPoint.y) / 2.f
			);

			auto firstPosLocal = firstLetter->getPosition();
			auto lastPosLocal = lastLetter->getPosition();

			float dx = (lastPosLocal.x - firstPosLocal.x) * obj->m_scaleX;
			float dy = (lastPosLocal.y - firstPosLocal.y) * obj->m_scaleY;
			float centerDistLocal = sqrtf(dx * dx + dy * dy);

			auto firstSize = firstLetter->getScaledContentSize() * CCSize(obj->m_scaleX, obj->m_scaleY);
			auto lastSize = lastLetter->getScaledContentSize() * CCSize(obj->m_scaleX, obj->m_scaleY);

			float widthLocal = centerDistLocal + (firstSize.width + lastSize.width) * 0.5f;
			float heightLocal = std::max(firstSize.height, lastSize.height);

			contentSize = CCSize(widthLocal, heightLocal);

		}
	}
	contentSize *= cameraScale;

	auto corners = ErGui::getCornersFromRotatedRect(centerPoint, contentSize, rot);

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