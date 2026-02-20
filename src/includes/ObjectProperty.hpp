#pragma once
#include <functional>
#include <imgui.h>
#include <imgui-cocos.hpp>
#include <Geode/Geode.hpp>
using namespace cocos2d;
using namespace geode::prelude;

namespace ErGui {
    template<typename GameObjectType, typename PropertyType, typename WidgetFunc>
    bool syncObjectProperty(CCArray* objArr, GameObjectType* obj, PropertyType GameObjectType::*fieldPtr, WidgetFunc widgetFunc) {
        PropertyType newValue = obj->*fieldPtr;
        bool isMixed = false;
        // Should remake it, so it would not cause nay lags
        for (auto objInArr : CCArrayExt<GameObjectType*>(objArr)) {
            if (objInArr->*fieldPtr != newValue) {
                isMixed = true;
                break;
            }
        }

        if (widgetFunc(&newValue, isMixed)) {
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