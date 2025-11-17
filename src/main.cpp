#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <imgui_internal.h>

#include <Geode/modify/CustomizeObjectLayer.hpp>

#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/CCEGLView.hpp>

#include <IconsMaterialDesignIcons.h>
#include <GearCopyPasteIcons.hpp>
#include "modules/EditGroupModule.hpp"
#include "modules/SelectFilterModule.hpp"
#include "modules/TransformObjectModule.hpp"
#include "modules/FooterModule.hpp"
#include "modules/ToolsModule.hpp"
#include "modules/ObjectListModule.hpp"
#include "modules/LayerModule.hpp"
#include "modules/EditObjectModule.hpp"
#include "modules/ActionHistoryModule.hpp"
#include "modules/EditColorModule.hpp"
#include "modules/GameWindowModule.hpp"
#include "modules/GlobalDockingView.hpp"
#include "modules/SettingsModule.hpp"
#include "modules/ContextMenuModule.hpp"
#include "modules/EditorUtilsModule.hpp"
#include "modules/DebugModule.hpp"

#include "includes/ObjectCategories.hpp"
#include <matjson.hpp>
#include <matjson/reflect.hpp>
#include <matjson/std.hpp>
#include <matjson/stl_serialize.hpp>

#include <Geode/Result.hpp>
#include "CopyEGMState.hpp"
#include "features/Hovering.hpp"
//#include "AdvancedUndoRedo.hpp"

using namespace geode::prelude;


class $modify(CCEGLView) {
	void pollEvents() {
		auto& io = ImGui::GetIO();

		if (ErGui::isGameWindowHovered) io.WantCaptureMouse = false;

		bool blockInput = false;
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);

			if (msg.message == WM_MOUSEHWHEEL) {
				CCDirector::sharedDirector()->getMouseDispatcher()->dispatchScrollMSG(0, GET_WHEEL_DELTA_WPARAM(msg.wParam) * 0.4f * -1);
				continue;
			}

			auto editorUI = EditorUI::get();
			if (editorUI && !editorUI->m_isPaused && !io.WantCaptureMouse) {
				switch (msg.message) {
					case WM_RBUTTONDOWN: {

						if (ErGui::rightTouch) {
							auto setCancel = CCSet::create();
							setCancel->addObject(ErGui::rightTouch);
							CCDirector::sharedDirector()->getTouchDispatcher()->touchesCancelled(setCancel, nullptr);

							ErGui::rightTouch->release();
							ErGui::rightTouch = nullptr;
						}

						ErGui::rightTouch = new CCTouch;
						ErGui::rightTouch->retain();
						ErGui::rightTouch->setTouchInfo(1, LOWORD(msg.lParam), HIWORD(msg.lParam));

						auto setBegan = CCSet::create();
						setBegan->addObject(ErGui::rightTouch);

						CCDirector::sharedDirector()->getTouchDispatcher()->touchesBegan(setBegan, nullptr);
						break;
					}
					case WM_MOUSEMOVE: {
						if (!ErGui::rightTouch) break;

						ErGui::rightTouch->setTouchInfo(1, LOWORD(msg.lParam), HIWORD(msg.lParam));

						auto setMoved = CCSet::create();
						setMoved->addObject(ErGui::rightTouch);
						CCDirector::sharedDirector()->getTouchDispatcher()->touchesMoved(setMoved, nullptr);

						break;
					}
					case WM_RBUTTONUP: {
						if (!ErGui::rightTouch) break;

						ErGui::rightTouch->setTouchInfo(1, LOWORD(msg.lParam), HIWORD(msg.lParam));

						auto setEnded = CCSet::create();
						setEnded->addObject(ErGui::rightTouch);
						CCDirector::sharedDirector()->getTouchDispatcher()->touchesEnded(setEnded, nullptr);

						ErGui::rightTouch->release();
						ErGui::rightTouch = nullptr;
						break;
					}
				}
			}

			if (io.WantCaptureMouse) {
				switch (msg.message) {
				case WM_RBUTTONDOWN:
					io.AddMouseButtonEvent(1, true);
					break;
				case WM_RBUTTONUP:
					io.AddMouseButtonEvent(1, false);
					break;
				case WM_MBUTTONDOWN:
					io.AddMouseButtonEvent(2, true);
					break;
				case WM_MBUTTONUP:
					io.AddMouseButtonEvent(2, false);
				}
			}

			if (!blockInput)
				DispatchMessage(&msg);

		}

		if (ErGui::rightTouch && !GetAsyncKeyState(VK_RBUTTON)) {
			auto setCancelled = CCSet::create();
			setCancelled->addObject(ErGui::rightTouch);
			CCDirector::sharedDirector()->getTouchDispatcher()->touchesCancelled(setCancelled, nullptr);

			ErGui::rightTouch->release();
			ErGui::rightTouch = nullptr;
		}

		CCEGLView::pollEvents();
	}
};


class $modify(CCTouchDispatcher) {
	void touches(CCSet * touches, CCEvent * event, unsigned int type) {
		auto& io = ImGui::GetIO();
		auto* touch = static_cast<CCTouch*>(touches->anyObject());
		if (!touch) return;

		

		if (type == CCTOUCHBEGAN && ErGui::isGameWindowHovered) {
			ErGui::isGameWindowTouching = true;
		}

		if (ErGui::isGameWindowTouching) {
			io.WantCaptureMouse = false;
			touch->setTouchInfo(touch->getID(), ErGui::gameWindowTouchCoordinatesConvertedToWorld.x, ErGui::gameWindowTouchCoordinatesConvertedToWorld.y);
			ImGui::ClearActiveID();
			io.MouseDown[0] = false;
			io.MouseDown[1] = false;
			io.MouseDown[2] = false;
		}

		if (type == CCTOUCHENDED || type == CCTOUCHCANCELLED) {
			ErGui::isGameWindowTouching = false;
		}

		
		CCTouchDispatcher::touches(touches, event, type);
	}
};

$on_mod(Loaded) {
	ErGui::editorUIbottomConstrainPatch->enable();
	ErGui::disablSelectObjectInEditorUI1->enable();
	ErGui::disablSelectObjectInEditorUI2->enable();
	ErGui::disablSelectObjectInEditorUI3->enable();
	ErGui::disablSelectObjectInEditorUI4->enable();
	ErGui::disablSelectObjectInEditorUI5->enable();
	ErGui::disablSelectObjectInEditorUI6->enable();
	//ErGui::vanillaGridOpaquePatch->enable();
	
	// DEBUG - allows to take a look on fields offsets
	//ErGui::objectCfg = data;
	//log::info("Offset DrawGridLayer::m_gridSize = {} bytes", offsetof(DrawGridLayer, DrawGridLayer::m_gridSize));

	ImGuiCocos::get().setup([] {
		ImGuiIO& io = ImGui::GetIO();
		
		if (!std::filesystem::exists(io.IniFilename)) {
			const char* prev_ini = io.IniFilename;
			io.IniFilename = nullptr;
			auto defaultCfgDir = Mod::get()->getResourcesDir() / "default_imgui.ini";
			ImGui::LoadIniSettingsFromDisk(defaultCfgDir.string().c_str());
			io.IniFilename = prev_ini;
		}


		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		auto interDir = Mod::get()->getResourcesDir() / "Inter_18pt-Regular.ttf";
		auto mdiDir = Mod::get()->getResourcesDir() / "materialdesignicons-webfont.ttf";
		auto cpiDir = Mod::get()->getResourcesDir() / "gear-copy-paste-icons.ttf";

		io.Fonts->AddFontFromFileTTF(interDir.string().c_str(), 15.f);
		
		static const ImWchar mdi_icons_ranges[] = { ICON_MIN_MDI, ICON_MAX_MDI, 0 };
		ImFontConfig iconConfig;
		iconConfig.MergeMode = true;
		iconConfig.GlyphOffset.y = 1.f;
		iconConfig.OversampleH = 3;
		iconConfig.OversampleV = 3;
		iconConfig.PixelSnapH = true;

		io.Fonts->AddFontFromFileTTF(mdiDir.string().c_str(), 24.f, &iconConfig, mdi_icons_ranges);

		static const ImWchar cpi_icons_ranges[] = { ICON_MIN_GEARCPI, ICON_MAX_GEARCPI, 0 };
		if (!io.Fonts->AddFontFromFileTTF(cpiDir.string().c_str(), 24.f, &iconConfig, cpi_icons_ranges)) {
			log::info("Failed to load font: {}", cpiDir.string());
		}
		
		ErGui::initImGuiStyling();
	

		ErGui::setupTriggersSettings();
		}).draw([] {
			if (auto lel = GameManager::sharedState()->getEditorLayer() && !ErGui::hideUI) {
				ErGui::renderGlobalDockingView();
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
				ErGui::renderSettingsModule();
				ErGui::renderLayerModule();
				ErGui::renderGameWindow();
				ErGui::renderContextMenu();

				ImGui::Begin("Debug");
				ImGui::Text("g_selectionVersion: % d", ErGui::g_selectVersion);
				ImGui::End();

				//ImGui::ShowStyleEditor();

			}
			else {
				ErGui::isGameWindowHovered = false;
			}
			});
}