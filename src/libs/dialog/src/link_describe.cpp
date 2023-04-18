#include "link_describe.hpp"

#include "core.h"
#include "dialog.hpp"
#include "dialog_utils.hpp"
#include "string_compare.hpp"
#include "vma.hpp"
#include "v_file_service.h"

namespace storm::dialog {

DlgLinkDescribe::~DlgLinkDescribe()
{
    if (rs && nFontID >= 0)
        rs->UnloadFont(nFontID);
}

void DlgLinkDescribe::ChangeText(ATTRIBUTES *pALinks)
{
    editMode_ = false;
    edit_.reset();
    asText.clear();
    anLineEndIndex.clear();
    if (!pALinks)
        return;

    const size_t number_of_links = pALinks->GetAttributesNum();
    for (size_t i = 0; i < number_of_links; ++i)
    {
        auto *pA = pALinks->GetAttributeClass(i);
        if (pA)
        {
            if (pA->GetAttribute("edit"))
            {
                const bool has_var_target = !iEquals(to_string(pA->GetAttribute("edit")), "string");
                edit_ = EditConfig{
                    .line = static_cast<int32_t>(i),
                    .varIndex = has_var_target ? static_cast<int32_t>(pA->GetAttributeAsDword("edit", 0)) : -1,
                };
            }
            Assert(pA->HasValue());
            AddToStringArrayLimitedByWidth(
                pA->GetValue(), nWindowWidth, asText,
                [this](const std::string_view &text) { return rs->StringWidth(text, nFontID, fScale); });
            anLineEndIndex.push_back(asText.size());
        }
    }

    nStartIndex = 0;
    selectedLine_ = 0;
    fCursorCurrentTime = 0.f;
}

void DlgLinkDescribe::Init()
{
    edit_.reset();
}

int32_t DlgLinkDescribe::GetShowHeight()
{
    int32_t n = asText.size() - nStartIndex;
    if (n > nShowQuantity)
        n = nShowQuantity;
    return (n * nLineInterval);
}

void DlgLinkDescribe::Show(int32_t nY)
{
    int32_t n, nBeg, nEnd, i, y;

    y = nY + offset.y;
    n = nStartIndex;
    nBeg = 0;
    if (selectedLine_ < anLineEndIndex.size() && selectedLine_ >= 0)
    {
        if (selectedLine_ > 0)
            nBeg = anLineEndIndex[selectedLine_ - 1];
        nEnd = anLineEndIndex[selectedLine_];
    }
    else
        nEnd = -1;
    for (i = 0; i < nShowQuantity && n < asText.size(); i++, n++)
    {
        rs->ExtPrint(nFontID, (n >= nBeg && n < nEnd) ? dwSelColor : dwColor, 0, PR_ALIGN_LEFT, true, fScale, 0, 0,
                     offset.x, y, asText[n].c_str());

        if (edit_.has_value() && (n >= nBeg && n < nEnd) && selectedLine_ == edit_->line)
        {
            editMode_ = true;
            UpdateEditMode(n);
            ShowEditMode(offset.x, y, n);
        }
        else
            editMode_ = false;

        y += nLineInterval;
    }
}

void DlgLinkDescribe::UpdateEditMode(int32_t nTextIdx)
{
    const auto nKeyQ = core.Controls->GetKeyBufferLength();
    if (nKeyQ > 0)
    {
        const auto *const pKeys = core.Controls->GetKeyBuffer();
        if (pKeys)
        {
            for (int32_t n = 0; n < nKeyQ; n++)
            {
                if (pKeys[n].bSystem)
                {
                    switch (pKeys[n].ucVKey.c)
                    {
                    case VK_LEFT:
                        if (edit_->charIndex > 0)
                            edit_->charIndex--;
                        break;
                    case VK_BACK:
                        if (edit_->charIndex > 0)
                        {
                            edit_->charIndex--;
                            int offset = utf8::u8_offset(asText[nTextIdx].c_str(), edit_->charIndex);
                            int length = utf8::u8_inc(asText[nTextIdx].c_str() + offset);
                            asText[nTextIdx].erase(offset, length);
                        }
                        break;
                    case VK_RIGHT: {
                        int strLength = utf8::Utf8StringLength(asText[nTextIdx].c_str());
                        if (edit_->charIndex < strLength)
                            edit_->charIndex++;
                    }
                    break;
                    case VK_DELETE: {
                        int strLength = utf8::Utf8StringLength(asText[nTextIdx].c_str());
                        if (edit_->charIndex >= 0 && edit_->charIndex < strLength)
                        {
                            int offset = utf8::u8_offset(asText[nTextIdx].c_str(), edit_->charIndex);
                            int length = utf8::u8_inc(asText[nTextIdx].c_str() + offset);
                            asText[nTextIdx].erase(offset, length);
                        }
                    }
                    break;
                    }
                    continue;
                }
                if (pKeys[n].ucVKey.c < 0x20)
                    continue;

                std::string tmp(pKeys[n].ucVKey.b, pKeys[n].ucVKey.l);
                if (rs->StringWidth(asText[nTextIdx], nFontID, fScale, 0) +
                        rs->CharWidth(pKeys[n].ucVKey, nFontID, fScale) <=
                    nWindowWidth)
                {
                    int offset = utf8::u8_offset(asText[nTextIdx].c_str(), edit_->charIndex);
                    asText[nTextIdx].insert(offset, tmp.c_str());
                    edit_->charIndex++;
                }
            }
        }
    }
}

void DlgLinkDescribe::ShowEditMode(int32_t nX, int32_t nY, int32_t nTextIdx)
{
    // show cursor
    fCursorCurrentTime += core.GetRDeltaTime() * 0.001f;
    if (fCursorCurrentTime > fCursorVisibleTime + fCursorInvisibleTime)
        fCursorCurrentTime -= fCursorVisibleTime + fCursorInvisibleTime;
    if (fCursorCurrentTime <= fCursorVisibleTime)
    {
        int32_t nW = 0;
        if (!asText[nTextIdx].empty())
        {
            int strLength = utf8::Utf8StringLength(asText[nTextIdx].c_str());
            if (edit_->charIndex < strLength)
            {
                int offset = utf8::u8_offset(asText[nTextIdx].c_str(), edit_->charIndex);
                nW = rs->StringWidth(asText[nTextIdx].substr(0, offset), nFontID, fScale, 0);
            }
            else
                nW = rs->StringWidth(asText[nTextIdx], nFontID, fScale, 0);
        }
        rs->ExtPrint(nFontID, dwSelColor, 0, PR_ALIGN_LEFT, true, fScale, 0, 0, nX + nW, nY, "_");
    }

    if (attributes_) {
        attributes_->SetAttribute("value", asText[nTextIdx]);
    }

    if (edit_->varIndex >= 0 && edit_->varIndex < 10)
    {
        auto *pDat = static_cast<VDATA *>(core.GetScriptVariable("dialogEditStrings"));
        if (pDat)
            pDat->Set((char *)asText[nTextIdx].c_str(), edit_->varIndex);
    }
}

DlgLinkDescribe &DlgLinkDescribe::SetAttributes(ATTRIBUTES *attributes)
{
    attributes_ = attributes;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetRenderer(VDX9RENDER *renderer)
{
    rs = renderer;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetOffset(const POINT &new_offset)
{
    offset = new_offset;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetWindowWidth(int32_t width)
{
    nWindowWidth = width;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetFont(int32_t font_id)
{
    nFontID = font_id;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetFontScale(float scale)
{
    fScale = scale;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetLineHeight(int32_t line_height)
{
    nLineInterval = line_height;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetMaxLinesPerPage(int32_t max_lines)
{
    nShowQuantity = max_lines;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetColor(uint32_t color)
{
    dwColor = color;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetSelectedColor(uint32_t color)
{
    dwSelColor = color;
    return *this;
}

int32_t DlgLinkDescribe::GetSelectedLine() const noexcept
{
    return selectedLine_;
}

bool DlgLinkDescribe::IsInEditMode() const noexcept
{
    return editMode_;
}

bool DlgLinkDescribe::CanMoveUp() const noexcept
{
    return selectedLine_ > 0;
}

DlgLinkDescribe &DlgLinkDescribe::MoveUp()
{
    if (CanMoveUp()) {
        --selectedLine_;
        if (selectedLine_ > 0) {
            if (nStartIndex > anLineEndIndex[selectedLine_ - 1]) {
                nStartIndex = anLineEndIndex[selectedLine_ - 1];
            }
        }
        else {
            nStartIndex = 0;
        }
    }
    return *this;
}

bool DlgLinkDescribe::CanMoveDown() const noexcept
{
    return selectedLine_ < asText.size() - 1;
}

DlgLinkDescribe &DlgLinkDescribe::MoveDown()
{
    if (CanMoveDown()) {
        ++selectedLine_;
        if (nStartIndex + nShowQuantity < anLineEndIndex[selectedLine_]) {
            nStartIndex = anLineEndIndex[selectedLine_] - nShowQuantity;
        }
    }
    return *this;
}

bool DlgLinkDescribe::CanScrollDown() const noexcept
{
    return (nStartIndex + nShowQuantity < asText.size());
}

bool DlgLinkDescribe::CanScrollUp() const noexcept
{
    return nStartIndex > 0;
}

} // namespace storm::dialog