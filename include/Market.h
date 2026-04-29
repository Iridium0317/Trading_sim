#ifndef MARKET_H
#define MARKET_H

#include <vector>

class Market {
private:
    std::vector<MarketData> data;
    int currentIndex;

public:
    Market(std::vector<MarketData> inputData);
    bool hasNext();
    MarketData next();
};
#endif