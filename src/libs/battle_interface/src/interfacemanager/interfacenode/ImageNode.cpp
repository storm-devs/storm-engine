#include "ImageNode.h"
#include "../../image/imgrender.h"

BI_ImageNode::BI_ImageNode(BI_ManagerBase *pManager, const char *texture, const FRECT &uv, const RECT &pos,
                           uint32_t color, long nPrioritet)
    : BI_BaseNode(pManager)
{
    m_pImage = Manager()->GetImageRender()->CreateImage(BIType_square, texture, color, uv, pos, nPrioritet);
}

BI_ImageNode::~BI_ImageNode()
{
    STORM_DELETE(m_pImage);
}

void BI_ImageNode::Update()
{
    BI_BaseNode::Update();
}
