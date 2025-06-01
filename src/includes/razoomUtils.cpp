#include "razoomUtils.hpp"
#include <numeric>


const float defaultObjSprSize = 60.f;

namespace {
    
    // fix for the GameObject setPosition function
    void setGameObjScalePositionFix(GameObject* obj, CCPoint pos, float scaleMultiplier = 1) {
        if (obj->m_colorSprite) {
            auto sprPos = obj->m_colorSprite->getPosition();
            auto sprSc = obj->m_colorSprite->getScale();
            obj->setPosition(pos);
            obj->setScale(obj->getScale() * scaleMultiplier);
            obj->m_colorSprite->setPosition(sprPos);
            obj->m_colorSprite->setScale(sprSc);
        } else {
            obj->setPosition(pos);
            obj->setScale(obj->getScale() * scaleMultiplier);
        }
    }

    struct ObjectBoundaries {
        float t, b, l, r; // top, bottom, left, right
    };

    // still most likely incorrect
    ObjectBoundaries recursiveGetRealSize(CCNode* obj, CCNode* objParent) { 
        float t, b, l, r;

        auto box = obj->boundingBox();
        auto tr = objParent->convertToWorldSpace(ccp(box.getMaxX(), box.getMaxY()));
        auto bl = objParent->convertToWorldSpace(ccp(box.getMinX(), box.getMinY()));
        
        r = tr.x;
        l = bl.x;
        t = tr.y;
        b = bl.y;

        for (auto* child : CCArrayExt<CCNode*>(obj->getChildren())) {
            auto childSz = recursiveGetRealSize(child, obj);
            if (childSz.r > r) r = childSz.r;
            if (childSz.l < l) l = childSz.l;
            if (childSz.t > t) t = childSz.t;
            if (childSz.b < b) b = childSz.b;
        }

        return {t, b, l, r};
    }


    void setColorToGameObjectNew(GameObject* gameObj, bool isBright) {
        // ! mostly decompiled code of EditorUI::updateCreateMenu() that sets the color
        ccColor3B color = isBright ? ccc3(255, 255, 255) : ccc3(127, 127, 127);
        int objId;
        if ((int)gameObj->m_classType == 1) {
            bool cVar14;
            if (gameObj->m_customColorType == 0) {
                cVar14 = gameObj->m_maybeNotColorable;
            } else {
                cVar14 = (gameObj->m_customColorType == 1);
            }
            if (cVar14 || gameObj->m_colorSprite || 
                gameObj->m_baseColor->m_defaultColorID == 0x3ec || 
                gameObj->m_baseColor->m_defaultColorID == 0x0  || 
                /* (*(char *)(gameObj + 0xdf) == '\0') */ false) {
                goto LAB_14010da56;
            }
            color = isBright ? ccc3(200, 200, 255) : ccc3(100, 100, 127);
            goto LAB_14010dac2;
        }
    LAB_14010da56:
        objId = gameObj->m_objectID;
        bool bVar12;
        if (objId < 0x531) {
            if (((objId == 0x530) || (objId == 0x396)) || (objId == 0x397)) goto LAB_14010daad;
            bVar12 = (objId == 0x52f);
    LAB_14010da89:
            if (bVar12) goto LAB_14010daad;
            auto piVar3 = gameObj->m_baseColor;
            if (piVar3 != 0) {
                objId = piVar3->m_colorID;
                if ((piVar3->m_defaultColorID == objId) || (objId == 0x0)) {
                    objId = piVar3->m_defaultColorID;
                }
                if (objId == 0x3f2) goto LAB_14010daad;
            }
        }
        else {
            if (objId != 0x630) {
                bVar12 = (objId == 0x7dc);
                goto LAB_14010da89;
            }
    LAB_14010daad:
            color = isBright ? ccc3(0, 0, 0) : ccc3(127, 127, 127);
        }
    LAB_14010dac2:
        gameObj->setObjectColor(color);
        color = isBright ? ccc3(200, 200, 255) : ccc3(100, 100, 127);
        gameObj->setChildColor(color);
    }


    CCSprite* renderSprites(CCSprite* sprites, float size) {
        auto renderer = CCRenderTexture::create(size, size);
        renderer->begin();
        sprites->visit();
        renderer->end();
        return renderer->getSprite();
    }

} // namespace


// возвращает правильно покрашенный GameObject в виде единого квадратного спрайта 60х60
CCSprite* ErGui::getGameObjectAsSingleSpriteById(uint16_t objId) {
    const float size = defaultObjSprSize;
    auto editor = EditorUI::get();

    CreateMenuItem* btn = editor->getCreateBtn(objId, 1);
	if (editor->m_createButtonArray->lastObject() == btn) {
		editor->m_createButtonArray->removeLastObject();
	}

	auto buttonSpr = static_cast<ButtonSprite*>(btn->getNormalImage());
	GameObject* obj = static_cast<GameObject*>(buttonSpr->m_subSprite);
	setColorToGameObjectNew(obj, true);

    auto realSz = recursiveGetRealSize(obj, buttonSpr); // чтобы вписать объект в квадрат
    auto contSize = CCSize(realSz.r - realSz.l, realSz.t - realSz.b);
    float maxSize = std::max({30.f, contSize.width, contSize.height});
    float scaleFactor = size / maxSize;

    setGameObjScalePositionFix(obj, ccp(size / 2,size / 2), scaleFactor);

    return renderSprites(obj, size);
}

CCSprite* ErGui::getGameObjectsAsSingleSprite(const std::string &objString) {
    const float size = defaultObjSprSize;
    auto editor = EditorUI::get();
    auto arr = CCArray::create();

    auto spr = editor->spriteFromObjectString(objString, false, false, 0, arr, nullptr, nullptr);

    for (auto* el : CCArrayExt<GameObject*>(arr)) {
        setColorToGameObjectNew(el, true);
    }

    spr->setScale(std::min(size / spr->getContentHeight(), size / spr->getContentWidth()));
    spr->setPosition(ccp(size / 2,size / 2));

    return renderSprites(spr, size);
}




float ErGui::getFPS() { // stolen from GDH
    static std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
    static std::vector<float> frameTimes;

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;

    frameTimes.push_back(deltaTime);
    if (frameTimes.size() > 60)
        frameTimes.erase(frameTimes.begin());

    float avgTime = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0f);
    avgTime /= frameTimes.size();
    return 1.f / avgTime;
}




