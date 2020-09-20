

//#event_handler("Event_ChrSnd_Body", "LAi_ChrSnd_Body");
void LAi_ChrSnd_Body()
{
	return;

	aref chr = GetEventData();
	if(CheckAttribute(chr, "sex"))
	{
		switch(chr.sex)
		{
		case "man":
			LAi_CharacterPlaySound(chr, "body");
			break;
		case "woman":
			LAi_CharacterPlaySound(chr, "body");
			break;
		case "skeleton":
			LAi_CharacterPlaySound(chr, "skeleton_body");
			break;
		case "monkey":
			LAi_CharacterPlaySound(chr, "monkey_body");
			break;
		}
	}
}

//#event_handler("Event_ChrSnd_Dead", "LAi_ChrSnd_Dead");
void LAi_ChrSnd_Dead()
{
	return;


	aref chr = GetEventData();
	if(CheckAttribute(chr, "sex"))
	{
		switch(chr.sex)
		{
		case "man":
			LAi_CharacterPlaySound(chr, "dead");
			break;
		case "woman":
			LAi_CharacterPlaySound(chr, "dead_wom");
			break;
		case "skeleton":
			LAi_CharacterPlaySound(chr, "skeleton_dead");
			break;
		case "monkey":
			LAi_CharacterPlaySound(chr, "monkey_dead");
			break;
		}
	}
}

//#event_handler("Event_ChrSnd_Attack", "LAi_ChrSnd_Attack");
void LAi_ChrSnd_Attack()
{
	return;



	aref chr = GetEventData();
	if(CheckAttribute(chr, "sex"))
	{
		switch(chr.sex)
		{
		case "man":
			LAi_CharacterPlaySound(chr, "man_attack");
			break;
		case "woman":
			LAi_CharacterPlaySound(chr, "woman_attack");
			break;
		case "skeleton":
			LAi_CharacterPlaySound(chr, "skeleton_attack");
			break;
		case "monkey":
			LAi_CharacterPlaySound(chr, "monkey_attack");
			break;
		}
	}
}

//#event_handler("Event_ChrSnd_Hit", "LAi_ChrSnd_Hit");
void LAi_ChrSnd_Hit()
{
	return;

	aref chr = GetEventData();
	if(CheckAttribute(chr, "sex"))
	{
		switch(chr.sex)
		{
		case "man":
			LAi_CharacterPlaySound(chr, "man_hit");
			break;
		case "woman":
			LAi_CharacterPlaySound(chr, "woman_hit");
			break;
		case "skeleton":
			LAi_CharacterPlaySound(chr, "skeleton_hit");
			break;
		case "monkey":
			LAi_CharacterPlaySound(chr, "monkey_hit");
			break;
		}
	}
}
