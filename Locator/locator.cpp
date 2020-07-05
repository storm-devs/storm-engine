#include "locator.h"
#include "..\common_h\messages.h"

INTERFACE_FUNCTION
CREATE_CLASS(LOCATOR)
CREATE_CLASS(BLAST)

LOCATOR::LOCATOR()
{
	gs = 0;
	geo = 0;
	groupID = -1;
	stringIndex = -1;
}

LOCATOR::~LOCATOR()
{
	if(geo) delete geo; geo = 0;
}

bool LOCATOR::Init() 
{	
	rs = (VDX8RENDER *)api->CreateService("dx8render");
	gs = (VGEOMETRY *)api->CreateService("geometry");
	if(!gs) return false;
	
	return true;
}

bool LOCATOR::VerifyParticles()
{
	if(!api->FindClass(&ParticlesID,"particles",0))
	{
		if(!api->CreateEntity(&ParticlesID,"particles")) return false;
	}
	return true;
}


void LOCATOR::LocateForI_L2(ATTRIBUTES * pA,GEOS * g, GEOS::LABEL & label)
{
	long groupID;
	long stringIndex;
	long n;
	char name[16];
	ATTRIBUTES * pAA;
	GEOS::LABEL label2;

	groupID = g->FindName(label.name);
	if(groupID < 0)
	{
		api->Trace("?void LOCATOR::LocateForI_L2(...)");
		return;
	}
	
	pA = pA->CreateSubAClass(pA,"ships");

	n = 0;
	for(stringIndex = 0; (stringIndex = g->FindLabelG(stringIndex, groupID))>=0; stringIndex++)
	{
		g->GetLabel(stringIndex, label2);
		wsprintf(name,"l%d",n);
		pAA = pA->CreateSubAClass(pA,name);
		pAA->SetAttributeUseFloat("x",label2.m[3][0]);
		pAA->SetAttributeUseFloat("y",label2.m[3][1]);
		pAA->SetAttributeUseFloat("z",label2.m[3][2]);
		pAA->SetAttributeUseFloat("ay",atan2f(label2.m[2][0],label2.m[2][2]));
		n++;
	}

}

void LOCATOR::LocateForI(VDATA * pData)
{
	ATTRIBUTES	* pA;
	ATTRIBUTES	* pAA;
	GEOS		* g;
	GEOS::LABEL label;
	long		groupID;
	long		i, n;

	if(pData == 0)
	{
		api->Trace("?void LOCATOR::LocateForI(VDATA * pData)");
		return;
	}
	pA = pData->GetAClass();
	if(pA == null)
	{
		api->Trace("?void LOCATOR::LocateForI(VDATA * pData)");
		return;
	}
	if(!pA->GetAttribute("locators"))
	{
		api->Trace("?void LOCATOR::LocateForI(VDATA * pData)");
		return;
	}
	char sFileLocators[256];
	ATTRIBUTES * pAFilesPath = pA->FindAClass(pA, "filespath.models");
	sprintf(sFileLocators, "%s\\%s", (pAFilesPath) ? pAFilesPath->GetThisAttr() : "", pA->GetAttribute("locators"));
	rs->SetLoadTextureEnable(false);
	g = gs->CreateGeometry(sFileLocators, "", 0);
	rs->SetLoadTextureEnable(true);
	if(!g)
	{
		api->Trace("?void LOCATOR::LocateForI(VDATA * pData)");
		return;
	}
	
	groupID = g->FindName("reload");
	if(groupID >= 0)
	{
		for(i=0; (i = g->FindLabelG(i, groupID))>=0; i++)
		{
			g->GetLabel(i, label);
			pAA = pA->FindAClass(pA,"reload");
			if (pAA) for(n=0;n<(long)pAA->GetAttributesNum();n++)
			{
				if(pAA->GetAttributeClass(n))
				{
					if(!pAA->GetAttributeClass(n)->GetAttribute("name"))
					{
						api->Trace("LOCATOR: no name");
						continue;
					}
					if(stricmp(pAA->GetAttributeClass(n)->GetAttribute("name"),label.name)==0)
					{
						pAA->GetAttributeClass(n)->SetAttributeUseFloat("x",label.m[3][0]);
						pAA->GetAttributeClass(n)->SetAttributeUseFloat("y",label.m[3][1]);
						pAA->GetAttributeClass(n)->SetAttributeUseFloat("z",label.m[3][2]);
						pAA->GetAttributeClass(n)->SetAttributeUseFloat("ay",atan2f(label.m[2][0],label.m[2][2]));
						LocateForI_L2(pAA->GetAttributeClass(n),g,label);
					}
				}
			}
		}
	}

	// check for unfind reloads
	pAA = pA->FindAClass(pA,"reload");
	if (pAA) for(n=0;n<(long)pAA->GetAttributesNum();n++)
	{
		ATTRIBUTES * pARC = pAA->GetAttributeClass(n);
		if (!pARC->FindAClass(pARC, "x"))
		{
			api->Trace("LOCATOR: Can't find locator with name: %s, geo: %s", pARC->GetAttribute("name"), pA->GetAttribute("locators"));
		}
	}


	groupID = g->FindName("quest_ships");
	if(groupID >= 0)
	{
		pAA = pA->FindAClass(pA,"Quest_ships");
		if (!pAA) pAA = pA->CreateAttribute("Quest_ships","");
		if (pAA)
			LocateForI_Locators(pAA,g,groupID,_XYZ_|_AY_);
	}

	groupID = g->FindName("net_deathmatch");
	if(groupID >= 0)
	{
		pAA = pA->CreateAttribute("net_deathmatch","");
		if (pAA)
			LocateForI_Locators(pAA,g,groupID,_XYZ_|_AY_);
	}

	groupID = g->FindName("net_team");
	if(groupID >= 0)
	{
		pAA = pA->CreateAttribute("net_team","");
		if (pAA)
			LocateForI_Locators(pAA,g,groupID,_XYZ_|_AY_);
	}

	groupID = g->FindName("net_convoy");
	if(groupID >= 0)
	{
		pAA = pA->CreateAttribute("net_convoy","");
		if (pAA)
			LocateForI_Locators(pAA,g,groupID,_XYZ_|_AY_);
	}

	groupID = g->FindName("net_fort");
	if(groupID >= 0)
	{
		pAA = pA->CreateAttribute("net_fort","");
		if (pAA)
			LocateForI_Locators(pAA,g,groupID,_XYZ_|_AY_);
	}

	groupID = g->FindName("fire");
	if(groupID >= 0)
	{
		pAA = pA->FindAClass(pA,"fire");
		if (!pAA) pAA = pA->CreateAttribute("fire","");
		if (pAA)
			LocateForI_Locators(pAA,g,groupID,_XYZ_);
	}

	ATTRIBUTES * pGA = pA->FindAClass(pA, "LoadGroup");
	if (pGA) for(n=0; n<(long)pGA->GetAttributesNum(); n++)
	{
		ATTRIBUTES * pARC = pGA->GetAttributeClass(n);
		const char * pLoadGroupName = pARC->GetThisAttr();
		if (!pLoadGroupName) continue;

		groupID = g->FindName(pLoadGroupName);
		if (groupID < 0) continue;

		pAA = pA->FindAClass(pA, pLoadGroupName);
		if (!pAA) pAA = pA->CreateAttribute(pLoadGroupName, "");
		if (pAA)
			LocateForI_Locators(pAA, g, groupID, _XYZ_|_AY_);
	}

	delete g;
}

void LOCATOR::LocateForI_Locators(ATTRIBUTES * pA, GEOS * geo, long iGroupID, dword dwFlags)
{
	long			i;
	GEOS::LABEL		label;
	ATTRIBUTES		* pAA;

	for(i = 0; (i = geo->FindLabelG(i, iGroupID))>=0; i++)
	{
		geo->GetLabel(i, label);
		pAA = pA->CreateSubAClass(pA,label.name);
		if (dwFlags & _X_) pAA->SetAttributeUseFloat("x",label.m[3][0]);
		if (dwFlags & _Y_) pAA->SetAttributeUseFloat("y",label.m[3][1]);
		if (dwFlags & _Z_) pAA->SetAttributeUseFloat("z",label.m[3][2]);
		if (dwFlags & _AY_) pAA->SetAttributeUseFloat("ay",atan2f(label.m[2][0],label.m[2][2]));
	}

}

dword _cdecl LOCATOR::ProcessMessage(MESSAGE & message)
{
	long message_code;
	char name[MAX_PATH];
	GEOS::LABEL label;
	ATTRIBUTES * pA;
	char buffer[MAX_PATH];

	message_code = message.Long();
	switch(message_code)
	{
		case LM_LOCATE_I:
			LocateForI(message.ScriptVariablePointer());
		break;
		case LM_LOCATE_FIRST:
			message.String(sizeof(buffer),buffer);
			pA = message.AttributePointer();
			groupID = geo->FindName(buffer);
			if(groupID >= 0)
			{
				VerifyParticles();
				stringIndex = geo->FindLabelG(0, groupID);
				if(stringIndex < 0) break;
				geo->GetLabel(stringIndex, label);
				

				if(pA)
				{
					pA->SetAttributeUseFloat("x",label.m[3][0]);
					pA->SetAttributeUseFloat("y",label.m[3][1]);
					pA->SetAttributeUseFloat("z",label.m[3][2]);
					pA->SetAttributeUseFloat("ay",atan2f(label.m[2][0],label.m[2][2]));
					pA->SetAttributeUseFloat("vx",label.m[2][0]);
					pA->SetAttributeUseFloat("vy",label.m[2][1]);
					pA->SetAttributeUseFloat("vz",label.m[2][2]);
				}
				stringIndex++;
				return 1;
			}
		return 0;
		case LM_LOCATE_NEXT:
			pA = message.AttributePointer();
			if(groupID >= 0)
			{
				VerifyParticles();
				stringIndex = geo->FindLabelG(stringIndex, groupID);
				if(stringIndex < 0) return 0;
				geo->GetLabel(stringIndex, label);
				if(pA)
				{
					pA->SetAttributeUseFloat("x",label.m[3][0]);
					pA->SetAttributeUseFloat("y",label.m[3][1]);
					pA->SetAttributeUseFloat("z",label.m[3][2]);
					pA->SetAttributeUseFloat("ay",atan2f(label.m[2][0],label.m[2][2]));
					pA->SetAttributeUseFloat("vx",label.m[2][0]);
					pA->SetAttributeUseFloat("vy",label.m[2][1]);
					pA->SetAttributeUseFloat("vz",label.m[2][2]);
				}
				stringIndex++;
				return 1;
			}
		return 0;

		case LM_SET_GEOMETRY:
			message.String(sizeof(name),name);
			if(geo) delete geo; geo = 0;
			rs->SetLoadTextureEnable(false);
			geo = gs->CreateGeometry(name,"",0);
			rs->SetLoadTextureEnable(true);
		break;
		/*case LM_LOCATE:
			groupID = geo->FindName("smoke");
			if(groupID >= 0)
			{
				VerifyParticles();
				for(stringIndex = 0; (stringIndex = geo->FindLabelG(stringIndex, groupID)) >= 0; stringIndex++)
				{
					geo->GetLabel(stringIndex, label);
					api->Send_Message(ParticlesID,"lsffffffl",
						PS_CREATE,"smoke",label.m[3][0],label.m[3][1],label.m[3][2],-1.57,0,0,0);
				}
			}

			groupID = geo->FindName("fire");
			if(groupID >= 0)
			{
				if(!api->FindClass(&ParticlesID,"particles",0))
				{
					if(!api->CreateEntity(&ParticlesID,"particles")) return 0;
				}
				for(stringIndex = 0; (stringIndex = geo->FindLabelG(stringIndex, groupID)) >= 0; stringIndex++)
				{
					geo->GetLabel(stringIndex, label);
					api->Send_Message(ParticlesID,"lsffffffl",
						PS_CREATE,"fire",label.m[3][0],label.m[3][1],label.m[3][2],-1.57,0,0,0);
				}
			}

			groupID = geo->FindName("water");
			if(groupID >= 0)
			{
				if(!api->FindClass(&ParticlesID,"particles",0))
				{
					if(!api->CreateEntity(&ParticlesID,"particles")) return 0;
				}
				for(stringIndex = 0; (stringIndex = geo->FindLabelG(stringIndex, groupID)) >= 0; stringIndex++)
				{
					
					geo->GetLabel(stringIndex, label);
					api->Send_Message(ParticlesID,"lsffffffl",
						PS_CREATEX,"waterfall",label.m[3][0],label.m[3][1],label.m[3][2],label.m[2][0],label.m[2][1],-label.m[2][2],0);
				}
			}
		break;*/
	}
	return 0;
}

dword LOCATOR::AttributeChanged(ATTRIBUTES * pA)
{

	return 0;
}