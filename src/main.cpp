#include <iostream>
#include <vector>
#include <string>
#include "Market.h"
#include "Strategy.h"
#include "Account.h"
#include "TradeLog.h"
#include "Analyzer.h"

using namespace std;


int main() {
    // TODO: 填入测试数据
    // vector<MarketData> prices = {
    //     {1, "AAPL", 100.0},
    //     {2, "AAPL", 102.0},
    //     {3, "AAPL", 105.0},
    //     {4, "AAPL", 108.0},
    //     {5, "AAPL", 106.0},
    

    vector<MarketData> prices = {};  

    double startCash = 100000.0;

    Market market(prices);
    Strategy strategy(3);
    Account account(startCash);
    TradeLog tradeLog;
    Analyzer analyzer(startCash);

    double lastPrice = 0;

    while (market.hasNext()) {
        MarketData data = market.next();
        Signal signal = strategy.decide(data.price);

        if (signal == BUY) {
            int sharesBefore = account.getShares(data.id);
            account.buy(data.id, data.price);
            int bought = account.getShares(data.id) - sharesBefore;
            if (bought > 0) {
                tradeLog.record(data.time, data.id, BUY, bought, data.price);
            }
        } else if (signal == SELL) {
            int sharesToSell = account.getShares(data.id);
            if (sharesToSell > 0) {
                account.sell(data.id,data.price);
                tradeLog.record(data.time, data.id, SELL, sharesToSell, data.price);
            }
        }

        lastPrice = data.price;
    }

    cout << endl;
    cout << "=== Trade History ===" << endl;
    tradeLog.printAll();
    cout << endl;

    cout << "=== Account Status ===" << endl;
    account.printResult();
    cout << endl;
    
    analyzer.report(account.getCash(), 0, lastPrice);
    // analyzer.report(account.getCash(), account.getShares(data.id), lastPrice);

    return 0;
}