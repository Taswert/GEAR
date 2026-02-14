#include "CollectibleObjects.hpp"
using namespace ErGui;

void drawCollectibleSettings(GameObject* obj, CCArray* objArr) {
    auto cObj = static_cast<EffectGameObject*>(obj);

    ImGui::Checkbox("Pickup Item", &cObj->m_collectibleIsPickupItem);
    if (cObj->m_collectibleIsPickupItem) {
        ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
        ImGui::InputInt("Item ID", &cObj->m_itemID);

        ImGui::Checkbox("Sub Count", &cObj->m_subtractCount);
    }

    ImGui::Checkbox("Toggle Trigger", &cObj->m_collectibleIsToggleTrigger);

    if (cObj->m_collectibleIsToggleTrigger) {
        ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
        ImGui::InputInt("Group ID", &cObj->m_targetGroupID);

        ImGui::Checkbox("Enable Group", &cObj->m_activateGroup);
    }

    ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
    ImGui::InputInt("Particle", &cObj->m_collectibleParticleID);

    ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
    ImGui::InputInt("Points", &cObj->m_collectiblePoints);

    ImGui::Checkbox("No Animation", &cObj->m_hasNoAnimation);
}

void ErGui::mapCollectibleObjects() {
    triggersMap[1614] = drawCollectibleSettings;
    triggersMap[3601] = drawCollectibleSettings;
    triggersMap[1275] = drawCollectibleSettings;
    triggersMap[1587] = drawCollectibleSettings;
    triggersMap[1589] = drawCollectibleSettings;
    triggersMap[1598] = drawCollectibleSettings;
    triggersMap[4401] = drawCollectibleSettings;
    triggersMap[4402] = drawCollectibleSettings;
    triggersMap[4403] = drawCollectibleSettings;
    triggersMap[4404] = drawCollectibleSettings;
    triggersMap[4405] = drawCollectibleSettings;
    triggersMap[4406] = drawCollectibleSettings;
    triggersMap[4518] = drawCollectibleSettings;
    triggersMap[4410] = drawCollectibleSettings;
    triggersMap[4411] = drawCollectibleSettings;
    triggersMap[4412] = drawCollectibleSettings;
    triggersMap[4421] = drawCollectibleSettings;
    triggersMap[4529] = drawCollectibleSettings;
    triggersMap[4530] = drawCollectibleSettings;
    triggersMap[4531] = drawCollectibleSettings;
    triggersMap[4532] = drawCollectibleSettings;
    triggersMap[4414] = drawCollectibleSettings;
    triggersMap[4415] = drawCollectibleSettings;
    triggersMap[4416] = drawCollectibleSettings;
    triggersMap[4417] = drawCollectibleSettings;
    triggersMap[4418] = drawCollectibleSettings;
    triggersMap[4419] = drawCollectibleSettings;
    triggersMap[4420] = drawCollectibleSettings;
    triggersMap[4413] = drawCollectibleSettings;
    triggersMap[4422] = drawCollectibleSettings;
    triggersMap[4423] = drawCollectibleSettings;
    triggersMap[4424] = drawCollectibleSettings;
    triggersMap[4425] = drawCollectibleSettings;
    triggersMap[4426] = drawCollectibleSettings;
    triggersMap[4521] = drawCollectibleSettings;
    triggersMap[4526] = drawCollectibleSettings;
    triggersMap[4407] = drawCollectibleSettings;
    triggersMap[4408] = drawCollectibleSettings;
    triggersMap[4409] = drawCollectibleSettings;
    triggersMap[4427] = drawCollectibleSettings;
    triggersMap[4428] = drawCollectibleSettings;
    triggersMap[4429] = drawCollectibleSettings;
    triggersMap[4430] = drawCollectibleSettings;
    triggersMap[4438] = drawCollectibleSettings;
    triggersMap[4439] = drawCollectibleSettings;
    triggersMap[4440] = drawCollectibleSettings;
    triggersMap[4441] = drawCollectibleSettings;
    triggersMap[4442] = drawCollectibleSettings;
    triggersMap[4443] = drawCollectibleSettings;
    triggersMap[4446] = drawCollectibleSettings;
    triggersMap[4447] = drawCollectibleSettings;
    triggersMap[4448] = drawCollectibleSettings;
    triggersMap[4449] = drawCollectibleSettings;
    triggersMap[4519] = drawCollectibleSettings;
    triggersMap[4451] = drawCollectibleSettings;
    triggersMap[4452] = drawCollectibleSettings;
    triggersMap[4453] = drawCollectibleSettings;
    triggersMap[4454] = drawCollectibleSettings;
    triggersMap[4455] = drawCollectibleSettings;
    triggersMap[4456] = drawCollectibleSettings;
    triggersMap[4457] = drawCollectibleSettings;
    triggersMap[4458] = drawCollectibleSettings;
    triggersMap[4459] = drawCollectibleSettings;
    triggersMap[4460] = drawCollectibleSettings;
    triggersMap[4461] = drawCollectibleSettings;
    triggersMap[4462] = drawCollectibleSettings;
    triggersMap[4463] = drawCollectibleSettings;
    triggersMap[4464] = drawCollectibleSettings;
    triggersMap[4522] = drawCollectibleSettings;
    triggersMap[4465] = drawCollectibleSettings;
    triggersMap[4466] = drawCollectibleSettings;
    triggersMap[4467] = drawCollectibleSettings;
    triggersMap[4468] = drawCollectibleSettings;
    triggersMap[4469] = drawCollectibleSettings;
    triggersMap[4470] = drawCollectibleSettings;
    triggersMap[4471] = drawCollectibleSettings;
    triggersMap[4472] = drawCollectibleSettings;
    triggersMap[4473] = drawCollectibleSettings;
    triggersMap[4474] = drawCollectibleSettings;
    triggersMap[4475] = drawCollectibleSettings;
    triggersMap[4476] = drawCollectibleSettings;
    triggersMap[4477] = drawCollectibleSettings;
    triggersMap[4480] = drawCollectibleSettings;
    triggersMap[4481] = drawCollectibleSettings;
    triggersMap[4482] = drawCollectibleSettings;
    triggersMap[4483] = drawCollectibleSettings;
    triggersMap[4479] = drawCollectibleSettings;
    triggersMap[4478] = drawCollectibleSettings;
    triggersMap[4484] = drawCollectibleSettings;
    triggersMap[4485] = drawCollectibleSettings;
    triggersMap[4486] = drawCollectibleSettings;
    triggersMap[4487] = drawCollectibleSettings;
    triggersMap[4488] = drawCollectibleSettings;
    triggersMap[4538] = drawCollectibleSettings;
    triggersMap[4489] = drawCollectibleSettings;
    triggersMap[4490] = drawCollectibleSettings;
    triggersMap[4491] = drawCollectibleSettings;
    triggersMap[4492] = drawCollectibleSettings;
    triggersMap[4493] = drawCollectibleSettings;
    triggersMap[4494] = drawCollectibleSettings;
    triggersMap[4495] = drawCollectibleSettings;
    triggersMap[4496] = drawCollectibleSettings;
    triggersMap[4497] = drawCollectibleSettings;
    triggersMap[4537] = drawCollectibleSettings;
    triggersMap[4527] = drawCollectibleSettings;
    triggersMap[4528] = drawCollectibleSettings;
    triggersMap[4434] = drawCollectibleSettings;
    triggersMap[4435] = drawCollectibleSettings;
    triggersMap[4498] = drawCollectibleSettings;
    triggersMap[4499] = drawCollectibleSettings;
    triggersMap[4500] = drawCollectibleSettings;
    triggersMap[4501] = drawCollectibleSettings;
    triggersMap[4502] = drawCollectibleSettings;
    triggersMap[4503] = drawCollectibleSettings;
    triggersMap[4504] = drawCollectibleSettings;
    triggersMap[4505] = drawCollectibleSettings;
    triggersMap[4506] = drawCollectibleSettings;
    triggersMap[4507] = drawCollectibleSettings;
    triggersMap[4508] = drawCollectibleSettings;
    triggersMap[4509] = drawCollectibleSettings;
    triggersMap[4510] = drawCollectibleSettings;
    triggersMap[4511] = drawCollectibleSettings;
    triggersMap[4512] = drawCollectibleSettings;
    triggersMap[4513] = drawCollectibleSettings;
    triggersMap[4514] = drawCollectibleSettings;
    triggersMap[4533] = drawCollectibleSettings;
    triggersMap[4534] = drawCollectibleSettings;
    triggersMap[4535] = drawCollectibleSettings;
    triggersMap[4431] = drawCollectibleSettings;
    triggersMap[4432] = drawCollectibleSettings;
    triggersMap[4433] = drawCollectibleSettings;
    triggersMap[4436] = drawCollectibleSettings;
    triggersMap[4437] = drawCollectibleSettings;
    triggersMap[4444] = drawCollectibleSettings;
    triggersMap[4445] = drawCollectibleSettings;
    triggersMap[4450] = drawCollectibleSettings;
    triggersMap[4515] = drawCollectibleSettings;
    triggersMap[4516] = drawCollectibleSettings;
    triggersMap[4517] = drawCollectibleSettings;
    triggersMap[4520] = drawCollectibleSettings;
    triggersMap[4523] = drawCollectibleSettings;
    triggersMap[4524] = drawCollectibleSettings;
    triggersMap[4525] = drawCollectibleSettings;
    triggersMap[4536] = drawCollectibleSettings;
    triggersMap[4539] = drawCollectibleSettings;
}