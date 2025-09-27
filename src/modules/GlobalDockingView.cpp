#include "./GlobalDockingView.hpp"

static void renderMenuBar() {
	if (ImGui::BeginMenuBar()) {
        bool undoEnabled = false;
        bool redoEnabled = false;
        
        if (LevelEditorLayer::get()->m_undoObjects && LevelEditorLayer::get()->m_undoObjects->count() > 0)
            undoEnabled = true;
        if (LevelEditorLayer::get()->m_redoObjects && LevelEditorLayer::get()->m_redoObjects->count() > 0)
            redoEnabled = true;
        
        if (ImGui::MenuItem("Undo", nullptr, false, undoEnabled)) {
			EditorUI::get()->undoLastAction(nullptr);
		}

		if (ImGui::MenuItem("Redo", nullptr, false, redoEnabled)) {
			EditorUI::get()->redoLastAction(nullptr);
		}

        if (ImGui::MenuItem("Hide UI")) {
            LevelEditorLayer::get()->getChildByID("hideUIMenu"_spr)->getChildByID("hideUIBtn"_spr)->setVisible(true);
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
            if (EditorUI::get()->m_selectedObjects && EditorUI::get()->m_selectedObjects->count() > 0)
                objectsSelected = true;

            if (ImGui::MenuItem("Align-X", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onAlignX(nullptr);
            }

            if (ImGui::MenuItem("Align-Y", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onAlignY(nullptr);
            }

            if (ImGui::MenuItem("Create Loop", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onCreateLoop(nullptr);
            }

            if (ImGui::MenuItem("Create Extras", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onCreateExtras(nullptr);
            }

            ImGui::Dummy({ 5.f, 5.f }); // Group Change

            if (ImGui::MenuItem("Build Helper", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onBuildHelper(nullptr);
            }

            if (ImGui::MenuItem("Re-Group", nullptr, false, objectsSelected)) {
                ErGui::getFakePauseLayer()->onReGroup(nullptr);
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

            if (ImGui::MenuItem("Paste + Color", nullptr, false, objectsSelected)) {
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

        if (ImGui::BeginMenu("Properties")) {

            auto gm = GameManager::sharedState();
            
            // Initialize
            bool ptMusic =                  gm->getGameVariable("0002");    // Plays music during playtesting
            bool ptNoGrid =                 gm->getGameVariable("0079");    // Hides grid during playtesting
            bool ptSmoothFix =              gm->getGameVariable("0102");    // Enables smooth fix during playtesting
            bool autoPause =                gm->getGameVariable("0150");    // Pauses gameplay when starting playtest from StartPos
            //bool swipeHold =              gm->getGameVariable("0057");    //      // Visuals should be reworked manually because of reworked selection system...
            //bool swipeCycle =             gm->getGameVariable("0059");    //      // This should be reworked manually because of reworked selection system...
            bool startOptimisation =        gm->getGameVariable("0151");    // 
            bool increaseUndoRedo =         gm->getGameVariable("0013");    //
            bool smallWarpBtns =            gm->getGameVariable("0169");    //
            bool ignoreDamage =             gm->getGameVariable("0009");    //
            bool triangleColorWheel =       geode::Mod::get()->getSavedValue<bool>("triangle-color-wheel");     //
            bool rotateColorWheel =         geode::Mod::get()->getSavedValue<bool>("rotate-color-wheel");       //
            bool hideObjectListPopup =      geode::Mod::get()->getSavedValue<bool>("hide-object-list-popup");   //
            bool autoswitchToBuildMode =    geode::Mod::get()->getSavedValue<bool>("autoswitch-to-build-mode"); //
            bool showLinkControls =         gm->getGameVariable("0097");                                        // 
            bool showZoomControls =         geode::Mod::get()->getSavedValue<bool>("show-zoom-controls");       // Shows zoom controls buttons on toolbox
            bool showObjectInfo =           gm->getGameVariable("0041");
			bool fillSelectionZone =        geode::Mod::get()->getSavedValue<bool>("fill-selection-zone");      // Fills selection zone with solid color
			bool hoveringSelects =          geode::Mod::get()->getSavedValue<bool>("hovering-selects");         // Hovers objects in the selection zone
            bool deselectControls =         geode::Mod::get()->getSavedValue<bool>("deselect-controls");        // Deselects GJRotation/Scale/TransformControl, when clicking on empty space in editor
            bool autoBuildhelper =          geode::Mod::get()->getSavedValue<bool>("auto-buildhelper");         // Automatically applies Build helper on duplicated objects
            //bool selectDirectionFromCursor = geode::Mod::get()->getSavedValue<bool>("select-direction-from-cursor");


            bool isAnyItemClicked = false;
            
            if (ImGui::BeginMenu("Selected Object Info")) {
                bool soiPosition =      geode::Mod::get()->getSavedValue<bool>("soi-position");
                bool soiRotation =      geode::Mod::get()->getSavedValue<bool>("soi-rotation");
                bool soiScale =         geode::Mod::get()->getSavedValue<bool>("soi-scale");
                bool soiColor =         geode::Mod::get()->getSavedValue<bool>("soi-color");
                bool soiHSV =           geode::Mod::get()->getSavedValue<bool>("soi-hsv");
                bool soiGroups =        geode::Mod::get()->getSavedValue<bool>("soi-groups");
                bool soiZLayer =        geode::Mod::get()->getSavedValue<bool>("soi-zlayer");
                bool soiZOrder =        geode::Mod::get()->getSavedValue<bool>("soi-zorder");
                bool soiObjectID =      geode::Mod::get()->getSavedValue<bool>("soi-objectid");
                bool soiTargetGroup =   geode::Mod::get()->getSavedValue<bool>("soi-targetgroup");
                bool soiItemID =        geode::Mod::get()->getSavedValue<bool>("soi-itemid");
                bool soiBlockID =       geode::Mod::get()->getSavedValue<bool>("soi-blockid");
                bool soiParticles =     geode::Mod::get()->getSavedValue<bool>("soi-particles");
                bool soiHidden =        geode::Mod::get()->getSavedValue<bool>("soi-hidden");
                bool soiNoTouch =       geode::Mod::get()->getSavedValue<bool>("soi-no-touch");
                bool soiHighDetail =    geode::Mod::get()->getSavedValue<bool>("soi-high-detail");
                bool soiObjectCount =   geode::Mod::get()->getSavedValue<bool>("soi-object-count");

                ImGui::MenuItem("Show Position ", NULL, &soiPosition);
                ImGui::MenuItem("Show Rotation ", NULL, &soiRotation);
                ImGui::MenuItem("Show Scale ", NULL, &soiScale);
                ImGui::MenuItem("Show Color ", NULL, &soiColor);
                ImGui::MenuItem("Show HSV ", NULL, &soiHSV);
                ImGui::MenuItem("Show Groups ", NULL, &soiGroups);
                ImGui::MenuItem("Show ZLayer ", NULL, &soiZLayer);
                ImGui::MenuItem("Show ZOrder ", NULL, &soiZOrder);
                ImGui::MenuItem("Show ObjectID ", NULL, &soiObjectID);
                ImGui::MenuItem("Show TargetGroup ", NULL, &soiTargetGroup);
                ImGui::MenuItem("Show ItemID ", NULL, &soiItemID);
                ImGui::MenuItem("Show BlockID ", NULL, &soiBlockID);
                ImGui::MenuItem("Show Particles ", NULL, &soiParticles);
                ImGui::MenuItem("Show Hidden ", NULL, &soiHidden);
                ImGui::MenuItem("Show NoTouch ", NULL, &soiNoTouch);
                ImGui::MenuItem("Show HighDetail ", NULL, &soiHighDetail);
                ImGui::MenuItem("Show ObjectCount ", NULL, &soiObjectCount);

                geode::Mod::get()->setSavedValue<bool>("soi-position", soiPosition);
                geode::Mod::get()->setSavedValue<bool>("soi-rotation", soiRotation);
                geode::Mod::get()->setSavedValue<bool>("soi-scale", soiScale);
                geode::Mod::get()->setSavedValue<bool>("soi-color", soiColor);
                geode::Mod::get()->setSavedValue<bool>("soi-hsv", soiHSV);
                geode::Mod::get()->setSavedValue<bool>("soi-groups", soiGroups);
                geode::Mod::get()->setSavedValue<bool>("soi-zlayer", soiZLayer);
                geode::Mod::get()->setSavedValue<bool>("soi-zorder", soiZOrder);
                geode::Mod::get()->setSavedValue<bool>("soi-objectid", soiObjectID);
                geode::Mod::get()->setSavedValue<bool>("soi-targetgroup", soiTargetGroup);
                geode::Mod::get()->setSavedValue<bool>("soi-itemid", soiItemID);
                geode::Mod::get()->setSavedValue<bool>("soi-blockid", soiBlockID);
                geode::Mod::get()->setSavedValue<bool>("soi-particles", soiParticles);
                geode::Mod::get()->setSavedValue<bool>("soi-hidden", soiHidden);
                geode::Mod::get()->setSavedValue<bool>("soi-no-touch", soiNoTouch);
                geode::Mod::get()->setSavedValue<bool>("soi-high-detail", soiHighDetail);
                geode::Mod::get()->setSavedValue<bool>("soi-object-count", soiObjectCount);

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Show Object Info", NULL, &showObjectInfo)) {
				isAnyItemClicked = true;
                
                // uh 
                if (LevelEditorLayer::get()) {
                    if (auto label = dynamic_cast<CCLabelBMFont*>(LevelEditorLayer::get()->getChildByID("object-info-label"_spr)))
                        if (showObjectInfo)
                            label->setVisible(true);
                        else
                            label->setVisible(false);
                }
            }

            ImGui::Dummy({ 5.f, 5.f });

            if (ImGui::MenuItem("Playtest Music", NULL, &ptMusic)) {
                isAnyItemClicked = true;
            }
            //ImGui::SetTooltip("Plays music during playtesting.");
            if (ImGui::MenuItem("Playtest No Grid", NULL, &ptNoGrid)) {
                isAnyItemClicked = true;
            }
            //ImGui::SetTooltip("Hides grid during playtesting.");
            if (ImGui::MenuItem("Playtest Smooth Fix", NULL, &ptSmoothFix)) {
                isAnyItemClicked = true;
            }
            //ImGui::SetTooltip("Enables smooth fix during playtesting.");
            if (ImGui::MenuItem("Auto-Pause", NULL, &autoPause)) {
                isAnyItemClicked = true;
            }
            //ImGui::SetTooltip("Pauses gameplay when starting from StartPos.");
            if (ImGui::MenuItem("Ignore Damage", NULL, &ignoreDamage)) {
                isAnyItemClicked = true;
            }
            //ImGui::SetTooltip("Disables damage during playtesting.");

            //ImGui::Dummy({ 5.f, 5.f });

            //ImGui::MenuItem("Hold To Swipe", NULL,        &swipeHold);       
            //ImGui::MenuItem("Swipe Cycle Mode", NULL,     &swipeCycle);

            ImGui::Dummy({ 5.f, 5.f });
            
            if (ImGui::MenuItem("Start Optimisation", NULL, &startOptimisation)) {
                isAnyItemClicked = true;
            }
            //ImGui::SetTooltip("Tries to speed up StartPos loading. RobTop's Experimental option.");
            if (ImGui::MenuItem("Increase Undo/Redo", NULL, &increaseUndoRedo)) {
                isAnyItemClicked = true;
            }
            //ImGui::SetTooltip("Increases maximum Undo/Redo limit from 200 to 1000.");
            if (ImGui::MenuItem("Small Warp Buttons", NULL, &smallWarpBtns)) {
                isAnyItemClicked = true;
            }
            //ImGui::SetTooltip("Warp control buttons are 50% smaller (Restart editor to work).");
            if (ImGui::MenuItem("Auto Build Helper", NULL, &autoBuildhelper)) {
                isAnyItemClicked = true;
            }


            ImGui::Dummy({ 5.f, 5.f });

            if (ImGui::MenuItem("Triangle Color Wheel", NULL, &triangleColorWheel)) {
                isAnyItemClicked = true;
            }
            //ImGui::SetTooltip("Sets GEAR color wheel to triangle.");
            if (ImGui::MenuItem("Rotate Color Wheel", NULL, &rotateColorWheel)) {
                isAnyItemClicked = true;
            }
            //ImGui::SetTooltip("Enables color wheel rotation in GEAR. Only works with triangle color wheel.");
            //ImGui::MenuItem("Select Direction From Cursor", NULL, &selectDirectionFromCursor);
            if (ImGui::MenuItem("Hide Objects Popup", NULL, &hideObjectListPopup)) {
                isAnyItemClicked = true;
            }
            if (ImGui::MenuItem("Autoswitch to Build Mode", NULL, &autoswitchToBuildMode)) {
                isAnyItemClicked = true;
            }

            ImGui::Dummy({ 5.f, 5.f });

            if (ImGui::MenuItem("Show Link Controls", NULL, &showLinkControls)) {
                isAnyItemClicked = true;
            }
            if (ImGui::MenuItem("Show Zoom Controls", NULL, &showZoomControls)) {
                isAnyItemClicked = true;
            }
            
            ImGui::Dummy({ 5.f, 5.f });

            if (ImGui::MenuItem("Fill Selection Zone", NULL, &fillSelectionZone)) {
                isAnyItemClicked = true;
            }
            if (ImGui::MenuItem("Hovering Selects", NULL, &hoveringSelects)) {
                isAnyItemClicked = true;
            }

            ImGui::Dummy({ 5.f, 5.f });

            if (ImGui::MenuItem("Deselect Controls", NULL, &deselectControls)) {
                isAnyItemClicked = true;
            }



            // Saving Values
            if (isAnyItemClicked) {
                gm->setGameVariable("0002", ptMusic);
                gm->setGameVariable("0079", ptNoGrid);
                gm->setGameVariable("0102", ptSmoothFix);
                gm->setGameVariable("0150", autoPause);
                //gm->setGameVariable("0057", swipeHold);
                //gm->setGameVariable("0059", swipeCycle);
                gm->setGameVariable("0151", startOptimisation);
                gm->setGameVariable("0013", increaseUndoRedo);
                gm->setGameVariable("0169", smallWarpBtns);
                gm->setGameVariable("0009", ignoreDamage);
                geode::Mod::get()->setSavedValue<bool>("triangle-color-wheel", triangleColorWheel);
                geode::Mod::get()->setSavedValue<bool>("rotate-color-wheel", rotateColorWheel);
                geode::Mod::get()->setSavedValue<bool>("hide-object-list-popup", hideObjectListPopup);
                geode::Mod::get()->setSavedValue<bool>("autoswitch-to-build-mode", autoswitchToBuildMode);
                gm->setGameVariable("0097", showLinkControls);
                geode::Mod::get()->setSavedValue<bool>("show-zoom-controls", showZoomControls);
                gm->setGameVariable("0041", showObjectInfo);
                geode::Mod::get()->setSavedValue<bool>("fill-selection-zone", fillSelectionZone);
                geode::Mod::get()->setSavedValue<bool>("hovering-selects", hoveringSelects);
                geode::Mod::get()->setSavedValue<bool>("deselect-controls", deselectControls);
                geode::Mod::get()->setSavedValue<bool>("auto-buildhelper", autoBuildhelper);

                LevelEditorLayer::get()->updateOptions();
            }
            //geode::Mod::get()->setSavedValue<bool>("select-direction-from-cursor", selectDirectionFromCursor);

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Level Settings")) {
            bool foundSettings = false;
            for (auto child : CCArrayExt<CCNode*>(CCDirector::sharedDirector()->getRunningScene()->getChildren())) {
                if (dynamic_cast<LevelSettingsLayer*>(child)) foundSettings = true;
            }
            if (!foundSettings)
                LevelEditorLayer::get()->m_editorUI->onSettings(nullptr);
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