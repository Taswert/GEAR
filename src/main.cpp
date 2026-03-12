#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <imgui-cocos.hpp>
#include <imgui.h>
#include <imgui_internal.h>

#include <Geode/modify/CustomizeObjectLayer.hpp>

#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

#include <IconsMaterialDesignIcons.h>
#include <GearIcons.hpp>
#include "Geode/DefaultInclude.hpp"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDispatcher.h"
#include "Geode/platform/cplatform.h"
#include "Geode/utils/Keyboard.hpp"
#include "modules/EditGroupModule.hpp"
#include "modules/SelectFilterModule.hpp"
#include "modules/TransformObjectModule.hpp"
#include "modules/FooterModule.hpp"
#include "modules/ToolsModule.hpp"
#include "modules/ObjectListModule.hpp"
#include "modules/LayerModule.hpp"
#include "modules/EditObject/EditObjectModule.hpp"
#include "modules/ActionHistoryModule.hpp"
#include "modules/EditColorModule.hpp"
#include "modules/GameWindowModule.hpp"
#include "modules/GlobalDockingView.hpp"
#include "modules/ViewModule.hpp"
#include "modules/ContextMenuModule.hpp"
#include "modules/DebugModule.hpp"
#include "modules/PropertiesModule.hpp"

#include <matjson.hpp>
#include <matjson/reflect.hpp>
#include <matjson/std.hpp>
#include <matjson/stl_serialize.hpp>

#include <Geode/Result.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
//#include "AdvancedUndoRedo.hpp"

using namespace geode::prelude;


class $modify(EditorPauseLayer) {
	bool init(LevelEditorLayer* layer) {
		if (!EditorPauseLayer::init(layer)) return false;
		
		auto om = static_cast<CCMenu*>(this->getChildByID("options-menu"));
		if (!om) return true;
		
		auto sft = static_cast<CCMenuItemToggler*>(om->getChildByID("select-filter-toggle"));
		if (!sft) return true;

		bool selectFilterBool = Mod::get()->getSavedValue<bool>("select-filter", false);
		sft->toggle(selectFilterBool);
		return true;
	}

	void toggleSelectFilter(CCObject *sender) {
		bool selectFilterBool = Mod::get()->getSavedValue<bool>("select-filter", false);
		Mod::get()->setSavedValue<bool>("select-filter", !selectFilterBool);
	}
};

class $modify(CCEGLView) {
	void pollEvents() {
		auto& io = ImGui::GetIO();

		if (ErGui::isGameWindowHovered) io.WantCaptureMouse = false;

		bool blockInput = false;
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);

			// Horizontal Scroll
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

ImGuiKey gearCocosToImGuiKey(cocos2d::enumKeyCodes key) {
	if (key >= KEY_A && key <= KEY_Z) {
		return static_cast<ImGuiKey>(ImGuiKey_A + (key - KEY_A));
	}
	if (key >= KEY_Zero && key <= KEY_Nine) {
		return static_cast<ImGuiKey>(ImGuiKey_0 + (key - KEY_Zero));
	}
	switch (key) {
		case KEY_Up: return ImGuiKey_UpArrow;
		case KEY_Down: return ImGuiKey_DownArrow;
		case KEY_Left: return ImGuiKey_LeftArrow;
		case KEY_Right: return ImGuiKey_RightArrow;

		case KEY_Control: return ImGuiKey_LeftCtrl;
		case KEY_LeftWindowsKey: return ImGuiKey_LeftSuper;
		case KEY_Shift: return ImGuiKey_LeftShift;
		case KEY_Alt: return ImGuiKey_LeftAlt;
		case KEY_Enter: return ImGuiKey_Enter;

		case KEY_Home: return ImGuiKey_Home;
		case KEY_End: return ImGuiKey_End;
		case KEY_Delete: return ImGuiKey_Delete;

		default: return ImGuiKey_None;
	}
}

bool gearShouldBlockInput() {
	auto& inst = ImGuiCocos::get();
	return inst.isVisible() && inst.getInputMode() == ImGuiCocos::InputMode::Blocking;
}

class $modify(CCKeyboardDispatcher) {
	static void onModify(auto& self) {
		if (!self.setHookPriorityPre("cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG", Priority::First)) {
			log::warn("Failed to set hook priority for dispatchKeyboardMSG");
		}
	}

	bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double timestamp){
		if (!ImGuiCocos::get().isInitialized())
			return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat, timestamp);

		// log::info("isKeyDown (cocos2d): {}", isKeyDown);
		// geode::key
		const bool shouldEatInput = ImGui::GetIO().WantCaptureKeyboard || gearShouldBlockInput();
		if (shouldEatInput || !isKeyDown) {
			const auto imKey = gearCocosToImGuiKey(key);
			if (imKey != ImGuiKey_None) {
				ImGui::GetIO().AddKeyEvent(imKey, isKeyDown);
				// log::info("IsDown: W {}", ImGui::IsKeyDown(ImGuiKey_W));
				// log::info("IsPressed: W {}", ImGui::IsKeyPressed(ImGuiKey_W));
			}
		}
		if (shouldEatInput) {
			return false;
		} else {
			return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat, timestamp);
		}
	}

	// bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double timestamp){
    //     auto& io = ImGui::GetIO();
    //     log::info("Hi from dispatchKeyboardMSG");
    //     if (io.WantCaptureKeyboard) {
    //         log::info("Blocked key event. (down: {}, repeat: {})", isKeyDown, isKeyRepeat);
    //         return false;
    //     }
    //     return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat, timestamp);
    // }
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

// $execute{
// 	KeyboardInputEvent().listen([](KeyboardInputData& data) {
//             log::info("Key: {}", CCKeyboardDispatcher::get()->keyToString(data.key));
//             return ListenerResult::Propagate;
//         }).leak();
// }

$on_mod(Loaded) {
	ErGui::editorUIbottomConstrainPatch->enable();

	#ifdef GEODE_IS_WINDOWS
		ErGui::disablSelectObjectInEditorUI1->enable();
		ErGui::disablSelectObjectInEditorUI2->enable();
		ErGui::disablSelectObjectInEditorUI3->enable();
		ErGui::disablSelectObjectInEditorUI4->enable();
		ErGui::disablSelectObjectInEditorUI5->enable();
		ErGui::disablSelectObjectInEditorUI6->enable();
	#endif
	// DEBUG - allows to take a look on fields offsets
	//ErGui::objectCfg = data;
	//log::info("Offset DrawGridLayer::m_gridSize = {} bytes", offsetof(DrawGridLayer, DrawGridLayer::m_gridSize));


	static auto isEditorEnabled = &GameManager::sharedState()->m_editorEnabled;

	ImGuiCocos::get().setup([] {
		ImGuiIO& io = ImGui::GetIO();
		
		if (!std::filesystem::exists(io.IniFilename)) {
			const char* prev_ini = io.IniFilename;
			io.IniFilename = nullptr;
			auto defaultCfgDir = Mod::get()->getResourcesDir() / "default_imgui.ini";
			ImGui::LoadIniSettingsFromDisk(utils::string::pathToString(defaultCfgDir).c_str());
			io.IniFilename = prev_ini;
		}


		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		auto interDir = Mod::get()->getResourcesDir() / "Inter_18pt-Regular.ttf";
		auto mdiDir = Mod::get()->getResourcesDir() / "materialdesignicons-webfont.ttf";
		auto cpiDir = Mod::get()->getResourcesDir() / "gear-copy-paste-icons.ttf";

		io.Fonts->AddFontFromFileTTF(utils::string::pathToString(interDir).c_str(), 15.f);
		
		static const ImWchar mdi_icons_ranges[] = { ICON_MIN_MDI, ICON_MAX_MDI, 0 };
		ImFontConfig iconConfig;
		iconConfig.MergeMode = true;
		iconConfig.GlyphOffset.y = 1.f;
		iconConfig.OversampleH = 3;
		iconConfig.OversampleV = 3;
		iconConfig.PixelSnapH = true;

		io.Fonts->AddFontFromFileTTF(utils::string::pathToString(mdiDir).c_str(), 24.f, &iconConfig, mdi_icons_ranges);

		static const ImWchar cpi_icons_ranges[] = { ICON_MIN_GEARCPI, ICON_MAX_GEARCPI, 0 };
		if (!io.Fonts->AddFontFromFileTTF(utils::string::pathToString(cpiDir).c_str(), 24.f, &iconConfig, cpi_icons_ranges)) {
			log::info("Failed to load font: {}", utils::string::pathToString(cpiDir));
		}
		
		ErGui::initImGuiStyling();
	

		ErGui::setupTriggersSettings();
		}).draw([] {
			if (*isEditorEnabled && !ErGui::hideUI) {
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
				ErGui::renderViewModule();
				ErGui::renderLayerModule();
				ErGui::renderGameWindow();
				ErGui::renderContextMenu();
				ErGui::renderPropertiesModule();

				ErGui::renderDebugModule();

				// ImGui::ShowStyleEditor();

			}
			else {
				ErGui::isGameWindowHovered = false;
			}
			});
}