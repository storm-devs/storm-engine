//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// Line
//============================================================================================

#ifndef _Line_h_
#define _Line_h_

#pragma pack(push, 1)

///Класс представления линии в 3D пространстве
class Line
{
public:
	union
	{
		struct
		{
			union
			{
				struct
				{
					///Начальная позиция линии
					Vector p1;
				};
				struct
				{
					///Начальная позиция линии
					Vector src;
				};
				struct
				{
					///Начальная позиция линии
					Vector start;
				};
				struct
				{
					///Начальная позиция линии
					Vector s;
				};
			};
			union
			{
				struct
				{
					///Конечная позиция линии
					Vector p2;
				};
				struct
				{
					///Конечная позиция линии
					Vector dst;
				};
				struct
				{
					///Конечная позиция линии
					Vector end;
				};
				struct
				{
					///Конечная позиция линии
					Vector e;
				};
			};
		};
		struct
		{
			///Представление в виде массива от 0->1
			Vector p[2];
		};
	};

//-----------------------------------------------------------
//Конструкторы
//-----------------------------------------------------------
public:
	///Пустой конструктор
	Line();
	///Заполнить числом
	Line(float f);
	///Заполнить векторами
	Line(const Vector & start, const Vector & end);


//-----------------------------------------------------------
//Утилитные
//-----------------------------------------------------------
public:
	//Найти точку пересечения с плоскостью
	bool Intersection(Plane & plane) const;
	//Найти точку пересечения с плоскостью
	bool Intersection(Plane & plane, Vector & point) const;
	//Найти точку пересечения с прямых, проходящих через линии
	bool IntersectionLines(const Line & line, Vector & point, float eps = 0.001f) const;
	//Найти точку пересечения с прямых, проходящих через линии
	bool IntersectionLinesXZ(const Line & line, Vector & point) const;
	//Найти расстояние от точки до прямой, проходящей через линию
	float DistanceToLine(const Vector & point) const;
	//Найти расстояние от точки до прямой, проходящей через линию
	float DistanceToLineXZ(const Vector & point) const;

	//Получить нормализованное направление прямой
	Vector Direction() const;
};

//===========================================================
//Конструкторы
//===========================================================

//Пустой конструктор
mathinline Line::Line(){}

//Заполнить числом
mathinline Line::Line(float f)
{
	p1 = 0.0f;
	p2 = 0.0f;
}

//Заполнить векторами
mathinline Line::Line(const Vector & start, const Vector & end)
{
	p1 = start;
	p2 = end;
}

//===========================================================
//Утилитные
//===========================================================

//Найти точку пересечения с плоскостью
mathinline bool Line::Intersection(Plane & plane) const
{
	return plane.Intersection(p1, p2);
}

//Найти точку пересечения с плоскостью
mathinline bool Line::Intersection(Plane & plane, Vector & point) const
{
	return plane.Intersection(p1, p2, point);
}

//Найти точку пересечения прямых, проходящих через линии
mathinline bool Line::IntersectionLines(const Line & line, Vector & point, float eps) const
{
	//Направляющие прямых
	Vector dir1 = dst - src;
	Vector dir2 = line.dst - line.src;
	//Плоскость образованая line и перпендикулярным отрезком между прямыми
	Plane plane((dir1 ^ dir2) ^ dir2);
	if(plane.normal.Normalize() <= 1e-30f) return false;
	plane.Move(line.src);
	//Точка пересечения отрезка
	float k;
	if(!plane.IntersectionLine(src, dst, k)) return false;
	Vector p = src + (dst - src)*k;
	k = line.DistanceToLine(p);
	if(k < 0.0f || k > eps) return false;
	point = p;
	return true;
}

//Найти точку пересечения с прямых, проходящих через линии
mathinline bool Line::IntersectionLinesXZ(const Line & line, Vector & point) const
{
	Line l1 = *this; l1.src.y = 0.0f; l1.dst.y = 0.0f;
	Line l2 = line; l2.src.y = 0.0f; l2.dst.y = 0.0f;
	return l1.IntersectionLines(l2, point);
}

//Найти расстояние от точки до прямой, проходящей через линию
mathinline float Line::DistanceToLine(const Vector & point) const
{
	Plane plane(dst - src);
	if(plane.n.Normalize() <= 1e-30f) return -1.0f;
	plane.Move(point);
	float k;
	if(!plane.IntersectionLine(src, dst, k)) return -1.0f;
	return (src + (dst - src)*k - point).GetLength();
}

//Найти расстояние от точки до прямой, проходящей через линию
mathinline float Line::DistanceToLineXZ(const Vector & point) const
{
	Vector p = point; p.y = 0.0f;
	Line l1 = *this; l1.src.y = 0.0f; l1.dst.y = 0.0f;
	return l1.DistanceToLine(p);
}

//Получить нормализованное направление прямой
mathinline Vector Line::Direction() const
{
	return !(dst - src);
}

#pragma pack(pop)

#endif
