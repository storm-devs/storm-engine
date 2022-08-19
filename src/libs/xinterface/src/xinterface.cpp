#include "options_parser.hpp"

#include <SDL2/SDL.h>

#include "xinterface.h"
#include "back_scene/back_scene.h"
#include "help_chooser/help_chooser.h"
#include "info_handler.h"
#include "string_compare.hpp"
#include "nodes/all_xi_node.h"
#include "string_service/obj_str_service.h"
#include "string_service/str_service.h"
#include "xservice.h"
#include <cstdio>

#define CHECK_FILE_NAME "PiratesReadme.txt"

#define TT_TITLE_OFFSET 4
#define TT_TITLE_HEIGHT 32
#define TT_TITLE_STRING_OFFSET 2
#define TT_PIC_UP_OFFSET 8
#define TT_PIC_DOWN_OFFSET 8
#define TT_TEX1_UP_OFFSET 8
#define TT_TEX1_DOWN_OFFSET 8
#define TT_TEX2RECT_UP_OFFSET 4
#define TT_TEX2RECT_DOWN_OFFSET 4
#define TT_TEX2_UP_OFFSET 8
#define TT_TEX2_DOWN_OFFSET 8
#define TT_TEX3_UP_OFFSET 8
#define TT_TEX3_DOWN_OFFSET 8
#define TT_FRAME_DOWN_OFFSET 8
#define TT_TEX4_UP_OFFSET 8
#define TT_TEX4_DOWN_OFFSET 8
#define TT_TEX4RECT_UP_OFFSET 4
#define TT_TEX4RECT_DOWN_OFFSET 4

#define DEFAULT_IMODE 0
#define CONTEXTHELP_IMODE 1

#define MIN_SCALE 0.1f
#define MAX_SCALE 3.0f

static bool g_bIExclusiveMode = false;
static bool DiskCheck = false;

char *XI_ParseStr(char *inStr, char *buf, size_t bufSize, char devChar = ',')
{
    if (bufSize <= 0 || buf == nullptr)
        return inStr;
    if (inStr == nullptr)
    {
        buf[0] = 0;
        return nullptr;
    }
    auto curSize = 0;
    char *curStr;
    for (curStr = inStr; *curStr != 0; curStr++)
    {
        if (*curStr == ' ' && curSize == 0)
            continue;
        if (*curStr == devChar || *curStr == 0)
            break;
        if (curSize < static_cast<int>(bufSize) - 1)
            buf[curSize++] = *curStr;
    }
    buf[curSize] = 0;
    if (*curStr == devChar)
        curStr++;
    return curStr;
}

bool CheckPCcd();

XINTERFACE *XINTERFACE::pThis = nullptr;

XINTERFACE::XINTERFACE()
{
    pThis = this;

    pQuestService = nullptr;
    pPictureService = nullptr;
    pStringService = nullptr;
    bActive = true;

    pRenderService = nullptr;
    m_pNodes = nullptr;
    m_pCurNode = nullptr;
    m_pContHelp = nullptr;
    m_pMonocracyNode = nullptr;

    oldKeyState.dwKeyCode = -1;
    m_bMouseClick = false;
    m_bDblMouseClick = false;

    m_nStringQuantity = 0;
    m_imgLists = nullptr;

    m_bShowPrevTexture = false;
    m_pTexture = nullptr;
    m_pPrevTexture = nullptr;

    m_pEvents = nullptr;

    m_nColumnQuantity = 15;
    m_fWaveAmplitude = 0.1f;
    m_fWavePhase = 0.4f;
    m_fWaveSpeed = 0.2f;
    m_nBlendStepMax = 0x100;
    m_nBlendSpeed = 4;

    m_bNotFirstPress = false;
    m_nPressDelay = 0;
    m_nMaxPressDelay = 300;

    m_nMouseLastClickTimeCur = 0;
    m_nMouseLastClickTimeMax = 300;

    m_bUse = false;
    m_bShowMouse = false;

    m_fBlindFactor = 0.f;
    m_bBlindUp = true;
    bDisableControl = true;

    m_dwCurDeltaTime = 0;
    m_dwGameTimeSec = 0;
    m_dwGameTimeMin = 0;
    m_dwGameTimeHour = 0;

    m_nInterfaceMode = DEFAULT_IMODE;
    m_idHelpTexture = -1;
    m_dwContHelpColor = ARGB(255, 128, 128, 128);
    m_frectDefHelpTextureUV = FXYRECT(0.0, 0.0, 1.0, 1.0);
    m_strDefHelpTextureFile = nullptr;
    DiskCheck = true; // false;

    m_pMouseWeel = nullptr;

    m_pSaveFindRoot = nullptr;

    m_pEditor = nullptr;

    m_pCurToolTipNode = nullptr;
    m_pMouseNode = nullptr;
}

XINTERFACE::~XINTERFACE()
{
    ReleaseOld();

    if (m_pTexture)
        pRenderService->Release(m_pTexture);
    if (m_pPrevTexture)
        pRenderService->Release(m_pPrevTexture);
    m_pTexture = m_pPrevTexture = nullptr;

    if (pPictureService != nullptr)
    {
        pPictureService->ReleaseAll();
        delete pPictureService;
        pPictureService = nullptr;
    }

    STORM_DELETE(pQuestService);
    STORM_DELETE(m_pEditor);

    ReleaseSaveFindList();
}

bool XINTERFACE::Init()
{
    // GUARD(XINTERFACE::Init())
    SetDevice();
    // UNGUARD
    return true;
}

void XINTERFACE::SetDevice()
{
    // GUARD(XINTERFACE::SetDevice())

    g_idInterface = GetId();
    m_UtilContainer.Init();

    // get render service
    pRenderService = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (!pRenderService)
    {
        throw std::runtime_error("No service: dx9render");
    }

    pStringService = static_cast<VSTRSERVICE *>(core.GetService("STRSERVICE"));
    if (!pStringService)
    {
        throw std::runtime_error("No service: strservice");
    }

    // Load common parameters
    LoadIni();

    // Create pictures and string lists service
    pPictureService = new XSERVICE;
    if (pPictureService == nullptr)
    {
        throw std::runtime_error("Not memory allocate");
    }
    pPictureService->Init(pRenderService, dwScreenWidth, dwScreenHeight);

    pQuestService = new storm::QuestFileReader;
    if (pQuestService == nullptr)
    {
        throw std::runtime_error("Not memory allocate");
    }
    auto *pvd = core.Event("GetQuestTextFileName", "");
    if (pvd != nullptr)
    {
        const int nq = pvd->GetElementsNum();
        const char *pstr;
        for (auto i = 0; i < nq; i++)
        {
            pvd->Get(pstr, i);
            pQuestService->SetQuestTextFileName(pstr);
        }
    }

    core.SetLayerType(INTERFACE_EXECUTE, layer_type_t::execute);
    core.SetLayerType(INTERFACE_REALIZE, layer_type_t::realize);
    // core.SystemMessages(GetId(),true);

    if (AttributesPointer)
    {
        auto *pA = AttributesPointer->GetAttributeClass("GameTime");
        if (pA)
        {
            m_dwGameTimeSec = pA->GetAttributeAsDword("sec", 0);
            m_dwGameTimeMin = pA->GetAttributeAsDword("min", 0);
            m_dwGameTimeHour = pA->GetAttributeAsDword("hour", 0);
        }
    }

    m_pMouseWeel = core.Event("evGetMouseWeel");

    m_pEditor = new GIEditor(this);

    // UNGUARD
}

bool XINTERFACE::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool XINTERFACE::LoadState(ENTITY_STATE *state)
{
    return true;
}

void XINTERFACE::Execute(uint32_t)
{
    m_UtilContainer.FrameUpdate();

    auto Delta_Time = core.GetRDeltaTime();
    if (!bActive)
        return;
    IncrementGameTime(Delta_Time);
    if (!m_bUse)
        return;

    // do blind factor
    if (m_bBlindUp)
        m_fBlindFactor += m_fBlindSpeed * Delta_Time;
    else
        m_fBlindFactor -= m_fBlindSpeed * Delta_Time;
    if (m_fBlindFactor > 1.f)
    {
        m_fBlindFactor = 1.f;
        m_bBlindUp = false;
    }
    if (m_fBlindFactor < 0.f)
    {
        m_fBlindFactor = 0.f;
        m_bBlindUp = true;
    }

    while (m_pEvents != nullptr)
    {
        core.Event(m_pEvents->sEventName, "ls", m_pEvents->nCommandIndex, m_pEvents->sNodeName);
        if (m_pEvents != nullptr)
        {
            auto *const pE = m_pEvents;
            m_pEvents = m_pEvents->next;
            delete pE;
        }
    }
    if (!m_bUse)
        return;

    if (m_nPressDelay > 0)
    {
        m_nPressDelay -= Delta_Time;
        if (m_nPressDelay < 0)
            m_nPressDelay = 0;
    }

    if (m_nMouseLastClickTimeCur > 0)
    {
        m_nMouseLastClickTimeCur -= Delta_Time;
        if (m_nMouseLastClickTimeCur < 0)
            m_nMouseLastClickTimeCur = 0;
    }

    DoControl();

    auto *pNod = m_pNodes;
    while (pNod != nullptr)
    {
        pNod->FrameProcess(Delta_Time);
        pNod = pNod->m_next;
    }

    if (m_pMouseWeel)
        m_pMouseWeel->Set(0);
}

void XINTERFACE::Realize(uint32_t)
{
    if (!m_bUse || !bActive)
        return;

    pRenderService->MakePostProcess();

    auto Delta_Time = core.GetRDeltaTime();

    CMatrix moldv, moldp, moldw;

    uint32_t dwFogFlag;
    pRenderService->GetRenderState(D3DRS_FOGENABLE, &dwFogFlag);
    if (pRenderService->TechniqueExecuteStart("iStartTechnique"))
        while (pRenderService->TechniqueExecuteNext())
            ;

    // Get old transformation
    pRenderService->GetTransform(D3DTS_VIEW, moldv);
    pRenderService->GetTransform(D3DTS_PROJECTION, moldp);
    // Set new transformation
    pRenderService->SetTransform(D3DTS_WORLD, matw);
    pRenderService->SetTransform(D3DTS_VIEW, matv);
    pRenderService->SetTransform(D3DTS_PROJECTION, matp);

    DrawNode(m_pNodes, Delta_Time, 0, 80);

    // Do mouse move
    auto *pOldNode = m_pCurNode;
    MouseMove();
    if (pOldNode != m_pCurNode)
    {
        core.Event(ISOUND_EVENT, "l", 2); // choosing a new node
    }

    // show dinamic pictures
    XI_ONLYONETEX_VERTEX pV[4];
    for (auto i = 0; i < 4; i++)
        pV[i].pos.z = 1.f;
    auto *pImg = m_imgLists;
    uint32_t oldTFactor;
    pRenderService->GetRenderState(D3DRS_TEXTUREFACTOR, &oldTFactor);
    while (pImg != nullptr)
    {
        if (pImg->idTexture != -1 && pImg->imageID != -1)
        {
            pRenderService->TextureSet(0, pImg->idTexture);
            FXYRECT frect;
            pPictureService->GetTexturePos(pImg->imageID, frect);
            pV[0].pos.x = pV[2].pos.x = static_cast<float>(pImg->position.left);
            pV[0].tu = pV[2].tu = frect.left;
            pV[1].pos.x = pV[3].pos.x = static_cast<float>(pImg->position.right);
            pV[1].tu = pV[3].tu = frect.right;
            pV[0].pos.y = pV[1].pos.y = static_cast<float>(pImg->position.top);
            pV[0].tv = pV[1].tv = frect.top;
            pV[2].pos.y = pV[3].pos.y = static_cast<float>(pImg->position.bottom);
            pV[2].tv = pV[3].tv = frect.bottom;
            if (pImg->doBlind)
            {
                pRenderService->SetRenderState(D3DRS_TEXTUREFACTOR,
                                               GetBlendColor(pImg->argbBlindMin, pImg->argbBlindMax, m_fBlindFactor));
                pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONLYONETEX_FVF, 2, pV,
                                                sizeof(XI_ONLYONETEX_VERTEX), "iBlindPictures");
            }
            else
            {
                if (pImg->sTechniqueName == nullptr)
                    pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONLYONETEX_FVF, 2, pV,
                                                    sizeof(XI_ONLYONETEX_VERTEX), "iDinamicPictures");
                else
                    pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONLYONETEX_FVF, 2, pV,
                                                    sizeof(XI_ONLYONETEX_VERTEX), pImg->sTechniqueName);
            }
        }
        pImg = pImg->next;
    }
    pRenderService->SetRenderState(D3DRS_TEXTUREFACTOR, oldTFactor);

    DrawNode(m_pNodes, Delta_Time, 81, 90);

    // Show dinamic stringes
    if (m_nStringQuantity > 0)
    {
        auto *tmpAttr = core.Entity_GetAttributeClass(g_idInterface, "strings");

        if (tmpAttr != nullptr)
            for (auto i = 0; i < m_nStringQuantity; i++)
                if (m_stringes[i].bUsed)
                {
                    pRenderService->ExtPrint(m_stringes[i].fontNum, m_stringes[i].dwColor, 0, m_stringes[i].eAlignment,
                                             true, m_stringes[i].fScale, dwScreenWidth, dwScreenHeight, m_stringes[i].x,
                                             m_stringes[i].y, "%s", static_cast<const char*>(tmpAttr->GetAttribute(m_stringes[i].sStringName)));
                }
    }

    DrawNode(m_pNodes, Delta_Time, 91, 65536);

    if (m_pCurToolTipNode)
        m_pCurToolTipNode->ShowToolTip();

    if (m_pEditor)
        if (m_pEditor->IsShowMode())
            m_pEditor->Render();
        else
            m_pEditor->DrawSizeBox();

    // Mouse pointer show
    if (m_bShowMouse)
    {
        pRenderService->TextureSet(0, m_idTex);
        pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONLYONETEX_FVF, 2, vMouse, sizeof(XI_ONLYONETEX_VERTEX),
                                        "iMouseCurShow");
    }

    // Show context help data
    ShowContextHelp();

    if (pRenderService->TechniqueExecuteStart("iExitTechnique"))
        while (pRenderService->TechniqueExecuteNext())
            ;
    pRenderService->SetRenderState(D3DRS_FOGENABLE, dwFogFlag);

    // Restore old transformation
    pRenderService->SetTransform(D3DTS_VIEW, moldv);
    pRenderService->SetTransform(D3DTS_PROJECTION, moldp);
}

int32_t oldCurNum = -1L;

uint64_t XINTERFACE::ProcessMessage(MESSAGE &message)
{
    auto cod = message.Long();

    switch (cod)
    {
    case MSG_INTERFACE_MSG_TO_NODE: {
        const std::string &param = message.String();
        if (m_pNodes != nullptr)
        {
            auto *pNode = m_pNodes->FindNode(param.c_str());
            if (pNode != nullptr)
            {
                auto msgCode = message.Long();
                if (msgCode == -1)
                {
                    msgCode = message.Long();
                    return pNode->CINODE::MessageProc(msgCode, message);
                }
                return pNode->MessageProc(msgCode, message);
            }
        }
    }
    break;
    case MSG_INTERFACE_SETHELPSTRING: {
        const std::string &param = message.String();
        if (m_pContHelp != nullptr)
            static_cast<CXI_CONTEXTHELP *>(m_pContHelp)->SetTempHelp(param.c_str());
    }
    break;
    case MSG_INTERFACE_SET_CURRENT_NODE: {
        auto *pNewCurNode = (CINODE *)message.Pointer();
        if (pNewCurNode != nullptr)
            SetCurNode(pNewCurNode);
    }
    break;
    case MSG_INTERFACE_NEW_CURRENT_NODE: {
        const std::string &param = message.String();
        auto *pNewCurNode = (m_pNodes != nullptr ? m_pNodes->FindNode(param.c_str()) : nullptr);
        if (pNewCurNode != nullptr)
            SetCurNode(pNewCurNode);
    }
    break;

    case MSG_INTERFACE_SET_NODE_USING: {
        const std::string &param = message.String();
        int nUsingCode = message.Long();
        auto *pTmpNod = (m_pNodes != nullptr ? m_pNodes->FindNode(param.c_str()) : nullptr);
        if (pTmpNod != nullptr)
            pTmpNod->SetUsing(nUsingCode != 0);
    }
    break;

    case MSG_INTERFACE_SCROLL_CHANGE: {
        const std::string &sNodeName = message.String();
        auto nItemNum = message.Long();

        auto *pScrollNode = (m_pNodes != nullptr ? m_pNodes->FindNode(sNodeName.c_str()) : nullptr);
        if (pScrollNode != nullptr)
        {
            if (pScrollNode->m_nNodeType == NODETYPE_SCROLLIMAGE)
                static_cast<CXI_SCROLLIMAGE *>(pScrollNode)->ChangeScroll(nItemNum);
            if (pScrollNode->m_nNodeType == NODETYPE_VIMGSCROLL)
                static_cast<CXI_VIMAGESCROLL *>(pScrollNode)->ChangeScroll(nItemNum);
        }
    }
    break;

    case MSG_INTERFACE_FOURIMAGE_CHANGE: {
        const std::string &sNodeName = message.String();
        auto nItemNum = message.Long();

        CINODE *pNode = (m_pNodes != nullptr ? m_pNodes->FindNode(sNodeName.c_str()) : nullptr);
        if (pNode != nullptr && pNode->m_nNodeType == NODETYPE_FOURIMAGE)
        {
            static_cast<CXI_FOURIMAGE *>(pNode)->ChangeItem(nItemNum);
        }
    }
    break;

    case MSG_INTERFACE_INIT: {
        const std::string &param = message.String();
        ReleaseOld();
        LoadDialog(param.c_str());
        m_bUse = true;
        bDisableControl = true;
    }
    break;

    case MSG_INTERFACE_ENABLE_STRING: {
        const std::string &param = message.String();
        for (int i = 0; i < m_nStringQuantity; i++)
            if (m_stringes[i].sStringName != nullptr && storm::iEquals(param, m_stringes[i].sStringName))
            {
                m_stringes[i].bUsed = true;
                break;
            }
    }
    break;

    case MSG_INTERFACE_DISABLE_STRING: {
        const std::string &param = message.String();
        for (int i = 0; i < m_nStringQuantity; i++)
            if (m_stringes[i].sStringName != nullptr && storm::iEquals(param, m_stringes[i].sStringName))
            {
                m_stringes[i].bUsed = false;
                break;
            }
    }
    break;

    case MSG_INTERFACE_RELEASE: {
        ReleaseOld();
        m_bUse = false;
        bDisableControl = true;
    }
    break;

    case MSG_INTERFACE_SET_EVENT: {
        const std::string &param = message.String();
        const std::string &nodeName = message.String();
        int32_t nCommand = message.Long();
        //
        auto *pEvent = new EVENT_Entity;
        if (pEvent == nullptr)
            throw std::runtime_error("allocate memory error");
        pEvent->next = m_pEvents;
        m_pEvents = pEvent;
        //
        auto len = param.size() + 1;
        pEvent->sEventName = new char[len];
        if (pEvent->sEventName == nullptr)
            throw std::runtime_error("allocate memory error");
        memcpy(pEvent->sEventName, param.c_str(), len);
        //
        len = nodeName.size() + 1;
        pEvent->sNodeName = new char[len];
        if (pEvent->sNodeName == nullptr)
            throw std::runtime_error("allocate memory error");
        memcpy(pEvent->sNodeName, nodeName.c_str(), len);
        //
        if (nCommand >= 0 && nCommand < COMMAND_QUANTITY)
            pEvent->nCommandIndex = pCommandsList[nCommand].code;
        else
            pEvent->nCommandIndex = -1;
    }
    break;

    case MSG_INTERFACE_CREATE_STRING: {
        const std::string &param = message.String();

        int l;
        for (l = 0; l < m_nStringQuantity; l++)
        {
            if (m_stringes[l].sStringName != nullptr && storm::iEquals(m_stringes[l].sStringName, param))
                break;
        }
        if (l == m_nStringQuantity)
        {
            m_nStringQuantity++;
            m_stringes.resize(m_nStringQuantity);

            const auto len = param.size() + 1;
            m_stringes[l].sStringName = new char[len];
            if (m_stringes[l].sStringName == nullptr)
                throw std::runtime_error("allocate memory error");
            memcpy(m_stringes[l].sStringName, param.c_str(), len);
        }
        else
        {
            FONT_RELEASE(pRenderService, m_stringes[l].fontNum);
        }

        const std::string &param2 = message.String();
        m_stringes[l].fontNum = pRenderService->LoadFont(param2.c_str());
        m_stringes[l].dwColor = static_cast<uint32_t>(message.Long());

        m_stringes[l].bUsed = true;
        m_stringes[l].x = message.Long() + GlobalRect.left;
        m_stringes[l].y = message.Long() + GlobalRect.top;
        switch (message.Long())
        {
        case SCRIPT_ALIGN_RIGHT:
            m_stringes[l].eAlignment = PR_ALIGN_RIGHT;
            break;
        case SCRIPT_ALIGN_CENTER:
            m_stringes[l].eAlignment = PR_ALIGN_CENTER;
            break;
        case SCRIPT_ALIGN_LEFT:
            m_stringes[l].eAlignment = PR_ALIGN_LEFT;
            break;
        }
        m_stringes[l].fScale = message.Float();
    }
    break;
    case MSG_INTERFACE_DELETE_STRING: {
        const std::string &param = message.String();
        for (int i = 0; i < m_nStringQuantity; i++)
        {
            if (m_stringes[i].sStringName != nullptr && storm::iEquals(m_stringes[i].sStringName, param))
            {
                STORM_DELETE(m_stringes[i].sStringName);
                FONT_RELEASE(pRenderService, m_stringes[i].fontNum);
                m_nStringQuantity--;
                m_stringes[i].sStringName = m_stringes[m_nStringQuantity].sStringName;
                m_stringes[i].fontNum = m_stringes[m_nStringQuantity].fontNum;
                break;
            }
        }
    }
    break;
    case MSG_INTERFACE_CHANGE_STR_COLOR: {
        const std::string &param = message.String();
        for (int i = 0; i < m_nStringQuantity; i++)
        {
            if (m_stringes[i].sStringName != nullptr && storm::iEquals(m_stringes[i].sStringName, param))
            {
                m_stringes[i].dwColor = message.Long();
                break;
            }
        }
    }
    break;
    case MSG_INTERFACE_PLACE_IMAGE: {
        const std::string &param = message.String();
        // find image
        IMAGE_Entity *pImg = m_imgLists;
        while (pImg != nullptr)
        {
            if (pImg->sImageName != nullptr && storm::iEquals(pImg->sImageName, param))
                break;
            pImg = pImg->next;
        }
        bool bGlobRect = message.Long() != 0;
        // get image position
        if (pImg != nullptr)
        {
            pImg->position.left = message.Long();
            pImg->position.top = message.Long();
            pImg->position.right = message.Long();
            pImg->position.bottom = message.Long();

            if (bGlobRect)
            {
                pImg->position.left += GlobalRect.left;
                pImg->position.top += GlobalRect.top;
                pImg->position.right += GlobalRect.left;
                pImg->position.bottom += GlobalRect.top;
            }
        }
    }
    break;
    case MSG_INTERFACE_SET_SELECTABLE: // "lsl"
    {
        const std::string &param = message.String();
        bool bSelectable = message.Long() != 0;
        if (m_pNodes == nullptr)
            break;
        CINODE *pNod = m_pNodes->FindNode(param.c_str());
        if (pNod != nullptr)
            pNod->m_bSelected = bSelectable;
    }
    break;
    case MSG_INTERFACE_GET_SELECTABLE: // ls
    {
        const std::string &param = message.String();
        if (m_pNodes == nullptr)
            break;
        CINODE *pNod = m_pNodes->FindNode(param.c_str());
        if (pNod != nullptr)
            return (pNod->m_bSelected && pNod->m_bUse);
    }
    break;
    case MSG_INTERFACE_DEL_SCROLLIMAGE: {
        const std::string &param = message.String();
        int32_t imgNum = message.Long();
        if (m_pNodes == nullptr)
            break;
        CINODE *pNod = m_pNodes->FindNode(param.c_str());
        if (pNod != nullptr)
        {
            if (pNod->m_nNodeType == NODETYPE_SCROLLIMAGE)
                static_cast<CXI_SCROLLIMAGE *>(pNod)->DeleteImage(imgNum);
            if (pNod->m_nNodeType == NODETYPE_VIMGSCROLL)
                static_cast<CXI_VIMAGESCROLL *>(pNod)->DeleteImage(imgNum);
        }
    }
    break;
    case MSG_INTERFACE_GET_CURRENT_NODE: // "le"
    {
        if (m_pCurNode != nullptr && m_pCurNode->m_nodeName != nullptr)
            message.ScriptVariablePointer()->Set(m_pCurNode->m_nodeName);
        else
            message.ScriptVariablePointer()->Set("");
    }
    break;
    case MSG_INTERFACE_REFRESH_SCROLL: // "ls"
    {
        const std::string &param = message.String();
        if (m_pNodes == nullptr)
            break;
        CINODE *pNod = m_pNodes->FindNode(param.c_str());
        if (pNod != nullptr)
        {
            if (pNod->m_nNodeType == NODETYPE_SCROLLIMAGE)
                static_cast<CXI_SCROLLIMAGE *>(pNod)->RefreshScroll();
            if (pNod->m_nNodeType == NODETYPE_VIMGSCROLL)
                static_cast<CXI_VIMAGESCROLL *>(pNod)->RefreshScroll();
        }
    }
    break;
    case MSG_INTERFACE_GET_LANGUAGE_STRING: {
        const char *lngStr;
        message.ScriptVariablePointer()->Get(lngStr);
        message.ScriptVariablePointer()->Set(pStringService->GetString(lngStr));
    }
    break;
    case MSG_INTERFACE_SET_TITLE_STRINGS: {
        const std::string &param = message.String();
        ATTRIBUTES *pA = message.AttributePointer();
        int tn = message.Long();
        if (m_pNodes == nullptr)
            break;
        CINODE *pNod = m_pNodes->FindNode(param.c_str());
        if (pNod != nullptr && pNod->m_nNodeType == NODETYPE_QTITLE)
            static_cast<CXI_QUESTTITLE *>(pNod)->SetNewTopQuest(pA, tn);
    }
    break;
    case MSG_INTERFACE_CHECK_QRECORD: {
        ATTRIBUTES *pA = message.AttributePointer();
        if (pA == nullptr)
            break;
        const std::string &param = message.String();
        const char *pText = pA->GetAttribute("Text");
        if (pText == nullptr)
            break;
        char subText[256];
        const char *pCur = pText;
        while (true)
        {
            subText[0] = 0;
            sscanf(pCur, "%[^,]", subText);
            int subSize = strlen(subText);
            if (storm::iEquals(subText, param))
                return 1;
            pCur += subSize;
            if (*pCur == ',')
                pCur++;
            if (subSize == 0 || *pCur == 0)
                break;
        }
    }
    break;
    case MSG_INTERFACE_INIT_QTEXT_SHOW: {
        const std::string &param = message.String();
        ATTRIBUTES *pA = message.AttributePointer();
        int qn = message.Long();
        if (m_pNodes == nullptr)
            break;
        CINODE *pNod = m_pNodes->FindNode(param.c_str());
        if (pNod != nullptr && pNod->m_nNodeType == NODETYPE_QTEXTS)
            static_cast<CXI_QUESTTEXTS *>(pNod)->StartQuestShow(pA, qn);
    }
    break;
    case MSG_INTERFACE_SET_SCROLLER: {
        const std::string &param = message.String();
        float spos = message.Float();
        if (m_pNodes == nullptr)
            break;
        CINODE *pNod = m_pNodes->FindNode(param.c_str());
        if (pNod != nullptr && pNod->m_nNodeType == NODETYPE_SCROLLER)
            static_cast<CXI_SCROLLER *>(pNod)->SetRollerPos(spos);
    }
    break;

    case MSG_INTERFACE_DO_SAVE_DATA: {
        const std::string &param1 = message.String();
        const std::string &param2 = message.String();
        SFLB_DoSaveFileData(param1.c_str(), param2.c_str());
    }
    break;
    case MSG_INTERFACE_GET_SAVE_DATA: {
        const std::string &param1 = message.String();
        char param2[256];
        param2[0] = 0;
        if (SFLB_GetSaveFileData(param1.c_str(), sizeof(param2), param2) && strlen(param2) > 0)
        {
            message.ScriptVariablePointer()->Set(param2);
        }
        else
        {
            message.ScriptVariablePointer()->Set("");
        }
    }
    break;
    case MSG_INTERFACE_SAVE_FILE_FIND: {
        char param[256];
        int32_t saveNum = message.Long();
        int32_t nSaveFileSize;
        char *sSaveName = SaveFileFind(saveNum, param, sizeof(param), nSaveFileSize);
        if (sSaveName != nullptr)
        {
            message.ScriptVariablePointer()->Set(sSaveName);
            message.ScriptVariablePointer()->Set(nSaveFileSize);
            return 1;
        }
    }
    break;
    case MSG_INTERFACE_NEW_SAVE_FILE_NAME: {
        const std::string &param = message.String();
        if (NewSaveFileName(param.c_str()))
            return 0;
        return 1;
    }
    break;
    case MSG_INTERFACE_DELETE_SAVE_FILE: {
        const std::string &param = message.String();
        DeleteSaveFile(param.c_str());
    }
    break;
    case MSG_INTERFACE_SET_FORMATEDTEXT: {
        const std::string &param = message.String();
        CINODE *pNod = m_pNodes ? m_pNodes->FindNode(param.c_str()) : nullptr;
        const std::string &param2 = message.String();
        if (pNod && pNod->m_nNodeType == NODETYPE_FORMATEDTEXTS && !pNod->m_bInProcessingMessageForThisNode)
        {
            pNod->m_bInProcessingMessageForThisNode = true;
            static_cast<CXI_FORMATEDTEXT *>(pNod)->SetFormatedText(param2.c_str());
            pNod->m_bInProcessingMessageForThisNode = false;
        }
    }
    break;
    case MSG_INTERFACE_SET_BLIND: {
        const std::string &param = message.String();
        // find image
        IMAGE_Entity *pImg = m_imgLists;
        while (pImg != nullptr)
        {
            if (pImg->sImageName != nullptr && storm::iEquals(pImg->sImageName, param))
                break;
            pImg = pImg->next;
        }
        // get image position
        if (pImg != nullptr)
        {
            pImg->doBlind = message.Long() != 0;
            pImg->argbBlindMin = message.Long();
            pImg->argbBlindMax = message.Long();
        }
    }
    break;
    case MSG_INTERFACE_GET_STRWIDTH: {
        const std::string &param = message.String();
        const std::string &param2 = message.String();
        float fScale = message.Float();
        int tmpFontID = pRenderService->LoadFont(param2.c_str());
        int retVal = pRenderService->StringWidth(param.c_str(), tmpFontID, fScale);
        pRenderService->UnloadFont(tmpFontID);
        return retVal;
    }
    break;
    case MSG_INTERFACE_CLICK_STATUS: {
        const std::string &param = message.String();
        if (m_pNodes == nullptr)
            break;
        CINODE *pNod = m_pNodes->FindNode(param.c_str());
        if (pNod != nullptr)
            return pNod->GetClickState();
    }
    break;
    case MSG_INTERFACE_LOCK_NODE: {
        if (!m_bUse)
            break;
        int32_t lockNode = message.Long();
        CINODE *pnod = nullptr;
        if (lockNode > 0)
        {
            const std::string &param = message.String();
            pnod = m_pNodes->FindNode(param.c_str());
        }
        switch (lockNode)
        {
        case 0:
            ExitFromExclusive();
            break;
        case 1:
            SetExclusiveNode(pnod);
            break;
        case 2:
            AddExclusiveNode(pnod);
            break;
        }
    }
    break;
    case MSG_INTERFACE_DELETE_PICTURE: {
        if (!m_bUse)
            break;
        const std::string &param = message.String();
        ReleaseDinamicPic(param.c_str());
    }
    break;
    case MSG_INTERFACE_GET_FREE_SPACE: {
        int32_t retVal = 0;
    }
    break;

    case MSG_INTERFACE_SAVEOPTIONS: {
        const std::string &param = message.String();
        ATTRIBUTES *pA = message.AttributePointer();
        SaveOptionsFile(param.c_str(), pA);
    }
    break;

    case MSG_INTERFACE_LOADOPTIONS: {
        const std::string &param = message.String();
        ATTRIBUTES *pA = message.AttributePointer();
        LoadOptionsFile(param, pA);
    }
    break;

    case MSG_INTERFACE_LAUNCH_DASHBOARD: {
    }
    break;

    case MSG_INTERFACE_FILENAME2DATASTR: {
        const std::string &param = message.String();
        VDATA *pvdat = message.ScriptVariablePointer();
        int i = 0;
        int nStrLen = param.size();
        if (pvdat)
            pvdat->Set(&param[i]);
    }
    break;

    case MSG_INTERFACE_GETTIME: {
        const std::string &param = message.String();
        std::time_t systTime;
        if (param[0] == 0)
        {
            systTime = std::time(nullptr);
        }
        else
        {
            if (!fio->_FileOrDirectoryExists(param.c_str()))
            {
                systTime = std::time(nullptr);
            }
            else
            {
                systTime = fio->_ToTimeT(fio->_GetLastWriteTime(param.c_str()));
            }
        }
        const auto locTime = std::localtime(&systTime);
        char param2[1024];
        std::strftime(param2, sizeof(param2), "%H:%M:%S", locTime);
        VDATA *pvdat = message.ScriptVariablePointer();
        if (pvdat)
        {
            pvdat->Set(param2);
        }

        if (message.GetFormat() == "lsee") 
        {
            std::strftime(param2, sizeof(param2), "%d.%m.%Y", locTime);
            pvdat = message.ScriptVariablePointer();
            if (pvdat)
            {
                pvdat->Set(param2);
            }
        }
    }
    break;

    case MSG_INTERFACE_CHECK_AGAIN:
        Assert(0);
        break;

    case MSG_INTERFACE_IS_READY_LOAD:
        return LoadIsExist();
        break;

    case MSG_INTERFACE_SET_TOOLTIP: {
        const char *pcHeader = message.StringPointer();
        const char *pcText1 = message.StringPointer();
        int32_t dwTextColor1 = message.Long();
        const char *pcText2 = message.StringPointer();
        int32_t dwTextColor2 = message.Long();
        const char *pcText3 = message.StringPointer();
        int32_t dwTextColor3 = message.Long();
        const char *pcText4 = message.StringPointer();
        int32_t dwTextColor4 = message.Long();
        const char *pcPicTextureName = message.StringPointer();
        const char *pcPicGroupName = message.StringPointer();
        const char *pcPicImageName = message.StringPointer();
        int32_t nPicWidth = message.Long();
        int32_t nPicHeight = message.Long();
        SetTooltip(pcHeader, pcText1, dwTextColor1, pcText2, dwTextColor2, pcText3, dwTextColor3, pcText4, dwTextColor4,
                   pcPicTextureName, pcPicGroupName, pcPicImageName, nPicWidth, nPicHeight);
    }
    break;
    }

    return 0;
}

static const char *RESOURCE_FILENAME = "resource\\ini\\interfaces\\interfaces.ini";

void XINTERFACE::LoadIni()
{
    // GUARD(XINTERFACE::LoadIni());
    char section[256];

    auto platform = "PC_SCREEN";
    auto ini = fio->OpenIniFile(RESOURCE_FILENAME);
    if (!ini)
        throw std::runtime_error("ini file not found!");

#ifdef _WIN32 // FIX_LINUX GetWindowRect
    RECT Screen_Rect;
    GetWindowRect(static_cast<HWND>(core.GetWindow()->OSHandle()), &Screen_Rect);
#else
    int sdlScreenWidth, sdlScreenHeight;
    SDL_GetWindowSize(reinterpret_cast<SDL_Window *>(core.GetWindow()->OSHandle()), &sdlScreenWidth, &sdlScreenHeight);
#endif

    fScale = 1.0f;
    const auto screenSize = core.GetScreenSize();
    dwScreenHeight = screenSize.height;
#ifdef _WIN32 // FIX_LINUX GetWindowRect
    dwScreenWidth = (Screen_Rect.right - Screen_Rect.left) * dwScreenHeight / (Screen_Rect.bottom - Screen_Rect.top);
#else
    dwScreenWidth = sdlScreenWidth * dwScreenHeight / sdlScreenHeight;
#endif
    if (dwScreenWidth < screenSize.width)
        dwScreenWidth = screenSize.width;
    GlobalScreenRect.top = 0;
    GlobalScreenRect.bottom = screenSize.height;
    GlobalScreenRect.left = (dwScreenWidth - screenSize.width) / 2;
    GlobalScreenRect.right = screenSize.width + GlobalScreenRect.left;

    sprintf_s(section, "COMMON");

    // set screen parameters
    if (ini->GetInt(platform, "bDynamicScaling", 0) == 0)
    {
        const auto &canvas_size = core.GetScreenSize();
        fScale = ini->GetFloat(platform, "fScale", 1.f);
        if (fScale < MIN_SCALE || fScale > MAX_SCALE)
            fScale = 1.f;
        dwScreenWidth = ini->GetInt(platform, "wScreenWidth", canvas_size.width);
        dwScreenHeight = ini->GetInt(platform, "wScreenHeight", canvas_size.height);
        GlobalScreenRect.left = ini->GetInt(platform, "wScreenLeft", 0);
        GlobalScreenRect.top = ini->GetInt(platform, "wScreenTop", canvas_size.height);
        GlobalScreenRect.right = ini->GetInt(platform, "wScreenRight", canvas_size.width);
        GlobalScreenRect.bottom = ini->GetInt(platform, "wScreenDown", 0);
    }

    m_fpMouseOutZoneOffset.x = ini->GetFloat(section, "mouseOutZoneWidth", 0.f);
    m_fpMouseOutZoneOffset.y = ini->GetFloat(section, "mouseOutZoneHeight", 0.f);
    m_nMouseLastClickTimeMax = ini->GetInt(section, "mouseDblClickInterval", 300);

    CMatrix oldmatp;
    pRenderService->GetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&oldmatp);
    GlobalRect.left = GlobalScreenRect.left;
    GlobalRect.top = GlobalScreenRect.top;
    GlobalRect.right = GlobalScreenRect.right;
    GlobalRect.bottom = GlobalScreenRect.bottom;

    matp.m[0][0] = 2.f / dwScreenWidth * fScale;
    matp.m[1][1] = -2.f / dwScreenHeight * fScale;
    matp.m[2][2] = 1.0003f;
    matp.m[2][3] = 1.f;
    matp.m[3][2] = 0.f;
    matp.m[3][3] = 0.f;
    matp.m[3][2] = -1.f;

    matv.m[3][0] = -(GlobalScreenRect.left + GlobalScreenRect.right) / 2.f;
    matv.m[3][1] = -(GlobalScreenRect.top + GlobalScreenRect.bottom) / 2.f;

    // set key press data
    m_nMaxPressDelay = ini->GetInt(section, "RepeatDelay", 500);

    // set mouse cursor
    char param[256];
    ini->ReadString(section, "MousePointer", param, sizeof(param) - 1, "");
    char param2[256];
    sscanf(param, "%[^,],%d,size:(%d,%d),pos:(%d,%d)", param2, &m_lMouseSensitive, &MouseSize.x, &MouseSize.y,
           &m_lXMouse, &m_lYMouse);
    m_idTex = pRenderService->TextureCreate(param2);
    //  RECT Screen_Rect;
    //  GetWindowRect(core.GetAppHWND(), &Screen_Rect);
#ifdef _WIN32 // FIX_LINUX Cursor
    lock_x = Screen_Rect.left + (Screen_Rect.right - Screen_Rect.left) / 2;
    lock_y = Screen_Rect.top + (Screen_Rect.bottom - Screen_Rect.top) / 2;
    SetCursorPos(lock_x, lock_y);
#endif
    fXMousePos = static_cast<float>(dwScreenWidth / 2);
    fYMousePos = static_cast<float>(dwScreenHeight / 2);
    for (int i = 0; i < 4; i++)
        vMouse[i].pos.z = 1.f;
    vMouse[0].tu = vMouse[1].tu = 0.f;
    vMouse[2].tu = vMouse[3].tu = 1.f;
    vMouse[0].tv = vMouse[2].tv = 0.f;
    vMouse[1].tv = vMouse[3].tv = 1.f;
#ifdef _WIN32 // FIX_LINUX Cursor
    ShowCursor(false);
#endif

    // set blind parameters
    m_fBlindSpeed = ini->GetFloat(section, "BlindTime", 1.f);
    if (m_fBlindSpeed <= 0.0001f)
        m_fBlindSpeed = 1.f;
    m_fBlindSpeed = 0.002f / m_fBlindSpeed;

    // set wave parameters
    m_nColumnQuantity = ini->GetInt(section, "columnQuantity", m_nColumnQuantity);
    m_fWaveAmplitude = ini->GetFloat(section, "waveAmplitude", m_fWaveAmplitude);
    m_fWavePhase = ini->GetFloat(section, "wavePhase", m_fWavePhase);
    m_fWaveSpeed = ini->GetFloat(section, "waveSpeed", m_fWaveSpeed);
    m_nBlendStepMax = ini->GetInt(section, "waveStepQuantity", m_nBlendStepMax);
    m_nBlendSpeed = ini->GetInt(section, "blendSpeed", m_nBlendSpeed);

    oldKeyState.dwKeyCode = -1;
    DoControl();
    // UNGUARD
}

void XINTERFACE::LoadDialog(const char *sFileName)
{
    char section[255];
    char skey[255];
    char param[255];
    int i;

    if (m_pEditor)
        m_pEditor->ReCreate();

    // initialize ini file
    m_sDialogFileName = sFileName;
    auto ini = fio->OpenIniFile(sFileName);
    if (!ini)
    {
        core.Trace("ini file %s not found!", sFileName);
        core.PostEvent("exitCancel", 1, nullptr);
        return;
    }
    auto ownerIni = fio->OpenIniFile("RESOURCE\\INI\\INTERFACES\\defaultnode.ini");

    sprintf_s(section, "MAIN");

    // load items
    CINODE *curnod = m_pNodes;
    int keyNum = 1;
    char nodeName[sizeof(param)];

    auto findName = "item";
    if (ini->ReadString(section, findName, skey, sizeof(skey) - 1, ""))
        while (true)
        {
            int priority;
            char *tmpStr = skey;
            tmpStr = XI_ParseStr(tmpStr, param, sizeof(param));
            if ((priority = atoi(param)) > 0)
                tmpStr = XI_ParseStr(tmpStr, param, sizeof(param));
            else
                priority = 80;
            if (storm::iEquals(param, "PC") || storm::iEquals(param, "XBOX") || storm::iEquals(param, "LANG"))
            {
                const bool bThisXBOX = false;
                if (storm::iEquals(param, "PC"))
                {
                    if (bThisXBOX)
                        param[0] = 0;
                    tmpStr = XI_ParseStr(tmpStr, param, sizeof(param));
                }
                else if (storm::iEquals(param, "XBOX"))
                {
                    if constexpr (!bThisXBOX)
                        param[0] = 0;
                    else
                        tmpStr = XI_ParseStr(tmpStr, param, sizeof(param));
                }
                else if (storm::iEquals(param, "LANG"))
                {
                    tmpStr = XI_ParseStr(tmpStr, param, sizeof(param));
                    char *strLangName = pStringService->GetLanguage();
                    if (strLangName == nullptr || !storm::iEquals(param, strLangName))
                        param[0] = 0;
                    else
                        tmpStr = XI_ParseStr(tmpStr, param, sizeof(param));
                }
            }
            tmpStr = XI_ParseStr(tmpStr, nodeName, sizeof(nodeName));
            if (param[0])
                SFLB_CreateNode(ownerIni.get(), ini.get(), param, nodeName, priority);

            i = 0;
            if (findName && storm::iEquals(findName, "item"))
            {
                ini->ReadString(section, findName, skey, sizeof(skey) - 1);
                for (; i < keyNum; i++)
                    if (!ini->ReadStringNext(section, findName, skey, sizeof(skey) - 1))
                        break;
            }

            if (i < keyNum)
            {
                // not more items
                if (findName && storm::iEquals(findName, "item"))
                {
                    findName = "glow";
                    if (m_pGlowCursorNode == nullptr)
                    {
                        if (!ini->ReadString(section, findName, skey, sizeof(skey) - 1, "GLOWCURSOR,GLOWCURSOR"))
                        {
                            if (ownerIni && !ownerIni->ReadString("LIST", "glow_cursor", skey, sizeof(skey) - 1,
                                                                  "GLOWCURSOR,GLOWCURSOR"))
                                printf(skey, "GLOWCURSOR,GLOWCURSOR");
                        }
                        continue;
                    }
                }
                break;
            }
            keyNum++;
        }

    // set help data
    if (m_pContHelp != nullptr)
    {
        HELPEntity *pHlist = static_cast<CXI_CONTEXTHELP *>(m_pContHelp)->m_pHelpList;
        const int32_t nListSize = static_cast<CXI_CONTEXTHELP *>(m_pContHelp)->m_helpQuantity;
        for (int n = 0; n < nListSize; n++)
            pHlist[n].pNode = m_pNodes->FindNode(pHlist[n].nodeName);
    }

    // set active item
    if (m_pNodes == nullptr)
        m_pCurNode = nullptr;
    else
    {
        if (!ini->ReadString(section, "start", param, sizeof(param) - 1, ""))
            m_pCurNode = nullptr;
        else if ((m_pCurNode = m_pNodes->FindNode(param)) == nullptr)
            m_pCurNode = GetActivingNode(m_pNodes);

        if (m_pEditor)
        {
            if (!ini->ReadString(section, "editnode", param, sizeof(param) - 1, ""))
                m_pEditor->SetEditNode(nullptr);
            m_pEditor->SetEditNode(m_pNodes->FindNode(param));
        }
    }
    if (ini->ReadString(section, "DefaultHelp", param, sizeof(param) - 1, ""))
    {
        const auto len = strlen(param) + 1;
        m_strDefHelpTextureFile = new char[len];
        if (m_strDefHelpTextureFile)
            memcpy(m_strDefHelpTextureFile, param, len);
    }
    m_frectDefHelpTextureUV = FXYRECT(0.f, 0.f, 1.f, 1.f);
    if (ini->ReadString(section, "DefaultHelpUV", param, sizeof(param) - 1, ""))
    {
        CINODE::GetDataStr(param, "ffff", &m_frectDefHelpTextureUV.left, &m_frectDefHelpTextureUV.top,
                           &m_frectDefHelpTextureUV.right, &m_frectDefHelpTextureUV.bottom);
    }
}

void XINTERFACE::CreateNode(const char *sFileName, const char *sNodeType, const char *sNodeName, int32_t priority)
{
    // there is already such a node
    if (m_pNodes && m_pNodes->FindNode(sNodeName))
        return;

    std::unique_ptr<INIFILE> ini;
    if (sFileName && sFileName[0])
    {
        ini = fio->OpenIniFile(sFileName);
        if (!ini)
        {
            core.Trace("ini file %s not found!", sFileName);
            return;
        }
    }
    auto ownerIni = fio->OpenIniFile("RESOURCE\\INI\\INTERFACES\\defaultnode.ini");

    SFLB_CreateNode(ownerIni.get(), ini.get(), sNodeType, sNodeName, priority);
}

void XINTERFACE::SFLB_CreateNode(INIFILE *pOwnerIni, INIFILE *pUserIni, const char *sNodeType, const char *sNodeName,
                                 int32_t priority)
{
    if (!sNodeType || !sNodeType[0])
    {
        core.Trace("Warning! Interface: Can`t create node with null type.");
        return;
    }
    /*if( !pOwnerIni->TestSection( sNodeType ) &&
      !pUserIni->TestSection( sNodeType ) )
    {
      core.Trace("Warning! Interface: Node type %s not present into describe.",sNodeType);
      return;
    }*/

    if (!sNodeName)
        sNodeName = "?";

    char param[1024];
    if (!pOwnerIni->ReadString(sNodeType, "class", param, sizeof(param) - 1, ""))
        sprintf_s(param, "%s", sNodeType);

    CINODE *pNewNod = nullptr;
    pNewNod = NewNode(param);
    if (pNewNod)
    {
        pNewNod->ptrOwner = this;
        pNewNod->pPictureService = pPictureService;
        pNewNod->pStringService = pStringService;
        pNewNod->SetPriority(priority);
        XYPOINT xypScreenSize;
        xypScreenSize.x = dwScreenWidth;
        xypScreenSize.y = dwScreenHeight;
        const auto len = strlen(sNodeName) + 1;
        pNewNod->m_nodeName = new char[len];
        if (!pNewNod->m_nodeName)
            throw std::runtime_error("allocate memory error");
        memcpy(pNewNod->m_nodeName, sNodeName, len);
        if (!pNewNod->Init(pUserIni, sNodeName, pOwnerIni, sNodeType, pRenderService, GlobalRect, xypScreenSize))
        {
            delete pNewNod;
            pNewNod = nullptr;
        }
        else
        {
            AddNodeToList(pNewNod, priority);
        }

        INIFILE *usedini = pUserIni;
        pNewNod->m_bBreakPress = pNewNod->GetIniBool(pUserIni, sNodeName, pOwnerIni, sNodeType, "bBreakCommand", false);
        if (pNewNod->GetIniBool(pUserIni, sNodeName, pOwnerIni, sNodeType, "moveMouseDoSelect", false))
            pNewNod->m_bMouseSelect = true;
        // if( pNewNod->ReadIniString(pUserIni,sNodeName, pOwnerIni,sNodeType, "command", param,sizeof(param)-1, "") )
        // do
        if (usedini && usedini->ReadString(pNewNod->m_nodeName, "command", param, sizeof(param) - 1, ""))
            do
            {
                // get command name
                char stmp[sizeof(param)];
                sscanf(param, "%[^,]", stmp);
                // search command
                const int nComNum = FindCommand(stmp);
                if (nComNum == -1)
                    continue;

                pNewNod->m_bSelected = true;
                pNewNod->m_bClickable = true;
                pNewNod->m_pCommands[nComNum].bUse = true;
                // command sequence
                size_t nFirstChar = strlen(stmp) + 1;
                while (nFirstChar < strlen(param))
                {
                    sscanf(&param[nFirstChar], "%[^,]", stmp);
                    nFirstChar += strlen(stmp) + 1;

                    // redirect control
                    if (!strncmp(stmp, "select:", 7))
                    {
                        DublicateString(pNewNod->m_pCommands[nComNum].sRetControl, &stmp[7]);
                        continue;
                    }

                    // subnodes control
                    if (!strncmp(stmp, "com:", 4))
                    {
                        char sSubNodName[sizeof(param)];
                        int nSubCommand;
                        char sSubCommand[sizeof(param)];
                        sscanf(stmp, "com:%[^:]:%[^,]", sSubCommand, sSubNodName);
                        if ((nSubCommand = FindCommand(sSubCommand)) == -1)
                            continue;

                        auto *pHead = new CINODE::COMMAND_REDIRECT{};
                        if (pHead == nullptr)
                            throw std::runtime_error("allocate memory error");
                        pHead->next = pNewNod->m_pCommands[nComNum].pNextControl;
                        pNewNod->m_pCommands[nComNum].pNextControl = pHead;
                        DublicateString(pHead->sControlName, sSubNodName);
                        pHead->command = pCommandsList[nSubCommand].code;

                        continue;
                    }

                    if (!strncmp(stmp, "event:", 6))
                    {
                        char sEventName[sizeof(param)];
                        sscanf(stmp, "event:%[^,]", sEventName);
                        DublicateString(pNewNod->m_pCommands[nComNum].sEventName, sEventName);
                        continue;
                    }

                    if (!strncmp(stmp, "delay:", 6))
                    {
                        sscanf(stmp, "delay:%d", &pNewNod->m_pCommands[nComNum].nActionDelay);
                        continue;
                    }

                    if (!strncmp(stmp, "sound:", 6))
                    {
                        sscanf(stmp, "sound:%d", &pNewNod->m_pCommands[nComNum].nSound);
                    }
                }
            } while (usedini->ReadStringNext(pNewNod->m_nodeName, "command", param, sizeof(param) - 1));
    }

    if (m_pEditor && pNewNod)
        m_pEditor->AddNode(pNewNod);
}

CINODE *XINTERFACE::NewNode(const char *pcNodType)
{
    if (!pcNodType)
        return nullptr;
    CINODE *pNewNod = nullptr;
    if (storm::iEquals(pcNodType, "BUTTON"))
        pNewNod = new CXI_BUTTON;
    else if (storm::iEquals(pcNodType, "VIDEO"))
        pNewNod = new CXI_VIDEO;
    else if (storm::iEquals(pcNodType, "SCROLLIMAGE"))
        pNewNod = new CXI_SCROLLIMAGE;
    else if (storm::iEquals(pcNodType, "IMAGECOLLECTION"))
        pNewNod = new CXI_IMGCOLLECTION;
    else if (storm::iEquals(pcNodType, "STRINGCOLLECTION"))
        pNewNod = new CXI_STRCOLLECTION;
    else if (storm::iEquals(pcNodType, "FOURIMAGES"))
        pNewNod = new CXI_FOURIMAGE;
    else if (storm::iEquals(pcNodType, "RECTANGLE"))
        pNewNod = new CXI_RECTANGLE;
    else if (storm::iEquals(pcNodType, "BOUNDER"))
        pNewNod = new CXI_BOUNDER;
    else if (storm::iEquals(pcNodType, "TITLE"))
        pNewNod = new CXI_TITLE;
    else if (storm::iEquals(pcNodType, "TEXTBUTTON"))
        pNewNod = new CXI_TEXTBUTTON;
    else if (storm::iEquals(pcNodType, "SCROLLBAR"))
        pNewNod = new CXI_SCROLLBAR;
    else if (storm::iEquals(pcNodType, "LINECOLLECTION"))
        pNewNod = new CXI_LINECOLLECTION;
    else if (storm::iEquals(pcNodType, "STATUSLINE"))
        pNewNod = new CXI_STATUSLINE;
    else if (storm::iEquals(pcNodType, "CHANGER"))
        pNewNod = new CXI_CHANGER;
    else if (storm::iEquals(pcNodType, "PICTURE"))
        pNewNod = new CXI_PICTURE;
    else if (storm::iEquals(pcNodType, "GLOWS"))
        pNewNod = new CXI_GLOWER;
    else if (storm::iEquals(pcNodType, "LRCHANGER"))
        pNewNod = new CXI_LRCHANGER;
    else if (storm::iEquals(pcNodType, "TWO_PICTURE"))
        pNewNod = new CXI_TWOPICTURE;
    else if (storm::iEquals(pcNodType, "SCROLLER"))
        pNewNod = new CXI_SCROLLER;
    else if (storm::iEquals(pcNodType, "QUESTTITLE"))
        pNewNod = new CXI_QUESTTITLE;
    else if (storm::iEquals(pcNodType, "QUESTTEXT"))
        pNewNod = new CXI_QUESTTEXTS;
    else if (storm::iEquals(pcNodType, "SLIDEPICTURE"))
        pNewNod = new CXI_SLIDEPICTURE;
    else if (storm::iEquals(pcNodType, "FORMATEDTEXT"))
        pNewNod = new CXI_FORMATEDTEXT;
    else if (storm::iEquals(pcNodType, "EDITBOX"))
        pNewNod = new CXI_EDITBOX;
    else if (storm::iEquals(pcNodType, "SLIDER"))
        pNewNod = new CXI_SLIDELINE;
    else if (storm::iEquals(pcNodType, "KEYCHOOSER"))
        pNewNod = new CXI_KEYCHANGER;
    else if (storm::iEquals(pcNodType, "VIDEORECTANGLE"))
        pNewNod = new CXI_VIDEORECT;
    else if (storm::iEquals(pcNodType, "VIMAGESCROLL"))
        pNewNod = new CXI_VIMAGESCROLL;
    else if (storm::iEquals(pcNodType, "PCEDITBOX"))
        pNewNod = new CXI_PCEDITBOX;
    else if (storm::iEquals(pcNodType, "SCROLLEDPICTURE"))
        pNewNod = new CXI_SCROLLEDPICTURE;
    else if (storm::iEquals(pcNodType, "WINDOW"))
        pNewNod = new CXI_WINDOW;
    else if (storm::iEquals(pcNodType, "CHECKBUTTON"))
        pNewNod = new CXI_CHECKBUTTONS;
    else if (storm::iEquals(pcNodType, "TABLE"))
        pNewNod = new CXI_TABLE;
    else if (storm::iEquals(pcNodType, "FRAME"))
        pNewNod = new CXI_BORDER;
    else if (storm::iEquals(pcNodType, "CONTEXTHELP"))
        m_pContHelp = pNewNod = new CXI_CONTEXTHELP;
    else if (storm::iEquals(pcNodType, "GLOWCURSOR"))
        m_pGlowCursorNode = pNewNod = new CXI_GLOWCURSOR;
    else
        core.Trace("Not supported node type:\"%s\"", pcNodType);
    return pNewNod;
}

void XINTERFACE::DeleteNode(const char *pcNodeName)
{
    if (!pcNodeName)
        return;
    // looking for a node by name
    CINODE *pPrevNod = nullptr;
    CINODE *pNod;
    for (pNod = m_pNodes; pNod; pNod = pNod->m_next)
    {
        if (pNod->m_nodeName && storm::iEquals(pNod->m_nodeName, pcNodeName))
            break;
        pPrevNod = pNod;
    }
    // found?
    if (pNod)
    {
        // checking pointers to a reference to a deleted node
        if (m_pCurNode == pNod)
            m_pCurNode = nullptr;
        if (m_pContHelp == pNod)
            m_pContHelp = nullptr;
        if (m_pGlowCursorNode == pNod)
            m_pGlowCursorNode = nullptr;
        // remove the found node from the list
        if (pPrevNod)
            pPrevNod->m_next = pNod->m_next;
        else
            m_pNodes = pNod->m_next;
        if (m_pEditor)
            m_pEditor->DelNode(pNod);
        // delete the node
        delete pNod;
    }
}

void XINTERFACE::SetTooltip(const char *pcHeader, const char *pcText1, uint32_t dwTextColor1, const char *pcText2,
                            uint32_t dwTextColor2, const char *pcText3, uint32_t dwTextColor3, const char *pcText4,
                            uint32_t dwTextColor4, const char *pcPicTextureName, const char *pcPicGroupName,
                            const char *pcPicImageName, int32_t nPicWidth, int32_t nPicHeight)
{
    CINODE *pTmpNod;
    pTmpNod = FindNode("tooltip_frame", nullptr);
    CXI_BORDER *pNodFrame =
        pTmpNod ? pTmpNod->m_nNodeType == NODETYPE_BORDER ? static_cast<CXI_BORDER *>(pTmpNod) : nullptr : nullptr;
    pTmpNod = FindNode("tooltip_titlerect", nullptr);
    CXI_RECTANGLE *pNodTitleRect =
        pTmpNod ? pTmpNod->m_nNodeType == NODETYPE_RECTANGLE ? static_cast<CXI_RECTANGLE *>(pTmpNod) : nullptr
                : nullptr;
    pTmpNod = FindNode("tooltip_picture", nullptr);
    CXI_PICTURE *pNodPic =
        pTmpNod ? pTmpNod->m_nNodeType == NODETYPE_PICTURE ? static_cast<CXI_PICTURE *>(pTmpNod) : nullptr : nullptr;
    pTmpNod = FindNode("tooltip_textborder2", nullptr);
    CINODE *pNodTextFrame2 = pTmpNod ? pTmpNod->m_nNodeType == NODETYPE_RECTANGLE ? pTmpNod : nullptr : nullptr;
    pTmpNod = FindNode("tooltip_textborder4", nullptr);
    CINODE *pNodTextFrame4 = pTmpNod ? pTmpNod->m_nNodeType == NODETYPE_RECTANGLE ? pTmpNod : nullptr : nullptr;
    pTmpNod = FindNode("tooltip_titile", nullptr);
    CXI_STRCOLLECTION *pNodTitle =
        pTmpNod
            ? pTmpNod->m_nNodeType == NODETYPE_STRINGCOLLECTION ? static_cast<CXI_STRCOLLECTION *>(pTmpNod) : nullptr
            : nullptr;
    pTmpNod = FindNode("tooltip_text1", nullptr);
    CXI_FORMATEDTEXT *pNodText1 =
        pTmpNod ? pTmpNod->m_nNodeType == NODETYPE_FORMATEDTEXTS ? static_cast<CXI_FORMATEDTEXT *>(pTmpNod) : nullptr
                : nullptr;
    pTmpNod = FindNode("tooltip_text2", nullptr);
    CXI_FORMATEDTEXT *pNodText2 =
        pTmpNod ? pTmpNod->m_nNodeType == NODETYPE_FORMATEDTEXTS ? static_cast<CXI_FORMATEDTEXT *>(pTmpNod) : nullptr
                : nullptr;
    pTmpNod = FindNode("tooltip_text3", nullptr);
    CXI_FORMATEDTEXT *pNodText3 =
        pTmpNod ? pTmpNod->m_nNodeType == NODETYPE_FORMATEDTEXTS ? static_cast<CXI_FORMATEDTEXT *>(pTmpNod) : nullptr
                : nullptr;
    pTmpNod = FindNode("tooltip_text4", nullptr);
    CXI_FORMATEDTEXT *pNodText4 =
        pTmpNod ? pTmpNod->m_nNodeType == NODETYPE_FORMATEDTEXTS ? static_cast<CXI_FORMATEDTEXT *>(pTmpNod) : nullptr
                : nullptr;
    if (!pNodFrame || !pNodTitleRect || !pNodPic || !pNodTextFrame2 || !pNodTextFrame4 || !pNodTitle || !pNodText1 ||
        !pNodText2 || !pNodText3 || !pNodText4)
    {
        core.Trace("Warning! Interface::SetTooltip - no precreated node");
        return;
    }
    // set
    pNodTitle->ChangeString(0, pcHeader);
    if (pcPicTextureName && pcPicTextureName[0])
        pNodPic->SetNewPicture(false, (char *)pcPicTextureName);
    if (pcPicGroupName && pcPicImageName && pcPicGroupName[0] && pcPicImageName[0])
        pNodPic->SetNewPictureByGroup((char *)pcPicGroupName, (char *)pcPicImageName);
    pNodPic->SetPictureSize(nPicWidth, nPicHeight);
    if (pcText1 && pcText1[0])
    {
        pNodText1->SetFormatedText((char *)pcText1);
        pNodText1->SetColor(dwTextColor1);
    }
    if (pcText2 && pcText2[0])
    {
        pNodText2->SetFormatedText((char *)pcText2);
        pNodText2->SetColor(dwTextColor2);
    }
    if (pcText3 && pcText3[0])
    {
        pNodText3->SetFormatedText((char *)pcText3);
        pNodText3->SetColor(dwTextColor3);
    }
    if (pcText4 && pcText4[0])
    {
        pNodText4->SetFormatedText((char *)pcText4);
        pNodText4->SetColor(dwTextColor4);
    }

    XYRECT rTitle, rPic, rTex1, rTex2Rect, rTex2, rTex3, rTex4Rect, rTex4;
    rTitle = pNodTitleRect->m_rect;
    rPic = pNodPic->m_rect;
    rTex1 = pNodText1->m_rect;
    rTex2 = pNodText2->m_rect;
    rTex3 = pNodText3->m_rect;
    rTex4 = pNodText4->m_rect;
    rTex2Rect = pNodTextFrame2->m_rect;
    rTex4Rect = pNodTextFrame4->m_rect;

    int32_t nAllHeight = TT_TITLE_OFFSET + TT_TITLE_HEIGHT;
    rTitle.top = TT_TITLE_OFFSET;
    rTitle.bottom = TT_TITLE_HEIGHT;

    if (nPicHeight > 0)
    {
        rPic.top = nAllHeight + TT_PIC_UP_OFFSET;
        rPic.bottom = nPicHeight;
        nAllHeight = rPic.top + rPic.bottom + TT_PIC_DOWN_OFFSET;
    }
    else
    {
        pNodPic->m_bUse = false;
    }

    int32_t nTmpH = pNodText1->GetAllHeight();
    if (nTmpH > 0)
    {
        rTex1.top = nAllHeight + TT_TEX1_UP_OFFSET;
        rTex1.bottom = nTmpH;
        nAllHeight = rTex1.top + rTex1.bottom + TT_TEX1_DOWN_OFFSET;
    }
    else
    {
        pNodText1->m_bUse = false;
    }

    nTmpH = pNodText2->GetAllHeight();
    if (nTmpH > 0)
    {
        rTex2Rect.top = nAllHeight + TT_TEX2RECT_UP_OFFSET;
        rTex2.top = rTex2Rect.top + TT_TEX2_UP_OFFSET;
        rTex2.bottom = nTmpH;
        rTex2Rect.bottom = rTex2.top - rTex2Rect.top + rTex2.bottom + TT_TEX2_DOWN_OFFSET;
        nAllHeight = rTex2Rect.top + rTex2Rect.bottom + TT_TEX2RECT_DOWN_OFFSET;
    }
    else
    {
        pNodTextFrame2->m_bUse = false;
        pNodText2->m_bUse = false;
    }

    nTmpH = pNodText3->GetAllHeight();
    if (nTmpH > 0)
    {
        rTex3.top = nAllHeight + TT_TEX3_UP_OFFSET;
        rTex3.bottom = nTmpH;
        nAllHeight = rTex3.top + rTex3.bottom + TT_TEX3_DOWN_OFFSET;
    }
    else
    {
        pNodText3->m_bUse = false;
    }

    nTmpH = pNodText4->GetAllHeight();
    if (nTmpH > 0)
    {
        rTex4Rect.top = nAllHeight + TT_TEX4RECT_UP_OFFSET;
        rTex4.top = rTex4Rect.top + TT_TEX4_UP_OFFSET;
        rTex4.bottom = nTmpH;
        rTex4Rect.bottom = rTex4.top - rTex4Rect.top + rTex4.bottom + TT_TEX4_DOWN_OFFSET;
        nAllHeight = rTex4Rect.top + rTex4Rect.bottom + TT_TEX4RECT_DOWN_OFFSET;
    }
    else
    {
        pNodTextFrame4->m_bUse = false;
        pNodText4->m_bUse = false;
    }

    nAllHeight += TT_FRAME_DOWN_OFFSET;

    // frame
    XYRECT rFrame = pNodFrame->m_rect;
    rFrame.top = (rFrame.top + rFrame.bottom - nAllHeight) / 2;
    rFrame.bottom = rFrame.top + nAllHeight;
    pNodFrame->ChangePosition(rFrame);
    // title
    rTitle.top += rFrame.top;
    rTitle.bottom += rTitle.top;
    pNodTitleRect->ChangePosition(rTitle);
    XYPOINT newTitlePnt;
    newTitlePnt.x = (rTitle.left + rTitle.right) / 2;
    newTitlePnt.y = rTitle.top + TT_TITLE_STRING_OFFSET;
    pNodTitle->ChangeStringPos(0, newTitlePnt);
    // pic
    rPic.top += rFrame.top;
    rPic.bottom += rPic.top;
    pNodPic->ChangePosition(rPic);
    // text 1
    rTex1.top += rFrame.top;
    rTex1.bottom += rTex1.top;
    pNodText1->ChangePosition(rTex1);
    // text 2 rect
    rTex2Rect.top += rFrame.top;
    rTex2Rect.bottom += rTex2Rect.top;
    pNodTextFrame2->ChangePosition(rTex2Rect);
    // text 2
    rTex2.top += rFrame.top;
    rTex2.bottom += rTex2.top;
    pNodText2->ChangePosition(rTex2);
    // text 3
    rTex3.top += rFrame.top;
    rTex3.bottom += rTex3.top;
    pNodText3->ChangePosition(rTex3);
    // text 4 rect
    rTex4Rect.top += rFrame.top;
    rTex4Rect.bottom += rTex4Rect.top;
    pNodTextFrame4->ChangePosition(rTex4Rect);
    // text 4
    rTex4.top += rFrame.top;
    rTex4.bottom += rTex4.top;
    pNodText4->ChangePosition(rTex4);
}

void XINTERFACE::ShowWindow(const char *pcWindowName, bool bShow)
{
    if (!m_pNodes)
        return;
    CINODE *pNod = m_pNodes->FindNode(pcWindowName);
    if (!pNod)
        return;
    if (pNod->m_nNodeType != NODETYPE_WINDOW)
        return;

    static_cast<CXI_WINDOW *>(pNod)->SetShow(bShow);
}

void XINTERFACE::DisableWindow(const char *pcWindowName, bool bDisable)
{
    if (!m_pNodes)
        return;
    CINODE *pNod = m_pNodes->FindNode(pcWindowName);
    if (!pNod)
        return;
    if (pNod->m_nNodeType != NODETYPE_WINDOW)
        return;

    static_cast<CXI_WINDOW *>(pNod)->SetActive(!bDisable);
}

void XINTERFACE::AddNodeToWindow(const char *pcNodeName, const char *pcWindowName)
{
    if (!m_pNodes)
    {
        core.Trace("Warning! Interface::AddNodeToWindow(%s,%s) : Empty node list", pcNodeName, pcWindowName);
        return;
    }

    CINODE *pNod = m_pNodes->FindNode(pcWindowName);
    if (!pNod || pNod->m_nNodeType != NODETYPE_WINDOW)
    {
        core.Trace("Warning! Interface::AddNodeToWindow(%s,%s) : Window not found", pcNodeName, pcWindowName);
        return;
    }

    static_cast<CXI_WINDOW *>(pNod)->AddNode(pcNodeName);
}

CXI_WINDOW *XINTERFACE::FindWindow(const char *pcWindowName)
{
    CINODE *pNod = m_pNodes->FindNode(pcWindowName);
    if (pNod && pNod->m_nNodeType == NODETYPE_WINDOW)
        return static_cast<CXI_WINDOW *>(pNod);
    return nullptr;
}

bool XINTERFACE::IsWindowActive(const char *pcWindowName)
{
    CINODE *pNod = m_pNodes->FindNode(pcWindowName);
    if (pNod && pNod->m_nNodeType == NODETYPE_WINDOW)
        return static_cast<CXI_WINDOW *>(pNod)->GetActive();
    return false;
}

void XINTERFACE::RegistryExitKey(const char *pcKeyName)
{
    m_asExitKey.push_back(pcKeyName);
}

int32_t XINTERFACE::StoreNodeLocksWithOff()
{
    m_aLocksArray.push_back(LocksInfo{});
    // int32_t nStoreSlot = m_aLocksArray.Add();
    int32_t nStoreCode;
    for (nStoreCode = 0; nStoreCode < 1000; nStoreCode++)
    {
        int32_t n;
        for (n = 0; n < m_aLocksArray.size(); n++)
            if (m_aLocksArray[n].nSaveCode == nStoreCode)
                break;
        if (n == m_aLocksArray.size())
            break;
    }
    m_aLocksArray.back().nSaveCode = nStoreCode;
    for (CINODE *pNod = m_pNodes; pNod; pNod = pNod->m_next)
        // if( !pNod->m_bLockStatus ) {
        if (!pNod->m_bLockedNode)
        {
            m_aLocksArray.back().aNode.push_back(pNod);
            // pNod->m_bLockStatus = true;
            pNod->m_bLockedNode = true;
        }
    return nStoreCode;
}

void XINTERFACE::RestoreNodeLocks(int32_t nStoreCode)
{
    int32_t n;
    for (n = 0; n < m_aLocksArray.size(); n++)
        if (m_aLocksArray[n].nSaveCode == nStoreCode)
            break;
    if (n == m_aLocksArray.size())
        return;
    for (int32_t i = 0; i < m_aLocksArray[n].aNode.size(); i++)
    {
        // m_aLocksArray[n].aNode[i]->m_bLockStatus = false;
        m_aLocksArray[n].aNode[i]->m_bLockedNode = false;
    }
    m_aLocksArray.erase(m_aLocksArray.begin() + n);
}

void XINTERFACE::DrawNode(CINODE *nod, uint32_t Delta_Time, int32_t startPrior, int32_t endPrior) const
{
    for (; nod != nullptr; nod = nod->m_next)
    {
        if (nod->GetPriority() < startPrior)
            continue;
        if (nod->GetPriority() > endPrior)
            break;
        if (nod->m_bUse)
        {
            if (nod == m_pGlowCursorNode)
            {
                nod->Draw(false, 0);
                continue;
            }
            if (nod == m_pCurNode)
            {
                if (m_pGlowCursorNode && m_pGlowCursorNode->m_bUse)
                    m_pGlowCursorNode->Draw(false, Delta_Time);
            }
            nod->Draw(nod == m_pCurNode, Delta_Time);
        }
        else
            nod->NotUsingTime(Delta_Time);
        DrawNode(nod->m_list, Delta_Time);
    }
}

void XINTERFACE::DoControl()
{
    int32_t nExitKey;
    CONTROL_STATE cs;
    if (!m_bUse)
        return;

    core.Controls->GetControlState("IContextHelp", cs);

    if (m_nInterfaceMode == CONTEXTHELP_IMODE)
    {
        if (cs.state == CST_ACTIVATED || cs.state == CST_INACTIVATED)
            ReleaseContextHelpData();
        return;
    }

    if (cs.state == CST_ACTIVATED)
    {
        GetContextHelpData();
        return;
    }

    if (m_pEditor && m_pEditor->ProcessControl())
        return;

    if (bDisableControl)
    {
        core.Controls->GetControlState(INTERFACE_CONTROL_RIGHT, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState(INTERFACE_CONTROL_LEFT, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState(INTERFACE_CONTROL_UP, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState(INTERFACE_CONTROL_DOWN, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState(INTERFACE_CONTROL_LSHIFT, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState(INTERFACE_CONTROL_RSHIFT, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState(INTERFACE_CONTROL_ENTER, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState(INTERFACE_CONTROL_BACK, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState(INTERFACE_CONTROL_BREAK, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState(INTERFACE_CONTROL_LCLICK, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState(INTERFACE_CONTROL_RCLICK, cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState("IStartButton", cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState("IUp2", cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState("IDown2", cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState("IRight2", cs);
        if (cs.state != CST_INACTIVE)
            return;
        core.Controls->GetControlState("ILeft2", cs);
        if (cs.state != CST_INACTIVE)
            return;

        for (nExitKey = 0; nExitKey < m_asExitKey.size(); nExitKey++)
        {
            core.Controls->GetControlState((char *)m_asExitKey[nExitKey].c_str(), cs);
            if (cs.state != CST_INACTIVE)
                return;
        }
    }
    if (!g_bIExclusiveMode && !DiskCheck)
        return;
    bDisableControl = false;

    for (nExitKey = 0; nExitKey < m_asExitKey.size(); nExitKey++)
    {
        core.Controls->GetControlState((char *)m_asExitKey[nExitKey].c_str(), cs);
        if (cs.state == CST_ACTIVATED)
        {
            core.Event("exitCancel");
            break;
        }
    }

    int wActCode = ACTION_NOTHING;

    KEYSTATE curKS;
    curKS.dwKeyCode = 0;

    bool bDeClick = false;
    bool bWasFirst = false;
    m_bMouseClick = false;
    m_bDblMouseClick = false;
    core.Controls->GetControlState(INTERFACE_CONTROL_LCLICK, cs);
    if (cs.state == CST_ACTIVATED)
    {
        m_bMouseClick = true;
        m_idButton = MOUSE_LBUTTON;
        bWasFirst = true;
    }
    if (cs.state == CST_INACTIVATED)
    {
        bDeClick = true;
        m_idButton = MOUSE_LBUTTON;
        if (bDeClick && m_pMouseNode && m_pMouseNode->m_bMakeActionInDeclick)
        {
            MouseDeClick();
        }
    }
    if (cs.state == CST_ACTIVE)
    {
        m_bMouseClick = true;
        m_idButton = MOUSE_LBUTTON;
    }

    core.Controls->GetControlState(INTERFACE_CONTROL_RCLICK, cs);
    if (!m_bMouseClick && cs.state == CST_ACTIVATED)
    {
        m_bMouseClick = true;
        m_idButton = MOUSE_RBUTTON;
        bWasFirst = true;
        core.Event("MouseRClickDown");
    }
    if (cs.state == CST_INACTIVATED)
    {
        core.Event("MouseRClickUP");
    }
    if (!m_bMouseClick && cs.state == CST_ACTIVE)
    {
        m_bMouseClick = true;
        m_idButton = MOUSE_RBUTTON;
    }

    if (m_bMouseClick || m_bDblMouseClick)
    {
        MouseClick(bWasFirst);
    }
    else
    {
        bool bFirstPress = false;
        // Get action code CST_ACTIVATED, CST_ACTIVE

        // right press
        core.Controls->GetControlState(INTERFACE_CONTROL_RIGHT, cs);
        if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
            curKS.rightButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        core.Controls->GetControlState("IRight2", cs);
        if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
            curKS.rightButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        // left press
        core.Controls->GetControlState(INTERFACE_CONTROL_LEFT, cs);
        if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
            curKS.leftButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        core.Controls->GetControlState("ILeft2", cs);
        if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
            curKS.leftButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        // up press
        core.Controls->GetControlState(INTERFACE_CONTROL_UP, cs);
        if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
            curKS.upButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        core.Controls->GetControlState("IUp2", cs);
        if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
            curKS.upButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        // down press
        core.Controls->GetControlState(INTERFACE_CONTROL_DOWN, cs);
        if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
            curKS.downButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        core.Controls->GetControlState("IDown2", cs);
        if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
            curKS.downButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        // left shift press
        core.Controls->GetControlState(INTERFACE_CONTROL_LSHIFT, cs);
        if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
            curKS.shiftButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        // right shift press
        core.Controls->GetControlState(INTERFACE_CONTROL_RSHIFT, cs);
        if (cs.state == CST_ACTIVE || cs.state == CST_ACTIVATED)
            curKS.shiftButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        // enter button press
        core.Controls->GetControlState(INTERFACE_CONTROL_ENTER, cs);
        if (cs.state == CST_ACTIVATED) //~!~
            curKS.enterButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        core.Controls->GetControlState("IStartButton", cs);
        if (cs.state == CST_ACTIVATED)
            curKS.enterButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        // back button press
        core.Controls->GetControlState(INTERFACE_CONTROL_BACK, cs);
        if (cs.state == CST_ACTIVATED)
            curKS.backButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;
        core.Controls->GetControlState(INTERFACE_CONTROL_BREAK, cs);
        if (cs.state == CST_ACTIVATED)
            curKS.backButton = true;
        if (cs.state == CST_ACTIVATED)
            bFirstPress = true;

        int32_t nWeel = 0;
        if (m_pMouseWeel)
            m_pMouseWeel->Get(nWeel);
        if (nWeel != 0 && m_pCurNode && m_pCurNode->IsWeelActive())
        {
            if (nWeel > 0)
                curKS.upButton = true;
            else
                curKS.downButton = true;
        }

        //
        if (AttributesPointer)
        {
            ATTRIBUTES *pA = AttributesPointer->GetAttributeClass("KeyStates");
            if (pA)
                pA->SetAttributeUseDword("shift", curKS.shiftButton);
        }

        if (oldKeyState.dwKeyCode == -1)
            oldKeyState.dwKeyCode = curKS.dwKeyCode;

        if (oldKeyState.dwKeyCode != curKS.dwKeyCode)
        {
            if (curKS.rightButton)
            {
                if (curKS.shiftButton)
                    wActCode = ACTION_SPEEDRIGHT;
                else
                    wActCode = ACTION_RIGHTSTEP;
            }
            if (curKS.leftButton)
            {
                if (curKS.shiftButton)
                    wActCode = ACTION_SPEEDLEFT;
                else
                    wActCode = ACTION_LEFTSTEP;
            }
            if (curKS.upButton)
            {
                if (curKS.shiftButton)
                    wActCode = ACTION_SPEEDUP;
                else
                    wActCode = ACTION_UPSTEP;
            }
            if (curKS.downButton)
            {
                if (curKS.shiftButton)
                    wActCode = ACTION_SPEEDDOWN;
                else
                    wActCode = ACTION_DOWNSTEP;
            }
            if (curKS.enterButton)
                wActCode = ACTION_ACTIVATE;
            if (curKS.backButton)
                wActCode = ACTION_DEACTIVATE;

            if (m_pCurNode == nullptr)
            {
                if (wActCode == ACTION_DEACTIVATE)
                    core.Event("exitCancel");
                return;
            }

            bool bBreakPress = false;
            if (!m_pCurNode->m_bLockStatus && wActCode != ACTION_NOTHING)
            {
                if (m_nPressDelay == 0)
                {
                    if (!m_bNotFirstPress)
                    {
                        m_nPressDelay = m_nMaxPressDelay;
                        m_bNotFirstPress = true;
                    }
                    CINODE *pNewNod = m_pCurNode->DoAction(wActCode, bBreakPress, bFirstPress);
                    if (pNewNod != nullptr)
                    {
                        SetCurNode(pNewNod);
                        bBreakPress = true;
                    }
                }
            }
            else
            {
                m_nPressDelay = 0;
                m_bNotFirstPress = false;
            }

            if (bBreakPress || curKS.dwKeyCode == 0)
                oldKeyState.dwKeyCode = curKS.dwKeyCode;
        }
        else
        {
            m_nPressDelay = 0;
            m_bNotFirstPress = false;
        }
    }
}

CINODE *XINTERFACE::GetActivingNode(CINODE *findRoot)
{
    CINODE *retVal = nullptr;

    while (findRoot)
    {
        if (findRoot->m_bClickable)
        {
            retVal = findRoot;
            break;
        }
        if ((retVal = GetActivingNode(findRoot->m_list)) != nullptr)
            break;

        findRoot = findRoot->m_next;
    }

    return retVal;
}

void XINTERFACE::MouseMove()
{
    if (m_nInterfaceMode == CONTEXTHELP_IMODE)
        return;
    CONTROL_STATE csv, csh;
    core.Controls->GetControlState(INTERFACE_MOUSE_VERT, csv);
    core.Controls->GetControlState(INTERFACE_MOUSE_HORZ, csh);

    float fOutX = 0.f;
    float fOutY = 0.f;

    CINODE *pNod = nullptr;

    if (csv.lValue != 0 || csh.lValue != 0)
    {
        m_nMouseLastClickTimeCur = 0;

        fYMousePos -= csv.fValue;
        fXMousePos += csh.fValue;

        if (fYMousePos < 0)
        {
            fOutY = fYMousePos;
            fYMousePos = 0.f;
        }
        if (fYMousePos > dwScreenHeight)
        {
            fOutY = fYMousePos - dwScreenHeight;
            fYMousePos = static_cast<float>(dwScreenHeight);
        }

        if (fXMousePos < 0)
        {
            fOutX = fXMousePos;
            fXMousePos = 0.f;
        }
        if (fXMousePos > dwScreenWidth)
        {
            fOutX = fXMousePos - dwScreenWidth;
            fXMousePos = static_cast<float>(dwScreenWidth);
        }

        m_bShowMouse = true;
        vMouse[0].pos.x = vMouse[1].pos.x = fXMousePos - MouseSize.x / 2;
        vMouse[2].pos.x = vMouse[3].pos.x = fXMousePos + MouseSize.x / 2;
        vMouse[0].pos.y = vMouse[2].pos.y = fYMousePos - MouseSize.y / 2;
        vMouse[1].pos.y = vMouse[3].pos.y = fYMousePos + MouseSize.y / 2;

        m_pCurToolTipNode = nullptr;
        pNod = m_pNodes ? m_pNodes->FindNode(fXMousePos + m_lXMouse, fYMousePos + m_lYMouse) : nullptr;
        while (pNod != nullptr)
        {
            if (pNod->m_bUse)
            {
                if (!m_pCurToolTipNode && pNod->CheckByToolTip(fXMousePos + m_lXMouse, fYMousePos + m_lYMouse))
                    m_pCurToolTipNode = pNod;
                if (!pNod->m_bLockedNode)
                {
                    if (pNod->m_bSelected)
                        pNod->MouseThis(fXMousePos + m_lXMouse, fYMousePos + m_lYMouse);
                    if (pNod->m_bMouseSelect && pNod->m_bSelected)
                    {
                        SetCurNode(pNod);
                        break;
                    }
                }
            }
            if (pNod->m_next == nullptr)
                break;
            pNod = pNod->m_next->FindNode(fXMousePos + m_lXMouse, fYMousePos + m_lYMouse);
        }
    }
    else
    {
        if (m_pCurToolTipNode)
            m_pCurToolTipNode->CheckByToolTip(fXMousePos + m_lXMouse, fYMousePos + m_lYMouse);
    }

    ATTRIBUTES *pA = AttributesPointer->GetAttributeClass("mousepos");
    if (!pA)
        pA = AttributesPointer->CreateSubAClass(AttributesPointer, "mousepos");
    if (pA)
    {
        pA->SetAttributeUseFloat("x", fXMousePos + m_lXMouse);
        pA->SetAttributeUseFloat("y", fYMousePos + m_lYMouse);
    }

    if (fXMousePos < m_fpMouseOutZoneOffset.x)
        fOutX += fXMousePos - m_fpMouseOutZoneOffset.x;
    if (fXMousePos > dwScreenWidth - m_fpMouseOutZoneOffset.x)
        fOutX += fXMousePos - dwScreenWidth + m_fpMouseOutZoneOffset.x;

    if (fYMousePos < m_fpMouseOutZoneOffset.y)
        fOutY += fYMousePos - m_fpMouseOutZoneOffset.y;
    if (fYMousePos > dwScreenHeight - m_fpMouseOutZoneOffset.y)
        fOutY += fYMousePos - dwScreenHeight + m_fpMouseOutZoneOffset.y;

    if (fOutX != 0.f || fOutY != 0.f)
    {
        for (pNod = m_pNodes; pNod != nullptr; pNod = pNod->m_next)
        {
            pNod->MoveMouseOutScreen(fOutX, fOutY);
        }
    }
}

void XINTERFACE::MouseClick(bool bFirstClick)
{
    if (!bFirstClick && m_nPressDelay > 0)
        return;
    CINODE *clickNod =
        GetClickNode(m_pNodes, static_cast<int32_t>(fXMousePos) + m_lXMouse, static_cast<int32_t>(fYMousePos) + m_lYMouse);
    if (!clickNod)
    {
        if (bFirstClick)
            m_pMouseNode = nullptr;
        return;
    }
    if (!m_bNotFirstPress)
        m_bNotFirstPress = true;
    m_nPressDelay = m_nMaxPressDelay;

    if (bFirstClick && m_nMouseLastClickTimeCur > 0 && clickNod->CheckCommandUsed(ACTION_MOUSEDBLCLICK))
    {
        m_bDblMouseClick = true;
    }
    m_nMouseLastClickTimeCur = m_nMouseLastClickTimeMax;

    if (clickNod != nullptr) //~!~
        if (m_pCurNode == nullptr || !m_pCurNode->m_bLockStatus)
        {
            if (bFirstClick)
                m_pMouseNode = clickNod;
            else if (m_pMouseNode != clickNod)
                return;

            if (m_idButton == MOUSE_LBUTTON && clickNod->m_bMakeActionInDeclick)
            {
                clickNod->MakeLClickPreaction();
            }
            else
            {
                bool bBreakPress;
                CINODE *pNewNod;

                if (m_bDblMouseClick)
                    if (m_idButton == MOUSE_LBUTTON)
                        pNewNod = clickNod->DoAction(ACTION_MOUSEDBLCLICK, bBreakPress, bFirstClick);
                    else
                        pNewNod = clickNod->DoAction(ACTION_MOUSERDBLCLICK, bBreakPress, bFirstClick);
                else if (m_idButton == MOUSE_LBUTTON)
                    pNewNod = clickNod->DoAction(ACTION_MOUSECLICK, bBreakPress, bFirstClick);
                else
                    pNewNod = clickNod->DoAction(ACTION_MOUSERCLICK, bBreakPress, bFirstClick);

                if (pNewNod != nullptr)
                {
                    if (pNewNod != m_pCurNode)
                        m_bNotFirstPress = false;
                    SetCurNode(pNewNod);
                }
                if (bBreakPress)
                {
                    m_bNotFirstPress = false;
                    m_bMouseClick = false;
                }
                m_bDblMouseClick = false;
            }
        }
}

void XINTERFACE::MouseDeClick()
{
    if (!m_pMouseNode || !m_pMouseNode->m_bMakeActionInDeclick)
        return;

    bool bBreakPress = true;
    CINODE *pNewNod = nullptr;
    if (m_idButton == MOUSE_LBUTTON)
        pNewNod = m_pMouseNode->DoAction(ACTION_MOUSECLICK, bBreakPress, true);

    if (pNewNod != nullptr)
    {
        if (pNewNod != m_pCurNode)
            m_bNotFirstPress = false;
        SetCurNode(pNewNod);
    }
    if (bBreakPress)
    {
        m_bNotFirstPress = false;
        m_bMouseClick = false;
    }
}

CINODE *XINTERFACE::GetClickNode(CINODE *searchNod, int32_t xPos, int32_t yPos) const
{
    CINODE *findNod = nullptr;

    while (searchNod != nullptr)
    {
        if (searchNod->m_bUse && !searchNod->m_bLockedNode)
        {
            if (searchNod->m_bSelected && searchNod->m_bClickable && searchNod->CheckCommandUsed(ACTION_MOUSECLICK))
            {
                if (searchNod->IsClick(m_idButton, xPos, yPos))
                    findNod = searchNod;
            }
            if (searchNod->m_list != nullptr)
            {
                CINODE *tmpNod = GetClickNode(searchNod, xPos, yPos);
                if (tmpNod != nullptr)
                    findNod = tmpNod;
            }
        }

        searchNod = searchNod->m_next;
    }

    return findNod;
}

void XINTERFACE::ShowPrevTexture()
{
    XI_ONETEX_VERTEX pV[32];
    int idx = 0;
    float fAngle = m_fAngle;

    uint32_t dwBlendColor = m_nBlendColor;
    if (dwBlendColor > 0xFFL)
        dwBlendColor = 0xFFL;
    const uint32_t dwColor = 0xFFFFFF | (dwBlendColor << 24L);

    const float fWaveAmplitude = m_fWaveAmplitude * sinf(3.14f * m_nBlendColor / m_nBlendStepMax);
    for (int ix = 0; ix < m_nColumnQuantity + 1; ix++)
    {
        pV[idx].color = pV[idx + 1].color = dwColor;
        pV[idx].pos.z = pV[idx + 1].pos.z = 1.f + fWaveAmplitude * sinf(fAngle);
        fAngle += m_fWavePhase;
        pV[idx].pos.x = pV[idx + 1].pos.x = (static_cast<float>(dwScreenWidth) / m_nColumnQuantity) * ix;
        pV[idx].pos.y = 0;
        pV[idx + 1].pos.y = static_cast<float>(dwScreenHeight);
        pV[idx].tv = 0.f;
        pV[idx + 1].tv = 1.f;
        pV[idx].tu = pV[idx + 1].tu = static_cast<float>(ix) / m_nColumnQuantity;
        idx += 2;
    }

    m_fAngle += m_fWaveSpeed;
    m_nBlendColor -= m_nBlendSpeed;
    if (m_nBlendColor <= 0)
    {
        m_nBlendColor = 0;
        m_fAngle = 0.f;
        m_bShowPrevTexture = false;
        if (m_pPrevTexture != nullptr)
            pRenderService->Release(m_pPrevTexture);
        m_pPrevTexture = nullptr;
    }

    pRenderService->SetTexture(0, m_pPrevTexture);
    pRenderService->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    pRenderService->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
    pRenderService->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    pRenderService->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    pRenderService->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    pRenderService->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 30, pV, sizeof(XI_ONETEX_VERTEX));
}

void XINTERFACE::ReleaseOld()
{
    if (m_pEditor)
        m_pEditor->ReCreate();

    ReleaseContextHelpData();
    STORM_DELETE(m_strDefHelpTextureFile);

    oldKeyState.dwKeyCode = 0;
    for (int i = 0; i < m_nStringQuantity; i++)
    {
        STORM_DELETE(m_stringes[i].sStringName);
        FONT_RELEASE(pRenderService, m_stringes[i].fontNum);
    }
    m_nStringQuantity = 0;

    while (m_pEvents != nullptr)
    {
        EVENT_Entity *ei = m_pEvents;
        m_pEvents = m_pEvents->next;
        delete ei;
    }

    while (m_pNodes != nullptr)
    {
        m_pNodes->ReleaseAll();
        CINODE *pNod = m_pNodes;
        m_pNodes = m_pNodes->m_next;
        delete pNod;
    }
    m_pCurNode = nullptr;
    m_pContHelp = nullptr;
    m_pGlowCursorNode = nullptr;

    while (m_imgLists != nullptr)
    {
        PICTURE_TEXTURE_RELEASE(pPictureService, m_imgLists->sImageListName, m_imgLists->idTexture);
        STORM_DELETE(m_imgLists->sImageListName);
        STORM_DELETE(m_imgLists->sImageName);
        STORM_DELETE(m_imgLists->sPicture);
        IMAGE_Entity *pCur = m_imgLists;
        m_imgLists = m_imgLists->next;
        delete pCur;
    }

    m_asExitKey.clear();

    m_bUse = false;
    m_pMouseNode = nullptr;
}

bool XINTERFACE::SetCurNode(CINODE *pNod)
{
    if (m_pCurNode == pNod)
        return false;
    m_pCurNode = pNod;

    if (m_pContHelp != nullptr)
        static_cast<CXI_CONTEXTHELP *>(m_pContHelp)->ChangeNode(pNod);

    return true;
}

uint32_t XINTERFACE::AttributeChanged(ATTRIBUTES *patr)
{
    if (patr != nullptr && patr->GetParent() != nullptr && patr->GetParent()->GetParent() != nullptr)
    {
        const char *sParentName = patr->GetParent()->GetParent()->GetThisName();
        if (sParentName == nullptr || !storm::iEquals(sParentName, "pictures"))
            return 0;
        const char *sImageName = patr->GetParent()->GetThisName();
        if (sImageName == nullptr)
            return 0;
        // find this picture
        IMAGE_Entity *pImList = m_imgLists;
        while (pImList != nullptr)
        {
            if (pImList->sImageName != nullptr && storm::iEquals(pImList->sImageName, sImageName))
                break;
            pImList = pImList->next;
        }
        // no this picture / create new
        if (pImList == nullptr)
        {
            pImList = new IMAGE_Entity;
            if (pImList == nullptr)
            {
                throw std::runtime_error("Allocation memory error");
            }
            *pImList = {};
            const auto len = strlen(sImageName) + 1;
            if ((pImList->sImageName = new char[len]) == nullptr)
            {
                throw std::runtime_error("Allocate memory error");
            }
            memcpy(pImList->sImageName, sImageName, len);
            // insert that into images list
            pImList->next = m_imgLists;
            m_imgLists = pImList;
        }
        if (patr->GetThisName() == nullptr)
            return 0;
        // set picture
        if (storm::iEquals(patr->GetThisName(), "pic"))
        {
            STORM_DELETE(pImList->sPicture);
            if (patr->HasValue())
            {
                const auto len = strlen(patr->GetThisAttr()) + 1;
                if ((pImList->sPicture = new char[len]) == nullptr)
                {
                    throw std::runtime_error("Allocate memory error");
                }
                memcpy(pImList->sPicture, patr->GetThisAttr(), len);
            }
            if (pImList->sImageListName == nullptr)
                return 0;
            pImList->imageID = pPictureService->GetImageNum(pImList->sImageListName, pImList->sPicture);
        }
        // set texture
        if (storm::iEquals(patr->GetThisName(), "tex"))
        {
            if (pImList->sImageListName != nullptr)
                pPictureService->ReleaseTextureID(pImList->sImageListName);
            STORM_DELETE(pImList->sImageListName);
            if (patr->HasValue())
            {
                const auto len = strlen(patr->GetThisAttr()) + 1;
                if ((pImList->sImageListName = new char[len]) == nullptr)
                {
                    throw std::runtime_error("Allocate memory error");
                }
                memcpy(pImList->sImageListName, patr->GetThisAttr(), len);
            }
            pImList->idTexture = pPictureService->GetTextureID(pImList->sImageListName);
            pImList->imageID = pPictureService->GetImageNum(pImList->sImageListName, pImList->sPicture);
        }
    }
    return 0;
}

bool XINTERFACE::SFLB_DoSaveFileData(const char *saveName, const char *saveData) const
{
    if (saveName == nullptr || saveData == nullptr)
        return false;
    const int32_t slen = strlen(saveData) + 1;
    if (slen <= 1)
        return false;

    entid_t ei;
    if (!(ei = core.GetEntityId("SCRSHOTER")))
        return false;
    int32_t textureId = core.Send_Message(ei, "l", MSG_SCRSHOT_MAKE);
    if (textureId == -1)
        return false;

    auto *pTex = static_cast<IDirect3DTexture9 *>(pRenderService->GetTextureFromID(textureId));

    D3DSURFACE_DESC dscr;
    pTex->GetLevelDesc(0, &dscr);

    auto pdat = static_cast<char *>(malloc(sizeof(SAVE_DATA_HANDLE) + slen));
    if (pdat == nullptr)
    {
        throw std::runtime_error("allocate memory error");
    }

    ((SAVE_DATA_HANDLE *)pdat)->StringDataSize = slen;
    // if(slen>0)
    memcpy(&pdat[sizeof(SAVE_DATA_HANDLE)], saveData, slen);

    int32_t ssize = 0;
    if (dscr.Height > 0)
    {
        D3DLOCKED_RECT lockRect;
        if (pTex->LockRect(0, &lockRect, nullptr, 0) == D3D_OK)
        {
            ssize = lockRect.Pitch * dscr.Height;
            pdat = static_cast<char *>(realloc(pdat, sizeof(SAVE_DATA_HANDLE) + slen + ssize));
            ((SAVE_DATA_HANDLE *)pdat)->SurfaceDataSize = ssize;
            memcpy(&pdat[sizeof(SAVE_DATA_HANDLE) + slen], lockRect.pBits, ssize);
            pTex->UnlockRect(0);
        }
        else
            core.Trace("Can`t lock screenshot texture");
    }

    core.SetSaveData(saveName, pdat, sizeof(SAVE_DATA_HANDLE) + slen + ssize);
    free(pdat);
    return true;
}

bool XINTERFACE::SFLB_GetSaveFileData(const char *saveName, int32_t bufSize, char *buf)
{
    if (buf == nullptr || bufSize <= 0)
        return false;
    int32_t allDatSize = 0;
    auto pdat = static_cast<char *>(core.GetSaveData(saveName, allDatSize));
    if (pdat == nullptr)
        return false;

    char *stringData = &pdat[sizeof(SAVE_DATA_HANDLE)];
    if (!utf8::IsValidUtf8(stringData))
    {
        utf8::FixInvalidUtf8(stringData);
    }

    int32_t strSize = ((SAVE_DATA_HANDLE *)pdat)->StringDataSize;
    if (strSize >= bufSize)
    {
        buf[bufSize - 1] = 0;
        strSize = bufSize - 1;
    }
    if (strSize > 0)
        memcpy(buf, stringData, strSize);

    delete pdat;
    return strSize > 0;
}

void XINTERFACE::ReleaseSaveFindList()
{
    while (m_pSaveFindRoot)
    {
        SAVE_FIND_DATA *p = m_pSaveFindRoot;
        m_pSaveFindRoot = m_pSaveFindRoot->next;
        STORM_DELETE(p->save_file_name);
        STORM_DELETE(p);
    }
}

void XINTERFACE::AddFindData(std::filesystem::path filePath)
{
    auto *p = new SAVE_FIND_DATA;
    if (p)
    {
        const auto sSaveFileName = filePath.filename().u8string();
        p->time = std::filesystem::last_write_time(filePath);
        p->file_size = 0; // original code always passed 0 to file_size
        p->next = m_pSaveFindRoot;
        m_pSaveFindRoot = p;
        const auto len = sSaveFileName.size() + 1;
        p->save_file_name = new char[len];
        if (p->save_file_name)
        {
            memcpy(p->save_file_name, sSaveFileName.c_str(), len);
        }
    }
}

void XINTERFACE::Sorting_FindData()
{
    if (!m_pSaveFindRoot)
    {
        return; // do nothing (empty list)
    }
    bool bMakeSorting = true;
    while (bMakeSorting)
    {
        bMakeSorting = false;
        SAVE_FIND_DATA *pprev = nullptr;
        for (SAVE_FIND_DATA *pcur = m_pSaveFindRoot; pcur->next; pcur = pcur->next)
        {
            if (pcur->next->time > pcur->time)
            {
                bMakeSorting = true;
                SAVE_FIND_DATA *p1 = pcur;
                SAVE_FIND_DATA *p2 = pcur->next;
                p1->next = p2->next;
                p2->next = p1;
                if (pprev)
                    pprev->next = p2;
                else
                    m_pSaveFindRoot = p2;
                pcur = p2;
            }
            pprev = pcur;
        }
    }
}

XINTERFACE::SAVE_FIND_DATA *XINTERFACE::GetSaveDataByIndex(int n) const
{
    SAVE_FIND_DATA *p = m_pSaveFindRoot;
    for (int i = 0; i < n; i++)
        if (p)
            p = p->next;
    return p;
}

char *XINTERFACE::SaveFileFind(int32_t saveNum, char *buffer, size_t bufSize, int32_t &fileSize)
{
    if (!m_pSaveFindRoot) // create save file list
    {
        // get file name for searching (whith full path)
        const char *sSavePath = AttributesPointer->GetAttribute("SavePath");
        if (sSavePath != nullptr)
        {
            fio->_CreateDirectory(sSavePath);
        }

        // start save file finding
        const auto vFilePaths = fio->_GetFsPathsByMask(sSavePath, nullptr, true);
        for (std::filesystem::path filePath : vFilePaths)
        {
            AddFindData(filePath);
        }
        // common part
        Sorting_FindData();
    }

    // default setting for finded file
    fileSize = 0;
    if (buffer)
        buffer[0] = 0;
    // get data for save file by his index
    SAVE_FIND_DATA *pSFD = GetSaveDataByIndex(saveNum);
    if (!pSFD)
    {
        ReleaseSaveFindList();
        return nullptr;
    }
    fileSize = pSFD->file_size;
    if (pSFD->save_file_name)
    {
        int q = strlen(pSFD->save_file_name);
        if (q >= static_cast<int>(bufSize))
            q = bufSize - 1;
        if (q > 0)
            strncpy_s(buffer, bufSize, pSFD->save_file_name, q);
        if (q >= 0)
            buffer[q] = 0;
    }
    return buffer;
}

bool XINTERFACE::NewSaveFileName(const char *fileName) const
{
    if (fileName == nullptr)
    {
        return false;
    }

    char param[256];
    const char *sSavePath = AttributesPointer->GetAttribute("SavePath");

    if (sSavePath == nullptr)
    {
        sprintf(param, "%s", fileName);
    }
    else
    {
        sprintf(param, "%s\\%s", sSavePath, fileName);
    }

    return !(fio->_FileOrDirectoryExists(param));
}

void XINTERFACE::DeleteSaveFile(const char *fileName)
{
    if (fileName == nullptr)
    {
        return;
    }
    char param[256];
    const char *sSavePath = AttributesPointer->GetAttribute("SavePath");
    if (sSavePath == nullptr)
    {
        sprintf(param, "%s", fileName);
    }
    else
    {
        sprintf(param, "%s\\%s", sSavePath, fileName);
    }
    fio->_DeleteFile(param);
}

uint32_t XINTERFACE_BASE::GetBlendColor(uint32_t minCol, uint32_t maxCol, float fBlendFactor)
{
    int32_t ad = static_cast<int32_t>(ALPHA(maxCol)) - static_cast<int32_t>(ALPHA(minCol));
    int32_t rd = static_cast<int32_t>(RED(maxCol)) - static_cast<int32_t>(RED(minCol));
    int32_t gd = static_cast<int32_t>(GREEN(maxCol)) - static_cast<int32_t>(GREEN(minCol));
    int32_t bd = static_cast<int32_t>(BLUE(maxCol)) - static_cast<int32_t>(BLUE(minCol));
    ad = ALPHA(minCol) + static_cast<int32_t>(ad * fBlendFactor);
    rd = RED(minCol) + static_cast<int32_t>(rd * fBlendFactor);
    gd = GREEN(minCol) + static_cast<int32_t>(gd * fBlendFactor);
    bd = BLUE(minCol) + static_cast<int32_t>(bd * fBlendFactor);
    return ARGB(ad, rd, gd, bd);
}

void XINTERFACE::AddNodeToList(CINODE *nod, int32_t priority)
{
    if (nod == nullptr)
        return;
    if (m_pNodes == nullptr || m_pNodes->GetPriority() > priority)
    {
        nod->m_next = m_pNodes;
        m_pNodes = nod;
        return;
    }
    CINODE *pnod;
    for (pnod = m_pNodes; pnod->m_next != nullptr; pnod = pnod->m_next)
        if (pnod->GetPriority() <= priority && pnod->m_next->GetPriority() > priority)
            break;
    nod->m_next = pnod->m_next;
    pnod->m_next = nod;
}

void XINTERFACE::SetExclusiveNode(CINODE *nod)
{
    g_bIExclusiveMode = true;
    for (CINODE *pnod = m_pNodes; pnod != nullptr; pnod = pnod->m_next)
        pnod->m_bLockedNode = true;
    if (nod != nullptr)
        nod->m_bLockedNode = false;
}

void XINTERFACE::AddExclusiveNode(CINODE *nod)
{
    if (nod != nullptr)
        nod->m_bLockedNode = false;
}

void XINTERFACE::ExitFromExclusive()
{
    g_bIExclusiveMode = false;
    for (CINODE *pnod = m_pNodes; pnod != nullptr; pnod = pnod->m_next)
        pnod->m_bLockedNode = false;
}

void XINTERFACE::ReleaseDinamicPic(const char *sPicName)
{
    if (sPicName == nullptr)
        return;

    IMAGE_Entity *prevImg = nullptr;
    IMAGE_Entity *findImg;
    for (findImg = m_imgLists; findImg != nullptr; findImg = findImg->next)
    {
        if (findImg->sImageName != nullptr && storm::iEquals(findImg->sImageName, sPicName))
            break;
        prevImg = findImg;
    }
    if (findImg == nullptr)
        return;

    PICTURE_TEXTURE_RELEASE(pPictureService, findImg->sImageListName, findImg->idTexture);
    STORM_DELETE(findImg->sImageListName);
    STORM_DELETE(findImg->sImageName);
    STORM_DELETE(findImg->sPicture);
    if (prevImg == nullptr)
        m_imgLists = findImg->next;
    else
        prevImg->next = findImg->next;
}

int32_t FindMaxStrForWidth(VDX9RENDER *pVR, int nW, char *str, int nFontID, float fScale)
{
    if (!pVR || !str || str[0] == '\0')
        return 0;
    int nPrev = -1;
    for (int curLen = strlen(str); curLen >= 0; curLen--)
    {
        if (str[curLen] == 32 || curLen == 0)
        {
            int nFirst = curLen;
            if (curLen != 0)
                nFirst++;
            if (pVR->StringWidth(&str[nFirst], nFontID, fScale) < nW)
            {
                nPrev = nFirst;
            }
            else
            {
                if (nPrev == -1)
                    nPrev = nFirst;
                break;
            }
            if (curLen == 0)
                break;
        }
    }
    return nPrev;
}

int32_t XINTERFACE::PrintIntoWindow(int32_t wl, int32_t wr, int32_t idFont, uint32_t dwFCol, uint32_t dwBCol, int32_t align,
                                 bool shadow, float scale, int32_t sxs, int32_t sys, int32_t left, int32_t top, const char *str,
                                 int nWidthForScaleCorrecting, int nSplit)
{
    if (!str)
        return 0;
    int32_t strWidth = pRenderService->StringWidth(str, idFont, scale);

    // check fontScale
    if (nWidthForScaleCorrecting > 0 && strWidth > nWidthForScaleCorrecting)
    {
        if (nSplit == 0)
        {
            // Do not split the line, but reduce the font scale
            scale *= static_cast<float>(nWidthForScaleCorrecting) / strWidth;
            strWidth = pRenderService->StringWidth(str, idFont, scale);
        }
        else
        {
            // Split the line with advancement by a given number
            int maxWidth = 0;
            const int nPrev = -1;
            char strLocTmp[1024];
            strcpy_s(strLocTmp, str);
            while (true) //~!~
            {
                int nStart = FindMaxStrForWidth(pRenderService, nWidthForScaleCorrecting, strLocTmp, idFont, scale);
                const int curWidth = PrintIntoWindow(wl, wr, idFont, dwFCol, dwBCol, align, shadow, scale, sxs, sys,
                                                     left, top, &strLocTmp[nStart]);
                if (curWidth > maxWidth)
                    maxWidth = curWidth;
                if (nStart == 0)
                    break;
                while (nStart > 0 && strLocTmp[nStart - 1] == 0x20)
                    nStart--;
                strLocTmp[nStart] = '\0';
                top += nSplit;
            }
            return maxWidth;
        }
    }

    int32_t strLeft = left;
    if (align == PR_ALIGN_RIGHT)
        strLeft = left - strWidth;
    if (align == PR_ALIGN_CENTER)
        strLeft = left - strWidth / 2;
    int32_t strRight = strLeft + strWidth;

    if (strLeft >= wl && strRight <= wr)
    {
        return pRenderService->ExtPrint(idFont, dwFCol, dwBCol, align, shadow, scale, sxs, sys, left, top, "%s", str);
    }

    char *newStr = const_cast<char *>(str); // TODO: Rewrite this code so the string is not (temporarily) modified
    // cut on the left
    while (strLeft < wl && newStr != nullptr && newStr[0] != 0)
    {
        newStr += utf8::u8_inc(newStr);
        strWidth = pRenderService->StringWidth(newStr, idFont, scale);
        strLeft = strRight - strWidth;
    }
    // cut on the right
    if (newStr != nullptr)
    {
        strRight = strLeft + strWidth;
        int32_t nEnd = strlen(newStr);
        while (nEnd > 0 && strRight > wr)
        {
            nEnd -= utf8::u8_dec(newStr + nEnd);
            const char chOld = newStr[nEnd];
            newStr[nEnd] = 0;
            strWidth = pRenderService->StringWidth(newStr, idFont, scale);
            newStr[nEnd] = chOld;
            strRight = strLeft + strWidth;
        }
        if (nEnd > 0)
        {
            const char chOld = newStr[nEnd];
            newStr[nEnd] = 0;
            strWidth = pRenderService->ExtPrint(idFont, dwFCol, dwBCol, PR_ALIGN_LEFT, shadow, scale, sxs, sys, strLeft,
                                                top, "%s", newStr);
            newStr[nEnd] = chOld;
            return strWidth;
        }
        return 0;
    }

    return pRenderService->ExtPrint(idFont, dwFCol, dwBCol, PR_ALIGN_LEFT, shadow, scale, sxs, sys, strLeft, top, "%s",
                                    newStr);
}

void XINTERFACE::IncrementGameTime(uint32_t dwDeltaTime)
{
    bool bYesChange = false;
    m_dwCurDeltaTime += dwDeltaTime;

    while (m_dwCurDeltaTime >= 1000)
    {
        m_dwGameTimeSec++;
        m_dwCurDeltaTime -= 1000;
        bYesChange = true;
    }

    while (m_dwGameTimeSec >= 60)
    {
        m_dwGameTimeMin++;
        m_dwGameTimeSec -= 60;
    }

    while (m_dwGameTimeMin >= 60)
    {
        m_dwGameTimeHour++;
        m_dwGameTimeMin -= 60;
    }

    if (bYesChange && m_bUse)
        core.Event("ievent_SetGameTime", "lll", m_dwGameTimeHour, m_dwGameTimeMin, m_dwGameTimeSec);
}

char *AddAttributesStringsToBuffer(char *inBuffer, char *prevStr, ATTRIBUTES *pAttr)
{
    size_t prevLen = 0;
    if (prevStr != nullptr)
        prevLen = strlen(prevStr) + std::size(".") - 1;

    const int q = pAttr->GetAttributesNum();
    for (int k = 0; k < q; k++)
    {
        ATTRIBUTES *pA = pAttr->GetAttributeClass(k);
        if (pA == nullptr)
            continue;

        const char *attrVal = pA->GetThisAttr();
        const char *attrName = pA->GetThisName();
        if (attrName && attrVal && attrVal[0])
        {
            int nadd = std::size("\n") - 1 + strlen(attrName) + std::size("=") - 1 + strlen(attrVal) + 1;
            nadd += prevLen;
            if (inBuffer != nullptr)
                nadd += strlen(inBuffer);

            auto pNew = new char[nadd];
            if (pNew == nullptr)
                continue;
            pNew[0] = 0;

            if (inBuffer)
            {
                strcat_s(pNew, nadd, inBuffer);
                strcat_s(pNew, nadd, "\n");
            }

            if (prevStr)
            {
                strcat_s(pNew, nadd, prevStr);
                strcat_s(pNew, nadd, ".");
            }

            strcat_s(pNew, nadd, attrName);
            strcat_s(pNew, nadd, "=");
            strcat_s(pNew, nadd, attrVal);

            delete inBuffer;
            inBuffer = pNew;
        }

        if (pA->GetAttributesNum() != 0)
        {
            char param[512];
            param[0] = 0;
            if (prevStr)
                sprintf_s(param, "%s.", prevStr);
            strcat_s(param, pA->GetThisName());
            inBuffer = AddAttributesStringsToBuffer(inBuffer, param, pA);
        }
    }

    return inBuffer;
}

void XINTERFACE::SaveOptionsFile(const char *fileName, ATTRIBUTES *pAttr)
{
    char FullPath[MAX_PATH];

    if (fileName == nullptr || pAttr == nullptr)
    {
        return;
    }
    strcpy_s(FullPath, fileName);

    PrecreateDirForFile(FullPath);
    auto fileS = fio->_CreateFile(FullPath, std::ios::binary | std::ios::out);
    if (!fileS.is_open())
    {
        return;
    }

    char *pOutBuffer = nullptr;

    if (pAttr)
    {
        pOutBuffer = AddAttributesStringsToBuffer(nullptr, nullptr, pAttr);
    }

    if (pOutBuffer)
    {
        fio->_WriteFile(fileS, pOutBuffer, strlen(pOutBuffer));
        delete pOutBuffer;
    }
    fio->_CloseFile(fileS);
}

void XINTERFACE::LoadOptionsFile(std::string_view fileName, ATTRIBUTES *pAttr)
{
    constexpr const unsigned int OPTION_NAME_MAX_LENGTH = 512;
    constexpr const unsigned int OPTION_VALUE_MAX_LENGTH = 1024;

    if (fileName.empty() || pAttr == nullptr)
    {
        return;
    }

    auto fileS = fio->_CreateFile(fileName.data(), std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        return;
    }

    const uint32_t fileSize = fio->_GetFileSize(fileName.data());
    if (fileSize == 0)
    {
        core.Event("evntOptionsBreak");
        fio->_CloseFile(fileS);
        return;
    }

    std::string buffer(fileSize + 1, '\0'); // + 1 for '\0'
    fio->_ReadFile(fileS, buffer.data(), fileSize);
    if (pAttr) //~!~
    {
        storm::removeCarriageReturn(buffer);
        storm::parseOptions(buffer, *pAttr);
    }

    fio->_CloseFile(fileS);
}

void XINTERFACE::GetContextHelpData()
{
    ReleaseContextHelpData();

    const char *texName = nullptr;

    if (m_pCurNode)
    {
        texName = m_pCurNode->m_strHelpTextureFile;
        m_frectHelpTextureUV = m_pCurNode->m_frectHelpTextureUV;
        if (texName == nullptr)
        {
            VDATA *pvdat = core.Event("ievntGetHelpTexture", "s", m_pCurNode->m_nodeName);
            if (pvdat != nullptr)
                pvdat->Get(texName);
            if (texName != nullptr && strlen(texName) < 1)
                texName = nullptr;
        }
    }
    if (!texName)
    {
        texName = m_strDefHelpTextureFile;
        m_frectHelpTextureUV = m_frectDefHelpTextureUV;
    }

    if (texName != nullptr)
        m_idHelpTexture = pRenderService->TextureCreate(texName);
    if (m_idHelpTexture != -1)
        m_nInterfaceMode = CONTEXTHELP_IMODE;
}

void XINTERFACE::ReleaseContextHelpData()
{
    TEXTURE_RELEASE(pRenderService, m_idHelpTexture);
    m_nInterfaceMode = DEFAULT_IMODE;
}

void XINTERFACE::ShowContextHelp()
{
    if (m_nInterfaceMode != CONTEXTHELP_IMODE)
        return;
    if (m_idHelpTexture == -1)
        return;

    // show picture
    XI_ONLYONETEX_VERTEX pV[4];
    for (int i = 0; i < 4; i++)
        pV[i].pos.z = 1.f;

    uint32_t oldTFactor;
    pRenderService->GetRenderState(D3DRS_TEXTUREFACTOR, &oldTFactor);

    pRenderService->TextureSet(0, m_idHelpTexture);

    pV[0].pos.x = pV[2].pos.x = 0; //(float)GlobalRect.left;
    pV[0].tu = pV[2].tu = m_frectHelpTextureUV.left;

    pV[1].pos.x = pV[3].pos.x = static_cast<float>(dwScreenWidth); //(float)GlobalRect.right;
    pV[1].tu = pV[3].tu = m_frectHelpTextureUV.right;

    pV[0].pos.y = pV[1].pos.y = 0; //(float)GlobalRect.top;
    pV[0].tv = pV[1].tv = m_frectHelpTextureUV.top;

    pV[2].pos.y = pV[3].pos.y = static_cast<float>(dwScreenHeight); //(float)GlobalRect.bottom;
    pV[2].tv = pV[3].tv = m_frectHelpTextureUV.bottom;

    pRenderService->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwContHelpColor);
    pRenderService->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONLYONETEX_FVF, 2, pV, sizeof(XI_ONLYONETEX_VERTEX),
                                    "iBlindPictures");
    pRenderService->SetRenderState(D3DRS_TEXTUREFACTOR, oldTFactor);
}

int XINTERFACE::LoadIsExist()
{
    char *sCurLngName = GetStringService()->GetLanguage();
    if (sCurLngName == nullptr)
    {
        return 0;
    }

    char param[1024];
    const char *sSavePath = AttributesPointer->GetAttribute("SavePath");
    if (sSavePath != nullptr)
    {
        fio->_CreateDirectory(sSavePath);
    }

    bool bFindFile = false;
    const auto vFilenames = fio->_GetPathsOrFilenamesByMask(sSavePath, "*", true);
    for (std::string path : vFilenames)
    {
        char datBuf[512];
        sprintf(param, path.c_str());
        if (SFLB_GetSaveFileData(param, sizeof(datBuf), datBuf))
        {
            int nLen = strlen(datBuf);
            int i;
            for (i = strlen(datBuf); i >= 0 && datBuf[i] != '@'; i--)
                ;
            if (i < 0)
            {
                i = 0;
            }
            if (datBuf[i] == '@')
            {
                i++;
            }
            if (storm::iEquals(sCurLngName, &datBuf[i]))
            {
                bFindFile = true;
                break;
            }
        }
    }
    return bFindFile ? 1 : 0;
}

void XINTERFACE::PrecreateDirForFile(const char *pcFullFileName)
{
    if (!pcFullFileName)
        return;
    char path[MAX_PATH];
    sprintf_s(path, sizeof(path), "%s", pcFullFileName);
    int32_t n;
    for (n = strlen(pcFullFileName) - 1; n > 0; n--)
        if (path[n] == '\\')
        {
            path[n] = 0;
            break;
        }
    if (n > 0)
        fio->_CreateDirectory(path);
}

// controls Container
CONTROLS_CONTAINER::CONTROLS_CONTAINER()
{
    pContainers = nullptr;
}

CONTROLS_CONTAINER::~CONTROLS_CONTAINER()
{
    CONTEINER_DESCR *pCont = pContainers;
    while (pContainers)
    {
        CONTEINER_DESCR *pCont = pContainers;
        pContainers = pContainers->next;

        while (pCont->pControls)
        {
            CONTEINER_DESCR::CONTROL_DESCR *pCntrl = pCont->pControls;
            pCont->pControls = pCont->pControls->next;
            STORM_DELETE(pCntrl->controlName);
            STORM_DELETE(pCntrl);
        }

        STORM_DELETE(pCont->resultName);

        STORM_DELETE(pCont);
    }
}

bool CONTROLS_CONTAINER::Init()
{
    if (AttributesPointer != nullptr)
        return CreateConteinerList(AttributesPointer);
    return false;
}

void CONTROLS_CONTAINER::Execute(uint32_t delta_time)
{
    CONTEINER_DESCR *pCont = pContainers;
    while (pCont)
    {
        CONTEINER_DESCR::CONTROL_DESCR *pDescr = pCont->pControls;
        CONTROL_STATE cs;
        CONTROL_STATE csPrev;
        cs.state = CST_INACTIVE;
        cs.fValue = 0.f;
        cs.lValue = 0;
        core.Controls->GetControlState(pCont->resultName, csPrev);
        while (pDescr)
        {
            if (pDescr->controlName)
            {
                CONTROL_STATE insideCS;
                core.Controls->GetControlState(pDescr->controlName, insideCS);

                switch (insideCS.state) //~!~
                {
                case CST_ACTIVATED:
                    if (csPrev.state != CST_ACTIVE && csPrev.state != CST_ACTIVATED)
                        cs.state = CST_ACTIVATED;
                    else
                        cs.state = CST_ACTIVE;
                    break;

                case CST_INACTIVATED:
                    if (cs.state != CST_ACTIVE && cs.state != CST_ACTIVATED)
                    {
                        if (csPrev.state != CST_INACTIVE && csPrev.state != CST_INACTIVATED)
                            cs.state = CST_INACTIVATED;
                        else
                            cs.state = CST_INACTIVE;
                    }
                    break;

                case CST_ACTIVE:
                    cs.state = CST_ACTIVE;
                    break;
                }

                float fVal = insideCS.fValue * pDescr->fValLimit;
                if (fVal == 0.f)
                    fVal = insideCS.lValue * pDescr->fValLimit;
                if (cs.fValue < fVal)
                    cs.fValue = fVal;

                const int32_t lVal = insideCS.lValue;
                if (cs.lValue < lVal)
                    cs.lValue = lVal;
            }
            pDescr = pDescr->next;
        }
        //debug code
        /*  if (storm::iEquals(pCont->resultName, "ChrTurnH1"))
            if (cs.state != CST_INACTIVE)
                cs.state = cs.state;
        */
        core.Controls->SetControlState(pCont->resultName, cs);
        core.Controls->GetControlState(pCont->resultName, csPrev);
        pCont = pCont->next;
    }
}

uint64_t CONTROLS_CONTAINER::ProcessMessage(MESSAGE &message)
{
    return 0;
}

bool CONTROLS_CONTAINER::CreateConteinerList(ATTRIBUTES *pA)
{
    if (!pA)
        return false;

    const int q = pA->GetAttributesNum();
    for (int i = 0; i < q; i++)
    {
        ATTRIBUTES *pAttr = pA->GetAttributeClass(i);
        if (!pAttr)
            continue;
        const char *containerName = pAttr->GetThisName();
        if (!containerName)
            continue;
        AddContainer(containerName);
        SetContainerLimitVal(containerName, static_cast<float>(atof(pAttr->GetThisAttr())));

        const int cntSize = pAttr->GetAttributesNum();
        for (int n = 0; n < cntSize; n++)
        {
            AddControlsToContainer(containerName, pAttr->GetAttributeName(n),
                                   static_cast<float>(atof(pAttr->GetAttribute(n))));
        }
    }

    return true;
}

void CONTROLS_CONTAINER::AddContainer(const char *container)
{
    if (!container)
        return;

    CONTEINER_DESCR *pCont = FindContainer(container);
    if (pCont)
        return;

    pCont = pContainers;
    pContainers = new CONTEINER_DESCR;
    if (!pContainers)
        throw std::runtime_error("allocate memory error");
    pContainers->fMaxVal = 1.f;
    pContainers->pControls = nullptr;
    pContainers->next = pCont;

    const auto len = strlen(container) + 1;
    pContainers->resultName = new char[len];
    if (!pContainers->resultName)
        throw std::runtime_error("allocate memory error");
    memcpy(pContainers->resultName, container, len);
}

void CONTROLS_CONTAINER::SetContainerLimitVal(const char *container, float fLimitVal)
{
    CONTEINER_DESCR *pCont = FindContainer(container);
    if (!pCont)
        return;
    pCont->fMaxVal = fLimitVal;
}

void CONTROLS_CONTAINER::AddControlsToContainer(const char *container, const char *controlName, float fValLimit)
{
    if (!container || !controlName)
        return;
    CONTEINER_DESCR *pCont = FindContainer(container);

    CONTEINER_DESCR::CONTROL_DESCR *pCtrl = pCont->FindControl(controlName);
    if (pCtrl)
        return;

    pCtrl = pCont->pControls;
    pCont->pControls = new CONTEINER_DESCR::CONTROL_DESCR;
    if (!pCont->pControls)
        throw std::runtime_error("allocate memory error");

    pCont->pControls->fValLimit = fValLimit;
    pCont->pControls->next = pCtrl;

    const auto len = strlen(controlName) + 1;
    pCont->pControls->controlName = new char[len];
    if (!pCont->pControls->controlName)
        throw std::runtime_error("allocate memory error");
    memcpy(pCont->pControls->controlName, controlName, len);
}

CONTROLS_CONTAINER::CONTEINER_DESCR *CONTROLS_CONTAINER::FindContainer(const char *sContainer)
{
    if (!sContainer)
        return nullptr;
    CONTEINER_DESCR *pCont = pContainers;
    while (pCont)
        if (pCont->resultName != nullptr && storm::iEquals(pCont->resultName, sContainer))
            return pCont;
    return nullptr;
}

CONTROLS_CONTAINER::CONTEINER_DESCR::CONTROL_DESCR *CONTROLS_CONTAINER::CONTEINER_DESCR::FindControl(
    const char *cntrlName)
{
    if (!cntrlName)
        return nullptr;

    CONTROL_DESCR *pCtrl = pControls;
    while (pCtrl)
    {
        if (pCtrl->controlName && storm::iEquals(pCtrl->controlName, cntrlName))
            return pCtrl;
        pCtrl = pCtrl->next;
    }

    return nullptr;
}
