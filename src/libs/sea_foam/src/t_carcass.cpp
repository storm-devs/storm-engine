#include "t_carcass.h"

#include "math_inlines.h"
#include "seafoam_defines.h"

#define FRONT_FADE_LEVEL 3
#define V_DIAGONAL_K 0.5f
#define WIDEN_K 3.0f

//--------------------------------------------------------------------
TCarcass::TCarcass(int _levelsCount, int _measurePointsCount, VDX9RENDER *_renderer, bool _normalsInverted /*= false*/)
    : normalsInverted(_normalsInverted), levelsCount(_levelsCount), ivElementIndex(0), indexesCreated(false), time(0),
      vSpeed(0.f), speedA(0.f), renderer(_renderer)
{
    BOUND_UPPER(levelsCount, MAX_LEVELS);

    measure.pointsCount = _measurePointsCount;
    BOUND_UPPER(measure.pointsCount, MAX_MEASURE_POINTS);

    uSpeed = 15e-5f;
    iBuffer = renderer->CreateIndexBuffer(3 * 2 * (MEASURE_POINTS - 1) * (TRACE_STEPS_Z - 1) * sizeof(uint16_t));
    vBuffer = renderer->CreateVertexBuffer(
        CARCASS_VERTEX_FORMAT, MEASURE_POINTS * (TRACE_STEPS_Z + 1) * sizeof(tCarcassVertex), D3DUSAGE_WRITEONLY);
}

//--------------------------------------------------------------------
TCarcass::~TCarcass()
{
    if (vBuffer)
        renderer->ReleaseVertexBuffer(vBuffer);
    if (iBuffer)
        renderer->ReleaseIndexBuffer(iBuffer);
}

//--------------------------------------------------------------------
void TCarcass::SetSpeed(float _uSpeed, float _vSpeed, float _speedA)
{
    uSpeed = _uSpeed;
    vSpeed = _vSpeed;
    speedA = _speedA;
}

//--------------------------------------------------------------------
void TCarcass::InitCircleMeasure(float _d, float _kx, float _ky)
{
    float t, tStep, x, y;

    t = 0;
    tStep = PI / (measure.pointsCount - 1);
    for (auto i = 0; i < measure.pointsCount; i++)
    {
        x = (cosf(t) + 1) * 0.5f * _d;
        y = sinf(t + 0.2f) * 0.5f * _d;
        measure.deltaPointX[i] = x * _kx;
        measure.deltaPointY[i] = y * _ky;
        t += tStep;
    }
}

//--------------------------------------------------------------------
void TCarcass::RebuildIndexes(uint16_t *_iBuffer)
{
    auto i = 0;

    if (!normalsInverted)
    {
        for (auto level = 0; level < (levelsCount - 1); level++)
        {
            for (auto leftMeasure = 0; leftMeasure < (measure.pointsCount - 1); leftMeasure++)
            {
                _iBuffer[i++] = measure.pointsCount * level + leftMeasure;
                _iBuffer[i++] = measure.pointsCount * (level + 1) + leftMeasure + 1;
                _iBuffer[i++] = measure.pointsCount * (level + 1) + leftMeasure;

                _iBuffer[i++] = measure.pointsCount * level + leftMeasure;
                _iBuffer[i++] = measure.pointsCount * level + leftMeasure + 1;
                _iBuffer[i++] = measure.pointsCount * (level + 1) + leftMeasure + 1;
            }
        }
    }
    else
    {
        for (auto level = 0; level < (levelsCount - 1); level++)
        {
            for (auto leftMeasure = 0; leftMeasure < (measure.pointsCount - 1); leftMeasure++)
            {
                _iBuffer[i++] = measure.pointsCount * level + leftMeasure;
                _iBuffer[i++] = measure.pointsCount * (level + 1) + leftMeasure;
                _iBuffer[i++] = measure.pointsCount * (level + 1) + leftMeasure + 1;

                _iBuffer[i++] = measure.pointsCount * level + leftMeasure;
                _iBuffer[i++] = measure.pointsCount * (level + 1) + leftMeasure + 1;
                _iBuffer[i++] = measure.pointsCount * level + leftMeasure + 1;
            }
        }
    }
}

//--------------------------------------------------------------------
void TCarcass::RebuildLevels(tCarcassVertex *_vBuffer, const CVECTOR *_starts, uint32_t _dTime)
{
    if (vertices.empty())
    {
        vertices.resize(levelsCount * measure.pointsCount);

        float lengthK = 0.2f * sqrtf(~(_starts[(levelsCount - 1) /**measure.pointsCount*/] - _starts[0]));

        for (auto level = 0; level < levelsCount; level++)
        {
            const auto levelK = static_cast<float>(level) / (levelsCount - 1);
            for (auto measurePoint = 0; measurePoint < measure.pointsCount; measurePoint++)
            {
                tCarcassVertex* tempVertex = &vertices[level * measure.pointsCount + measurePoint];

                float heightK;
                if (level <= FRONT_FADE_LEVEL)
                    heightK = 1.f + 1.35f * static_cast<float>(level) / FRONT_FADE_LEVEL;
                else
                    heightK = 1.f + (1.f - levelK);

                tempVertex->pos = CVECTOR((1.f + WIDEN_K * levelK) * measure.deltaPointX[measurePoint],
                                          heightK * measure.deltaPointY[measurePoint], 0.f);
                tempVertex->pos.y += .01f;

                auto measureK = 0.f;
                if (!measurePoint)
                    measureK = 1.f;

                auto levelA = 1.f;
                if (level <= FRONT_FADE_LEVEL)
                    levelA = static_cast<float>(level) / FRONT_FADE_LEVEL;
                else
                    levelA = (1.f - levelK);

                auto a = speedA * sqrtf(levelA) * (1.f - measureK);
                tempVertex->color = (static_cast<uint32_t>(a * 0xFF) << 24) | 0x00FFFFFF;
                tempVertex->u = lengthK * (1.f - levelK);
                tempVertex->v = 1.f - static_cast<float>(measurePoint) / (measure.pointsCount - 1) + tempVertex->u * V_DIAGONAL_K;
            }
        }
    }
    else
    {
        const auto dv = fmodf(_dTime * vSpeed, 1.f);
        const auto du = fmodf(_dTime * uSpeed, 1.f);
        for (auto index = 0; index < levelsCount * measure.pointsCount; index++)
        {
            tCarcassVertex* tempVertex = &vertices[index];
            tempVertex->u = tempVertex->u + du;
            tempVertex->v = tempVertex->v - dv;
        }
    }

    std::vector<tCarcassVertex> tempVertices(vertices);

    for (auto level = 0; level < levelsCount; level++)
    {
        for (auto measurePoint = 0; measurePoint < measure.pointsCount; measurePoint++)
        {
            tCarcassVertex* tempVertex = &tempVertices[level * measure.pointsCount + measurePoint];
            tempVertex->pos += _starts[level];
        }
    }

    memcpy(_vBuffer, tempVertices.data(), levelsCount * measure.pointsCount * sizeof(tCarcassVertex));
}

//--------------------------------------------------------------------
bool TCarcass::Initialize()
{
    return true;
}

//--------------------------------------------------------------------
void TCarcass::Uninitialize()
{
}

//--------------------------------------------------------------------
void TCarcass::Execute(uint32_t _dTime, CMatrix &_mtx, const CVECTOR *_starts)
{
    sceneMatrix = _mtx;

    auto *vBufferPointer = static_cast<tCarcassVertex *>(renderer->LockVertexBuffer(vBuffer));
    auto *iBufferPointer = static_cast<uint16_t *>(renderer->LockIndexBuffer(iBuffer));

    if (!indexesCreated)
    {
        RebuildIndexes(iBufferPointer);
        indexesCreated = true;
    }

    RebuildLevels(vBufferPointer, _starts, _dTime);

    renderer->UnLockVertexBuffer(vBuffer);
    renderer->UnLockIndexBuffer(iBuffer);
    time += _dTime;
}

//--------------------------------------------------------------------
void TCarcass::Realize(const char *_technique)
{
    renderer->SetTransform(D3DTS_WORLD, sceneMatrix);
    renderer->DrawBuffer(vBuffer, sizeof(tCarcassVertex), iBuffer, 0, MEASURE_POINTS * (TRACE_STEPS_Z + 1), 0,
                         2 * (MEASURE_POINTS - 1) * (TRACE_STEPS_Z - 1), const_cast<char *>(_technique));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
