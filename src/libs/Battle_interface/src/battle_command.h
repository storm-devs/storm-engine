#pragma once

#include "bi_defines.h"
#include <string>
#include <vector>

class BIImageRender;

// nCommandType
#define BISCT_Command 1
#define BISCT_Ability 2
#define BISCT_Ship 3
#define BISCT_Fort 4
#define BISCT_Land 5
#define BISCT_Charge 6
#define BISCT_UserIcon 7
#define BISCT_Cancel 8

class BICommandList
{
  public:
    BICommandList(BICommandList &&) = delete;
    BICommandList(const BICommandList &) = delete;
    BICommandList(entid_t eid, ATTRIBUTES *pA, VDX9RENDER *rs);
    virtual ~BICommandList();

    void Draw();
    void Update(long nTopLine, long nCharacterIndex, long nCommandMode);
    virtual void FillIcons() = 0;

    size_t AddTexture(const char *pcTextureName, uint32_t nCols, uint32_t nRows);

    // commands
    long ExecuteConfirm();
    long ExecuteLeft();
    long ExecuteRight();
    long ExecuteCancel();

    void SetActive(bool bActive);
    bool GetActive() const
    {
        return m_bActive;
    }

    void SetUpDown(bool bUp, bool bDown);

    virtual void Init();

    long AddToIconList(long nTextureNum, long nNormPictureNum, long nSelPictureNum, long nCooldownPictureNum,
                       long nCharacterIndex, const char *pcCommandName, long nTargetIndex, const char *pcLocName,
                       const char *pcNoteName);
    void AddAdditiveToIconList(long nTextureNum, long nPictureNum, float fDist, float fWidth, float fHeight);

  protected:
    entid_t m_idHostObj;
    ATTRIBUTES *m_pARoot;
    VDX9RENDER *m_pRS;

    BIImageRender *m_pImgRender;

    struct TextureDescr
    {
        std::string sFileName;
        uint32_t nCols;
        uint32_t nRows;
    };

    std::vector<TextureDescr> m_aTexture;

    struct UsedCommand
    {
        long nCharIndex;
        std::string sCommandName;
        long nTargetIndex;
        std::string sLocName;
        std::string sNote;

        long nTextureIndex;
        long nSelPictureIndex;
        long nNormPictureIndex;
        long nCooldownPictureIndex;

        float fCooldownFactor;

        struct AdditiveIcon
        {
            long nTex;
            long nPic;
            float fDelta;
            FPOINT fpSize;
        };

        std::vector<AdditiveIcon> aAddPicList;
    };

    bool m_bActive;

    std::vector<UsedCommand> m_aUsedCommand;
    long m_nStartUsedCommandIndex;
    long m_nSelectedCommandIndex;
    long m_nIconShowMaxQuantity;
    bool m_bLeftArrow;
    bool m_bRightArrow;

    POINT m_pntActiveIconOffset;
    POINT m_pntActiveIconSize;
    std::string m_sActiveIconTexture;
    FRECT m_frActiveIconUV1;
    FRECT m_frActiveIconUV2;
    std::string m_sActiveIconNote;

    bool m_bUpArrow;
    bool m_bDownArrow;
    std::string m_sUpDownArrowTexture;
    FRECT m_frUpArrowUV;
    FRECT m_frDownArrowUV;
    POINT m_pntUpDownArrowSize;
    POINT m_pntUpArrowOffset;
    POINT m_pntDownArrowOffset;

    std::string m_sCurrentCommandName;
    long m_nCurrentCommandCharacterIndex;
    long m_nCurrentCommandMode;

    POINT m_LeftTopPoint;
    POINT m_IconSize;
    long m_nIconSpace;

    long m_NoteFontID;
    uint32_t m_NoteFontColor;
    float m_NoteFontScale;
    POINT m_NotePos;
    POINT m_NoteOffset;
    std::string m_NoteText;

    struct CoolDownUpdateData
    {
        long nIconNum;
        float fTime;
        float fUpdateTime;
    };

    std::vector<CoolDownUpdateData> m_aCooldownUpdate;

    void Release();

    long IconAdd(long nPictureNum, long nTextureNum, RECT &rpos);
    long ClockIconAdd(long nForePictureNum, long nBackPictureNum, long nTextureNum, RECT &rpos, float fFactor);
    void AdditiveIconAdd(float fX, float fY, std::vector<UsedCommand::AdditiveIcon> &aList);
    FRECT &GetPictureUV(long nTextureNum, long nPictureNum, FRECT &uv);
    RECT &GetCurrentPos(long num, RECT &rpos) const;
    RECT &GetAddingPos(long num, RECT &rpos);

    void UpdateShowIcon();
    void SetNote(const char *pcNote, long nX, long nY);

    ATTRIBUTES *GetCurrentCommandAttribute() const;
};
