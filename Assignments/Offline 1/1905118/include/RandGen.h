#ifndef RandGen_h
#define RandGen_h

#include <random>
#include <cmath>

class RandGen {
public:
    RandGen() = delete;
    RandGen(int seed, double mean);
    double get();

private:
    std::mt19937 gen;
    std::uniform_real_distribution<> dist;
    double mean;
};

#endif // RandGen_h