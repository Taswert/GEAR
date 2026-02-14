#include "AnimatedObjects.hpp"
using namespace ErGui;

void drawAnimationSettings(GameObject* obj, CCArray* objArr) {
    auto eObj = static_cast<EnhancedGameObject*>(obj);

    ImGui::Checkbox("Randomize Start", &eObj->m_animationRandomizedStart);
    if (ImGui::Checkbox("Use Speed", &eObj->m_animationShouldUseSpeed)) {
        eObj->setupAnimationVariables();
    }
    ImGui::Checkbox("Animate on Trigger", &eObj->m_animateOnTrigger);
    ImGui::Checkbox("Disable Delayed Loop", &eObj->m_disableDelayedLoop);
    ImGui::Checkbox("Disable Anim Shine", &eObj->m_disableAnimShine);
    ImGui::Checkbox("Only if Active", &eObj->m_animateOnlyWhenActive);

    ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
    if (ImGui::DragFloat("Speed", &eObj->m_animationSpeed, .05f, -2.f, 2.f)) {
        eObj->setupAnimationVariables();
    }

    ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
    ImGui::InputInt("Single Frame", &eObj->m_singleFrame);

    ImGui::Checkbox("Offset Anim", &eObj->m_animationOffset);
}

void ErGui::mapAnimatedObjects() {
    triggersMap[1050] = drawAnimationSettings;
    triggersMap[1051] = drawAnimationSettings;
    triggersMap[1052] = drawAnimationSettings;
    triggersMap[3000] = drawAnimationSettings;
    triggersMap[3001] = drawAnimationSettings;
    triggersMap[3002] = drawAnimationSettings;
    triggersMap[1591] = drawAnimationSettings;
    triggersMap[1593] = drawAnimationSettings;
    triggersMap[1053] = drawAnimationSettings;
    triggersMap[1054] = drawAnimationSettings;
    triggersMap[1592] = drawAnimationSettings;
    triggersMap[1516] = drawAnimationSettings;
    triggersMap[920] = drawAnimationSettings;
    triggersMap[923] = drawAnimationSettings;
    triggersMap[924] = drawAnimationSettings;
    triggersMap[921] = drawAnimationSettings;
    triggersMap[2874] = drawAnimationSettings;
    triggersMap[2034] = drawAnimationSettings;
    triggersMap[2865] = drawAnimationSettings;
    triggersMap[2864] = drawAnimationSettings;
    triggersMap[1936] = drawAnimationSettings;
    triggersMap[1937] = drawAnimationSettings;
    triggersMap[1938] = drawAnimationSettings;
    triggersMap[1939] = drawAnimationSettings;
    triggersMap[1583] = drawAnimationSettings;
    triggersMap[1851] = drawAnimationSettings;
    triggersMap[1852] = drawAnimationSettings;
    triggersMap[1854] = drawAnimationSettings;
    triggersMap[1849] = drawAnimationSettings;
    triggersMap[1850] = drawAnimationSettings;
    triggersMap[1518] = drawAnimationSettings;
    triggersMap[1853] = drawAnimationSettings;
    triggersMap[1855] = drawAnimationSettings;
    triggersMap[1858] = drawAnimationSettings;
    triggersMap[1519] = drawAnimationSettings;
    triggersMap[1618] = drawAnimationSettings;
    triggersMap[2040] = drawAnimationSettings;
    triggersMap[2044] = drawAnimationSettings;
    triggersMap[1697] = drawAnimationSettings;
    triggersMap[1698] = drawAnimationSettings;
    triggersMap[1699] = drawAnimationSettings;
    triggersMap[1857] = drawAnimationSettings;
    triggersMap[1860] = drawAnimationSettings;
    triggersMap[2882] = drawAnimationSettings;
    triggersMap[2883] = drawAnimationSettings;
    triggersMap[2884] = drawAnimationSettings;
    triggersMap[2885] = drawAnimationSettings;
    triggersMap[2886] = drawAnimationSettings;
    triggersMap[2887] = drawAnimationSettings;
    triggersMap[2888] = drawAnimationSettings;
    triggersMap[2889] = drawAnimationSettings;
    triggersMap[2890] = drawAnimationSettings;
    triggersMap[2891] = drawAnimationSettings;
    triggersMap[2892] = drawAnimationSettings;
    triggersMap[2893] = drawAnimationSettings;
    triggersMap[1839] = drawAnimationSettings;
    triggersMap[1841] = drawAnimationSettings;
    triggersMap[1842] = drawAnimationSettings;
    triggersMap[2035] = drawAnimationSettings;
    triggersMap[2036] = drawAnimationSettings;
    triggersMap[2038] = drawAnimationSettings;
    triggersMap[2048] = drawAnimationSettings;
    triggersMap[2049] = drawAnimationSettings;
    triggersMap[2050] = drawAnimationSettings;
    triggersMap[2037] = drawAnimationSettings;
    triggersMap[2039] = drawAnimationSettings;
    triggersMap[2869] = drawAnimationSettings;
    triggersMap[2870] = drawAnimationSettings;
    triggersMap[2871] = drawAnimationSettings;
    triggersMap[2052] = drawAnimationSettings;
    triggersMap[2053] = drawAnimationSettings;
    triggersMap[2868] = drawAnimationSettings;
    triggersMap[2020] = drawAnimationSettings;
    triggersMap[2021] = drawAnimationSettings;
    triggersMap[2041] = drawAnimationSettings;
    triggersMap[2042] = drawAnimationSettings;
    triggersMap[2026] = drawAnimationSettings;
    triggersMap[2027] = drawAnimationSettings;
    triggersMap[2028] = drawAnimationSettings;
    triggersMap[2867] = drawAnimationSettings;
    triggersMap[2046] = drawAnimationSettings;
    triggersMap[2047] = drawAnimationSettings;
    triggersMap[2051] = drawAnimationSettings;
    triggersMap[2055] = drawAnimationSettings;
    triggersMap[2873] = drawAnimationSettings;
    triggersMap[2879] = drawAnimationSettings;
    triggersMap[2876] = drawAnimationSettings;
    triggersMap[2880] = drawAnimationSettings;
    triggersMap[2045] = drawAnimationSettings;
    triggersMap[2029] = drawAnimationSettings;
    triggersMap[2223] = drawAnimationSettings;
    triggersMap[2246] = drawAnimationSettings;
    triggersMap[2629] = drawAnimationSettings;
    triggersMap[2630] = drawAnimationSettings;
    triggersMap[3303] = drawAnimationSettings;
    triggersMap[3304] = drawAnimationSettings;
    triggersMap[3482] = drawAnimationSettings;
    triggersMap[3483] = drawAnimationSettings;
    triggersMap[3484] = drawAnimationSettings;
    triggersMap[2605] = drawAnimationSettings;
    triggersMap[2694] = drawAnimationSettings;
    triggersMap[4211] = drawAnimationSettings;
    triggersMap[3119] = drawAnimationSettings;
    triggersMap[3120] = drawAnimationSettings;
    triggersMap[3121] = drawAnimationSettings;
    triggersMap[3219] = drawAnimationSettings;
    triggersMap[1856] = drawAnimationSettings;
    triggersMap[2022] = drawAnimationSettings;
    triggersMap[2023] = drawAnimationSettings;
    triggersMap[2024] = drawAnimationSettings;
    triggersMap[2025] = drawAnimationSettings;
    triggersMap[2030] = drawAnimationSettings;
    triggersMap[2031] = drawAnimationSettings;
    triggersMap[2032] = drawAnimationSettings;
    triggersMap[2033] = drawAnimationSettings;
    triggersMap[2043] = drawAnimationSettings;
    triggersMap[2054] = drawAnimationSettings;
    triggersMap[2872] = drawAnimationSettings;
    triggersMap[2875] = drawAnimationSettings;
    triggersMap[2877] = drawAnimationSettings;
    triggersMap[2878] = drawAnimationSettings;
    triggersMap[2881] = drawAnimationSettings;
    triggersMap[2894] = drawAnimationSettings;
    triggersMap[3492] = drawAnimationSettings;
    triggersMap[3493] = drawAnimationSettings;
    triggersMap[4300] = drawAnimationSettings;
    triggersMap[1329] = drawAnimationSettings;
    triggersMap[1614] = drawAnimationSettings;
}
