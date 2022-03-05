//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WorldMap
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "entity.h"
#include <string>

#define WDMAP_MAXOBJECTS 4096

class ATTRIBUTES;
class MESSAGE;
class WdmRenderObject;
class WdmRenderModel;
class VDX9RENDER;
class WdmCamera;
class WdmEventWindow;
class WdmWaitMenu;

class WorldMap : public Entity
{
    struct RObject
    {
        WdmRenderObject *ro;
        int32_t level;
        int32_t next;
        int32_t prev;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WorldMap();
    ~WorldMap() override;

    //--------------------------------------------------------------------------------------------
    // Entity
    //--------------------------------------------------------------------------------------------
  public:
    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    // Messages
    uint64_t ProcessMessage(MESSAGE &message) override;
    // Changing an attribute
    uint32_t AttributeChanged(ATTRIBUTES *apnt) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    // --------------------------------------------------------------------------------------------
    // Objects management
    // --------------------------------------------------------------------------------------------
  public:
    // Add object
    WdmRenderObject *AddObject(WdmRenderObject *obj, int32_t level = 0);
    // Add object to render list before reflection
    void AddPObject(WdmRenderObject *obj, int32_t level = 0);
    // Add object to reflection render list
    void AddMObject(WdmRenderObject *obj, int32_t level = 0);
    // Add object to render list after reflection
    void AddLObject(WdmRenderObject *obj, int32_t level = 0);
    // Delete object
    void DeleteObject(WdmRenderObject *obj);

    // Initialize the model and add it to the required render lists
    WdmRenderObject *CreateModel(WdmRenderModel *rm, const char *modelName, bool pr = false, bool mr = true,
                                 bool lr = true, int32_t objectLevel = 0, int32_t drawLevel = 0);

    //--------------------------------------------------------------------------------------------

    VDX9RENDER *GetRS() const;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Objects management
    // Include a record about an object in the list with the required level
    int32_t GetObject(int32_t &first, int32_t level);
    // Exclude entry from the list
    void FreeObject(int32_t &first, int32_t i);

    // Utilities
    // Create a storm if possible
    bool CreateStorm(bool isTornado, float time = -1.0f, ATTRIBUTES *save = nullptr);
    // Create a merchant's ship
    bool CreateMerchantShip(const char *modelName, const char *locNameStart, const char *locNameEnd, float kSpeed,
                            float time = -1.0f, ATTRIBUTES *save = nullptr);
    // boal Create a merchant's ship in coordinates
    bool CreateMerchantShipXZ(const char *modelName, float x1, float z1, float x2, float z2, float kSpeed,
                              float time = -1.0f, ATTRIBUTES *save = nullptr);
    // Create a ship that follows us
    bool CreateFollowShip(const char *modelName, float kSpeed, float time = -1.0f, ATTRIBUTES *save = nullptr);
    // Create a pair of warring ships
    bool CreateWarringShips(const char *modelName1, const char *modelName2, float time = -1.0f,
                            ATTRIBUTES *save1 = nullptr, ATTRIBUTES *save2 = nullptr);
    // Delete all encounters
    void ReleaseEncounters();
    // Create an attribute to save the encounter parameters
    ATTRIBUTES *GetEncSaveData(const char *type, const char *retName);

    // Find coordinates and radius by destination
    bool FindIslandPosition(const char *name, float &x, float &z, float &r);

    void ResetScriptInterfaces() const;

  private:
    // Render service
    VDX9RENDER *rs;
    WdmCamera *camera;

    ATTRIBUTES *aStorm;
    ATTRIBUTES *aEncounter;
    ATTRIBUTES *aInfo;
    ATTRIBUTES *saveData;

    // Event
    float encTime;

    // Objects
    int32_t firstFreeObject;
    int32_t firstObject;   // All existing objects
    int32_t firstPrObject; // Objects rendering before the mirror
    int32_t firstMrObject; // Objects rendered into the mirror
    int32_t firstLrObject; // Objects rendered after the mirror

    RObject object[WDMAP_MAXOBJECTS];

    ATTRIBUTES *aDate;
    float timeScale;

    std::string bufForSave;

  public:
    uint32_t encCounter;

    float hour;
    int32_t day;
    int32_t mon;
    int32_t year;

    static int32_t month[];
};

inline VDX9RENDER *WorldMap::GetRS() const
{
    return rs;
}
