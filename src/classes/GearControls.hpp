#include "Geode/modify/GJRotationControl.hpp"
#include "Geode/modify/GJScaleControl.hpp"
#include "Geode/modify/GJTransformControl.hpp"

#define MAKE_TOUCH_FLAG_MODIFY(GearClass, BaseClass)            \
class $modify(GearClass, BaseClass) {                           \
    struct Fields {                                             \
        bool m_touchedControl = false;                          \
    };                                                          \
                                                                \
    virtual bool ccTouchBegan(CCTouch* p0, CCEvent* p1) {       \
        if (!BaseClass::ccTouchBegan(p0, p1))                   \
            return false;                                       \
        this->m_fields->m_touchedControl = true;                \
        return true;                                            \
    }                                                           \
                                                                \
    virtual void ccTouchEnded(CCTouch* p0, CCEvent* p1) {       \
        this->m_fields->m_touchedControl = false;               \
        BaseClass::ccTouchEnded(p0, p1);                        \
    }                                                           \
};

MAKE_TOUCH_FLAG_MODIFY(GearRotationControl, GJRotationControl)
MAKE_TOUCH_FLAG_MODIFY(GearScaleControl, GJScaleControl)
MAKE_TOUCH_FLAG_MODIFY(GearTransformControl, GJTransformControl)
