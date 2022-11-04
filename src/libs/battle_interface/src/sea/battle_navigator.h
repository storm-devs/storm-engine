#pragma once

#include "bi_defines.h"
#include "weather_base.h"

#define RADIAL_QUANTITY 36
#define BI_DEFAULT_COLOR 0xFF0010C0
#define BI_ONESIDE_SIZE 6
#define BI_SIDE_QUANTITY 4
#define FIRERANGE_QUANTITY (BI_ONESIDE_SIZE + 1) * BI_SIDE_QUANTITY
#define MAX_ENEMY_SHIP_QUANTITY 20

class BATTLE_NAVIGATOR
{
    VDX9RENDER *rs{};
    Entity *m_pOwnerEI{};

  public:
    BATTLE_NAVIGATOR(BATTLE_NAVIGATOR &&) = delete;
    BATTLE_NAVIGATOR(const BATTLE_NAVIGATOR &) = delete;
    BATTLE_NAVIGATOR();
    ~BATTLE_NAVIGATOR();

    void Draw() const;
    void Update();
    void Init(VDX9RENDER *RenderService, Entity *pOwnerEI);
    void SetIsland();

    void DecrementScale()
    {
        m_fCurScale -= m_fScaleStep;
        if (m_fCurScale < m_fMinScale)
            m_fCurScale = m_fMinScale;
    }

    void IncrementScale()
    {
        m_fCurScale += m_fScaleStep;
        if (m_fCurScale > m_fMaxScale)
            m_fCurScale = m_fMaxScale;
    }

    void SetEnoughBalls(bool notEnoughBallFlag)
    {
        m_bNotEnoughBallFlag = notEnoughBallFlag;
    }

    void LostRender();
    void RestoreRender();

  protected:
    void CalculateTextureRect(FRECT &texRect, int32_t num, int32_t hq, int32_t vq);
    int32_t SetCircleVertexPos(BI_ONETEXTURE_VERTEX *v, float x, float y, float rad, float angle = 0) const;
    int32_t SetCircleVertexTex(BI_ONETEXTURE_VERTEX *v, float x = .5f, float y = .5f, float rad = .5f, float angle = 0);
    int32_t SetRectangleVertexPos(BI_ONETEXTURE_VERTEX *v, float x, float y, float width, float height,
                                  float angle = 0) const;
    int32_t SetRectangleVertexTex(BI_ONETEXTURE_VERTEX *v, float x = .5f, float y = .5f, float width = 1.f,
                                  float height = 1.f, float angle = 0);
    int32_t SetRectangleSegVertexPos(BI_ONETEXTURE_VERTEX *v, float x, float y, float width, float height,
                                     float begAngle, float endAngle);
    int32_t SetRectangleSegVertexTex(BI_ONETEXTURE_VERTEX *v, float x, float y, float width, float height,
                                     float begAngle, float endAngle);
    float CalculateCrossX(int side, float w, float h, float angl);
    float CalculateCrossY(int side, float w, float h, float angl);

    void SetMainCharacterData();
    void SetAnotherShip();
    void ReleaseAll();
    void UpdateMiniMap();
    void UpdateFireRangeBuffer() const;
    void FillOneSideFireRange(BI_NOTEXTURE_VERTEX *pv, ATTRIBUTES *pShip, ATTRIBUTES *pChar, const char *pstr) const;
    void UpdateCurrentCharge();

    void UpdateWindParam();

  protected:
    uint32_t m_dwBackGradColor1{};
    uint32_t m_dwBackGradColor2{};
    // visible horizon parameters
    float m_fMapRadius{};       // radius of the minimap on screen
    float m_fTextureRad{};      // radius of the minimap in texture
    float m_fWorldRad{};        // radius of the minimap in world coordinates
    float m_fCurScale{1.f};     // minimap scale
    float m_fDefaultScale{1.f}; // default map scale
    float m_fMinScale{};        // minimum map scale
    float m_fMaxScale{};        // maximum map scale
    float m_fScaleStep{};       // zoom step
    float m_fShipShowRad{};     // radius of the displayed ship

    // navigation window options
    int32_t m_NavigationWidth{};
    int32_t m_NavigationHeight{};
    int32_t m_XNavigator{};
    int32_t m_YNavigator{};

    // coordinates of our ship
    float m_fXPos{};
    float m_fYPos{};
    float m_fAngle{};
    float m_fShipSpeed{};
    float m_fMaxShipSpeed{};
    float m_fShipSpeedScale{-1.f};
    float m_fShipWindAgainst{};

    // island coordinates
    bool m_bYesIsland = false;
    float m_fXIsland{};
    float m_fYIsland{};
    float m_fIslandWidth{};
    float m_fIslandHeight{};
    float m_fIslandRadius{};

    // wind
    float m_fWindAngle{};
    float m_fWindStrength{};
    float m_fWindMaxStrength{};
    int32_t m_windWidth{};
    int32_t m_windHeight{};

    // cannon charge parameters
    uint32_t m_dwChargeCannon{};
    uint32_t m_dwReadyCannon{};
    uint32_t m_dwDamagedCannon{};

    float m_fBegAnglLeftCharge{};
    float m_fCurAnglLeftCharge{};
    float m_fCurAnglLeftDamage{};
    float m_fEndAnglLeftCharge{};

    float m_fBegAnglRightCharge{};
    float m_fCurAnglRightCharge{};
    float m_fCurAnglRightDamage{};
    float m_fEndAnglRightCharge{};

    float m_fBegAnglForwardCharge{};
    float m_fCurAnglForwardCharge{};
    float m_fCurAnglForwardDamage{};
    float m_fEndAnglForwardCharge{};

    float m_fBegAnglBackCharge{};
    float m_fCurAnglBackCharge{};
    float m_fCurAnglBackDamage{};
    float m_fEndAnglBackCharge{};

    // ship speed and wind parameters
    float m_fBegAnglShipSpeed{};
    float m_fEndAnglShipSpeed{};
    float m_fCurAnglShipSpeed{};
    float m_fBegAnglWindSpeed{};
    float m_fEndAnglWindSpeed{};
    float m_fCurAnglWindSpeed{};

    // textures
    int32_t m_idCompasTex{-1};     // compass
    int32_t m_idSpeedTex{-1};      // ship speed and wind
    int32_t m_idCannonTex{-1};     // cannon charge
    int32_t m_idEmptyTex{-1};      // empty space for speed and charge
    int32_t m_idIslandTexture{-1}; // Isle
    int32_t m_idWindTex{-1};       // wind
    int32_t m_idBestCourseTex{-1}; // best direction pointers
    int32_t m_idChargeTexture{-1}; // current cannon charge type
    int32_t m_idPowderTexture{-1}; // current gunpowder
    int32_t m_idWindTexture{-1};   // wind speed
    int32_t m_idSailTexture{-1};   // sail position / ship speed
    IDirect3DTexture9 *m_pIslandTexture{};

    uint32_t m_dwSeaColor{};                 // color of the sea on the minimap
    uint32_t m_dwFireZoneColor = 0x20FF0050; // color of the fire zone on the minimap
    uint32_t m_dwEnemyShipColor{};           // color of enemy ships
    uint32_t m_dwFrendShipColor{};           // color of friend ships
    uint32_t m_dwNeutralShipColor{};         // color of neutral ships
    uint32_t m_dwDeadShipColor{};            // color of a sinking ship

    // buffers
    int32_t m_idEmptyVBuf{-1};
    int32_t m_idCourseVBuf{-1};
    int32_t m_idMapVBuf{-1};
    // --------------------
    int32_t m_idFireZoneVBuf{-1}; // cannons fire zone
    // --------------------
    int32_t m_idCannonVBuf{-1};
    int32_t m_nvCannonCharge{};
    int32_t m_nvCannonReady{};
    int32_t m_nvCannonDamage{};
    //--------------------
    int32_t m_idSpeedVBuf{-1};
    int32_t m_nvSpeed{};
    //--------------------
    int32_t m_idShipsVBuf{-1};
    int32_t m_nvShips{};
    //--------------------
    int32_t m_idGradBackVBuf{-1};
    //--------------------
    int32_t m_idCurChargeVBuf{-1};

    // fonts and labels
    int32_t m_speedFont{-1};
    int32_t m_ySpeedShow{};
    int32_t m_xShipSpeed{};
    int32_t m_xWindSpeed{};

    WEATHER_BASE *m_wb{};
    ATTRIBUTES *m_pAWeather{};

    // icon of the current cannon charge type
    POINT m_ChargeGreed{};
    POINT m_ChargePos{};
    POINT m_ChargeSize{};
    int32_t m_curCharge{-1};
    // gunpowder icon
    POINT m_PowderGreed{};
    POINT m_PowderPos{};
    POINT m_PowderSize{};
    int32_t m_curPowder{};
    bool m_bPowderRunOut{}; // for blinking
    // wind icon
    int32_t m_curWindPic{};
    POINT m_WindGreed{};
    POINT m_WindPos{};
    POINT m_WindSize{};
    // sail position icon
    int32_t m_curSailState{};
    POINT m_SailGreed{};
    POINT m_SailPos{};
    POINT m_SailSize{};

    float m_fFontScale{};

    bool m_bNotEnoughBallFlag = false;

    float m_fAspectRatio{};
};
