#include "InterfaceManager.h"
#include "../image/imgrender.h"
#include "MousePointer.h"
#include "interfacenode/AllNodes.h"

#include "interfacegroup/SeaGroup.h"

#include "core.h"
#include "shared/bimanager/messages.h"
#include "vmodule_api.h"

BI_InterfaceManager::BI_InterfaceManager()
{
    m_pRS = nullptr;
    m_pImgRender = nullptr;
    m_pMouse = nullptr;
    m_pInterfaceSheet = nullptr;
}

BI_InterfaceManager::~BI_InterfaceManager()
{
    STORM_DELETE(m_pInterfaceSheet);
    // m_aNodes.DelAllWithPointers();
    for (const auto &node : m_aNodes)
        delete node;
    STORM_DELETE(m_pMouse);
    STORM_DELETE(m_pImgRender);
}

bool BI_InterfaceManager::Init()
{
    m_pRS = static_cast<VDX9RENDER *>(core.CreateService("DX9RENDER"));
    Assert(m_pRS);
    m_pImgRender = new BIImageRender(m_pRS);
    Assert(m_pImgRender);
    m_pMouse = new MousePointer(this, AttributesPointer);
    Assert(m_pMouse);

    auto [nBaseWidth, nBaseHeight] = core.GetScreenSize();

    long nBaseXOffset = 0;
    long nBaseYOffset = 0;
    if (AttributesPointer)
    {
        auto *pA = AttributesPointer->GetAttributeClass("BaseWindow");
        if (pA)
        {
            nBaseWidth = pA->GetAttributeAsDword("width", nBaseWidth);
            nBaseHeight = pA->GetAttributeAsDword("height", nBaseHeight);
            nBaseXOffset = pA->GetAttributeAsDword("xoffset", nBaseXOffset);
            nBaseYOffset = pA->GetAttributeAsDword("yoffset", nBaseYOffset);
        }
    }
    m_pImgRender->SetBaseScreenSize(nBaseWidth, nBaseHeight, nBaseXOffset, nBaseYOffset);

    return true;
}

void BI_InterfaceManager::Execute(uint32_t delta_time)
{
}

void BI_InterfaceManager::Realize(uint32_t delta_time)
{
    if (m_pInterfaceSheet)
        m_pInterfaceSheet->Update();

    for (long n = 0; n < m_aNodes.size(); n++)
        m_aNodes[n]->Update();

    m_pMouse->Update();

    m_pImgRender->Render();
}

uint64_t BI_InterfaceManager::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_BIMANAGER_DELETE_SHEET:
        STORM_DELETE(m_pInterfaceSheet);
        break;

    case MSG_BIMANAGER_LOAD_SHEET:
        return MsgLoadSheet(message);
        break;

    case MSG_BIMANAGER_EVENT:
        return MsgEvent(message);
        break;

    case MSG_BIMANAGER_CREATE_IMAGE:
        return MsgCreateImage(message);
        break;

    case MSG_BIMANAGER_DELETE_IMAGE:
        return MsgDeleteNode(message);
        break;

    case MSG_BIMANAGER_CREATE_STRING:
        return MsgCreateString(message);
        break;

    case MSG_BIMANAGER_DELETE_STRING:
        return MsgDeleteNode(message);
        break;
    }
    return 0;
}

BI_ManagerNodeBase *BI_InterfaceManager::CreateImageNode(const char *texture, const FRECT &uv, const RECT &pos,
                                                         uint32_t color, long nPrioritet)
{
    BI_ManagerNodeBase *pNod = new BI_ImageNode(this, texture, uv, pos, color, nPrioritet);
    return pNod;
}

BI_ManagerNodeBase *BI_InterfaceManager::CreateStringNode(const char *text, const char *font, uint32_t color,
                                                          float scale, const RECT &pos, long nHAlign, long nVAlign,
                                                          long prioritet)
{
    BI_ManagerNodeBase *pNod = new BI_StringNode(this, text, font, color, scale, pos, nHAlign, nVAlign, prioritet);
    return pNod;
}

void BI_InterfaceManager::DeleteNode(BI_ManagerNodeBase *pNod)
{
    const auto it = std::find(m_aNodes.begin(), m_aNodes.end(), pNod);
    if (it != m_aNodes.end())
        m_aNodes.erase(it);

    // long n = m_aNodes.Find( pNod );
    // if( n<0 ) return;
    // m_aNodes.DelIndex( n );
}

long BI_InterfaceManager::MsgLoadSheet(MESSAGE &message)
{
    // remove the old interface
    STORM_DELETE(m_pInterfaceSheet);

    const std::string &param = message.String();
    if (_stricmp(param.c_str(), "sea") == 0)
    {
        // loading sea interface
        m_pInterfaceSheet = new BI_SeaGroup(this);
        if (m_pInterfaceSheet)
        {
            m_pInterfaceSheet->Init();
        }
    }
    else if (_stricmp(param.c_str(), "land") == 0)
    {
        // loading the land interface
    }
    return 0;
}

long BI_InterfaceManager::MsgCreateImage(MESSAGE &message)
{
    /*char texture[MAX_PATH];    message.String( sizeof(texture), texture );
    FRECT uv;
    RECT pos;
    uint32_t color;
    long nPrioritet;

    return (long)CreateImageNode(texture,uv,pos,color,nPrioritet);*/
    return 0;
}

long BI_InterfaceManager::MsgCreateString(MESSAGE &message)
{
    // return (long)CreateStringNode();
    return 0;
}

long BI_InterfaceManager::MsgDeleteNode(MESSAGE &message)
{
    auto *pNod = (BI_ManagerNodeBase *)message.Pointer();
    if (!pNod)
        return 0;

    // if( m_aNodes.Find(pNod) != INVALID_ARRAY_INDEX ) {
    //    STORM_DELETE(pNod);
    //}
    //~!~ DeleteNode?
    const auto it = std::find(m_aNodes.begin(), m_aNodes.end(), pNod);
    if (it != m_aNodes.end())
        STORM_DELETE(*it);

    return 0;
}

long BI_InterfaceManager::MsgEvent(MESSAGE &message)
{
    return 0;
}
