#ifndef _C_ATOM_H_
#define _C_ATOM_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif
#include "..\..\common_h\entity.h"

#define _LAYERS_BITMASK_DWORDS	4
#define _MAX_LAYERS				(_LAYERS_BITMASK_DWORDS*32)

struct ATOM_STATE
{
	ATOM_STATE()
	{
		Realize_ticks_av = 0;
		Realize_ticks_max = 0;
		Execute_ticks_av = 0;
		Execute_ticks_max = 0;
		Message_ticks_av = 0;
		Message_ticks_max = 0;
	};
	bool Connected;
	bool Deleted;
	//bool Service;
	dword Layers_mask[_LAYERS_BITMASK_DWORDS];
	dword SysLayers_mask[_LAYERS_BITMASK_DWORDS];

	dword Realize_ticks_av;
	dword Realize_ticks_max;
	dword Execute_ticks_av;
	dword Execute_ticks_max;
	dword Message_ticks_av;
	dword Message_ticks_max;

};

class C_ATOM 
{
public:
	C_ATOM();
	~C_ATOM(){};
	void SetLayerAttribute(dword index,bool system);
	void ClrLayerAttribute(dword index,bool system);
	bool TstLayerAttribute(dword index,bool system);

	ENTITY_ID atom_id;
	ATOM_STATE as;
};

#endif