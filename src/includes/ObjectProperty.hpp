#pragma once
#include <imgui.h>
#include <imgui-cocos.hpp>
#include <Geode/Geode.hpp>
using namespace cocos2d;
using namespace geode::prelude;

namespace ErGui {
    template<typename GameObjectType, typename PropertyType, typename ImGuiWidgetFunc>
    bool syncObjectProperty(CCArray* objArr, GameObjectType* obj, PropertyType GameObjectType::*fieldPtr, ImGuiWidgetFunc widgetFunc) {
        PropertyType newValue = obj->*fieldPtr;
        if (widgetFunc(&newValue)) {
            obj->*fieldPtr = newValue;
            if (!objArr) return true;
            for (auto objInArr : CCArrayExt<GameObjectType*>(objArr)) {
                objInArr->*fieldPtr = newValue;
            }
            return true;
        }
        return false;
    }


    template<typename GameObjectType, typename PropertyType, typename ImGuiWidgetFunc>
    bool syncObjectPropertyCheckbox(CCArray* objArr, GameObjectType* obj, PropertyType GameObjectType::*fieldPtr, ImGuiWidgetFunc widgetFunc) {
        PropertyType newValue = obj->*fieldPtr;
        if (widgetFunc(&newValue)) {
            obj->*fieldPtr = newValue;
            if (!objArr) return true;
            for (auto objInArr : CCArrayExt<GameObjectType*>(objArr)) {
                objInArr->*fieldPtr = newValue;
            }
            return true;
        }
        return false;
    }
}