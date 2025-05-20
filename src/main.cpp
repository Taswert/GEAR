#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <imgui_internal.h>

#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>

#include <Geode/modify/CustomSongLayer.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/ObjectToolbox.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#include <IconsMaterialDesignIcons.h>
#include "modules/EditGroupModule.hpp"
#include "modules/SelectFilterModule.hpp"
#include "modules/TransformObjectModule.hpp"
#include "modules/FooterModule.hpp"
#include "modules/ToolsModule.hpp"
#include "modules/CameraSettings.hpp"
#include "modules/ObjectListModule.hpp"
#include "modules/LayerModule.hpp"
#include "modules/EditObjectModule.hpp"
#include "modules/ActionHistoryModule.hpp"
#include "modules/EditColorModule.hpp"
#include "modules/GameWindowModule.hpp"

#include "includes/ObjectCategories.hpp"
#include <matjson.hpp>
#include <matjson/reflect.hpp>
#include <matjson/std.hpp>
#include <matjson/stl_serialize.hpp>

#include <Geode/Result.hpp>

//#include <nlohmann/json.hpp>
//using json = nlohmann::json;

using namespace geode::prelude;

template <class R, class T>
R& from(T base, intptr_t offset) {
	return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
}

void initImGuiStyling() {
	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO();

	style.WindowPadding = ImVec2(8.f, 8.f);
	style.WindowRounding = 4.f;
	style.WindowBorderSize = 0.f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 4.f;
	style.ChildBorderSize = 1.f;
	style.PopupRounding = 4.f;
	style.PopupBorderSize = 1.f;
	style.FramePadding = ImVec2(6.f, 3.f);
	style.FrameRounding = 2.f;
	style.FrameBorderSize = 0.f;
	style.ItemSpacing = ImVec2(10.f, 4.f);
	style.ItemInnerSpacing = ImVec2(4.f, 4.f);
	style.CellPadding = ImVec2(4.f, 2.f);
	style.TouchExtraPadding = ImVec2(0.f, 0.f);
	style.IndentSpacing = 24.f;
	style.ScrollbarSize = 14.f;
	style.ScrollbarRounding = 6.f;
	style.GrabMinSize = 12.f;
	style.GrabRounding = 4.f;
	style.LogSliderDeadzone = 4.f;
	style.TabRounding = 4.f;
	style.TabBorderSize = 0.f;
	style.TabBarBorderSize = 2.f;
	style.TabBarOverlineSize = 0.f;
	style.TableAngledHeadersAngle = 35.f;
	style.TableAngledHeadersTextAlign = ImVec2(0.5f, 0.f);
	style.ColorButtonPosition = ImGuiDir_Left;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.f, 0.f);
	style.SeparatorTextBorderSize = 4.f;
	style.SeparatorTextAlign = ImVec2(0.f, 0.f);
	style.SeparatorTextPadding = ImVec2(40.f, 2.f);
	style.DockingSeparatorSize = 4.f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.79f, 0.79f, 0.79f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 0.71f, 0.28f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.84f, 0.52f, 0.04f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.71f, 0.28f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.84f, 0.56f, 0.14f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.71f, 0.28f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.78f, 0.78f, 0.78f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.85f, 0.54f, 0.07f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.75f, 0.75f, 0.75f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.85f, 0.54f, 0.07f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.50f, 0.08f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.65f, 0.11f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.65f, 0.11f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.65f, 0.11f, 0.95f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.97f, 0.68f, 0.25f, 0.80f);
	colors[ImGuiCol_Tab] = ImVec4(0.53f, 0.53f, 0.53f, 0.86f);
	colors[ImGuiCol_TabSelected] = ImVec4(0.82f, 0.54f, 0.11f, 1.00f);
	colors[ImGuiCol_TabSelectedOverline] = ImVec4(1.00f, 0.65f, 0.11f, 0.00f);
	colors[ImGuiCol_TabDimmed] = ImVec4(0.20f, 0.20f, 0.20f, 0.97f);
	colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
	colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(1.00f, 0.65f, 0.11f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextLink] = ImVec4(1.00f, 0.65f, 0.11f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.65f, 0.11f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavCursor] = ImVec4(1.00f, 0.65f, 0.11f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

bool filterSingleObj(bool filterBool, int objParameter, std::vector<int> vec) {
	if (!filterBool) return true;
	for (int i = 0; i < vec.size(); i++) {
		if (objParameter == vec[i])
			return true;
	}
	return false;
}

class $modify(LevelEditorLayer) {
	void removeSpecial(GameObject * obj) {
		if (obj->m_objectID == 3642) {
			for (auto alObj : this->m_drawGridLayer->m_audioLineObjects) {
				if (alObj.second == obj) {
					this->m_drawGridLayer->m_audioLineObjects.erase(alObj.first);
				}
			}
		}
		LevelEditorLayer::removeSpecial(obj);
	}
};

class $modify(ObjectToolbox) {
	float gridNodeSizeForKey(int id) {
		auto size = Mod::get()->template getSavedValue<float>("grid-size");
		if (size < 1 || roundf(size) == 30) {
			return ObjectToolbox::gridNodeSizeForKey(id);
		}
		return size;
	}
};

class $modify(EditorUI) {
	void scrollWheel(float p0, float p1) {
		//Zoom To Cursor + Expanded constractions
		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed()) {
			auto winSize = CCDirector::sharedDirector()->getWinSize();
			CCLayer* batchLayer = nullptr;
			if (this->m_editorLayer->getChildByID("main-node") != nullptr)
				batchLayer = static_cast<CCLayer*>(this->m_editorLayer->getChildByID("main-node")->getChildByID("batch-layer"));
			else if (this->m_editorLayer->m_shaderLayer != nullptr)
				batchLayer = static_cast<CCLayer*>(this->m_editorLayer->m_shaderLayer->getChildByID("main-node")->getChildByID("batch-layer"));
			else EditorUI::scrollWheel(p0, p1);

			float oldScale = batchLayer->getScale();
			CCPoint oldPosition = batchLayer->getPosition();
			CCPoint glPos = CCDirector::sharedDirector()->getOpenGLView()->getMousePosition();

			CCSize frameSize = CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();
			CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
			CCPoint visibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();

			float vx = (glPos.x / frameSize.width) * visibleSize.width + visibleOrigin.x;
			float vy = (glPos.y / frameSize.height) * visibleSize.height + visibleOrigin.y;
			vy = visibleSize.height - vy;
			CCPoint visibleCursor{ vx, vy };

			float scaleStep = oldScale * 0.1f;
			float newScale = oldScale + (p0 > 0 ? -scaleStep : scaleStep);
			newScale = clamp(newScale, 0.1f, 1000.0f);

			float ratio = newScale / oldScale;

			CCPoint delta = oldPosition - visibleCursor;
			CCPoint newPos = visibleCursor + delta * ratio;

			batchLayer->setScale(newScale);
			batchLayer->setPosition(newPos);

			if (this->m_rotationControl)
				this->m_rotationControl->setScale(1.f / newScale);

			return;
		}



		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getShiftKeyPressed())
			EditorUI::scrollWheel(p1, p0);
		else
			EditorUI::scrollWheel(p0, p1);
	}


	GameObject* createObject(int p0, CCPoint p1) {
		bool enableColorBuild1 = Mod::get()->getSavedValue<bool>("enable-build-color-1");
		bool enableColorBuild2 = Mod::get()->getSavedValue<bool>("enable-build-color-2");

		int color1 = Mod::get()->template getSavedValue<int>("build-color-1");
		int color2 = Mod::get()->template getSavedValue<int>("build-color-2");
		auto obj = EditorUI::createObject(p0, p1);
		if (obj->m_baseColor && enableColorBuild1) {
			obj->m_baseColor->m_colorID = color1;
			std::cout << "main\n";
		}
		if (obj->m_detailColor && enableColorBuild2) {
			obj->m_detailColor->m_colorID = color2;
			std::cout << "det\n";
		}
		return obj;
	}

	void selectObject(GameObject* obj, bool p1) {
		if (ErGui::selectFilterRealization(obj)) {
			if (auto eObj = static_cast<EffectGameObject*>(obj)) 
				ErGui::saveHueValues(&eObj->m_triggerTargetColor);
			EditorUI::selectObject(obj, p1);
		}
		return;
	}

	void selectObjects(CCArray* objArrInRect, bool p1) {
		
		int selectMode = Mod::get()->getSavedValue<int>("select-mode");

		//orig
		switch (selectMode)
		{
			case 3: {
				CCArray objArr;
				for (auto obj : CCArrayExt<GameObject*>(ErGui::selectFilterRealization(objArrInRect))) {
					if (obj->m_isSelected) {
						objArr.addObject(obj);
					}
				}
				EditorUI::deselectAll();
				if (objArr.count() > 0) {
					EditorUI::selectObjects(&objArr, p1);
				}
				break;
			}
			case 2: {
				for (auto obj : CCArrayExt<GameObject*>(ErGui::selectFilterRealization(objArrInRect))) {
					EditorUI::deselectObject(obj);
				}
				break;
			}
			case 1:
			default: {
				EditorUI::selectObjects(ErGui::selectFilterRealization(objArrInRect), p1);
				break;
			}
		}
		ErGui::groupInfoUpdate();
	}

	bool init(LevelEditorLayer* lel) {
		ErGui::editorUIDrawNode = CCDrawNode::create();
		this->addChild(ErGui::editorUIDrawNode);
		auto ret = EditorUI::init(lel);
		//ErGui::originalCameraPosition = lel->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();

		ErGui::og_prevMode = lel->m_previewMode;
		ErGui::og_prevShad = lel->m_previewShaders;
		ErGui::og_prevPart = lel->m_previewParticles;
		ErGui::og_prevAnim = lel->m_previewAnimations;

		this->setVisible(false);

		return ret;
	}

	virtual void keyDown(cocos2d::enumKeyCodes p0) {
		if (p0 == cocos2d::enumKeyCodes::KEY_Four) {
			GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 4;
		}

		if (p0 == cocos2d::enumKeyCodes::KEY_Five) {
			GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedMode = 5;
		}

		if (CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed() && p0 == cocos2d::enumKeyCodes::KEY_A) {
			EditorUI::processSelectObjects(this->m_editorLayer->m_objects);
			EditorUI::updateButtons();
			//EditorUI::deactivateRotationControl();
			EditorUI::deactivateScaleControl();
			EditorUI::deactivateTransformControl();
			EditorUI::updateObjectInfoLabel();
			
		}

		EditorUI::keyDown(p0);
	}

	void updateGridNodeSize() {
		auto actualMode = m_selectedMode;
		m_selectedMode = 2;
		EditorUI::updateGridNodeSize();
		m_selectedMode = actualMode;
	}

	bool ccTouchBegan(CCTouch* touch, CCEvent* event) {


		//LASSO
		if (this->m_selectedMode == 3 && ErGui::isLassoEnabled && m_swipeEnabled) {
			ErGui::editorUILassoPoints.clear();
			CCPoint pt = touch->getLocation();
			ErGui::editorUILassoPoints.push_back(pt);
			ErGui::editorUIDrawNode->clear();
			return true;
		}
		return EditorUI::ccTouchBegan(touch, event);
	}


	void ccTouchMoved(CCTouch* touch, CCEvent* event) {
		//ZOOM MODE
		if (this->m_selectedMode == 4) {
			auto editorUI = GameManager::sharedState()->getEditorLayer()->m_editorUI;
			float zoomMul = Mod::get()->template getSavedValue<float>("zoom-multiplier");
			editorUI->m_editorZoom += (touch->getLocation().x - touch->getPreviousLocation().x) * 0.01f * zoomMul;
			if (editorUI->m_editorZoom > 4.f) editorUI->m_editorZoom = 4.f;
			if (editorUI->m_editorZoom < 0.1f) editorUI->m_editorZoom = 0.1f;
			editorUI->updateZoom(editorUI->m_editorZoom);
			//GameManager::sharedState()->getEditorLayer()->m_editorUI->m_editorZoom;
			//EditorUI::get()->m_editorZoom += touch->getLocation().x - touch->getPreviousLocation().x;
			return;
		}

		//LASSO
		if (this->m_selectedMode == 3 && ErGui::isLassoEnabled && m_swipeEnabled) {
			CCPoint pt = touch->getLocation();
			ErGui::editorUILassoPoints.push_back(pt);
			ErGui::editorUIDrawNode->clear();

			if (ErGui::editorUILassoPoints.size() > 1) {
				ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUILassoPoints.data(), ErGui::editorUILassoPoints.size(), { 0, 0, 0, 0 }, 0.2f, { 0, 0.7f, 0, 1.f });
			}
			return;
		}
		EditorUI::ccTouchMoved(touch, event);
	}

	void ccTouchEnded(CCTouch* touch, CCEvent* event) {


		//LASSO
		if (ErGui::editorUILassoPoints.size() > 2 && this->m_selectedMode == 3 && ErGui::isLassoEnabled && m_swipeEnabled) {
			ErGui::editorUIDrawNode->clear();
			ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUILassoPoints.data(), ErGui::editorUILassoPoints.size(), { 0, 0, 0, 0 }, 0.2f, { 0, 1.f, 0, 1.f });

			CCArray* objArr = CCArray::create();
			for (auto obj : CCArrayExt<GameObject*>(GameManager::sharedState()->getEditorLayer()->m_objects)) {

				auto editorLayer = GameManager::sharedState()->getEditorLayer();
				auto cameraPos = editorLayer->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();
				auto cameraScale = editorLayer->getChildByID("main-node")->getChildByID("batch-layer")->getScale();

				auto objPos = obj->getPosition();
				auto newPos = cocos2d::CCPoint(
					(objPos.x * cameraScale) + cameraPos.x,
					(objPos.y * cameraScale) + cameraPos.y
				);

				if (ErGui::isPointInPolygon(newPos, ErGui::editorUILassoPoints)) {
					objArr->addObject(obj);
				}
			}
			//std::cout << "\n";
			if (objArr->count() > 0)
				this->selectObjects(objArr, false);
			CC_SAFE_RELEASE(objArr);
			ErGui::editorUIDrawNode->clear();
		}

		EditorUI::ccTouchEnded(touch, event);
	}

	void destructor() {
		CC_SAFE_RELEASE(ErGui::editorUIDrawNode);
		ErGui::editorUIDrawNode = nullptr;
		EditorUI::~EditorUI();
	}
};

//int someIterator = 0;
class $modify(CCTouchDispatcher) {
	void touches(CCSet* touches, CCEvent* event, unsigned int type) {
		//std::cout << someIterator << " - touchesHook\n";
		//someIterator++;
		CCTouchDispatcher::touches(touches, event, type);
	}
};


static bool touchHasBegan = false;
static bool touchHasBegan2 = false;
static float winMultiplier = 1.5f;
static int selectedTab = 0;


void ShowMainWindow()
{
	// Окно «MainWindow» занимает всё окно приложения
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0, 0, 0, 0));

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoInputs;
	ImGui::Begin("MainWindow", nullptr, mainWindowFlags);

	// Создаём док-спейс внутри основного окна
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));


	//if (ImGui::IsItemHovered()) {
	//	ImGuiCocos::get().setShouldPassClicks(true);
	//}

	ImGui::End(); // Завершаем "MainWindow"

	ImGui::PopStyleColor(2);
}


class $modify(CCTouchDispatcher) {
	void touches(CCSet * touches, CCEvent * event, unsigned int type) {
		auto& io = ImGui::GetIO();
		auto* touch = static_cast<CCTouch*>(touches->anyObject());

		if (touch == nullptr) {
			return;
		}

		if (ErGui::isGameWindowHovered) {
			ImGui::GetIO().WantCaptureMouse = false;
			touch->setTouchInfo(touch->getID(), ErGui::gameWindowTouchCoordinatesConvertedToWorld.x, ErGui::gameWindowTouchCoordinatesConvertedToWorld.y);
		}
		CCTouchDispatcher::touches(touches, event, type);
	}
};

$on_mod(Loaded) {
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

	if (Mod::get()->getSavedValue<float>("grid-size") == 0.f) Mod::get()->setSavedValue("grid-size", 30.f);
	if (Mod::get()->getSavedValue<float>("zoom-multiplier") == 0.f) Mod::get()->setSavedValue("zoom-multiplier", 1.f);
	if (Mod::get()->getSavedValue<int>("select-mode") == 0) Mod::get()->setSavedValue("select-mode", 1);
	//if (Mod::get()->getSavedValue<int>("build-color-1") == 0) Mod::get()->setSavedValue("build-color-1", 1);
	//if (Mod::get()->getSavedValue<int>("build-color-2") == 0) Mod::get()->setSavedValue("build-color-2", 1);
	//if (Mod::get()->getSavedValue<bool>("enable-build-color-1") == 0) Mod::get()->setSavedValue("enable-build-color-1", 1);
	//if (Mod::get()->getSavedValue<bool>("enable-build-color-2") == 0) Mod::get()->setSavedValue("enable-build-color-2", 1);



	//Json is not working properly now, I'll fix that a bit later...

	auto cfgDir = Mod::get()->getSettingValue<std::filesystem::path>("object-list-config");
	std::map<const char*, std::vector<ErGui::ObjectConfig>> data = ErGui::getDefaultObjectCfg();

	matjson::Value j;

	for (const auto& kv : data) {
		j[kv.first] = kv.second;
	}

	//std::cout << j.dump() << "\n\n";
	std::cout << cfgDir << "\n\n";
	std::ifstream cfgFile = std::ifstream(cfgDir);
	//geode::Result res;
	geode::Result parsed = matjson::parse(cfgFile);
	
	if (!parsed) {
		auto someError = parsed.unwrapErr();
		if (parsed.unwrapErr().message == "eof") {
			std::ofstream oCfgFile = std::ofstream(cfgDir);
			oCfgFile.write(j.dump().c_str(), j.dump().size());
			oCfgFile.close();
		}
		std::cout << "Failed to parse json: " << someError.message << " " << typeid(someError).name() << "\n";
	
		ErGui::objectCfg = data;
	}
	//else {
	//	matjson::Value parsedObject = parsed.unwrap();
	//	std::map<const char*, std::vector<ErGui::ObjectConfig>> newData;
	//	for (const auto& [key, mapValue] : parsedObject) {
	//		auto myVectorOfObjCfg = mapValue.asArray().unwrap();
	//		std::vector<ErGui::ObjectConfig> objCfgVector;
	//		for (const auto& objCfgValue : myVectorOfObjCfg) {
	//			objCfgVector.push_back(objCfgValue.as<ErGui::ObjectConfig>().unwrap());
	//		}
	//		newData[key.c_str()] = objCfgVector;
	//	}
	//
	//	ErGui::objectCfg = newData;
	//
	//	std::cout << parsedObject.size() << "\n";
	//}
	
	cfgFile.close();
	
	ErGui::objectCfg = data;

	//std::cout
	//	<< "Offset ColorSelectPopup::m_touchTriggered = "
	//	<< offsetof(EffectGameObject, EffectGameObject::m_centerGroupID)
	//	<< " bytes\n";

	ImGuiCocos::get().setup([] {
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		auto fontDir = Mod::get()->getResourcesDir() / "Inter_18pt-Regular.ttf";
		std::cout << fontDir << "\n";

		io.Fonts->AddFontFromFileTTF(fontDir.string().c_str(), 15.f);

		initImGuiStyling();
		
		ErGui::setupTriggersSettings();
		}).draw([] {

			if (auto lel = GameManager::sharedState()->getEditorLayer()) {
				ErGui::renderEditGroupModule();
				ErGui::renderSelectFilter();
				ErGui::renderTransformation();
				ErGui::renderFooter();
				ErGui::renderToolsModule1();
				ErGui::renderToolsModule2();
				ErGui::renderObjectList();
				ErGui::renderEditObjectModule();
				ErGui::renderActionHistoryModule();
				ErGui::renderEditColor();
				ErGui::renderGameWindow();

				ImGui::ShowStyleEditor();

				//ErGui::renderCameraSettings();
				

				//ErGui::editorUIDrawNode->setPosition({ 0.f, 0.f });
				//if (ImGui::Begin("Geometry Dash-1")) {
				//
				//	//ErGui::editorUIDrawNode->setPosition({ 0.f, 0.f });
				//	lel->getChildByID("main-node")->getChildByID("batch-layer")->setPosition(ErGui::originalCameraPosition);
				//	//lel->m_previewAnimations =	ErGui::og_prevAnim;
				//	//lel->m_previewMode =		ErGui::og_prevMode;
				//	//lel->m_previewParticles =	ErGui::og_prevPart;
				//	//lel->m_previewShaders =		ErGui::og_prevShad;
				//	lel->update(1.f);
				//
				//	auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
				//	auto winHeight = CCDirector::sharedDirector()->getWinSize().height;
				//
				//	auto renderTexture = CCRenderTexture::create(winWidth, winHeight);
				//
				//	renderTexture->begin();
				//	glPushAttrib(GL_ALL_ATTRIB_BITS);
				//	CCDirector::sharedDirector()->getRunningScene()->visit();
				//	glPopAttrib();
				//	renderTexture->end();
				//
				//	GLuint gameTexture = renderTexture->getSprite()->getTexture()->getName();
				//
				//
				//	ImGui::Image(static_cast<ImTextureID>(gameTexture), ImVec2(winWidth * winMultiplier, winHeight * winMultiplier), ImVec2(0, 1), ImVec2(1, 0));
				//
				//
				//	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0))
				//	{
				//		ImVec2 clickPos = ImGui::GetIO().MousePos;
				//		ImVec2 windowPos = ImGui::GetItemRectMin();
				//		std::cout << "win: X=" << winWidth << " Y=" << winHeight << "\n";
				//		std::cout << "clickPos: X=" << clickPos.x << " Y=" << clickPos.y << "\n";
				//		std::cout << "windowPos: X=" << windowPos.x << " Y=" << windowPos.y << "\n";
				//
				//		float localX = (clickPos.x - windowPos.x) / winMultiplier;
				//		float localY = (clickPos.y - windowPos.y) / winMultiplier;
				//
				//		CCTouch* touch = new CCTouch;
				//		touch->setTouchInfo(0, localX, localY);
				//
				//		std::cout << "Locals: X=" << localX << " Y=" << localY << "\n";
				//
				//		CCSet* touches = CCSet::create();
				//		touches->addObject(touch);
				//		touch->release();  // CCSet берет владение объектом
				//
				//		CCEvent* event = new CCEvent();
				//		if (!touchHasBegan) {
				//			lel->m_editorUI->ccTouchBegan(touch, event);
				//			touchHasBegan = true;
				//		}
				//		else {
				//			lel->m_editorUI->ccTouchMoved(touch, event);
				//			ErGui::originalCameraPosition = lel->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();
				//		}
				//
				//
				//		CC_SAFE_RELEASE(event);
				//		//CCDirector::sharedDirector()->getTouchDispatcher()->touchesBegan(touches, nullptr);
				//		std::cout << "\n";
				//	}
				//	else if (touchHasBegan) {
				//		ImVec2 clickPos = ImGui::GetIO().MousePos;
				//		ImVec2 windowPos = ImGui::GetItemRectMin();
				//
				//		float localX = clickPos.x - windowPos.x;
				//		float localY = clickPos.y - windowPos.y;
				//
				//		CCTouch* touch = new CCTouch;
				//		touch->setTouchInfo(0, localX, localY);
				//
				//		CCEvent* event = new CCEvent();
				//
				//		lel->m_editorUI->ccTouchEnded(touch, event);
				//		touchHasBegan = false;
				//		CC_SAFE_RELEASE(event);
				//		CC_SAFE_RELEASE(event);
				//	}
				//}
				//ErGui::enableClicks();
				//ImGui::End();
				//
				//
				//
				//
				//if (ImGui::Begin("Geometry Dash-2")) {
				//	
				//	lel->getChildByID("main-node")->getChildByID("batch-layer")->setPosition({ 
				//		ErGui::originalCameraPosition.x + ErGui::cam2_offsetX,
				//		ErGui::originalCameraPosition.y + ErGui::cam2_offsetY
				//		});
				//	//lel->m_previewAnimations =	ErGui::cam2_prevAnim;
				//	//lel->m_previewMode =		false;
				//	//lel->m_previewParticles =	ErGui::cam2_prevPart;
				//	//lel->m_previewShaders =		ErGui::cam2_prevShad;
				//	lel->update(1.f);
				//	
				//	auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
				//	auto winHeight = CCDirector::sharedDirector()->getWinSize().height;
				//
				//	auto renderTexture = CCRenderTexture::create(winWidth, winHeight);
				//
				//	renderTexture->begin();
				//	glPushAttrib(GL_ALL_ATTRIB_BITS);
				//	CCDirector::sharedDirector()->getRunningScene()->visit();
				//	glPopAttrib();
				//	renderTexture->end();
				//
				//	GLuint gameTexture = renderTexture->getSprite()->getTexture()->getName();
				//
				//
				//	ImGui::Image(static_cast<ImTextureID>(gameTexture), ImVec2(winWidth * winMultiplier, winHeight * winMultiplier), ImVec2(0, 1), ImVec2(1, 0));
				//
				//
				//	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0))
				//	{
				//		ImVec2 clickPos = ImGui::GetIO().MousePos;
				//		ImVec2 windowPos = ImGui::GetItemRectMin();
				//		std::cout << "win: X=" << winWidth << " Y=" << winHeight << "\n";
				//		std::cout << "clickPos: X=" << clickPos.x << " Y=" << clickPos.y << "\n";
				//		std::cout << "windowPos: X=" << windowPos.x << " Y=" << windowPos.y << "\n";
				//
				//		float localX = (clickPos.x - windowPos.x) / winMultiplier;
				//		float localY = (clickPos.y - windowPos.y) / winMultiplier;
				//
				//		CCTouch* touch = new CCTouch;
				//		touch->setTouchInfo(0, localX, localY);
				//
				//		std::cout << "Locals: X=" << localX << " Y=" << localY << "\n";
				//
				//		CCSet* touches = CCSet::create();
				//		touches->addObject(touch);
				//		touch->release();  // CCSet берет владение объектом
				//
				//		CCEvent* event = new CCEvent();
				//		if (!touchHasBegan2) {
				//			lel->m_editorUI->ccTouchBegan(touch, event);
				//			touchHasBegan2 = true;
				//		}
				//		else {
				//			lel->m_editorUI->ccTouchMoved(touch, event);
				//			ErGui::originalCameraPosition = lel->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();
				//			ErGui::originalCameraPosition.x -= ErGui::cam2_offsetX;
				//			ErGui::originalCameraPosition.y -= ErGui::cam2_offsetY;
				//		}
				//
				//		CC_SAFE_RELEASE(event);
				//		//CCDirector::sharedDirector()->getTouchDispatcher()->touchesBegan(touches, nullptr);
				//		std::cout << "\n";
				//	}
				//	else if (touchHasBegan2) {
				//		ImVec2 clickPos = ImGui::GetIO().MousePos;
				//		ImVec2 windowPos = ImGui::GetItemRectMin();
				//
				//		float localX = clickPos.x - windowPos.x;
				//		float localY = clickPos.y - windowPos.y;
				//
				//		CCTouch* touch = new CCTouch;
				//		touch->setTouchInfo(0, localX, localY);
				//
				//		CCEvent* event = new CCEvent();
				//
				//		lel->m_editorUI->ccTouchEnded(touch, event);
				//		touchHasBegan2 = false;
				//		CC_SAFE_RELEASE(event);
				//		CC_SAFE_RELEASE(event);
				//	}
				//
				//}
				//ErGui::enableClicks();
				//ImGui::End();
				//
				//// Индекс текущей вкладки
				//
				//// Определяем ширину боковой панели
				//ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
				//ImGui::BeginGroup(); // Группируем кнопки
				//
				//// Создаём вертикальные кнопки
				//if (ImGui::Selectable("A", selectedTab == 0, 0, ImVec2(40, 40))) selectedTab = 0;
				//if (ImGui::Selectable("B", selectedTab == 1, 0, ImVec2(40, 40))) selectedTab = 1;
				//if (ImGui::Selectable("C", selectedTab == 2, 0, ImVec2(40, 40))) selectedTab = 2;
				//if (ImGui::Selectable("D", selectedTab == 3, 0, ImVec2(40, 40))) selectedTab = 3;
				//
				//ImGui::EndGroup();
				//ImGui::SameLine(); // Размещаем контент рядом
				//
				//// Основное содержимое вкладки
				//ImGui::BeginChild("TabContent", ImVec2(0, 0), true);
				//switch (selectedTab) {
				//case 0:
				//	ImGui::Text("Transform");
				//	ImGui::SliderFloat("X", nullptr, 0.0f, 100.0f);
				//	ImGui::SliderFloat("Y", nullptr, 0.0f, 100.0f);
				//	ImGui::SliderFloat("Z", nullptr, 0.0f, 100.0f);
				//	break;
				//case 1:
				//	ImGui::Text("Object Parameteres");
				//	break;
				//case 2:
				//	ImGui::Text("Animation");
				//	break;
				//case 3:
				//	ImGui::Text("Display");
				//	break;
				//}
				//ImGui::EndChild();
				//
				//ImGui::End();
			}
			});
}

// Mod Developers:
// 
// Taswert
// Rainix
//
// ----------------------------------
// Thanks to:
//
// Coders:
// Emiya		/ Code Help
// HJFod		/ Main Idea, DevTools
// RaZooM		/ Object Categories
// Mat			/ ImGui integration + Circle Tool
// MgostiH		/ First Supported! 
// 
// Creators:
// CHRAPIVA		/ Ideas + Feedback
// Kisss		/ Ideas + Feedback
// Vernam		/ Creator Contacts + MOTIVATION
// Shlyapa		/ Ideas + Feedback
// Pes11		/ Ideas
// 
// Others:
// Herowhither	/ Moral Support
// Dbrxvich		/ Moral Support
// Pololak		/ Moral Support
// 
//

//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!
//TURN OFF SELECT FILTER!!!