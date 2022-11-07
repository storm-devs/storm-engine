#pragma once

#include "bi_defines.h"

#define MESSAGE_ICONS_COLUMN_QUANTITY 4

class MESSAGE_ICONS
{
    VDX9RENDER *rs;
    entid_t m_idHost;

  public:
    MESSAGE_ICONS();
    ~MESSAGE_ICONS();

    void Update(uint32_t deltaTime);
    void Draw() const;

    void SetShowMsg(bool bShow)
    {
        m_bShowMsgIcon = m_vMsgIconBufID >= 0 ? bShow : false;
    }

    void StartData(ATTRIBUTES *pAData[MESSAGE_ICONS_COLUMN_QUANTITY], int32_t pLeft[MESSAGE_ICONS_COLUMN_QUANTITY]);
    bool InitData(entid_t host_eid, VDX9RENDER *_rs, ATTRIBUTES *pARoot);

  protected:
    // message icons
    bool m_bShowMsgIcon;
    int32_t m_vMsgIconBufID;
    int32_t m_iMsgIconBufID;
    int32_t m_nMsgIconWidth;
    int32_t m_nMsgIconHeight;
    int32_t m_nMsgIconDist; // The distance between the icons
    int32_t m_nBottomY;     // The coordinates of the bottom edge of the icons
    int32_t m_nMsgIconQnt;
    int32_t m_nMsgIconRowQnt;

    // texture
    int32_t m_idMsgIconsTexture;
    int32_t m_nHorzTextureSize; // size in icons
    int32_t m_nVertTextureSize; // size in icons

    ATTRIBUTES *m_pIconsAttr[MESSAGE_ICONS_COLUMN_QUANTITY]{};

    float m_fNormalStateTime; // Icon highlighting time, sec
    float m_fBlendTime;       // Icon disappearance time, sec
    float m_fFallSpeed;       // Icon falling-to-free-space speed, pix / sec
    float m_fBlindTimeUp;     // Time to increase color when blinking, sec
    float m_fBlindTimeDown;   // Time to decrease color when blinking, sec
    float m_fCurBlindTime;    // Current time to blink
    bool m_bBlindDirectUp;    // Color modification stage for blinking

    uint32_t m_dwHighBlindColor; // Blinking color maximum
    uint32_t m_dwLowBlindColor;  // Blinking color minimum
    uint32_t m_dwNormalColor;    // Normal color

    struct COLUMN_DESCRIBER
    {
        COLUMN_DESCRIBER()
        {
            pRow = nullptr;
        }

        ~COLUMN_DESCRIBER()
        {
            delete[] pRow;
            pRow = nullptr;
        }

        struct ROW_DESCR
        {
            int32_t pic;
            float bottom;
            float curTime;

            bool bDoBlend;
            bool bDoBlind;
            uint32_t color;
        };

        size_t rowQ;
        int32_t startVertex;
        int32_t leftPos;
        ROW_DESCR *pRow;
    };

    COLUMN_DESCRIBER m_pMsgColumns[MESSAGE_ICONS_COLUMN_QUANTITY];
};
