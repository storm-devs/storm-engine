#pragma once

#include <chrono>

#define WindFieldSize 64
#define WindFieldSteps 64
#define WindFieldUpdateTime 0.1f

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

class WindField
{
    enum CurrentStep
    {
        cs_initors,
        cs_copy1,
        cs_lines,
        cs_copy2,
    };

    struct Point
    {
        Point()
        {
            x = y = 0.0f;
            dx = dy = 0.0f;
        };

        float x, y;
        float dx, dy;
    };

    struct Wind
    {
        float x, z;

        void Lerp(const Wind &w1, const Wind &w2, float k)
        {
            x = w1.x + (w2.x - w1.x) * k;
            z = w1.z + (w2.z - w1.z) * k;
        };
    };

    struct Direction
    {
        float x, y;
        int32_t i, j;
    };

  public:
    // Constructing a field on a given area
    WindField()
    {
        const auto minX = 0.0f, minZ = 0.0f, maxX = 1.0f, maxZ = 1.0f;
        baseX = minX;
        baseZ = minZ;
        kX = (WindFieldSize - 2) / (maxX - minX);
        kZ = (WindFieldSize - 2) / (maxZ - minZ);
        updateTime = 0.0f;
        step = cs_initors;
        srand(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
        steps = WindFieldSteps;
        curLine = -100000;
        curWind = 1;
        memset(wind, 0, sizeof(wind));
        memset(field, 0, sizeof(field));
        memset(tmp, 0, sizeof(tmp));
        const auto diag = 1.0f / sqrtf(2);
        dir[0].x = -diag;
        dir[0].y = -diag;
        dir[0].i = -1;
        dir[0].j = -1;
        dir[1].x = 0.0f;
        dir[1].y = -1.0f;
        dir[1].i = -1;
        dir[1].j = 0;
        dir[2].x = diag;
        dir[2].y = -diag;
        dir[2].i = -1;
        dir[2].j = 1;
        dir[3].x = 1.0f;
        dir[3].y = 0.0f;
        dir[3].i = 0;
        dir[3].j = 1;
        dir[4].x = diag;
        dir[4].y = diag;
        dir[4].i = 1;
        dir[4].j = 1;
        dir[5].x = 0.0f;
        dir[5].y = 1.0f;
        dir[5].i = 1;
        dir[5].j = 0;
        dir[6].x = -diag;
        dir[6].y = diag;
        dir[6].i = 1;
        dir[6].j = -1;
        dir[7].x = -1.0f;
        dir[7].y = 0.0f;
        dir[7].i = 0;
        dir[7].j = -1;
    }

    // Initialization
    void Init(float minX, float minZ, float maxX, float maxZ)
    {
        baseX = minX;
        baseZ = minZ;
        kX = (WindFieldSize - 2) / (maxX - minX);
        kZ = (WindFieldSize - 2) / (maxZ - minZ);
        Reinit();
    }

    // Reinitialization
    void Reinit()
    {
        updateTime = 0.0f;
        step = cs_initors;
        srand(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
        steps = WindFieldSteps;
        curLine = -100000;
        curWind = 1;
        memset(wind, 0, sizeof(wind));
        memset(field, 0, sizeof(field));
        memset(tmp, 0, sizeof(tmp));
        const auto diag = 1.0f / sqrtf(2);
        dir[0].x = -diag;
        dir[0].y = -diag;
        dir[0].i = -1;
        dir[0].j = -1;
        dir[1].x = 0.0f;
        dir[1].y = -1.0f;
        dir[1].i = -1;
        dir[1].j = 0;
        dir[2].x = diag;
        dir[2].y = -diag;
        dir[2].i = -1;
        dir[2].j = 1;
        dir[3].x = 1.0f;
        dir[3].y = 0.0f;
        dir[3].i = 0;
        dir[3].j = 1;
        dir[4].x = diag;
        dir[4].y = diag;
        dir[4].i = 1;
        dir[4].j = 1;
        dir[5].x = 0.0f;
        dir[5].y = 1.0f;
        dir[5].i = 1;
        dir[5].j = 0;
        dir[6].x = -diag;
        dir[6].y = diag;
        dir[6].i = 1;
        dir[6].j = -1;
        dir[7].x = -1.0f;
        dir[7].y = 0.0f;
        dir[7].i = 0;
        dir[7].j = -1;
        for (int32_t i = 0; i < 1000; i++)
        {
            Step(1000.0f);
        }
    }

    // Take a calculation step
    void Step(float dltTime)
    {
        switch (step)
        {
        case cs_initors:
            SubStep1();
            break;
        case cs_copy1:
            SubStep2();
            break;
        case cs_lines:
            SubStep3();
            break;
        case cs_copy2:
            SubStep4();
            break;
        }
        updateTime += dltTime;
    }

    // Get the wind at a given point
    void GetWind(float x, float z, float &wx, float &wz) const
    {
        // Get coordinates in the field system
        x = (x - baseX) * kX + 1.0f;
        z = (z - baseZ) * kZ + 1.0f;
        // Parameters for sampling from a frame
        auto fx1 = static_cast<int32_t>(x);
        auto fx2 = fx1 + 1;
        auto fz1 = static_cast<int32_t>(z);
        auto fz2 = fz1 + 1;
        const auto kx = x - fx1;
        const auto kz = z - fz1;
        if (fx1 < 0)
            fx1 = 0;
        if (fx1 >= WindFieldSize)
            fx1 = WindFieldSize - 1;
        if (fx2 < 0)
            fx2 = 0;
        if (fx2 >= WindFieldSize)
            fx2 = WindFieldSize - 1;
        if (fz1 < 0)
            fz1 = 0;
        if (fz1 >= WindFieldSize)
            fz1 = WindFieldSize - 1;
        if (fz2 < 0)
            fz2 = 0;
        if (fz2 >= WindFieldSize)
            fz2 = WindFieldSize - 1;
        // get the next frame
        auto nextWind = curWind + 1;
        if (nextWind > 2)
            nextWind = 0;
        // Blending coefficient between frames
        auto k = updateTime * (1.0f / WindFieldUpdateTime);
        if (k > 1.0f)
            k = 1.0f;
        // Making a selection in the first frame
        Wind tmp1, tmp2, res;
        tmp1.Lerp(wind[curWind][fz1][fx1], wind[curWind][fz1][fx2], kx);
        tmp2.Lerp(wind[curWind][fz2][fx1], wind[curWind][fz2][fx2], kx);
        res.Lerp(tmp1, tmp2, kz);
        // make a selection in the second frame and get the result
        tmp1.Lerp(wind[nextWind][fz1][fx1], wind[nextWind][fz1][fx2], kx);
        tmp2.Lerp(wind[nextWind][fz2][fx1], wind[nextWind][fz2][fx2], kx);
        tmp2.Lerp(tmp1, tmp2, kz);
        res.Lerp(res, tmp2, k);
        wx = res.x;
        wz = res.z;
    }

  private:
    void SubStep1()
    {
        if (steps >= WindFieldSteps)
        {
            steps = 0;
            for (int32_t i = 0; i < 4; i++)
            {
                const auto ang = rand() * (3.14159265358979f * 2.0f / RAND_MAX);
                const auto amp = rand() * (4.0f / RAND_MAX);
                initors[i][1].x = (amp * sinf(ang) - initors[i][0].x) * (1.0f / (WindFieldSteps - 1));
                initors[i][1].y = (amp * cosf(ang) - initors[i][0].y) * (1.0f / (WindFieldSteps - 1));
            }
        }
        steps++;
        for (int32_t i = 0; i < 4; i++)
        {
            initors[i][0].x += initors[i][1].x;
            initors[i][0].y += initors[i][1].y;
        }
        for (int32_t i = 0; i < WindFieldSize; i++)
        {
            const auto w2 = i * 1.0f / (WindFieldSize - 1);
            const auto w1 = 1.0f - w2;
            field[0][i].x = w1 * initors[0][0].x + w2 * initors[1][0].x;
            field[0][i].y = w1 * initors[0][0].y + w2 * initors[1][0].y;
            field[WindFieldSize - 1][i].x = w1 * initors[2][0].x + w2 * initors[3][0].x;
            field[WindFieldSize - 1][i].y = w1 * initors[2][0].y + w2 * initors[3][0].y;
            field[i][0].x = w1 * initors[0][0].x + w2 * initors[2][0].x;
            field[i][0].y = w1 * initors[0][0].y + w2 * initors[2][0].y;
            field[i][WindFieldSize - 1].x = w1 * initors[1][0].x + w2 * initors[3][0].x;
            field[i][WindFieldSize - 1].y = w1 * initors[1][0].y + w2 * initors[3][0].y;
        }
        step = cs_copy1;
    }

    void SubStep2()
    {
        // Moving the array
        memcpy(tmp, field, sizeof(tmp));
        curLine = 1;
        step = cs_lines;
    }

    void SubStep3()
    {
        // The number of lines to be calculated
        auto needLines =
            static_cast<int32_t>(updateTime * (static_cast<float>(WindFieldSize) / WindFieldUpdateTime) + 0.99f - curLine);
        if (needLines < 1)
            needLines = 1;
        for (; needLines > 0; needLines--)
        {
            // count along the lines
            const auto i = curLine++;
            if (curLine >= WindFieldSize)
            {
                curLine = -1000;
                step = cs_copy2;
                return;
            }
            for (int32_t j = 1; j < WindFieldSize - 1; j++)
            {
                auto &pnt = tmp[i][j];
                auto x = tmp[i][j].x * 0.2f;
                auto y = tmp[i][j].y * 0.2f;
                for (int32_t n = 0; n < 8; n++)
                {
                    auto &d = dir[n];
                    auto &p = tmp[i + d.i][j + d.j];
                    auto prj = (p.x * d.x + p.y * d.y);
                    if (d.i & d.j)
                        prj *= 0.707f;
                    x += d.x * prj * 0.3f;
                    y += d.y * prj * 0.3f;
                }
                auto &p = field[i][j];
                p.x = x;
                p.y = y;
                auto len = p.x * p.x + p.y * p.y;
                if (len > 1.0f)
                {
                    len = 1.0f / sqrtf(len);
                    p.x *= len;
                    p.y *= len;
                    len = 1.0f;
                }
                if (len > 0.0f)
                {
                    p.dx += (p.x - p.dx) * 0.01f;
                    p.dy += (p.y - p.dy) * 0.01f;
                    len = (p.dx * p.x + p.dy * p.y) / len;
                    if (len < 0.0f)
                        len = 0.0f;
                    if (len > 1.0f)
                        len = 1.0f;
                    len = len * 0.2f;
                    p.x -= p.x * len;
                    p.y -= p.y * len;
                }
            }
        }
    }

    void SubStep4()
    {
        if (updateTime < WindFieldUpdateTime)
        {
            return;
        }
        updateTime = 0.0f;
        curWind++;
        if (curWind > 2)
            curWind = 0;
        const auto frame = (curWind + 1) % 3;
        auto *from = &field[0][0];
        auto *to = &wind[frame][0][0];
        const int32_t count = WindFieldSize * WindFieldSize;
        for (int32_t i = 0; i < count; i++, from++, to++)
        {
            to->x = from->x;
            to->z = from->y;
        }
        step = cs_initors;
    }

    Point field[WindFieldSize][WindFieldSize];
    Point tmp[WindFieldSize][WindFieldSize];
    Wind wind[3][WindFieldSize][WindFieldSize];
    Point initors[4][2];
    Direction dir[8];
    int32_t curWind;
    int32_t steps;
    int32_t curLine;
    float updateTime;
    CurrentStep step;
    float baseX, baseZ, kX, kZ;
};
