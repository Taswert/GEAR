#pragma once
namespace SelectedObjectInfo {
    inline bool g_soiNeedsUpdate = false;
    void update();
    bool init(LevelEditorLayer* lel);
    void selectObjectsBefore();
    void selectObjectsAfter();
    void selectObjectBefore();
    void selectObjectAfter();
    void deselectAll();
    void onDelete();
    void onDeleteSelected();
    void undoLastAction();
    void redoLastAction();
}