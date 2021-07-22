#include "shipsign.h"
#include "../Utils.h"

WMShipIcon::WMShipIcon(entid_t BIEntityID, VDX9RENDER *pRS) : BISignIcon(BIEntityID, pRS)
{
}

WMShipIcon::~WMShipIcon()
{
}

void WMShipIcon::ExecuteCommand(CommandType command)
{
}

long WMShipIcon::CalculateSignQuantity()
{
    long n;
    char param[256];

    if (m_pAData)
    {
        for (n = 0; n < MAX_SIGN_QUANTITY; n++)
        {
            sprintf_s(param, sizeof(param), "sign%d", n + 1);
            auto *pA = m_pAData->GetAttributeClass(param);
            if (pA)
            {
                m_Sign[n].bUse = true;
                m_Sign[n].fLeftState = pA->GetAttributeAsFloat("leftprogress", 0.f);
                m_Sign[n].fRightState = pA->GetAttributeAsFloat("rightprogress", 0.f);
                m_Sign[n].fStarProgress = pA->GetAttributeAsFloat("starprogress", 0.f);
                FULLRECT(m_Sign[n].rFaceUV);
                BIUtils::ReadRectFromAttr(pA, "faceuv", m_Sign[n].rFaceUV, m_Sign[n].rFaceUV);
                const char *attr = pA->GetAttribute("text");
                if (attr != nullptr)
                    m_Sign[n].sText = attr;
            }
            else
            {
                m_Sign[n].bUse = false;
            }
        }
    }

    return BISignIcon::CalculateSignQuantity();
}

void WMShipIcon::UpdateChildrens()
{
}
