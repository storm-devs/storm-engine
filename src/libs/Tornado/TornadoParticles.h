//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	TornadoParticles
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _TornadoParticles_H_
#define _TornadoParticles_H_

#include "Matrix.h"
#include "Pillar.h"
#include "sea_base.h"

class TornadoParticles
{
    struct Particle
    {
        CVECTOR pos;
        float angle;
        float size;
        float alpha;
        float k;
    };

    struct PillarParticle : public Particle
    {
        float ang;
        float sz;
    };

    struct GroundParticle : public Particle
    {
        float a;  //Угл в радиальной сисеме координат
        float r;  //Радиус начала движения
        float t;  //Относительное время движения
        float dt; //Относительное время движения
        float p;  //Степень искривлённости траектории
    };

    struct Vertex
    {
        CVECTOR pos;
        uint32_t color;
        float u, v;
    };

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    TornadoParticles(Pillar &_pillar);
    virtual ~TornadoParticles();

    void SetSea();

    void Update(float dltTime);
    void Draw(VDX9RENDER *rs);

    long txtPillarPrts;
    long txtGroundPrts;

    void SetGlobalAlpha(float a);

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  private:
    void DrawParticles(VDX9RENDER *rs, void *prts, long num, long size, long texture, const char *tech);
    static float GetRand(float r, float dev = 0.5f); // dev = 0..1, return 0..r

  private:
    Pillar &pillar;
    entid_t seaID;
    float galpha;
    CMatrix camMtx;
    GroundParticle groundPrt[128];
    PillarParticle pillarPrt[1024];
    Vertex buffer[256 * 6];
};

// kUse = 0..1, return 0..r
inline float TornadoParticles::GetRand(float r, float dev)
{
    return r * (1.0f - dev + rand() * dev / RAND_MAX);
}

#endif
