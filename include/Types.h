#ifndef TYPES_H
#define TYPES_H

#include <string>

enum Signal { BUY, SELL, HOLD };

struct MarketData {
    int time;
    std::string id;
    double price;
};

struct TradeRecord {
    int time;
    std::string id;
    Signal action;
    int quantity;
    double price;
};

#endif