#ifndef POWERUP_H
#define POWERUP_H



class PowerUp
{
    public:
        int attribx;
        int attriby;
        void effect();
        bool collision;

        PowerUp();
};

#endif