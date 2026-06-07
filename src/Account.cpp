#include <iostream>
#include "Account.h"
using namespace std;

Account::Account(double initCash) : cash(initCash) {}

void Account::buy(string id, double price) {
    int maxShares = (int)(cash / price);
    if (maxShares <= 0) return;

    FillResult fill = orderBook_.fill(true, maxShares, price);
    if (fill.qty > 0) {
        cash -= fill.qty * fill.avg;
        positions[id] += fill.qty;
    }
}

void Account::sell(string id, double price) {
    int qty = positions[id];
    if (qty <= 0) return;

    FillResult fill = orderBook_.fill(false, qty, price);
    if (fill.qty > 0) {
        cash += fill.qty * fill.avg;
        positions[id] -= fill.qty;
    }
}

double Account::getCash() { return cash; }

int Account::getShares(string id) { return positions[id]; }

void Account::printResult() {
    cout << "Cash: " << cash << endl;
    for (auto& p : positions) {
        if (p.second > 0) {
            cout << "  " << p.first << ": " << p.second << " shares" << endl;
        }
    }
}