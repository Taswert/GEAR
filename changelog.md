# 1.1.0-beta.3
- Added Full Changelog
- Added Rotation Snap button in Transform module and respective keybind setting (Empty by default)
- Stop button in Toolbar is now always visible (disabled if not playtesting)
- All Layer and Next Free Layer now have their respective keybinds (Ctrl+Left and Ctrl+Right by Default)
- Changed object selection behavior on RMB
- Select Filter now can be toggled in toolbar
- Select Filter and Auto Buildhelper now habe their respective keybinds (Y and H by Default)
- Select Filter toggler in Editor Pause Layer now affects GEAR Select Filter System (Vanilla system is turned off)
- Fixed Glow Orbs selection hitboxes


# 1.1.0-beta.2 - Hotfix
- Fixed position slider crash in Transform Module (OOOOPS....)
- Fixed Rotation for Transform Control in Transform Module


# 1.1.0-beta.1 - Geode v5 and GD 2.2081
## Additions
- Added: Saving Transform Steps
- Added: Ignore Damage in Toolbar
## Fixes
- Keybind rotation for single slabs and slopes was fixed
- Fixed EditorSounds compatibility with moveObjectCall
- Scaling and Rotating in Transform module now updates Touch Triggered Hitbox dynamically
- Transform control now follows rotation and moving by bind and by transform module
- Transform control now works pretty well with Undo/Redo
- Zoom now affects Transform control buttons scale
- Swipe by shift from object now works fine with free move enabled
- Swipe Teleportation bug is fixed (when you release shift while in select mode, there was some crazy teleportation. Also, it's a - game bug, lol)
- Reworked move/rotation by bind system a bit, so Custom Keybinds mod would work with GEAR
## Removes
- Warp Custom Bind was removed (It was in the vanilla game, oops)
- Some unnecessary binds were removed for Custom Keybinds compatibility
## Changes
- ImGui::InputInt in Edit Group Module is changed with ErGui::DragInt for consistency


# 1.0.0-beta.10
- Added Smooth Zooming
- Fixed Selection bug
- Fixed Deletion bug


# 1.0.0-beta.9 - Geode Index Release
- Added Setting (Show Keybinds in Toolbar)
- Fixed Keybinds crashing the game
- Fixed Item aligning in trasnform module in Steps popups


# 1.0.0-beta.8
## Additions
- Properties Module
- Custom Select Mode Colors
- Custom Selection/Hovering Colors
- Auto Build Helper in Toolbar
- ZOrder in Stack Tool
- Vanilla Edit Group in Edit Object module
- Keyframe Index and Group settings
- Flip buttons in Transform Module
- Custom Keybinds integration
- Added Half Grid Move binds (None by default)
- WASD / QE transform object step settings
- Select Mode Icons

## Fixes
- Fixed playtest not working properly, when cursor is on ImGui
- Fixed Text objects multiline hitbox
- Removed kerning constrains for Text Object
- Fixed rolling objects on deletetion
- Updating selected object info on undo/redo/delete
- Fixed Hovering/Selection coloring (now it won't change the color every frame, and it would change color on unhover)
- Fixed delete filter crashes
- Changed Object IDs to Object pictures in Select Filter Module

## WIP
- Edit Object module (Bunch of new triggers + Multiedit for the same type of triggers)


# 1.0.0-beta.7
- Selection critical bug Hotfix
- Selections removed while Playtesting


# 1.0.0-beta.6 - Polz Update
## Additions in triggers
- Text settings @Pololak
- StartPos settings @Pololak
- Checkpoint settings @Pololak
- Force Block settings @Pololak
- Random Trigger settings @Pololak
- Options Trigger settings @Pololak
- Old Enter Effect Triggers settings @Pololak
- Old Color Triggers settings @Pololak

## Other Additions
- Object Type select filter @Pololak
- Reset Filter button @Pololak
- Warp Hotkey (Ctrl + W)
- Delete Layer Button with Confirmation Popup // Layers Tab
- Enable/Disable Link Controls in ToolBar
- Stack Tool (By position offset, rotation and scale) @Pololak & @Taswert
- Hide/Show objects in context menu

## Fixes
- Fixed Hide UI bug @Pololak
- Revamped Text Hitboxes
- Context Menu fixes
- Next Free fixes
- Fixed Parent Group button
- Fully Rewritten Selection
- Dynamic layer object counter refreshing
- Refined Layer Buttons
- Now triggers can be Spawn and Touch triggered at the same time
- Pulse Trigger update label
- Copy/Paste State/Color Tooltips in Disabled mode

## Removed
- Refresh Layers Button
- Delay Constrain in Spawn Trigger


# 1.0.0-beta.5 - Hotfix
- Fixed tooltip crash


# 1.0.0-beta.4
- Changed some std concats to fmt
- Added DEBUG: Show Objects Boxes


# 1.0.0-beta.3
- Fixed RMB Bug


# 1.0.0-beta.2
- Code rework
- Show Particle Icons bug fix


# 1.0.0-beta.1 - Initial Public Beta Release
- Megahack compatibility issue fixed (Thank you @matcool)
- Footer fixes
- Minor Menubar bugfixes


# 0.2.1-beta
- Changed the behavior of the game window when resizing, removing the game window margins
- Added a setting that restores the old game window behavior
- Added tooltips for settings in "Properties"
- Toolbar buttons now have a rounded corners


# 0.2.0-beta
- Widgets in the Transform Module are now more polished, and some tooltips have been adjusted
- Fixed the display of hidden objects. They are now always visible when selected
- Fixed a bug where some settings in the Utilities tab were not saved immediately
- Fixed another Hover Bug that highlighted objects located on a different layer
- Changed how objects are added to the UndoList during transformation, they are no longer added every frame while being modified
- Minor reworks of the external components of various modules. Notably: all checkboxes in the Edit Group module can now be expanded with a single button. Checkboxes are still separated by categories
- Custom Color Type has been temporarily removed from the Edit Color tab
- Fixed a critical bug where attempting to color multiple objects would only apply the color to one
- Build Helper has been added to the context menu
- Build Helper can now be activated by pressing Ctrl+B
- Added an Auto Build Helper setting in the Properties tab of the menu bar
- Slightly redesigned the appearance of the top toolbar panel in Delete mode
- Fixed a bug in the Edit Object module: when selecting a Pulse trigger, color button IDs conflicted


# 0.1.9-beta
- Fixed incorrect display of the Area Fade effect in playtest
- InputFloat widgets in the Transform tab have been changed to DragFloat. Simply put: instead of typing a value, you can now hold LMB on the widget and drag left/right to change it. The step is configured by the +/- buttons to the right of the widget
- The Transform tab for a selected array of objects is now more convenient
- Fixed display of the group list. Group buttons now have a static size and can no longer go off-screen
- Default keybinds are now labeled on the toolbar buttons. (Custom Keybinds not supported yet.)
- Changed the display of layers B1, B2, etc. in edit group module. Instead of a dropdown, they are now buttons
- Toolbar buttons can now align in a single row when the toolbar is stretched
- Removed the scrollbar from the toolbar (scrolling is now possible only with the mouse wheel)
- Default categorization of some objects in the Object List has been revised. To reset categorization, delete the file at ... \Geometry Dash\geode\config\taswert.gear\object_list.json
- Fixed a bug with incorrect line breaks in the Object List
- Added Hide UI
- ObjectList-Module and ActionHistory-Module have been renamed to Object List and Action History, respectively
- The Color Module has been changed. Widgets are now aligned better and distributed into subcategories
- The Color Popup has been updated. Special colors are now at the top. Colors are distributed into categories, and you can now see which color is selected
- Sliders that control level scrolling now have a dynamic size
- Hover Bug fixed. Thanks @Pololak!!!
- The Level Settings button has been moved to the Menu Bar
- Added buttons to the Tool Bar: Copy Values, Paste State, Paste Colors. Also, custom textures and font have been made for them (may change in the future)
- Fixed bugs where vanilla scale, rotation, and warp controls disappeared in Edit mode
- New setting: Deselect Controls. Clicking on an empty area of the editor disables the vanilla rotation, scale, and warp control widgets. Disabled by default


# 0.1.8-beta
- Fixed a bug that could cause the game to crash after pressing any button (in particular the space bar)
- Code refactoring
- Added fill inside the selection area and a respective setting
- Added highlighting of objects that will be affected by the selection area and a respective setting
- Automatic switching to Build Mode after selecting an object is now enabled by default


# 0.1.7-beta
## Fixes
- Fixed a bug with deleting an object using a Toolbar button, which could cause the object to reappear at some point or even crash the game
- Fixed a bug with the "Show Invisible" button functionality
- Fixed "Visible Selection" being reset when selecting objects
- Selection colours are removed from an object when its HSV is changed
- Fixed a bug where the Colour trigger did not update the caption when changing the Target Colour
- Fixed No Touch, now you don't need to re-enter the editor for it to work
- Rotation Axis Lock is now enabled by default
- Zoom buttons have been moved to the left panel
- Offsets and filters in the Select Filter tab have been swapped
- The "CL (Current Layer)" button in the Edit Group tab no longer causes the editor to load indefinitely when setting the object layer to -1
- Changed the texture of the Playback button in the Toolbar
- Fixed the display of objects in the top panel

## Additions
- A button has been added to the Toolbar to call up the vanilla Scale Transform Controls
- A setting has been added that allows you to enable/disable hiding popup with objects when they are selected (Object List Module)
- Presets for grid sizes have been added
- Added a setting that automatically changes the editor mode to Build when an object is selected in the Object List Module
- Added Custom Selected Object Info


# 0.1.6-beta
- Fixed that one old bug with ImGui Inputs


# 0.1.5-beta
- Added Context Menu
- Added Extended layers module (by @RaZooM)
- Added View Module (by @RaZooM & @Táswert)
- Added Fade In, Fade Out and Hold triggers display (Idea by @Pololak)
- Added Menu Bar
- Added AutoBackup (by @RaZooM)
- Added Advanced Regroup // EXPERIMENTAL


# 0.1.4-beta
- Fixed Ctrl+A and lasso selection on All Editor Layer
- Added "All" button which switches Editor Layer to "All" Layer
- Added "Next Free" button for Editor Layer
- Changed the interface element responsible for changing the Z Layer - instead of setting a number, there is now a drop-down list
- Fixed a bug where Free Move broke when Swipe was enabled (OOPS)
- Added a "Current Layer" button to set the layer of an object to the current layer of the editor
- Fixed objects Z Layer dynamic display when Z Layer is changed (Thanks @Pololak)


# 0.1.3-beta
- Subtractive and Intersective modes now have their respective selection colors
- Shortened names for some modules
- Added duplicate button in toolbox
- ImGui UI elements are now losing their focus if game window was clicked
- Fixed Lasso and Undo List compatibility in Subtractive mode
- Fixed Intersective and Subtractive compatibility with Undo List
- Fixed mouse out of window dragging
- Fixed bug, which caused a game crash, when shrinking object to 0 scale without Pos Snap


# 0.1.2-beta
- Added Constrain By Last Object Y checkbox
- Fixed DrawNode display for lasso and rectangle selections (it was below objects, oops)
- Fixed lasso and Intersective/Subtractive compatibility
- Fixed Select Filter causing selection bugs
- Ctrl+A selecting all objects only on current editor layer now


# 0.1.1-beta
- Added slider for Vertical editor move
- Fixed touch bug
- Fixed bug, which allowed for level settings to open nultiple times
- Edit Group Module: "Groups" and "Groups Settings" are in the same subcategory now 


# 0.1.0-beta - Initial Closed Beta Release