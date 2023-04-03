#pragma once
#include <random>

class RandomNumberGenerator
{
private:
    std::random_device rd;
    std::mt19937 generator;
    std::uniform_int_distribution<>intDistr;
    std::uniform_real_distribution<>floatDistr;
public:
    RandomNumberGenerator(int intMin = 0, int intMax = 0, float floatMin = 0.f, float floatMax = 1.f) : generator(rd()), intDistr(intMin, intMax), floatDistr(floatMin, floatMax) {}
    int getRandomInt() { return intDistr(generator); }
    double getRandomFloat() { return floatDistr(generator); }
    void setFloatRange(float min, float max) { std::uniform_real_distribution<> distr(min, max); floatDistr = distr; }
    void setIntRange(int min, int max) { std::uniform_int_distribution<> distr(min, max); intDistr = distr; }
};



class RandomIntGenerator
{
private:
    std::random_device rd;
    std::mt19937 generator;
    std::uniform_int_distribution<>xDistr;
    std::uniform_int_distribution<>yDistr;
public:
    RandomIntGenerator(int xMax, int yMax) : generator(rd()), xDistr(0, xMax), yDistr(0, yMax) {}
    int getRandomXRange() { return xDistr(generator);}
    int getRandomYRange() { return yDistr(generator);}
};