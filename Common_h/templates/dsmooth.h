#ifndef DSMOOTH_HPP
#define DSMOOTH_HPP

class DSmooth
{
public:
	DSmooth(float fSmoothTime);
	~DSmooth();

	void Add(float fValue);
	float GetSmoothValue();
	void Update(float fDeltaTime);

private:
	float fSmoothValue, fValue;
	float fCurrentTime, fSmoothTime;
	long iNumFrames;
};

inline DSmooth::DSmooth(float fSmoothTime)
{
	this->fValue = 0.0f;
	this->iNumFrames = 0;
	this->fSmoothTime = fSmoothTime;
	this->fCurrentTime = 0.0f;
}

inline DSmooth::~DSmooth()
{
}

inline float DSmooth::GetSmoothValue()
{
	return fSmoothValue;
}

inline void DSmooth::Add(float fValue)
{
	this->fValue += fValue;
	iNumFrames++;
}

inline void DSmooth::Update(float fDeltaTime)
{
	fCurrentTime += fDeltaTime;
	if (fCurrentTime > fSmoothTime)
	{
		while (fCurrentTime > fSmoothTime) fCurrentTime -= fSmoothTime;
		fSmoothValue = fValue / ((iNumFrames) ? iNumFrames : 1);
		fValue = 0.0f;
		iNumFrames = 0;
	}
}

#endif
