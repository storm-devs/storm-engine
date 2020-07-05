#ifndef SEA_FORT_AI_HPP
#define SEA_FORT_AI_HPP

#include "AICannon.h"
#include "AIShip.h"

#include "..\common_h\ShipLights.h"

// ============================================================================
// Master class AIFort
// Contain AI part for fort, fort cannons, collision part and other
// ============================================================================
class AIShip;
class AIGroup;

class AIFort : public VAI_OBJBASE
{
public:
	class AI_FORT : public VAI_INNEROBJ
	{
	private:
		class TmpVAI_OBJBASE : public VAI_OBJBASE
		{
		public:
			TmpVAI_OBJBASE()
			{
				mTmp.SetIdentity();
			}

			CMatrix mTmp;
			AI_FORT * pFort;

			virtual CMatrix * GetMatrix() { return &GetModel()->mtx; };

			virtual MODEL *		GetModel() { return pFort->GetModel(); };
			virtual ENTITY_ID	GetModelEID() { return pFort->GetModelEID(); };

			virtual void Save(CSaveLoad * pSL) {};
			virtual void Load(CSaveLoad * pSL) {};	
			virtual void Fire(const CVECTOR & vPos)	{};

			virtual float Cannon_Trace(long iBallOwner, const CVECTOR &src, const CVECTOR &dst) { return 2.0f; };
			virtual float Trace(const CVECTOR & vSrc, const CVECTOR & vDst) { return 2.0f; };
			virtual bool Clip(const PLANE * planes, long nplanes, const CVECTOR & vCenter, float radius, ADD_POLYGON_FUNC addpoly) { return false; };
			virtual const char * GetCollideMaterialName() { return null; };
			virtual bool GetCollideTriangle(TRIANGLE & triangle) { return false; };
		};

		ENTITY_ID			eidModel;
		ENTITY_ID			eidBlot;
					

	public:
		CMatrix				mOldMatrix;
		CVECTOR				vPos;
		ATTRIBUTES			* pFortLabelAP;
		TmpVAI_OBJBASE		tmpObject;

		array<AICannon>		aCannons;			// fort cannons container
		array<AICannon>		aCulverins;			// fort culverins container
		array<AICannon>		aMortars;			// fort mortars container

		dword				dwCannonType, dwCulverinType, dwMortarType;

		MODEL *		GetModel() { Assert(api->ValidateEntity(&GetModelEID())); return (MODEL*)api->GetEntityPointer(&GetModelEID()); }
		void		SetModelEID(ENTITY_ID _eidModel) { eidModel = _eidModel; } 
		ENTITY_ID	GetModelEID() { return eidModel; } 
		void		SetBlotEID(ENTITY_ID _eidBlot) { eidBlot = _eidBlot; } 
		ENTITY_ID	GetBlotEID() { return eidBlot; } 
		dword		GetAllCannonsNum() const { return aCannons.Size() + aCulverins.Size() + aMortars.Size(); }
		dword		GetCannonType(dword dwCannonIndex) 
		{ 
			if (dwCannonIndex < aCannons.Size()) return dwCannonType;
			if (dwCannonIndex < aCannons.Size() + aCulverins.Size()) return dwCulverinType;
			return dwMortarType;
		} 
		dword		GetDamagedCannonsNum() 
		{
			dword i, dwDamagedNum = 0;

			for (i=0; i<aCannons.Size(); i++) if (aCannons[i].isDamaged()) dwDamagedNum++;
			for (i=0; i<aCulverins.Size(); i++) if (aCulverins[i].isDamaged()) dwDamagedNum++;
			for (i=0; i<aMortars.Size(); i++) if (aMortars[i].isDamaged()) dwDamagedNum++;
			
			return dwDamagedNum;
		}

		bool		isNormalMode()
		{
			ATTRIBUTES * pAFM = GetACharacter()->FindAClass(GetACharacter(),"Fort.Mode");
			if (pAFM) return (pAFM->GetAttributeAsDword() == FORT_NORMAL);
			return false;
		}

		AICannon	* GetCannon(dword dwCannonIndex)
		{
			if (dwCannonIndex < aCannons.Size()) return &aCannons[dwCannonIndex];
			if (dwCannonIndex < aCannons.Size() + aCulverins.Size()) return &aCulverins[dwCannonIndex - aCannons.Size()];
			return &aMortars[dwCannonIndex - (aCannons.Size() + aCulverins.Size())];
		}

		AI_FORT(ATTRIBUTES * _pFortLabelAP) : aMortars(_FL_), aCannons(_FL_, 32), aCulverins(_FL_, 32)
		{
			SetObjType(AIOBJ_FORT);

			tmpObject.pFort = this;

			pFortLabelAP = _pFortLabelAP;

			vPos.x = pFortLabelAP->GetAttributeAsFloat("x");
			vPos.y = 0.0f;//pFortLabelAP->GetAttributeAsFloat("y");
			vPos.z = pFortLabelAP->GetAttributeAsFloat("z");
		}

		virtual CVECTOR		GetPos() const { return vPos; };
		virtual CVECTOR		GetAttackPoint(VAI_INNEROBJ *);
		virtual CVECTOR		GetFollowPoint(VAI_INNEROBJ *) { return 0.0f; };
		virtual CVECTOR		GetAbordagePoint(VAI_INNEROBJ *) { return 0.0f; };

		virtual float		GetMaxFireDistance() { return 0.0f; };
		virtual float		GetMinFireDistance() { return 0.0f; };

		void Save(CSaveLoad * pSL);
		void Load(CSaveLoad * pSL, ENTITY_ID eid);
	};

	dword		GetNumForts() { return aForts.Size(); }
	AI_FORT		* FindFort(ATTRIBUTES * pACharacter);
	AI_FORT		* GetFort(dword k)
	{
		Assert(k < aForts.Size());
		return aForts[k];
	}
private:
	CMatrix					mtxFort;

	DTimer					dtFiredTimer;
	AI_FORT					* pLastTraceFort;
	array<AI_FORT*>			aForts;				// fort container

	float	fMinCannonDamageDistance;

	// Ships lights 
	IShipLights			* pShipsLights;

	void	AddFortHit(long iCharacterIndex, CVECTOR & vHitPos);
	float	GetSpeedV0(dword dwFortIndex);
	bool	ScanFortForCannons(AI_FORT * pFort, char * pModelsDir, char * pLocatorsName);
	bool	AddFort(ATTRIBUTES * pIslandAP, ATTRIBUTES * pFortAP, ATTRIBUTES * pFortCharacter, ENTITY_ID eidModel, ENTITY_ID eidBlot);
	AI_FORT * FindFort(ENTITY_ID eidModel);

public:
	static AIFort			* pAIFort;

	AIFort();
	~AIFort();

	// AI section
		bool		isAttack(AIGroup * pGroup);
		bool		isDead();		// is group dead?
		float		GetPower();

	// inherit functions ENTITY
		bool Init();
		void SetDevice();

		void Realize(dword Delta_Time);
		void Execute(dword Delta_Time);
		
		bool CreateState(ENTITY_STATE_GEN * state_gen);
		bool LoadState(ENTITY_STATE * state);
		
		dword _cdecl ProcessMessage(MESSAGE & message);

		dword AttributeChanged(ATTRIBUTES * pAttribute);

	// inherit functions COLLISION_OBJECT
		virtual float Trace(const CVECTOR & vSrc, const CVECTOR & vDst);
		virtual bool Clip(const PLANE * planes, long nplanes, const CVECTOR & vCenter, float radius, ADD_POLYGON_FUNC addpoly) { return false; };

		virtual const char * GetCollideMaterialName() { return null; };
		virtual bool GetCollideTriangle(TRIANGLE & triangle) { return false; };

	// inherit functions CANNON_TRACE_BASE
		float		Cannon_Trace(long iBallOwner, const CVECTOR & vSrc, const CVECTOR & vDst);

	// inherit functions VAI_OBJBASE
		ATTRIBUTES	* GetACharacter();

		CMatrix		* GetMatrix() { return &mtxFort; };
		MODEL		* GetModel() { return null; };
		ENTITY_ID	GetModelEID() { ENTITY_ID eid; return eid; };
		CVECTOR		GetPos() { return CVECTOR(0.0f,0.0f,0.0f); };
		CVECTOR		GetAng() { return CVECTOR(0.0f,0.0f,0.0f); };

		bool		Mount(ATTRIBUTES * pAttribute);

		void Save(CSaveLoad * pSL);
		void Load(CSaveLoad * pSL);

		void Fire(const CVECTOR & vPos);
};

#endif