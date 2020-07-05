#define GOODS_QUANTITY  44 // boal

#define GOOD_BALLS	    0
#define GOOD_GRAPES	    1
#define GOOD_KNIPPELS	2
#define GOOD_BOMBS	    3
// boal -->
#define GOOD_POWDER     4
#define GOOD_FOOD       5
#define GOOD_WEAPON	    6
#define GOOD_MEDICAMENT 7

#define GOOD_SAILCLOTH  8
#define GOOD_PLANKS     9
#define GOOD_CHOCOLATE	10
#define GOOD_COFFEE	    11
#define GOOD_TOBACCO	12
#define GOOD_SUGAR	    13
#define GOOD_WHEAT      14
#define GOOD_WINE	    15
#define GOOD_RUM	    16
#define GOOD_ALE	    17
#define GOOD_COTTON	    18
#define GOOD_LINEN	    19
#define GOOD_LEATHER	20
#define GOOD_SILK	    21
#define GOOD_CLOTHES	22
#define GOOD_EBONY      23
#define GOOD_MAHOGANY 	24
#define GOOD_SANDAL	    25
#define GOOD_CINNAMON   26
#define GOOD_COPRA      27
#define GOOD_PAPRIKA    28
#define GOOD_FRUITS     29
#define GOOD_OIL        30
#define GOOD_BRICK      31
//
#define GOOD_SLAVES     32
#define GOOD_GOLD       33
#define GOOD_SILVER	    34
//   cannons ............
#define GOOD_CANNON_12   35
#define GOOD_CANNON_16   36
#define GOOD_CANNON_24   37
#define GOOD_CANNON_32   38
#define GOOD_CANNON_42   39

#define GOOD_CULVERINE_12 40
#define GOOD_CULVERINE_16 41
#define GOOD_CULVERINE_24 42
#define GOOD_CULVERINE_32 43
// boal <--

object  Goods[GOODS_QUANTITY];

ref GetGoodByType(int iGoodType)
{
	return &Goods[iGoodType];
}

int FindGood(string sGood)
{
	for (int i=0; i<GOODS_QUANTITY; i++) { if (Goods[i].Name == sGood) return i; }
	return -1;
}

ref GetGoodByID(string sGood)
{
	for (int i=0; i<GOODS_QUANTITY; i++) { 
		if (Goods[i].Name == sGood) {return &Goods[i];}
	}
	trace("WARNING! Goods ID (" + sGood +") not found!");
}

int GetGoodWeightByType(int iGoodType,int quantity)
{
	int unitSize = sti(Goods[iGoodType].Units);
	int unitQuantity = (quantity+unitSize-1)/unitSize;
	int weight = makeint(unitQuantity*stf(Goods[iGoodType].Weight) + 0.05);
	return weight;
}

int GetGoodQuantityByWeight(int iGoodType,int weight)
{
	int unitSize = sti(Goods[iGoodType].Units);
	int uqnt = makeint( weight / stf(Goods[iGoodType].Weight) );
	return unitSize*uqnt;
}