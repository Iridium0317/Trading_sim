// Unit tests for OrderBook matching logic.
// Build: clang++ -std=c++17 -I../include ../src/OrderBook.cpp test_orderbook.cpp -o test_orderbook
// Run:   ./test_orderbook

#include "OrderBook.h"

#include <cmath>
#include <iostream>
#include <string>

// ── minimal harness ───────────────────────────────────────────────────────────

namespace {

int g_pass = 0, g_fail = 0;

void check(const std::string& name, bool cond) {
    if (cond) {
        std::cout << "  [PASS] " << name << "\n";
        ++g_pass;
    } else {
        std::cout << "  [FAIL] " << name << "\n";
        ++g_fail;
    }
}

// Monetary comparison: tolerance of half a cent ($0.005)
void checkPrice(const std::string& name, double actual, double expected) {
    check(name, std::fabs(actual - expected) < 0.005);
}

void section(const std::string& name) {
    std::cout << "\n[" << name << "]\n";
}

} // namespace

// ── test cases ────────────────────────────────────────────────────────────────

// 1. Basic buy: qty and avg price are exact.
void test_basic_buy() {
    section("basic buy");
    OrderBook ob;
    FillResult r = ob.fill(true, 100, 100.0);
    check    ("qty == 100",     r.qty == 100);
    checkPrice("avg == $100.00", r.avg, 100.0);
}

// 2. Basic sell: qty and avg price are exact.
void test_basic_sell() {
    section("basic sell");
    OrderBook ob;
    FillResult r = ob.fill(false, 200, 55.0);
    check    ("qty == 200",    r.qty == 200);
    checkPrice("avg == $55.00", r.avg, 55.0);
}

// 3. Fractional dollar price: $102.50 survives the double→cents→double round-trip.
void test_fractional_price() {
    section("fractional price");
    OrderBook ob;
    FillResult r = ob.fill(true, 1, 102.50);
    check    ("qty == 1",        r.qty == 1);
    checkPrice("avg == $102.50", r.avg, 102.50);
}

// 4. Fine cents: $99.99 round-trip.
void test_fine_cents() {
    section("fine cents");
    OrderBook ob;
    FillResult r = ob.fill(false, 10, 99.99);
    check    ("qty == 10",       r.qty == 10);
    checkPrice("avg == $99.99",  r.avg, 99.99);
}

// 5. Zero-quantity guard: returns {0, 0.0} without crashing.
void test_zero_qty() {
    section("zero qty guard");
    OrderBook ob;
    FillResult r = ob.fill(true, 0, 100.0);
    check    ("qty == 0",  r.qty == 0);
    checkPrice("avg == 0", r.avg, 0.0);
}

// 6. Book is clean between calls: two fills at different prices must not
//    cross-contaminate (i.e., no synthetic order left over from call 1).
void test_book_clean_between_calls() {
    section("book clean between calls");
    OrderBook ob;
    FillResult r1 = ob.fill(true, 50, 100.0);
    FillResult r2 = ob.fill(true, 50, 200.0);
    check    ("call-1 qty == 50",      r1.qty == 50);
    check    ("call-2 qty == 50",      r2.qty == 50);
    checkPrice("call-1 avg == $100",   r1.avg, 100.0);
    checkPrice("call-2 avg == $200",   r2.avg, 200.0);
}

// 7. Mixed buys and sells in sequence stay independent.
void test_interleaved_buys_and_sells() {
    section("interleaved buys and sells");
    OrderBook ob;
    FillResult b1 = ob.fill(true,  100, 100.0);
    FillResult s1 = ob.fill(false,  50, 110.0);
    FillResult b2 = ob.fill(true,   30, 90.0);
    FillResult s2 = ob.fill(false,  20, 95.0);
    check    ("b1 qty",  b1.qty == 100); checkPrice("b1 avg", b1.avg, 100.0);
    check    ("s1 qty",  s1.qty ==  50); checkPrice("s1 avg", s1.avg, 110.0);
    check    ("b2 qty",  b2.qty ==  30); checkPrice("b2 avg", b2.avg,  90.0);
    check    ("s2 qty",  s2.qty ==  20); checkPrice("s2 avg", s2.avg,  95.0);
}

// 8. P&L consistency: buy then sell at a higher price → correct net gain.
void test_pnl_consistency() {
    section("P&L consistency");
    OrderBook ob;
    FillResult buy  = ob.fill(true,  100, 100.0);
    FillResult sell = ob.fill(false, 100, 110.0);
    double pnl = sell.qty * sell.avg - buy.qty * buy.avg;  // should be $1000
    check    ("buy qty == 100",        buy.qty  == 100);
    check    ("sell qty == 100",       sell.qty == 100);
    checkPrice("net gain == $1000",    pnl, 1000.0);
}

// 9. Large quantity: 1 million shares.
void test_large_qty() {
    section("large qty (1M shares)");
    OrderBook ob;
    FillResult r = ob.fill(true, 1'000'000, 10.0);
    check    ("qty == 1 000 000",  r.qty == 1'000'000);
    checkPrice("avg == $10.00",    r.avg, 10.0);
}

// 10. Single share: minimal fill.
void test_single_share() {
    section("single share");
    OrderBook ob;
    FillResult r = ob.fill(false, 1, 333.33);
    check    ("qty == 1",        r.qty == 1);
    checkPrice("avg == $333.33", r.avg, 333.33);
}

// 11. Many sequential fills at varying prices: each must return its own price.
void test_many_sequential_fills() {
    section("many sequential fills");
    struct Case { bool isBuy; int qty; double price; };
    Case cases[] = {
        {true,  10, 50.00},
        {false, 20, 75.25},
        {true,  30, 100.00},
        {false, 40, 125.50},
        {true,  50, 200.00},
    };
    OrderBook ob;
    for (auto& c : cases) {
        FillResult r = ob.fill(c.isBuy, c.qty, c.price);
        std::string label = std::to_string(c.qty) + "@" + std::to_string((int)(c.price * 100));
        check    ("qty " + label, r.qty == c.qty);
        checkPrice("avg " + label, r.avg, c.price);
    }
}

// ── main ──────────────────────────────────────────────────────────────────────

int main() {
    test_basic_buy();
    test_basic_sell();
    test_fractional_price();
    test_fine_cents();
    test_zero_qty();
    test_book_clean_between_calls();
    test_interleaved_buys_and_sells();
    test_pnl_consistency();
    test_large_qty();
    test_single_share();
    test_many_sequential_fills();

    std::cout << "\n──────────────────────────────\n";
    std::cout << g_pass << " passed, " << g_fail << " failed\n";
    return g_fail == 0 ? 0 : 1;
}
