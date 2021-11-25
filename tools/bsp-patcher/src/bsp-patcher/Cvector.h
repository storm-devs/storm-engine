#ifndef __CVECTOR_H__
#define __CVECTOR_H__

#include <math.h>

struct CVECTOR
{
public:
	float x;
	float y;
	float z;
	CVECTOR() {};
	CVECTOR(float a) { x = a;	y = a;	z = a; };
	CVECTOR(double a) { x = float(a);	y = float(a);	z = float(a); };
	CVECTOR(float _x, float _y, float _z) { x = _x;	y = _y;	z = _z; };
	CVECTOR(double _x, double _y, double _z) { x = float(_x);	y = float(_y);	z = float(_z); };
	CVECTOR(float a[3]) { x = a[0];	y = a[1];	z = a[2]; };
	CVECTOR(double a[3]) { x = float(a[0]);	y = float(a[1]);	z = float(a[2]); };
	//CVECTOR(const DVECTOR &d) : x((float)d.x), y((float)d.y), z((float)d.z){};

	inline CVECTOR& operator += (const CVECTOR& v)
	{
		x += v.x;	y += v.y;	z += v.z;
		return *this;
	};
	inline CVECTOR& operator -= (const CVECTOR& v)
	{
		x -= v.x;	y -= v.y;	z -= v.z;
		return *this;
	};
	inline CVECTOR& operator *= (const CVECTOR& v)
	{
		x *= v.x;	y *= v.y;	z *= v.z;
		return *this;
	};
	inline CVECTOR& operator /= (const CVECTOR& v)
	{
		x /= v.x;	y /= v.y;	z /= v.z;
		return *this;
	};

	//square magnitude
	friend inline float operator ~ (const CVECTOR& v)
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	};

	//normalize
	friend inline CVECTOR operator ! (const CVECTOR& v)
	{
		double sqrtRes = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		if (sqrtRes == 0) {
			return CVECTOR(0.0,0.0,0.0);
		}

		double len = 1.0 / sqrtRes;
		return CVECTOR(v.x * len, v.y * len, v.z * len);
	};

	//none
	friend inline CVECTOR operator + (const CVECTOR& v)
	{
		return v;
	};
	//add
	friend inline CVECTOR operator + (const CVECTOR& v1, const CVECTOR& v2)
	{
		return CVECTOR(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	};
	//negative
	friend inline CVECTOR operator - (const CVECTOR& v)
	{
		return CVECTOR(-v.x, -v.y, -v.z);
	};
	//sub
	friend inline CVECTOR operator - (const CVECTOR& v1, const CVECTOR& v2)
	{
		return CVECTOR(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	};
	//multiply each element by each
	friend inline CVECTOR operator * (const CVECTOR& v1, const CVECTOR& v2)
	{
		return CVECTOR(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	};
	//multiply each element by float
	friend inline CVECTOR operator * (const CVECTOR& v1, float f)
	{
		return CVECTOR(v1.x * f, v1.y * f, v1.z * f);
	};
	//divide each element by each
	friend inline CVECTOR operator / (const CVECTOR& v1, const CVECTOR& v2)
	{
		return CVECTOR(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
	};
	//divide each element by float
	friend inline CVECTOR operator / (const CVECTOR& v1, float f)
	{
		double _f = 1.0 / f;	return CVECTOR(v1.x * _f, v1.y * _f, v1.z * _f);
	};
	//cross product
	friend inline CVECTOR operator ^ (const CVECTOR& v1, const CVECTOR& v2)
	{
		return CVECTOR(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
	}
	//dot product
	friend inline float operator | (const CVECTOR& v1, const CVECTOR& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	//==
	inline bool operator==(const CVECTOR& v)
	{
		return (v.x == x && v.y == y && v.z == z);
	}

};

struct DVECTOR
{
public:
	double x;
	double y;
	double z;
	DVECTOR() {};
	DVECTOR(double a) { x = a;	y = a;	z = a; };
	DVECTOR(double _x, double _y, double _z) { x = _x;	y = _y;	z = _z; };
	DVECTOR(double a[3]) { x = a[0];	y = a[1];	z = a[2]; };
	DVECTOR(const CVECTOR& d) { x = d.x;	y = d.y;	z = d.z; };

	inline DVECTOR& operator += (const DVECTOR& v)
	{
		x += v.x;	y += v.y;	z += v.z;
		return *this;
	};
	inline DVECTOR& operator -= (const DVECTOR& v)
	{
		x -= v.x;	y -= v.y;	z -= v.z;
		return *this;
	};
	inline DVECTOR& operator *= (const DVECTOR& v)
	{
		x *= v.x;	y *= v.y;	z *= v.z;
		return *this;
	};
	inline DVECTOR& operator /= (const DVECTOR& v)
	{
		x /= v.x;	y /= v.y;	z /= v.z;
		return *this;
	};

	//square magnitude
	friend inline double operator ~ (const DVECTOR& v)
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	};

	//normalize
	friend inline DVECTOR operator ! (const DVECTOR& v)
	{
		double len = 1.0 / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		return DVECTOR(v.x * len, v.y * len, v.z * len);
	};

	//none
	friend inline DVECTOR operator + (const DVECTOR& v)
	{
		return v;
	};
	//add
	friend inline DVECTOR operator + (const DVECTOR& v1, const DVECTOR& v2)
	{
		return DVECTOR(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	};
	//negative
	friend inline DVECTOR operator - (const DVECTOR& v)
	{
		return DVECTOR(-v.x, -v.y, -v.z);
	};
	//sub
	friend inline DVECTOR operator - (const DVECTOR& v1, const DVECTOR& v2)
	{
		return DVECTOR(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	};
	//multiply each element by each
	friend inline DVECTOR operator * (const DVECTOR& v1, const DVECTOR& v2)
	{
		return DVECTOR(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	};
	//multiply each element by double
	friend inline DVECTOR operator * (const DVECTOR& v1, double f)
	{
		return DVECTOR(v1.x * f, v1.y * f, v1.z * f);
	};
	//divide each element by each
	friend inline DVECTOR operator / (const DVECTOR& v1, const DVECTOR& v2)
	{
		return DVECTOR(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
	};
	//divide each element by double
	friend inline DVECTOR operator / (const DVECTOR& v1, double f)
	{
		double _f = 1.0 / f;	return DVECTOR(v1.x * _f, v1.y * _f, v1.z * _f);
	};
	//cross product
	friend inline DVECTOR operator ^ (const DVECTOR& v1, const DVECTOR& v2)
	{
		return DVECTOR(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
	}
	//dot product
	friend inline double operator | (const DVECTOR& v1, const DVECTOR& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}
};

#endif
