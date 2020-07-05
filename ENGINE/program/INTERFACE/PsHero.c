string totalInfo = "";

void InitInterface(string iniName)
{
 	StartAboveForm(true);
	
    SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

    SetFormatedText("MAP_CAPTION", XI_ConvertString("titlePsHero"));

	SetEventHandler("InterfaceBreak","ProcessBreakExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	//SetEventHandler("ievnt_command","ProcCommand",0);
	SetEventHandler("evntDoPostExit","DoPostExit",0); // выход из интерфейса
	FillTable();
}

void ProcessBreakExit()
{
	IDoExit( RC_INTERFACE_ANY_EXIT );
}

void ProcessCancelExit()
{
	IDoExit( RC_INTERFACE_ANY_EXIT );
}

void IDoExit(int exitCode)
{
    EndAboveForm(true);
	DelEventHandler("InterfaceBreak","ProcessBreakExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
	//DelEventHandler("ievnt_command","ProcCommand");
	DelEventHandler("evntDoPostExit","DoPostExit");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();
	
	switch(nodName)
	{

	}
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}


void FillTable()
{
	ref chr;
	int i, n;
	string row;
	
	
	n = 1;
	// шапка -->
    GameInterface.TABLE_HERO.select = 0;
    GameInterface.TABLE_HERO.hr.td1.str = "chr.id";
    GameInterface.TABLE_HERO.hr.td1.scale = 0.8;
    GameInterface.TABLE_HERO.hr.td2.str = "ФИО";
    GameInterface.TABLE_HERO.hr.td2.scale = 0.85;
	GameInterface.TABLE_HERO.hr.td3.str = "chr.rank";
	GameInterface.TABLE_HERO.hr.td3.scale = 0.8;
	GameInterface.TABLE_HERO).hr.td4.str = "nation";
	GameInterface.TABLE_HERO.hr.td4.scale = 0.8;
	GameInterface.TABLE_HERO.hr.td5.str = "корабль";
	GameInterface.TABLE_HERO.hr.td5.scale = 0.8;
	GameInterface.TABLE_HERO.hr.td6.str = "местоположение";
	GameInterface.TABLE_HERO).hr.td6.scale = 0.8;
	GameInterface.TABLE_HERO.hr.td7.str = "направление";
	GameInterface.TABLE_HERO.hr.td7.scale = 0.8;

	for (i = 1; i <= PsHeroQty; i++)
	{
		chr = CharacterFromID("PsHero_" + i);

		//временно либо офицер, либо компаньон... не работаем с ним.
		//if (!sti(chr.PGGAi.IsPGG)) continue;
		//помер, нефиг мертвых качать.
		//if (LAi_IsDead(chr)) continue;
		
		row = "tr" + n;
		GameInterface.TABLE_HERO.(row).td1.icon.texture = "INTERFACES\PORTRAITS\128\face_" + chr.faceId + ".tga";
		GameInterface.TABLE_HERO.(row).td1.icon.uv = "0,0,1,1";
	    GameInterface.TABLE_HERO.(row).td1.str = chr.id;
	    GameInterface.TABLE_HERO.(row).td1.scale = 0.8;
	    GameInterface.TABLE_HERO.(row).td2.str = GetFullName(chr);
	    GameInterface.TABLE_HERO.(row).td2.scale = 0.8;
	    GameInterface.TABLE_HERO.(row).td3.str = chr.rank;
	    GameInterface.TABLE_HERO.(row).td3.scale = 0.8;
	    GameInterface.TABLE_HERO.(row).td4.str = Nations[sti(chr.nation)].name;
	    GameInterface.TABLE_HERO.(row).td4.scale = 0.8;
	    if (GetCharacterShipType(chr) != SHIP_NOTUSED)
	    {
	    	GameInterface.TABLE_HERO.(row).td5.str = RealShips[GetCharacterShipType(chr)].BaseName;
	    	GameInterface.TABLE_HERO.(row).td5.scale = 0.8;
	    }
	    GameInterface.TABLE_HERO.(row).td6.str = chr.PGGAi.location + " "+ chr.PGGAi.location.town;
	    GameInterface.TABLE_HERO.(row).td6.scale = 0.8;
	    if (CheckAttribute(chr, "PGGAi.task.target"))
	    {
	    	GameInterface.TABLE_HERO.(row).td7.str = chr.PGGAi.task.target;
	    	GameInterface.TABLE_HERO.(row).td7.scale = 0.8;
	    }
	    n++;
	}
	Table_UpdateWindow("TABLE_HERO");
}