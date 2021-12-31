#include "t_dynamic_object.h"
#include "rands.h"

//--------------------------------------------------------------------
TDynamicObject::TDynamicObject()
{
}

//--------------------------------------------------------------------
TDynamicObject::~TDynamicObject()
{
}

//--------------------------------------------------------------------
void TDynamicObject::Initialize(const CVECTOR &_center, float _radius)
{
    pos.x = _center.x + randCentered(_radius);
    pos.y = 0.0f;
    pos.z = _center.z + randCentered(_radius);
    // ang = randCentered(PI*2.0);
    velocity = !CVECTOR(randCentered(1.0f), 0.0f, randCentered(1.0f));
}

//--------------------------------------------------------------------
void TDynamicObject::Calculate(TDynamicObject **a, int aCount, TDynamicObject **d, int dCount, float _k)
{
    // ang = fmod(ang, 2.0*PI);

    CVECTOR aEffect(0.0f, 0.0f, 0.0f);
    CVECTOR dEffect(0.0f, 0.0f, 0.0f);
    TDynamicObject **effectObject;
    int i;

    for (i = 0, effectObject = a; i < aCount; i++, effectObject++)
    {
        if (!(*effectObject) || (this == *effectObject))
            continue;

        aEffect += ATTRACT_FACTOR * ((*effectObject)->pos - pos);
    }

    for (i = 0, effectObject = d; i < dCount; i++, effectObject++)
    {
        if (!(*effectObject) || (this == *effectObject))
            continue;

        CVECTOR delta;
        delta = (*effectObject)->pos - pos;
        auto d = static_cast<float>(sqrt(~delta));
        if (d < 1e-10f)
            d = 1e-10f;
        dEffect += DEFLECT_FACTOR * !delta / d;
    }

    velocity = !(velocity + MOVEMENT_FACTOR * !(aEffect - dEffect));
    pos += _k * velocity;

    /*
      float dEffect = 0;
      for (i=0, effectObject = a; i<aCount; i++, effectObject++)
      {
        if (!effectObject || (this == effectObject))
          continue;

        effect = effectObject->pos - pos;
        float effectAng = atan2(effect.z, effect.x);
        dEffect += ATTRACT_FACTOR * ~effect;

        if (dEffect > MAX_ANG_DELTA)
          dEffect = MAX_ANG_DELTA;

        if (fabs(ang) < PId2)
        {
          if (effectAng > ang)
            ang += dEffect;
          else
            ang -= dEffect;
        }
        else
        {
          if (effectAng > ang)
            ang -= dEffect;
          else
            ang += dEffect;
        }
      }

        pos.x += cos(ang);
      pos.z += sin(ang);
      */
}

//--------------------------------------------------------------------
/*
void TDynamicObject::Draw(HDC _dc, DWORD _color)
{
    static int MAIN_LINE_LENGTH = 16;
    static int xOffset = 320;
    static int yOffset = 240;

    HPEN pen = CreatePen(PS_SOLID, 0, _color);
    SelectObject(_dc, pen);

    float ang = (float) atan2(velocity.z, velocity.x);

    int frontX = (int) (xOffset + pos.x + 0.5*cos(ang)*MAIN_LINE_LENGTH);
    int frontY = (int) (yOffset + pos.z + 0.5*sin(ang)*MAIN_LINE_LENGTH);

    int rearX = (int) (xOffset + pos.x + 0.5*cos(ang + PI)*MAIN_LINE_LENGTH);
    int rearY = (int) (yOffset + pos.z + 0.5*sin(ang + PI)*MAIN_LINE_LENGTH);

    int leftX = (int) (xOffset + pos.x + 0.1*cos(ang + PId2)*MAIN_LINE_LENGTH);
    int leftY = (int) (yOffset + pos.z + 0.1*sin(ang + PId2)*MAIN_LINE_LENGTH);

    int rightX = (int) (xOffset + pos.x + 0.1*cos(ang - PId2)*MAIN_LINE_LENGTH);
    int rightY = (int) (yOffset + pos.z + 0.1*sin(ang - PId2)*MAIN_LINE_LENGTH);

    MoveToEx(_dc, frontX, frontY, 0);
    LineTo(_dc, rearX, rearY);
    MoveToEx(_dc, frontX, frontY, 0);
    LineTo(_dc, leftX, leftY);
    MoveToEx(_dc, frontX, frontY, 0);
    LineTo(_dc, rightX, rightY);

    DeleteObject(pen);
}
*/
//--------------------------------------------------------------------
void TDynamicObject::SetXYZ(const CVECTOR &_pos)
{
    pos = _pos;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
