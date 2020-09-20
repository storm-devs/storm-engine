
void GenerateRumour()
{
    string RumText;
    ref CurRum;

   ///////////////////////////////////////Золотой флот///////////////////////////////////////////////
   if ( isGoldFleet == 0 && Colonies[FindColony("Havana")].nation == SPAIN
    && Colonies[FindColony("PortoBello")].nation == SPAIN && rand(5) == 0 )
    {

        
        Log_TestInfo("loading GoldFleet "+XI_ConvertString("target_month_"+GetAddingDataMonth(0, 6, 0)));
        isGoldFleet = true;
        string sQuest = "GenerateGoldFleet";
        SetTimerCondition(sQuest, 0, 6, 0, false);
        pchar.quest.(sQuest).win_condition = "StartGoldFleet";
        pchar.quest.(sQuest).function= "StartGoldFleet";
        
        GoldMonth = XI_ConvertString("target_month_"+GetAddingDataMonth(0, 6, 0));
        AddTemplRumour("Init_GoldFleet", id_counter+1);
    }
    //////////////////////////////////////Наводка на купца/////////////////////////////////////////////
    ref MerPrm;
    makeref(MerPrm, MerchantParam);
    if (!CheckAttribute(MerPrm, "active") || MerPrm.active == false)
    AddTemplRumour("MerchantOnMap", id_counter+1);
    ////////////////////////////////////////Цены в магазине////////////////////////////////////////////
    if (rand(5)==0)AddTemplRumour("GetColonyPrices", id_counter+1);
    //////////////////////////////////////////////////////////////////////////////////////////////////
   /* aref aData;
    makearef(aData, NullCharacter.Siege);
    if (!CheckAttribute(aData, "isSiege") || aData.isSiege == false)
    {
        sQuest = "CreateSiege";
        SetTimerCondition(sQuest, 0, 2+rand(1), rand(30), false);
        pchar.quest.(sQuest).win_condition = "CreateSiege";
        pchar.quest.(sQuest).function= "CreateSiege";
        aData.isSiege = 1;
    }*/
    //////////////////////////////////////////////////////////////////////////////////////////////////
}