extern void StoreInit();

void InitStores()
{
    int i;

    for(i=0; i<STORE_QUANTITY; i++)
    {
		stores[i].index = i;
        StoreVoidFill(&stores[i]);
    }

	if(LoadSegment("store\store_init.c"))
	{
		StoreInit();
		UnloadSegment("store\store_init.c");
	}

   for(i=0; i<STORE_QUANTITY; i++)
   {
       FillStoreGoods(&stores[i]);
   }
}

void SetStoresTradeType(int StoreNum,int GoodsNum,int TradeType)
{
	string tmpstr = Goods[GoodsNum].Name;
	Stores[StoreNum].Goods.(tmpstr).TradeType = TradeType;
}

void FillStoreGoods(ref pRef)
{
	int i,j,nq,n,tt;
	string goodName;
	//int goodBasePrice;

	int iColony = FindColony(pRef.Colony); // город магазина
	aref arTypes, arCurType;
	if (iColony != -1)
	{
		ref rColony = GetColonyByIndex(iColony);
		int islandIdx = FindIsland(rColony.island); // остров города
		
		if (islandIdx!=-1)
		{
            ref rIsland = GetIslandByIndex(islandIdx);
			makearef(arTypes, rIsland.Trade); // вся инфа как и в ПКМ по островам
			nq = GetAttributesNum(arTypes);
			for(i=0;i<nq;i++)
			{
				arCurType = GetAttributeN(arTypes,i);
				tt=TRADE_TYPE_NORMAL;
				switch(GetAttributeName(arCurType))
				{
					case "Export":		tt=TRADE_TYPE_EXPORT;		break;
					case "Import":		tt=TRADE_TYPE_IMPORT;		break;
					case "Contraband":	tt=TRADE_TYPE_CONTRABAND;	break;
				}
				n = GetAttributesNum(arCurType);
				for(j=0;j<n;j++)
				{
					goodName = Goods[sti(GetAttributeValue(GetAttributeN(arCurType,j)))].name;
					pRef.Goods.(goodName).TradeType = tt;
				}
			}
		}
		else
		{		
			trace("Mistake island id into store:  id="+rColony.island);
		}
	
  		for (i=0; i<GOODS_QUANTITY; i++)
		{
		   	goodName = Goods[i].Name;
		   	// boal 22.01.2004 -->
	   		//goodBasePrice = MakeInt(goods[i].Cost);  // рудимент уже
			switch(sti(pRef.Goods.(goodName).TradeType))
			{
				case TRADE_TYPE_NORMAL:
				    pRef.Goods.(goodName).Quantity = 200 + Rand(2500) + rand(500); // boal
					pRef.Goods.(goodName).RndPriceModify = frnd() * 0.2;//0.4
					break;

				case TRADE_TYPE_EXPORT:
				    pRef.Goods.(goodName).Quantity = 500 + Rand(4000)+ rand(6000); // boal
					pRef.Goods.(goodName).RndPriceModify = frnd() * 0.2; //0.4
					break;

				case TRADE_TYPE_IMPORT:
				    pRef.Goods.(goodName).Quantity = 60 + Rand(500) + rand(500); // boal
					pRef.Goods.(goodName).RndPriceModify = frnd() * 0.35; // 0.7 boal
					break;

				case TRADE_TYPE_CONTRABAND:
				    pRef.Goods.(goodName).Quantity = 1+Rand(50)*sti(goods[i].Units);// boal 20.01.2004
					pRef.Goods.(goodName).RndPriceModify = frnd() * 0.5; //1.0
					break;
				case TRADE_TYPE_AMMUNITION:  //делаю все тоже, что и для нормального товара, а тип нужен, чтоб на корабле не скупали лишнее.
				    pRef.Goods.(goodName).Quantity = 200 + Rand(2500) + rand(500); // boal
					pRef.Goods.(goodName).RndPriceModify = frnd() * 0.2;//0.4
					break;
					
				case TRADE_TYPE_CANNONS:  
				    pRef.Goods.(goodName).Quantity = 20 + Rand(50) + rand(50); // boal
					pRef.Goods.(goodName).RndPriceModify = frnd() * 0.4
					break;	
			}

			if (i==GOOD_GOLD || i==GOOD_SILVER  || i==GOOD_WEAPON || i==GOOD_MEDICAMENT) //|| i==GOOD_FOOD
			{
				pRef.Goods.(goodName).Quantity = makeint(sti(pRef.Goods.(goodName).Quantity) / 6); // не нужно сверх прибылей
			}
			// boal 22.01.2004 <--
			// размер имеет значение
			if (pRef.StoreSize == "small")
			{
			    pRef.Goods.(goodName).Quantity = makeint(sti(pRef.Goods.(goodName).Quantity) / 2);
			}
		}
	}
	else
	{
		trace("Mistake Colony id into store:  id=" + pRef.Colony);
	}
}

void StoreVoidFill(ref pRef)
{
	string goodName;
	//pRef.StoreSize = "large"; // "small"
	pRef.Island = "";
	for(int i=0; i<GOODS_QUANTITY; i++)
	{
		goodName = Goods[i].Name;
		// boal fix -->
		if (CheckAttribute(&Goods[i], "type"))
		{
		    pRef.Goods.(goodName).TradeType = Goods[i].type;
		}
		else
		{ // boal fix <--
			pRef.Goods.(goodName).TradeType = TRADE_TYPE_NORMAL;
		}
		pRef.Goods.(goodName).NotUsed = false;
		pRef.Goods.(goodName).Quantity = 0;
	}
}