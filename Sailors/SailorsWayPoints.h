
#include "..\common_h\dx8render.h"
#include "..\common_h\templates\string.h"
#include "..\common_h\templates\array.h"
#include "..\common_h\matrix.h"


//#include "d3dx8math.h"

const int MAX_POINTS= 100;

enum PointType { PT_TYPE_NORMAL, PT_TYPE_CANNON_L,
								 PT_TYPE_CANNON_R,
								 PT_TYPE_CANNON_F,
								 PT_TYPE_CANNON_B,

								 PT_TYPE_MAST_1,
								 PT_TYPE_MAST_2,
								 PT_TYPE_MAST_3,
								 PT_TYPE_MAST_4, 
								 PT_TYPE_MAST_5, PT_TYPE_NOT_TARGETPOINT };




#define COLOR_SHADOW           D3DCOLOR_ARGB(150,0,0,0);
#define COLOR_SHADOW_SELECTED  D3DCOLOR_ARGB(200,150,150,150);
#define COLOR_POINT            D3DCOLOR_ARGB(255,200,200,0);
#define COLOR_GRAY             D3DCOLOR_ARGB(200, 200,200,200);
#define COLOR_SELECTED         0xFFFFFFFF;
//-----------------------------------------------------------------------------------------------
struct Path{
  byte  length;                                            // Длина цепочки (кол-во элементов)
  float   min;                                             // Значение пути
  byte  point[MAX_POINTS];                                 // Последовательность обхода
  int currentPointPosition;								   // Текущее положение в пути

  Path(){
	length= 0;
	min= -1;
	currentPointPosition= -1;
  };


};

//-----------------------------------------------------------------------------------------------
struct Link{
	int first,next;
};
//-----------------------------------------------------------------------------------------------

class Links
{
public:
	array <Link> link;
	int selected;
	int count;

	void Add();
	void Delete(int Index);

	Links() : link(_FL_) {
		selected= -1;
		count= 0;
	};
};
//-----------------------------------------------------------------------------------------------


struct Point{

	float		x,y,z;
	PointType   pointType;

	bool  buisy;
	bool  disabled;
	bool  cannonReloaded;

	int climbPosition; //Если на мачте более одного человека

	Point(){

		pointType= PT_TYPE_NORMAL;
		buisy= false;
		disabled= false;
		cannonReloaded= true;
		climbPosition= 0;
		x= 0;
		y= 8;
		z= 0;
	};

	bool IsMast();
	bool IsCannon();
	bool IsNormal();
};


//-----------------------------------------------------------------------------------------------


struct Points
{
	array <Point> point;
	
	int count;
	int selected;

	void Add();
	void Delete(int Index);

	Points() : point(_FL_) { count= 0; selected= -1; }
};

//-----------------------------------------------------------------------------------------------


class SailorsPoints
{
private:
	bool  PointsPassed[MAX_POINTS];            //tmp отметка пройденных точек(для поиска пути)
	float matrix[MAX_POINTS][MAX_POINTS];		// Матрица для быстрого поиска пути

	Path getPath(int src, int dst, int l);   // Поиск пути

public:
	Points points;
	Links  links;

	void Draw(VDX8RENDER * rs, bool pointmode);
	void Draw_(VDX8RENDER * rs, bool pointmode);
	void DrawLinks(VDX8RENDER * rs);

    Path findPath(Path &path, int from, int to); // Посчитать путь
   

	void UpdateLinks(); //Обновить матрицу поиска пути

	int WriteToFile(string fileName);
	int ReadFromFile(string fileName);
};


//-------------------------------------------------------------------------------------



inline float Dest( const CVECTOR &_v1, const CVECTOR &_v2){

	return float(sqrt( (_v2.x- _v1.x)*(_v2.x- _v1.x)+ (_v2.y- _v1.y)*(_v2.y- _v1.y)+ (_v2.z- _v1.z)*(_v2.z- _v1.z)));
};


inline bool Dest( const CVECTOR &_v1, const CVECTOR &_v2, float d){


	return (abs(_v2.x- _v1.x)< d && abs(_v2.y- _v1.y)< d && abs(_v2.z- _v1.z)< d);
};


inline float Vector2Angle(const CVECTOR &_v)
{
	float result= (float) atan2(_v.x, _v.z);

	while (result>= PI*2) result-= PI*2;
	while (result<  0)    result+= PI*2;

	return result;
} 