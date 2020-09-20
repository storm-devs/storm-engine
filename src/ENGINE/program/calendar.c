//--------------------------------------------------------------------
// Environment section
//--------------------------------------------------------------------
#define DAY_TIME_NIGHT		"Night"
#define DAY_TIME_MORNING	"Morning"
#define DAY_TIME_DAY		"Day"
#define DAY_TIME_EVENING	"Evening"

float  GetTime(){ return stf(Environment.time); }
float  GetHour(){ return stf(Environment.date.hour); }
float  GetMinute(){ return stf(Environment.date.min); }
float  GetSecond(){ return stf(Environment.date.sec); }
int	GetDataYear(){ return sti(Environment.date.year); }
int	GetDataMonth(){ return sti(Environment.date.month); }
int	GetDataDay(){ return sti(Environment.date.day); }

void SetDayTime(string name)
{
	switch(name)
	{
	case DAY_TIME_NIGHT:	SetCurrentTime(0,1);	break;
	case DAY_TIME_MORNING:	SetCurrentTime(6,1);	break;
	case DAY_TIME_DAY:		SetCurrentTime(11,1);	break;
	case DAY_TIME_EVENING:	SetCurrentTime(19,1);	break;
	}
}

string GetDayTime()
{
	if(GetTime()<6.0) return DAY_TIME_NIGHT;
	if(GetTime()<11.0) return DAY_TIME_MORNING;
	if(GetTime()<19.0) return DAY_TIME_DAY;
	if(GetTime()<24.0) return DAY_TIME_EVENING;
	return DAY_TIME_NIGHT;
}
bool IsDay()
{
	if(GetTime() > 6.0)	{if(GetTime() < 23.0) return true; } return false;
}

bool IsLoginTime()
{
	if(GetTime() > 6.0)	{if(GetTime() < 21.99) return true; } return false;
}

int GetMonthDays(int month)
{
	while(month>12) {month -= 12;}
	switch(month)
	{
	case 1:		return 31;	break;
	case 2:		return 28;	break;
	case 3:		return 31;	break;
	case 4:		return 30;	break;
	case 5:		return 31;	break;
	case 6:		return 30;	break;
	case 7:		return 31;	break;
	case 8:		return 31;	break;
	case 9:		return 30;	break;
	case 10:	return 31;	break;
	case 11:	return 30;	break;
	case 12:	return 31;	break;
	}
	Trace("Error: " + month + " month is not relevant");
	return 30;
}
int GetAddingDataYear(int addYear,int addMonth,int addDay)
{
	int nextDay = GetDataDay()+addDay;
	int curMonth = GetDataMonth();
	while(GetMonthDays(curMonth)<nextDay)
	{
		nextDay = nextDay - GetMonthDays(curMonth);
		curMonth++;
	}
	int nextMonth = curMonth+addMonth;
	int nextYear = GetDataYear()+addYear;
	while(12<nextMonth)
	{
		nextMonth = nextMonth - 12;
		nextYear++;
	}
	return nextYear;
}
int GetAddingDataMonth(int addYear,int addMonth,int addDay)
{
	int nextDay = GetDataDay()+addDay;
	int curMonth = GetDataMonth();
	while(GetMonthDays(curMonth)<nextDay)
	{
		nextDay = nextDay - GetMonthDays(curMonth);
		curMonth++;
	}
	int nextMonth = curMonth+addMonth;
	while(12<nextMonth)
	{
		nextMonth = nextMonth - 12;
	}
	return nextMonth;
}
int GetAddingDataDay(int addYear,int addMonth,int addDay)
{
	int nextDay = GetDataDay()+addDay;
	int curMonth = GetDataMonth();
	while(GetMonthDays(curMonth)<nextDay)
	{
		nextDay = nextDay - GetMonthDays(curMonth);
		curMonth++;
	}
	return nextDay;
}
int GetAddingTimeDay(int hour,int minute)
{
	float curtime = GetTime();
	float nexttime = curtime+hour+(makefloat(minute)/60.0);
	int addingDays = 0;
	while(nexttime>=24.0)
	{
		nexttime = nexttime - 24.0;
		addingDays++;
	}
	return addingDays;
}
int AddTimeToCurrent(int hour,int minute)
{
	float curtime = GetTime();
	float nexttime = curtime+hour+(makefloat(minute)/60.0);
	int addingDays = 0;
	while(nexttime>=24.0)
	{
		nexttime = nexttime - 24.0;
		addingDays++;
	}
	AddDataToCurrent(0,0,addingDays);
    /*if(addingDays>0) PostEvent("NextDay",0); Нах.. оно нужно не знаю, если до этого вызывается метод
                              AddDataToCurrent() с соответствующим вызовом события. navy. */
	Environment.time = nexttime;
	Environment.date.hour = makeint(nexttime);
	worldMap.date.hour = makeint(nexttime);
	nexttime = (nexttime - stf(Environment.date.hour))*60.0;
	Environment.date.min = makeint(nexttime);
	worldMap.date.min = makeint(nexttime);
    Weather.Time.time = GetTime(); // новая погода
    
	return addingDays; // boal верунть число дней, чтоб понять, что новый наступил
}
void AddDataToCurrent(int addYear,int addMonth,int addDay)
{
	int nextDay = GetDataDay()+addDay;
	int curMonth = GetDataMonth();
	while(GetMonthDays(curMonth)<nextDay)
	{
		nextDay = nextDay - GetMonthDays(curMonth);
		curMonth++;
	}
	int nextMonth = curMonth+addMonth;
	int nextYear = GetDataYear()+addYear;
	while(12<nextMonth)
	{
		nextMonth = nextMonth - 12;
		nextYear++;
	}
	Environment.date.year = nextYear;
	Environment.date.month = nextMonth;
	Environment.date.day = nextDay;
	worldMap.date.year = nextYear;
	worldMap.date.month = nextMonth;
	worldMap.date.day = nextDay;
	if(addYear!=0 || addMonth!=0 || addDay!=0)	PostEvent("NextDay",500); // fix boal Отложить время, чтоб успеть выйти из форм
	//  for (int i=0; i<addDay; i++) PostEvent("NextDay",0);
}

// "year","month","day","hour","minute"
int GetPastTime(string timeUnit,
				int pastYear,int pastMonth,int pastDay, float pastTime,
				int currentYear,int currentMonth,int currentDay, float currentTime)
{
	float dtime = currentTime - pastTime;
	int dyear = currentYear - pastYear;
	int dmonth = currentMonth - pastMonth;
	int dday = currentDay - pastDay;

	if(dtime<0.0)
	{
		dday--;
		dtime = dtime + 24.0;
	}
	if(dday<0)
	{
		dmonth--;
		if(pastMonth>1) dday = dday + GetMonthDays(pastMonth-1);
		else dday = dday + GetMonthDays(12);
	}
	if(dmonth<0)
	{
		dyear--;
		dmonth = dmonth+12;
	}

	if(dyear<0) return 0;
	if(timeUnit=="year") return dyear;

	if(timeUnit=="month")
	{
		return dmonth+dyear*12;
	}
	
	int dayCount = dday + dyear*365;
	for(int i=0; i<dmonth; i++)
	{
		dayCount = dayCount + GetMonthDays(pastMonth);
		pastMonth++;
		if(pastMonth>12) pastMonth = 1;
	}
	//if(timeUnit=="day")	return dayCount;

	if(timeUnit=="hour") return dayCount*24 + makeint(dtime);
	if(timeUnit=="minute") return dayCount*1440 + makeint(dtime*60.0);
	return dayCount;
}

void SetCurrentTime(int hour, int minutes)
{
	Environment.date.hour = makefloat(hour);
	Environment.date.min = makefloat(minutes);
	Environment.time = makefloat(hour) + makefloat(minutes)/60.0;
	worldMap.date.hour = makefloat(hour);
	worldMap.date.min = makefloat(minutes);
}
// boal этот метод дублирует  GetPastTime и нигде е используется
/*
int GetFuterTime(string timeUnit,
				int pastYear,int pastMonth,int pastDay, float pastTime,
				int currentYear,int currentMonth,int currentDay, float currentTime)
{
	float dtime = pastTime - currentTime;
	int dyear = pastYear - currentYear;
	int dmonth = pastMonth - currentMonth;
	int dday = pastDay - currentDay;

	if(dtime<0.0)
	{
		dday--;
		dtime = dtime + 24.0;
	}
	if(dday<0)
	{
		dmonth--;
		if(pastMonth>1) dday = dday + GetMonthDays(pastMonth-1);
		else dday = dday + GetMonthDays(12);
	}
	if(dmonth<0)
	{
		dyear--;
		dmonth = dmonth+12;
	}

	if(dyear<0) return 0;
	if(timeUnit=="year") return dyear;

	if(timeUnit=="month")
	{
		return dmonth+dyear*12;
	}
	
	int dayCount = dday + dyear*365;
	for(int i=0; i<dmonth; i++)
	{
		dayCount = dayCount + GetMonthDays(pastMonth);
		pastMonth++;
		if(pastMonth>12) pastMonth = 1;
	}
	//if(timeUnit=="day")	return dayCount;

	if(timeUnit=="hour") return dayCount*24 + makeint(dtime);
	if(timeUnit=="minute") return dayCount*1440 + makeint(dtime*60.0);
	return dayCount;
} */