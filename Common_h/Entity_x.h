#ifndef _ENTITY_X_H_
#define _ENTITY_X_H_

#include "entity.h"
#include "vmodule_api.h"

class ENTITY_X : public ENTITY 
{
	VAPI * api;
public:
	ENTITY_X(){api = (VAPI *)_VSYSTEM_API->GetCoreApi();};
	virtual ~ENTITY_X() {};
	
	void SystemMessages(ENTITY_ID eid, bool on)	{api->SystemMessages(eid,on);}
	void Exit() {api->Exit();};
	void Reset() {api->Reset();};
	HWND GetAppHWND() {return api->GetAppHWND();};
	void * GetUserData(long * data_size) {return api->GetUserData(data_size);};
	void * SetUserData(void * ud_PTR,long data_size) {return api->SetUserData(ud_PTR,data_size);};
	void  SetTimeScale(float _scale) {api->SetTimeScale( _scale);};
	float GetTimeScale() {return api->GetTimeScale();};
	dword SetExceptions(dword _flags) {return api->SetExceptions(_flags);};
	dword ClrExceptions(dword _flags){return api->ClrExceptions(_flags);};
	void  EngineDisplay(bool on) {api->EngineDisplay(on);};

	dword Class_Name2Code(char * class_name) {return api->Class_Name2Code(class_name);};
	bool FindClass(ENTITY_ID * id_PTR, char * class_name, dword class_code)
		{return api->FindClass(id_PTR,class_name,class_code);};
	bool FindClassNext(ENTITY_ID * id_PTR) {return api->FindClassNext(id_PTR);};
	void * CreateService(char * service_name) { return api->CreateService(service_name);};
	void FreeService(char * service_name){api->FreeService(service_name);};
	bool CompareID(ENTITY_ID * ida_PTR,ENTITY_ID * idb_PTR){return api->CompareID(ida_PTR,idb_PTR);};
	bool ValidateEntity(ENTITY_ID * id_PTR) {return api->ValidateEntity(id_PTR);};
	bool CreateEntity(ENTITY_ID * id_PTR, char * class_name){return api->CreateEntity(id_PTR,class_name);};
	bool DeleteEntity(ENTITY_ID entity_id){return api->DeleteEntity(entity_id);};
	ENTITY * GetEntityPointer(ENTITY_ID * id_PTR){return api->GetEntityPointer(id_PTR);};
	bool GetEntity(ENTITY_ID * id_PTR){return api->GetEntity(id_PTR);};
	bool GetEntityNext(ENTITY_ID * id_PTR){return api->GetEntityNext(id_PTR);};
	bool SetEntityScanLayer(char * layer_name){return api->SetEntityScanLayer(layer_name);};

	
	//dword _cdecl Send_Message(ENTITY_ID Destination,char * Format,...)= 0;
	
	// layer managment 
	
	bool LayerCreate(char * layer_name, bool ordered, bool fail_if_exist){return api->LayerCreate(layer_name,ordered,fail_if_exist);};
	void LayerDelete(char * layer_name){api->LayerDelete(layer_name);};
	void LayerSetFlags(char * layer_name, dword flags){api->LayerSetFlags(layer_name,flags);};
	void LayerClrFlags(char * layer_name, dword flags){api->LayerClrFlags(layer_name,flags);};
	bool LayerAdd(char * layer_name, ENTITY_ID eid, dword priority){return api->LayerAdd(layer_name,eid,priority);};
	void LayerDel(char * layer_name, ENTITY_ID eid) {api->LayerDel(layer_name,eid);};
	bool LayerDeleteContent(char * layer_name){return api->LayerDeleteContent(layer_name);};
	void LayerSetSleep(char * layer_name,dword sleep_time_ms){api->LayerSetSleep(layer_name,sleep_time_ms);};
	dword LayerGetFlags(char * layer_name){return api->LayerGetFlags(layer_name);};
	void LayerSetExecute(char * layer_name, bool on){api->LayerSetExecute(layer_name,on);};
	void LayerSetRealize(char * layer_name, bool on){api->LayerSetRealize(layer_name,on);};

	
	bool SaveState(char * file_name){return api->SaveState(file_name);};
	bool InitiateStateLoading(char * file_name){return api->InitiateStateLoading(file_name);};

	dword EngineFps(){return api->EngineFps();};
	dword SetTimer(dword elapse,ENTITY_ID eid){return api->SetTimer(elapse,eid);};
	void SetDeltaTime(long delta_time){api->SetDeltaTime(delta_time);};

};

#endif