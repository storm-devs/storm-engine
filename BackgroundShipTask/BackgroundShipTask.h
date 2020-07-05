#ifndef BACKGROUNDSHIPTASK_H
#define BACKGROUNDSHIPTASK_H

#include "backgrounddate.h"

#include "..\common_h\dx8render.h"
#include "..\common_h\defines.h"

class BackgroundShipTask : public ENTITY
{
public:
	enum BackgroundTask
	{
		BTask_Raze,
		BTask_FortSiege
	};

	BackgroundShipTask();
	~BackgroundShipTask();
	bool Init();
	void Execute(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);
	dword AttributeChanged(ATTRIBUTES * pAttr);

protected:
	void Update();
	void UpdateCurrentDate();
	bool CheckBattleResult();
	void ResetTask();
	void SetStartTiming( long nDayQuantity );
	void SetNextCheckingTime();
	void WriteDataToAttribute();
	void ReadParametersFromAttributes();

	//data
	ATTRIBUTES* m_pDateAttribute;
	BackgroundDataDescriber m_StartDate;
	BackgroundDataDescriber m_EndDate;
	BackgroundDataDescriber m_RecalculateDate;
	BackgroundDataDescriber m_CurrentDate;

	BackgroundTask m_TaskType;

	long m_nCharacterIndex;
	bool m_bTaskPresent;
	bool m_bGoToNextData;
};

#endif