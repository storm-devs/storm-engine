#pragma once

#include "editor_defines.h"

class XINTERFACE;
class CXI_IMAGE;
class GIEditorList;
class CINODE;
class VDX9RENDER;

class GIEditor : public GIEditorObject
{
  public:
    GIEditor(GIEditor &&) = delete;
    GIEditor(const GIEditor &) = delete;
    GIEditor(XINTERFACE *pInterface);
    ~GIEditor();

    void Release();

    void Render() const;
    bool ProcessControl();

    bool IsShowMode() const;
    void MakeShowMode(bool bShow);

    void SetEditNode(CINODE *pNode);

    void ReCreate() const;
    void AddNode(CINODE *pNode) const;
    void DelNode(CINODE *pNode) const;

    void DrawSizeBox() const;

  protected:
    bool m_bShowMode;
    CINODE *m_pEditableNode;
    float m_fLastKeyPressTime;

    CXI_IMAGE *m_pImageBack;

    GIEditorList *m_pNodeList;
    bool m_bSubNameOn;
    GIEditorList *m_pSubNameList;

  public:
    VDX9RENDER *m_pRS;
    XINTERFACE *m_pGIOwner;

    void ChangeNodeName();
    void ChangeSubNodeName() const;
};
