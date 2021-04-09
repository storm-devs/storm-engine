#pragma once

#include "../bi_defines.h"

class WMShipIcon;
class WMShipCommandList;

class WM_INTERFACE : public Entity
{
    VDX9RENDER *rs;

  public:
    WM_INTERFACE();
    ~WM_INTERFACE();
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
    long m_nCommandListVerticalOffset;
    long m_nMainCharIndex;
    long m_nCommandMode;

  protected:
    void LoadIniFile();

    void MakeControl();
    void ExecuteCommand(long command);
    void UpdateCommandList() const;
    long GetCurrentCommandTopLine() const;
    long GetCurrentCommandCharacterIndex() const;
    long GetCurrentCommandMode() const;

    bool IsCommandMenuActive() const;
};
