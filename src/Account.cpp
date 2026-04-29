#include <iostream>
#include "Account.h"
using namespace std;

Account::Account(double initCash) {
    cash = initCash;
}

void Account::buy(string id, double price) {
    int maxShares = (int)(cash / price);
    cash -= maxShares * price;
    positions[id] += maxShares;  // 
}

void Account::sell(string id, double price) {
    int qty = positions[id];     // 
    cash += qty * price;
    positions[id] = 0;
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