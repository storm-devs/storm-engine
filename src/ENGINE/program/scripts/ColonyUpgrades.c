void BuildFortUpgrade(int iColony)
{
	string sColony = colonies[iColony].id;

	int iFortProgress = sti(colonies[iColony].fort);

	string sFortCommander = sColony + " Fort Commander";

	switch(iFortProgress)
	{
		case 1:
			
		break;
	}
}

bool CheckForUpdate(int iUpgradeType, int iColony)
{
	aref aUpgrades, rUpgrade;

	string sUpgrade = colonyupgrades[iUpgradeType].id;

	int iCurrentLevel = sti(colonies[iColony].(sUpgrade)));

	int iNextLevel;

	if(iCurrentLevel < sti(ColonyUpgrades[iUpgradeType].maxlevel))
	{
		iNextLevel = iCurrentLevel + 1;
	}
	else
	{
		return false;//построен макисмум
	}

	if(iNextLevel == 0)
	{
		return false;//невозможно построить вообще (начальное значение -1)
	}

	string sStep = "l"+iNextLevel;
	string sResult;

	makearef(aUpgrades, ColonyUpgrades[iUpgradeType].requred.(sStep));
	int iStep = GetAttributesNum(aUpgrades);

	for(int i=0; i <iStep; i++)
	{
		rUpgrade = GetAttributeN(aUpgrades, i);

		sResult = GetAttributeName(rUpgrade);

		if(sti(colonies[iColony].(sResult)) < sti(ColonyUpgrades[iUpgradeType].requred.(sStep).(sResult)))
		{
			return false;
		}
		if(sti(colonies[iColony].money) < sti(ColonyUpgrades[iUpgradeType].(sStep).money))
		{
			return false;
		}
	}

	if(sti(colonies[iColony].colonylevel) < sti(ColonyUpgrades[iUpgradeType].colonylevel))
	{
		return false;
	}

	return true;
}

int BuildUpgrade(int iColony, int iUpgrade, bool isRealBuild)
{
	if (sti(colonies[iColony].nation) == PIRATE || CheckAttribute(colonies[iColony], "notBuild")) return 1; // пираты не строятся

	string sLocation = colonies[iColony].id + "_town";
	int iLocation = FindLocation(sLocation);

	string sBuildModel = ColonyUpgrades[iUpgrade].id;

	if(isRealBuild == true)
	{
		colonies[iColony].(sBuildModel) = sti(colonies[iColony].(sBuildModel)) + 1;
	}

	if(sti(ColonyUpgrades[iUpgrade].model) == 1)
	{
		bool isBuild = false;
		int iBuildStep = 1;

		if(sti(colonies[iColony].(sBuildModel)) > 0)
		{
			isBuild = true;
			iBuildStep = sti(colonies[iColony].(sBuildModel));
		}

		if(isBuild == true)
		{
			Locations[iLocation].models.always.(sBuildModel) = "..\..\upgrades/" + sBuildModel + iBuildStep;
		}
		else
		{
			Locations[iLocation].models.always.(sBuildModel) = "..\..\upgrades/" + sBuildModel + "_destroyed";
		}
		Locations[iLocation].models.always.(sBuildModel).locator.group = "Upgrades";
		Locations[iLocation].models.always.(sBuildModel).locator.name = sBuildModel;
		
		Locations[iLocation].models.always.(sBuildModel).tech = "dLightModel";
		
		if(iUpgrade == UPGRADE_SHIPYARD)
		{
			string sShipyardLocation = colonies[iColony].id + "_shipyard";
			int iShipyardLocation = FindLocation(sShipyardLocation);
			if (iShipyardLocation > 0)
			{
				string sNewString = "shipyard0" + colonies[iColony].(sBuildModel);
				Locations[iShipyardLocation].filespath.models = "locations/inside/" + sNewString;

				//locations[iShipyardLocation].image = "loading/" + sNewString + ".tga";

				locations[iShipyardLocation].models.always.locators = sNewString + "_locators";
				locations[iShipyardLocation].models.always.shipyard = sNewString;
				locations[iShipyardLocation].models.day.charactersPatch = sNewString + "_patch";

				locations[iShipyardLocation].models.night.charactersPatch = sNewString + "_patch";

				locations[iShipyardLocation].models.always.window = sNewString + "_window";
				locations[iShipyardLocation].models.always.window.tech = "LocationWindows";
				locations[iShipyardLocation].models.always.window.level = 50;

				locations[iShipyardLocation].models.always.back = "..\inside_back";
			}
		}

		string sTownLocation = colonies[iColony].id + "_town";
		int iTownLocation = FindLocation(sTownLocation);

		if(1)//iUpgrade == UPGRADE_CHURCH && isBuild == true)
		{
			locations[iTownLocation].reload.l7.disable = false;
			// --> второй вход в церковь, если есть.
            if (CheckAttribute(locations[iTownLocation], "reload.l71.disable")) locations[iTownLocation].reload.l71.disable = false;
		}
		else
		{
			locations[iTownLocation].reload.l7.disable = true;
			if (CheckAttribute(locations[iTownLocation], "reload.l71.disable")) locations[iTownLocation].reload.l71.disable = true;
		}
		
		if(1)//iUpgrade == UPGRADE_SHIPYARD && isBuild == true)
		{
			locations[iTownLocation].reload.l5.disable = false;
		}
		else
		{
			locations[iTownLocation].reload.l5.disable = true;
		}

		if(iUpgrade == UPGRADE_MILL && isBuild == true)
		{
			string sMovieModel = sBuildModel + "_movie";
			
			Locations[iLocation].models.always.(sMovieModel) = "..\..\upgrades/" + sMovieModel;
			Locations[iLocation].models.always.(sMovieModel).locator.group = "Upgrades";
			Locations[iLocation].models.always.(sMovieModel).locator.name = sMovieModel;

			Locations[iLocation].models.always.(sMovieModel).rotate.x = 0.0;
			Locations[iLocation].models.always.(sMovieModel).rotate.y = 0.0;
			Locations[iLocation].models.always.(sMovieModel).rotate.z = 1.0;
			Locations[iLocation].models.always.(sMovieModel).tech = "dLightModel";
		}
	}

	string sUpgrade = sBuildModel;

	//colonies[iColony].(sUpgrade) = sti(colonies[iColony].(sUpgrade)) + 1;
	int iLevel = colonies[iColony].(sUpgrade);
	string sLevel = "l"+iLevel;
	if(isRealBuild)
	{
		colonies[iColony].money = sti(colonies[iColony].money) - sti(ColonyUpgrades[iUpgrade].(sLevel).money));
	}

	CheckForFirstLevelUp(iColony);
	CheckForSecondLevelUp(iColony);
	CheckForThirdLevelUp(iColony);
	return iLevel;
}

bool CheckForStoreHouseUpdate(int iColony)
{
	if(sti(colonies[iColony].storehouse) != 0)
	{
		return false;
	}

	if(sti(colonies[iColony].colonylevel) < sti(ColonyUpgrades[UPGRADE_STOREHOUSE].colonylevel))
	{
		return false;
	}

	if(sti(colonies[iColony].money) > sti(ColonyUpgrades[UPGRADE_STOREHOUSE].cost))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CheckForAcademyUpdate(int iColony)
{
	if(sti(colonies[iColony].academy) != 0)
	{
		return false;
	}

	if(sti(colonies[iColony].colonylevel) < sti(ColonyUpgrades[UPGRADE_ACADEMY].colonylevel))
	{
		return false;
	}

	if(sti(colonies[iColony].money) > sti(ColonyUpgrades[UPGRADE_ACADEMY].cost))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CheckForHutsUpdate(int iColony)
{
	if(sti(colonies[iColony].huts) != 0)
	{
		return false;
	}

	if(sti(colonies[iColony].colonylevel) < sti(ColonyUpgrades[UPGRADE_HUTS].colonylevel))
	{
		return false;
	}

	if(sti(colonies[iColony].money) > sti(ColonyUpgrades[UPGRADE_HUTS].cost))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CheckForWheatUpdate(int iColony)
{
	if(sti(colonies[iColony].wheatfield) != 0)
	{
		return false;
	}

	if(sti(colonies[iColony].money) > sti(ColonyUpgrades[UPGRADE_WHEAT].cost))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CheckForPierUpdate(int iColony)
{
	if(sti(colonies[iColony].fishingpier) != 0)
	{
		return false;
	}

	if(sti(colonies[iColony].money) > sti(ColonyUpgrades[UPGRADE_PIER].cost))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CheckForMillUpdate(int iColony)
{
	if(sti(colonies[iColony].mill) != 0)
	{
		return false;
	}

	if(sti(colonies[iColony].money) > sti(ColonyUpgrades[UPGRADE_MILL].cost))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CheckForFirstLevelUp(int iColony)
{
	if(sti(colonies[iColony].fort) < 1)
	{
		return;
	}
	if(sti(colonies[iColony].shipyard) < 1)
	{
		return;
	}
	if(sti(colonies[iColony].wheatfield) < 1)
	{
		return;
	}
	if(sti(colonies[iColony].fishingpier) < 1)
	{
		return;
	}
	if(sti(colonies[iColony].mill) < 1)
	{
		return;
	}

	if(sti(colonies[iColony].colonylevel) < 2)
	{
		colonies[iColony].colonylevel = 2;
	}
}

void CheckForSecondLevelUp(int iColony)
{
	if(sti(colonies[iColony].huts) < 1)
	{
		return;
	}
	if(sti(colonies[iColony].storehouse) < 1)
	{
		return;
	}
	if(sti(colonies[iColony].fort) < 2)
	{
		return;
	}
	if(sti(colonies[iColony].shipyard) < 2)
	{
		return;
	}

	if(sti(colonies[iColony].colonylevel) < 3)
	{
		colonies[iColony].colonylevel = 3;
	}
}

void CheckForThirdLevelUp(int iColony)
{
	if(sti(colonies[iColony].church) < 1)
	{
		return;
	}
	if(sti(colonies[iColony].fort) < 3)
	{
		return;
	}
	if(sti(colonies[iColony].shipyard) < 3)
	{
		return;
	}

	if(sti(colonies[iColony].colonylevel) < 4)
	{
		colonies[iColony].colonylevel = 4;
	}
}

int FindUpgrade(string sName)
{
	for(int i = 0; i < MAX_UPGRADES; i++)
	{
		if(ColonyUpgrades[i].id == sName)
		{
			return i;
		}
	}
	return -1;
}

string GetUpgradeNameFromColony(string sName, int iColony)
{
	int iUpgrade = FindUpgrade(sName);

	int iLevel = sti(colonies[iColony].(sName));

	if(iLevel < 1 )
	{
		return "error! upgrade not builded";
	}

	string sResult = GetUpgradeName(iUpgrade, iLevel);

	return sResult;
}


string GetNextUpgradeNameFromColony(string sName, int iColony)
{
	int iUpgrade = FindUpgrade(sName);

	int iLevel = sti(colonies[iColony].(sName));


	iLevel = iLevel + 1;

	if(iLevel > sti(ColonyUpgrades[iUpgrade].maxlevel))
	{
		iLevel = sti(ColonyUpgrades[iUpgrade].maxlevel);
	}

	if(iLevel < 1 )
	{
		iLevel = 1;
	}

	string sResult = GetUpgradeName(iUpgrade, iLevel);

	return sResult;
}

string GetUpgradeName(int iUpgrade, int iLevel)
{
	string sLevelText = "l"+iLevel;

	string sResult;

	if(CheckAttribute(&ColonyUpgrades[iUpgrade],sLeveltext + ".name"))
	{
		sResult = ColonyUpgrades[iUpgrade].(sLeveltext).name;
	}
	else
	{
		sResult = "-1";
	}

	return sResult;
}

string GetUpgradeText(int iUpgrade, int iColony)
{
	string sName = ColonyUpgrades[iUpgrade].id;

	int iLevel = sti(colonies[iColony].(sName));
	
	string sResult = GetUpgradeRealText(iUpgrade, iLevel);

	return sResult;
}

string GetUpgradeRealText(int iUpgrade, int iLevel)
{
	if(iLevel < 1)
	{
		iLevel = 1;
	}

	string sLevelText = "l"+iLevel;

	string sResult;

	if(CheckAttribute(&ColonyUpgrades[iUpgrade],sLeveltext + ".text"))
	{
		//sResult = ColonyUpgrades[iUpgrade].(sLeveltext).text;
		sResult = "Upgrade" + ColonyUpgrades[iUpgrade].id + iLevel;
		sResult = XI_ConvertString(sResult);
	}
	else
	{
		sResult = "-1";
	}

	return sResult;
}

string GetUpgradeNextText(int iUpgrade, int iColony)
{
	string sName = ColonyUpgrades[iUpgrade].id;

	int iLevel = sti(colonies[iColony].(sName));
	iLevel = iLevel + 1;

	string sResult = GetUpgradeRealText(iUpgrade, iLevel);

	if(sResult == "-1")
	{
		iLevel = iLevel - 1;
		sResult = GetUpgradeRealText(iUpgrade, iLevel);
	}

	return sResult;
}

bool CheckForCompletedUpgrade(int iUpgrade, int iColony)
{
	string sUpgrade = colonyupgrades[iUpgrade].id;

	if(sti(colonies[iColony].(sUpgrade)) == sti(ColonyUpgrades[iUpgrade].maxlevel))
	{
		return true;
	}
	return false;
}