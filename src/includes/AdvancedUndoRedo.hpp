#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "CopyEGMState.hpp"
using namespace geode::prelude;


namespace ErGui {
    enum class EnhancedUndoCommand {
        Delete = 1,
        New = 2,
        Paste = 3,
        DeleteMulti = 4,
        Transform = 5,
        Select = 6,
        ObjectState = 7,
        GroupAdd = 8,
        GroupRemove = 9,
        ObjectColor = 10,
        LevelValues = 11,
        EditorValues = 12,
    };

    class EnhancedUndoObject : public UndoObject {
    public:
		CopyEGMState m_copyState;
        int m_intValue;             // so basically these fields are changed value of matching type. Group, layer, or something like that.
		bool m_boolValue;           // this could be something like checkbox value

        static EnhancedUndoObject* create(GameObject* obj, EnhancedUndoCommand command);
        static EnhancedUndoObject* create(GameObject* obj, EnhancedUndoCommand command, int intValue);
        static EnhancedUndoObject* create(GameObject* obj, EnhancedUndoCommand command, bool boolValue);
        bool init(GameObject* obj, EnhancedUndoCommand command);
        bool init(GameObject* obj, EnhancedUndoCommand command, int intValue);
        bool init(GameObject* obj, EnhancedUndoCommand command, bool boolValue);
    };
}