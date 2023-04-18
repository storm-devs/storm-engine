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

    VDX9RENDER *rs = nullptr;
    POINT offset{};
    int32_t nFontID = -1;
    float fScale{1.0f};
    int32_t nLineInterval{};
    uint32_t dwColor{};
    uint32_t dwSelColor{};
    int32_t nWindowWidth;
    int32_t selectedLine_{};
    int32_t nStartIndex{};
    int32_t nShowQuantity{5};

    struct EditConfig {
        int32_t line = 0;
        int32_t varIndex = -1;
        int32_t charIndex = 0;
    };

    std::optional<EditConfig> edit_;

    float fCursorCurrentTime{};
    float fCursorVisibleTime{0.8f};
    float fCursorInvisibleTime{0.2f};

    std::vector<std::string> asText;

    std::vector<int32_t> anLineEndIndex;

    bool editMode_ = false;

    ATTRIBUTES *attributes_ = nullptr;
};

} // namespace storm::dialog
