#include "TradeLog.h"
#include "Types.h"
#include <iostream>
using namespace std;


void TradeLog::record(int time, string id, Signal action,
            int quantity, double price) {
    records.push_back({time, id, action, quantity, price});
}

void TradeLog::printAll() {
    for (int i = 0; i < (int)records.size(); i++) {
        TradeRecord r = records[i];
        string actionStr;
        if (r.action == BUY)  actionStr = "BUY";
        else                  actionStr = "SELL";

        cout << "Time: " << r.time
                << "  " << r.id
                << "  " << actionStr
                << "  Qty: " << r.quantity
                << "  Price: " << r.price << endl;
    }
}
