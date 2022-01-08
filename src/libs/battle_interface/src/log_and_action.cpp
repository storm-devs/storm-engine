#include "log_and_action.h"
#include "controls.h"
#include "core.h"
#include "message.h"
#include "shared/battle_interface/log_msg.h"
#include <stdio.h>

static entid_t g_ILogAndActions;

#define BI_FAST_COMMANDS "BIFastCommand"

//#define SPECIAL_VERSION

void CalculateTexturePos(FRECT &texRect, int hort, int vert, int numt)
{
    const auto vn = numt / hort;
    const auto hn = numt - vn * hort;
    texRect.right = (texRect.left = static_cast<float>(hn) / hort) + 1.f / hort;
    texRect.bottom = (texRect.top = static_cast<float>(vn) / vert) + 1.f / vert;
}

ILogAndActions::ILogAndActions()
{
    rs = nullptr;
    m_idIconTexture = -1;
    m_sRoot = nullptr;
    m_fontID = -1;
    m_fFontScale = 1.f;
    m_bShowActiveCommand = false;
    m_bShowLogStrings = false;
    m_bThatRealAction = false;
    m_nStringBegin = 0;
    m_bDontShowAll = false;
    m_sOldActionName[0] = 0;
    m_nTimeCounter = 0;
}

ILogAndActions::~ILogAndActions()
{
    Release();
}

bool ILogAndActions::Init()
{
    if ((rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"))) == nullptr)
    {
        throw std::runtime_error("Can`t create render service");
    }
    D3DVIEWPORT9 vp;
    rs->GetViewport(&vp);
    core.Event("SetWindowSize", "lll", vp.Width, vp.Height, false);
    g_ILogAndActions = GetId();
    return true;
}

void ILogAndActions::Execute(uint32_t delta_time)
{
    if (m_bDontShowAll)
        return;

    CONTROL_STATE cs;
    core.Controls->GetControlState(BI_FAST_COMMANDS, cs);
    if (cs.state == CST_ACTIVATED)
        core.Event("BI_FastCommand", "s", m_sActionName);

    // fade out lines
    const auto colDelta = delta_time * m_fBlendSpeed;
    STRING_DESCR *prev_sd = nullptr;
    STRING_DESCR *sd;
    for (sd = m_sRoot; sd != nullptr;)
    {
        if (sd->alpha <= 255.f)
            if ((sd->alpha -= colDelta) <= 0)
            {
                if (prev_sd == nullptr)
                    m_sRoot = sd->next;
                else
                    prev_sd->next = sd->next;
                STORM_DELETE(sd->str);
                STORM_DELETE(sd);
                sd = m_sRoot;
                continue;
            }
        prev_sd = sd;
        sd = sd->next;
    }

    // move lines to free positions
    const auto delta = delta_time * m_fShiftSpeed;
    auto top = 0.f;
    for (sd = m_sRoot; sd != nullptr; sd = sd->next)
    {
        if (sd->offset > top)
        {
            sd->offset -= delta;
            if (sd->offset < top)
                sd->offset = top;
        }
        top += m_nStringOffset;
    }
}

uint64_t ILogAndActions::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case LOG_ADD_STRING: {
        const auto stringImmortal = message.Long() != 0;
        const std::string &param = message.String();
        if (stringImmortal)
        {
            // find the last element of the list
            STRING_DESCR *last;
            for (last = m_sRoot; last != nullptr; last = last->next)
                if (last->alpha > 255.f)
                    break;
            if (last == nullptr)
                SetString(param.c_str(), true);
            else
            {
                STORM_DELETE(last->str);
                if (param[0] != 0)
                {
                    const auto len = param.size() + 1;
                    if ((last->str = new char[len]) == nullptr)
                    {
                        throw std::runtime_error("allocate memory error");
                    }
                    strcpy_s(last->str, len, param.c_str());
                }
                else
                {
                    if (last == m_sRoot)
                        m_sRoot = m_sRoot->next;
                    else
                    {
                        for (auto *prev = m_sRoot; prev != nullptr && prev->next != last; prev = prev->next)
                            if (prev != nullptr && prev->next == last) //~!~
                            {
                                prev->next = last->next;
                                break;
                            }
                    }
                    STORM_DELETE(last);
                }
            }
        }
        else
            SetString(param.c_str(), false);
    }
    break;
    case LOG_SET_ACTIVE_ACTION: {
        const std::string &param = message.String();
        SetAction(param.c_str());
    }
    break;
    case LOG_AND_ACTIONS_INIT: {
        m_sOldActionName[0] = '0';
        const auto bfc = message.Long();
        const auto bls = message.Long();
        Create(bfc != 0, bls != 0);
    }
    break;
    case LOG_AND_ACTIONS_CHANGE: {
        m_sOldActionName[0] = '0';
        const auto bfc = message.Long();
        const auto bls = message.Long();
        ActionChange(bfc != 0, bls != 0);
    }
    break;
    case LI_SET_VISIBLE:
        m_bDontShowAll = (message.Long() == 0);
        break;
    case LI_CLEAR_STRINGS:
        while (m_sRoot != nullptr)
        {
            auto *const p = m_sRoot;
            m_sRoot = p->next;
            STORM_DELETE(p->str);
            delete p;
        }
        break;
    case LI_OTHER_MSG: {
        const std::string &param = message.String();
        if (storm::iEquals(param, "SetTimeScale"))
        {
            core.SetTimeScale(message.Float());
        }
    }
    break;
    }
    return 0;
}

void ILogAndActions::Realize(uint32_t delta_time)
{
#ifdef SPECIAL_VERSION
    if (core.Controls->GetDebugAsyncKeyState(VK_F8) >= 0)
    {
        m_nTimeCounter += core.GetDeltaTime();
        if (m_nTimeCounter > 10000)
        {
            m_nTimeCounter = 0;
        }
        int32_t nA = 0;
        if (m_nTimeCounter < 500)
        {
            nA = (int32_t)(255.f * m_nTimeCounter / 500.f);
        }
        else if (m_nTimeCounter < 2000)
        {
            nA = 255;
        }
        else if (m_nTimeCounter < 2500)
        {
            nA = (int32_t)(255.f * (2500 - m_nTimeCounter) / 500.f);
        }
        rs->ExtPrint(m_fontID, ARGB(nA, 255, 255, 255), 0, PR_ALIGN_CENTER, false, 3.9f, 800, 600, 400, 300,
                     "¬≈–—»я ƒЋя ѕ–≈——џ");
    }
#endif
    if (core.Controls->GetDebugAsyncKeyState('K') < 0)
        return;
    if (rs == nullptr)
        return;
    if (m_bDontShowAll)
        return;

    rs->MakePostProcess();

    // Show Active Action
    //---------------------
    if (m_bShowActiveCommand)
    {
        CMatrix matw;
        rs->SetTransform(D3DTS_WORLD, (D3DXMATRIX *)&matw);
        // show icon
        if ((m_idIconTexture != -1L) && m_bThatRealAction)
        {
            rs->TextureSet(0, m_idIconTexture);
            rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, BI_ONETEX_VERTEX_FORMAT, 2, m_IconVertex,
                                sizeof(BI_ONETEXTURE_VERTEX), "battle_rectangle");

            m_ActionHint1.Print();
            m_ActionHint2.Print();
        }
    }

    // Show log strings
    if (m_bShowLogStrings)
    {
        if (m_sRoot == nullptr)
            return;
        auto *ptr = m_sRoot;
        int32_t nAlign = PR_ALIGN_LEFT;
        auto strX = m_nWindowLeft;
        if (m_nWindowRight >= 0)
        {
            strX = m_nWindowRight;
            nAlign = PR_ALIGN_RIGHT;
        }
        auto strY = m_nStringBegin;
        while (ptr != nullptr)
        {
            // rs->Print(m_fontID,m_dwColor,strX,strY,"%s",ptr->str);
            if (ptr->alpha <= 255.f)
                // rs->Print(m_fontID,m_dwColor+(int32_t(ptr->alpha)<<24),strX,m_nWindowUp+(int32_t)ptr->offset,"%s",ptr->str);
                rs->ExtPrint(m_fontID, m_dwColor + (static_cast<int32_t>(ptr->alpha) << 24), 0, nAlign, true, m_fFontScale,
                             0, 0, strX, m_nWindowUp + static_cast<int32_t>(ptr->offset), "%s", ptr->str);
            else
                // rs->Print(m_fontID,m_dwColor+0xFF000000,strX,m_nWindowUp+(int32_t)ptr->offset,"%s",ptr->str);
                rs->ExtPrint(m_fontID, m_dwColor + 0xFF000000, 0, nAlign, true, m_fFontScale, 0, 0, strX,
                             m_nWindowUp + static_cast<int32_t>(ptr->offset), "%s", ptr->str);
            strY += m_nStringOffset;
            ptr = ptr->next;
        }
    }
}

void ILogAndActions::Create(bool bFastComShow, bool bLogStringShow)
{
    m_bShowActiveCommand = bFastComShow;
    m_bShowLogStrings = bLogStringShow;

    // Set parameters for the active action icon
    auto *pA = core.Entity_GetAttributeClass(g_ILogAndActions, "ActiveActions");
    if (pA != nullptr)
    {
        m_idIconTexture = rs->TextureCreate(pA->GetAttribute("TextureName"));
        m_horzDiv = pA->GetAttributeAsDword("horzQ", 1);
        m_vertDiv = pA->GetAttributeAsDword("vertQ", 1);
        m_nIconWidth = pA->GetAttributeAsDword("width", 64);
        m_nIconHeight = pA->GetAttributeAsDword("height", 64);
        m_nIconLeft = pA->GetAttributeAsDword("left", 0);
        m_nIconUp = pA->GetAttributeAsDword("top", 0);

        m_ActionHint1.Init(rs, pA->GetAttributeClass("text1"));
        m_ActionHint2.Init(rs, pA->GetAttributeClass("text2"));
    }
    else
    {
        m_idIconTexture = -1L;
        m_horzDiv = 1;
        m_vertDiv = 1;
        m_nIconWidth = 64;
        m_nIconHeight = 64;
        m_nIconLeft = 0;
        m_nIconUp = 0;
    }
    // build a rectangle for drawing the active action
    m_IconVertex[0].w = m_IconVertex[1].w = m_IconVertex[2].w = m_IconVertex[3].w = .5f;
    m_IconVertex[0].pos.z = m_IconVertex[1].pos.z = m_IconVertex[2].pos.z = m_IconVertex[3].pos.z = 1.f;
    m_IconVertex[0].pos.x = m_IconVertex[1].pos.x = static_cast<float>(m_nIconLeft);
    m_IconVertex[2].pos.x = m_IconVertex[3].pos.x = static_cast<float>(m_nIconLeft + m_nIconWidth);
    m_IconVertex[0].pos.y = m_IconVertex[2].pos.y = static_cast<float>(m_nIconUp);
    m_IconVertex[1].pos.y = m_IconVertex[3].pos.y = static_cast<float>(m_nIconUp + m_nIconHeight);
    m_IconVertex[0].tu = m_IconVertex[1].tu = 0.f;
    m_IconVertex[2].tu = m_IconVertex[3].tu = 1.f / static_cast<float>(m_horzDiv);
    m_IconVertex[0].tv = m_IconVertex[2].tv = 0.f;
    m_IconVertex[1].tv = m_IconVertex[3].tv = 1.f / static_cast<float>(m_vertDiv);

    // set parameters for the previous action lines
    pA = core.Entity_GetAttributeClass(g_ILogAndActions, "Log");
    if (pA != nullptr)
    {
        m_nWindowWidth = pA->GetAttributeAsDword("width", 200);
        m_nWindowHeight = pA->GetAttributeAsDword("height", 128);
        m_nWindowLeft = pA->GetAttributeAsDword("left", -1);
        m_nWindowRight = pA->GetAttributeAsDword("right", -1);
        m_nWindowUp = pA->GetAttributeAsDword("up", 0);
        m_fontID = rs->LoadFont(pA->GetAttribute("font"));
        m_fFontScale = pA->GetAttributeAsFloat("fontscale", 1.f);
        m_dwColor = pA->GetAttributeAsDword("color", 0x00FFFFFF);
        m_nStringOffset = pA->GetAttributeAsDword("offsetString", 24);
        m_fShiftSpeed = pA->GetAttributeAsFloat("speed", 1.f / 50.f);
        m_fBlendSpeed = pA->GetAttributeAsFloat("color_speed", 1.f / 50.f);
    }
    else
    {
        m_nWindowWidth = 200;
        m_nWindowHeight = 128;
        m_nWindowLeft = 64;
        m_nWindowUp = 0;
        m_fontID = -1L;
        m_fFontScale = 1.f;
        m_dwColor = 0x00FFFFFF;
        m_nStringBegin = 0;
        m_nStringOffset = 24;
        m_fShiftSpeed = 1.f / 50.f;
        m_fBlendSpeed = 1.f / 50.f;
    }
}

void ILogAndActions::ActionChange(bool bFastComShow, bool bLogStringShow)
{
    m_bShowActiveCommand = bFastComShow;
    m_bShowLogStrings = bLogStringShow;

    m_bThatRealAction = false;

    // Delete the old parameters
    TEXTURE_RELEASE(rs, m_idIconTexture);

    // Set parameters for the active action icon
    ATTRIBUTES *pA = core.Entity_GetAttributeClass(g_ILogAndActions, "ActiveActions");
    if (pA != nullptr)
    {
        m_idIconTexture = rs->TextureCreate(pA->GetAttribute("TextureName"));
        m_horzDiv = pA->GetAttributeAsDword("horzQ", 1);
        m_vertDiv = pA->GetAttributeAsDword("vertQ", 1);
        m_nIconWidth = pA->GetAttributeAsDword("width", 64);
        m_nIconHeight = pA->GetAttributeAsDword("height", 64);
        m_nIconLeft = pA->GetAttributeAsDword("left", 0);
        m_nIconUp = pA->GetAttributeAsDword("top", 0);

        m_ActionHint1.Init(rs, pA->GetAttributeClass("text1"));
        m_ActionHint2.Init(rs, pA->GetAttributeClass("text2"));
    }
    else
    {
        m_idIconTexture = -1L;
        m_horzDiv = 1;
        m_vertDiv = 1;
        m_nIconWidth = 64;
        m_nIconHeight = 64;
        m_nIconLeft = 0;
        m_nIconUp = 0;
    }
    // build a rectangle for drawing the active action
    m_IconVertex[0].w = m_IconVertex[1].w = m_IconVertex[2].w = m_IconVertex[3].w = .5f;
    m_IconVertex[0].pos.z = m_IconVertex[1].pos.z = m_IconVertex[2].pos.z = m_IconVertex[3].pos.z = 1.f;
    m_IconVertex[0].pos.x = m_IconVertex[1].pos.x = static_cast<float>(m_nIconLeft);
    m_IconVertex[2].pos.x = m_IconVertex[3].pos.x = static_cast<float>(m_nIconLeft + m_nIconWidth);
    m_IconVertex[0].pos.y = m_IconVertex[2].pos.y = static_cast<float>(m_nIconUp);
    m_IconVertex[1].pos.y = m_IconVertex[3].pos.y = static_cast<float>(m_nIconUp + m_nIconHeight);
    m_IconVertex[0].tu = m_IconVertex[1].tu = 0.f;
    m_IconVertex[2].tu = m_IconVertex[3].tu = 1.f / static_cast<float>(m_horzDiv);
    m_IconVertex[0].tv = m_IconVertex[2].tv = 0.f;
    m_IconVertex[1].tv = m_IconVertex[3].tv = 1.f / static_cast<float>(m_vertDiv);
}

void ILogAndActions::Release()
{
    TEXTURE_RELEASE(rs, m_idIconTexture);

    rs->UnloadFont(m_fontID);
    while (m_sRoot != nullptr)
    {
        STRING_DESCR *p = m_sRoot;
        m_sRoot = p->next;
        STORM_DELETE(p->str);
        delete p;
    }
    m_ActionHint1.Release();
    m_ActionHint2.Release();
    rs = nullptr;
}

void ILogAndActions::SetString(const char *str, bool immortal)
{
    if (str == nullptr)
        return;

    // find the last element of the list
    STRING_DESCR *last = m_sRoot;
    if (last != nullptr)
        while (last->next != nullptr)
            last = last->next;

    // Return if such a line already exists and it is last
    if (last != nullptr && last->str != nullptr && storm::iEquals(last->str, str))
        return;

    // create a new line descriptor
    auto *newDescr = new STRING_DESCR;
    if (newDescr == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }
    // it will be the last on the list
    newDescr->next = nullptr;
    // add the specified string to it
    const auto len = strlen(str) + 1;
    if ((newDescr->str = new char[len]) == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }
    strcpy_s(newDescr->str, len, str);
    // set the maximum visibility
    if (immortal)
        newDescr->alpha = 10000.f;
    else
        newDescr->alpha = 255.f;

    // if the list is empty, put the string as the root
    if (last == nullptr)
    {
        newDescr->offset = static_cast<float>(m_nStringBegin);
        m_sRoot = newDescr;
    }
    // otherwise add it to the end of the list
    else
    {
        newDescr->offset = last->offset + m_nStringOffset;
        last->next = newDescr;
        if (newDescr->offset + m_nStringOffset > m_nWindowHeight)
        {
            const int32_t offsetDelta = static_cast<int32_t>(newDescr->offset) + m_nStringOffset - m_nWindowHeight;
            for (STRING_DESCR *tmpDescr = m_sRoot; tmpDescr != nullptr;)
            {
                if ((tmpDescr->offset -= offsetDelta) < 0)
                {
                    m_sRoot = tmpDescr->next;
                    STORM_DELETE(tmpDescr->str);
                    delete tmpDescr;
                    tmpDescr = m_sRoot;
                    continue;
                }
                tmpDescr = tmpDescr->next;
            }
        }
    }
}

void ILogAndActions::SetAction(const char *actionName)
{
    ATTRIBUTES *pA;

    if (actionName == nullptr)
        return;
    if ((strlen(actionName) + 1) > sizeof(m_sActionName))
    {
        core.Trace("Action name: %s  - overup size of name");
        return;
    }
    pA = core.Entity_GetAttributeClass(g_ILogAndActions, "ActiveActions");
    if (pA != nullptr)
        pA = pA->GetAttributeClass(actionName);
    if (pA == nullptr)
        return;
    strcpy_s(m_sActionName, actionName);
    // set texture coordinates for this action icon
    FRECT texRect;
    const int32_t curIconNum = pA->GetAttributeAsDword("IconNum", 0);
    if (curIconNum == -1)
    {
        m_bThatRealAction = false;
        return;
    }
    m_bThatRealAction = true;
    CalculateTexturePos(texRect, m_horzDiv, m_vertDiv, curIconNum);
    m_IconVertex[1].tu = m_IconVertex[0].tu = texRect.left;
    m_IconVertex[2].tu = m_IconVertex[3].tu = texRect.right;
    m_IconVertex[0].tv = m_IconVertex[2].tv = texRect.top;
    m_IconVertex[1].tv = m_IconVertex[3].tv = texRect.bottom;

    pA = core.Entity_GetAttributeClass(g_ILogAndActions, "ActiveActions");
    if (pA)
    {
        m_ActionHint1.Init(rs, pA->GetAttributeClass("text1"));
        m_ActionHint2.Init(rs, pA->GetAttributeClass("text2"));
    }
    else
    {
        m_ActionHint1.Init(rs, nullptr);
        m_ActionHint2.Init(rs, nullptr);
    }
}
