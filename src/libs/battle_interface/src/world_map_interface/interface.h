#pragma once

#include "../bi_defines.h"

class WMShipIcon;
class WMShipCommandList;

class WM_INTERFACE : public Entity
{
    VDX9RENDER *rs;

  public:
    WM_INTERFACE();
    ~WM_INTERFACE() override;
    bool Init() override;
    void Realize(uint32_t delta_time);
    uint64_t ProcessMessage(MESSAGE &message) override;
    uint32_t AttributeChanged(ATTRIBUTES *pAttr) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
            // case Stage::execute:
            //    Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

  protected:
    WMShipIcon *m_pShipIcon; // icons for commands and targeting in the combat menu
    WMShipCommandList *m_pCommandList;
    bool m_bVisible;
    int32_t m_nCommandListVerticalOffset;
    int32_t m_nMainCharIndex;
    int32_t m_nCommandMode;

  protected:
    void LoadIniFile();

    void MakeControl();
    void ExecuteCommand(int32_t command);
    void UpdateCommandList() const;
    int32_t GetCurrentCommandTopLine() const;
    int32_t GetCurrentCommandCharacterIndex() const;
    int32_t GetCurrentCommandMode() const;

    bool IsCommandMenuActive() const;
};
