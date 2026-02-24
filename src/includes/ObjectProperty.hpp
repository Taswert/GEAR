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
        // for (auto objInArr : CCArrayExt<GameObjectType*>(objArr)) {
        //     if (objInArr->*fieldPtr != newValue) {
        //         isMixed = true;
        //         break;
        //     }
        // }

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


    template<typename GameObjectType, typename PropertyType, typename WidgetFunc>
    bool syncObjectGroupProperty(CCArray* objArr, PropertyType* maxValue, PropertyType* minValue, PropertyType GameObjectType::*fieldPtr, WidgetFunc widgetFunc) {
        PropertyType newMaxValue = *maxValue; // Custom Type / MaxValue
        std::string valueFormat = 
            *maxValue != *minValue ? 
            fmt::format("{} .. {}", *minValue, *maxValue) :
            fmt::format("{}", *maxValue);
        if (widgetFunc(&newMaxValue, valueFormat)) {
            PropertyType delta = newMaxValue - *maxValue;

            for (auto obj : CCArrayExt<GameObjectType*>(objArr)) {
                obj->*fieldPtr += delta;
            }

            // in place of groupInfoUpdate()
            *maxValue += delta;
            *minValue += delta;

            return true;
        }

        return false;
    }


    
}

