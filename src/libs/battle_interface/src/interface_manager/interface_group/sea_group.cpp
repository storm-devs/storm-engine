#include "sea_group.h"
#include "bi_utils.h"
#include "platform/platform.hpp"

BI_SeaGroup::BI_SeaGroup(BI_ManagerBase *pManager) : BI_BaseGroup(pManager)
{
}

BI_SeaGroup::~BI_SeaGroup()
{
}

void BI_SeaGroup::Init()
{
    BI_BaseGroup::Init();

    auto *pARoot = Manager()->AttributesPointer;
    if (pARoot)
        pARoot = pARoot->GetAttributeClass("sea");
    if (!pARoot)
        return;

    ATTRIBUTES *pA;
    char texture[MAX_PATH];
    uint32_t color;
    FRECT uv;
    RECT pos;

    // back
    pA = pARoot->GetAttributeClass("back");
    if (pA)
    {
        BIUtils::ReadStringFromAttr(pA, "texture", texture, sizeof(texture), "");
        color = pA->GetAttributeAsDword("color");
        FULLRECT(uv);
        BIUtils::ReadRectFromAttr(pA, "uv", uv, uv);
        pos = {};
        BIUtils::ReadRectFromAttr(pA, "pos", pos, pos);

        auto *const pNod = Manager()->CreateImageNode(texture, uv, pos, color, BIImagePrioritet_Group_Beg);
        if (pNod)
            m_aNodes.push_back(pNod);
        // Manager()->GetImageRender()->CreateImage( BIType_square, texture, color, uv, pos, BIImagePrioritet_Group_Beg
        // );
    }
}
