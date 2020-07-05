#ifndef _CONTROLTREE_H_
#define _CONTROLTREE_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\controls.h"
#include "..\common_h\templates\string.h"
#include "..\common_h\templates\array.h"

struct ControlTree
{
public:
	struct ControlChild
	{
		ControlChild();
		~ControlChild();

		void Process(float fDeltaTime,ControlTree* pControlTree);
		bool ControlInAction( ControlTree* pControlTree, const char* pcControlName, long _nLayer );

		long index;
		string sControlName;
		array<ControlChild> aChild;
		bool bActive;
		bool bWaitReleaseControl;
		float fTimeOut;
		float fCurTime;
		string sOutControlName;
		long nLayer;
	};

	struct OutControlInfo
	{
		string sControlName;
		CONTROL_STATE_TYPE state;
	};

public:
	ControlTree();
	~ControlTree();

	void Process();

	long AddControlChild(long nParentIdx,const char* pcControlName,const char* pcOutControlName,float fTimeOut);
	CONTROL_STATE_TYPE CheckControlActivated(const char* pcControlName);

	bool AddOutControl( const char* pcOutControlName, bool isActive );
	void ControlInAction( const char* pcControlName, long nLayer );
	bool ExcludeControlFromActive( const char* pcControlName );

protected:
	void Init();
	void Release();
	ControlChild* FindControlChild(long idx);
	ControlChild* FindControlChild(long idx,ControlChild* pParent);

protected:
	long m_nControlsNum;
	ControlChild m_RootControl;

	array<OutControlInfo> m_aOutControlList;
};

#endif
