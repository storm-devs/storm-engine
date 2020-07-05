#include "vcollide.h"
#include "..\common_h\object.h"

INTERFACE_FUNCTION
CREATE_SERVICE(COLL)

ENTITY_ID last_trace_eid;

COLL::COLL()
{
}
COLL::~COLL()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
LOCAL_COLLIDE *COLL::CreateLocalCollide(const char *layerName)
{
	return NEW LCOLL(layerName, *_CORE_API);
}
//----------------------------------------------------------------------------------
//Ray tracing 
//----------------------------------------------------------------------------------
float COLL::Trace(ENTITY_ID &entity, const CVECTOR &src, const CVECTOR &dst)
{
	COLLISION_OBJECT *cob = static_cast<COLLISION_OBJECT*>(_CORE_API->GetEntityPointer(&entity));
	if(static_cast<ENTITY*>(cob)==null)	return 2.0f;

	last_trace_eid = entity;
	return cob->Trace(src, dst);
}

//----------------------------------------------------------------------------------
//with enclusion list
//----------------------------------------------------------------------------------
float COLL::Trace(VIDWALKER &walker, const CVECTOR &src, const CVECTOR &dst, const ENTITY_ID *exclude_list, long entities)
{
	float best_res = 2.0f;
	ENTITY_ID *eid = walker.GetID();
	while(eid!=0)
	{
		for(long e=0; e<entities; e++)
			if(*eid==exclude_list[e])	break;
		if(e==entities)
		{
			COLLISION_OBJECT *cob = static_cast<COLLISION_OBJECT*>(_CORE_API->GetEntityPointer(eid));
			if(cob!=null)
			{
				float res = cob->Trace(src, dst);
				if(res<best_res)
				{
					best_res = res;
					last_trace_eid = *eid;
				}
			}
		}

		eid = walker.GetIDNext();
	}

	return best_res;
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool COLL::Clip(VIDWALKER &walker, const PLANE *planes, long nplanes, const CVECTOR &center, float radius,
	ADD_POLYGON_FUNC addpoly, const ENTITY_ID *exclude_list, long entities)
{
	bool retval = false;

	ENTITY_ID *eid = walker.GetID();
	while(eid!=0)
	{
		for(long e=0; e<entities; e++)
			if(*eid==exclude_list[e])	break;
		if(e==entities)
		{
			COLLISION_OBJECT *cob = static_cast<COLLISION_OBJECT*>(_CORE_API->GetEntityPointer(eid));
			if(cob!=null)
			{
				last_trace_eid = *eid;
				if(cob->Clip(planes, nplanes, center, radius, addpoly)==true)
					retval = true;
			}
		}

		eid = walker.GetIDNext();
	}

	return retval;
}


//----------------------------------------------------------------------------------
//get last trace entity id
//----------------------------------------------------------------------------------
ENTITY_ID COLL::GetObjectID()
{
	return last_trace_eid;
}
