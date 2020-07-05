#include "c_atom.h"

C_ATOM::C_ATOM()
{
	ZeroMemory(&as,sizeof(as));
	//as.Service = false;
	as.Deleted = false;
	as.Connected = false;
	ZeroMemory(&as.Layers_mask,sizeof(as.Layers_mask));
}

void C_ATOM::SetLayerAttribute(dword index,bool system)
{
	dword layer_flag;
	dword dword_number;
	dword_number = index/32;
	layer_flag = (1<<(index - dword_number*32));
	if(system) as.SysLayers_mask[dword_number] |= layer_flag;
	else as.Layers_mask[dword_number] |= layer_flag;
}

void C_ATOM::ClrLayerAttribute(dword index,bool system)
{
	dword layer_flag;
	dword dword_number;
	dword_number = index/32;
	layer_flag = (1<<(index - dword_number*32));
	if(system) as.SysLayers_mask[dword_number] &= (~layer_flag);
	else as.Layers_mask[dword_number] &= (~layer_flag);
}

bool C_ATOM::TstLayerAttribute(dword index,bool system)
{
	dword layer_flag;
	dword dword_number;
	dword_number = index/32;
	layer_flag = (1<<(index - dword_number*32));
	if(system) layer_flag = layer_flag & as.SysLayers_mask[dword_number];
	else layer_flag = layer_flag & as.Layers_mask[dword_number];
	if(layer_flag != 0) return true;
	return false;
}
