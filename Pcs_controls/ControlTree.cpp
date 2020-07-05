#include "ControlTree.h"

ControlTree::ControlChild::ControlChild() :
	aChild(_FL)
{
	index = -1;
	sControlName = "";
	bActive = false;
	fTimeOut = 0.05f;  // boal fix  // 0.5f;
	fCurTime = 0.f;
	sOutControlName = "";
	nLayer = 0;
}

ControlTree::ControlChild::~ControlChild()
{
	index = -1;
	bActive = false;
	fCurTime = 0.f;
	aChild.DelAll();
}

void ControlTree::ControlChild::Process(float fDeltaTime,ControlTree* pControlTree)
{
	long n;

	// нет базовой контролки - всегда считаем активной
	// сразу переходим к дочерним веткам
	if( sControlName.IsEmpty() ) {
		bActive = true;
		for(n=0; n<aChild; n++)
			aChild[n].Process( fDeltaTime, pControlTree );
		return;
	}

	CONTROL_STATE cs;
	api->Controls->GetControlState((char*)sControlName.GetBuffer(),cs);

	// если узел уже активизирован, то считаем таймаут
	if( bActive )
	{
		if( fCurTime <= fTimeOut )
		{
			fCurTime += fDeltaTime;
		}
		/*else
		{
		    bActive = false;    // boal чтоб снималось
		} */
	}
	else
	//if (!bActive)   // boal если уже нет
	// если контролка не активизировалась, то проверяем ее на активизацию
	{
		// ничего не нажато - уходим
		if( cs.state==CST_INACTIVATED || cs.state==CST_INACTIVE )
			return;

		// было первое нажатие нажатие - активизируем и ставим таймаут на начало
		bActive = true;
		bWaitReleaseControl = false;
		fCurTime = 0.f;

		for(n=0; n<aChild; n++) {
			aChild[n].bActive = false;
			aChild[n].bWaitReleaseControl = false;
		}

		pControlTree->ControlInAction( sControlName, nLayer );
	}

	// таймаут прошел - включаем выходную контролку
	if( (!bWaitReleaseControl) && fCurTime>=fTimeOut ) {
		bActive = pControlTree->AddOutControl( sOutControlName, cs.state==CST_ACTIVATED || cs.state==CST_ACTIVE );
		return;
	}

	// переходим к дочерним веткам
	bool bChildActive = false;
	for(n=0; n<aChild; n++)
	{
		aChild[n].Process( fDeltaTime, pControlTree );
		if( aChild[n].bActive ) bChildActive = true;
	}
	// как только активизируется потомок, то текущая ветка не может выдавать контрол по таймауту
	if( bChildActive ) bWaitReleaseControl = true;

	if( bWaitReleaseControl && !bChildActive ) {
		if( cs.state==CST_INACTIVE || cs.state==CST_INACTIVATED )
			bActive = false;
	}
}

bool ControlTree::ControlChild::ControlInAction( ControlTree* pControlTree, const char* pcControlName, long _nLayer )
{
	if( _nLayer <= nLayer ) return false;

	if( sControlName == pcControlName ) {
		pControlTree->ExcludeControlFromActive(sOutControlName);
		bWaitReleaseControl = true;
		bActive = true;
		fCurTime = 0.f;
	}

	if( bActive ) {
		for(long n=0; n<aChild; n++)
		{
			if( aChild[n].ControlInAction( pControlTree, pcControlName, _nLayer ) )
				bWaitReleaseControl = true;
		}
	}

	return bWaitReleaseControl;
}

ControlTree::ControlTree() :
	m_aOutControlList(_FL)
{
	Init();
}

ControlTree::~ControlTree()
{
	Release();
}

void ControlTree::Process()
{
	long n;

	// все контролы которые деактивировались становятся неактивными
	// а все акивированные активными
	for(n=0; n<m_aOutControlList; n++)
	{
		if( m_aOutControlList[n].state == CST_INACTIVATED )
			m_aOutControlList[n].state = CST_INACTIVE;
		if( m_aOutControlList[n].state == CST_ACTIVATED )
			m_aOutControlList[n].state = CST_ACTIVE;
	}

	// работаем с деревом
	m_RootControl.Process(api->GetDeltaTime()*0.001f, this);

	// удалим деактивированные контролы
	for(n=0; n<m_aOutControlList; n++)
	{
		if( m_aOutControlList[n].state == CST_INACTIVE ) {
			m_aOutControlList.DelIndex( n );
			n--;
		}
	}
}

long ControlTree::AddControlChild(long nParentIdx,const char* pcControlName,const char* pcOutControlName,float fTimeOut)
{
	ControlChild* pParent = FindControlChild(nParentIdx);
	if( !pParent ) return -1;
	long n = pParent->aChild.Add();
	pParent->aChild[n].bActive = false;
	pParent->aChild[n].fCurTime = 0.f;
	pParent->aChild[n].fTimeOut = fTimeOut;
	pParent->aChild[n].index = m_nControlsNum;
	pParent->aChild[n].sControlName = pcControlName;
	pParent->aChild[n].sOutControlName = pcOutControlName;
	pParent->aChild[n].nLayer = pParent->nLayer + 1;
	m_nControlsNum++;
	return m_nControlsNum-1;
}

CONTROL_STATE_TYPE ControlTree::CheckControlActivated(const char* pcControlName)
{
	if( !pcControlName ) return CST_INACTIVE;
	for(long n=0; n<m_aOutControlList; n++)
		if( m_aOutControlList[n].sControlName == pcControlName )
			return m_aOutControlList[n].state;
	return CST_INACTIVE;
}

void ControlTree::Init()
{
	m_nControlsNum = 0;
}

void ControlTree::Release()
{
}

ControlTree::ControlChild* ControlTree::FindControlChild(long idx)
{
	if( idx<0 || idx>=m_nControlsNum ) return &m_RootControl;
	ControlChild* pCC = FindControlChild(idx,&m_RootControl);
	if( pCC ) return pCC;
	return &m_RootControl;
}

ControlTree::ControlChild* ControlTree::FindControlChild(long idx,ControlChild* pParent)
{
	if(!pParent) return 0;
	if( pParent->index == idx ) return pParent;
	for(long n=0; n<pParent->aChild; n++)
	{
		ControlChild* pCC = FindControlChild(idx,&pParent->aChild[n]);
		if( pCC ) return pCC;
	}
	return 0;
}

bool ControlTree::AddOutControl( const char* pcOutControlName, bool isActive )
{
	if( !pcOutControlName || !pcOutControlName[0] ) return false;

	// ищем контролку в списке
	for(long n=0; n<m_aOutControlList; n++)
		if( m_aOutControlList[n].sControlName == pcOutControlName )
			break;

	// не было такой контролки - добавляем с предыдущим состоянием = ненажата
	if( n>=m_aOutControlList ) {
		n = m_aOutControlList.Add();
		m_aOutControlList[n].sControlName = pcOutControlName;
		m_aOutControlList[n].state = CST_INACTIVE;
		isActive = true;
	}

	// нажата контролка
	CONTROL_STATE_TYPE cs_prev = m_aOutControlList[n].state;
	if( isActive ) {
		if( cs_prev==CST_INACTIVE || cs_prev==CST_INACTIVATED ) {
			m_aOutControlList[n].state = CST_ACTIVATED;
		} else {
			m_aOutControlList[n].state = CST_ACTIVE;
		}
	}
	else
	{
		if( cs_prev==CST_ACTIVE || cs_prev==CST_ACTIVATED ) {
			m_aOutControlList[n].state = CST_INACTIVATED;
		} else {
			m_aOutControlList[n].state = CST_INACTIVE;
		}
	}

	return (m_aOutControlList[n].state==CST_ACTIVE || m_aOutControlList[n].state==CST_ACTIVATED);
}

void ControlTree::ControlInAction( const char* pcControlName, long nLayer )
{
	m_RootControl.ControlInAction( this, pcControlName, nLayer );
}

bool ControlTree::ExcludeControlFromActive( const char* pcControlName )
{
	for(long n=0; n<m_aOutControlList; n++)
	{
		if( m_aOutControlList[n].sControlName == pcControlName ) {
			if( m_aOutControlList[n].state == CST_ACTIVATED ) {
				m_aOutControlList.DelIndex( n );
				return true;
			}
			return false;
		}
	}
	return true;
}