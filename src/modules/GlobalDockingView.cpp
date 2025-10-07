#include "./GlobalDockingView.hpp"

static void renderMenuBar() {
	if (ImGui::BeginMenuBar()) {
        bool undoEnabled = false;
        bool redoEnabled = false;
        auto editorUI = EditorUI::get();
        auto lel = LevelEditorLayer::get();
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
            bool triangleColorWheel =       geode::Mod::get()->getSavedValue<bool>("triangle-color-wheel", true);       //
            bool rotateColorWheel =         geode::Mod::get()->getSavedValue<bool>("rotate-color-wheel", false);        //
            bool hideObjectListPopup =      geode::Mod::get()->getSavedValue<bool>("hide-object-list-popup", true);     //
            bool autoswitchToBuildMode =    geode::Mod::get()->getSavedValue<bool>("autoswitch-to-build-mode", true);   //
            bool showLinkControls =         gm->getGameVariable("0097");                                                // 
            bool showZoomControls =         geode::Mod::get()->getSavedValue<bool>("show-zoom-controls", true);         // Shows zoom controls buttons on toolbox
            bool showObjectInfo =           gm->getGameVariable("0041");
			bool fillSelectionZone =        geode::Mod::get()->getSavedValue<bool>("fill-selection-zone", false);       // Fills selection zone with solid color
			bool hoveringSelects =          geode::Mod::get()->getSavedValue<bool>("hovering-selects", true);           // Hovers objects in the selection zone
            bool deselectControls =         geode::Mod::get()->getSavedValue<bool>("deselect-controls", false);         // Deselects GJRotation/Scale/TransformControl, when clicking on empty space in editor
            bool autoBuildhelper =          geode::Mod::get()->getSavedValue<bool>("auto-buildhelper", false);          // Automatically applies Build helper on duplicated objects
			bool gamewindowStaticRatio =    geode::Mod::get()->getSavedValue<bool>("gamewindow-static-ratio", false);   // Game window keeps executable window ratio
            //bool selectDirectionFromCursor = geode::Mod::get()->getSavedValue<bool>("select-direction-from-cursor");


            bool isAnyItemClicked = false;
            
            if (ImGui::BeginMenu("Selected Object Info")) {
                bool soiPosition =      geode::Mod::get()->getSavedValue<bool>("soi-position", true);
                bool soiRotation =      geode::Mod::get()->getSavedValue<bool>("soi-rotation", true);
                bool soiScale =         geode::Mod::get()->getSavedValue<bool>("soi-scale", true);
                bool soiColor =         geode::Mod::get()->getSavedValue<bool>("soi-color", true);
                bool soiHSV =           geode::Mod::get()->getSavedValue<bool>("soi-hsv", false);
                bool soiGroups =        geode::Mod::get()->getSavedValue<bool>("soi-groups", true);
                bool soiZLayer =        geode::Mod::get()->getSavedValue<bool>("soi-zlayer", true);
                bool soiZOrder =        geode::Mod::get()->getSavedValue<bool>("soi-zorder", true);
                bool soiObjectID =      geode::Mod::get()->getSavedValue<bool>("soi-objectid", false);
                bool soiTargetGroup =   geode::Mod::get()->getSavedValue<bool>("soi-targetgroup", true);
                bool soiItemID =        geode::Mod::get()->getSavedValue<bool>("soi-itemid", false);
                bool soiBlockID =       geode::Mod::get()->getSavedValue<bool>("soi-blockid", false);
                bool soiParticles =     geode::Mod::get()->getSavedValue<bool>("soi-particles", true);
                bool soiHidden =        geode::Mod::get()->getSavedValue<bool>("soi-hidden", true);
                bool soiNoTouch =       geode::Mod::get()->getSavedValue<bool>("soi-no-touch", true);
                bool soiHighDetail =    geode::Mod::get()->getSavedValue<bool>("soi-high-detail", false);
                bool soiObjectCount =   geode::Mod::get()->getSavedValue<bool>("soi-object-count", true);

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
                    if (auto label = typeinfo_cast<CCLabelBMFont*>(LevelEditorLayer::get()->getChildByID("object-info-label"_spr)))
                        if (showObjectInfo)
                            label->setVisible(true);
                        else
                            label->setVisible(false);
                }
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Shows some information about selected object.");
            }

            ImGui::Dummy({ 5.f, 5.f });

            if (ImGui::MenuItem("Playtest Music", NULL, &ptMusic)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Plays music during playtesting.");
            }
            
            if (ImGui::MenuItem("Playtest No Grid", NULL, &ptNoGrid)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Hides grid during playtesting.");
            }
            
            if (ImGui::MenuItem("Playtest Smooth Fix", NULL, &ptSmoothFix)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Enables smooth fix during playtesting");
            }

            if (ImGui::MenuItem("Auto-Pause", NULL, &autoPause)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Pauses gameplay when starting from StartPos.");
            }

            if (ImGui::MenuItem("Ignore Damage", NULL, &ignoreDamage)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Disables damage during playtesting.");
            }

            //ImGui::Dummy({ 5.f, 5.f });

            //ImGui::MenuItem("Hold To Swipe", NULL,        &swipeHold);       
            //ImGui::MenuItem("Swipe Cycle Mode", NULL,     &swipeCycle);

            ImGui::Dummy({ 5.f, 5.f });
            
            if (ImGui::MenuItem("Start Optimisation", NULL, &startOptimisation)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Tries to speed up StartPos loading. RobTop's experimental option.");
            }

            if (ImGui::MenuItem("Increase Undo/Redo", NULL, &increaseUndoRedo)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Increases maximum Undo/Redo limit from 200 to 1000");
            }

            if (ImGui::MenuItem("Small Warp Buttons", NULL, &smallWarpBtns)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Warp control buttons are 50% smaller (Restart editor to work).");
            }

            if (ImGui::MenuItem("Auto Build Helper", NULL, &autoBuildhelper)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Automatically applies build helper on pasted objects.");
            }


            ImGui::Dummy({ 5.f, 5.f });

            if (ImGui::MenuItem("Triangle Color Wheel", NULL, &triangleColorWheel)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Sets GEAR color wheel to triangle.");
            }

            if (ImGui::MenuItem("Rotate Color Wheel", NULL, &rotateColorWheel)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Enables color wheel rotation in GEAR. Only works with triangle color wheel.");
            }

            if (ImGui::MenuItem("Hide Objects Popup", NULL, &hideObjectListPopup)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Hides object folder popup, when selecting an object.");
            }

            if (ImGui::MenuItem("Autoswitch to Build Mode", NULL, &autoswitchToBuildMode)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
				ImGui::SetTooltip("Automatically switches to Build Mode when selecting an object in object list.");
            }

            ImGui::Dummy({ 5.f, 5.f });

            if (ImGui::MenuItem("Show Link Controls", NULL, &showLinkControls)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Shows and enables link controls in the toolbar.");
            }
            if (ImGui::MenuItem("Show Zoom Controls", NULL, &showZoomControls)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Shows zoom controls in the toolbar.");
            }
            
            ImGui::Dummy({ 5.f, 5.f });

            if (ImGui::MenuItem("Fill Selection Zone", NULL, &fillSelectionZone)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Fills selection zone with slightly opaque color of selection type.");
            }
            if (ImGui::MenuItem("Hovering Selects", NULL, &hoveringSelects)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Objects that are in the selection zone, change their color to yellow.");
            }

            ImGui::Dummy({ 5.f, 5.f });

            if (ImGui::MenuItem("Deselect Controls", NULL, &deselectControls)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Scale/Rotation/Warp controls are deselected, when touched an empty space in editor.");
            }

            if (ImGui::MenuItem("Game Window Static Ratio", NULL, &gamewindowStaticRatio)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Enables static ratio for game window.");
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
                geode::Mod::get()->setSavedValue<bool>("gamewindow-static-ratio", gamewindowStaticRatio);
                

                lel->updateOptions();
            }
            //geode::Mod::get()->setSavedValue<bool>("select-direction-from-cursor", selectDirectionFromCursor);

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Level Settings")) {
            bool foundSettings = false;
            for (const auto& child : CCArrayExt<CCNode*>(CCDirector::sharedDirector()->getRunningScene()->getChildren())) {
                if (typeinfo_cast<LevelSettingsLayer*>(child)) foundSettings = true;
            }
            if (!foundSettings)
                lel->m_editorUI->onSettings(nullptr);
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