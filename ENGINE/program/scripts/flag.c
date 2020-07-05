

void ChangeCharacterNation(aref chr, int iNation)
{
	chr.nation = iNation;
	ChangeCharacterFlag(chr, iNation);

}

void ChangeCharacterFlag(aref chr, int iNation)
{
	if(!CheckAttribute(chr, "nation.flag"))
	{
		chr.nation.flag = iNation;
	}

	if(sti(chr.nation.flag) != iNation)
	{
		chr.nation.flag = iNation;
		int iLuck = sti(chr.skills.sneak);
		int iFame = sti(chr.fame);

		int isSucces = rand(100);
		int iLuckModif = rand(12);

		if(iFame < isSucces)
		{
			isSucces = isSucces - iFame;
		}
		if(iLuck < iLuckModif)
		{
			isSucces = isSucces - iLuckModif + iLuck;
		}

		if(isSucces > rand(isSucces))
		{
			chr.nation.flag.success = 1;
		}
		else
		{
			chr.nation.flag.success = -1;
		}
	}
}