#ifndef NET_NURBS_HPP
#define NET_NURBS_HPP

class Curve5
{
public:
	Curve5() {};
	~Curve5() {};

	__forceinline void SetLine(const CVECTOR & v0, const CVECTOR & v1, const CVECTOR & v2, const CVECTOR & v3, const CVECTOR & v4);
	__forceinline CVECTOR GetPoint(float fPos);

private:
	union
	{
		struct 
		{
			CVECTOR vP0;				// first point
			CVECTOR vkA, vkB, vkC, vkD;	// coeff
		};
		struct 
		{
			CVECTOR vK[5];
		};
	};
};

__forceinline void Curve5::SetLine(const CVECTOR & v0, const CVECTOR & v1, const CVECTOR & v2, const CVECTOR & v3, const CVECTOR & v4)
{
	vP0 = v0;
	/*vkA = 1.f/24*256 * v4  -  1.f/6*256 * v3  +  1.f/4*256 * v2  -  1.f/6*256 * v1  +  1.f/24*256 * v0;
	vkB = -1.f/4*64 * v4  +  7.f/6*64 * v3  -  2.f*64 * v2  +  3.f/2*64 * v1  -  5.f/12*64 * v0;
	vkC = 11.f/24*16 * v4  -  7.f/3*16 * v3  +  19.f/4*16 * v2  -  13.f/3*16 * v1  +  35.f/24*16 * v0;
	vkD = -1.f/4*4 * v4  +  4.f/3*4 * v3  -  3.f*4 * v2  +  4.f*4 * v1  -  25.f/12*4 * v0;*/
	vkA = 0.0f;
	vkB = 0.0f;
	vkC = 2.0f * v4 - 4.0f * v2 + 2.0f * v0;
	vkD = 4.0f * v2 - v4 - 3.0f * v0;
}

__forceinline CVECTOR Curve5::GetPoint(float fPos)
{
	return vP0 + fPos * fPos * fPos * fPos * vkA + fPos * fPos * fPos * vkB + fPos * fPos * vkC + fPos * vkD;
}

#endif