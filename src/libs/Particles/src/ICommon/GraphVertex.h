#pragma once

struct GraphVertex
{
    union {
        struct
        {
            float Time;
            float Val;
        };

        struct
        {
            float x;
            float y;
        };
    };
};
