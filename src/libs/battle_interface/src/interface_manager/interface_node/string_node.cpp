#include "string_node.h"
#include "../../image/img_render.h"

BI_StringNode::BI_StringNode(BI_ManagerBase *pManager, const char *text, const char *font, uint32_t color, float scale,
                             const RECT &pos, int32_t nHAlign, int32_t nVAlign, int32_t prioritet)
    : BI_BaseNode(pManager)
{
    m_pStr = Manager()->GetImageRender()->CreateString(text, font, scale, color, nVAlign, nHAlign, pos, prioritet);
}

BI_StringNode::~BI_StringNode()
{
    STORM_DELETE(m_pStr);
}

void BI_StringNode::Update()
{
}
