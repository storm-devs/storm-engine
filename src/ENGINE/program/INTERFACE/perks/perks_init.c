
void extrnInitPerks()
{
    DeleteAttribute(&ChrPerksList, "list");
    // NPCOnly - только НПС  PlayerOnly - только ГГ, Hiden - скрыт для всех, кодерские трюки Боссов-НПС
    // navigator  boatswain  cannoner doctor carpenter treasurer fighter  - офицеры  для  .OfficerType
    
    // список из ВМЛ -->
    //	Personal Skills Group
	//==========================================================
	ChrPerksList.list.BasicDefense.descr = "perkBasicDefense";
    ChrPerksList.list.BasicDefense.BaseType = "self"; // boal 05.05.04 ввел разграничения

	ChrPerksList.list.AdvancedDefense.descr = "perkAdvancedDefense";
	ChrPerksList.list.AdvancedDefense.condition.BasicDefense = true;
    ChrPerksList.list.AdvancedDefense.BaseType = "self";

    ChrPerksList.list.Ciras.descr = "perkCiras";
	ChrPerksList.list.Ciras.condition.AdvancedDefense = true;
	ChrPerksList.list.Ciras.BaseType = "self";

    ChrPerksList.list.CriticalHit.descr = "perkCriticalHit";
    ChrPerksList.list.CriticalHit.BaseType = "self";
    
	ChrPerksList.list.HPPlus.descr = "perkHPPlus";
	ChrPerksList.list.HPPlus.BaseType = "self";
	ChrPerksList.list.HPPlus.condition.AdvancedDefense = true;
	
	ChrPerksList.list.HardHitter.descr = "perkHardHitter";
	ChrPerksList.list.HardHitter.BaseType = "self";
	
	ChrPerksList.list.Tireless.descr = "perkTireless";
	ChrPerksList.list.Tireless.BaseType = "self";
	
	ChrPerksList.list.Rush.descr = "perkRush";
	ChrPerksList.list.Rush.TimeDelay    = 70; // 10+60
	ChrPerksList.list.Rush.TimeDuration = 10;
	ChrPerksList.list.Rush.BaseType = "self";
	ChrPerksList.list.Rush.condition.Tireless     = true;
	ChrPerksList.list.Rush.condition.BasicDefense = true;
	ChrPerksList.list.Rush.PlayerOnly = true;
	
	ChrPerksList.list.EnergyPlus.descr = "perkEnergyPlus";
	ChrPerksList.list.EnergyPlus.BaseType = "self";
	ChrPerksList.list.EnergyPlus.condition.Tireless = true;
	
	ChrPerksList.list.Sliding.descr = "perkSliding";
	ChrPerksList.list.Sliding.condition.AdvancedDefense = true;
	ChrPerksList.list.Sliding.BaseType = "self";
	
	ChrPerksList.list.BladeDancer.descr = "perkBladeDancer";
	ChrPerksList.list.BladeDancer.condition.CriticalHit = true;
	ChrPerksList.list.BladeDancer.condition.AdvancedDefense = true;
	ChrPerksList.list.BladeDancer.BaseType = "self";
	
	ChrPerksList.list.SwordplayProfessional.descr = "perkSwordplayProfessional";
	ChrPerksList.list.SwordplayProfessional.condition.CriticalHit = true;
	ChrPerksList.list.SwordplayProfessional.condition.AdvancedDefense = true;
	ChrPerksList.list.SwordplayProfessional.condition.Tireless = true;
	ChrPerksList.list.SwordplayProfessional.BaseType = "self";
	
	ChrPerksList.list.Gunman.descr = "perkGunman";
	ChrPerksList.list.Gunman.BaseType = "self";

	ChrPerksList.list.GunProfessional.descr = "perkGunProfessional";
	ChrPerksList.list.GunProfessional.condition.Gunman = true;
	ChrPerksList.list.GunProfessional.BaseType = "self";

	ChrPerksList.list.Grus.descr = "perkGrus";
	ChrPerksList.list.Grus.BaseType = "self";

 	ChrPerksList.list.Medic.descr = "perkMedic";
	ChrPerksList.list.Medic.condition.AdvancedDefense = true;
	ChrPerksList.list.Medic.BaseType = "self";
	ChrPerksList.list.Medic.PlayerOnly = true;
	
	ChrPerksList.list.ByWorker.descr = "perkByWorker";
	ChrPerksList.list.ByWorker.BaseType = "self";
	ChrPerksList.list.ByWorker.NPCOnly = true;
	
	ChrPerksList.list.ByWorker2.descr = "perkByWorker2";
	ChrPerksList.list.ByWorker2.BaseType = "self";
	ChrPerksList.list.ByWorker2.condition.ByWorker = true;
	ChrPerksList.list.ByWorker2.NPCOnly = true;

	ChrPerksList.list.SharedExperience.descr = "perkSharedExperience";
	ChrPerksList.list.SharedExperience.BaseType = "self";
	ChrPerksList.list.SharedExperience.PlayerOnly = true;

	ChrPerksList.list.IronWill.descr = "perkIronWill";
	ChrPerksList.list.IronWill.BaseType = "self";
	ChrPerksList.list.IronWill.OfficerType = "boatswain";

	ChrPerksList.list.ShipEscape.descr = "perkShipEscape";
	ChrPerksList.list.ShipEscape.condition.IronWill = true;
	ChrPerksList.list.ShipEscape.BaseType = "self";

	ChrPerksList.list.Trustworthy.descr = "perkTrustworthy";
	ChrPerksList.list.Trustworthy.condition.AdvancedCommerce = true;
	ChrPerksList.list.Trustworthy.condition.IronWill = true;
	ChrPerksList.list.Trustworthy.BaseType = "self";
	ChrPerksList.list.Trustworthy.PlayerOnly = true;
	
	/////////////////////////  КОРАБЕЛЬНЫЕ /////////////////////////////////////
    /////////////////////////////////////////////////////////////
	/// Перки для ГГ
	/////////////////////////////////////////////////////////////
    ChrPerksList.list.FlagPir.descr = "perkFlagPir";   // лично ГГ
    ChrPerksList.list.FlagPir.BaseType = "ship";
    ChrPerksList.list.FlagPir.PlayerOnly = true;

    ChrPerksList.list.FlagEng.descr = "perkFlagEng";
    ChrPerksList.list.FlagEng.BaseType = "ship";
    ChrPerksList.list.FlagEng.PlayerOnly = true;

    ChrPerksList.list.FlagFra.descr = "perkFlagFra";
    ChrPerksList.list.FlagFra.BaseType = "ship";
    ChrPerksList.list.FlagFra.PlayerOnly = true;

    ChrPerksList.list.FlagSpa.descr = "perkFlagSpa";
    ChrPerksList.list.FlagSpa.BaseType = "ship";
    ChrPerksList.list.FlagSpa.PlayerOnly = true;

    ChrPerksList.list.FlagHol.descr = "perkFlagHol";
    ChrPerksList.list.FlagHol.BaseType = "ship";
    ChrPerksList.list.FlagHol.PlayerOnly = true;
    
	/////////////////////////////////////////////////////////////
	/// Перки для боцман
	/////////////////////////////////////////////////////////////
	ChrPerksList.list.Brander.descr = "perkBrander";  // боцман
	ChrPerksList.list.Brander.BaseType = "ship";
	ChrPerksList.list.Brander.OfficerType = "boatswain";

	/*ChrPerksList.list.Aboard.descr = "perkAboard";
	ChrPerksList.list.Aboard.condition.Brander = true;
	ChrPerksList.list.Aboard.BaseType = "ship"; */ // без перека есть сразу

	ChrPerksList.list.Troopers.descr = "perkTroopers";  // боцман
	ChrPerksList.list.Troopers.BaseType = "ship";
	ChrPerksList.list.Troopers.OfficerType = "boatswain";
	
	ChrPerksList.list.LongRangeGrappling.descr = "perkLongRangeGrappling";  // боцман
	ChrPerksList.list.LongRangeGrappling.BaseType = "ship";
    ChrPerksList.list.LongRangeGrappling.OfficerType = "boatswain";
    
	ChrPerksList.list.GrapplingProfessional.descr = "perkGrapplingProfessional";   // боцман
	ChrPerksList.list.GrapplingProfessional.condition.LongRangeGrappling = true;
	ChrPerksList.list.GrapplingProfessional.condition.MusketsShoot = true;
	ChrPerksList.list.GrapplingProfessional.BaseType = "ship";
	ChrPerksList.list.GrapplingProfessional.OfficerType = "boatswain";
	
	ChrPerksList.list.MusketsShoot.descr = "perkMusketsShoot";     // боцман
	ChrPerksList.list.MusketsShoot.condition.CrewDamageUp = true;
	ChrPerksList.list.MusketsShoot.BaseType = "ship";
	ChrPerksList.list.MusketsShoot.OfficerType = "boatswain";

    /////////////////////////////////////////////////////////////
	/// Перки для казначе
	/////////////////////////////////////////////////////////////
	
	ChrPerksList.list.BasicCommerce.descr = "perkBasicCommerce";  // казначей
	ChrPerksList.list.BasicCommerce.BaseType = "ship";
	ChrPerksList.list.BasicCommerce.OfficerType = "treasurer";

	ChrPerksList.list.AdvancedCommerce.descr = "perkAdvancedCommerce";  // казначей
	ChrPerksList.list.AdvancedCommerce.condition.BasicCommerce = true;
	ChrPerksList.list.AdvancedCommerce.BaseType = "ship";
	ChrPerksList.list.AdvancedCommerce.OfficerType = "treasurer";

	/////////////////////////////////////////////////////////////
	/// Перки для канонир
	/////////////////////////////////////////////////////////////
	ChrPerksList.list.FastReload.descr = "perkFastReload";  // канонир
	ChrPerksList.list.FastReload.BaseType = "ship";
	ChrPerksList.list.FastReload.OfficerType = "cannoner";

	ChrPerksList.list.ImmediateReload.descr = "perkImmediateReload";  // канонир
	ChrPerksList.list.ImmediateReload.condition.FastReload = true;
	ChrPerksList.list.ImmediateReload.TimeDelay = 300;
	ChrPerksList.list.ImmediateReload.TimeDuration = 60;
	ChrPerksList.list.ImmediateReload.BaseType = "ship";
	ChrPerksList.list.ImmediateReload.OfficerType = "cannoner";

	ChrPerksList.list.HullDamageUp.descr = "perkHullDamageUp";   // канонир
	ChrPerksList.list.HullDamageUp.BaseType = "ship";
	ChrPerksList.list.HullDamageUp.OfficerType = "cannoner";

	ChrPerksList.list.SailsDamageUp.descr = "perkSailsDamageUp";  // канонир
	ChrPerksList.list.SailsDamageUp.BaseType = "ship";
	ChrPerksList.list.SailsDamageUp.OfficerType = "cannoner";

	ChrPerksList.list.CrewDamageUp.descr = "perkCrewDamageUp";   // канонир
	ChrPerksList.list.CrewDamageUp.BaseType = "ship";
	ChrPerksList.list.CrewDamageUp.OfficerType = "cannoner";

	ChrPerksList.list.CriticalShoot.descr = "perkCriticalShoot";  // канонир
	ChrPerksList.list.CriticalShoot.condition.HullDamageUp = true;
	ChrPerksList.list.CriticalShoot.condition.SailsDamageUp = true;
	ChrPerksList.list.CriticalShoot.condition.CrewDamageUp = true;
	ChrPerksList.list.CriticalShoot.BaseType = "ship";
	ChrPerksList.list.CriticalShoot.OfficerType = "cannoner";

	ChrPerksList.list.LongRangeShoot.descr = "perkLongRangeShoot";   // канонир
	ChrPerksList.list.LongRangeShoot.BaseType = "ship";
	ChrPerksList.list.LongRangeShoot.OfficerType = "cannoner";

	ChrPerksList.list.CannonProfessional.descr = "perkCannonProfessional";  // канонир
	ChrPerksList.list.CannonProfessional.condition.CriticalShoot = true;
	ChrPerksList.list.CannonProfessional.condition.LongRangeShoot = true;
	ChrPerksList.list.CannonProfessional.condition.FastReload = true;
	ChrPerksList.list.CannonProfessional.BaseType = "ship";
	ChrPerksList.list.CannonProfessional.OfficerType = "cannoner";



	// Ship Control Skills Group
	//================================================================
	/////////////////////////////////////////////////////////////
	/// Перки для плотника
	/////////////////////////////////////////////////////////////
	ChrPerksList.list.Carpenter.descr = "perkCarpenter";
    ChrPerksList.list.Carpenter.OfficerType = "carpenter";
    ChrPerksList.list.Carpenter.BaseType = "ship";
    
	ChrPerksList.list.Builder.descr = "perkBuilder";
	ChrPerksList.list.Builder.condition.Carpenter = true;
    ChrPerksList.list.Builder.OfficerType = "carpenter";
    ChrPerksList.list.Builder.BaseType = "ship";
    
	ChrPerksList.list.LightRepair.descr = "perkLightRepair";   // плотник
	ChrPerksList.list.LightRepair.TimeDelay = 240;
	ChrPerksList.list.LightRepair.TimeDuration = 60;
	ChrPerksList.list.LightRepair.BaseType = "ship";
	ChrPerksList.list.LightRepair.OfficerType = "carpenter";

    if (MOD_SKILL_ENEMY_RATE < 10)
    {
    	ChrPerksList.list.InstantRepair.descr = "perkInstantRepair";    // плотник
    	ChrPerksList.list.InstantRepair.condition.LightRepair = true;
    	ChrPerksList.list.InstantRepair.condition.Carpenter = true;
    	ChrPerksList.list.InstantRepair.TimeDelay = 240;// boal 13.02.2004
    	ChrPerksList.list.InstantRepair.TimeDuration = 30;
    	ChrPerksList.list.InstantRepair.BaseType = "ship";
    	ChrPerksList.list.InstantRepair.OfficerType = "carpenter";
    }
    
    ChrPerksList.list.BasicBattleState.descr = "perkBasicBattleState";   // плотник разделить с доктором корпус и команду
    ChrPerksList.list.BasicBattleState.OfficerType = "carpenter";
	ChrPerksList.list.BasicBattleState.BaseType = "ship";

	ChrPerksList.list.AdvancedBattleState.descr = "perkAdvancedBattleState";
	ChrPerksList.list.AdvancedBattleState.condition.BasicBattleState = true;
	ChrPerksList.list.AdvancedBattleState.OfficerType = "carpenter";
	ChrPerksList.list.AdvancedBattleState.BaseType = "ship";

	ChrPerksList.list.ShipDefenseProfessional.descr = "perkShipDefenseProfessional";
	ChrPerksList.list.ShipDefenseProfessional.condition.AdvancedBattleState = true;
	ChrPerksList.list.ShipDefenseProfessional.BaseType = "ship";
	ChrPerksList.list.ShipDefenseProfessional.OfficerType = "carpenter";
    /////////////////////////////////////////////////////////////
	/// Перки для штурман
	/////////////////////////////////////////////////////////////
	ChrPerksList.list.ShipSpeedUp.descr = "perkShipSpeedUp";  // штурман
	ChrPerksList.list.ShipSpeedUp.BaseType = "ship";
	ChrPerksList.list.ShipSpeedUp.OfficerType = "navigator";

	ChrPerksList.list.ShipTurnRateUp.descr = "perkShipTurnRateUp";    // штурман
	ChrPerksList.list.ShipTurnRateUp.BaseType = "ship";
	ChrPerksList.list.ShipTurnRateUp.OfficerType = "navigator";

	ChrPerksList.list.StormProfessional.descr = "perkStormProfessional";   // штурман
	ChrPerksList.list.StormProfessional.BaseType = "ship";
	ChrPerksList.list.StormProfessional.OfficerType = "navigator";

	ChrPerksList.list.WindCatcher.descr = "perkWindCatcher";
	ChrPerksList.list.WindCatcher.BaseType = "ship";
	ChrPerksList.list.WindCatcher.OfficerType = "navigator";

	ChrPerksList.list.SailsMan.descr = "perkSailsMan";
	ChrPerksList.list.SailsMan.BaseType = "ship";
	ChrPerksList.list.SailsMan.OfficerType = "navigator";
	
	ChrPerksList.list.Turn180.descr = "perkTurn180";           // штурман
	ChrPerksList.list.Turn180.condition.ShipSpeedUp = true;
	ChrPerksList.list.Turn180.condition.ShipTurnRateUp = true;
	ChrPerksList.list.Turn180.condition.WindCatcher = true;
	ChrPerksList.list.Turn180.TimeDelay = 240;
	ChrPerksList.list.Turn180.TimeDuration = 60;
	ChrPerksList.list.Turn180.BaseType = "ship";
	ChrPerksList.list.Turn180.OfficerType = "navigator";

    ChrPerksList.list.SandbankManeuver.descr = "perkSandbankManeuver";   // штурман
	ChrPerksList.list.SandbankManeuver.condition.SailsMan = true;
	ChrPerksList.list.SandbankManeuver.condition.ShipTurnRateUp = true;
	ChrPerksList.list.SandbankManeuver.TimeDuration = 60;
	ChrPerksList.list.SandbankManeuver.TimeDelay = 180;
	ChrPerksList.list.SandbankManeuver.BaseType = "ship";
	ChrPerksList.list.SandbankManeuver.OfficerType = "navigator";

	ChrPerksList.list.SailingProfessional.descr = "perkSailingProfessional";   // штурман
	ChrPerksList.list.SailingProfessional.condition.SandbankManeuver = true;
	ChrPerksList.list.SailingProfessional.condition.StormProfessional = true;
	ChrPerksList.list.SailingProfessional.condition.WindCatcher = true;
	ChrPerksList.list.SailingProfessional.BaseType = "ship";
	ChrPerksList.list.SailingProfessional.OfficerType = "navigator";
    // список из ВМЛ <--


	/////////////////////////////////////////////////////////////
	/// Перки для доктора
	/////////////////////////////////////////////////////////////
	ChrPerksList.list.Doctor1.descr = "perkDoctor1";
    ChrPerksList.list.Doctor1.BaseType = "ship";
	ChrPerksList.list.Doctor1.OfficerType = "doctor";
	
	ChrPerksList.list.Doctor2.descr = "perkDoctor2";
	ChrPerksList.list.Doctor2.condition.Doctor1 = true;
    ChrPerksList.list.Doctor2.BaseType = "ship";
	ChrPerksList.list.Doctor2.OfficerType = "doctor";
	
	/////////////////////////////////////////////////////////////
	/// Перки для контрабанды
	/////////////////////////////////////////////////////////////
	//ChrPerksList.list.Smuggler.descr = "perkSmuggler";

	/////////////////////////////////////////////////////////////
	/// Скрытые перки
	/////////////////////////////////////////////////////////////
	
	// скрытый перк дает 1.5 к приросту энергии, дается ГГ и боссам уровней
	ChrPerksList.list.Energaiser.descr = "perkEnergaiser";
	ChrPerksList.list.Energaiser.Hiden = true;

}