#include "t_sink_splash.h"

#include "math_inlines.h"
#include "sink_splash_defines.h"

#include "rands.h"

//--------------------------------------------------------------------
TSinkSplash::TSinkSplash()
    : ps(nullptr), enabled(false), sea(nullptr), time(0), distortDivider(0), center(), dir(), growK(0)
{
}

//--------------------------------------------------------------------
TSinkSplash::~TSinkSplash()
{
    Release();
}

//--------------------------------------------------------------------
void TSinkSplash::Initialize(INIFILE *_ini, SEA_BASE *_sea)
{
    // ps = new SEPS_PS();
    // ps->Init(_ini, "sink");

    sea = _sea;
}

//--------------------------------------------------------------------
void TSinkSplash::Release()
{
    // if (ps)
    //    delete ps;
}

//--------------------------------------------------------------------
void TSinkSplash::Start(const CVECTOR &_pos, uint16_t *_indexes, SINK_VERTEX *_vertexes, int32_t vOffset)
{
    time = 0;
    center = _pos;
    enabled = true;
    growK = rand(1.5f) + randUpper(0.1f);

    const auto midY = sea->WaveXZ(center.x, center.z) + sink_effect::SPLASH_MOVE_Y;
    distortDivider = rand(sink_effect::SPLASH_DISTORT_DIVIDER) + 10.0f;
    //    _VSYSTEM_API->Trace("i=%X, v=%X", _indexes, _vertexes);
    const auto stepSize = sink_effect::GRID_LENGTH / sink_effect::GRID_STEPS;
    const auto halfSize = sink_effect::GRID_LENGTH / 2.0f;
    for (uint16_t z = 0; z < sink_effect::GRID_STEPS; ++z)
        for (uint16_t x = 0; x < sink_effect::GRID_STEPS; ++x)
        {
            _vertexes->pos.x = center.x + stepSize * x - halfSize;
            _vertexes->pos.z = center.z + stepSize * z - halfSize;
            _vertexes->pos.y = midY;
            _vertexes->tu = static_cast<float>(x) / static_cast<float>(sink_effect::GRID_STEPS);
            _vertexes->tv = static_cast<float>(z) / static_cast<float>(sink_effect::GRID_STEPS);
            //_vertexes->color = 0x44FFFF00;
            ++_vertexes;
        }

    for (uint16_t z = 0; z < sink_effect::GRID_STEPS - 1; ++z)
        for (uint16_t x = 0; x < sink_effect::GRID_STEPS - 1; ++x)
        {
            *(_indexes++) = vOffset + sink_effect::GRID_STEPS * z + x;
            *(_indexes++) = vOffset + sink_effect::GRID_STEPS * (z + 1) + x;
            *(_indexes++) = vOffset + sink_effect::GRID_STEPS * (z + 1) + x + 1;

            *(_indexes++) = vOffset + sink_effect::GRID_STEPS * z + x;
            *(_indexes++) = vOffset + sink_effect::GRID_STEPS * (z + 1) + x + 1;
            *(_indexes++) = vOffset + sink_effect::GRID_STEPS * z + x + 1;
        }

    // ps->Reset();
}

//--------------------------------------------------------------------
float TSinkSplash::HeightF(uint32_t _time, float _r, float _k)
{
    auto tPhase = static_cast<float>(_time) * (PI / sink_effect::SPLASH_FADE_TIME);

    auto fCos = 1 / (_r + sink_effect::SPLASH_START_ARG + _time / distortDivider);
    const auto secondPart = ((sink_effect::GRID_LENGTH - _r) / sink_effect::GRID_LENGTH) *
                            (1.0f + fabs(sin((-PI * static_cast<float>(_time) / sink_effect::SPLASH_FADE_TIME) +
                                             PIm2 * _r / sink_effect::GRID_LENGTH)));
    auto k = static_cast<float>(_time) / sink_effect::SPLASH_FADE_TIME;
    if (k > 1.0f)
        k = 1.0f;
    return _k * (sink_effect::SPLASH_HEIGHT_MULTIPLIER * (/*(1.0f-k)*fCos*sin(tPhase) +*/ k * secondPart));
}

//--------------------------------------------------------------------
void TSinkSplash::Process(uint32_t _dTime, uint16_t *_indexes, SINK_VERTEX *_vertexes)
{
    if (!enabled)
        return;

    const auto alpha = rand(PI * 2);
    auto rho = randUpper(sink_effect::MAX_RHO);
    const auto r = sink_effect::SPLASH_RADIUS;
    static CVECTOR pos;
    static CVECTOR localDir;
    pos = center;
    pos.x += r * cos(alpha);
    pos.z += r * sin(alpha);
    localDir.x = sin(rho) * cos(alpha);
    localDir.y = 1.0f;
    localDir.z = sin(rho) * sin(alpha);
    // localDir += /*(((float) time) / (2 * SPLASH_FADE_TIME))**/this->dir;
    // ps->SetEmitter(pos, localDir);

    time += _dTime;

    if (time > 3 * sink_effect::SPLASH_FADE_TIME)
    {
        enabled = false;
        return;
    }

    const auto midY = sea->WaveXZ(center.x, center.z) + sink_effect::SPLASH_MOVE_Y;
    const auto stepSize = static_cast<float>(sink_effect::GRID_LENGTH) / static_cast<float>(sink_effect::GRID_STEPS);
    const auto halfSize = sink_effect::GRID_LENGTH / 2.0f;
    CVECTOR curPos;
    int x, z;
    float localHeight, prolongK;

    for (z = 0; z < sink_effect::GRID_STEPS; ++z)
        for (x = 0; x < sink_effect::GRID_STEPS; ++x)
        {
            const auto curPosNormed = !CVECTOR(stepSize * x - halfSize, 0.0f, stepSize * z - halfSize);
            prolongK = curPosNormed.x * this->dir.x + curPosNormed.z * this->dir.z;
            curPos.x = (center.x + stepSize * x) - halfSize;
            curPos.z = (center.z + stepSize * z) - halfSize;

            _vertexes->pos.x = center.x + growK * (stepSize * x * prolongK - halfSize);
            _vertexes->pos.z = center.z + growK * (stepSize * z * prolongK - halfSize);
            _vertexes->color = ((((3 * sink_effect::SPLASH_FADE_TIME - static_cast<int32_t>(time))) << 8) /
                                (3 * sink_effect::SPLASH_FADE_TIME))
                               << 24;

            if ((x > 0) && (z > 0) && (x < sink_effect::GRID_STEPS - 1) && (z < sink_effect::GRID_STEPS - 1))
            {
                rho =
                    sqrt((curPos.x - center.x) * (curPos.x - center.x) + (curPos.z - center.z) * (curPos.z - center.z));
                localHeight = HeightF(time, rho, (sink_effect::GRID_LENGTH - rho) / sink_effect::GRID_LENGTH);

                _vertexes->pos.x = curPos.x;
                _vertexes->pos.y = midY + growK * localHeight;
                _vertexes->pos.z = curPos.z;
            }
            else
            {
                _vertexes->pos.x = curPos.x;
                _vertexes->pos.y = midY;
                _vertexes->pos.z = curPos.z;
            }

            ++_vertexes;
        }
}

//--------------------------------------------------------------------
bool TSinkSplash::Enabled()
{
    return enabled;
}

//--------------------------------------------------------------------
void TSinkSplash::Reset(uint16_t *_indexes, SINK_VERTEX *_vertexes)
{
    time = 0;
    enabled = false;
    /*
      float midY = sea->WaveXZ(center.x, center.z) + SPLASH_MOVE_Y;
      float stepSize = GRID_LENGTH / GRID_STEPS;
      float halfSize = GRID_LENGTH / 2.0f;
      int x,z;
      for (z=0; z < GRID_STEPS; ++z)
      for (x=0; x < GRID_STEPS; ++x)
      {
        _vertexes->pos.x = center.x + stepSize * x - halfSize;
        _vertexes->pos.z = center.z + stepSize * z - halfSize;
        _vertexes->pos.y = midY;
        _vertexes->tu = ((float) x) / ((float) GRID_STEPS);
        _vertexes->tv = ((float) z) / ((float) GRID_STEPS);
        ++_vertexes;
      }
    */
}

//--------------------------------------------------------------------
void TSinkSplash::AdditionalRealize(uint32_t dTime)
{
    // ps->Realize(_dTime);
}

//--------------------------------------------------------------------
