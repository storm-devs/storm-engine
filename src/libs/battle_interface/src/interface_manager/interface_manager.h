#pragma once

#include "../bi_defines.h"
#include "base_manager.h"
#include <vector>

class BI_BaseGroup;

class BI_InterfaceManager : public BI_ManagerBase
{
  public:
    BI_InterfaceManager();
    ~BI_InterfaceManager() override;

    bool Init() override;
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    uint64_t ProcessMessage(MESSAGE &message) override;

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
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    BI_ManagerNodeBase *CreateImageNode(const char *texture, const FRECT &uv, const RECT &pos, uint32_t color,
                                        int32_t nPrioritet) override;
    BI_ManagerNodeBase *CreateStringNode(const char *text, const char *font, uint32_t color, float scale,
                                         const RECT &pos, int32_t nHAlign, int32_t nVAlign, int32_t prioritet) override;
    void DeleteNode(BI_ManagerNodeBase *pNod) override;

  protected:
    int32_t MsgLoadSheet(MESSAGE &message);
    int32_t MsgCreateImage(MESSAGE &message);
    int32_t MsgCreateString(MESSAGE &message);
    int32_t MsgDeleteNode(MESSAGE &message);
    int32_t MsgEvent(MESSAGE &message);

    std::vector<BI_ManagerNodeBase *> m_aNodes;

    BI_BaseGroup *m_pInterfaceSheet;
};
