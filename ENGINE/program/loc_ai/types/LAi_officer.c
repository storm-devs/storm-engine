/*
���: ������

	������������ �������:
		stay
		dialog
*/



#define LAI_TYPE_OFFICER		"officer"


//�������������
void LAi_type_officer_Init(aref chr)
{
	//����� ���
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	chr.chr_ai.type = LAI_TYPE_OFFICER;
	if(CheckAttribute(chr, "chr_ai.tmpl"))
	{
		if (chr.chr_ai.tmpl != LAI_TMPL_STAY)//������� �����!
		{
	    	if (!LAi_IsFightMode(GetMainCharacter()))
	        {
	    	    LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
	    	}
	    	else
	    	{
	            LAi_tmpl_fight_SetWaitState(chr);
	    	}
		}
	}
	else
	{
     	LAi_tmpl_SetFollow(chr, pchar, -1.0);
	}
	//��������� �������� ���������
	LAi_SetDefaultStayAnimation(chr);
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

void LAi_type_follower_Init(aref chr)
{
	//����� ���
	DeleteAttribute(chr, "location.follower");
	DeleteAttribute(chr, "chr_ai.type");
	chr.chr_ai.type = "follower";
	LAi_tmpl_SetFollow(chr, pchar, -1.0);
	//��������� �������� ���������
	LAi_SetDefaultStayAnimation(chr);
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

//��������������� ���� ���������
void LAi_type_officer_CharacterUpdate(aref chr, float dltTime)
{
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return;
	string btl = "";
	//��� ���������� ����������
	if(CheckAttribute(chr, "chr_ai.poison"))
	{
		if(LAi_GetCharacterRelHP(chr) < 0.8)
		{
			bool antidotUse = false;
			if(LAi_GetCharacterRelHP(chr) < 0.6)
			{
				if(GetCharacterItem(&Characters[sti(chr.index)], "potion4") > 0)
				{
					antidotUse = true;
					DoCharacterUsedItem(&Characters[sti(chr.index)], "potion4");
				}
			}
			if(!antidotUse)
			{
				if(GetCharacterItem(&Characters[sti(chr.index)], "potion3") > 0)
				{
					DoCharacterUsedItem(&Characters[sti(chr.index)], "potion3");
				}
			}
		}
	}
	//��������� �� �������� ���������
	float dist = 0.0;
	float dhlt;
	if(!GetCharacterDistByChr3D(chr, pchar, &dist)) dist = -1.0;
	if(!LAi_IsBottleWork(chr))
	{
		if(LAi_GetCharacterRelHP(chr) < 0.7)
		{
			dhlt = LAi_GetCharacterMaxHP(chr) - LAi_GetCharacterHP(chr);
			btl = FindHealthForCharacter(&Characters[sti(chr.index)], dhlt);
			DoCharacterUsedItem(&Characters[sti(chr.index)], btl);
		}
	}
	//Log_Info("LAi_type_officer_CharacterUpdate "+chr.chr_ai.tmpl);
	if (chr.chr_ai.tmpl == LAI_TMPL_STAY) return; // ������ ��� ������. ����� �� ���� 18.06.05
	
	//��������� �� �������� ���������
	if(!GetCharacterDistByChr3D(chr, GetMainCharacter(), &dist)) dist = -1.0;
	if (dist < 0.0 || dist > 30)
	{
        //if( SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0)
        //{
            LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
    		return;
		//}
	}
	LAi_type_officer_FindTarget(chr);   // fix
	//������� ��������
	if(chr.chr_ai.tmpl == LAI_TMPL_FOLLOW)
	{
		if(dist < 30.0)
		{
			LAi_type_officer_FindTarget(chr);
		}
	}else{
		if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT)
		{
			//������� �� ������������� ���������
			/*if(!LAi_IsBottleWork(chr)) //������� ����� �� ���������, �� ���� ���� ���
			{
				if(rand(100) < (50 + 49*LAi_GetCharacterFightLevel(chr)))
				{
					float hlt = MinHealthPotionForCharacter(&Characters[sti(chr.index)]);
					if(LAi_GetCharacterRelHP(chr) < 0.5) hlt = dhlt;
					dhlt = LAi_GetCharacterMaxHP(chr) - LAi_GetCharacterHP(chr);
					if(hlt <= dhlt)
					{
						btl = FindHealthForCharacter(&Characters[sti(chr.index)], dhlt);
						DoCharacterUsedItem(&Characters[sti(chr.index)], btl);
					}					
				}
			}  */
			//
			if(LAi_tmpl_fight_LostTarget(chr))
			{
				LAi_type_officer_FindTarget(chr);
			}else{
				if(LAi_tmpl_fight_IsGo(chr))
				{
					if(dist > 12.0)
					{
						//���� ������������
						if( SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0)
                        {
						    LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
						}
					}
				}
			}
		}
		else
		{
		    LAi_type_officer_FindTarget(chr);
		}
	}
}

void LAi_type_follower_CharacterUpdate(aref chr, float dltTime)
{
}

//�������� ��������� � �������
bool LAi_type_officer_CharacterLogin(aref chr)
{
	return true;
}

bool LAi_type_follower_CharacterLogin(aref chr)
{
	return true;
}

//�������� ��������� �� �������
bool LAi_type_officer_CharacterLogoff(aref chr)
{
	return true;
}

bool LAi_type_follower_CharacterLogoff(aref chr)
{
	return true;
}

//���������� ������ ���������
void LAi_type_officer_TemplateComplite(aref chr, string tmpl)
{
	if(chr.chr_ai.tmpl != LAI_TMPL_FOLLOW)
    {
        if (chr.chr_ai.tmpl != LAI_TMPL_STAY && SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0) // fix
        {
            LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
        }
    }
}

void LAi_type_follower_TemplateComplite(aref chr, string tmpl)
{
	if(chr.chr_ai.tmpl != LAI_TMPL_FOLLOW) LAi_tmpl_SetFollow(chr, pchar, -1.0);
}

//�������� � ������� ������� ������
void LAi_type_officer_NeedDialog(aref chr, aref by)
{
}

void LAi_type_follower_NeedDialog(aref chr, aref by)
{
}


//������ �� ������, ���� ���������� true �� � ���� ������ ����� ������ ������
bool LAi_type_officer_CanDialog(aref chr, aref by)
{
	if(LAi_IsBoardingProcess()) return false;
	//���� ��� �������, �� �������
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return false;
	if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT) return false;
	//���������� �� ������
	return true;
}

bool LAi_type_follower_CanDialog(aref chr, aref by)
{
	if(LAi_IsBoardingProcess()) return false;
	//���� ��� �������, �� �������
	if(chr.chr_ai.tmpl == LAI_TMPL_DIALOG) return false;
	if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT) return false;
	//���������� �� ������
	return true;
}

//������ ������
void LAi_type_officer_StartDialog(aref chr, aref by)
{
	//���� �� �������, ��������� ������ ��� ������� ����������
	LAi_CharacterSaveAy(chr);
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

void LAi_type_follower_StartDialog(aref chr, aref by)
{
	//���� �� �������, ��������� ������ ��� ������� ����������
	LAi_CharacterSaveAy(chr);
	CharacterTurnByChr(chr, by);
	LAi_tmpl_SetActivatedDialog(chr, by);
}

//��������� ������
void LAi_type_officer_EndDialog(aref chr, aref by)
{
	LAi_CharacterRestoreAy(chr);
	if (chr.chr_ai.tmpl != LAI_TMPL_STAY)//������� �����!
	   LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
}

void LAi_type_follower_EndDialog(aref chr, aref by)
{
	LAi_CharacterRestoreAy(chr);
	LAi_tmpl_SetFollow(chr, pchar, -1.0);
}


//�������� ���������
void LAi_type_officer_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{

}


//�������� ��������
void LAi_type_officer_Attacked(aref chr, aref by)
{
	//����� ����������
	if(!LAi_group_IsEnemy(chr, by)) return;
	float dist = -1.0;
	//��������� ����������� ����� ����
// boal ������ ������ ����, � �� ���� ������ �� �����
	/*if(chr.chr_ai.tmpl == LAI_TMPL_FIGHT)
	{
		bool canChange = false;
		if(LAi_tmpl_fight_IsGo(chr)) canChange = true;
		if(LAi_tmpl_fight_LostTarget(chr)) canChange = true;
		if(LAi_tmpl_fight_NoActive(chr)) canChange = true;
		if(!canChange) return;
		dist = -1.0;
		if(!GetCharacterDistByChr3D(chr, by, &dist)) return;
		if(dist >= 0.0)
		{
			if(dist < 5.0) return;
		}
	}*/
	//���������� ��������� �� ������ ����������
	dist = -1.0;
	if(!GetCharacterDistByChr3D(chr, by, &dist)) return;
	if(dist < 0.0) return;
	if(dist > 20.0) return;
	//������������
	if(!LAi_tmpl_SetFight(chr, by))
	{
		LAi_type_officer_FindTarget(chr);
	}
}


void LAi_type_officer_FindTarget(aref chr)
{
	//�������� ������� ������
	int trg = LAi_group_GetTarget(chr);
	if(trg >= 0)
	{
		if(LAi_type_officer_CheckDists(chr, &Characters[trg]))
		{
			if(LAi_tmpl_SetFight(chr, &Characters[trg])) return;
		}
	}
	trg = LAi_group_GetTarget(pchar);
	if(trg >= 0)
	{
		if(LAi_type_officer_CheckDists(chr, &Characters[trg]))
		{
			if(!LAi_tmpl_SetFight(chr, &Characters[trg]))
			{
				//�������� ������������ ������
				//LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
				LAi_tmpl_fight_SetWaitState(chr);
			}
		}
	}else{
		if(chr.chr_ai.tmpl != LAI_TMPL_FOLLOW)
		{
            if( SendMessage(GetMainCharacter(),"ls",MSG_CHARACTER_EX_MSG,"IsFightMode") == 0) // fix
            {
                LAi_tmpl_SetFollow(chr, GetMainCharacter(), -1.0);
            }
		}
	}
}

bool LAi_type_officer_CheckDists(aref chr, aref trg)
{
	float dist = -1.0;
	if(!GetCharacterDistByChr3D(pchar, trg, &dist)) dist = -1.0;
	if(dist >= 0.0 && dist < 30.0)
	{
		if(!GetCharacterDistByChr3D(chr, trg, &dist)) dist = -1.0;
		if(dist >= 0.0 && dist < 30.0)
		{
			return true;
		}
	}
	return false;
}

