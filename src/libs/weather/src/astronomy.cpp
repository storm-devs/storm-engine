#include "astronomy.h"

#include "core.h"

VDX9RENDER *Astronomy::pRS = nullptr;
VGEOMETRY *Astronomy::pGS = nullptr;

Astronomy::Astronomy()
{
    dTimeScale = 0.0;
    dHour = 0.0f;
}

Astronomy::~Astronomy()
{
}

bool Astronomy::Init()
{
    SetDevice();

    return true;
}

void Astronomy::SetDevice()
{
    pRS = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    Assert(pRS);
    pGS = static_cast<VGEOMETRY *>(core.GetService("geometry"));
}

void Astronomy::Realize(uint32_t Delta_Time)
{
    const auto dDeltaTime = static_cast<double>(Delta_Time) * 0.001;
    dHour += dDeltaTime / (60.0 * 60.0);

    Stars.Realize(dDeltaTime, dHour);
    Planets.Realize(dDeltaTime, dHour);
}

uint32_t Astronomy::AttributeChanged(ATTRIBUTES *pA)
{
    if (*pA == "isDone")
    {
        Stars.Init(AttributesPointer);
        Planets.Init(AttributesPointer);
        return 0;
    }

    if (*pA == "TimeScale")
    {
        dTimeScale = static_cast<double>(pA->GetAttributeAsFloat());
        return 0;
    }

    if (*pA == "TimeUpdate")
    {
        Planets.TimeUpdate(AttributesPointer);
        Stars.TimeUpdate(AttributesPointer);
        return 0;
    }

    return 0;
}
