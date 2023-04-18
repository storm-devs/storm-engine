#pragma once

#include "matrix.h"

#include <optional>
#include <string>
#include <vector>

class VDX9RENDER;
class ATTRIBUTES;
class INIFILE;

namespace storm::dialog {

class DlgLinkDescribe final
{
  public:
    ~DlgLinkDescribe();

    void ChangeText(ATTRIBUTES *pALinks);
    void Init();
    [[nodiscard]] int32_t GetShowHeight();
    void Show(int32_t nY);

    DlgLinkDescribe &SetAttributes(ATTRIBUTES *attributes);
    DlgLinkDescribe &SetRenderer(VDX9RENDER *renderer);
    DlgLinkDescribe &SetOffset(const POINT &offset);
    DlgLinkDescribe &SetWindowWidth(int32_t width);
    DlgLinkDescribe &SetFont(int32_t font_id);
    DlgLinkDescribe &SetFontScale(float scale);
    DlgLinkDescribe &SetLineHeight(int32_t line_height);
    DlgLinkDescribe &SetMaxLinesPerPage(int32_t max_lines);
    DlgLinkDescribe &SetColor(uint32_t color);
    DlgLinkDescribe &SetSelectedColor(uint32_t color);

    [[nodiscard]] int32_t GetSelectedLine() const noexcept;
    [[nodiscard]] bool IsInEditMode() const noexcept;

    [[nodiscard]] bool CanMoveUp() const noexcept;
    DlgLinkDescribe &MoveUp();
    [[nodiscard]] bool CanMoveDown() const noexcept;
    DlgLinkDescribe &MoveDown();
    [[nodiscard]] bool CanScrollDown() const noexcept;
    [[nodiscard]] bool CanScrollUp() const noexcept;

  private:
    void UpdateEditMode(int32_t nTextIdx);
    void ShowEditMode(int32_t nX, int32_t nY, int32_t nTextIdx);

    VDX9RENDER *renderer_ = nullptr;
    POINT offset_{};
    int32_t fontId_ = -1;
    float fontScale_{1.0f};
    int32_t lineHeight_{};
    uint32_t textColor_{};
    uint32_t selectedTextColor_{};
    int32_t windowWidth_;
    int32_t selectedLine_{};
    int32_t startIndex_{};
    int32_t maxLinesPerPage_{5};

    struct EditConfig {
        int32_t line = 0;
        int32_t varIndex = -1;
        int32_t charIndex = 0;
    };

    std::optional<EditConfig> edit_;

    float currentCursorTime_{};

    std::vector<std::string> textLines_;
    std::vector<int32_t> lineBreaks_;

    bool editMode_ = false;

    ATTRIBUTES *attributes_ = nullptr;
};

} // namespace storm::dialog
