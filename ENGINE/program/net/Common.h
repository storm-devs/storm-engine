#include "Net\Common\Cannons.c"
#include "Net\Common\Goods.c"
#include "Net\Common\Islands.c"
#include "Net\Common\Perks.c"
#include "Net\Common\Ships.c"
#include "Net\Common\Skills.c"
#include "Net\Common\Rewards.c"
#include "Net\Common\Upgrades.c"

#define NET_DEBUG				0
//#define NET_DEBUG				1
#define CONVOY_POINT_RADIUS		40.0

#define NET_TEST_DWORD			12345678		// hack
#define NET_SCRIPT_VERSION		101

string sUserFacesPath = "net\faces\";
string sUserSailsPath = "net\sails\";
string sUserFlagsPath = "net\flags\";

string	sNetBortNames[4] = {"cannonl", "cannonr", "cannonf", "cannonb"};
object	NetModes[4];
int		NetTeamColor[4];
string	NetTeamName[4];
