#pragma once

#include "BaseNode.h"

class IBIString;

class BI_StringNode : public BI_BaseNode
{
  public:
    BI_StringNode(BI_ManagerBase *pManager, const char *text, const char *font, uint32_t color, float scale,
                  const RECT &pos, long nHAlign, long nVAlign, long prioritet);
    virtual ~BI_StringNode();

    void Update() override;

  protected:
    IBIString *m_pStr;
};
