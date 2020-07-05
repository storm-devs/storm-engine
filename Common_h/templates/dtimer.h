#ifndef DTIMER_HPP
#define DTIMER_HPP

class DTimer
{
private:
	float	fCurrent, fMin, fMax;

public:
	DTimer() { Setup(0.0f, 1.0f); };
	DTimer(float _fCurrent, float _fMax) { Setup(_fCurrent, _fMax); };
	~DTimer() {};

	void Setup(float _fCurrent, float _fMin, float _fMax)
	{
		fCurrent = _fCurrent;
		fMin = _fMin;
		fMax = _fMax;
	}

	void Setup(float _fCurrent, float _fMax)
	{
		fCurrent = _fCurrent;
		fMin = _fMax;
		fMax = _fMax;
	}

	bool Update(dword dwDeltaTime) { return Update(0.001f * float(dwDeltaTime)); }
	bool Update(float fDeltaTime) 
	{
		fCurrent -= fDeltaTime;
		if (fCurrent <= 0.0f)
		{
			fCurrent = fMin + FRAND(fMax - fMin);
			return true;
		}
		return false;
	}
};

#endif