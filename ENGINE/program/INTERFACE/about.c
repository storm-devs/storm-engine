// Авторы, форма про нас :)
// boal ролики делать не умею, зато умею кодить на скриптах, титры на них ;)
float fNewPos;
#define MAX_LINE         143

string STR_TEXT[MAX_LINE] = {
"",
"",
"",
"",
"Создатели игры",
"",
"",
"",
"",
"",
"",
"",
"Студия 'Seaward.Ru'",
"",
"Менеджер проекта",
"Эдуард (Eddy) Зайцев",
"",
"Ведущий гейм-дизайнер",
"Алексей (ALexusB) Бобровников",
"",
"Арт-директор",
"Юрий (Ursus) Рогач",
"",
"Художники",
"Игорь (incus) Госманн",
"Петр (free_corsair) Доценко",
"Светлана (Dogeva) Терещенко",
"",
"Арт заставок",
"Игорь (incus) Госманн",
"",
"3D-моделирование",
"Юрий (Ursus) Рогач",
"Эдуард (Eddy) Зайцев",
"Алексей (lexa) Чернышев",
"",
//
"Обработка локаций: освещение",
"Андрей (Delamer) Ларионов",
"",
"Обработка локаций: пена у берега",
"Алексей (LexaLloyd) Кучеренко",
"Павел (Pavka) Карпов",
"Дмитрий (SeaLegend) Щегольков",
"",
//
"Сюжет и концепция игры",
"Алексей (ALexusB) Бобровников",
"Эдуард (Eddy) Зайцев",
"",
"Дизайн интерфейсов",
"Алексей (ALexusB) Бобровников",
"",
"Дизайн ролевой системы",
"Алексей (ALexusB) Бобровников",
"",
"Идея и сценарий национальных квестов",
"Эдуард (Eddy) Зайцев",
"",
"Тексты диалогов",
"Эдуард (Eddy) Зайцев",
"Алексей (ALexusB) Бобровников",
"",
"Программирование",
"Алексей (ALexusB) Бобровников",
"Эдуард (Eddy) Зайцев",
"Алексей (Navy) Нуреев",
"Михаил (homosapienz) Волков",
"",
"Sound Engine",
"'FMOD Ex Sound System' (Firelight Technologies)",
"",
"Обработка звука и видео",
"Алексей (Navy) Нуреев",
"Игорь (incus) Госманн",
"",
"Тестирование",
"Андрей (Delamer) Ларионов",
"Дмитрий (Танат) Иваненко",
"Михаил (homosapienz) Волков",
"Сергей (Dir) Цыпин",
"Алексей (LexaLloyd) Кучеренко",
"Вячеслав (Strelok) Смирнов",
"Александр (Alex_S) Сергеев",
"Станислав (Kaly) Майоров",
"",
"Бета-тест",
"Николай (dark toster) Волокитин",
"Сергей (RIZIY) Майоров",
"Артем (МОлЬ) Родин",
"Сергей (Jacques) Лобанов",
"Василий (Forsayte) Шишов",
"Павел (Destynar) Хворостюк",
"Павел (Pavka) Карпов",
"Евгений (Изгой) Иванов",
"Дмитрий (SeaLegend) Щегольков",
//"Вадим (cRst) Еськов",
"",
"Руководство по игре",
"Алексей (ALexusB) Бобровников",
"",
"Студия 'Seaward.Ru' благодарит",
"Дмитрия Архипова и Петра Виниченко",
"за предоставленную возможность создать игру",
"",
"",
"",
"Компания 'Акелла'",
"",
"Вице-президент по разработке",
"Дмитрий Архипов",
"",
"Начальник отдела внешних проектов",
"Сергей Белистов",
"",
"Продюсер",
"Петр Виниченко",
"",
"Руководитель отдела тестирования",
"Денис Епифанов",
"",
"PR менеджеры",
"Петр Головин",
"Ирина Семенова",
"Ольга Пак",
"", //
"",
"",
"Студия 'Корсары'",
"",
"Руководитель студии",
"Артем (Gray Angel) Романко",
"",
"Дизайнер",
"Дмитрий (Lokk) Баранов",
//"",
"",
"Арт",
"Юрий (Ursus) Рогач",
"Елена Саблина",
"",
"Программисты",
"Никита (Nike) Курносов",
"Иван (Vano) Щеблыкин",
"",
"",
"",
};

string STR_COLOR[MAX_LINE] = {
"",
"",
"",
"",
"blue",
"",
"",
"",
"",
"",
"",
"",
"red",
"",
"yellow",
"Эдуард (Eddy) Зайцев",
"",
"yellow",
"Алексей (ALexusB) Бобровников",
"",
"yellow",
"Юрий (Ursus) Рогач",
"",
"yellow",
"Игорь (incus) Госманн",
"Петр (free_corsair) Доценко",
"Светлана (Dogeva) Терещенко",
"",
"yellow",
"Игорь (incus) Госманн",
"",
"yellow",
"Юрий (Ursus) Рогач",
"Алексей (lexa) Чернышев",
"Эдуард (Eddy) Зайцев",
"",
"yellow",
"Андрей (Delamer) Ларионов",
"",
"yellow",
"Алексей (LexaLloyd) Кучеренко",
"Павел (Pavka) Карпов",
"Дмитрий (SeaLegend) Щегольков",
"",
"yellow",
"Эдуард (Eddy) Зайцев",
"Алексей (ALexusB) Бобровников",
"",
"yellow",
"Алексей (ALexusB) Бобровников",
"",
"yellow",
"Алексей (ALexusB) Бобровников",
"",
"yellow",
"Эдуард (Eddy) Зайцев",
"",
"yellow",
"Эдуард (Eddy) Зайцев",
"Алексей (ALexusB) Бобровников",
"",
"yellow",
"Алексей (ALexusB) Бобровников",
"Эдуард (Eddy) Зайцев",
"Алексей (Navy) Нуреев",
"Михаил (homosapienz) Волков",
"",
"yellow",
"'FMOD Ex Sound System' Firelight Technologies",
"",
"yellow",
"Алексей (Navy) Нуреев",
"Игорь (incus) Госманн",
"",
"yellow",
"Андрей (Delamer) Ларионов",
"Дмитрий (Танат) Иваненко",
"Михаил (homosapienz) Волков",
"Сергей (Dir) Цыпин",
"Алексей (LexaLloyd) Кучеренко",
"Вячеслав (Strelok) Смирнов",
"Александр (Alex_S) Сергеев",
"Станислав (Kaly) Майоров",
"",
"yellow",
"Николай (dark toster) Волокитин",
"Сергей (RIZIY) Майоров",
"Артем (МОлЬ) Родин",
"Сергей (Jacques) Лобанов",
"Василий (Forsayte) Шишов",
"Павел (Destynar) Хворостюк",
"Павел (Pavka) Карпов",
"Евгений (Изгой) Иванов",
"Дмитрий (SeaLegend) Щегольков",
//"Вадим (cRst) Еськов",
"",
"yellow",
"Алексей (ALexusB) Бобровников",
"",
"blue",
"blue",
"blue",
"",
"",
"",
"red",
"",
"yellow",
"Дмитрий Архипов",
"",
"yellow",
"Сергей Белистов",
"",
"yellow",
"Петр Виниченко",
"",
"yellow",
"Денис Епифанов",
"",
"yellow",
"Петр Головин",
"Ирина Семенова",
"Ольга Пак",
"", //
"",
"",
"red",
"",
"yellow",
"Артем (Gray Angel) Романко",
"",
"yellow",
"Дмитрий (Lokk) Баранов",
//"",
"",
"yellow",
"Юрий (Ursus) Рогач",
"Елена Саблина",
"",
"yellow",
"Никита (Nike) Курносов",
"Иван (Vano) Щеблыкин",
"",
"",
"",
};


void InitInterface(string iniName)
{
	GameInterface.title = "";
	
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessCancelExit",0);
	SetEventHandler("exitCancel","ProcessCancelExit",0);
	SetEventHandler("My_MoveText","MoveText",0);

	SetFormatedText("INFO_TEXT", "");
	int i, k;
	for (i = 0; i < MAX_LINE; i++)
	{
		SendMessage( &GameInterface,"lsle",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT", 0, STR_TEXT[i] + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	}
	for (i = 0; i < MAX_LINE; i++)
	{
		if (STR_COLOR[i] != "")
		{
			switch (STR_COLOR[i])
			{
				case "yellow":
					SendMessage( &GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT", 8, i, argb(255,251,237,68));
				break;
				case "red":
					SendMessage( &GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT", 8, i, argb(255,255,162,162));
				break;
				case "blue":
					SendMessage( &GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT", 8, i, argb(255,0,255,255));
				break;
			}
		}
	}

	PostEvent("My_MoveText", 800);
	fNewPos = 0;
}

void ProcessCancelExit()
{
    if( CheckAttribute(&InterfaceStates,"showGameMenuOnExit") && sti(InterfaceStates.showGameMenuOnExit) == true) {
		IDoExit(RC_INTERFACE_LAUNCH_GAMEMENU);
		return;
	}

	IDoExit(RC_INTERFACE_OPTIONSCREEN_EXIT);
	if( !CheckAttribute(&InterfaceStates,"InstantExit") || sti(InterfaceStates.InstantExit)==false ) {
		ReturnToMainMenu();
	}
}

void IDoExit(int exitCode)
{
 	DelEventHandler("InterfaceBreak","ProcessCancelExit");
	DelEventHandler("exitCancel","ProcessCancelExit");
    DelEventHandler("My_MoveText","MoveText");
    
	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"InstantExit") && sti(InterfaceStates.InstantExit)==true ) {
		EndCancelInterface(true);
	} else {
		EndCancelInterface(false);
	}
}

void MoveText()
{
    if (fNewPos >= 1)
	{
	    ProcessCancelExit();
	}
	else
	{
		SendMessage(&GameInterface,"lslf",MSG_INTERFACE_MSG_TO_NODE,"INFO_TEXT",2, fNewPos);
		PostEvent("My_MoveText", 20);
		fNewPos = fNewPos + 0.0002;
	}
}