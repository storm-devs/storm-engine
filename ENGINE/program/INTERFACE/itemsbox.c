// boal пернос формы обыска и обмена, переделка  08/03/06
#define ITEM_MODE_SWAP		0
#define ITEM_MODE_TAKE		1
#define ITEM_MODE_NONE		2
#define ITEM_MODE_INFO		3

aref g_refItems;

int nCurScroll1;
int nCurScroll2;

string oldNodeName="";
int oldItemMode = -1;

bool g_bIBoxUsed;

string oldCurScrollName = "ONE_ITEMSLIST";

int idLngFile;

void InitInterface_RS(string iniName,ref itemsRef,string faceID)
{
	int i, skel;
	
	ref mchref = GetMainCharacter();
    
    LAi_SetActorTypeNoGroup(pchar);
    
    DelBakSkillAttr(GetMainCharacter()); // boal оптимизация скилов
    ClearCharacterExpRate(GetMainCharacter());
    
	StartAboveForm(false);

    if(faceID=="")
	{
		GameInterface.title = "titleItemsBox";
		QuestCheckTakeBoxes(itemsRef); //перехват
	}
	else
	{
        // boal -->
        if (faceID=="666") 
		{
			GameInterface.title = "titleDeadItems";
			//navy -- Убрать тело ;)
			SetNodeUsing("DELBODY_BUTTON", true);
		}
		else {GameInterface.title = "titleExchangeItems";}
		// boal <--
    // boal -->
      if(!CheckAttribute(itemsRef, "HoldEquip") && CheckAttribute(itemsRef, "rank"))
      {
          if (sti(itemsRef.rank) < sti(mchref.rank)) // только, если ранг больше
          {
              RemoveCharacterEquip(itemsRef, GUN_ITEM_TYPE);
              if (!IsEquipCharacterByItem(itemsRef, "unarmed"))
              {
                  RemoveCharacterEquip(itemsRef, BLADE_ITEM_TYPE);
              }
              RemoveCharacterEquip(itemsRef, CIRASS_ITEM_TYPE); //броня 081004 boal
          }
      }
    // boal <--
	}
	g_refItems = itemsRef;

	idLngFile = LanguageOpenFile("ItemsDescribe.txt");
    // сортировка boal -->
	SortItems(mchref);
	SortItems(itemsRef);
	// сортировка boal -->
	
	FillScroll();
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);
	// navy -->
    SetNodeUsing("DELBODY_BUTTON", false);
    if (faceID=="666") 
	{
		SetNodeUsing("DELBODY_BUTTON", true);
	}
	SetNewGroupPicture("MyFACEPICT",GetFaceGroupName(GetMainCharacterIndex()),GetFacePictureName(GetMainCharacterIndex()));

	GameInterface.pictures.OtherPic.tex = "";
	GameInterface.pictures.OtherPic.pic = "";
	//SetNodeUsing( "FACEPICT", faceID!="" );
	if(faceID=="")
	{
		g_bIBoxUsed = true;
		SetNewGroupPicture("FACEPICT","BOX_IMAGE","BoxImage");
	}
	else
	{
		g_bIBoxUsed = false;
		SetNewPicture("FACEPICT", "interfaces\portraits\128\face_" + faceID + ".tga");
	}
	
    // boal 19.01.2004 -->
    CreateString(true,"MainWeight","",FONT_NORMAL,COLOR_NORMAL,215,265,SCRIPT_ALIGN_LEFT,1.0);
    CreateString(true,"BoxWeight","",FONT_NORMAL,COLOR_NORMAL,215,302,SCRIPT_ALIGN_LEFT,1.0);
    // boal 19.01.2004 <--
    
	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("frame","FrameProcess",1);
	SetEventHandler("MouseRClickUP","EndTooltip",0);
	SetEventHandler("ShowItemInfo", "ShowItemInfo", 0);

	SetEventHandler("ievnt_command","ProcInterfaceCommand",0);

	SetItemMode(ITEM_MODE_SWAP);

	IGotoNewNode("TWO_ITEMSLIST");
	IGotoNewNode("ONE_ITEMSLIST");

	SetUpDownUsed();
	SetScrollsUsing();
	SetFormatedText("TITLE_CAPTION1", XI_ConvertString(GameInterface.title));
}

void FrameProcess()
{
	IGotoNewNode(GetCurrentNode());

	if( nCurScroll1 != sti(GameInterface.one_itemslist.current) ||
		nCurScroll2 != sti(GameInterface.two_itemslist.current) )
	{
		nCurScroll1 = sti(GameInterface.one_itemslist.current);
		nCurScroll2 = sti(GameInterface.two_itemslist.current);
		if(oldItemMode!=ITEM_MODE_SWAP)	{SetItemMode(ITEM_MODE_SWAP);}
		SetUpDownUsed();
	}
}

void FillScroll()
{
    int i,j,q;
    string attributeName;
	aref curItem,arItem,rootItems;
	aref arImgGrp;
	ref mchref = GetMainCharacter();

	if(CheckAttribute(&GameInterface,"one_itemslist.current"))
		nCurScroll1 = sti(GameInterface.one_itemslist.current);
	else	nCurScroll1 = 0;
	if(CheckAttribute(&GameInterface,"two_itemslist.current"))
		nCurScroll2 = sti(GameInterface.two_itemslist.current);
	else	nCurScroll2 = 0;

	DeleteAttribute(&GameInterface,"one_itemslist");
	DeleteAttribute(&GameInterface,"two_itemslist");

	//============================================================================
	// Наши вещи
	//============================================================================
	GameInterface.one_itemslist.NotUsed = 5;
	GameInterface.one_itemslist.current = nCurScroll1;
	makearef(arImgGrp,GameInterface.one_itemslist.ImagesGroup);
	FillImagesGroupForItems(arImgGrp);

	i=0;
	// Пропишем деньги
	if( sti(mchref.Money)>0 )
	{
		GameInterface.one_itemslist.pic1.img1 = GetItemPictureName("Gold");
		GameInterface.one_itemslist.pic1.tex1 = GetItemPictureTexture("one_itemslist.ImagesGroup","Gold");
		GameInterface.one_itemslist.pic1.itemID = "Money";
		GameInterface.one_itemslist.pic1.str1 = "#"+mchref.Money;
		i++;
	}
	// Заполним вещами от нас
	makearef(rootItems,mchref.Items);
    for(j=0; j<GetAttributesNum(rootItems); j++)
    {
		attributeName = "pic" + (i+1);
		curItem = GetAttributeN(rootItems,j);

		if( Items_FindItem(GetAttributeName(curItem),&arItem)>=0 )
		{
			GameInterface.one_itemslist.(attributeName).img1 = GetItemPictureName(arItem.id);
			GameInterface.one_itemslist.(attributeName).tex1 = GetItemPictureTexture("one_itemslist.ImagesGroup",arItem.id);
			GameInterface.one_itemslist.(attributeName).itemID = arItem.id;
			if(CheckConsumedItem(arItem.id))
				GameInterface.one_itemslist.(attributeName).str1 = "#"+sti(GetAttributeValue(curItem));
		}
		else
		{
			trace("WARNING! Can`t find item ID = "+GetAttributeName(curItem));
			GameInterface.one_itemslist.(attributeName).img1 = "itm16";
			GameInterface.one_itemslist.(attributeName).tex1 = 0;
		}
		i++;
    }
	GameInterface.one_itemslist.ListSize = i;

	//============================================================================
	// Чужие вещи
	//============================================================================
	GameInterface.two_itemslist.NotUsed = 5;
	GameInterface.two_itemslist.current = nCurScroll2;
	makearef(arImgGrp,GameInterface.two_itemslist.ImagesGroup);
	FillImagesGroupForItems(arImgGrp);

	i=0;
	// Пропишем деньги
	if(!CheckAttribute(g_refItems,"Money"))	g_refItems.Money = 0;

	makearef(rootItems,g_refItems.Items);
	q = GetAttributesNum(rootItems);

	// Найдем все монеты и переведем их в деньги
	for(j=0; j<q; j++)
	{
		curItem = GetAttributeN(rootItems,j);
		if(sti(GetAttributeValue(curItem))<=0)	continue;
		if( Items_FindItem(GetAttributeName(curItem),&arItem)>=0 )
		{
			//fix eddy. золото не шло в деньги, arItem - структура предмета! -->
			if( arItem.id == "gold" )
			{
				g_refItems.Money = makeint(g_refItems.Money) + makeint(rootItems.Gold); 
				DeleteAttribute(rootItems, "Gold");
				j--; 
				q--;
			}
			//<-- fix eddy. золото не шло в деньги
		}
	}
	if( sti(g_refItems.Money)>0 )
	{
		GameInterface.two_itemslist.pic1.img1 = GetItemPictureName("Gold");
		GameInterface.two_itemslist.pic1.tex1 = GetItemPictureTexture("two_itemslist.ImagesGroup","Gold");
		GameInterface.two_itemslist.pic1.itemID = "Money";
		GameInterface.two_itemslist.pic1.str1 = "#"+g_refItems.Money;
		i++;
	}

	for(j=0; j<q; j++)
	{
		attributeName = "pic" + (i+1);
		curItem = GetAttributeN(rootItems,j);

		if(sti(GetAttributeValue(curItem))<=0)	continue;

		if( Items_FindItem(GetAttributeName(curItem),&arItem)>=0 )
		{
			GameInterface.two_itemslist.(attributeName).img1 = GetItemPictureName(arItem.id);
			GameInterface.two_itemslist.(attributeName).tex1 = GetItemPictureTexture("two_itemslist.ImagesGroup",arItem.id);
			GameInterface.two_itemslist.(attributeName).itemID = arItem.id;
			if(CheckConsumedItem(arItem.id)) {
				GameInterface.two_itemslist.(attributeName).str1 = "#"+sti(GetAttributeValue(curItem));
			}
		}
		else
		{
			trace("WARNING! Can`t find item ID = "+GetAttributeName(curItem));
			GameInterface.two_itemslist.(attributeName).img1 = "itm16";
			GameInterface.two_itemslist.(attributeName).tex1 = 0;
		}
		i++;
	}
	GameInterface.two_itemslist.ListSize = i;

	nCurScroll1 = sti(GameInterface.one_itemslist.current);
	nCurScroll2 = sti(GameInterface.two_itemslist.current);
}

void ProcessCancelExit()
{
	// boal проверка на перегруз 21.01.2004 -->
	CheckAndSetOverloadMode(GetMainCharacter());
	// boal 21.01.2004 <--
    if(oldItemMode==ITEM_MODE_INFO) SetItemMode(ITEM_MODE_SWAP);
	else	IDoExit(RC_INTERFACE_ITEMSBOX_EXIT);
}

void IDoExit(int exitCode)
{
	LanguageCloseFile(idLngFile);
	if(!g_bIBoxUsed)
	{
        if (CheckAttribute(g_refItems, "skill.Fencing")) //boal fix иначе не берут саблю при перегрузе
		{
            EquipCharacterByItem(g_refItems, FindCharacterItemByGroup(g_refItems,BLADE_ITEM_TYPE));
        }
        if (CheckAttribute(g_refItems, "skill.Fencing") &&  GetCharacterSkill(g_refItems,"Fencing") > 0.1 )
		{
			EquipCharacterByItem(g_refItems, FindCharacterItemByGroup(g_refItems,CIRASS_ITEM_TYPE)); // boal 08.10.04 броню офицерам
		}
		// boal new skill -->
		if (CheckAttribute(g_refItems, "skill.Pistol") && GetCharacterSkill(g_refItems,"Pistol") > 0.1 )
		{
			EquipCharacterByItem(g_refItems, FindCharacterItemByGroup(g_refItems,GUN_ITEM_TYPE));
		}
	}
	else
	{   // снимаем флаг 'в боксе'
		bMainCharacterInBox = false;
	}
	DelEventHandler("InterfaceBreak","ProcessCancelExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	DelEventHandler("frame","FrameProcess");
	DelEventHandler("ievnt_command","ProcInterfaceCommand");
	DelEventHandler("MouseRClickUP","EndTooltip");
	DelEventHandler("ShowItemInfo", "ShowItemInfo");
	
    EndAboveForm(true);//  шел сброс времени на х10 false);
	//AllControlsFreeze(false);
    interfaceResultCommand = exitCode;
    EndCancelInterface(true);
    LAi_SetPlayerType(pchar);
}

void IGotoNewNode(string newNode)
{
	if(oldNodeName==newNode) return;
	oldNodeName=newNode;
	if(newNode=="ONE_ITEMSLIST")	SetScroll(true);
	if(newNode=="TWO_ITEMSLIST")	SetScroll(false);

	if(oldItemMode==ITEM_MODE_SWAP)
	{
		if(newNode=="DESCRIBEWINDOW")	{SetItemMode(ITEM_MODE_INFO);}
	}
	else
	{
		if(newNode!="DESCRIBEWINDOW")
		{
			SetItemMode(ITEM_MODE_SWAP);
			SetUpDownUsed();
		}
	}
}

void SetScroll(bool bIsOne)
{
	if(bIsOne)
	{
		if(oldCurScrollName=="TWO_ITEMSLIST")
		{
			SetNodeUsing("TWO_LEFTSCROLLBUTTON",false);
			SetNodeUsing("TWO_RIGHTSCROLLBUTTON",false);
		}
		SetNodeUsing("ONE_LEFTSCROLLBUTTON",true);
		SetNodeUsing("ONE_RIGHTSCROLLBUTTON",true);
		oldCurScrollName = "ONE_ITEMSLIST";
	}
	else
	{
		if(oldCurScrollName=="ONE_ITEMSLIST")
		{
			SetNodeUsing("ONE_LEFTSCROLLBUTTON",false);
			SetNodeUsing("ONE_RIGHTSCROLLBUTTON",false);
		}
		SetNodeUsing("TWO_LEFTSCROLLBUTTON",true);
		SetNodeUsing("TWO_RIGHTSCROLLBUTTON",true);
		oldCurScrollName = "TWO_ITEMSLIST";
	}
}

void SetItemMode(int mode)
{
    switch(oldItemMode)
	{
	case ITEM_MODE_SWAP:
		SetNodeUsing("INFO_BUTTON",false);
		// boal 19.01.2004 -->
		SetNodeUsing("MAIN_IMAGES",false);
		GameInterface.strings.MainWeight = "";
		GameInterface.strings.BoxWeight = "";
		// boal 19.01.2004 <--
		SetNodeUsing("SWAP_BUTTON",false);
		SetNodeUsing("ONEUP_BUTTON",false);
		SetNodeUsing("ONEDOWN_BUTTON",false);
		SetNodeUsing("ALLUP_BUTTON",false);
		SetNodeUsing("ALLDOWN_BUTTON",false);
	break;
	case ITEM_MODE_INFO:	SetNodeUsing("DESCRIBEWINDOW",false);	break;
	}

	oldItemMode = mode;

	switch(mode)
	{
	case ITEM_MODE_SWAP:
		SetNodeUsing("INFO_BUTTON",true);
		// boal 19.01.2004 -->
		SetNodeUsing("MAIN_IMAGES",true);
		SetWeightToChar();
		// boal 19.01.2004 <--
		SetNodeUsing("SWAP_BUTTON",true);
		SetNodeUsing("ONEUP_BUTTON",true);
		SetNodeUsing("ONEDOWN_BUTTON",true);
		SetNodeUsing("ALLUP_BUTTON",true);
		SetNodeUsing("ALLDOWN_BUTTON",true);
	break;
	case ITEM_MODE_INFO:	SetNodeUsing("DESCRIBEWINDOW",true);	break;
	}
}

void ProcInterfaceCommand()
{
	string comName = GetEventData();
	string nodeName = GetEventData();

	switch(nodeName)
	{
 	case "ONEDOWN_BUTTON":
		if(comName=="upstep") {GoToTopScroll();}
		if(comName=="downstep") {GiveOneItem();}//GoToBottomScroll();}
		if(comName=="leftstep") {SelectButtonLeft("INFO_BUTTON");}
		if(comName=="rightstep") {SelectButtonRight("ONEUP_BUTTON");}
		if(comName=="activate" || comName=="click")	{GiveOneItem();}
		if(comName=="deactivate")	{SetCurrentNode(oldCurScrollName);}
		// boal 19.01.2004 -->
		SetWeightToChar();
		// boal 19.01.2004 <--
	break;

	case "ONEUP_BUTTON":
		if(comName=="upstep") {TakeOneItem();}//GoToTopScroll();}
		if(comName=="downstep") {GoToBottomScroll();}
		if(comName=="leftstep") {SelectButtonLeft("ONEDOWN_BUTTON");}
		if(comName=="rightstep") {SelectButtonRight("SWAP_BUTTON");}
		if(comName=="activate" || comName=="click")	{TakeOneItem();}
		if(comName=="deactivate")	{SetCurrentNode(oldCurScrollName);}
		// boal 19.01.2004 -->
		SetWeightToChar();
		// boal 19.01.2004 <--
	break;

	case "SWAP_BUTTON":
		if(comName=="upstep") {GoToTopScroll();}
		if(comName=="downstep") {GoToBottomScroll();}
		if(comName=="leftstep") {SelectButtonLeft("ONEUP_BUTTON");}
		if(comName=="rightstep") {SelectButtonRight("ALLUP_BUTTON");}
		if(comName=="activate" || comName=="click")	{ChangeItemsSlot();}
		if(comName=="deactivate")	{SetCurrentNode(oldCurScrollName);}
		// boal 19.01.2004 -->
		SetWeightToChar();
		// boal 19.01.2004 <--
	break;

	case "ALLUP_BUTTON":
		if(comName=="upstep") {GoToTopScroll();}
		if(comName=="downstep") {GoToBottomScroll();}
		if(comName=="leftstep") {SelectButtonLeft("SWAP_BUTTON");}
		if(comName=="rightstep") {SelectButtonRight("ALLDOWN_BUTTON");}
		if(comName=="activate" || comName=="click")	{TakeItemsCollection();}
		if(comName=="deactivate")	{SetCurrentNode(oldCurScrollName);}
		// boal 19.01.2004 -->
		SetWeightToChar();
		// boal 19.01.2004 <--
	break;

	case "ALLDOWN_BUTTON":
		if(comName=="upstep") {GoToTopScroll();}
		if(comName=="downstep") {GoToBottomScroll();}
		if(comName=="leftstep") {SelectButtonLeft("ALLUP_BUTTON");}
		if(comName=="activate" || comName=="click")	{GiveItemsCollection();}
		if(comName=="deactivate")	{SetCurrentNode(oldCurScrollName);}
		// boal 19.01.2004 -->
		SetWeightToChar();
		// boal 19.01.2004 <--
	break;

	case "ONE_ITEMSLIST":
		if(comName=="downstep")	{SelectButtonLeft("SWAP_BUTTON");}
	break;

	case "TWO_ITEMSLIST":
		if(comName=="upstep")	{SelectButtonLeft("SWAP_BUTTON");}
	break;

//navy --> Убрать тело ;)
	case "DELBODY_BUTTON":
	    if(comName=="activate" || comName=="click")
	    {
			//оставил только на DEV версии... Алекс, только на тебя надежда, что в длл-ке поправишь.
			/*if (MOD_BETTATESTMODE == "On")
			{
	            Dead_DelLoginedCharacter(g_refItems);
				CharacterExitFromLocation(&Characters[sti(g_refItems.index)]);
			}*/
			//все равно падает.. не понятно :(
			//PostEvent("LAi_event_Character_Dead", 10, "i", &Characters[sti(g_refItems.index)]);
			Dead_DelLoginedCharacter(g_refItems);
			ProcessCancelExit();
		}
	break;
//navy <--
	}
}

void ChangeItemsSlot()
{
	string itm1 = GetItemIDByOrder(true,nCurScroll1);
	string itm2 = GetItemIDByOrder(false,nCurScroll2);
	if(itm1=="" || itm1!=itm2)
	{
		if( !CanCharacterTakeItem(true,itm2) ) return;
	}

	int iq1 = GetItemQntByOrder(true,nCurScroll1);
	int iq2 = GetItemQntByOrder(false,nCurScroll2);

	aref mchref; makearef(mchref,Characters[GetMainCharacterIndex()]);

	if(iq1>0)
	{
		if(itm1=="Money")
		{
			mchref.Money = 0;
			g_refItems.Money = iq1+sti(g_refItems.Money);
		}
		else
		{
			TakeNItems(GetMainCharacter(),itm1,-iq1);
			if( CheckAttribute(g_refItems,"Items."+itm1) )
			{
				iq1 = iq1 + sti(g_refItems.Items.(itm1));
			}
			g_refItems.Items.(itm1) = iq1;
		}
	}
	if(iq2>0)
	{
		if(itm2=="Money")
		{
			mchref.Money = sti(mchref.Money)+iq2;
			g_refItems.Money = sti(g_refItems.Money)-iq2;
		}
		else
		{
			TakeNItems(GetMainCharacter(),itm2,iq2);
			g_refItems.Items.(itm2) = sti(g_refItems.Items.(itm2))-iq2;
			if( sti(g_refItems.Items.(itm2))<=0 ) DeleteAttribute(g_refItems,"Items."+itm2);
		}
	}

	FillScroll();
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"ONE_ITEMSLIST",-1);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"TWO_ITEMSLIST",-1);
	SetUpDownUsed();
	SetScrollsUsing();
}
void TakeOneItem()
{
	string itm2 = GetItemIDByOrder(false,nCurScroll2);
	int iq2 = GetItemQntByOrder(false,nCurScroll2);
	if(iq2<=0) return;

	aref mchref; makearef(mchref,Characters[GetMainCharacterIndex()]);
	iq2--;
	if(itm2=="Money")
	{
		mchref.Money = sti(mchref.Money)+1;
		g_refItems.Money = iq2;
	}
	else
	{
		TakeNItems(GetMainCharacter(),itm2,1);
		g_refItems.Items.(itm2) = iq2;
		if( iq2<=0 ) DeleteAttribute(g_refItems,"Items."+itm2);
	}

	FillScroll();
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"ONE_ITEMSLIST",-1);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"TWO_ITEMSLIST",-1);
	SetUpDownUsed();
	SetScrollsUsing();
}
void GiveOneItem()
{
	string itm1 = GetItemIDByOrder(true,nCurScroll1);
	int iq1 = GetItemQntByOrder(true,nCurScroll1);
	if(iq1<=0) return;

	aref mchref; makearef(mchref,Characters[GetMainCharacterIndex()]);
	iq1--;
	if(itm1=="Money")
	{
		mchref.Money = sti(mchref.Money)-1;
		g_refItems.Money = sti(g_refItems.Money)+1;
	}
	else
	{
		TakeNItems(GetMainCharacter(),itm1,-1);
		if( CheckAttribute(g_refItems,"Items."+itm1) )
			g_refItems.Items.(itm1) = 1 + sti(g_refItems.Items.(itm1));
		else	g_refItems.Items.(itm1) = 1;
	}

	FillScroll();
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"ONE_ITEMSLIST",-1);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"TWO_ITEMSLIST",-1);
	SetUpDownUsed();
	SetScrollsUsing();
}
void TakeItemsCollection()
{
	string itm2 = GetItemIDByOrder(false,nCurScroll2);
	int iq2 = GetItemQntByOrder(false,nCurScroll2);
	if(iq2<=0) return;

    ref  rItem1;
	float w1 = 0;
	ref   mainCh = GetMainCharacter();
	
	if (itm2=="Money")
	{
		mainCh.Money = sti(mainCh.Money)+iq2;
		g_refItems.Money = 0;
	}
	else
	{
        rItem1 = ItemsFromID(itm2);
    	w1 = stf(rItem1.Weight);
    
		if ((GetItemsWeight(mainCh) + iq2 *w1) > GetMaxItemsWeight(mainCh))
        {
            iq2 = makeint((GetMaxItemsWeight(mainCh) - GetItemsWeight(mainCh)) / w1);
        }
		TakeNItems(mainCh, itm2, iq2);
		g_refItems.Items.(itm2) = sti(g_refItems.Items.(itm2)) - iq2;
		if (sti(g_refItems.Items.(itm2))<=0 ) DeleteAttribute(g_refItems,"Items."+itm2);
	}

	FillScroll();
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"ONE_ITEMSLIST",-1);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"TWO_ITEMSLIST",-1);
	SetUpDownUsed();
	SetScrollsUsing();
}
void GiveItemsCollection()
{
	string itm1 = GetItemIDByOrder(true,nCurScroll1);
	int iq1 = GetItemQntByOrder(true,nCurScroll1);
	if(iq1<=0) return;

    ref  rItem1;
	float w1 = 0;

	aref mchref; makearef(mchref,Characters[GetMainCharacterIndex()]);
	if(itm1=="Money")
	{
		mchref.Money = 0;
		g_refItems.Money = sti(g_refItems.Money)+iq1;
	}
	else
	{
        rItem1 = ItemsFromID(itm1);
    	w1 = stf(rItem1.Weight);
    
		if ((GetItemsWeight(g_refItems) + iq1 *w1) > GetMaxItemsWeight(g_refItems))
        {
            iq1 = makeint((GetMaxItemsWeight(g_refItems) - GetItemsWeight(g_refItems)) / w1);
        }
        
		mchref.Items.(itm1) = sti(mchref.Items.(itm1)) - iq1;
		if (sti(mchref.Items.(itm1))<=0 ) DeleteAttribute(mchref, "Items."+itm1);

		if( CheckAttribute(g_refItems,"Items."+itm1) )
			g_refItems.Items.(itm1) = iq1 + sti(g_refItems.Items.(itm1));
		else	g_refItems.Items.(itm1) = iq1;
	}

	FillScroll();
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"ONE_ITEMSLIST",-1);
	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"TWO_ITEMSLIST",-1);
	SetUpDownUsed();
	SetScrollsUsing();
}


//==============================
// items utilites
//==============================
string GetItemIDByOrder(bool my,int num)
{
	string attrName = "pic"+(num+1);
	if(my)
	{
		if( CheckAttribute(&GameInterface,"one_itemslist."+attrName+".itemID") )
			return GameInterface.one_itemslist.(attrName).itemID;
	}
	else
	{
		if( CheckAttribute(&GameInterface,"two_itemslist."+attrName+".itemID") )
			return GameInterface.two_itemslist.(attrName).itemID;
	}
	return "";
}

bool GetItemRefByOrder(bool my,int num, ref refItem)
{
	aref ar;
	string itemID = GetItemIDByOrder(my,num);
	if(itemID=="Money") itemID=GOLD;
	if( Items_FindItem(itemID,&ar)<0 ) return false;
	refItem = ar;
	return true;
}

int GetItemQntByOrder(bool my,int num)
{
	string itmName = GetItemIDByOrder(my,num);
	if(my)
	{
		if(itmName=="Money") return sti(Characters[GetMainCharacterIndex()].Money);
		return GetCharacterItem(GetMainCharacter(),itmName);
	}
	if(itmName=="Money") return sti(g_refItems.Money);
	if( CheckAttribute(g_refItems,"items."+itmName) )
		return sti(g_refItems.items.(itmName));
	return 0;
}

bool CheckConsumedItem(string itemID)
{
	aref arone;
	makearef(arone,Characters[GetMainCharacterIndex()].items);
	aref artwo;
	makearef(artwo,g_refItems.items);

	int n=0;
	if( CheckAttribute(arone,itemID) ) n = sti(arone.(itemID));
	if( CheckAttribute(artwo,itemID) ) n = n + sti(artwo.(itemID));

	if(n>1) return true;
	return false;
}

bool CanCharacterTakeItem(bool myCh,string itemName)
{
	if(!myCh) return true;
	aref chref;
	makearef(chref,Characters[GetMainCharacterIndex()]);
	if( CheckAttribute(chref,"items."+itemName) ) return true;
	if( GetChrItemQuantity(GetMainCharacter())<MAX_ITEM_TAKE ) return true;
	return false;
}

void SetUpDownUsed()
{
	aref iref; makearef(iref,Characters[GetMainCharacterIndex()].items);

	string itmName1 = GetItemIDByOrder(true,nCurScroll1);
	int iq1 = GetItemQntByOrder(true,nCurScroll1);
	string itmName2 = GetItemIDByOrder(false,nCurScroll2);
	int iq2 = GetItemQntByOrder(false,nCurScroll2);
	// boal 20.01.2004 -->
	bool bYes1, bYes2;
	aref arItem1, arItem2;
	float w1 = 0;
    float w2 = 0;
    ref   mainCh = GetMainCharacter();
    
    bYes1 = GetItemRefByOrder(true,nCurScroll1,&arItem1);
    bYes2 = GetItemRefByOrder(false,nCurScroll2,&arItem2);
    if (bYes1)
    {
       w1 = stf(arItem1.Weight);
    }
    if (bYes2)
    {
       w2 = stf(arItem2.Weight);
    }
    SetSelectable("ONEDOWN_BUTTON",false);
	SetSelectable("ALLDOWN_BUTTON",false);
	
	if(iq1>0 && (GetItemsWeight(g_refItems) + w1) <= GetMaxItemsWeight(g_refItems))
	{
		SetSelectable("ONEDOWN_BUTTON",true);
		SetSelectable("ALLDOWN_BUTTON",true);
	}
	/*if(iq1>0 && (GetItemsWeight(g_refItems) + iq1*w1) <= GetMaxItemsWeight(g_refItems))
	{
		SetSelectable("ALLDOWN_BUTTON",true);
	}*/
    SetSelectable("ONEUP_BUTTON",false);
	SetSelectable("ALLUP_BUTTON",false);
	if(iq2>0 && (GetItemsWeight(mainCh) + w2) <= GetMaxItemsWeight(mainCh))
	{
		if( CheckAttribute(iref,itmName2) || GetAttributesNum(iref)<MAX_ITEM_TAKE )
		{
			SetSelectable("ONEUP_BUTTON",true);
			SetSelectable("ALLUP_BUTTON",true);
			/*if( (GetItemsWeight(mainCh) + iq2 *w2) <= GetMaxItemsWeight(mainCh))
			{
			    SetSelectable("ALLUP_BUTTON",true);
			} */
		}
	}
	
	SetSelectable("SWAP_BUTTON",false);
	
	if(itmName2=="" || itmName2=="Money" || GetAttributesNum(iref)<MAX_ITEM_TAKE)
	{
		SetSelectable("SWAP_BUTTON",true);
	}
	else
	{
		if( CheckAttribute(iref,itmName2) )	SetSelectable("SWAP_BUTTON",true);
		else
		{
			if(itmName1!="" && itmName1!="Money") SetSelectable("SWAP_BUTTON",true);
		}
	}
	// запрет обмена
	if ((GetItemsWeight(mainCh) + iq2 *w2 - iq1*w1) > GetMaxItemsWeight(mainCh))
	{
	    SetSelectable("SWAP_BUTTON",false);
	}
	if ((GetItemsWeight(g_refItems) + iq1*w1 - iq2 *w2) > GetMaxItemsWeight(g_refItems))
	{
	    SetSelectable("SWAP_BUTTON",false);
	}
    // boal 20.01.2004 <--
	if( IsQuestUsedItem(itmName1) ) //нельзя отдать свои, но квестовые можно взять!
	{
		SetSelectable("SWAP_BUTTON",false);
		SetSelectable("ONEDOWN_BUTTON",false);
		SetSelectable("ALLDOWN_BUTTON",false);
	}

	ThatOneAndEquiped(itmName1,itmName2);

	if(!GetSelectable(GetCurrentNode()))	SetCurrentNode(oldCurScrollName);
}

void SelectButtonLeft(string startBtnNode)
{
	if(GetSelectable(startBtnNode))	SetCurrentNode(startBtnNode);
	else
	{
		switch(startBtnNode)
		{
		case "ONEDOWN_BUTTON":	SelectButtonLeft("INFO_BUTTON"); break;
		case "ONEUP_BUTTON":	SelectButtonLeft("ONEDOWN_BUTTON"); break;
		case "SWAP_BUTTON":		SelectButtonLeft("ONEUP_BUTTON"); break;
		case "ALLUP_BUTTON":	SelectButtonLeft("SWAP_BUTTON"); break;
		case "ALLDOWN_BUTTON":	SelectButtonLeft("ALLUP_BUTTON"); break;
		}
	}
}

void SelectButtonRight(string startBtnNode)
{
	if(GetSelectable(startBtnNode))	SetCurrentNode(startBtnNode);
	else
	{
		switch(startBtnNode)
		{
		case "INFO_BUTTON":		SelectButtonRight("ONEDOWN_BUTTON"); break;
		case "ONEDOWN_BUTTON":	SelectButtonRight("ONEUP_BUTTON"); break;
		case "ONEUP_BUTTON":	SelectButtonRight("SWAP_BUTTON"); break;
		case "SWAP_BUTTON":		SelectButtonRight("ALLUP_BUTTON"); break;
		case "ALLUP_BUTTON":	SelectButtonRight("ALLDOWN_BUTTON"); break;
		}
	}
}

void ThatOneAndEquiped(string itmName1, string itmName2)
{
	if( !GetRemovable(g_refItems) )
	{
		SetSelectable("ONEDOWN_BUTTON",false);
		SetSelectable("ALLDOWN_BUTTON",false);
		SetSelectable("SWAP_BUTTON",false);
		SetSelectable("ONEUP_BUTTON",false);
		SetSelectable("ALLUP_BUTTON",false);
		return;
	}

	if( IsEquipCharacterByItem(GetMainCharacter(),itmName1) )
	{
		if( GetCharacterItem(GetMainCharacter(),itmName1)<=1 )
		{
			SetSelectable("ONEDOWN_BUTTON",false);
		}
		SetSelectable("ALLDOWN_BUTTON",false);
		SetSelectable("SWAP_BUTTON",false);
	}

	if( IsEquipCharacterByItem(g_refItems,itmName2) )
	{
		if( GetCharacterItem(g_refItems,itmName2)<=1 )
		{
			SetSelectable("ONEUP_BUTTON",false);
		}
		SetSelectable("ALLUP_BUTTON",false);
		SetSelectable("SWAP_BUTTON",false);
	}
}

void SetScrollsUsing()
{
	if( CheckAttribute(&GameInterface,"two_itemslist.pic1") ) {
		SendMessage(&GameInterface,"lsll", MSG_INTERFACE_MSG_TO_NODE,"TWO_ITEMSLIST",0, true);
		SetSelectable("TWO_ITEMSLIST",true);
		SetSelectable("TWO_LEFTSCROLLBUTTON",true);
		SetSelectable("TWO_RIGHTSCROLLBUTTON",true);
	} else {
		SendMessage(&GameInterface,"lsll", MSG_INTERFACE_MSG_TO_NODE,"TWO_ITEMSLIST",0, false);
		SetSelectable("TWO_ITEMSLIST",false);
		SetSelectable("TWO_LEFTSCROLLBUTTON",false);
		SetSelectable("TWO_RIGHTSCROLLBUTTON",false);
		if( GetCurrentNode()=="TWO_ITEMSLIST" ) {
			SetCurrentNode("ONE_ITEMSLIST");
		}
	}

	if( CheckAttribute(&GameInterface,"one_itemslist.pic1") ) {
		SendMessage(&GameInterface,"lsll", MSG_INTERFACE_MSG_TO_NODE,"ONE_ITEMSLIST",0, true);
		SetSelectable("ONE_ITEMSLIST",true);
		SetSelectable("ONE_LEFTSCROLLBUTTON",true);
		SetSelectable("ONE_RIGHTSCROLLBUTTON",true);
	} else {
		SendMessage(&GameInterface,"lsll", MSG_INTERFACE_MSG_TO_NODE,"ONE_ITEMSLIST",0, false);
		SetSelectable("ONE_ITEMSLIST",false);
		SetSelectable("ONE_LEFTSCROLLBUTTON",false);
		SetSelectable("ONE_RIGHTSCROLLBUTTON",false);
		if( GetCurrentNode()=="ONE_ITEMSLIST" ) {
			if( GetSelectable("TWO_ITEMSLIST") ) {SetCurrentNode("TWO_ITEMSLIST");}
		}
	}
}

void GoToTopScroll()
{
	if( GetSelectable("ONE_ITEMSLIST") ) SetCurrentNode("ONE_ITEMSLIST");
}

void GoToBottomScroll()
{
	if( GetSelectable("TWO_ITEMSLIST") ) SetCurrentNode("TWO_ITEMSLIST");
}

// boal 19.01.2004 -->
void SetWeightToChar()
{
        ref mchref = GetMainCharacter();
     
        GameInterface.strings.MainWeight = "" +GetItemsWeight(mchref) + " / "+GetMaxItemsWeight(mchref);
		if (CheckAttribute(g_refItems, "Skill.Fencing"))// критерий трупа и сундука
		{
		   GameInterface.strings.BoxWeight = "" +FloatToString(GetItemsWeight(g_refItems), 1) + " / "+GetMaxItemsWeight(g_refItems);
		}
}
// boal 19.01.2004 <--

/*void ShowItemInfo()
{
	string sHeader, sText1, sText2, sText3, sText4;
	int iColor2, iColor3, iColor4;
	sHeader = "test";
	sText1 = "test1";
	string sPicture = "none";
	string sGroup = "";
	string sGroupPicture = "";
	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, iColor2, sText3, iColor3, sText4, iColor4,sPicture, sGroup, sGroupPicture, 64, 64);
}
*/
void EndTooltip()
{
	CloseTooltip();
}

void ShowItemInfo()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sPicture;
	string sGroup, sGroupPicture;
	int iItem;

	sPicture = "-1";
	string sAttributeName;
	int nChooseNum = -1;
	nChooseNum = SendMessage( &GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sCurrentNode, 2 );
	sAttributeName = "pic" + (nChooseNum + 1);
	if (CheckAttribute(&GameInterface, sCurrentNode + "." + sAttributeName + ".itemID"))
	{
        if (GameInterface.(sCurrentNode).(sAttributeName).itemID == "Money")
        {
            iItem = FindItem("Gold");
        }
        else
        {
			iItem = FindItem(GameInterface.(sCurrentNode).(sAttributeName).itemID);
		}

		sGroup = Items[iItem].picTexture;
		sGroupPicture = "itm" + Items[iItem].picIndex;

		sHeader = Items[iItem].name;
		sText1 = GetItemDescribe(iItem);
		sHeader = LanguageConvertString(idLngFile,sHeader);
		
		/*if (CheckAttribute(&Items[iItem], "groupID"))
		{
			string sGroupID = Items[iItem].groupID;
			sGroup = Items[iItem].picTexture;
			sGroupPicture = "itm" + Items[iItem].picIndex;
			if(sGroupID == BLADE_ITEM_TYPE)
			{
				ShowBladeInfo(iItem, &sHeader, &sText1, &sText2, &sText3);
			}
			if(sGroupID == GUN_ITEM_TYPE)
			{
				ShowGunInfo(iItem, &sHeader, &sText1, &sText2, &sText3);
			}
			if(sGroupID == SPYGLASS_ITEM_TYPE)
			{
				ShowSpyglassInfo(iItem, &sHeader, &sText1, &sText2, &sText3);
			}
		}	
		// boal 19.01.2004 -->
		sText1 += NewStr()+"Вес: " + FloatToString(stf(Items[iItem].Weight), 1);
		// boal 19.01.2004 <--
		*/
	}

	CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 64, 64);

}

void ShowBladeInfo(int iItem, aref sHeader, aref sText1, aref sText2, aref sText3)
{
	sHeader = Items[iItem].name;
	sText1 = Items[iItem].describe;

    //boal 16.08.05 -->
    sText2 = "";
    ref arItem = &Items[iItem];
    
	if (CheckAttribute(arItem,"FencingType"))
	{
		arItem.FencingTypeName = XI_ConvertString(arItem.FencingType);
		sText2 += GetAssembledString( LanguageConvertString(idLngFile,"weapon blade type"),	arItem) + "\n";
	}
	else
	{
        sText2 += " ERROR \n";
	}
	// boal <--
	sText2 += XI_ConvertString("WeaponDamage") + sti(Items[iItem].dmg_min) + " - " + sti(Items[iItem].dmg_max);
	sText2 = sText2 + "\n";
	//sText2 = sText2 + XI_ConvertString("WeaponBlock") + sti(Items[iItem].block) + "%";
	//sText2 = sText2 + "\n";
	//sText2 = sText2 + XI_ConvertString("WeaponPiercing") + sti(Items[iItem].piercing) + "%";
	//sText2 = sText2 + "\n";
	//sText2 = sText2 + XI_ConvertString("WeaponRank") + sti(Items[iItem].minlevel);

	//sText3 = sText3 + XI_ConvertString("WeaponRarity") + sti(Items[iItem].rare) + "%";
	sText3 = sText3 + "\n";
	if(CheckAttribute(&Items[iItem], "uniq") && sti(Items[iItem].uniq) == 1)
	{
		sText3 = sText3 + XI_ConvertString("WeaponUnic");
	}

	sHeader = LanguageConvertString(idLngFile,sHeader);
	sText1 = LanguageConvertString(idLngFile,sText1);
}

void ShowSpyglassInfo(int iItem, aref sHeader, aref sText1, aref sText2, aref sText3)
{
	//idLngFile = LanguageOpenFile("ItemsDescribe.txt");

	sHeader = Items[iItem].name;
	sText1 = Items[iItem].describe;

	sText2 = XI_ConvertString("SpyGlassZoom") + sti(Items[iItem].scope.zoom);
	sText2 = sText2 + "\n";
	if(sti(Items[iItem].scope.show.nation) == 1)
	{
		sText2 = sText2 + XI_ConvertString("SpyGlassShowNation");
		sText2 = sText2 + "\n";
	}
	if(sti(Items[iItem].scope.show.cannons) == 1)
	{
		sText2 = sText2 + XI_ConvertString("SpyGlassShowCannons");
		sText2 = sText2 + "\n";
	}
	if(sti(Items[iItem].scope.show.crew) == 1)
	{
		sText2 = sText2 + XI_ConvertString("SpyGlassShowCrew");
		sText2 = sText2 + "\n";
	}
	if(sti(Items[iItem].scope.show.charge) == 1)
	{
		sText2 = sText2 + XI_ConvertString("SpyGlassShowCharge");
		sText2 = sText2 + "\n";
	}
	if(sti(Items[iItem].scope.show.captain_skills) == 1)
	{
		sText2 = sText2 + XI_ConvertString("SpyGlassShowSkills");
	}

	//sText3 = sText3 + XI_ConvertString("WeaponRarity") + sti(Items[iItem].rare) + "%";
	//sText3 = sText3 + "\n";
	if(CheckAttribute(&Items[iItem], "uniq") && sti(Items[iItem].uniq) == 1)
	{
		sText3 = sText3 + XI_ConvertString("WeaponUnic");
	}

	sHeader = LanguageConvertString(idLngFile,sHeader);
	sText1 = LanguageConvertString(idLngFile,sText1);
	//LanguageCloseFile(idLngFile);
}

void ShowGunInfo(int iItem, aref sHeader, aref sText1, aref sText2, aref sText3)
{
	//idLngFile = LanguageOpenFile("ItemsDescribe.txt");

	sHeader = Items[iItem].name;
	sText1 = Items[iItem].describe;

	sText2 = XI_ConvertString("WeaponDamage") + sti(Items[iItem].dmg_min) + " - " + sti(Items[iItem].dmg_max);
	sText2 = sText2 + "\n";
	sText2 = sText2 + XI_ConvertString("GunAccuracy") + sti(Items[iItem].accuracy) + "%";
	sText2 = sText2 + "\n";
	sText2 = sText2 + XI_ConvertString("GunCharges") + sti(Items[iItem].chargeQ);
	sText2 = sText2 + "\n";
	//sText2 = sText2 + XI_ConvertString("WeaponRank") + sti(Items[iItem].minlevel);

	//sText3 = XI_ConvertString("WeaponPrice") + sti(Items[iItem].price);
	//sText3 = sText3 + "\n";
	//sText3 = sText3 + XI_ConvertString("WeaponRarity") + sti(Items[iItem].rare) + "%";
	//sText3 = sText3 + "\n";
	if(CheckAttribute(&Items[iItem], "uniq") && sti(Items[iItem].uniq) == 1)
	{
		sText3 = sText3 + XI_ConvertString("WeaponUnic");
	}

	sHeader = LanguageConvertString(idLngFile,sHeader);
	sText1 = LanguageConvertString(idLngFile,sText1);
	//LanguageCloseFile(idLngFile);
}