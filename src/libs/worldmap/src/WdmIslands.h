//============================================================================================
//    Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//    Sea Dogs II
//--------------------------------------------------------------------------------------------
//    WdmIslands
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "PtcData.h"
#include "WdmRenderModel.h"
#include "geometry.h"

class WdmIslandWaves : public WdmRenderModel
{
  public:
    WdmIslandWaves();

    void Update(float dltTime) override;
    void LRender(VDX9RENDER *rs) override;
    void Render(VDX9RENDER *rs, float k);

  private:
    float phase;
};

class WdmIslands : public WdmRenderObject
{
    struct Islands
    {
        WdmRenderModel *model; // Island model
        WdmRenderModel *area;  // Island area model
        WdmRenderModel *palms; // Model with palm trees
        WdmIslandWaves *waves; // Model with foam
        Matrix toLocal;       // Conversion to the local island system
        std::string modelName; // Island Model Name
        Vector worldPosition; // The position of the island in the world
    };

    struct Label
    {
        std::string text;        // Label text
        Vector pos;             // Label position
        float l, t, r, b;        // Rectangle describing the label in screen coordinates
        float dl, dt, dr, db;    // Offsets to get a rectangle at a known point on the screen
        float textX, textY;      // Relative text position
        float iconX, iconY;      // The relative position of the picture
        float alpha;             // The current state of the rectangle
        float heightView;        // Height from which the label goes out
        long font;               // Font index in font array
        long icon;               // Image index
        uint32_t weight;         // Offset weight
        std::string id;          // Label ID
        uint32_t idHash;         // Hash value of identifier
        long next;               // Next label on the list
        std::string locatorName; // The name of the locator in which it is located
    };

    struct Font
    {
        std::string name; // Font name
        long id;          // Its identifier
    };

    struct Icons
    {
        float w, h;
        float u, v;
        uint32_t num;
        uint32_t frames;
        float fps;
        float frame;
        float f[2];
        long blend;
        long texture;
    };

    struct Quest
    {
        Vector pos;
        std::string name;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmIslands(WdmIslands &&) = delete;
    WdmIslands(const WdmIslands &) = delete;
    WdmIslands();
    ~WdmIslands() override;

    // Check for possible collision
    bool CollisionTest(Matrix &objMtx, float length, float width, bool heighTest = true);
    // Check for the presence of triangles in this place
    bool ObstacleTest(float x, float z, float radius);

    // Read Island Data
    void SetIslandsData(ATTRIBUTES *apnt, bool isChange);

    // Find the direction to arrive at a given destination from the current
    void FindDirection(const Vector &position, const Vector &destination, Vector &direction) const;
    // Find the repulsive force
    void FindReaction(const Vector &position, Vector &reaction) const;
    // Find a random point for a merchant
    bool GetRandomMerchantPoint(Vector &p);
    // Get the coordinates of the quest locator
    bool GetQuestLocator(const char *locName, Vector &p);

    // Check if the boat is in the island zone
    bool CheckIslandArea(const char *islandName, float x, float z);
    // Get the closest point to the island zone
    void GetNearPointToArea(const char *islandName, float &x, float &z);

    void Update(float dltTime) override;
    void LRender(VDX9RENDER *rs) override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    bool IsShipInArea(long islIndex, const Vector &pos);
    static bool AddEdges(const GEOS::VERTEX *vrt, long numVrt);
    static bool FindNearPoint(const GEOS::VERTEX *vrt, long numVrt);
    void LabelsReadIconParams(ATTRIBUTES *apnt);
    long LabelsFind(const char *id, uint32_t hash);
    bool LabelsFindLocator(const char *name, Vector &pos) const;
    long LabelsAddFont(const char *name);
    void LabelsRelease();
    static Vector Norm2D(const Vector &ret);

  private:
    // Model containing all locators
    WdmRenderModel *baseModel;
    // Pathfinder patch
    PtcData *patch;
    // Island models
    std::vector<Islands> islands;
    // Labels
    std::vector<Label> labels;
    // Fonts used by labels
    std::vector<Font> fonts;
    // Images
    Icons icons;
    // Labels registered for rendering
    std::vector<long> labelSort;
    // Merchant destinations
    std::vector<Vector> merchants;
    // Destination Points of Quest Encounters
    std::vector<Quest> quests;

    // Input table for finding labels
    long labelsEntry[1024];

    static Matrix curMatrix, locMatrix;
    static long numEdges;
    static Vector curPos;
    static bool checkMode;

  public:
    static Vector centPos;
};

inline Vector WdmIslands::Norm2D(const Vector &v)
{
    auto ret = v;
    ret.y = 0.0f;
    double len = ret.x * ret.x + ret.z * ret.z;
    if (len >= 1e-30f)
    {
        len = 1.0f / sqrt(len);
        ret.x = static_cast<float>(len * ret.x);
        ret.z = static_cast<float>(len * ret.z);
    }
    else
    {
        ret = 0.0f;
    }
    return ret;
}
