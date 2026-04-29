#ifndef STRATEGY_H
#define STRATEGY_H
#include "Types.h"
class Strategy {
private:
    double lastPrice;
    int streak;
    int threshold;

public:
    Strategy(int t);
    Signal decide(double currentPrice);
};
#endif