#pragma once

#include "bi_utils.h"
#include "bi_defines.h"
#include "battle_navigator.h"
#include "dx9render.h"

class BIShipIcon;
class ShipInfoImages;

class BATTLE_INTERFACE : public Entity
{
    VDX9RENDER *rs;

  public:
    BATTLE_INTERFACE();
    ~BATTLE_INTERFACE() override;
    bool Init() override;
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    uint64_t ProcessMessage(MESSAGE &message) override;
    uint32_t AttributeChanged(ATTRIBUTES *pAttr) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
        case Stage::lost_render:
            LostRender();
            break;
        case Stage::restore_render:
            RestoreRender();
            break;
        }
    }

    void LostRender()
    {
        BattleNavigator.LostRender();
    };

    void RestoreRender()
    {
        BattleNavigator.RestoreRender();
    };

  protected:
    BATTLE_NAVIGATOR BattleNavigator; // navigation icons
    // MESSAGE_ICONS *        m_pMessageIcons;
    BIShipIcon *m_pShipIcon; // icons of commands and target pointers in the combat menu
    bool m_bShowCommandMenu;
    bool m_bShowBattleNavigator;
    bool m_bYesShowAll;
    bool m_bVisible;

    float m_fBlinkSpeed;
    float m_fCurBlinkTime;

    bool m_bNeedIslandSet;

    bool m_bMyShipView;

    std::vector<BITextInfo> m_TextArray;
    BILinesInfo m_LinesInfo;
    bool m_bShowBattleBorder;
    BIBorderInfo m_BattleBorder;
    BIImagesInfo m_ImagesInfo;

    ShipInfoImages *m_pShipInfoImages;

  protected:
    void LoadIniFile();
    void CheckSeaState();
    void EnableMessageIcons(VDATA *pvdat);
};
