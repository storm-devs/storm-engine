object NetSkills[16];
int iNetSkillsNum;

ref Net_GetSkillByName(string sSkill) 
{
	for (int i=0; i<iNetSkillsNum; i++) { if (sSkill == NetSkills[i].Name) return &NetSkills[i]; }
	trace("WARNING! NetSkills ID (" + sSkill + ") not found!");
}
ref Net_GetSkillByIndex(int iIndex) { return &NetSkills[iIndex]; }
int Net_GetSkillsNum() { return iNetSkillsNum; }

int Net_GetSkillCost(int iSkillIndex, int iNumSkillPoints)
{
	return sti(NetSkills[iSkillIndex].Cost) * iNumSkillPoints;
}

void Net_InitSkills()
{
	ref rS;
	int n = 0;

	makeref(rS, NetSkills[n]); n++;
	rS.Name = "Accuracy";
	rS.Cost = 150;

	makeref(rS, NetSkills[n]); n++;
	rS.Name = "Cannons";
	rS.Cost = 150;

	makeref(rS, NetSkills[n]); n++;
	rS.Name = "Sailing";
	rS.Cost = 100;
	
	makeref(rS, NetSkills[n]); n++;
	rS.Name = "Defence";
	rS.Cost = 150;

	makeref(rS, NetSkills[n]); n++;
	rS.Name = "Repair";
	rS.Cost = 200;

	iNetSkillsNum = n;
}