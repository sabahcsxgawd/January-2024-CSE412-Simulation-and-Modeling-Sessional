#ifndef RANDGEN_H
#define RANDGEN_H

#include <vector>

class RandGen {
public:
    RandGen();
    double getExponential(double mean);
    double getUniform(double a, double b);
    int getRandomInt(std::vector<double> &probability_distribution);
};

#endif // RANDGEN_H