#include "../include/RandGen.h"
#include "../include/lcgrand.h"

#include <cmath>

RandGen::RandGen() {
    this->mean = 0.0;
}

double RandGen::get(double mean) {
    return -mean * log(lcgrand(1));
}