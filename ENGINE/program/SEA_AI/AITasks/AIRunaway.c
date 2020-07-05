void AIRunaway_GroupRunaway(string sGroupID, string sGroupAwayFrom)
{
	ref rG = Group_GetGroupByID(sGroupID);

	if (Group_isDeadR(rG)) { return; }
	
	if (Group_FindGroup(sGroupAwayFrom) < 0)
	{
		Trace("AIRunaway_GroupRunaway can't find away group sGroupID = " + sGroupAwayFrom);
	}
	ref rCommander = Group_GetGroupCommander(sGroupAwayFrom);
 	if (!CheckAttribute(rCommander, "index"))
	{
		trace("AIRunaway_GroupRunaway нет rCommander.index для sGroupID = " + sGroupID);
		if (MOD_BETTATESTMODE == "On")
		{
		    Log_Info("AIRunaway_GroupRunaway нет rCommander.index для sGroupID = " + sGroupID);
		}
		return;
	}
	
	int iEnemyCommanderIndex = sti(rCommander.index);

	int i = 0;
	while (true)
	{
		int iCharacterIndex = Group_GetCharacterIndexR(rG, i); i++;
		if (iCharacterIndex < 0) { break; }
		if (LAi_IsDead(&Characters[iCharacterIndex])) { continue; }
		Characters[iCharacterIndex].SeaAI.Task.Target = iEnemyCommanderIndex;
		Ship_SetTaskRunaway(SECONDARY_TASK, iCharacterIndex, sti(Characters[iCharacterIndex].SeaAI.Task.Target));
	}	
}