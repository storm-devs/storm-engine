#ifndef _BI_ITEM_ENTITY_H_
#define _BI_ITEM_ENTITY_H_

#include "..\..\common_h\dx8render.h"
#include "..\..\common_h\defines.h"
#include "..\..\common_h\templates\string.h"
#include "..\..\common_h\animation.h"
#include "..\..\common_h\vparticle_system.h"

class MODEL;
class NODE;

class ItemEntity : public ENTITY
{
public:
	ItemEntity();
	~ItemEntity();

	bool Init();
	void Realize(dword delta_time);
	dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	bool ReadAndCreate();
	void SetBeginData();
	void Release();

	void SetModelToPosition(const CMatrix& mtx);
	void SetTechnique( const char* pcTechnique );
	bool TieToLocator(ENTITY_ID& mdlEID, const char* pcLocName);
	void UnTieFromLocator();
	void EndEventProcess();

	void DrawIntoLocator();

	void SetEventListener(ENTITY_ID& mdlEID, ENTITY_ID& mdlToTieEID,const char* pcLocName, const char* pcStartEvent,const char* pcEndEvent);
	ENTITY_ID GetModelEIDFromCharacterEID(ENTITY_ID& chrEID);

protected: // data
	bool m_bVisible;
	ENTITY_ID m_eidModel;
	MODEL* m_pModel;
	CMatrix m_mtxpos;

	bool m_bTieToLocator;
	ENTITY_ID m_eidTieModel;
	NODE* m_pMdlNode;
	string m_sTieLocName;

	class EventListener : public AnimationEventListener
	{
	public:
		//Принять событие
		virtual void Event(Animation * animation, long playerIndex, const char * eventName);

		ItemEntity* item;
		string m_sStartEvent;
		string m_sEndEvent;
		bool m_bStartWaiting;
		ENTITY_ID m_eidListenedModel;
		ENTITY_ID m_eidToTieModel;
		string m_sToTieLocator;
	};
	friend EventListener;
	EventListener m_eventListener;

	VPARTICLE_SYSTEM* m_pParticle;
	bool CreateParticle();
	void DeleteParticle();
};

#endif
