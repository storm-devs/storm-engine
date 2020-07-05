
#define MAX_STORY_EVENTS	100;

object HistoryEvents[MAX_STORY_EVENTS];

void ActivateHistoryEvents()
{
	FillHistoryEvents();
	//RechargeHistoryEvents();
	
	
	string sQuest = "";
	int iday, imonth, iyear;
	for(int i = 0; i<MAX_STORY_EVENTS; i++)
	{
		if (CheckAttribute(&HistoryEvents[i], "text"))
		{
			sQuest = "history_event" + "_" + i;
			iday = sti(HistoryEvents[i].day);
			imonth = sti(HistoryEvents[i].month);
			iyear = sti(HistoryEvents[i].year);
			
			pchar.quest.(sQuest).win_condition.l1 = "timer";
			pchar.quest.(sQuest).win_condition.l1.date.day = iday;
			pchar.quest.(sQuest).win_condition.l1.date.month = imonth;
			pchar.quest.(sQuest).win_condition.l1.date.year = iyear;
			pchar.quest.(sQuest).win_condition.l1.date.hour = rand(23);
			pchar.quest.(sQuest).win_condition = "Historical_event";
			pchar.quest.(sQuest).number = i;
		}
	}
}

void CheckHistoryEvent(string sQuest)
{
	
	
	int iNumber = sti(pchar.quest.(sQuest).number);
	GenerateRumour("history_event", iNumber, 0, 0);
}

void FillHistoryEvents()
{
	int n = 0;
	
	HistoryEvents[n].text = "В Англии при Эджгилле произошла первая битва, положившая начало гражданской войне.";
	HistoryEvents[n].day = 23;
	HistoryEvents[n].month = 10;
	HistoryEvents[n].year = 1642;
	n++;
	
	HistoryEvents[n].text = "Во Франции скончался кардинал Ришелье (Арман Жан дю Плеси).";
	HistoryEvents[n].day = 4;
	HistoryEvents[n].month = 12;
	HistoryEvents[n].year = 1642;
	n++;
	
	HistoryEvents[n].text = "Во Франции скончался король Людовик XIII. Джулио Раймондо Мазарини назначается первым министром и регентом при малолетнем Людовике XIV.";
	HistoryEvents[n].day = 14;
	HistoryEvents[n].month = 5;
	HistoryEvents[n].year = 1643;
	n++;
	
	HistoryEvents[n].text = "В сражении сражении при Несби произошло окончательное поражение армий английского короля Карла I.";
	HistoryEvents[n].day = 14;
	HistoryEvents[n].month = 6;
	HistoryEvents[n].year = 1645;
	n++;
	
	HistoryEvents[n].text = "Войска парламента вступили в Оксфорд и Карл I был вынужден бежать в Шотландию.";
	HistoryEvents[n].day = 23;
	HistoryEvents[n].month = 6;
	HistoryEvents[n].year = 1646;
	n++;
	
	HistoryEvents[n].text = "В сражении близ Престона Оливер Кромвель разбил армию шотландцев и захватил в плен Карла I.";
	HistoryEvents[n].day = 17;
	HistoryEvents[n].month = 8;
	HistoryEvents[n].year = 1648;
	n++;
	
	HistoryEvents[n].text = "Заключение Вестфальского мирного договора, положившего конец Тридцатилетней войне.";
	HistoryEvents[n].day = 24;
	HistoryEvents[n].month = 10;
	HistoryEvents[n].year = 1648;
	n++;
	
	HistoryEvents[n].text = "По приговору суда в Англии был казнен король Карл I. Англия провозглашена республикой.";
	HistoryEvents[n].day = 30;
	HistoryEvents[n].month = 1;
	HistoryEvents[n].year = 1649;
	n++;
	
	HistoryEvents[n].text = "Оливер Кромвель разогнал английский парламент и государственный совет.";
	HistoryEvents[n].day = 20;
	HistoryEvents[n].month = 4;
	HistoryEvents[n].year = 1652;
	n++;
	
	HistoryEvents[n].text = "Англия начала войну с Голландией.";
	HistoryEvents[n].day = 29;
	HistoryEvents[n].month = 5;
	HistoryEvents[n].year = 1652;
	n++;
	
	HistoryEvents[n].text = "Англия подписала мир Голландией.";
	HistoryEvents[n].day = 20;
	HistoryEvents[n].month = 4;
	HistoryEvents[n].year = 1654;
	n++;
	
	HistoryEvents[n].text = "В Вестминстерском дворце состоялась торжественная церемония «введения» Кромвеля в должность Лорда-Протектора.";
	HistoryEvents[n].day = 16;
	HistoryEvents[n].month = 2;
	HistoryEvents[n].year = 1654;
	n++;
	
	HistoryEvents[n].text = "Коронация Людовика XIV.";
	HistoryEvents[n].day = 7;
	HistoryEvents[n].month = 6;
	HistoryEvents[n].year = 1654;
	n++;

	HistoryEvents[n].text = "В результати болезни скончался Оливер Кромвель.";
	HistoryEvents[n].day = 3;
	HistoryEvents[n].month = 9;
	HistoryEvents[n].year = 1658;
	n++;
	
	HistoryEvents[n].text = "На Фазаньем острове на пограничной реке Бидассоа был подписан Пиренейский мир между Францией и Испанией.";
	HistoryEvents[n].day = 7;
	HistoryEvents[n].month = 11;
	HistoryEvents[n].year = 1659;
	n++;
	
	HistoryEvents[n].text = "Генерал Джордж Монк вводит войска в Лондон и провозглашает королем Карла II.";
	HistoryEvents[n].day = 29;
	HistoryEvents[n].month = 5;
	HistoryEvents[n].year = 1660;
	n++;
	
	HistoryEvents[n].text = "В Фуэнтерабиа Людовик XIV сочитался браком с испанской инфантой Марией Терезией.";
	HistoryEvents[n].day = 3;
	HistoryEvents[n].month = 6;
	HistoryEvents[n].year = 1660;
	n++;
	
	HistoryEvents[n].text = "Во Франции скончался кардинал Джулио Раймондо Мазарини. Людовик XIV берет бразды правления в свои руки.";
	HistoryEvents[n].day = 9;
	HistoryEvents[n].month = 3;
	HistoryEvents[n].year = 1661;
	n++;
	
	HistoryEvents[n].text = "Бубновая чума пришла в Лондон.";
	HistoryEvents[n].day = 10;
	HistoryEvents[n].month = 4;
	HistoryEvents[n].year = 1665;
	n++;
	
	HistoryEvents[n].text = "Карл II становится королем Испании.";
	HistoryEvents[n].day = 1;
	HistoryEvents[n].month = 1;
	HistoryEvents[n].year = 1665;
	n++;
	
} 