#ifndef _BI_MESSAGE_ICONS_H_
#define _BI_MESSAGE_ICONS_H_

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
    void StartData(ATTRIBUTES *pAData[MESSAGE_ICONS_COLUMN_QUANTITY], long pLeft[MESSAGE_ICONS_COLUMN_QUANTITY]);
    bool InitData(entid_t host_eid, VDX9RENDER *_rs, ATTRIBUTES *pARoot);

  protected:
    // иконки сообщений
    bool m_bShowMsgIcon;
    long m_vMsgIconBufID;
    long m_iMsgIconBufID;
    long m_nMsgIconWidth;
    long m_nMsgIconHeight;
    long m_nMsgIconDist; // Дистанция между иконками
    long m_nBottomY;     // Координаты нижнего края иконок
    long m_nMsgIconQnt;
    long m_nMsgIconRowQnt;

    // текстура
    long m_idMsgIconsTexture;
    long m_nHorzTextureSize; // размер в иконках
    long m_nVertTextureSize; // размер в иконках

    ATTRIBUTES *m_pIconsAttr[MESSAGE_ICONS_COLUMN_QUANTITY];

    float m_fNormalStateTime; // Время высвечивания иконки, sec
    float m_fBlendTime;       // Время исчезания иконки, sec
    float m_fFallSpeed;     // Скорость падения иконки на свободное место, pix/sec
    float m_fBlindTimeUp;   // Время на повышение цвета при мигании, sec
    float m_fBlindTimeDown; // Время на понижение цвета при мигании, sec
    float m_fCurBlindTime;  // Текущее время для мигания
    bool m_bBlindDirectUp;  // Этап повышения цвета для мигания

    uint32_t m_dwHighBlindColor; // Цвет мигания максимальный
    uint32_t m_dwLowBlindColor;  // Цвет мигания минимальный
    uint32_t m_dwNormalColor;    // Нормальный цвет

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
            long pic;
            float bottom;
            float curTime;

            bool bDoBlend;
            bool bDoBlind;
            uint32_t color;
        };

        size_t rowQ;
        long startVertex;
        long leftPos;
        ROW_DESCR *pRow;
    };

    COLUMN_DESCRIBER m_pMsgColumns[MESSAGE_ICONS_COLUMN_QUANTITY];
};

#endif
