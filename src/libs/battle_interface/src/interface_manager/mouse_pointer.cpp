#include "mouse_pointer.h"
#include "../utils.h"
#include "../image/img_render.h"
#include "core.h"
#include "vma.hpp"

#define BI_MOUSECURSOR_ICON_ORDER 35000

MousePointer::MousePointer(BI_ManagerBase *pManager, ATTRIBUTES *pARoot)
{
    Assert(pManager && pARoot);

    m_pManager = pManager;
    m_pARoot = pARoot;

    m_pIcon = nullptr;
    m_mousepos.x = 0.f;
    m_mousepos.y = 0.f;
    m_mousesensivity.x = 1.f;
    m_mousesensivity.y = 1.f;

    const auto screenSize = core.GetScreenSize();
    m_cursorzone.left = 0;
    m_cursorzone.top = 0;
    m_cursorzone.right = screenSize.width;
    m_cursorzone.bottom = screenSize.height;

    InitMouseCursors();
}

MousePointer::~MousePointer()
{
    STORM_DELETE(m_pIcon);
}

void MousePointer::Update()
{
    MoveCursor();
    SetCurrentCursor();
}

void MousePointer::InitMouseCursors()
{
    m_nCurrentCursor = -1;
    auto *pACursors = m_pARoot ? m_pARoot->GetAttributeClass("cursors") : nullptr;
    if (!pACursors)
        return;

    BIUtils::ReadPosFromAttr(pACursors, "size", m_cursorsize.x, m_cursorsize.y, 32, 32);
    BIUtils::ReadPosFromAttr(pACursors, "startpos", m_mousepos.x, m_mousepos.y, 0.f, 0.f);
    BIUtils::ReadPosFromAttr(pACursors, "sensivity", m_mousesensivity.x, m_mousesensivity.y, 8.f, 8.f);
    BIUtils::ReadRectFromAttr(pACursors, "zone", m_cursorzone, m_cursorzone);

    pACursors = pACursors->GetAttributeClass("list");
    if (!pACursors)
        return;

    const size_t q = pACursors->GetAttributesNum();
    for (int32_t n = 0; n < q; n++)
    {
        auto *pA = pACursors->GetAttributeClass(n);
        if (pA)
        {
            const int32_t i = pA->GetAttributeAsDword("index", -1);
            if (i >= 0 && i < BI_CURSORS_QUANTITY)
            {
                m_aCursors[i].offset.x = pA->GetAttributeAsDword("xoffset", 0);
                m_aCursors[i].offset.y = pA->GetAttributeAsDword("yoffset", 0);
                m_aCursors[i].texture = to_string(pA->GetAttribute("texture"));
                FULLRECT(m_aCursors[i].uv);
                BIUtils::ReadRectFromAttr(pA, "uv", m_aCursors[i].uv, m_aCursors[i].uv);
            }
        }
    }

    m_nCurrentCursor = BI_CURSOR_COMMON;
    m_pIcon = m_pManager->GetImageRender()->CreateImage(BIType_square, m_aCursors[BI_CURSOR_COMMON].texture.c_str(),
                                                        0xFF808080, m_aCursors[BI_CURSOR_COMMON].uv,
                                                        GetCurrentCursorIconPos(), BI_MOUSECURSOR_ICON_ORDER);
}

void MousePointer::MoveCursor()
{
    CONTROL_STATE cs;
    const auto fDeltaTime = core.GetDeltaTime() * 0.001f;

    core.Controls->GetControlState("ITurnH", cs);
    m_mousepos.x += m_mousesensivity.x * fDeltaTime * cs.fValue;

    core.Controls->GetControlState("ITurnV", cs);
    m_mousepos.y -= m_mousesensivity.y * fDeltaTime * cs.fValue;

    if (m_mousepos.x < static_cast<float>(m_cursorzone.left))
        m_mousepos.x = static_cast<float>(m_cursorzone.left);
    if (m_mousepos.x > static_cast<float>(m_cursorzone.right))
        m_mousepos.x = static_cast<float>(m_cursorzone.right);
    if (m_mousepos.y < static_cast<float>(m_cursorzone.top))
        m_mousepos.y = static_cast<float>(m_cursorzone.top);
    if (m_mousepos.y > static_cast<float>(m_cursorzone.bottom))
        m_mousepos.y = static_cast<float>(m_cursorzone.bottom);
}

void MousePointer::SetCurrentCursor()
{
    const int32_t nNewCursor = BI_CURSOR_COMMON;

    if (nNewCursor != m_nCurrentCursor)
    {
        if (!m_pIcon || m_aCursors[m_nCurrentCursor].texture != m_aCursors[nNewCursor].texture)
        {
            STORM_DELETE(m_pIcon);
            m_nCurrentCursor = nNewCursor;
            m_pIcon = m_pManager->GetImageRender()->CreateImage(BIType_square, m_aCursors[nNewCursor].texture.c_str(),
                                                                0xFF808080, m_aCursors[nNewCursor].uv,
                                                                GetCurrentCursorIconPos(), BI_MOUSECURSOR_ICON_ORDER);
            return;
        }
        m_nCurrentCursor = nNewCursor;
        m_pIcon->SetUV(m_aCursors[nNewCursor].uv);
    }

    const auto pos = GetCurrentCursorIconPos();
    m_pIcon->SetPosition(pos.left, pos.top, pos.right, pos.bottom);
}

RECT MousePointer::GetCurrentCursorIconPos() const
{
    RECT pos;
    pos.left = static_cast<int32_t>(m_mousepos.x) + m_aCursors[m_nCurrentCursor].offset.x;
    pos.top = static_cast<int32_t>(m_mousepos.y) + m_aCursors[m_nCurrentCursor].offset.y;
    pos.right = pos.left + m_cursorsize.x;
    pos.bottom = pos.top + m_cursorsize.y;
    return pos;
}
