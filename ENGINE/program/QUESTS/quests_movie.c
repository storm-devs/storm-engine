

int questMovieProcess = 0;
object questMovieStack;
object questMovieWideScreen;
bool questMovieIsLockPlayerCtrl = false;
string questMovieOldSaveState = true;



void ResetQuestMovie()
{
	questMovieProcess = 0;
	questMovieIsLockPlayerCtrl = false;
	questMovieOldSaveState = true;
}


bool StartQuestMovie(bool noReload, bool noSave, bool lockPlayerCtrl)
{
	Event("EventStartQuestMovie", "lll", noReload, noSave, lockPlayerCtrl);
	//Stack level
	int prevLevel = questMovieProcess - 1;
	string level = "L" + questMovieProcess;
	questMovieStack.(level) = "";
	aref stk;
	makearef(stk, questMovieStack.(level));
	//Set current parameters
	stk.noReload = noReload;
	stk.noSave = noSave;
	stk.lock = lockPlayerCtrl;
	//Set parameters
	if(questMovieProcess > 0)
	{
		InterfaceStates.Buttons.Save.enable = noSave;
		if(lockPlayerCtrl != false)
		{
			QuestMovieLockPlayer();
		}else{
			QuestMovieUnLockPlayer();
		}
	}else{
		questMovieOldSaveState = InterfaceStates.Buttons.Save.enable;
		InterfaceStates.Buttons.Save.enable = noSave;
		if(lockPlayerCtrl != false) QuestMovieLockPlayer();
	}
	questMovieProcess = questMovieProcess + 1;
	return true;
}

bool EndQuestMovie()
{
	if(questMovieProcess == 0) return false;
	Event("EndStartQuestMovie");
	questMovieProcess = questMovieProcess - 1;
	if(questMovieProcess == 0)
	{
		QuestMovieUnLockPlayer();
		InterfaceStates.Buttons.Save.enable = questMovieOldSaveState;
		return false;
	}
	string level = "L" + questMovieProcess;
	aref stk;
	makearef(stk, questMovieStack.(level));
	InterfaceStates.Buttons.Save.enable = stk.noSave;
	if(sti(stk.lock) != 0)
	{
		QuestMovieLockPlayer();
	}else{
		QuestMovieUnLockPlayer();
	}
	return true;
}

void QuestMovieLockPlayer()
{
	if(questMovieIsLockPlayerCtrl == true) return;
	questMovieIsLockPlayerCtrl = true;
	SetCharacterTask_Stay(GetMainCharacter());
	CreateEntity(&questMovieWideScreen, "WideScreen");
}

void QuestMovieUnLockPlayer()
{
	if(questMovieIsLockPlayerCtrl == false) return;
	questMovieIsLockPlayerCtrl = false;
	SetCharacterTask_None(GetMainCharacter());
	SendMessage(&questMovieWideScreen, "");
}

bool qmIsNoReload()
{
	if(questMovieProcess > 0)
	{
		int l = questMovieProcess - 1;
		string level = "L" + l + ".noReload";
		if(sti(questMovieStack.(level)) != 0) return true;
	}
	return false;
}

