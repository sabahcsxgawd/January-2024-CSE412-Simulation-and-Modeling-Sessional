#include "include/RandGen.h"

RandGen::RandGen(int seed, double mean) : gen(seed), dist(0, 1), mean(mean) {}
double RandGen::get() { return -mean * log(dist(gen)); }