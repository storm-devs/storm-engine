#include "link_describe.hpp"

#include "dialog.hpp"

#include <core.h>
#include <dialog/dialog_utils.hpp>
#include <string_compare.hpp>
#include <vma.hpp>
#include <v_file_service.h>

namespace storm::dialog {

namespace {

constexpr float kEditCursorVisibleTime{0.8f};
constexpr float kEditCursorInvisibleTime{0.2f};

} // namespace

DlgLinkDescribe::~DlgLinkDescribe()
{
    if (renderer_ && fontId_ >= 0)
        renderer_->UnloadFont(fontId_);
}

void DlgLinkDescribe::ChangeText(ATTRIBUTES *pALinks)
{
    editMode_ = false;
    edit_.reset();
    textLines_.clear();
    lineBreaks_.clear();
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
                pA->GetValue(), windowWidth_, textLines_,
                [this](const std::string_view &text) { return renderer_->StringWidth(text, fontId_, fontScale_); });
            lineBreaks_.push_back(textLines_.size());
        }
    }

    startIndex_ = 0;
    selectedLine_ = 0;
    currentCursorTime_ = 0.f;
}

void DlgLinkDescribe::Init()
{
    edit_.reset();
}

int32_t DlgLinkDescribe::GetShowHeight()
{
    int32_t n = textLines_.size() - startIndex_;
    if (n > maxLinesPerPage_)
        n = maxLinesPerPage_;
    return (n * lineHeight_);
}

void DlgLinkDescribe::Show(int32_t nY)
{
    int32_t n, nBeg, nEnd, i, y;

    y = nY + offset_.y;
    n = startIndex_;
    nBeg = 0;
    if (selectedLine_ < lineBreaks_.size() && selectedLine_ >= 0)
    {
        if (selectedLine_ > 0)
            nBeg = lineBreaks_[selectedLine_ - 1];
        nEnd = lineBreaks_[selectedLine_];
    }
    else
        nEnd = -1;
    for (i = 0; i < maxLinesPerPage_ && n < textLines_.size(); i++, n++)
    {
        renderer_->ExtPrint(fontId_, (n >= nBeg && n < nEnd) ? selectedTextColor_ : textColor_, 0, PR_ALIGN_LEFT, true, fontScale_, 0, 0, offset_.x, y, textLines_[n].c_str());

        if (edit_.has_value() && (n >= nBeg && n < nEnd) && selectedLine_ == edit_->line)
        {
            editMode_ = true;
            UpdateEditMode(n);
            ShowEditMode(offset_.x, y, n);
        }
        else
            editMode_ = false;

        y += lineHeight_;
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
                            int offset = utf8::u8_offset(textLines_[nTextIdx].c_str(), edit_->charIndex);
                            int length = utf8::u8_inc(textLines_[nTextIdx].c_str() + offset);
                            textLines_[nTextIdx].erase(offset, length);
                        }
                        break;
                    case VK_RIGHT: {
                        int strLength = utf8::Utf8StringLength(textLines_[nTextIdx].c_str());
                        if (edit_->charIndex < strLength)
                            edit_->charIndex++;
                    }
                    break;
                    case VK_DELETE: {
                        int strLength = utf8::Utf8StringLength(textLines_[nTextIdx].c_str());
                        if (edit_->charIndex >= 0 && edit_->charIndex < strLength)
                        {
                            int offset = utf8::u8_offset(textLines_[nTextIdx].c_str(), edit_->charIndex);
                            int length = utf8::u8_inc(textLines_[nTextIdx].c_str() + offset);
                            textLines_[nTextIdx].erase(offset, length);
                        }
                    }
                    break;
                    }
                    continue;
                }
                if (pKeys[n].ucVKey.c < 0x20)
                    continue;

                std::string tmp(pKeys[n].ucVKey.b, pKeys[n].ucVKey.l);
                if (renderer_->StringWidth(textLines_[nTextIdx], fontId_, fontScale_, 0) +
                        renderer_->CharWidth(pKeys[n].ucVKey, fontId_, fontScale_) <=
                    windowWidth_)
                {
                    int offset = utf8::u8_offset(textLines_[nTextIdx].c_str(), edit_->charIndex);
                    textLines_[nTextIdx].insert(offset, tmp.c_str());
                    edit_->charIndex++;
                }
            }
        }
    }
}

void DlgLinkDescribe::ShowEditMode(int32_t nX, int32_t nY, int32_t nTextIdx)
{
    // show cursor
    currentCursorTime_ += core.GetRDeltaTime() * 0.001f;
    if (currentCursorTime_ > kEditCursorVisibleTime + kEditCursorInvisibleTime)
        currentCursorTime_ -= kEditCursorVisibleTime + kEditCursorInvisibleTime;
    if (currentCursorTime_ <= kEditCursorVisibleTime)
    {
        int32_t nW = 0;
        if (!textLines_[nTextIdx].empty())
        {
            int strLength = utf8::Utf8StringLength(textLines_[nTextIdx].c_str());
            if (edit_->charIndex < strLength)
            {
                int offset = utf8::u8_offset(textLines_[nTextIdx].c_str(), edit_->charIndex);
                nW = renderer_->StringWidth(textLines_[nTextIdx].substr(0, offset), fontId_, fontScale_, 0);
            }
            else
                nW = renderer_->StringWidth(textLines_[nTextIdx], fontId_, fontScale_, 0);
        }
        renderer_->ExtPrint(fontId_, selectedTextColor_, 0, PR_ALIGN_LEFT, true, fontScale_, 0, 0, nX + nW, nY, "_");
    }

    if (attributes_) {
        attributes_->SetAttribute("value", textLines_[nTextIdx]);
    }

    if (edit_->varIndex >= 0 && edit_->varIndex < 10)
    {
        auto *pDat = static_cast<VDATA *>(core.GetScriptVariable("dialogEditStrings"));
        if (pDat)
            pDat->Set((char *)textLines_[nTextIdx].c_str(), edit_->varIndex);
    }
}

DlgLinkDescribe &DlgLinkDescribe::SetAttributes(ATTRIBUTES *attributes)
{
    attributes_ = attributes;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetRenderer(VDX9RENDER *renderer)
{
    renderer_ = renderer;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetOffset(const POINT &new_offset)
{
    offset_ = new_offset;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetWindowWidth(int32_t width)
{
    windowWidth_ = width;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetFont(int32_t font_id)
{
    fontId_ = font_id;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetFontScale(float scale)
{
    fontScale_ = scale;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetLineHeight(int32_t line_height)
{
    lineHeight_ = line_height;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetMaxLinesPerPage(int32_t max_lines)
{
    maxLinesPerPage_ = max_lines;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetColor(uint32_t color)
{
    textColor_ = color;
    return *this;
}

DlgLinkDescribe &DlgLinkDescribe::SetSelectedColor(uint32_t color)
{
    selectedTextColor_ = color;
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
            if (startIndex_ > lineBreaks_[selectedLine_ - 1]) {
                startIndex_ = lineBreaks_[selectedLine_ - 1];
            }
        }
        else {
            startIndex_ = 0;
        }
    }
    return *this;
}

bool DlgLinkDescribe::CanMoveDown() const noexcept
{
    return selectedLine_ < lineBreaks_.size() - 1;
}

DlgLinkDescribe &DlgLinkDescribe::MoveDown()
{
    if (CanMoveDown()) {
        ++selectedLine_;
        if (startIndex_ + maxLinesPerPage_ < lineBreaks_[selectedLine_]) {
            startIndex_ = lineBreaks_[selectedLine_] - maxLinesPerPage_;
        }
    }
    return *this;
}

bool DlgLinkDescribe::CanScrollDown() const noexcept
{
    return (startIndex_ + maxLinesPerPage_ < textLines_.size());
}

bool DlgLinkDescribe::CanScrollUp() const noexcept
{
    return startIndex_ > 0;
}

} // namespace storm::dialog