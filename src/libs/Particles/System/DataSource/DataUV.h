#ifndef _PARTICLE_DATA_UV_H_
#define _PARTICLE_DATA_UV_H_

#include "../../ICommon/MemFile.h"

#include "math3d.h"
#include <string>
#include <vector>

class DataUV
{
    std::string Name;

    std::vector<Vector4> Frames;

  public:
    //конструктор/деструктор
    DataUV();
    virtual ~DataUV();

    //Получить значение   [ x,y = UV1; z,w = UV2 ]
    const Vector4 &GetValue(uint32_t FrameNum);

    //Установить значения
    void SetValues(const Vector4 *_Frames, uint32_t FramesCount);

    //Получить кол-во кадров
    uint32_t GetFrameCount() const;

    void Load(MemFile *File);
    void Write(MemFile *File);

    void SetName(const char *szName);
    const char *GetName() const;
};

#endif
