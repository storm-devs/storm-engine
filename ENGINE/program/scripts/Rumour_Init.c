#define MAX_RUMOURS 30  // длина стека слухов
#include "scripts\Rumour_templates.c"
#include "scripts\Rumour_func.c"
#include "scripts\Rumour_generate.c"
#include "scripts\Rumour_Data.h"
#include "scripts\GoldFleet.c"

object	Rumour[MAX_RUMOURS]; // стек слухов
object RumourParams;         // дерево переменных для шаблонных слухов

int id_counter;




void SetRumourDefaults()// заполняем стек начальными - простыми слухами
{
	int i;
	ref CurrentRumour;
	id_counter = 0;

	for(i = 0; i < MAX_RUMOURS; i++)
	{
		makeref(CurrentRumour, Rumour[i]);
		CurrentRumour.id = id_counter;  // ID слуха
		CurrentRumour.text = DEFAULT_TEXT[i];// Подгружаем начальные слухи в очередь
		CurrentRumour.state = "1"; // Сколько раз может повторять ся один и тот же слух
		CurrentRumour.tip = "citizen,capitan"; // типаж
		CurrentRumour.rep = "none" // репутация
		CurrentRumour.starttime = (DateToInt(0));// через сколько дней активировать слух
		CurrentRumour.actualtime = (DateToInt(40));// Как долго могут ходить слухи = 40 дней
		CurrentRumour.event = "none"; // Действие - нет
		CurrentRumour.next = "none"; // Продолжение - нет
		id_counter++; // счетчик слухов
	}
	for(i = 0; i < 9; i++)
	{
		makeref(CurrentRumour, Rumour[i]);
		CurrentRumour.id = id_counter;  // ID слуха
		CurrentRumour.text = POOR_RUMOUR_TEXT[i];// Подгружаем начальные слухи в очередь
		CurrentRumour.state = "5"; // Сколько раз может повторять ся один и тот же слух
		CurrentRumour.tip = "poor"; // типаж
		CurrentRumour.rep = "none" // репутация
		CurrentRumour.starttime = (DateToInt(0));// через сколько дней активировать слух
		CurrentRumour.actualtime = (DateToInt(100));// Как долго могут ходить слухи = 40 дней
		CurrentRumour.event = "none"; // Действие - нет
		CurrentRumour.next = "none"; // Продолжение - нет
		id_counter++; // счетчик слухов
	}
}

void RumourInit();
{
	//Log_SetStringToLog("Слухи!");
	trace("Слухи!");
	TemplateLoad();
	SetRumourDefaults();
	// ==> отметка времени для осад homo 05/11/06
	SaveCurrentQuestDateParam("Siege_DailyUpdate");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////