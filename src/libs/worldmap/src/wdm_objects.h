//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmObjects
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "matrix.h"
#include "dx9render.h"

#include "wdm_objects_wind.h"

#include <string>
#include <vector>
#include <optional>

class WdmIslands;
class WdmShip;
class WdmPlayerShip;
class WorldMap;
class WdmCamera;
class WdmLocations;
class WdmStorm;
class WdmEnemyShip;

//#define WDM_WORLD_SIZE_X    2000.0f
//#define WDM_WORLD_SIZE_Z    2000.0f

enum EnemyShipType
{
    wdmest_unknow,
    wdmest_merchant,
    wdmest_warring,
    wdmest_follow,
};

class GEOS;
class VGEOMETRY;

class WdmObjects
{
    struct Model
    {
        GEOS *geo;
        std::string path;
        uint32_t hash;
        int32_t next;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmObjects();
    virtual ~WdmObjects();

    void SetWorldSize(float x, float z);
    void Clear();

    void AddShip(WdmShip *ship);
    void DelShip(WdmShip *ship);

    void AddStorm(WdmStorm *storm);
    void DelStorm(WdmStorm *storm);

    // Create geometry
    GEOS *CreateGeometry(const char *path);

    // Engine object, which is in charge of everything
    WorldMap *wm;
    // Render service
    VDX9RENDER *rs;
    // Geometry service
    VGEOMETRY *gs;
    // Camera
    WdmCamera *camera;
    // Islands
    WdmIslands *islands;

    // The ships
    // Player ship
    WdmShip *playerShip;
    // All existing ships
    std::vector<WdmShip *> ships;

    WdmEnemyShip *enemyShip;
    bool enableSkipEnemy;

    // Storm
    std::vector<WdmStorm *> storms;
    bool playarInStorm;

    const char *curIsland;

    bool isPause;
    bool isDebug;

    void DrawCircle(const CVECTOR &pos, float radius, uint32_t color) const;
    void DrawCircle(CMatrix &mtx, float radius, uint32_t color) const;
    void DrawVector(const CVECTOR &start, const CVECTOR &end, uint32_t color) const;
    void DrawLine(const CVECTOR &start, const CVECTOR &end, uint32_t color) const;
    void DrawBox2D(CMatrix &mtx, float l, float w, uint32_t color) const;
    void GetVPSize(float &w, float &h) const;

    float shipSpeedOppositeWind; // Relative speed of the ship against the wind
    float shipSpeedOverWind;     // Relative speed of the ship downwind

    float enemyshipViewDistMin; // The distance at which the ship begins to disappear
    float enemyshipViewDistMax; // The distance at which the ship disappears completely
    float enemyshipDistKill;    // The distance at which the ship is killed
    float enemyshipBrnDistMin;  // The minimum distance at which a ship is spawned
    float enemyshipBrnDistMax;  // The maximum distance at which the ship is spawned

    float stormViewDistMin; // The distance at which the storm begins to fade
    float stormViewDistMax; // The distance at which the storm disappears completely
    float stormDistKill;    // The distance at which the storm is killed
    float stormBrnDistMin;  // The minimum distance at which a storm is spawned
    float stormBrnDistMax;  // The maximum distance at which a storm is spawned
    float stormZone;        // Total storm radius

    char attrSec[256];    // Seconds at the current frame
    char attrMin[256];    // Minutes at the current frame
    char attrHour[256];   // Clock at the current frame
    char attrDay[256];    // Day at the current frame
    char attrMonth[256];  // Month at the current frame
    char attrYear[256];   // Year at the current frame
    bool isNextDayUpdate; // Time to update next day data

    std::optional<int32_t> nationFlagIndex; // national flag index

    std::string coordinate; // to output the coordinate string
    char stCoordinate[128];

    float resizeRatio; // for resizing interfaces
    float worldSizeX;  // The size of the world by X
    float worldSizeZ;  // The size of the world by Z

    std::vector<Model> models; // Models
    int32_t entryModels[1024];    // Geometry Quick Find Table
    std::string modelPath;

    // Get wind direction and strength
    float GetWind(float x, float z, CVECTOR &dir);
    // Update wind state
    void UpdateWind(float dltTime);
    // Get save string
    const char *GetWindSaveString(std::string &windData);
    // Set save string
    void SetWindSaveString(const char *str);
    // Add float to string
    void AddDataToString(std::string &str, uint8_t d);
    // Get float from string
    int32_t GetDataFromString(const char *&cur);

    WindField windField;

  private:
    struct Vertex
    {
        CVECTOR v;
        uint32_t c;
    };

    static Vertex vertex[1024];
};

extern WdmObjects *wdmObjects;
