#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

struct FillResult {
    int    qty;  // shares actually filled (0 = nothing done)
    double avg;  // average fill price in dollars (0.0 when qty == 0)
};

class OrderBook {
public:
    explicit OrderBook(int maxLevel = 2000000);
    ~OrderBook();

    // Simulate an immediate fill at marketPrice against synthetic liquidity.
    // isBuy=true → buy qty shares; isBuy=false → sell qty shares.
    FillResult fill(bool isBuy, int qty, double marketPrice);

private:
    class Impl;
    Impl* impl_;
};

#endif
