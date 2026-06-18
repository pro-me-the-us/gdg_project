#ifndef POWER_UPS_H
#define POWER_UPS_H

#include <cstdint>
#include "PowerUp.h"
#include <vector>
#include <memory>

class Power_Ups
{
    public:
        /*
        Different power ups have differnt effects thus occupy different size in memory
        This instead of array of powers ups, i have made an array of pointer to the power ups
        This ensures that there is no data in loss in something called "Object Slicing"
        */
        std::vector<std::unique_ptr<PowerUp>> power_ups;
        Power_Ups();
};

#endif