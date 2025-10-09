#include "./GlobalDockingView.hpp"

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
            bool triangleColorWheel =       mod->getSavedValue<bool>("triangle-color-wheel", true);       //
            bool rotateColorWheel =         mod->getSavedValue<bool>("rotate-color-wheel", false);        //
            bool hideObjectListPopup =      mod->getSavedValue<bool>("hide-object-list-popup", true);     //
            bool autoswitchToBuildMode =    mod->getSavedValue<bool>("autoswitch-to-build-mode", true);   //
            bool showLinkControls =         gm->getGameVariable("0097");                                                // 
            bool showZoomControls =         mod->getSavedValue<bool>("show-zoom-controls", true);         // Shows zoom controls buttons on toolbox
            bool showObjectInfo =           gm->getGameVariable("0041");
			bool fillSelectionZone =        mod->getSavedValue<bool>("fill-selection-zone", false);       // Fills selection zone with solid color
			bool hoveringSelects =          mod->getSavedValue<bool>("hovering-selects", true);           // Hovers objects in the selection zone
            bool deselectControls =         mod->getSavedValue<bool>("deselect-controls", false);         // Deselects GJRotation/Scale/TransformControl, when clicking on empty space in editor
            bool autoBuildhelper =          mod->getSavedValue<bool>("auto-buildhelper", false);          // Automatically applies Build helper on duplicated objects
			bool gamewindowStaticRatio =    mod->getSavedValue<bool>("gamewindow-static-ratio", false);   // Game window keeps executable window ratio
            bool debugShowObjectsBoxes =    mod->getSavedValue<bool>("debug-show-objects-boxes", false);  // Shows hitboxes of all objects
            //bool selectDirectionFromCursor = geode::Mod::get()->getSavedValue<bool>("select-direction-from-cursor");


            bool isAnyItemClicked = false;
            
            if (ImGui::BeginMenu("Selected Object Info")) {
                bool soiPosition =      mod->getSavedValue<bool>("soi-position", true);
                bool soiRotation =      mod->getSavedValue<bool>("soi-rotation", true);
                bool soiScale =         mod->getSavedValue<bool>("soi-scale", true);
                bool soiColor =         mod->getSavedValue<bool>("soi-color", true);
                bool soiHSV =           mod->getSavedValue<bool>("soi-hsv", false);
                bool soiGroups =        mod->getSavedValue<bool>("soi-groups", true);
                bool soiZLayer =        mod->getSavedValue<bool>("soi-zlayer", true);
                bool soiZOrder =        mod->getSavedValue<bool>("soi-zorder", true);
                bool soiObjectID =      mod->getSavedValue<bool>("soi-objectid", false);
                bool soiTargetGroup =   mod->getSavedValue<bool>("soi-targetgroup", true);
                bool soiItemID =        mod->getSavedValue<bool>("soi-itemid", false);
                bool soiBlockID =       mod->getSavedValue<bool>("soi-blockid", false);
                bool soiParticles =     mod->getSavedValue<bool>("soi-particles", true);
                bool soiHidden =        mod->getSavedValue<bool>("soi-hidden", true);
                bool soiNoTouch =       mod->getSavedValue<bool>("soi-no-touch", true);
                bool soiHighDetail =    mod->getSavedValue<bool>("soi-high-detail", false);
                bool soiObjectCount =   mod->getSavedValue<bool>("soi-object-count", true);

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

                mod->setSavedValue<bool>("soi-position", soiPosition);
                mod->setSavedValue<bool>("soi-rotation", soiRotation);
                mod->setSavedValue<bool>("soi-scale", soiScale);
                mod->setSavedValue<bool>("soi-color", soiColor);
                mod->setSavedValue<bool>("soi-hsv", soiHSV);
                mod->setSavedValue<bool>("soi-groups", soiGroups);
                mod->setSavedValue<bool>("soi-zlayer", soiZLayer);
                mod->setSavedValue<bool>("soi-zorder", soiZOrder);
                mod->setSavedValue<bool>("soi-objectid", soiObjectID);
                mod->setSavedValue<bool>("soi-targetgroup", soiTargetGroup);
                mod->setSavedValue<bool>("soi-itemid", soiItemID);
                mod->setSavedValue<bool>("soi-blockid", soiBlockID);
                mod->setSavedValue<bool>("soi-particles", soiParticles);
                mod->setSavedValue<bool>("soi-hidden", soiHidden);
                mod->setSavedValue<bool>("soi-no-touch", soiNoTouch);
                mod->setSavedValue<bool>("soi-high-detail", soiHighDetail);
                mod->setSavedValue<bool>("soi-object-count", soiObjectCount);

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
                ImGui::BeginTooltip();
                ImGui::TextUnformatted("Warp control buttons are 50% smaller (Restart editor to work).");
                ImGui::EndTooltip();
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

            ImGui::Dummy({ 5.f, 5.f });
            if (ImGui::MenuItem("DEBUG: Show Objs Box", NULL, &debugShowObjectsBoxes)) {
                isAnyItemClicked = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("Shows objects hitboxes");
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
                mod->setSavedValue<bool>("triangle-color-wheel", triangleColorWheel);
                mod->setSavedValue<bool>("rotate-color-wheel", rotateColorWheel);
                mod->setSavedValue<bool>("hide-object-list-popup", hideObjectListPopup);
                mod->setSavedValue<bool>("autoswitch-to-build-mode", autoswitchToBuildMode);
                gm->setGameVariable("0097", showLinkControls);
                mod->setSavedValue<bool>("show-zoom-controls", showZoomControls);
                gm->setGameVariable("0041", showObjectInfo);
                mod->setSavedValue<bool>("fill-selection-zone", fillSelectionZone);
                mod->setSavedValue<bool>("hovering-selects", hoveringSelects);
                mod->setSavedValue<bool>("deselect-controls", deselectControls);
                mod->setSavedValue<bool>("auto-buildhelper", autoBuildhelper);
                mod->setSavedValue<bool>("gamewindow-static-ratio", gamewindowStaticRatio);
                mod->setSavedValue<bool>("debug-show-objects-boxes", debugShowObjectsBoxes);
                

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