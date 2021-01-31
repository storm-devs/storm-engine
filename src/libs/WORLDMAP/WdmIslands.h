//============================================================================================
//	Spirenkov Maxim, 2001
//--------------------------------------------------------------------------------------------
//	Sea Dogs II
//--------------------------------------------------------------------------------------------
//	WdmIslands
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _WdmIslands_H_
#define _WdmIslands_H_

#include "PtcData.h"
#include "WdmRenderModel.h"
#include "geometry.h"

class WdmIslandWaves : public WdmRenderModel
{
  public:
    WdmIslandWaves();

    void Update(float dltTime) override;
    void LRender(VDX9RENDER *rs) override;
    void Render(VDX9RENDER *rs, float k);

  private:
    float phase;
};

class WdmIslands : public WdmRenderObject
{
    struct Islands
    {
        WdmRenderModel *model; //Моделька острова
        WdmRenderModel *area;  //Моделька области острова
        WdmRenderModel *palms; //Моделька с пальмами
        WdmIslandWaves *waves; //Моделька с пеной
        CMatrix toLocal;       //Преобразование в локальную систему острова
        std::string modelName; //Имя модельки острова
        CVECTOR worldPosition; //Позиция острова в мире
    };

    struct Label
    {
        std::string text; //Текст метки
        CVECTOR pos;      //Позиция метки
        float l, t, r, b; //Прямоугольник, описывающий метку в экранных координатах
        float dl, dt, dr, db; //Смещения, чтобы получить прямоугольник, при известной точке на экране
        float textX, textY;   //Относительная позиция текста
        float iconX, iconY;   //Относительная позиция картинки
        float alpha;          //Текущее состояние прямоугольника
        float heightView;     //Высота начиная с которой метка гаснет
        long font;            //Индекс шкифта в массиве шрифтов
        long icon;            //Индекс картинки
        uint32_t weight;      //Вес смещения
        std::string id;       //Идентификатор метки
        uint32_t idHash;      //Хэшь значение идентификатора
        long next;            //Следующая в списке метка
        std::string locatorName; //Имя локатора на котором размещаемся
    };

    struct Font
    {
        std::string name; //Имя шрифта
        long id;          //Его идентификатор
    };

    struct Icons
    {
        float w, h;
        float u, v;
        uint32_t num;
        uint32_t frames;
        float fps;
        float frame;
        float f[2];
        long blend;
        long texture;
    };

    struct Quest
    {
        CVECTOR pos;
        std::string name;
    };

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    WdmIslands(WdmIslands &&) = delete;
    WdmIslands(const WdmIslands &) = delete;
    WdmIslands();
    virtual ~WdmIslands();

    //Проверить на возможное столкновение
    bool CollisionTest(CMatrix &objMtx, float length, float width, bool heighTest = true);
    //Проверить наличие в данном месте треугольников
    bool ObstacleTest(float x, float z, float radius);

    //Зачитать данные об островах
    void SetIslandsData(ATTRIBUTES *apnt, bool isChange);

    //Найти направление для прибытия в заданную точку назначения из текущей
    void FindDirection(const CVECTOR &position, const CVECTOR &destination, CVECTOR &direction) const;
    //Найти силу отталкивания
    void FindReaction(const CVECTOR &position, CVECTOR &reaction) const;
    //Найти случайную точку для мерчанта
    bool GetRandomMerchantPoint(CVECTOR &p);
    //Получить координаты квестового локатора
    bool GetQuestLocator(const char *locName, CVECTOR &p);

    //Проверить попадание кораблика в зону острова
    bool CheckIslandArea(const char *islandName, float x, float z);
    //Получить ближайшую точку к зоне острова
    void GetNearPointToArea(const char *islandName, float &x, float &z);

    void Update(float dltTime) override;
    void LRender(VDX9RENDER *rs) override;

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    bool IsShipInArea(long islIndex, const CVECTOR &pos);
    static bool AddEdges(const GEOS::VERTEX *vrt, long numVrt);
    static bool FindNearPoint(const GEOS::VERTEX *vrt, long numVrt);
    void LabelsReadIconParams(ATTRIBUTES *apnt);
    long LabelsFind(const char *id, uint32_t hash);
    bool LabelsFindLocator(const char *name, CVECTOR &pos) const;
    long LabelsAddFont(const char *name);
    void LabelsRelease();
    static CVECTOR Norm2D(const CVECTOR &ret);

  private:
    //Модель, содержащая все локаторы
    WdmRenderModel *baseModel;
    //Патч для поиска пути
    PtcData *patch;
    //Модели островов
    std::vector<Islands> islands;
    //Метки
    std::vector<Label> labels;
    //Шрифты используемые метками
    std::vector<Font> fonts;
    //Картинки
    Icons icons;
    //Зарегистрированные на отрисовку метки
    std::vector<long> labelSort;
    //Точки места назначения мерчантов
    std::vector<CVECTOR> merchants;
    //Точки места назначения квестовых энкоунтеров
    std::vector<Quest> quests;

    //Входная таблица для поиска меток
    long labelsEntry[1024];

    static CMatrix curMatrix, locMatrix;
    static long numEdges;
    static CVECTOR curPos;
    static bool checkMode;

  public:
    static CVECTOR centPos;
};

inline CVECTOR WdmIslands::Norm2D(const CVECTOR &v)
{
    auto ret = v;
    ret.y = 0.0f;
    double len = ret.x * ret.x + ret.z * ret.z;
    if (len >= 1e-30f)
    {
        len = 1.0f / sqrt(len);
        ret.x = static_cast<float>(len * ret.x);
        ret.z = static_cast<float>(len * ret.z);
    }
    else
    {
        ret = 0.0f;
    }
    return ret;
}

#endif
