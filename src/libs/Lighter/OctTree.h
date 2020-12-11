//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	OctTree
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _OctTree_H_
#define _OctTree_H_

#include "LGeometry.h"

class OctTree
{

    struct OTNode
    {
        OTNode(const CVECTOR &_min, const CVECTOR &_max);
        ~OTNode();
        OTNode *node[8]; //Дети
        CVECTOR min;     //Минимальное значение
        CVECTOR max;     //Максимальное значение
        Vertex **vrt;    //Индексы вершин
        long num;        //Количество вершин
    };

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    OctTree();
    virtual ~OctTree();

    //Инициализировать дерево
    void Init(LGeometry *g);
    //Найти вершины в заданном радиусе
    void FindVerts(const CVECTOR &pos, float r);

    std::vector<OctFndVerts> verts;
    long numVerts;
    long maxVerts;

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    //Добавление вершин
    bool AddVertex(OTNode *node, Vertex *v);
    //Оптимизация дерева
    void Optimize(OTNode *node);
    //Поиск
    void FindVerts(OTNode *node);

    long Check(OTNode *node, Vertex *v, long num);

  private:
    Vertex *vrt;
    long numVrt;
    OTNode *root;
    CVECTOR vertsPos, vertsPosMin, vertsPosMax;
    float vertsR;
};

#endif
