// общие методы
int nCurScrollNum;
ref xi_refCharacter;

string CurTable, CurRow;
int iSelected; // курсор в таблице

void FillCharactersScroll()
{
	int i;
	string faceName;
	string attributeName;
	string PsgAttrName;
	int _curCharIdx;
	ref _refCurChar;
	aref pRef, pRef2;

 	DeleteAttribute(&GameInterface, "CHARACTERS_SCROLL");

	nCurScrollNum = -1;
	GameInterface.CHARACTERS_SCROLL.current = 0;
	makearef(pRef,pchar.Fellows.Passengers);

	int nOffSize = 6;
	//int nListSize = GetFreePassengersQuantity(pchar);

	GameInterface.CHARACTERS_SCROLL.NotUsed = 0;

	GameInterface.CHARACTERS_SCROLL.ImagesGroup.t0 = "EMPTYFACE";

	FillFaceList("CHARACTERS_SCROLL.ImagesGroup", pchar, 0); // officers
	FillFaceList("CHARACTERS_SCROLL.ImagesGroup", pchar, 1); // companions
	FillFaceList("CHARACTERS_SCROLL.ImagesGroup", pchar, 2); // passengers

	GameInterface.CHARACTERS_SCROLL.BadTex1 = 0;
	GameInterface.CHARACTERS_SCROLL.BadPic1 = "emptyface";

	int m = 0;

	attributeName = "pic" + (m+1);
	makearef(pRef2,GameInterface.CHARACTERS_SCROLL.(attributeName));
	GameInterface.CHARACTERS_SCROLL.(attributeName).character = GetCharacterIndex(pchar.id);
	GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(pchar);
	GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+pchar.FaceID);
	pRef2.str1 = "#" + XI_ConvertString("MainCharacter");
	pRef2.str2 = "#" + pchar.name;
	pRef2.str3 = "#" + pchar.lastname;
	//pRef2.str4 = "#" + pchar.quest.OfficerPrice;
	pRef2.str5 = "#" + pchar.rank;

	m = m + 1;

	for(i=0; i<nOffSize;i++)
	{
		attributeName = "pic" + (m+1);
		makearef(pRef2,GameInterface.CHARACTERS_SCROLL.(attributeName));
		switch(i)
		{
			case 0:
				PsgAttrName = "navigator";
				pRef2.str1 = "#" + XI_ConvertString("navigator");
			break;
			case 1:
				PsgAttrName = "boatswain";
				pRef2.str1 = "#" + XI_ConvertString("boatswain");
			break;
			case 2:
				PsgAttrName = "cannoner";
				pRef2.str1 = "#" + XI_ConvertString("cannoner");
			break;
			case 3:
				PsgAttrName = "doctor";
				pRef2.str1 = "#" + XI_ConvertString("doctor");
			break;
			case 4:
				PsgAttrName = "treasurer";
				pRef2.str1 = "#" + XI_ConvertString("treasurer");
			break;
			case 5:
				PsgAttrName = "carpenter";
				pRef2.str1 = "#" + XI_ConvertString("carpenter");
			break;
		}
		_curCharIdx = sti(pRef.(PsgAttrName));
		if(_curCharIdx!=-1)
		{
			GameInterface.CHARACTERS_SCROLL.(attributeName).character = _curCharIdx;
			GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
			GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
			pRef2.str2 = "#" + Characters[_curCharIdx].name;
			pRef2.str3 = "#" + Characters[_curCharIdx].lastname;
			//pRef2.str4 = "#" + Characters[_curCharIdx].quest.OfficerPrice;
			pRef2.str5 = "#" + Characters[_curCharIdx].rank;
		}
		else
		{
			GameInterface.CHARACTERS_SCROLL.(attributeName).character = "0";
			GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = "face";//"FACE128_" + PsgAttrName;
			GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+PsgAttrName);
			pRef2.str2 = "#" + " ";
			pRef2.str3 = "#" + " ";
			pRef2.str4 = "#" + XI_ConvertString("not assigned");
			pRef2.str5 = "#" + " ";
		}
		m++;
	}

	for(int k=1; k<4; k++)
	{
		attributeName = "pic" + (m+1);
		makearef(pRef2,GameInterface.CHARACTERS_SCROLL.(attributeName));
		PsgAttrName = "fighter";
		pRef2.str1 = "#" + XI_ConvertString("fighter");
		_curCharIdx = GetOfficersIndex(pchar, k);
		if(_curCharIdx!=-1)
		{
			GameInterface.CHARACTERS_SCROLL.(attributeName).character = _curCharIdx;
			GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
			GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
			pRef2.str2 = "#" + Characters[_curCharIdx].name;
			pRef2.str3 = "#" + Characters[_curCharIdx].lastname;
			//pRef2.str4 = "#" + Characters[_curCharIdx].quest.OfficerPrice;
			pRef2.str5 = "#" + Characters[_curCharIdx].rank;
		}
		else
		{
			GameInterface.CHARACTERS_SCROLL.(attributeName).character = "0";
			GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = "face";//"FACE128_" + PsgAttrName;
			GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+PsgAttrName);
			pRef2.str2 = "#" + " ";
			pRef2.str3 = "#" + " ";
			pRef2.str4 = "#" + XI_ConvertString("not assigned");
			pRef2.str5 = "#" + " ";
		}
		m++;
	}

	int iCompanionQuantity = getCompanionQuantity(pchar);
	if(iCompanionQuantity > 1)
	{
		for(int n=1; n<COMPANION_MAX; n++)
		{
			attributeName = "pic" + (m+1);
			makearef(pRef2,GameInterface.CHARACTERS_SCROLL.(attributeName));
			pRef2.str1 = "#" + XI_ConvertString("companionship");
			_curCharIdx = GetCompanionIndex(pchar, n);
			if(_curCharIdx!=-1)
			{
				GameInterface.CHARACTERS_SCROLL.(attributeName).character = _curCharIdx;
				GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
				GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
				pRef2.str2 = "#" + Characters[_curCharIdx].name;
				pRef2.str3 = "#" + Characters[_curCharIdx].lastname;
				if(CheckAttribute(&Characters[_curCharIdx], "quest.OfficerPrice"))
				{
					//pRef2.str4 = "#" + sti(Characters[_curCharIdx].quest.OfficerPrice);
				}
				if(CheckAttribute(&Characters[_curCharIdx], "quest.convoyquest.money"))
				{
					//pRef2.str4 = "#" + sti(Characters[_curCharIdx].quest.convoyquest.money);
				}
				pRef2.str5 = "#" + Characters[_curCharIdx].rank;
				m++;
			}
		}
	}

	for(i=0; i<GetPassengersQuantity(pchar); i++)
	{
		attributeName = "pic" + (m+1);
		PsgAttrName = "id"+(i+1);
		_curCharIdx = GetPassenger(pchar, i);

		if (_curCharIdx!=-1 && !CheckAttribute(&characters[_curCharIdx], "isfree")) // нужно ставить isfree для офицеров, чтоб два раза не попадали.
		{
			makearef(pRef2,GameInterface.CHARACTERS_SCROLL.(attributeName));
			GameInterface.CHARACTERS_SCROLL.(attributeName).character = _curCharIdx;
			GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
			GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("CHARACTERS_SCROLL.ImagesGroup","FACE128_"+Characters[_curCharIdx].FaceID);
			pRef2.str2 = "#" + Characters[_curCharIdx].name;
			pRef2.str3 = "#" + Characters[_curCharIdx].lastname;
			pRef2.str5 = "#" + Characters[_curCharIdx].rank;
			if (CheckAttribute(&Characters[_curCharIdx], "prisoned") && sti(characters[_curCharIdx].prisoned) == true)
			{
				//pRef2.str4 = "#" + CalculateRansomPrice(&characters[_curCharIdx]);
				pRef2.str1 = "#" + XI_ConvertString("prisonership");
			}
			else
			{
				pRef2.str1 = "#" + XI_ConvertString("passengership");
				if(checkAttribute(&Characters[_curCharIdx], "quest.OfficerPrice"))
				{
					//pRef2.str4 = "#" + Characters[_curCharIdx].quest.OfficerPrice;
				}
				if(checkAttribute(&Characters[_curCharIdx], "quest.deliverymoney"))
				{
					//pRef2.str4 = "#" + Characters[_curCharIdx].quest.deliverymoney;
				}
			}
			m++;
		}
		else
		{
			GameInterface.CHARACTERS_SCROLL.(attributeName).character = "0";
			GameInterface.CHARACTERS_SCROLL.(attributeName).img1 = "emptyface";
			GameInterface.CHARACTERS_SCROLL.(attributeName).tex1 = 0;
		}
	}
	GameInterface.CHARACTERS_SCROLL.ListSize = m;
}

void NullSelectTable(string sControl)
{
	if (sControl != CurTable)
	{
	    GameInterface.(sControl).select = 0;
	    Table_UpdateWindow(sControl);
	}
}