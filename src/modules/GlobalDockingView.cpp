#include "./GlobalDockingView.hpp"
#include "PropertiesModule.hpp"

static void renderMenuBar() {
	if (ImGui::BeginMenuBar()) {
        bool undoEnabled = false;
        bool redoEnabled = false;
        auto editorUI = EditorUI::get();
        auto lel = LevelEditorLayer::get();
        auto mod = geode::Mod::get();
        auto undoObjects = lel->m_undoObjects;
        auto redoObjects = lel->m_redoObjects;

        if (undoObjects && undoObjects->count() > 0)
            undoEnabled = true;
        if (redoObjects && redoObjects->count() > 0)
            redoEnabled = true;
        
        if (ImGui::MenuItem("Undo", nullptr, false, undoEnabled)) {
			editorUI->undoLastAction(nullptr);
		}

		if (ImGui::MenuItem("Redo", nullptr, false, redoEnabled)) {
            editorUI->redoLastAction(nullptr);
		}

        if (ImGui::MenuItem("Hide UI")) {
            lel->getChildByID("hideUIMenu"_spr)->getChildByID("hideUIBtn"_spr)->setVisible(true);
            ErGui::hideUI = !ErGui::hideUI;
        }

		if (ImGui::BeginMenu("Utilites")) {
            // Selections

            if (ImGui::MenuItem("Select All")) {
                ErGui::getFakePauseLayer()->onSelectAll(nullptr);
            }

            if (ImGui::MenuItem("Select All Right")) {
                ErGui::getFakePauseLayer()->onSelectAllRight(nullptr);
            }

            if (ImGui::MenuItem("Select All Left")) {
                ErGui::getFakePauseLayer()->onSelectAllLeft(nullptr);
            }
            
            ImGui::Dummy({ 5.f, 5.f }); // Create / Edit
            
            bool objectsSelected = false;
            auto selectedObjects = editorUI->m_selectedObjects;
            if (selectedObjects || selectedObjects->count() > 0)
                objectsSelected = true;
            

            if (ImGui::MenuItem("Align X", nullptr, false, objectsSelected)) {
                editorUI->alignObjects(editorUI->getSelectedObjects(), false);
            }

            if (ImGui::MenuItem("Align Y", nullptr, false, objectsSelected)) {
                editorUI->alignObjects(editorUI->getSelectedObjects(), true);
            }

            if (ImGui::MenuItem("Create Loop", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onCreateLoop(nullptr);
            }

            if (ImGui::MenuItem("Create Extras", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onCreateExtras(nullptr);
            }

            ImGui::Dummy({ 5.f, 5.f }); // Group Change

            if (ImGui::MenuItem("Build Helper", nullptr, false, objectsSelected)) {
                editorUI->dynamicGroupUpdate(false);
            }

            if (ImGui::MenuItem("Re-Group", nullptr, false, objectsSelected)) {
                editorUI->dynamicGroupUpdate(true);
            }

            if (ImGui::MenuItem("New Group X", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onNewGroupX(nullptr);
            }

            if (ImGui::MenuItem("New Group Y", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onNewGroupY(nullptr);
            }

            ImGui::Dummy({ 5.f, 5.f }); // Other

            if (ImGui::MenuItem("Copy + Color", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onCopyWColor(nullptr);
            }

            if (ImGui::MenuItem("Paste + Color")) {
                ErGui::getFakePauseLayer()->onPasteWColor(nullptr);
            }

            if (ImGui::MenuItem("Unlock Layers")) {
                ErGui::getFakePauseLayer()->onUnlockAllLayers(nullptr);
            }

            if (ImGui::MenuItem("Reset Unused")) {
                // getFakePauseLayer()->onResetUnusedColors(nullptr);
                createQuickPopup("Reset Unused Colors", "Reset all unused color channels?\n"
                    "<cy>This will make those channels available as 'next free'</c>", "NO", "YES", 300.f,
                    [](auto, bool isBtn2) {
                        if (isBtn2) {
                            LevelEditorLayer::get()->resetUnusedColorChannels();
                        }
                    },
                    true, true
                );
            }

            if (ImGui::MenuItem("Uncheck Portals")) {
                ErGui::getFakePauseLayer()->uncheckAllPortals(nullptr);
            }

            ImGui::EndMenu();
		}

        if (ImGui::MenuItem("Properties")) {
            ErGui::showProperties = true;
        }

        if (ImGui::MenuItem("Level Settings")) {
            bool foundSettings = false;
            for (const auto& child : CCArrayExt<CCNode*>(CCDirector::sharedDirector()->getRunningScene()->getChildren())) {
                if (typeinfo_cast<LevelSettingsLayer*>(child)) foundSettings = true;
            }
            if (!foundSettings) {
                LevelSettingsLayer::create(lel->m_levelSettings, lel)->show();
            }
                
        }

		ImGui::EndMenuBar();
	}
}

namespace ErGui {
	void renderGlobalDockingView() {
		auto io = ImGui::GetIO();
		auto viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		
		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_MenuBar;

		ImGui::Begin("GD", nullptr, window_flags);
		ImGui::PopStyleVar(2);
		ImGuiID dockspace_id = ImGui::GetID("GlobalDocking");
		ImGui::DockSpace(dockspace_id, ImVec2(0.f, 0.f), ImGuiDockNodeFlags_None);

		renderMenuBar();
		
		ImGui::End();
	}
}