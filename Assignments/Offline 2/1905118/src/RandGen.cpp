#include "../include/lcgrand.h"
#include "../include/RandGen.h"
#include <cmath>

RandGen::RandGen() {}

double RandGen::getExponential(double mean) {
    return -mean * log(lcgrand(1));
}

double RandGen::getUniform(double a, double b) {
    return a + (b - a) * lcgrand(1);
}

int RandGen::getRandomInt(std::vector<double> &probability_distribution) {
    double u = lcgrand(1);
    int i = 0;

    for(i = 0; u >= probability_distribution[i] && i < (int) probability_distribution.size(); i++) {
        ;
    }

    return i + 1;
}