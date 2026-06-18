#ifndef POWERUP_H
#define POWERUP_H

#include <cstdint>

class PowerUp
{
    public:
        int32_t attribx;
        int32_t attriby;
        void effect();
        bool collision;

        PowerUp();
};

#endif