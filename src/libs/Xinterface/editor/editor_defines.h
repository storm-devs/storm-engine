#pragma once
#include <vector>

enum GIEditorStates
{
    GIState_Nothing = 0,

    GIState_ListChange = 1,

    GIState_ForcedDword = 0xFFFFFFFF
};

class GIEditorEventHandler;
class GIEditorObject;

typedef void (GIEditorObject::*GIEditorEvent)();

class GIEditorObject
{
  public:
    virtual ~GIEditorObject()
    {
    }

  public:
    void LinkEvent(GIEditorEventHandler *pEventHandler, const GIEditorEvent &pEventFunction);
};

class GIEditorEventHandler
{
  public:
    GIEditorEventHandler();
    ~GIEditorEventHandler();

    bool Execute();

    void AddEventFunction(GIEditorObject *pObj, const GIEditorEvent &pEventFunction);

  protected:
    struct FuncDescr
    {
        GIEditorObject *pObj;
        GIEditorEvent func;
    };

    std::vector<FuncDescr> m_aEventFuncs;
};
