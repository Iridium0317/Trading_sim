#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <map>

class Account {
private:
    double cash;
    std::map<std::string, int> positions; // 持仓

public:
    Account(double initCash);
    void buy(std::string id, double price);
    void sell(std::string id, double price);
    double getCash();
    int getShares(std::string id);
    void printResult();
};

#endif