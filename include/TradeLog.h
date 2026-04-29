#ifndef TRADELOG_H
#define TRADELOG_H
#include <vector>
#include <string>
#include "Types.h"

class TradeLog {
private:
    std::vector<TradeRecord> records;

public:
    void record(int time, std::string id, Signal action,
                int quantity, double price);
    void printAll();
};
#endif