#include <iostream>
#include "Analyzer.h"
using namespace std;

Analyzer::Analyzer(double cash) {
    initCash = cash;
}

void Analyzer::report(double finalCash, int shares, double lastPrice) {
    double totalAsset = finalCash + shares * lastPrice;
    double returnRate = (totalAsset - initCash) / initCash;
    cout << "=== Performance Report ===" << endl;
    cout << "Initial Cash: " << initCash << endl;
    cout << "Final Cash:   " << finalCash << endl;
    cout << "Shares Held:  " << shares << endl;
    cout << "Total Asset:  " << totalAsset << endl;
    cout << "Return:       " << returnRate * 100 << "%" << endl;
}