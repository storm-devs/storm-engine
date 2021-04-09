//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Window
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "LTypes.h"
#include "LighterLights.h"
#include "dx9render.h"

class Window
{
    struct Vertex
    {
        float x, y;
        float z, rhw;
    };

    struct ListElement
    {
        enum Type
        {
            t_quick,
            t_trace,
            t_smooth,
            t_blur,
            t_save,
            t_load,
            t_savelight,
            t_amb,
            t_light,
            t_glight,
        };

        char *name; // Name
        Type type;  // A type
        CVECTOR c;  // Current selected color
        float st;   // Color saturation
        // Modification data
        CVECTOR *color; // Colour
        float *cosine;  // Coefficient of cosine
        float *shadow;  // shadow Coefficient
        float *bright;  // Shadow brightness
        float *contr;   // Shadow contrast
        float *gamma;   // shadow Gamma
        float *att0;    // Attenuation D^0
        float *att1;    // Attenuation D^1
        float *att2;    // Attenuation D^2
        float *range;   // Radius of action
        bool *isOn;     // Included
        long litIndex;  // Source index
        float h;        // Additional window height
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Window(Window &&) = delete;
    Window(const Window &) = delete;
    Window();
    virtual ~Window();

    bool Init(VDX9RENDER *rs);
    void InitList(LighterLights &ls);
    void Draw(float dltTime);
    void Reset(bool isVis);

    // Checkboxes controlling actions
    bool isNeedInit;
    bool isNoPrepared;
    bool isVisible;
    bool isTraceShadows;
    float tracePrc;
    bool isSmoothShadows;
    float smoothPrc;
    float smoothRad;
    bool smoothNorm;
    bool isBlurLight;
    bool isResetBlurLight;
    bool isTraceBlur;
    float blurPrc;
    float blurRad;
    float blurAtt;
    float blurCos;
    float kBlur;
    bool isLockCtrl;
    bool isUpdateLight;
    long updateLight;
    bool updateLightCos, updateLightAtt, updateLightSdw;
    bool isSaveLight;
    float isSuccessful;
    float isFailed;
    bool isFailedInit;
    bool isSmallSlider;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    void DrawRect(float x1, float y1, float x2, float y2, uint32_t color);
    void DrawLine(float x1, float y1, float x2, float y2, uint32_t color);
    void DrawLRect(float x1, float y1, float x2, float y2, uint32_t bkgColor, uint32_t lnColor);
    void Print(long color, float xleft, float xright, float y, float scale, bool isAlign, const char *format, ...);
    void DrawCursor();

    bool Slider(long id, float y, const char *text, float &value, float min = 0.0f, float max = 1.0f,
                bool drawVal = false);
    bool ColorPicker(long id, float y, CVECTOR &ref, float st, CVECTOR &res);
    bool Button(float x, float y, float w, float h, const char *text, long *act = nullptr, long init = -1);
    void Checker(float x, float y, const char *text, bool &res);
    long SelPreset();

    void SavePreset(long prs);
    void LoadPreset(long prs);

    char *GenerateName(const char *f, const char *n);

    void UpdateColors();
    void UpdateLight(long idxLight, bool isCos, bool isAtt, bool isSdw);

  private:
    //
    VDX9RENDER *rs;
    // Screen size
    float sw, sh;
    // Window options
    float winx, winy, winw, winh;
    uint32_t bkgColor, frmColor, textColor, selColor;
    float sldLen;
    // Font
    long font;
    float fontHeight;
    // Mouse
    bool isMouseDown;
    bool isActiveMouseState;
    bool isOldMouseState;
    float cursx, cursy;
    // List
    bool isList;
    ListElement *list;
    long numElements;
    long selected;
    float listPos;
    float listWait;
    // Slider
    long slidID;
    float slidDltX;
    // Picker
    bool isPikerActive;
    float pikerWait;
    IDirect3DTexture9 *pickerTexture;
    // Presets
    long lastPreset;
    char prsComment[64];
    // String buffer
    char ver[256];
    char stringBuffer[1024];
};
