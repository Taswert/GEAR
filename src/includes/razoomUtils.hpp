#pragma once
// #include <imgui.h>
#include <imgui-cocos.hpp>
#include <Geode/Geode.hpp>
// using namespace cocos2d;
using namespace geode::prelude;


namespace ErGui {

    CCSprite* getGameObjectAsSingleSpriteById(uint16_t objId);
    CCSprite* getGameObjectsAsSingleSprite(const std::string &objString);

    float getFPS(); // must be called exactly once per frame
    float getLastObjectXFast();
    float getLastObjectYFast();

    EditorPauseLayer* getFakePauseLayer();

    inline int lastObjCountX = 0;
    inline int lastObjCountY = 0;
    inline GameObject* lastObjX = nullptr;
    inline GameObject* lastObjY = nullptr;
} // ErGui
