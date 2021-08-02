#include "Animals.h"

#include "shared/messages.h"

CREATE_CLASS(ANIMALS)

ANIMALS::ANIMALS() : seagulls(nullptr), fishSchools(nullptr), butterflies(nullptr)
{
    seagulls = std::make_unique<TSeagulls>();
    fishSchools = std::make_unique<TFishSchools>();
    butterflies = std::make_unique<TButterflies>();
}

bool ANIMALS::Init()
{
    EntityManager::AddToLayer(REALIZE, GetId(), 77);
    EntityManager::AddToLayer(EXECUTE, GetId(), 77);

    seagulls->Init();
    fishSchools->Init();
    butterflies->Init();

    return true;
}

uint64_t ANIMALS::ProcessMessage(MESSAGE &message)
{
    const auto code = message.Long();
    uint64_t outValue = 0;

    switch (code)
    {
    case MSG_SOUND_SET_MASTER_VOLUME:
        break;
    default:
        outValue = seagulls->ProcessMessage(code, message);
        if (outValue)
        {
            return outValue;
        }            
        outValue = fishSchools->ProcessMessage(code, message);
        if (outValue)
        {
            return outValue;
        }            
        outValue = butterflies->ProcessMessage(code, message);
        if (outValue)
        {
            return outValue;
        }      
        break;
    }

    return outValue;
}

void ANIMALS::Realize(const uint32_t _dTime)
{
    seagulls->Realize(_dTime);
    fishSchools->Realize(_dTime);
    butterflies->Realize(_dTime);
}

void ANIMALS::Execute(const uint32_t _dTime)
{
    seagulls->Execute(_dTime);
    fishSchools->Execute(_dTime);
    butterflies->Execute(_dTime);
}

uint32_t ANIMALS::AttributeChanged(ATTRIBUTES *_pA)
{
    if (*_pA == "midY")
    {
        seagulls->SetStartY(this->AttributesPointer->GetAttributeAsFloat("midY"));
    }

    return 0;
}
