#include "Timer.h"
#include "../Utils.h"
#include "../image/imgrender.h"
#include "core.h"
#include "message.h"

BITimer::BITimer()
{
    SetBeginData();
}

BITimer::~BITimer()
{
    Release();
}

bool BITimer::Init()
{
    if (!ReadAndCreate())
        return false;
    return true;
}

void BITimer::Realize(uint32_t delta_time)
{
    if (m_fCurTimerCounter > 0.f && m_fMaxTimerCounter > 0.f)
    {
        m_fCurTimerCounter -= delta_time * 0.001f;
        if (m_fCurTimerCounter > 0.f)
        {
            if (m_pForeImage)
                m_pForeImage->CutSide(0.f, 1.f - m_fCurTimerCounter / m_fMaxTimerCounter, 0.f, 0.f);
            if (m_pImgRndr)
            {
                m_pRender->MakePostProcess();
                m_pImgRndr->Render();
            }
        }
        else
            CloseTimer(true);
    }
}

uint64_t BITimer::ProcessMessage(MESSAGE &message)
{
    const auto nMsgCod = message.Long();
    switch (nMsgCod)
    {
    case 0: {
        const auto fTime = message.Float();
        const std::string &eventName = message.String();
        OpenTimer(fTime, eventName.c_str());
    }
    break;
    case 1:
        CloseTimer(false);
        break;
    }
    return 0;
}

bool BITimer::ReadAndCreate()
{
    if (!m_pImgRndr)
        return false;

    // read position
    RECT rBackPos, rForePos;
    rBackPos.left = rBackPos.top = rBackPos.right = rBackPos.bottom = 0;
    rForePos.left = rForePos.top = rForePos.right = rForePos.bottom = 0;
    BIUtils::ReadRectFromAttr(AttributesPointer, "timerpos", rBackPos, rBackPos);
    BIUtils::ReadRectFromAttr(AttributesPointer, "timeroffset", rForePos, rForePos);
    rForePos.left = rBackPos.left + rForePos.left;
    rForePos.top = rBackPos.top + rForePos.top;
    rForePos.right = rBackPos.right - rForePos.right;
    rForePos.bottom = rBackPos.bottom - rForePos.bottom;

    // read uv
    FRECT rBackUV, rForeUV;
    rBackUV.left = rBackUV.top = rForeUV.left = rForeUV.top = 0.f;
    rBackUV.right = rBackUV.bottom = rForeUV.right = rForeUV.bottom = 1.f;
    BIUtils::ReadRectFromAttr(AttributesPointer, "timerbackuv", rBackUV, rBackUV);
    BIUtils::ReadRectFromAttr(AttributesPointer, "timerforeuv", rForeUV, rForeUV);

    // read texture & color
    auto *pcBackTexture = AttributesPointer ? AttributesPointer->GetAttribute("timerbacktexture") : nullptr;
    auto *pcForeTexture = AttributesPointer ? AttributesPointer->GetAttribute("timerforetexture") : nullptr;
    auto dwColorBack =
        AttributesPointer ? AttributesPointer->GetAttributeAsDword("timerbackcolor", 0xFFFFFFFF) : 0xFFFFFFFF;
    auto dwColorFore =
        AttributesPointer ? AttributesPointer->GetAttributeAsDword("timerforecolor", 0xFFFFFFFF) : 0xFFFFFFFF;

    // create
    if (pcBackTexture)
        m_pBackImage = m_pImgRndr->CreateImage(BIType_square, pcBackTexture, dwColorBack, rBackUV, rBackPos);
    if (pcForeTexture)
        m_pForeImage = m_pImgRndr->CreateImage(BIType_square, pcForeTexture, dwColorFore, rForeUV, rForePos);
    return true;
}

void BITimer::SetBeginData()
{
    m_fCurTimerCounter = 0.f;
    m_fMaxTimerCounter = 0.f;

    m_pRender = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    Assert(m_pRender);
    m_pImgRndr = new BIImageRender(m_pRender);
    m_pBackImage = nullptr;
    m_pForeImage = nullptr;
}

void BITimer::Release()
{
    STORM_DELETE(m_pBackImage);
    STORM_DELETE(m_pForeImage);
    STORM_DELETE(m_pImgRndr);
}

void BITimer::OpenTimer(float fTime, const char *pcEventName)
{
    m_fCurTimerCounter = m_fMaxTimerCounter = fTime;
    m_sEventName = pcEventName;
}

void BITimer::CloseTimer(bool bTimeOut)
{
    m_fCurTimerCounter = 0.f;
    m_pForeImage->CutSide(0.f, 0.f, 0.f, 0.f);
    if (bTimeOut)
    {
        core.Event((char *)m_sEventName.c_str(), nullptr);
    }
}
