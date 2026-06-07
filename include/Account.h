#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <map>
#include "OrderBook.h"

class Account {
private:
    double cash;
    std::map<std::string, int> positions;
    OrderBook orderBook_;

public:
    Account(double initCash);
    void buy(std::string id, double price);
    void sell(std::string id, double price);
    double getCash();
    int getShares(std::string id);
    void printResult();
};

#endif