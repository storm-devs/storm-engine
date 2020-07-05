#define PRICE_TYPE_BUY	0
#define PRICE_TYPE_SELL	1

#event_handler("NextDay","StoreDayUpdateStart");
#event_handler("EvStoreDayUpdate","StoreDayUpdate");

void SetStoreGoods(ref _refStore,int _Goods,int _Quantity)
{
	string tmpstr = Goods[_Goods].name;
	_refStore.Goods.(tmpstr).Quantity = _Quantity;
}

void AddStoreGoods(ref _refStore,int _Goods,int _Quantity)
{
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	makearef(refGoods,_refStore.Goods.(tmpstr));
	int q = 0;
	if( CheckAttribute(refGoods,"Quantity") ) {q = sti(refGoods.Quantity);}
	refGoods.Quantity = q +_Quantity;
}

void RemoveStoreGoods(ref _refStore,int _Goods,int _Quantity)
{
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	makearef(refGoods,_refStore.Goods.(tmpstr));
	int q = 0;
	if( CheckAttribute(refGoods,"Quantity") ) q = sti(refGoods.Quantity);
	if(q<_Quantity) {refGoods.Quantity = 0;}
	else {refGoods.Quantity = q - _Quantity;}
}

int GetStoreGoodsQuantity(ref _refStore,int _Goods)
{
	string tmpstr = Goods[_Goods].name;
	int q = 0;
	if( CheckAttribute(_refStore,"Goods."+tmpstr) )
		q = sti(_refStore.Goods.(tmpstr).Quantity);
	return q;
}
int GetStoreGoodsPrice(ref _refStore, int _Goods, int _PriceType, ref chref, int _qty)
{
	float _TradeSkill = GetSummonSkillFromNameToOld(chref,SKILL_COMMERCE);
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	int basePrice = MakeInt(Goods[_Goods].Cost);
	if (!CheckAttribute(_refStore,"Goods."+tmpstr) ) return 0;
	makearef(refGoods,_refStore.Goods.(tmpstr));
 	int tradeType = MakeInt(refGoods.TradeType);

	float tradeModify = 1.0;

	switch (tradeType)
	{
		case TRADE_TYPE_NORMAL:
			tradeModify = 0.85 + stf(refGoods.RndPriceModify); //0.8
			break;
		case TRADE_TYPE_EXPORT:
			tradeModify = 0.55 + stf(refGoods.RndPriceModify); //0.3
			break;
		case TRADE_TYPE_IMPORT:
			tradeModify = 1.2 + stf(refGoods.RndPriceModify); //1.3
			break;
		case TRADE_TYPE_CONTRABAND:
			tradeModify = 2.4 + stf(refGoods.RndPriceModify); //3.1
			break;
		case TRADE_TYPE_AMMUNITION:
			//return basePrice; делаю все тоже, что и дл€ нормального товара, а тип нужен, чтоб на корабле не скупали лишнее.
			tradeModify = 0.85 + stf(refGoods.RndPriceModify);
			break;  
		case TRADE_TYPE_CANNONS:
			tradeModify = 0.85 + stf(refGoods.RndPriceModify); //0.8
			break;	
	}

	float skillModify;
	if(_PriceType==PRICE_TYPE_BUY)
	{
		skillModify = 1.46 - _TradeSkill*0.019; // boal 20.01.2004
		if(CheckOfficersPerk(chref,"AdvancedCommerce"))	{ skillModify -= 0.2; }
		else
		{
			if(CheckOfficersPerk(chref,"BasicCommerce"))	{ skillModify -= 0.1; }
		}
	}
	else
	{
		skillModify = 0.69 + _TradeSkill*0.019; // boal
		if(CheckOfficersPerk(chref,"AdvancedCommerce"))	skillModify += 0.05;
	}

	// boal 23.01.2004 -->
	if (MakeInt(basePrice*tradeModify*skillModify) < 1) return 1;
	// boal 23.01.2004 <--
    return MakeInt(basePrice*tradeModify*skillModify*_qty);
}

// обратное преобразование цены в RndPriceModify
float GetStoreGoodsRndPriceModify(ref _refStore, int _Goods, int _PriceType, ref chref, int _price)
{
    float skillModify;
	float _TradeSkill = GetSummonSkillFromNameToOld(chref,SKILL_COMMERCE);
	aref refGoods;
	string tmpstr = Goods[_Goods].name;
	int basePrice = MakeInt(Goods[_Goods].Cost);
	if (!CheckAttribute(_refStore,"Goods."+tmpstr) ) return 0;
	makearef(refGoods,_refStore.Goods.(tmpstr));
 	int tradeType = MakeInt(refGoods.TradeType);

	float tradeModify = 1.0;

    if(_PriceType==PRICE_TYPE_BUY)
	{
		skillModify = 1.46 - _TradeSkill*0.019;
		if(CheckOfficersPerk(chref,"AdvancedCommerce"))	{ skillModify -= 0.2; }
		else
		{
			if(CheckOfficersPerk(chref,"BasicCommerce"))	{ skillModify -= 0.1; }
		}
	}
	else
	{
		skillModify = 0.69 + _TradeSkill*0.019;
		if(CheckOfficersPerk(chref,"AdvancedCommerce"))	skillModify += 0.05;
	}
	tradeModify = makefloat(_price) / (basePrice*skillModify);
	
	switch (tradeType)
	{
		case TRADE_TYPE_NORMAL:
			tradeModify = tradeModify - 0.85;
			break;
		case TRADE_TYPE_EXPORT:
			tradeModify = tradeModify - 0.55;
			break;
		case TRADE_TYPE_IMPORT:
			tradeModify = tradeModify - 1.2;
			break;
		case TRADE_TYPE_CONTRABAND:
			tradeModify = tradeModify - 2.4;
			break;
		case TRADE_TYPE_AMMUNITION:
			tradeModify = tradeModify - 0.85;
			break;
		case TRADE_TYPE_CANNONS:
			tradeModify = tradeModify - 0.85;
			break;	
	}
	 // может быть минусом, а что делать :(
    return tradeModify;
}
// <--
string GetStoreGoodsType(ref _refStore,int _Goods)
{
	string tmpstr = Goods[_Goods].name;
	int tradeType = TRADE_TYPE_NORMAL;
	if( CheckAttribute(_refStore,"Goods."+tmpstr) ) {
		tradeType = sti(_refStore.Goods.(tmpstr).TradeType);
	}

	switch(tradeType)
	{
	case TRADE_TYPE_NORMAL:
		return TRADE_TYPE_NORMAL_NAME;
		break;
	case TRADE_TYPE_EXPORT:
		return TRADE_TYPE_EXPORT_NAME;
		break;
	case TRADE_TYPE_IMPORT:
		return TRADE_TYPE_IMPORT_NAME;
		break;
	case TRADE_TYPE_CONTRABAND:
		return TRADE_TYPE_CONTRABAND_NAME;
		break;
	case TRADE_TYPE_AMMUNITION:
		return TRADE_TYPE_AMMUNITION_NAME;
		break;
	case TRADE_TYPE_CANNONS:   //new boal
		return TRADE_TYPE_CANNONS_NAME;
		break;		
	}
	Trace("Missing trade type");
	return TRADE_TYPE_NORMAL_NAME;
}

bool GetStoreGoodsUsed(ref _refStore,int _Goods)
{
	string tmpstr = Goods[_Goods].name;
	if( !CheckAttribute(_refStore,"Goods."+tmpstr) ) return false;
	if( sti(_refStore.Goods.(tmpstr).NotUsed)==true ) return false;
	if( sti(_refStore.Goods.(tmpstr).TradeType) == TRADE_TYPE_CONTRABAND )
	{	if( !CheckOfficersPerk(GetMainCharacter(),"Trustworthy") ) return false;
	}
	return true;
}

void SetStoresTradeUsed(int StoreNum,int GoodsNum,bool goodsUsed)
{
	string tmpstr = Goods[GoodsNum].Name;
	Stores[StoreNum].Goods.(tmpstr).NotUsed = !goodsUsed;
}

int storeDayUpdateCnt = -1;
void StoreDayUpdateStart()
{	
	if(storeDayUpdateCnt >= 0) return;
	storeDayUpdateCnt = 0;
	PostEvent("EvStoreDayUpdate", 30);

	Event("EvSituationsUpdate", "l", 0);   // вызов размазаных вычислений на Ќехтƒай
}

void StoreDayUpdate()
{
	if(storeDayUpdateCnt >= 0)
	{
		UpdateStore(&Stores[storeDayUpdateCnt]);
		storeDayUpdateCnt++;
		if(storeDayUpdateCnt >= STORE_QUANTITY) storeDayUpdateCnt = -1;
		PostEvent("EvStoreDayUpdate", 30);
	}
}

void UpdateStore(ref pStore)
{
	bool bigSize = (pStore.StoreSize=="large");
	aref gref,curref;
	makearef(gref,pStore.Goods);
	int nq = GetAttributesNum(gref);
	int n;
	string tmpstr;
	bool   bIType;
	for(int i=0; i<GOODS_QUANTITY; i++)
	{
		tmpstr = Goods[i].name;
		if( !CheckAttribute(gref,tmpstr) ) continue;
		makearef(curref,gref.(tmpstr));
		// boal 25.01.2004 -->
		/*if( sti(curref.TradeType)==TRADE_TYPE_AMMUNITION ) {
			curref.Quantity = 1000+Rand(50)*20;
			continue;
		}
		if( i==GOOD_SAILCLOTH || i==GOOD_PLANKS ) {
			curref.Quantity = 600+Rand(100);
			continue;
		}*/
		bIType = (i==GOOD_GOLD) || (i==GOOD_SILVER) || (i==GOOD_WEAPON) || (i==GOOD_MEDICAMENT);
		if(  bIType && sti(curref.Quantity) < 20 && sti(curref.TradeType) == TRADE_TYPE_IMPORT)
        {
			curref.Quantity = 140 + Rand(125);
			if (!bigSize) curref.Quantity = makeint(sti(curref.Quantity) / 2);
			continue;
		}
		if( sti(curref.Quantity) < 80 && sti(curref.TradeType) == TRADE_TYPE_IMPORT)
        {
			curref.Quantity = 500 + Rand(500);
			if (!bigSize) curref.Quantity = makeint(sti(curref.Quantity) / 2);
			continue;
		}

		// boal 25.01.2004 <--
		n = sti(curref.Quantity) - GetConsumeGoods(sti(curref.TradeType),bigSize, i); // boal 27.01.2004
		if(n<0) n=0;
		n += GetProductGoods(sti(curref.TradeType),bigSize, i); // boal 27.01.2004
		curref.RndPriceModify = AddPriceModify(stf(curref.RndPriceModify), sti(curref.TradeType), n, i, bigSize);// boal 25.01.2004
		curref.Quantity = n;
	}
}

float AddPriceModify(float curModify, int tradeType, int currentQuantity, int goodsId, bool bigSize)// boal 25.01.2004
{
	float fMaxModify = 0.39;
	float fMinModify = 0.0;
	// boal 25.01.2004 -->
	float rateInc = 0.0;

	if( goodsId == GOOD_GOLD || goodsId == GOOD_SILVER  || goodsId == GOOD_WEAPON || goodsId == GOOD_MEDICAMENT)
	{
	    currentQuantity = currentQuantity * 6;
	}
	if (!bigSize)
	{
	    currentQuantity = currentQuantity * 2; // приведение к мелкому магазу
	}
	switch(tradeType)
	{
        case TRADE_TYPE_NORMAL:
             rateInc = currentQuantity / 2000;
             break;
        case TRADE_TYPE_AMMUNITION:
             rateInc = currentQuantity / 2000;
             break;

		case TRADE_TYPE_EXPORT:
             fMaxModify = 0.5;
		     rateInc = currentQuantity / 4000;
             break;

		case TRADE_TYPE_IMPORT:
		     fMaxModify = 0.7;
		     fMinModify = -0.35;
		     rateInc = currentQuantity / 600;
             break;

		case TRADE_TYPE_CONTRABAND:
		     fMaxModify = 0.99;
		     fMinModify = -1.9;
		     rateInc = currentQuantity / 400; // при затаривании цена падает ниже экспорта :)
             break;
             
		case TRADE_TYPE_CANNONS:
			 fMaxModify = 0.45;
		     rateInc = currentQuantity / 80; 
             break;           
	}
	if (rateInc <= 1.0)
	{
	    curModify += 0.01*frnd();
	}
	else
	{
	    curModify -= 0.01*frnd();
	}

	if(curModify < fMinModify)
	    curModify = fMinModify;

	if(curModify > fMaxModify)
        curModify = fMaxModify;
    // boal 25.01.2004 <--

	return curModify;
}

int GetProductGoods(int tradeType,bool bLarge, int _i)
{
	// boal 27.01.2004 -->
    int rate = 1;
    if ((_i==GOOD_GOLD) || (_i==GOOD_SILVER) || (_i==GOOD_WEAPON) || (_i==GOOD_MEDICAMENT))//|| i==GOOD_FOOD
	{
	   rate = 6;
	}
	// boal 27.01.2004 <--
    if (!bLarge)
	{
	    rate = rate * 2;
	}
    switch(tradeType)
	{
		// boal 25.01.2004 -->
        case TRADE_TYPE_NORMAL:		return makeint((20 + rand(10)*10) / rate);	break;
        case TRADE_TYPE_AMMUNITION:	return makeint((20 + rand(10)*10) / rate);	break;
		case TRADE_TYPE_EXPORT:		return makeint( (20 + rand(10)*10) / rate);	break;
		case TRADE_TYPE_IMPORT:		return makeint( (10+ rand(6)*10) / rate);	break;
		case TRADE_TYPE_CONTRABAND: return rand(20);	break;
		case TRADE_TYPE_CANNONS:    return rand(15);	break;
		// boal 25.01.2004 <--
	}

	return 0;
}

int GetConsumeGoods(int tradeType,bool bLarge, int _i)
{
	// boal 27.01.2004 -->
    int rate = 1;
    if ((_i==GOOD_GOLD) || (_i==GOOD_SILVER) || (_i==GOOD_WEAPON) || (_i==GOOD_MEDICAMENT))//|| i==GOOD_FOOD
	{
	   rate = 6;
	}
	// boal 27.01.2004 <--
	if (!bLarge)
	{
	    rate = rate * 2;
	}
	switch(tradeType)
	{
		// boal 25.01.2004 -->
        case TRADE_TYPE_NORMAL:		return makeint((20 + rand(10)*10) / rate);	break;
        case TRADE_TYPE_AMMUNITION:		return makeint((20 + rand(10)*10) / rate);	break;
		case TRADE_TYPE_EXPORT:		return makeint((10 + rand(6)*10) / rate);	break;
		case TRADE_TYPE_IMPORT:		return makeint((20+rand(10)*10) / rate);	break;
		case TRADE_TYPE_CONTRABAND: return 50+rand(100);	break;
		case TRADE_TYPE_CANNONS: return rand(14);	break;
		// boal 25.01.2004 <--
	}

	return 0;
}

void FillShipStore( ref chr)
{
	ref pRef = &stores[SHIP_STORE];
	// boal все лишнее убрал, просто сбросить RndPriceModify   в 1 и все, тк иначе затаривание будет
	int iQuantity = 0;
	string goodName;
	
	for(int i = 0; i<GOODS_QUANTITY; i++)
	{
		iQuantity = GetCargoGoods(chr, i); 
		SetStoreGoods(pref, i, iQuantity);

		goodName = Goods[i].name;
		pRef.Goods.(goodName).RndPriceModify = 1.0;
	}
}

int FindStore(string sColony)
{
	for(int i = 0; i < STORE_QUANTITY; i++)
	{
		if (Stores[i].colony == sColony)
		{
			return i;
		}
	}
	return -1;
}

void CheckForGoodsSpoil(aref chr)
{
	int iGoodsQ = 0;
	for(int i = 0; i < GOODS_QUANTITY; i++)
	{
		iGoodsQ = GetCargoGoods(chr, i);
		if(iGoodsQ > 0)
		{
			if(CheckAttribute(&Goods[i], "AfraidWater"))
			{
				if(sti(Goods[i].AfraidWater) > 0)
				{
					iGoodsQ = iGoodsQ - sti(Goods[i].AfraidWater);
					if(iGoodsQ < 0)
					{
						iGoodsQ = 0;
					}
					SetCharacterGoods(chr, i, iGoodsQ);
				}
			}
		}
	}
}

int GetStoreFreeSpace(object refStore)
{
	int iGoodsQ = 0;
	int iWeight = 0;
	for(int i = 0; i < GOODS_QUANTITY; i++)
	{
		iGoodsQ = GetStoreGoodsQuantity(refStore, i);
		iWeight = iWeight + iGoodsQ * stf(Goods[i].weight);
	}
	return iWeight;
}

// boal -->
string GetGoodsNameAlt(int idx)
{
    string ret;
    int iSeaGoods = LanguageOpenFile("ShipEatGood.txt");
    ret = LanguageConvertString(iSeaGoods, "seg_" + Goods[idx].Name);
    LanguageCloseFile(iSeaGoods);

    return ret;
}
// запоминаем цены в √√
void SetPriceListByStoreMan(ref rchar)   //rchar - это колони€
{
    ref refStore, nulChr;
    string attr1, sGoods;
    int i, tradeType;
    string tmpstr;
    aref   refGoods;

    nulChr = &NullCharacter;

    if (sti(rchar.StoreNum) >= 0)
    {
        refStore = &stores[sti(rchar.StoreNum)];

        attr1 = rchar.id; // ветка, где храним цены
        for (i = 0; i < GOODS_QUANTITY; i++)
        {
            sGoods = "Gidx" + i;

        	tmpstr = Goods[i].name;
        	tradeType = 0;
        	if (CheckAttribute(refStore,"Goods."+tmpstr))
        	{
        	   makearef(refGoods, refStore.Goods.(tmpstr));
         	   tradeType = MakeInt(refGoods.TradeType);
         	}
            nulChr.PriceList.(attr1).(sGoods).tradeType = tradeType; // тип торговли
            //attr2  = "Buy" + i;
            nulChr.PriceList.(attr1).(sGoods).Buy  = GetStoreGoodsPrice(refStore, i, PRICE_TYPE_BUY, pchar, 1);
            if (tradeType == TRADE_TYPE_CONTRABAND && !bBettaTestMode)
            {
                nulChr.PriceList.(attr1).(sGoods).Buy  = "???";
            }
            //attr2  = "Sell" + i;
            nulChr.PriceList.(attr1).(sGoods).Sell = GetStoreGoodsPrice(refStore, i, PRICE_TYPE_SELL, pchar, 1);
            if (tradeType == TRADE_TYPE_CONTRABAND && !bBettaTestMode)
            {
                nulChr.PriceList.(attr1).(sGoods).Sell  = "???";
            }
            //attr2  = "Qty" + i;
            nulChr.PriceList.(attr1).(sGoods).Qty  = GetStoreGoodsQuantity(refStore, i);
            if (tradeType == TRADE_TYPE_CONTRABAND && !bBettaTestMode)
            {
                nulChr.PriceList.(attr1).(sGoods).Qty  = "???";
            }
        }
        nulChr.PriceList.(attr1).AltDate = GetQuestBookDataDigit();
    }
}
// нулим магазин при захвате города эскадрой
void SetNull2StoreMan(ref rchar)
{
    ref refStore;
    int i;
    string tmpstr

    if (sti(rchar.StoreNum) >= 0)
    {
        refStore = &stores[sti(rchar.StoreNum)];

        for (i = 0; i < GOODS_QUANTITY; i++)
        {
            tmpstr = Goods[i].name;
        	if( CheckAttribute(refStore,"Goods."+tmpstr) )
        	{
        		refStore.Goods.(tmpstr).Quantity = 0;
            }
        }
    }
}
// делим колво товара - остаток дл€ грабежа rchar - это колони€
void SetNull2StoreManPart(ref rchar, float part)
{
    ref refStore;
    int i;
    string tmpstr

    if (sti(rchar.StoreNum) >= 0)
    {
        refStore = &stores[sti(rchar.StoreNum)];

        for (i = 0; i < GOODS_QUANTITY; i++)
        {
            tmpstr = Goods[i].name;
        	if (CheckAttribute(refStore,"Goods."+tmpstr) )
        	{
        		refStore.Goods.(tmpstr).Quantity = makeint(sti(refStore.Goods.(tmpstr).Quantity) / part);
            }
        }
    }
}

// нулим ростовщиков
void SetNull2Deposit(string _city)
{
    if (CheckAttribute(Pchar, "quest.Deposits." + _city))
    {
        Log_Info("¬се вклады у ростовщика в городе " + GetCityName(_city) + " пропали.");
        DeleteAttribute(Pchar, "quest.Deposits." + _city);
    }
}