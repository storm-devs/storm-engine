#ifndef MousePointer_h
#define MousePointer_h

#include "BaseManager.h"

#define BI_CURSORS_QUANTITY 10
#define BI_CURSOR_COMMON 0

class IBIImage;

class MousePointer : public BI_MousePointerBase
{
  public:
    MousePointer(BI_ManagerBase *pManager, ATTRIBUTES *pARoot);
    ~MousePointer();

    virtual void Update();

  protected:
    BI_ManagerBase *m_pManager;
    ATTRIBUTES *m_pARoot;

    IBIImage *m_pIcon;
    FPOINT m_mousepos;
    FPOINT m_mousesensivity;
    RECT m_cursorzone;

    struct MouseCursorInfo
    {
        POINT offset;
        string texture;
        FRECT uv;
    };
    MouseCursorInfo m_aCursors[BI_CURSORS_QUANTITY];
    long m_nCurrentCursor;
    POINT m_cursorsize;

    void InitMouseCursors();
    void MoveCursor();
    void SetCurrentCursor();
    RECT GetCurrentCursorIconPos();
};

#endif
