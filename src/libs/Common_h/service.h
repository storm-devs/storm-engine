#pragma once

//-------------------------------------------------------------------------------------------------
// Base class for SERVICE is a nothing more than base entity class with three new functions:
// Run_Start() and Run_End() These functions mark beginning and ending of the main programm loop
// Run_Section() this function used to determine, from which objects function this service may be
// called.
// For example: if function returned SECTION_REALIZE value, function Run_Start() would be called
// before core start transfer program control to objects Realize() functions and call Run_End()
// after program control leave objects Realize sections

#include "entity_state.h"

#define SECTION_ALL 0x0
#define SECTION_EXECUTE 0x1
#define SECTION_REALIZE 0x2
#define SECTION_PROCESS_MESSAGE_SYSTEM 0x4
#define SECTION_NEVER 0xffffffff

#define SD_SERVERMESSAGE (WM_USER + 0)
#define SD_CLIENTMESSAGE (WM_USER + 1)

class SERVICE // : public Entity
{
  public:
    virtual ~SERVICE() = default;

    virtual void RunStart()
    {
    }

    virtual void RunEnd()
    {
    }

    virtual uint32_t RunSection()
    {
        return SECTION_ALL;
    }
    virtual bool Init()
    {
        return true;
    }
    virtual bool LoadState(ENTITY_STATE *state)
    {
        return true;
    }
    virtual bool CreateState(ENTITY_STATE_GEN *state_gen)
    {
        return true;
    }
};
