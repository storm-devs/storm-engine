#include "layer_service.h"

extern dword Exceptions_Mask;

LAYER_SERVICE::LAYER_SERVICE()
{
	ToClean = false;
}

// Return index (unical code) of layer by name
dword LAYER_SERVICE::GetIndex(char * layer_name)
{
	GUARD(LAYER_SERVICE::GetIndex)
	dword n;
	if(layer_name == null) return INVALID_LAYER_CODE;
	for(n=0;n<=lss.Layer_max_index;n++)
	{
		if(Layer_Table[n] == null) continue;
		if(Layer_Table[n]->ls.Deleted) continue;
		if(stricmp(layer_name,Layer_Table[n]->Name)== 0) return n;
	}
	UNGUARD
	return INVALID_LAYER_CODE;
}

// check layer for presence
bool LAYER_SERVICE::Verify(char * layer_name)
{
	GUARD(LAYER_SERVICE::Verify)
	dword n;
	if(layer_name == null) return false;
	for(n=0;n<=lss.Layer_max_index;n++)
	{
		if(Layer_Table[n] == null) continue;
		if(stricmp(layer_name,Layer_Table[n]->Name)== 0) return true;
	}
	UNGUARD
	return false;
}

void LAYER_SERVICE::Fit(dword index, char * layer_name, LAYER_STATE ls)
{
	GUARD(LAYER_SERVICE::FitLayer)
	if(layer_name == null) _THROW(zero name);
	if(index > lss.Layer_max_index) _THROW(invalid index);
	if(Layer_Table[index] != null) THROW;

	Layer_Table[index] = NEW LAYER(layer_name,ls.Ordered,ls.System,ls.System_flags);
	if(Layer_Table[index] == null) THROW;
	Layer_Table[index]->ls = ls;
	UNGUARD
}

bool LAYER_SERVICE::Create(char * layer_name, bool ordered, bool fail_if_exist)
{
	GUARD(LAYER_SERVICE::Create)
	dword n;
	if(layer_name == null) return false;
	for(n=0;n<_MAX_LAYERS;n++)
	{
		if(Layer_Table[n] != null) 
		{
			if(Layer_Table[n]->ls.Deleted) continue;
			// name must be unical
			if(stricmp(layer_name,Layer_Table[n]->Name)== 0) 
			{
				if(fail_if_exist)
				{
					CheckAutoExceptions();
					return false;
				}
				return true;
			}
			continue;
		}

		Layer_Table[n] = NEW LAYER(layer_name,ordered,0,0);
		if(Layer_Table[n] == null) THROW;
		if(lss.Layer_max_index < n) lss.Layer_max_index = n;
		lss.Layers_number++;
		return true;
	}
	CheckAutoExceptions();
	UNGUARD
	return false;
}

void LAYER_SERVICE::Erase(dword index)
{
	GUARD(LAYER_SERVICE::EraseLayer)
	LAYER * l_PTR;

	if(index < 0 || index > lss.Layer_max_index) {__THROW(NON_FATAL,ghost layer);}
	l_PTR = Layer_Table[index]; if(l_PTR == null) {__THROW(NON_FATAL,ghost layer);}

/*  transfer this block to core
	if(Scan_Layer_Code == index) Scan_Layer_Code = INVALID_LAYER_CODE;
	// clear layer flag for objects
	eid_PTR = l_PTR->GetID();
	while(eid_PTR)
	{
		if(ValidateEntity(eid_PTR)) Atoms_PTR[eid_PTR->atom_position]->ClrLayerAttribute(index);
		eid_PTR = l_PTR->GetNextID();
	}
*/
	
	// delete layer object
	delete Layer_Table[index];
	Layer_Table[index] = null;
	if(lss.Layers_number > 0) lss.Layers_number--; else THROW;
	if(index == lss.Layer_max_index)
	for(index = lss.Layer_max_index;index>0;index--)
	{
		if(Layer_Table[index] != null)
		{
			lss.Layer_max_index = index;
			return;
		}
	}
	UNGUARD
}

// Mark layer for deletion before next game loop
void LAYER_SERVICE::Delete(char * layer_name)
{
	GUARD(LAYER_SERVICE::Delete)
	dword index;
	index = GetIndex(layer_name);	
	if(index == INVALID_LAYER_CODE)
	{
		__THROW(NON_FATAL,attempt to delete non existing layer);
	}
	Layer_Table[index]->ls.Deleted = true;
	ToClean = true;
	if(lss.Scan_layer_code == index) lss.Scan_layer_code = INVALID_LAYER_CODE;
	UNGUARD
}

void LAYER_SERVICE::SetFlags(char * layer_name, dword flags)
{
	GUARD(LAYER_SERVICE::SetFlags)
	dword index;
	index = GetIndex(layer_name); if(index == INVALID_LAYER_CODE) return;
	Layer_Table[index]->SetFlag(flags);
	UNGUARD
}

void LAYER_SERVICE::ClrFlags(char * layer_name, dword flags)
{
	GUARD(LAYER_SERVICE::ClrFlags)
	dword index;
	index = GetIndex(layer_name); if(index == INVALID_LAYER_CODE) return;
	Layer_Table[index]->ClrFlag(flags);
	UNGUARD
}

dword LAYER_SERVICE::GetFlags(char * layer_name)
{
	dword index;
	dword flags;
	GUARD(LAYER_SERVICE::GetFlags)
	index = GetIndex(layer_name); if(index == INVALID_LAYER_CODE) return 0;
	flags = Layer_Table[index]->ls.Flags;
	UNGUARD
	return flags;
}


bool LAYER_SERVICE::Add(char * layer_name, ENTITY_ID eid, dword priority)
{
	GUARD(LAYER_SERVICE::Add)
	dword index;
	index = GetIndex(layer_name);	if(index == INVALID_LAYER_CODE) return false;

/*	TC
	if(!ValidateEntity(&eid)) return false;
	// if object already in layer, just return 
	if(Atoms_PTR[eid.atom_position]->TstLayerAttribute(index)) return true;
	// mark object belonged to layer
	Atoms_PTR[eid.atom_position]->SetLayerAttribute(index);
*/
	// add object to layer list
	if(!Layer_Table[index]->Add(eid,priority)) THROW;
	
	UNGUARD
	return true;
}

void LAYER_SERVICE::Del(char * layer_name, ENTITY_ID eid)
{
	GUARD(LAYER_SERVICE::LayerDel)
	dword index;
	index = GetIndex(layer_name); if(index == INVALID_LAYER_CODE) return;
	Layer_Table[index]->Del(eid);
	
	/* unmark object, T to core
	if(!ValidateEntity(&eid)) return;
	Atoms_PTR[eid.atom_position]->ClrLayerAttribute(index);*/
	UNGUARD
}

// set layer sleeping time, layer will skip execution till this time
void LAYER_SERVICE::SetSleep(char * layer_name,dword sleep_time_ms)
{

}

void LAYER_SERVICE::CheckAutoExceptions()
{
	if(!(Exceptions_Mask & _X_NO_LAYER)) _THROW(no layer);
}

// delete layers, previously marked as deleted
void LAYER_SERVICE::Clean()
{
	GUARD(LAYER_SERVICE::Clean)
	dword n;
	
	if(!ToClean) return;
	
	for(n=0;n<=lss.Layer_max_index;n++)
	{
		if(Layer_Table[n] == null) continue;
		if(!Layer_Table[n]->ls.Deleted) continue;
		Erase(n);
	}

	ToClean = false;
	UNGUARD
}

void LAYER_SERVICE::Release()
{
	GUARD(LAYER_SERVICE::Release)
	dword n;
	for(n=0;n<=lss.Layer_max_index;n++)
	{
		if(Layer_Table[n] == null) continue;
		Erase(n);
	}
	UNGUARD
}

LAYER * LAYER_SERVICE::GetLayer(char * layer_name)
{
	dword index;
	index = GetIndex(layer_name);
	if(index == INVALID_LAYER_CODE) return null;
	return Layer_Table[index];
}

LAYER * LAYER_SERVICE::GetLayer(dword index)
{
	if(index == INVALID_LAYER_CODE || index > lss.Layer_max_index) return null;
	return Layer_Table[index];
}

bool LAYER_SERVICE::Add(dword index, ENTITY_ID eid, dword priority)
{
	if(Layer_Table[index] == 0) _THROW(invalid layer index);
	if(!Layer_Table[index]->Add(eid,priority)) THROW;
	return true;
}

void LAYER_SERVICE::Del(dword index, ENTITY_ID eid)
{
	if(Layer_Table[index] != 0) Layer_Table[index]->Del(eid);
}