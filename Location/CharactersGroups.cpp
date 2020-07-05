//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	CharactersGroups
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#include "CharactersGroups.h"
#include "NPCharacter.h"


#define CGS_LOOK				15.0f
#define CGS_HEAR				2.5f
#define CGS_SAY					5.0f
#define CGS_START_ALARM			0.0f
#define CGS_ALARMDOWN			0.02f
#define CGS_ALARMMIN			0.3f
#define CGS_ALARMMAX			0.7f
#define CGS_ALARMVIS			0.3f
#define CGS_DEFMAXTIME			20.0f
#define CGS_VIEWANGLE			100.0f


//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

CharactersGroups::CharactersGroups()
{
	groups = null;
	numGroups = 0;
	maxGroups = 0;
	location = null;
	waveTime = 10.0f;
	curExecuteChr = -1;
}

CharactersGroups::~CharactersGroups()
{
	if(groups)
	{
		for(long i = 0; i < numGroups; i++) 
		{
			if(groups[i]->relations) delete groups[i]->relations;
			delete groups[i];
		}
		delete groups;
	}
}

CharactersGroups::String::String()
{
	name = null;
	len = 0;
	max = 0;
	hash = 0;
}

CharactersGroups::String::String(const char * str)
{
	name = null;
	len = 0;
	max = 0;
	hash = 0;
	*this = str;
}

CharactersGroups::String::~String()
{
	if(name) delete name;
}

void CharactersGroups::String::operator = (const char * str)
{
	if(!str || !str[0])
	{
		len = 0;
		name = NEW char[1];
		name[0] = 0;
	}else{
		len = strlen(str);
		if(len + 1 > max)
		{
			if(name) delete name;
			max = (len + 16) & ~15;
			name = NEW char[max];
		}
		strcpy(name, str);
		hash = LocatorArray::CalcHashString(str);
	}
}

bool CharactersGroups::String::Cmp(const char * str, long l, long h)
{
	if(!name || !name[0])
	{
		if(!str || !str[0]) return true;
		return false;
	}
	if(hash != h) return false;
	if(len != l) return false;
	return stricmp(name, str) == 0;
}

long CharactersGroups::String::GetHash(const char * str)
{
	if(!str) return 0;
	return LocatorArray::CalcHashString(str);
}

long CharactersGroups::String::GetLen(const char * str)
{
	if(!str) return 0;
	return strlen(str);
}


//Инициализация
bool CharactersGroups::Init()
{
	//Указатель на локацию
	ENTITY_ID loc;
	_CORE_API->FindClass(&loc, "location", 0);
	location = (Location *)_CORE_API->GetEntityPointer(&loc);
	if(!location) return false;
	RegistryGroup("");
	_CORE_API->LayerCreate("execute", true, false);
	_CORE_API->LayerSetFlags("execute", LRFLAG_EXECUTE);
	_CORE_API->LayerAdd("execute", GetID(), 10);
	return true;
}

//Исполнение
void CharactersGroups::Execute(dword delta_time)
{
/*#ifndef _XBOX
#ifdef _DEBUG
	if(api->Controls->GetDebugAsyncKeyState(VK_F11) < 0) DumpRelations();
#endif
#endif*/ //boal свои у нас читы
	//Время
	float dltTime = delta_time*0.001f;
	//Исполняем группы
	long playerGroup = FindGroupIndex("Player");
	float playerAlarm = 0.0f;
	bool playerActive = false;
	bool isDeactivate = false;
	for(long i = 0; i < numGroups; i++)
	{
		Relation * rel = groups[i]->relations;
		for(long j = 0; j <= i; j++)
		{
			rel[j].alarm -= dltTime*rel[j].alarmdown;
			if(rel[j].alarm < 0.0f) rel[j].alarm = 0.0f;
			bool lastActive = rel[j].isActive;
			if(rel[j].alarm <= rel[j].alarmmin) rel[j].isActive = false;
			if(rel[j].alarm >= rel[j].alarmmax) rel[j].isActive = true;
			if(rel[j].isActive != lastActive)
			{
				if(!rel[j].isActive)
				{
					rel[j].curState = rel[j].relState;
					isDeactivate = true;
				}else{
					rel[j].curState = rel[j].actState;
				}
			}
		}
		if(playerGroup >= 0 && playerGroup != i && groups[i]->numChr > 0)
		{
			Relation & rl = FindRelation(playerGroup, i);
			if(rl.curState != rs_enemy) continue;
			if(playerAlarm < rl.alarm) playerAlarm = rl.alarm;
			for(long n = 0; n < groups[i]->numChr; n++)
			{
				Character * cg = (Character *)api->GetEntityPointer(&groups[i]->c[n]);
				if(cg && cg->IsSetBlade()) break;
			}
			if(n >= groups[i]->numChr) continue;			
			if(rl.isActive) playerActive = true;
		}
	}
	//Удалим все неправильные цели
	if(isDeactivate) RemoveAllInvalidTargets();
	//Сообщим о текущем положении дел у игрока
	api->Event("CharacterGroup_UpdateAlarm", "fl", playerAlarm, playerActive);	
	//Исполняем персонажей
	waveTime += dltTime;
	if(curExecuteChr >= 0)
	{
		if(curExecuteChr < location->supervisor.numCharacters)
		{
			Character * c = location->supervisor.character[curExecuteChr].c;
			CharacterVisibleCheck(c);
			curExecuteChr++;
		}else curExecuteChr = -1;
	}else{
		if(waveTime >= 0.5f)
		{
			waveTime = 0.0f;
			curExecuteChr = 0;
		}
	}
}

//Проверка обнаружения персонажем остальных
void CharactersGroups::CharacterVisibleCheck(Character * chr)
{
	if(!chr) return;
	//Удалим отработавшие цели
	RemoveInvalidTargets(chr);
	//Группа персонажа
	long gi = GetCharacterGroup(chr);
	if(gi < 0) return;
	Group * grp = groups[gi];
	//Видимая область
	long num;
	if(location->supervisor.FindCharacters(fnd, num, chr, grp->look, CGS_VIEWANGLE, 0.05f))
	{
		FindEnemyFromFindList(chr, grp, num, true);
	}
	//Слышимая область
	if(location->supervisor.FindCharacters(fnd, num, chr, grp->hear))
	{
		FindEnemyFromFindList(chr, grp, num, false);
	}
}

//Проверить найденных персонажей на врагов
void CharactersGroups::FindEnemyFromFindList(Character * chr, Group * grp, long num, bool visCheck)
{
	Character * targets[MAX_CHARACTERS];
	long numTrg = 0;
	//По всем найденным персонажем
	for(long i = 0; i < num; i++)
	{
		//Группа найденного персонажа
		long gi = GetCharacterGroup(fnd[i].c);
		if(gi < 0) continue;
		Relation & r = FindRelation(grp->index, gi);
		if(r.curState != rs_enemy) continue;
		//Обнаружен враг, если невидим, то пропустим его
		if(visCheck && !chr->VisibleTest(fnd[i].c)) continue;
		//Обнаружили врага, добавим
		if(AddEnemyTarget(chr, fnd[i].c))
		{
			if(numTrg < MAX_CHARACTERS) targets[numTrg++] = fnd[i].c;
		}
	}
	//Сообщим окружающим об обнаруженных целях
	if(numTrg > 0 && location->supervisor.FindCharacters(fnd, num, chr, grp->say))
	{
		for(i = 0; i < num; i++)
		{
			Character * c = fnd[i].c;
			//Если невидим, то пропустим его
			if(!chr->VisibleTest(c)) continue;
			//Получим группу персонажа
			long cgrp = GetCharacterGroup(c);
			if(cgrp < 0) continue;
			//Группа найденного персонажа
			Relation & r = FindRelation(grp->index, cgrp);
			if(r.curState != rs_friend) continue;
			for(long j = 0; j < numTrg; j++)
			{
				if(grp->index != cgrp)
				{
					long tgrp = GetCharacterGroup(targets[j]);
					if(tgrp >= 0)
					{
						Relation & tr = FindRelation(cgrp, tgrp);
						if(tr.curState == rs_friend)
						{
							//Обе группы дружественны к текущей, смотрим на приоритеты
							if(groups[tgrp]->priority >= groups[grp->index]->priority) continue;
						}
					}
				}
				AddEnemyTarget(c, targets[j]);
			}
		}
	}
}

//Добавить или обновить врага
bool CharactersGroups::AddEnemyTarget(Character * chr, Character * enemy, float maxtime)
{
	if(!chr || !enemy) return false;
	//Обновим аларм
	bool isSelf = false;
	Relation & r = FindRelation(GetCharacterGroup(chr), GetCharacterGroup(enemy));
	Assert(!isSelf);
	r.alarm = 1.0f;
	if(r.alarm >= r.alarmmax) r.isActive = true;
	if(r.isActive) r.curState = r.actState;	
	if(r.actState != rs_enemy) return false;
	//Ищим среди добавленных
	for(long i = 0; i < chr->numTargets; i++)
	{
		if(enemy == api->GetEntityPointer(&chr->grpTargets[i].chr))
		{
			chr->grpTargets[i].time = 0.0f;
			return true;
		}
	}
	if(chr->numTargets >= sizeof(chr->grpTargets)/sizeof(Character::GrpTarget)) return false;
	//Добавим новую цель
	Assert(stricmp(chr->group, enemy->group) != 0);
	Character::GrpTarget & trg = chr->grpTargets[chr->numTargets++];
	trg.chr = enemy->GetID();
	trg.time = 0.0f;
	if(maxtime < 0.0f) trg.timemax = 3.0f + (rand() & 7); else trg.timemax = maxtime;
	return true;
}

//Удалить все неактивные или неправильные цели
void CharactersGroups::RemoveAllInvalidTargets()
{
	//Обновим списки целей
	for(long i = 0; i < location->supervisor.numCharacters; i++)
	{
		RemoveInvalidTargets(location->supervisor.character[i].c);
	}
}

//Удалить неактивные или неправильные цели
bool CharactersGroups::RemoveInvalidTargets(Character * chr, Character * check)
{
	if(!chr) return false;
	long gi = GetCharacterGroup(chr);
	if(gi < 0)
	{
		chr->numTargets = 0;
		return false;
	}
	bool isValidate = false;
	for(long i = 0; i < chr->numTargets; )
	{
		bool isDelete = true;
		Character::GrpTarget & trg = chr->grpTargets[i];
		Character * c = (Character *)api->GetEntityPointer(&trg.chr);
		if(c && (trg.time < trg.timemax || trg.timemax < 0.0f))
		{
			if(!c->IsDead())
			{
				//Персонаж существует и о нём помнят
				long gc = GetCharacterGroup(c);
				Relation & r = FindRelation(gi, gc);
				if(r.curState == rs_enemy)
				{
					isDelete = false;
					if(check == c) isValidate = true;
				}
			}
		}
		if(isDelete)
		{
			trg = chr->grpTargets[--chr->numTargets];
		}else i++;
	}
	return isValidate;
}

//Сообщения
dword _cdecl CharactersGroups::ProcessMessage(MESSAGE & message)
{
	char cmd[64];
	message.String(sizeof(cmd), cmd); cmd[sizeof(cmd) - 1] = 0;
	if(!cmd[0]) return 0;
	if(stricmp(cmd, "VldTrg") == 0)
	{
		return MsgIsValidateTarget(message);
	}else
	if(stricmp(cmd, "GetTrg") == 0)
	{
		return MsgGetOptimalTarget(message);
	}else
	if(stricmp(cmd, "IsEnemy") == 0)
	{
		return MsgIsEnemy(message);
	}else
	if(stricmp(cmd, "MoveChr") == 0)
	{
		return MoveCharacterToGroup(message);
	}else
	if(stricmp(cmd, "Attack") == 0)
	{
		MsgAttack(message);
		return 1;
	}else
	if(stricmp(cmd, "AddTarget") == 0)
	{
		MsgAddTarget(message);
		return 1;
	}else
	if(stricmp(cmd, "UpdChrTrg") == 0)
	{
		MsgUpdChrTrg(message);
		return 1;
	}else
	if(stricmp(cmd, "RegistryGroup") == 0)
	{
		MsgRegistryGroup(message);
		return 1;
	}else
	if(stricmp(cmd, "ReleaseGroup") == 0)
	{
		MsgReleaseGroup(message);
		return 1;
	}else
	if(stricmp(cmd, "SetRelation") == 0)
	{
		MsgSetRelation(message);
		return 1;
	}else
	if(stricmp(cmd, "SetAlarmReaction") == 0)
	{
		MsgSetAlarmReaction(message);
		return 1;
	}else
	if(stricmp(cmd, "SetGroupLook") == 0)
	{		
		return MsgSetGroupLook(message);
	}else
	if(stricmp(cmd, "SetGroupHear") == 0)
	{		
		return MsgSetGroupHear(message);
	}else
	if(stricmp(cmd, "SetGroupSay") == 0)
	{
		return MsgSetGroupSay(message);
	}else
	if(stricmp(cmd, "SetGroupPriority") == 0)
	{
		return MsgSetGroupPriority(message);
	}else
	if(stricmp(cmd, "UnloadCharacter") == 0)
	{
		UnloadCharacter(message);
		return 1;
	}else
	if(stricmp(cmd, "ResetWaveTime") == 0)
	{
		waveTime = 1000.0f;
		return 1;
	}else
	if(stricmp(cmd, "SetAlarm") == 0)
	{
		return MsgSetAlarm(message);
	}else
	if(stricmp(cmd, "SetAlarmDown") == 0)
	{
		return MsgSetAlarmDown(message);
	}else
	if(stricmp(cmd, "ClearAllTargets") == 0)
	{
		ClearAllTargets();
		return 1;
	}else
	if(stricmp(cmd, "SaveData") == 0)
	{
		SaveData();
		return 1;
	}else
	if(stricmp(cmd, "LoadDataRelations") == 0)
	{
		LoadDataRelations();
		return 1;
	}else
	if(stricmp(cmd, "RestoreStates") == 0)
	{
		RestoreStates();
		return 1;
	}
	return 0;
}

//Изменение атрибута
dword CharactersGroups::AttributeChanged(ATTRIBUTES * apnt)
{

	return 0;
}

//Проверить на действительность цель
bool CharactersGroups::MsgIsValidateTarget(MESSAGE & message)
{
	ENTITY_ID chr = message.EntityID();
	ENTITY_ID trg = message.EntityID();
	Character * c = (Character *)api->GetEntityPointer(&chr);
	if(!c) return false;
	Character * en = (Character *)api->GetEntityPointer(&trg);
	if(!en) return false;
	CVECTOR vP1, vP2;
	c->GetPosition(vP1);
	en->GetPosition(vP2);
	float fDistance = sqrtf(~(vP1 - vP2));
	AttributesPointer->SetAttributeUseFloat("distance", fDistance);
	return RemoveInvalidTargets(c, en);
}

//Найти оптимальную цель
bool CharactersGroups::MsgGetOptimalTarget(MESSAGE & message)
{
	ENTITY_ID chr = message.EntityID();
	Character * c = (Character *)api->GetEntityPointer(&chr);
	if(!c) return false;
	VDATA * vd = message.ScriptVariablePointer();
	if(!vd) return false;
	if(c->numTargets <= 0) return false;
	long s = 0;
	if(c->numTargets > 1)
	{
		CVECTOR pos, p;
		c->GetPosition(pos);
		long numChr = location->supervisor.numCharacters;
		Supervisor::CharacterEx * cEx = location->supervisor.character;
		//Выберем оптимальную цель
		float value;
		s = -1;
		for(long i = 0; i < c->numTargets; i++)
		{
			//Указатель на персонажа
			NPCharacter * nc = (NPCharacter *)api->GetEntityPointer(&c->grpTargets[i].chr);
			if(!nc) continue;			
			if(!nc->IsSetBlade()) continue;
			//Соберём количество персонажей воюющих с этим хмырём
			for(long j = 0, n = 0; j < numChr; j++)
			{
				if(cEx[j].c == nc || cEx[j].c == c) continue;
				if(((NPCharacter *)cEx[j].c)->GetAttackedCharacter() == nc) n++;
			}
			//Найдём дистанцию
			nc->GetPosition(p);
			float v = ~(pos - p);
			//Посчитаем эврестическое значение
			v *= 0.5f + n*n*0.5f;
			//Учтём наличие у него оружия
			//if(!nc->IsSetBlade()) v *= v*100.0f;
			//Учитываем выбор цели
			if(s >= 0)
			{
				if(v < value)
				{
					value = v;
					s = i;
				}
			}else{
				value = v;
				s = i;
			}
		}
		if(s < 0) s = 0;
	}
	c = (Character *)api->GetEntityPointer(&c->grpTargets[s].chr);
	//if(!c->IsSetBlade()) return false;
	if(c->AttributesPointer)
	{
		vd->Set(long(c->AttributesPointer->GetAttributeAsDword("index", -1)));
	}else{
		vd->Set(long(-1));
	}	
	return true;
}

//Враг ли данный персонаж
bool CharactersGroups::MsgIsEnemy(MESSAGE & message)
{
	long g1 = GetCharacterGroup((Character *)api->GetEntityPointer(&message.EntityID()));
	long g2 = GetCharacterGroup((Character *)api->GetEntityPointer(&message.EntityID()));
	if(g1 < 0 || g2 < 0) return false;
	bool isSelf = false;
	Relation & r = FindRelation(g1, g2, &isSelf);
	if(isSelf) return false;
	return r.curState == rs_enemy;
}

//Реакция групп на атаку
void CharactersGroups::MsgAttack(MESSAGE & message)
{
	//Получим группы
	Group * gAttack = GetGroup(message);
	Group * gHit = GetGroup(message);
	Assert(gAttack);
	Assert(gHit);
	Relation & r = FindRelation(gAttack->index, gHit->index);
	r.alarm = 1.0f;
	if(r.alarm >= r.alarmmax) r.isActive = true;
	if(r.isActive) r.curState = r.actState;
	//Установим враждебные отношения у всех дружественных к пострадавшей групп
	for(long i = 0; i < numGroups; i++)
	{
		if(gHit == groups[i]) continue;
		if(gAttack == groups[i]) continue;
		Relation & r = FindRelation(gHit->index, i);
		if(r.curState != rs_friend) continue;
		//Посорим с нападающими
		Relation & ra = FindRelation(gAttack->index, i);
		ra.alarm = 1.0f;
		if(ra.alarm >= ra.alarmmax) ra.isActive = true;
		if(ra.isActive) ra.curState = ra.actState;
	}
}

void CharactersGroups::MsgAddTarget(MESSAGE & message)
{
	//Получаем персонажей
	ENTITY_ID eid = message.EntityID();
	Character * chr = (Character *)api->GetEntityPointer(&eid);
	eid = message.EntityID();
	Character * enemy = (Character *)api->GetEntityPointer(&eid);
	if(!chr || !enemy) return;
	//Проверяем на враждебность
	long g1 = GetCharacterGroup(chr);
	long g2 = GetCharacterGroup(enemy);
	if(g1 < 0 || g2 < 0) return;
	bool isSelf = false;
	Relation & r = FindRelation(g1, g2, &isSelf);
	if(isSelf) return;
	if(r.curState != rs_enemy) return;
	//Добавляем врага
	AddEnemyTarget(chr, enemy, message.Float());
	//Сообщаем окружающим о новой цели
	long num = 0;
	if(location->supervisor.FindCharacters(fnd, num, chr, groups[g1]->say))
	{
		for(long i = 0; i < num; i++)
		{
			Character * c = fnd[i].c;
			//Если невидим, то пропустим его
			if(!chr->VisibleTest(c)) continue;
			//Группа найденного персонажа
			Relation & r = FindRelation(g1, GetCharacterGroup(c));
			if(r.curState != rs_friend) continue;
			AddEnemyTarget(c, enemy);			
		}
	}
}

//Обновить цели у данного персонажа
void CharactersGroups::MsgUpdChrTrg(MESSAGE & message)
{
	ENTITY_ID eid = message.EntityID();
	Character * chr = (Character *)api->GetEntityPointer(&eid);
	if(chr) CharacterVisibleCheck(chr);
}

//Зарегистрировать группу
void CharactersGroups::MsgRegistryGroup(MESSAGE & message)
{
	char grpName[128];
	message.String(sizeof(grpName), grpName); grpName[sizeof(grpName) - 1] = 0;
	RegistryGroup(grpName);
}

//Удалить группу
void CharactersGroups::MsgReleaseGroup(MESSAGE & message)
{
	char grpName[128];
	message.String(sizeof(grpName), grpName); grpName[sizeof(grpName) - 1] = 0;
	ReleaseGroup(grpName);
}

//Зарегистрировать группу
long CharactersGroups::RegistryGroup(const char * groupName)
{
	long idxgrp = FindGroupIndex(groupName);
	if(idxgrp >= 0) return idxgrp;
	//Регестрируем
	if(numGroups >= maxGroups)
	{
		maxGroups += 16;
		groups = (Group **)RESIZE(groups, sizeof(Group *)*maxGroups);
	}
	Group * grp = NEW Group();
	grp->index = numGroups;
	grp->name = groupName;
	grp->look = CGS_LOOK;
	grp->hear = CGS_HEAR;
	grp->say = CGS_SAY;
	grp->priority = 10;
	groups[numGroups++] = grp;
	if(numGroups)
	{
		//Таблица отношений
		grp->relations = NEW Relation[numGroups];
		for(long i = 0; i < numGroups - 1; i++)
		{
			grp->relations[i].alarm = CGS_START_ALARM;
			grp->relations[i].alarmdown = CGS_ALARMDOWN;
			grp->relations[i].alarmmin = CGS_ALARMMIN;
			grp->relations[i].alarmmax = CGS_ALARMMAX;
			grp->relations[i].isActive = false;
			if(grp->relations[i].alarm >= grp->relations[i].alarmmax) grp->relations[i].isActive = true;
			grp->relations[i].curState = rs_neitral;
			grp->relations[i].actState = rs_enemy;
			grp->relations[i].relState = rs_neitral;
		}
		//Сами с собой всегда дружим
		grp->relations[numGroups - 1].alarm = 0.0f;
		grp->relations[numGroups - 1].alarmdown = 0.0f;
		grp->relations[numGroups - 1].alarmmin = 0.0f;
		grp->relations[numGroups - 1].alarmmax = 10.0f;
		grp->relations[numGroups - 1].isActive = false;
		grp->relations[numGroups - 1].curState = rs_friend;
		grp->relations[numGroups - 1].actState = rs_friend;
		grp->relations[numGroups - 1].relState = rs_friend;
	}else grp->relations = null;
	grp->numChr = 0;
	return numGroups - 1;
}

void CharactersGroups::ReleaseGroup(const char * groupName)
{
	long idxgrp = FindGroupIndex(groupName);
	if(idxgrp < 0) return; // нет группы нечего удалять
	if( numGroups > 1 )
	{
		groups[idxgrp] = groups[numGroups-1];
		groups[numGroups-1] = 0;
	}
	numGroups--;
}

//Установить для группы радиус видимости
bool CharactersGroups::MsgSetGroupLook(MESSAGE & message)
{
	//Текущая группа
	Group * grp = GetGroup(message, false);
	if(!grp) return false;
	//Радиус
	grp->look = message.Float();
	if(grp->look < 0.0f) grp->look = 0.0f;
	return true;
}

//Установить для группы радиус слышимости
bool CharactersGroups::MsgSetGroupHear(MESSAGE & message)
{
	//Текущая группа
	Group * grp = GetGroup(message, false);
	if(!grp) return false;
	//Радиус
	grp->hear = message.Float();
	if(grp->hear < 0.0f) grp->hear = 0.0f;
	return true;
}

//Установить для группы радиус сообщения
bool CharactersGroups::MsgSetGroupSay(MESSAGE & message)
{
	//Текущая группа
	Group * grp = GetGroup(message, false);
	if(!grp) return false;
	//Радиус
	grp->say = message.Float();
	if(grp->say < 0.0f) grp->say = 0.0f;
	return true;
}

//Установить для группы приоритет
bool CharactersGroups::MsgSetGroupPriority(MESSAGE & message)
{
	//Текущая группа
	Group * grp = GetGroup(message, false);
	if(!grp) return false;
	//Приоритет
	grp->priority = message.Long();
	return true;
}

//Установить скорость уровень тревоги
bool CharactersGroups::MsgSetAlarm(MESSAGE & message)
{
	//Текущая группа
	Group * g1 = GetGroup(message, false);
	if(!g1) return false;
	Group * g2 = GetGroup(message, false);
	if(!g2) return false;
	bool isSelf = false;
	Relation & r = FindRelation(g1->index, g2->index, &isSelf);
	if(isSelf) return false;
	//Уровень тревоги
	r.alarm = message.Float();
	if(r.alarm < 0.0f) r.alarm = 0.0f;
	if(r.alarm > 1.0f) r.alarm = 1.0f;
	if(r.alarm <= r.alarmmin) r.isActive = false;
	if(r.alarm >= r.alarmmax) r.isActive = true;
	RemoveAllInvalidTargets();
	return true;
}

//Установить скорость убывания тревоги
bool CharactersGroups::MsgSetAlarmDown(MESSAGE & message)
{
	//Текущая группа
	Group * g1 = GetGroup(message, false);
	if(!g1) return false;
	Group * g2 = GetGroup(message, false);
	if(!g2) return false;
	bool isSelf = false;
	Relation & r = FindRelation(g1->index, g2->index, &isSelf);
	if(isSelf) return false;
	//Скорость убывания
	r.alarmdown = message.Float();
	if(r.alarmdown < 0.0f) r.alarmdown = CGS_ALARMDOWN;
	return true;
}

//Добавить в группу персонажа
bool CharactersGroups::MoveCharacterToGroup(MESSAGE & message)
{
	ENTITY_ID eid = message.EntityID();
	Character * chr = (Character *)api->GetEntityPointer(&eid);
	if(!chr) return false;
	//Создадим группу
	char grpName[128];
	message.String(sizeof(grpName), grpName); grpName[sizeof(grpName) - 1] = 0;
	Group * grp = FindGroup(grpName);
	if(!grp) RegistryGroup(grpName);
	grp = FindGroup(grpName);
	Assert(grp);
	//Удалим персонажа из предыдущей группы
	RemoveCharacterFromAllGroups(&eid);
	//Проверим на свободное место в группе
	// boal fix for intel cpp if(grp->numChr >= sizeof(CharactersGroups::Group::c)/sizeof(Character *)) return false;
	if(grp->numChr >= MAX_CHARACTERS) return false;//fix
	//Разместим в новой
	grp->c[grp->numChr++] = eid;
	strcpy(chr->group, grpName);
	RemoveInvalidTargets(chr);
	return true;
}

//Установить отношения между группами
void CharactersGroups::MsgSetRelation(MESSAGE & message)
{
	bool isSelf = false;
	Relation & r = FindRelation(message, &isSelf);
	if(isSelf) return;
	char buf[32];
	message.String(sizeof(buf), buf); buf[sizeof(buf) - 1] = 0;
	RelState actState = rs_enemy;
	RelState relState = rs_neitral;
	if(stricmp(buf, "friend") == 0)
	{
		r.curState = rs_friend;
		actState = rs_enemy;
		relState = rs_neitral;
		r.alarm = 0.0f;
		r.alarmdown = CGS_ALARMDOWN;
		r.alarmmin = CGS_ALARMMIN;
		r.alarmmax = CGS_ALARMMAX;
		r.isActive = false;
	}else
	if(stricmp(buf, "neitral") == 0)
	{
		r.curState = rs_neitral;
		actState = rs_enemy;
		relState = rs_neitral;
		r.alarm = 0.0f;
		r.alarmdown = CGS_ALARMDOWN;
		r.alarmmin = CGS_ALARMMIN;
		r.alarmmax = CGS_ALARMMAX;
	}else
	if(stricmp(buf, "enemy") == 0)
	{
		r.curState = rs_enemy;
		actState = rs_enemy;
		relState = rs_enemy;
		r.alarm = 1.0f;
		r.isActive = true;
		r.alarmdown = 0.0f;
		r.alarmmin = 0.0f;
		r.alarmmax = 10.0f;
	}
	r.actState = actState;
	r.relState = relState;
	if(r.isActive) r.curState = r.actState;
}

//Установить реакцию на тревогу для пары групп
void CharactersGroups::MsgSetAlarmReaction(MESSAGE & message)
{
	bool isSelf = false;
	Relation & r = FindRelation(message, &isSelf);
	if(isSelf) return;
	char act[32];
	message.String(sizeof(act), act); act[sizeof(act) - 1] = 0;
	char rel[32];
	message.String(sizeof(rel), rel); rel[sizeof(rel) - 1] = 0;
	RelState actState = rs_enemy;
	if(stricmp(act, "neitral") == 0)
	{
		actState = rs_neitral;
	}else
	if(stricmp(act, "friend") == 0)
	{
		actState = rs_friend;
	}
	RelState relState = rs_neitral;
	if(stricmp(rel, "enemy") == 0)
	{
		relState = rs_enemy;
	}else
	if(stricmp(rel, "friend") == 0)
	{
		relState = rs_friend;
	}
	r.actState = actState;
	r.relState = relState;
	if(r.isActive) r.curState = r.actState;
}

//Исключить персонажа из всех групп
void CharactersGroups::RemoveCharacterFromAllGroups(ENTITY_ID * chr)
{
	Character * ch = chr != null ? (Character *)api->GetEntityPointer(chr) : null;
	//Удалим персонажа из предыдущей группы
	for(long i = 0; i < numGroups; i++)
	{
		Group * g = groups[i];
		ENTITY_ID * cid  = g->c;
		for(long j = 0; j < g->numChr; )
		{
			Character * c = (Character *)api->GetEntityPointer(&cid[j]);
			if(c == null || c == ch)
			{
				cid[j] = cid[--g->numChr];
			}else j++;
		}
	}	
}

//Выгрузка персонажа
void CharactersGroups::UnloadCharacter(MESSAGE & message)
{
	ENTITY_ID eid = message.EntityID();
	RemoveCharacterFromAllGroups(&eid);
}


//Получить группу из сообщения
CharactersGroups::Group * CharactersGroups::GetGroup(MESSAGE & message, bool isRegistry)
{
	char grpName[128];
	message.String(sizeof(grpName), grpName); grpName[sizeof(grpName) - 1] = 0;
	Group * grp = FindGroup(grpName);
	if(!grp && isRegistry)
	{
		RegistryGroup(grpName);
		grp = FindGroup(grpName);
		Assert(grp);
	}
	return grp;
}

//Найти группу по имени
CharactersGroups::Group * CharactersGroups::FindGroup(const char * name)
{
	long gi = FindGroupIndex(name);
	if(gi < 0) return null;
	return groups[gi];
	if(!name) return null;
}

//Найти группу по имени
inline long CharactersGroups::FindGroupIndex(const char * name)
{
	if(!name) return -1;
	long l = String::GetLen(name);
	long h = String::GetHash(name);
	
	//Ищем среди зарегистрированных
	for(long i = 0; i < numGroups; i++)
	{
		if(groups[i]->name.Cmp(name, l, h)) return i;
	}
	return -1;
}

//Найти отношение групп
CharactersGroups::Relation & CharactersGroups::FindRelation(MESSAGE & message, bool * selfgroup)
{
	char grpName1[128];
	message.String(sizeof(grpName1), grpName1); grpName1[sizeof(grpName1) - 1] = 0;
	char grpName2[128];
	message.String(sizeof(grpName2), grpName2); grpName2[sizeof(grpName2) - 1] = 0;
	return FindRelation(grpName1, grpName2, selfgroup);
}

//Найти отношение групп
inline CharactersGroups::Relation & CharactersGroups::FindRelation(const char * name1, const char * name2, bool * selfgroup)
{
	long g1 = FindGroupIndex(name1);
	if(g1 < 0)
	{
		RegistryGroup(name1);
		g1 = FindGroupIndex(name1);
		Assert(g1 >= 0);
	}
	long g2 = FindGroupIndex(name2);
	if(g2 < 0)
	{
		RegistryGroup(name2);
		g2 = FindGroupIndex(name2);
		Assert(g2 >= 0);
	}
	return FindRelation(g1, g2, selfgroup);
}

//Найти отношение групп
inline CharactersGroups::Relation & CharactersGroups::FindRelation(long g1, long g2, bool * selfgroup)
{
	Assert(g1 >= 0 && g1 < numGroups);
	Assert(g2 >= 0 && g2 < numGroups);
	if(selfgroup) *selfgroup = (g1 == g2);
	if(g1 <= g2)
	{
		return groups[g2]->relations[g1];
	}
	return groups[g1]->relations[g2];
}

//Получить индекс группы персонажа
inline long CharactersGroups::GetCharacterGroup(Character * c)
{
	if(!c) return -1;
	if(c->groupID >= 0 && c->groupID < numGroups)
	{
		if(stricmp(c->group, groups[c->groupID]->name) == 0)
		{
			return c->groupID;
		}
	}
	c->groupID = FindGroupIndex(c->group);
	return c->groupID;
}

//Удалить все цели
void CharactersGroups::ClearAllTargets()
{
	//Обновим списки целей
	for(long i = 0; i < location->supervisor.numCharacters; i++)
	{
		location->supervisor.character[i].c->numTargets = 0;
	}
}



//Сохранить даные в объект
void CharactersGroups::SaveData()
{
	//Корневой атрибут для сохранения данных
	if(!AttributesPointer)
	{
		api->Trace("CharactersGroups::SaveData -> no attributes");
		return;
	}
	ATTRIBUTES * saveData = AttributesPointer->FindAClass(AttributesPointer, "savedata");
	if(saveData) AttributesPointer->DeleteAttributeClassX(saveData);
	saveData = AttributesPointer->CreateSubAClass(AttributesPointer, "savedata");
	//Сохраняем отношения групп
	for(long i = 0, cnt = 0; i < numGroups; i++)
	{
		for(long j = 0; j < i; j++)
		{
			//Раздел отношений
			char buf[16];
			sprintf(buf, "r%.4i", cnt++);
			ATTRIBUTES * grp = saveData->CreateSubAClass(saveData, buf);
			//Сохраним параметры групп
			grp->SetAttribute("name1", groups[i]->name.name);
			grp->SetAttributeUseFloat("look1", groups[i]->look);
			grp->SetAttributeUseFloat("hear1", groups[i]->hear);
			grp->SetAttributeUseFloat("say1", groups[i]->say);
			grp->SetAttributeUseDword("prt1", groups[i]->priority);
			grp->SetAttribute("name2", groups[j]->name.name);
			grp->SetAttributeUseFloat("look2", groups[j]->look);
			grp->SetAttributeUseFloat("hear2", groups[j]->hear);
			grp->SetAttributeUseFloat("say2", groups[j]->say);
			grp->SetAttributeUseDword("prt2", groups[j]->priority);
			//Сохраним отношения
			Relation & r = FindRelation(i, j);
			grp->SetAttributeUseFloat("alarm", r.alarm);
			grp->SetAttributeUseFloat("alarmdown", r.alarmdown);
			grp->SetAttributeUseFloat("alarmmin", r.alarmmin);
			grp->SetAttributeUseFloat("alarmmax", r.alarmmax);
			grp->SetAttributeUseDword("isactive", r.isActive);
			grp->SetAttributeUseDword("curState", r.curState);
			grp->SetAttributeUseDword("actState", r.actState);
			grp->SetAttributeUseDword("relState", r.relState);
		}
	}
	//Сохраняем параметры персонажей
	/*
	saveData = AttributesPointer->FindAClass(AttributesPointer, "characters");
	if(saveData) AttributesPointer->DeleteAttributeClassX(saveData);
	saveData = AttributesPointer->CreateSubAClass(AttributesPointer, "characters");
	*/

}

//Прочитать даные из объект
void CharactersGroups::LoadDataRelations()
{
	//Корневой атрибут для сохранения данных
	if(!AttributesPointer)
	{
		api->Trace("CharactersGroups::LoadDataRelations -> no attributes");
		return;
	}
	ATTRIBUTES * saveData = AttributesPointer->FindAClass(AttributesPointer, "savedata");
	if(!saveData) return;
	long numG = saveData->GetAttributesNum();
	for(long i = 0; i < numG; i++)
	{
		ATTRIBUTES * grp = saveData->GetAttributeClass(i);
		//Регестрируем первую группу
		const char * gname = grp->GetAttribute("name1");
		if(!gname) gname = "";
		long g1 = RegistryGroup(gname);
		groups[g1]->look = grp->GetAttributeAsFloat("look1", groups[g1]->look);
		groups[g1]->hear = grp->GetAttributeAsFloat("hear1", groups[g1]->hear);
		groups[g1]->say = grp->GetAttributeAsFloat("say1", groups[g1]->say);
		groups[g1]->priority = grp->GetAttributeAsDword("prt1", groups[g1]->priority);
		//Регестрируем вторую группу
		gname = grp->GetAttribute("name2");
		if(!gname) gname = "";
		long g2 = RegistryGroup(gname);
		groups[g2]->look = grp->GetAttributeAsFloat("look2", groups[g2]->look);
		groups[g2]->hear = grp->GetAttributeAsFloat("hear2", groups[g2]->hear);
		groups[g2]->say = grp->GetAttributeAsFloat("say2", groups[g2]->say);
		groups[g2]->priority = grp->GetAttributeAsDword("prt2", groups[g2]->priority);
		//Востановим отношения
		Assert(g1 != g2);
		Relation & r = FindRelation(g1, g2);
		r.alarm = grp->GetAttributeAsFloat("alarm", r.alarm);
		if(r.alarm < 0.0f) r.alarm = 0.0f;
		r.alarmdown = grp->GetAttributeAsFloat("alarmdown", r.alarmdown);
		r.alarmmin = grp->GetAttributeAsFloat("alarmmin", r.alarmmin);
		r.alarmmax = grp->GetAttributeAsFloat("alarmmax", r.alarmmax);
		r.isActive = grp->GetAttributeAsDword("isactive", r.isActive) != 0;
		long curState = grp->GetAttributeAsDword("curState", r.curState);
		long actState = grp->GetAttributeAsDword("actState", r.actState);
		long relState = grp->GetAttributeAsDword("relState", r.relState);
		if(curState <= rs_beginvalue || curState >= rs_endvalue)
		{
			api->Trace("CharactersGroups::LoadDataRelations -> invalide curState value, set this neitral");
			curState = rs_neitral;
		}
		r.curState = RelState(curState);
		if(actState <= rs_beginvalue || actState >= rs_endvalue)
		{
			api->Trace("CharactersGroups::LoadDataRelations -> invalide actState value, set this enemy");
			actState = rs_enemy;
		}
		r.actState = RelState(actState);
		if(relState <= rs_beginvalue || relState >= rs_endvalue)
		{
			api->Trace("CharactersGroups::LoadDataRelations -> invalide relState value, set this neitral");
			relState = rs_neitral;
		}
		r.relState = RelState(relState);
	}
}

//Установить отношения для активных групп
void CharactersGroups::RestoreStates()
{
	for(long i = 0, cnt = 0; i < numGroups; i++)
	{
		for(long j = 0; j < i; j++)
		{
			Relation & r = FindRelation(i, j);
			bool oldState = r.isActive;
			if(r.alarmdown > 0) r.alarm = 0.0f;
			if(r.alarm <= r.alarmmin) r.isActive = false;
			if(r.alarm >= r.alarmmax) r.isActive = true;
			if(oldState != r.isActive)
			{
				if(r.isActive) r.curState = r.actState; else r.curState = r.relState;
			}
		}
	}
	RemoveAllInvalidTargets();
}

//Вывести информацию об отношениях
void CharactersGroups::DumpRelations()
{
	//Сохраняем отношения групп
	for(long i = 0; i < numGroups; i++)
	{
		for(long j = 0; j < i; j++)
		{
			api->Trace("\"%s\" <-> \"%s\"", groups[i]->name.name, groups[j]->name.name);
			//Сохраним отношения
			Relation & r = FindRelation(i, j);
			api->Trace("alarm: %f", r.alarm);
			api->Trace("alarmdown: %f", r.alarmdown);
			api->Trace("alarmmin: %f", r.alarmmin);
			api->Trace("alarmmax: %f", r.alarmmax);
			api->Trace("isActive: %s", r.isActive ? "true" : "false");
			api->Trace("curState: \"%s\"", GetTextState(r.curState));
			api->Trace("actState: \"%s\"", GetTextState(r.actState));
			api->Trace("relState: \"%s\"", GetTextState(r.relState));
			api->Trace("");
		}
	}
	api->Trace("Groups info:");
	api->Trace("");
	for(i = 0; i < numGroups; i++)
	{
		api->Trace("name: \"%s\"", groups[i]->name.name);
		api->Trace("    look: %f", groups[i]->look);
		api->Trace("    hear: %f", groups[i]->hear);
		api->Trace("    say: %f", groups[i]->say);
		api->Trace("");
	}
}

//Получить состояние в виде строки
const char * CharactersGroups::GetTextState(RelState state)
{
	switch(state)
	{
	case rs_friend:
		return "friend";
	case rs_neitral:
		return "neitral";
	case rs_enemy:
		return "enemy";
	};
	return "unknow value";
}