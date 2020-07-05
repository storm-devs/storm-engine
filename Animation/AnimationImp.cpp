//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	AnimationImp
//--------------------------------------------------------------------------------------------
//	Реализация интерфейса Animation
//============================================================================================

#include "AnimationImp.h"
#include "AnimationServiceImp.h"

//============================================================================================


//Указатель на сервис анимации
AnimationServiceImp * AnimationImp::aniService = null;

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

AnimationImp::AnimationImp(long id, AnimationInfo * animationInfo)
{
	Assert(animationInfo);
	aniInfo = animationInfo;
	aniInfo->AddRef();
	thisID = id;
	for(long i = 0; i < ANI_MAX_ACTIONS; i++) 
	{
		action[i].SetAnimation(this, i);
		timer[i].SetAnimation(this);
	}
	matrix = NEW CMatrix[aniInfo->NumBones()];
	memset(ae_listeners, 0, sizeof(ae_listeners));
	ae_listenersExt = null;
	//Автонормализация
	isAutoNormalize = true;
	//Пользовательский блендинг
	isUserBlend = true;
}

AnimationImp::~AnimationImp()
{
	aniInfo->RelRef();
	aniService->DeleteAnimation(this);
	delete [] matrix;
}

//--------------------------------------------------------------------------------------------
//Animation
//--------------------------------------------------------------------------------------------

//Доступиться к проигрывателю действий
ActionPlayer & AnimationImp::Player(long index)
{
	Assert(index >= 0 && index < ANI_MAX_ACTIONS);
	return action[index];
}

//Доступиться к таймеру анимации
AnimationTimer & AnimationImp::Timer(long index)
{
	Assert(index >= 0 && index < ANI_MAX_ACTIONS);
	return timer[index];
}

//События
//Установить внутренние событие
long AnimationImp::SetEvent(AnimationEvent event, long index, AnimationEventListener * ael)
{
	Assert(event < ae_numevents);
	if(!ael) return -1;
	for(long i = 0; i < ANIIMP_MAXLISTENERS; i++)
		if(!ae_listeners[event][i])
		{
			ae_listeners[event][i] = ael;
			return (event << 16) | i;
		}
	return -1;
}

//Удалить внутренние событие
void AnimationImp::DelEvent(long eventID)
{
	if(eventID < 0) return;
	long event = eventID >> 16;
	long lindex = eventID & 0xffff;
	Assert(event < ae_numevents);
	Assert(lindex < ANIIMP_MAXLISTENERS);
	Assert(ae_listeners[event][lindex]);
	ae_listeners[event][lindex] = null;
}

//Установить обработчик внешнего события
void AnimationImp::SetEventListener(AnimationEventListener * ael)
{
	ae_listenersExt = ael;
}

//Получить количество костей в скелете
long AnimationImp::GetNumBones() const
{
	return aniInfo->NumBones();
}

//Получить матрицу анимации для кости
CMatrix & AnimationImp::GetAnimationMatrix(long iBone) const
{
	Assert(iBone >= 0 && iBone < aniInfo->NumBones());
	return matrix[iBone];
}

//Получить пользовательские данные для анимации
const char * AnimationImp::GetData(const char * dataName) const
{
	return aniInfo->GetUserData().GetData(dataName);
}

//Копировать состояние одного плеера в другой
void AnimationImp::CopyPlayerState(long indexSrc, long indexDst, bool copyTimerState)
{
	Assert(indexSrc >= 0 && indexSrc < ANI_MAX_ACTIONS);
	Assert(indexDst >= 0 && indexDst < ANI_MAX_ACTIONS);
	if(indexSrc == indexDst) return;
	action[indexDst].CopyState(action[indexSrc]);
	for(long i = 0; i < ANI_MAX_ACTIONS; i++)
	{
		bool isInv = false;
		if(timer[i].IsUsedPlayer(indexSrc, &isInv))
		{
			timer[i].SetPlayer(indexDst, isInv);
		}
	}
}

//Получить скорость исполнения анимации
float AnimationImp::GetFPS()
{
	return aniInfo->GetFPS();
}

//Установить режимы блендинга
//Автоматическая нормализация коэфициентов блендинга
bool AnimationImp::SetAutoNormalize(bool isNormalize)
{
	bool b = isAutoNormalize;
	isAutoNormalize = isNormalize;
	return b;
}

bool AnimationImp::GetAutoNormalize()
{
	return isAutoNormalize;
}

//Разрешить использование пользовательских коэфициентов блендинга в ActionPlayer
bool AnimationImp::UserBlend(bool isBlend)
{
	bool b = isUserBlend;
	isUserBlend = isBlend;
	return b;
}

bool AnimationImp::IsUserBlend()
{
	return isUserBlend;
}


//--------------------------------------------------------------------------------------------
//AnimationImp
//--------------------------------------------------------------------------------------------

//Сделать шаг по времени
void AnimationImp::Execute(long dltTime)
{
	//Исполним animation
	for(long i = 0; i < ANI_MAX_ACTIONS; i++)
						action[i].Execute(dltTime);
	//Исполним таймеры
	for(i = 0; i < ANI_MAX_ACTIONS; i++)
						timer[i].Execute(dltTime);
	//Расчитаем матрицы анимации
	BuildAnimationMatrices();
}
			
//Расчитать матрицы анимации
void AnimationImp::BuildAnimationMatrices()
{
	long nFrames = aniInfo->GetAniNumFrames();
	long nbones = aniInfo->NumBones();
	//Посмотрим сколько плееров играет, считаем текущии коэфициенты блендинга
	long plCnt = 0;
	float normBlend = 0.0f;
	for(long i = 0; i < ANI_MAX_ACTIONS; i++)
		if(action[i].IsPlaying())
		{
			plCnt++;
			action[i].kBlendCurrent = action[i].TimerBlend();
			if(isUserBlend) action[i].kBlendCurrent *= action[i].Blend();
			normBlend += action[i].kBlendCurrent;
		}
	if(!plCnt) return;

	//Автонормализация
	if(normBlend != 0.0f)
		if(plCnt > 1)
		{
			normBlend = 1.0f/normBlend;

			float frame0 = action[0].GetCurrentFrame();
			long f0 = long(frame0);
			float ki0 = frame0 - float(f0);
			if(f0>=nFrames)
			{
				f0 = nFrames-1;
				ki0 = 0.0f;
			}

			float frame1 = action[1].GetCurrentFrame();
			long f1 = long(frame1);
			float ki1 = frame1 - float(f1);
			if(f1>=nFrames)
			{
				f1 = nFrames-1;
				ki1 = 0.0f;
			}

			float kBlend = 1.0f - action[0].kBlendCurrent*normBlend;
			//-------------------------------------------------------------------------
			for(long j = 0; j < nbones; j++)	
			{
				Bone &bn = aniInfo->GetBone(j);
				CMatrix inmtx;
				D3DXQUATERNION qt0,qt1, qt;
				bn.BlendFrame(f0, ki0, qt0);
				bn.BlendFrame(f1, ki1, qt1);
				D3DXQuaternionSlerp(&qt, &qt0, &qt1, kBlend);
				D3DXMatrixRotationQuaternion(inmtx, &qt);
				inmtx.Pos() = bn.pos0;
				if(j==0)
				{
					CVECTOR p0 = bn.pos[f0] + ki0*(bn.pos[f0+1] - bn.pos[f0]);
					CVECTOR p1 = bn.pos[f1] + ki1*(bn.pos[f1+1] - bn.pos[f1]);
					inmtx.Pos() = p0 + kBlend*(p1-p0);
				}

				if(bn.parent)	bn.matrix.EqMultiply( inmtx, CMatrix(bn.parent->matrix));
				else	bn.matrix = inmtx;
			}
		}
		else
			if(action[0].IsPlaying())
			{
				float frame = action[0].GetCurrentFrame();
				long f = long(frame);
				float ki = frame - float(f);
				if(f>=nFrames)
				{
					f = nFrames-1;
					ki = 0.0f;
				}

				//-------------------------------------------------------------------------
				for(long j = 0; j < nbones; j++)	
				{
					Bone &bn = aniInfo->GetBone(j);
					CMatrix inmtx;
					D3DXQUATERNION qt;
					bn.BlendFrame(f, ki, qt);
					D3DXMatrixRotationQuaternion(inmtx, &qt);
					inmtx.Pos() = bn.pos0;
					if(j==0)	inmtx.Pos() = bn.pos[f] + ki*(bn.pos[f+1] - bn.pos[f]);

					if(bn.parent)	bn.matrix.EqMultiply( inmtx, CMatrix(bn.parent->matrix));
					else	bn.matrix = inmtx;
				}
			}
			else
				if(action[1].IsPlaying())
				{
					float frame = action[1].GetCurrentFrame();
					long f = long(frame);
					float ki = frame - float(f);
					if(f>=nFrames)
					{
						f = nFrames-1;
						ki = 0.0f;
					}

					//-------------------------------------------------------------------------
					for(long j = 0; j < nbones; j++)	
					{
						Bone &bn = aniInfo->GetBone(j);
						CMatrix inmtx;
						D3DXQUATERNION qt;
						bn.BlendFrame(f, ki, qt);
						D3DXMatrixRotationQuaternion(inmtx, &qt);
						inmtx.Pos() = bn.pos0;
						if(j==0)	inmtx.Pos() = bn.pos[f] + ki*(bn.pos[f+1] - bn.pos[f]);

						if(bn.parent)	bn.matrix.EqMultiply( inmtx, CMatrix(bn.parent->matrix));
						else	bn.matrix = inmtx;
					}
				}
				else
				{
					_CORE_API->Trace("AnimationImp::BuildAnimationMatrices -> Not support mode");
					_asm int 3;
				//	float frame = 0.0f;
				//	for(long j = 0; j < nbones; j++)
				//		aniInfo->GetBone(j).BlendFrame(frame);
				}
	for(long j=0; j<nbones; j++)
	{
		Bone &bn = aniInfo->GetBone(j);
		matrix[j] = CMatrix(bn.start)*CMatrix(bn.matrix);
	}
}

//События
//Разослать события
void AnimationImp::SendEvent(AnimationEvent event, long index)
{
	for(long i = 0; i < ANIIMP_MAXLISTENERS; i++)
	{
		if(ae_listeners[event][i])
		{
			ae_listeners[event][i]->Event(this, index, (event << 16) | i, event);
		}
	}
}

