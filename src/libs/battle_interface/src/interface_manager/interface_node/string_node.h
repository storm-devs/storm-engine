#pragma once

#include "base_node.h"

class IBIString;

class BI_StringNode : public BI_BaseNode
{
  public:
    BI_StringNode(BI_ManagerBase *pManager, const char *text, const char *font, uint32_t color, float scale,
                  const RECT &pos, int32_t nHAlign, int32_t nVAlign, int32_t prioritet);
    ~BI_StringNode() override;

    void Update() override;

  protected:
    IBIString *m_pStr;
};
