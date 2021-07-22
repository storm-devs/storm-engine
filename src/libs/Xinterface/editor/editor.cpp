#include "editor.h"

#include "core.h"

#include "../Nodes/xi_image.h"
#include "../xinterface.h"
#include "list.h"

GIEditor::GIEditor(XINTERFACE *pInterface)
{
    m_pGIOwner = pInterface;
    m_pRS = pInterface->RenderService();

    m_bShowMode = false;

    m_pEditableNode = nullptr;
    m_fLastKeyPressTime = -1.f;

    m_pImageBack = new CXI_IMAGE;
    Assert(m_pImageBack);
    m_pImageBack->SetPosition(100, 60);
    m_pImageBack->SetSize(600, 400);
    m_pImageBack->SetColor(0xD0606060);
    m_pImageBack->SetThisRectangleNotUseTexture(true);

    m_pNodeList = new GIEditorList(this);
    Assert(m_pNodeList);
    m_pNodeList->SetPosition(120, 80, 360, 380);
    LinkEvent(m_pNodeList->m_pChangeSelected, static_cast<GIEditorEvent>(&GIEditor::ChangeNodeName));

    m_bSubNameOn = false;
    m_pSubNameList = new GIEditorList(this);
    Assert(m_pSubNameList);
    m_pSubNameList->SetPosition(380, 80, 500, 380);
    LinkEvent(m_pSubNameList->m_pChangeSelected, (GIEditorEvent)&GIEditor::ChangeSubNodeName);
}

GIEditor::~GIEditor()
{
    Release();
}

void GIEditor::Release()
{
    m_pGIOwner = nullptr;
    m_bShowMode = false;

    STORM_DELETE(m_pImageBack);
    STORM_DELETE(m_pNodeList);
    STORM_DELETE(m_pSubNameList);
}

void GIEditor::Render() const
{
    if (!m_bShowMode)
        return;

    const auto pntMouse = m_pGIOwner->GetMousePoint();
    m_pNodeList->CheckMouseInside(pntMouse.x, pntMouse.y);
    if (m_bSubNameOn)
        m_pSubNameList->CheckMouseInside(pntMouse.x, pntMouse.y);

    CONTROL_STATE cs;
    core.Controls->GetControlState(INTERFACE_CONTROL_LCLICK, cs);
    if (cs.state == CST_ACTIVATED)
    {
        m_pNodeList->MakeMouseClick(pntMouse.x, pntMouse.y);
        if (m_bSubNameOn)
            m_pSubNameList->MakeMouseClick(pntMouse.x, pntMouse.y);
    }

    if (m_bSubNameOn)
        m_pSubNameList->DoKeyChecking();
    else
        m_pNodeList->DoKeyChecking();

    m_pImageBack->Draw();
    m_pNodeList->Render();
    if (m_bSubNameOn)
        m_pSubNameList->Render();
}

bool GIEditor::ProcessControl()
{
    if (!m_bShowMode)
    {
        if (core.Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0 &&
            core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0 && core.Controls->GetDebugAsyncKeyState('E') < 0)
        {
            m_bShowMode = true;
            m_bSubNameOn = false;
            return true;
        }
    }

    if (m_bShowMode)
    {
        if (core.Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0 &&
            core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0 && core.Controls->GetDebugAsyncKeyState('Q') < 0)
            m_bShowMode = false;

        CONTROL_STATE cs;
        core.Controls->GetControlState("IStartButton", cs);
        if (cs.state == CST_INACTIVATED)
        {
            m_bShowMode = false;
            if (!m_bSubNameOn)
            {
                std::vector<std::string> aStr;
                if (m_pEditableNode && m_pEditableNode->GetInternalNameList(aStr))
                {
                    m_bShowMode = true;
                    m_bSubNameOn = false;
                    m_pSubNameList->RemoveAllStrings();
                    for (long n = 0; n < aStr.size(); n++)
                        m_pSubNameList->AddString(aStr[n]);
                    m_bSubNameOn = true;
                    m_pSubNameList->SetSelectIndex(0);
                    aStr.clear();
                }
            }
        }

        if (m_bSubNameOn && core.Controls->GetDebugAsyncKeyState(VK_ESCAPE) < 0)
        {
            m_bSubNameOn = false;
        }

        return true;
    }

    if (!m_pEditableNode)
        return false;

    auto bMove = false;
    auto bSize = false;
    if (core.Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0) // moving
    {
        bMove = true;
        if (core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0) // change the size
        {
            bMove = false;
            bSize = true;
        }
    }

    if (core.Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0 && core.Controls->GetDebugAsyncKeyState(VK_SHIFT) < 0 &&
        core.Controls->GetDebugAsyncKeyState('S') < 0)
    {
        m_pEditableNode->SaveParametersToIni();
    }

    if (bMove || bSize)
    {
        long nHorz = 0;
        if (core.Controls->GetDebugAsyncKeyState(VK_LEFT) < 0)
            if (core.Controls->GetDebugAsyncKeyState(VK_MENU) < 0)
                nHorz -= 10;
            else
                nHorz--;
        if (core.Controls->GetDebugAsyncKeyState(VK_RIGHT) < 0)
            if (core.Controls->GetDebugAsyncKeyState(VK_MENU) < 0)
                nHorz += 10;
            else
                nHorz++;
        long nVert = 0;
        if (core.Controls->GetDebugAsyncKeyState(VK_UP) < 0)
            if (core.Controls->GetDebugAsyncKeyState(VK_MENU) < 0)
                nVert -= 10;
            else
                nVert--;
        if (core.Controls->GetDebugAsyncKeyState(VK_DOWN) < 0)
            if (core.Controls->GetDebugAsyncKeyState(VK_MENU) < 0)
                nVert += 10;
            else
                nVert++;

        if (nHorz != 0 || nVert != 0)
        {
            if (m_fLastKeyPressTime < 0.f)
                m_fLastKeyPressTime = 0.f;
            else
            {
                m_fLastKeyPressTime += core.GetDeltaTime() * .001f;
                if (m_fLastKeyPressTime < 0.8f)
                    nHorz = nVert = 0;
            }
        }
        else
            m_fLastKeyPressTime = -1.f;

        if (nHorz != 0 || nVert != 0)
        {
            auto rectNew = m_pEditableNode->m_rect;
            if (nHorz != 0)
            {
                rectNew.right += nHorz;
                if (bMove)
                    rectNew.left += nHorz;
                /*
                        if( rectNew.right < 0 ) rectNew.right = 0;
                        if(
                 * rectNew.left > (long)m_pGIOwner->GetScreenWidth() ) rectNew.left =
 m_pGIOwner->GetScreenWidth();
                 * if( rectNew.right < rectNew.left ) rectNew.right = rectNew.left;*/
            }
            if (nVert != 0)
            {
                rectNew.bottom += nVert;
                if (bMove)
                    rectNew.top += nVert;

                /*                if( rectNew.bottom < 0 ) rectNew.bottom = 0;
                        if( rectNew.top >
                 * (long)m_pGIOwner->GetScreenHeight() ) rectNew.top =
                   m_pGIOwner->GetScreenHeight();
                 * if( rectNew.bottom < rectNew.top ) rectNew.bottom = rectNew.top;*/
            }
            m_pEditableNode->ChangePosition(rectNew);
        }

        return true;
    }
    return false;
}

bool GIEditor::IsShowMode() const
{
    return m_bShowMode;
}

void GIEditor::MakeShowMode(bool bShow)
{
    m_bShowMode = bShow;
}

void GIEditor::SetEditNode(CINODE *pNode)
{
    m_pEditableNode = pNode;
}

void GIEditor::ReCreate() const
{
    m_pNodeList->RemoveAllStrings();
    std::string sStr = "Nothing";
    m_pNodeList->AddString(sStr);
    m_pNodeList->SetSelectIndex(0);
}

void GIEditor::AddNode(CINODE *pNode) const
{
    if (!pNode)
        return;
    std::string sStr = pNode->m_nodeName;
    m_pNodeList->AddString(sStr);
}

void GIEditor::DelNode(CINODE *pNode) const
{
    if (!pNode)
        return;
    m_pNodeList->RemoveString(pNode->m_nodeName);
}

void GIEditor::DrawSizeBox() const
{
    if (!m_pEditableNode)
        return;
    if (core.Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0) // showing
    {
        RS_LINE rsl[8];
        for (long n = 0; n < 8; n++)
        {
            rsl[n].dwColor = 0xFFFFFFFF;
            rsl[n].vPos.z = 1.f;
        }
        rsl[0].vPos.x = static_cast<float>(m_pEditableNode->m_rect.left);
        rsl[0].vPos.y = static_cast<float>(m_pEditableNode->m_rect.top);
        rsl[1].vPos.x = static_cast<float>(m_pEditableNode->m_rect.right);
        rsl[1].vPos.y = static_cast<float>(m_pEditableNode->m_rect.top);

        rsl[2].vPos.x = static_cast<float>(m_pEditableNode->m_rect.right);
        rsl[2].vPos.y = static_cast<float>(m_pEditableNode->m_rect.top);
        rsl[3].vPos.x = static_cast<float>(m_pEditableNode->m_rect.right);
        rsl[3].vPos.y = static_cast<float>(m_pEditableNode->m_rect.bottom);

        rsl[4].vPos.x = static_cast<float>(m_pEditableNode->m_rect.right);
        rsl[4].vPos.y = static_cast<float>(m_pEditableNode->m_rect.bottom);
        rsl[5].vPos.x = static_cast<float>(m_pEditableNode->m_rect.left);
        rsl[5].vPos.y = static_cast<float>(m_pEditableNode->m_rect.bottom);

        rsl[6].vPos.x = static_cast<float>(m_pEditableNode->m_rect.left);
        rsl[6].vPos.y = static_cast<float>(m_pEditableNode->m_rect.bottom);
        rsl[7].vPos.x = static_cast<float>(m_pEditableNode->m_rect.left);
        rsl[7].vPos.y = static_cast<float>(m_pEditableNode->m_rect.top);
        m_pRS->DrawLines(rsl, 4, "Line");
        // boal -->
        // idFont, dwFCol, dwBCol, align, shadow, scale, sxs, sys,
        // left, top, "%s", str
        const auto &screenSize = core.GetScreenSize();
        const auto m_fontID = m_pGIOwner->GetRenderService()->LoadFont("interface_normal");
        m_pGIOwner->GetRenderService()->ExtPrint(
            m_fontID, 0xFFFFFFFF, 0, PR_ALIGN_LEFT, false, 1.0, screenSize.width, screenSize.height, 10, 10,
            "(%d, %d) - (%d, %d) W - %d H - %d", static_cast<int>(m_pEditableNode->m_rect.left),
            static_cast<int>(m_pEditableNode->m_rect.top), static_cast<int>(m_pEditableNode->m_rect.right),
            static_cast<int>(m_pEditableNode->m_rect.bottom),
            (static_cast<int>(m_pEditableNode->m_rect.right) - static_cast<int>(m_pEditableNode->m_rect.left)),
            (static_cast<int>(m_pEditableNode->m_rect.bottom) - static_cast<int>(m_pEditableNode->m_rect.top)));
        m_pGIOwner->GetRenderService()->UnloadFont(m_fontID);
        // boal <--
    }
}

void GIEditor::ChangeNodeName()
{
    if (m_pNodeList->GetSelectString() == "Nothing" || m_pNodeList->GetSelectString() == "")
    {
        m_pEditableNode = nullptr;
        return;
    }

    m_pEditableNode = m_pGIOwner->FindNode(m_pNodeList->GetSelectString().c_str(), nullptr);
}

void GIEditor::ChangeSubNodeName() const
{
    if (!m_bSubNameOn)
        return;
    if (!m_pEditableNode)
        return;

    m_pEditableNode->SetInternalName(m_pSubNameList->GetSelectString());
}
