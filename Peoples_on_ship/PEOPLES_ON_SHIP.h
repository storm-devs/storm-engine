#ifndef _PEOPLE_ON_SHIP_H_
#define _PEOPLE_ON_SHIP_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\ship_base.h"
#include "..\common_h\model.h"
#include "..\common_h\geos.h"
#include "TWalkGraph.h"

#define PI  3.14159265358979323846f
#define PIm2 (PI * 2.0f)
#define PId2 (PI / 2.0f)

#define MAX_SHIPS				25
#define MAX_VERTS				100
#define MAX_PEOPLE				100
#define CREW2VISIBLE(n)			5.0f*logf(n)
#define MAN_SPEED_DEVIATION_K	0.35f
#define MAN_MIN_TURN			0.35f //0.55
#define MAX_VIEW_DISTANCE		150.0f

#define MAN_STATE_COUNT			10
#define SHIP_STATE_COUNT		4

#define LOCATOR_WALK   0
#define LOCATOR_CANNON 1
#define LOCATOR_DOOR   2
#define LOCATOR_SPAWN  3
#define LOCATOR_TOP    4
#define LOCATOR_VANT   5
#define LOCATOR_MARS   6
/*
enum tShipState
{
	SHIP_SAIL     = 0,
	SHIP_WAR      = 1,
	SHIP_STORM    = 2,
	SHIP_ABORDAGE = 3
};
*/
enum tManState {
	//common actions (chosen randomly)
	MAN_WALK, 
	MAN_TURN_LEFT, 
	MAN_TURN_RIGHT, 
	MAN_RUN, 
	MAN_STAND1,
	MAN_STAND2,
	MAN_STAND3,
	MAN_STAND4,
	//special actions
	MAN_RELOAD,
	//animated action sequences
	MAN_CRAWL_WALK_VANT,
	MAN_CRAWL_VANT_TOP,
	MAN_CRAWL_TOP_MARS,
	MAN_CRAWL_MARS,
	MAN_CRAWL_MARS_TOP,
	MAN_CRAWL_TOP_VANT,
	MAN_CRAWL_VANT_WALK
};

float crewPart[SHIP_STATE_COUNT] = {0.3f, 0.5f, 0.1f, 1.0f};
float probs[SHIP_STATE_COUNT][MAN_STATE_COUNT]={ 
	{0.7f, 0.0f, 0.0f, 0.1f, 0.05f, 0.05f, 0.05f, 0.05f, 0.00f, 0.30f}, //SAIL
	{0.0f, 0.0f, 0.0f, 0.5f, 0.00f, 0.00f, 0.00f, 0.00f, 0.99f, 0.10f}, //WAR
	{0.1f, 0.0f, 0.0f, 0.9f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.05f}, //STORM
	{0.0f, 0.0f, 0.0f, 1.0f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.10f}, //ABORDAGE
};

struct tShipMan
{
	int sourceI, destI;
	CVECTOR pos;
	float ang;
	CVECTOR dir;
	float speed;
	MODEL *model;
	tManState state;
	float newAngle;
	bool visible;
	ENTITY_ID modelID;
};

struct tShipWalk
{
	SHIP_BASE *ship;
	MODEL *shipModel;
	TWalkGraph graph;
	CVECTOR verts[MAX_VERTS];
	byte vertTypes[MAX_VERTS];
	dword vertBusy[MAX_VERTS];
	int vertsCount;

	int crewCount;
	int showCount;
	tShipMan crew[MAX_PEOPLE];
	int state;
	bool enabled;
	bool myShip;
};

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class PEOPLE_ON_SHIP : public ENTITY 
{
public:
	virtual ~PEOPLE_ON_SHIP();
	PEOPLE_ON_SHIP();

	virtual bool Init();
	virtual void Realize(dword _dTime);
	virtual void Execute(dword _dTime);
	virtual dword _cdecl ProcessMessage(MESSAGE &message);
	virtual dword AttributeChanged(ATTRIBUTES *_newAttr);

protected:
	void SetShipState(tShipWalk *_shipWalk, int _newState);

private:
	bool ProcessManCrawl(tShipWalk *_shipWalk, tShipMan *_man, dword _dTime);
	bool ProcessManWalk(tShipWalk *_shipWalk, tShipMan *_man, dword _dTime);
	bool ProcessManTurn(tShipWalk *_shipWalk, tShipMan *_man, dword _dTime);
	bool ProcessManStand(tShipWalk *_shipWalk, tShipMan *_man, dword _dTime);
	void ChooseNewAction(tShipWalk *_shipWalk, tShipMan *_man);

	void AddShipWalk(ENTITY_ID &_shipID, int vCount, VDATA *vArray, VDATA *gArray, VDATA *tArray);
	void InitShipMan(int _shipN, int _manN);
	float Vector2Angle(const CVECTOR &_v);
	void SetManVisible (tShipMan *_man, bool _visible);

	VDX8RENDER *renderer;

	PLANE clipPlanes[4];
	CVECTOR cameraPosition;

	tShipWalk shipWalk[MAX_SHIPS];
	int shipsCount;
	float manSpeed, manRunSpeed, manTurnSpeed, approachDist;
	bool cameraOnDeck;
	bool hidePeopleOnDeck;
	bool isNight;
};

#endif