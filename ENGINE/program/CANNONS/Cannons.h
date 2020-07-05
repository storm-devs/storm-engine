#define CANNON_TYPES_QUANTITY		10

//#define CANNON_TYPE_CULVERINE_LBS8	0
#define CANNON_TYPE_CULVERINE_LBS12 0
#define CANNON_TYPE_CULVERINE_LBS16 1
#define CANNON_TYPE_CULVERINE_LBS24 2
#define CANNON_TYPE_CULVERINE_LBS32 3
//#define CANNON_TYPE_CANNON_LBS8	4
#define CANNON_TYPE_CANNON_LBS12    4
#define CANNON_TYPE_CANNON_LBS16	5
#define CANNON_TYPE_CANNON_LBS24    6
#define CANNON_TYPE_CANNON_LBS32    7
//#define CANNON_TYPE_CANNON_LBS36	9
#define CANNON_TYPE_CANNON_LBS42    8
#define CANNON_TYPE_CANNON_LBS48    9
//#define CANNON_TYPE_MORTAR			12

#define CANNON_TYPE_NONECANNON		1000

#define CANNON_NAME_CANNON              1
#define CANNON_NAME_CULVERINE           2
#define CANNON_NAME_SPECIAL_CANNON		3
//#define CANNON_NAME_MORTAR				4

object  Cannon[CANNON_TYPES_QUANTITY];

ref GetCannonByType(int iCannonType) 
{ 
	if (iCannonType == CANNON_TYPE_NONECANNON) return &NullCharacter; // fix для тартант boal
	return &Cannon[iCannonType]; 
}

int GetCannonCaliber(int nCannon)
{
    switch(nCannon)
    {
    case CANNON_TYPE_NONECANNON:
        return 0;
        break;
		/*case CANNON_TYPE_CULVERINE_LBS8:
        return 8;
        break;
    case CANNON_TYPE_CANNON_LBS8:
        return 8;
        break;*/
    case CANNON_TYPE_CULVERINE_LBS12:
        return 12;
        break;
    case CANNON_TYPE_CANNON_LBS12:
        return 12;
        break;
    case CANNON_TYPE_CULVERINE_LBS16:
        return 16;
        break;
    case CANNON_TYPE_CANNON_LBS16:
        return 16;
        break;
    case CANNON_TYPE_CULVERINE_LBS24:
        return 24;
        break;
    case CANNON_TYPE_CANNON_LBS24:
        return 24;
        break;
    case CANNON_TYPE_CULVERINE_LBS32:
        return 32;
        break;
    case CANNON_TYPE_CANNON_LBS32:
        return 32;
        break;
    /*case CANNON_TYPE_CANNON_LBS36:
        return 36;
        break;  */
    case CANNON_TYPE_CANNON_LBS42:
        return 42;
        break;
    case CANNON_TYPE_CANNON_LBS48:
        return 48;
        break;
	/*case CANNON_TYPE_MORTAR:
        return 92;
        break; */
    }
}