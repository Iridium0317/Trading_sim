#include "OrderBook.h"

#include <algorithm>
#include <cmath>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>

// ── OrderMatchingEngine ───────────────────────────────────────────────────────
// Prices are in integer cents (dollar price × 100) throughout this class.

class OrderMatchingEngine {
public:
    explicit OrderMatchingEngine(int max_level) : max_level_(max_level) {}

    std::string process(const std::string& command) {
        std::istringstream iss(command);
        std::string type;
        iss >> type;

        if (type == "GTC") {
            int id, price, qty; std::string side;
            iss >> id >> side >> price >> qty;
            return handleGTC(id, side == "T", price, qty);
        }
        if (type == "IOC") {
            std::string side; int price, qty;
            iss >> side >> price >> qty;
            return handleIOC(side == "T", price, qty);
        }
        if (type == "MARKET") {
            std::string side; int qty;
            iss >> side >> qty;
            return handleMarket(side == "T", qty);
        }
        if (type == "CANCEL") {
            int id; iss >> id;
            return handleCancel(id);
        }
        if (type == "MODIFY") {
            int id, price, qty;
            iss >> id >> price >> qty;
            return handleModify(id, price, qty);
        }
        if (type == "BBO") {
            std::string side; iss >> side;
            return handleBBO(side == "T");
        }
        return "INVALID OUTPUT";
    }

private:
    int max_level_;

    struct Order { int id; int price; int qty; };

    std::map<int, std::list<Order>, std::greater<int>> buy_book_;
    std::map<int, std::list<Order>>                    sell_book_;

    struct OrderRef {
        bool is_buy;
        int  price;
        std::list<Order>::iterator it;
    };
    std::unordered_map<int, OrderRef> order_map_;

    bool isTooAggressive(bool is_buy, int price) {
        if (is_buy) {
            if (buy_book_.empty()) return false;
            return price < (long long)buy_book_.begin()->first - max_level_;
        } else {
            if (sell_book_.empty()) return false;
            return price > (long long)sell_book_.begin()->first + max_level_;
        }
    }

    std::pair<long long, int> doMatch(bool is_buy, int price, int& qty, bool has_limit) {
        long long total_cost = 0;
        int filled = 0;

        if (is_buy) {
            while (qty > 0 && !sell_book_.empty()) {
                auto lv = sell_book_.begin();
                if (has_limit && price < lv->first) break;
                auto& q = lv->second;
                while (qty > 0 && !q.empty()) {
                    auto& f = q.front();
                    int fill = std::min(qty, f.qty);
                    total_cost += (long long)lv->first * fill;
                    filled += fill;
                    qty -= fill;
                    f.qty -= fill;
                    if (f.qty == 0) { order_map_.erase(f.id); q.pop_front(); }
                }
                if (q.empty()) sell_book_.erase(lv);
            }
        } else {
            while (qty > 0 && !buy_book_.empty()) {
                auto lv = buy_book_.begin();
                if (has_limit && price > lv->first) break;
                auto& q = lv->second;
                while (qty > 0 && !q.empty()) {
                    auto& f = q.front();
                    int fill = std::min(qty, f.qty);
                    total_cost += (long long)lv->first * fill;
                    filled += fill;
                    qty -= fill;
                    f.qty -= fill;
                    if (f.qty == 0) { order_map_.erase(f.id); q.pop_front(); }
                }
                if (q.empty()) buy_book_.erase(lv);
            }
        }
        return {total_cost, filled};
    }

    std::string fmtFill(long long cost, int filled) {
        if (filled == 0) return "FILLED 0 0";
        int avg = (int)std::round((double)cost / filled);
        return "FILLED " + std::to_string(avg) + " " + std::to_string(filled);
    }

    void addToBook(int id, bool is_buy, int price, int qty) {
        Order o{id, price, qty};
        if (is_buy) {
            auto& q = buy_book_[price];
            q.push_back(o);
            order_map_[id] = {true, price, std::prev(q.end())};
        } else {
            auto& q = sell_book_[price];
            q.push_back(o);
            order_map_[id] = {false, price, std::prev(q.end())};
        }
    }

    void removeFromBook(int id) {
        auto it = order_map_.find(id);
        if (it == order_map_.end()) return;
        auto& ref = it->second;
        if (ref.is_buy) {
            auto bi = buy_book_.find(ref.price);
            bi->second.erase(ref.it);
            if (bi->second.empty()) buy_book_.erase(bi);
        } else {
            auto si = sell_book_.find(ref.price);
            si->second.erase(ref.it);
            if (si->second.empty()) sell_book_.erase(si);
        }
        order_map_.erase(it);
    }

    std::string handleGTC(int id, bool is_buy, int price, int qty) {
        if (isTooAggressive(is_buy, price)) return "REJECTED";
        auto [cost, filled] = doMatch(is_buy, price, qty, true);
        if (qty > 0) addToBook(id, is_buy, price, qty);
        return fmtFill(cost, filled);
    }

    std::string handleIOC(bool is_buy, int price, int qty) {
        if (isTooAggressive(is_buy, price)) return "REJECTED";
        auto [cost, filled] = doMatch(is_buy, price, qty, true);
        return fmtFill(cost, filled);
    }

    std::string handleMarket(bool is_buy, int qty) {
        auto [cost, filled] = doMatch(is_buy, 0, qty, false);
        return fmtFill(cost, filled);
    }

    std::string handleCancel(int id) {
        if (order_map_.find(id) == order_map_.end()) return "REJECTED";
        removeFromBook(id);
        return "CANCELLED";
    }

    std::string handleModify(int id, int new_price, int new_qty) {
        auto it = order_map_.find(id);
        if (it == order_map_.end()) return "REJECTED";
        bool is_buy   = it->second.is_buy;
        int  old_price = it->second.price;
        if (isTooAggressive(is_buy, new_price)) return "REJECTED";
        if (old_price == new_price) {
            it->second.it->qty = new_qty;
            int remaining = new_qty;
            auto [cost, filled] = doMatch(is_buy, new_price, remaining, true);
            if (remaining <= 0) removeFromBook(id);
            else order_map_.find(id)->second.it->qty = remaining;
            return fmtFill(cost, filled);
        } else {
            removeFromBook(id);
            int remaining = new_qty;
            auto [cost, filled] = doMatch(is_buy, new_price, remaining, true);
            if (remaining > 0) addToBook(id, is_buy, new_price, remaining);
            return fmtFill(cost, filled);
        }
    }

    std::string handleBBO(bool is_buy) {
        if (is_buy) {
            if (buy_book_.empty()) return "BBO 0 0";
            int p = buy_book_.begin()->first, q = 0;
            for (auto& o : buy_book_.begin()->second) q += o.qty;
            return "BBO " + std::to_string(p) + " " + std::to_string(q);
        } else {
            if (sell_book_.empty()) return "BBO 0 0";
            int p = sell_book_.begin()->first, q = 0;
            for (auto& o : sell_book_.begin()->second) q += o.qty;
            return "BBO " + std::to_string(p) + " " + std::to_string(q);
        }
    }
};

// ── OrderBook::Impl ───────────────────────────────────────────────────────────

class OrderBook::Impl {
public:
    explicit Impl(int maxLevel) : engine_(maxLevel) {}

    FillResult fill(bool isBuy, int qty, double marketPrice) {
        if (qty <= 0) return {0, 0.0};

        int cents = static_cast<int>(std::round(marketPrice * 100.0));

        // Inject synthetic counter-party at the current market price.
        // isBuy  → we need a SELL (side "F") sitting in the book.
        // !isBuy → we need a BUY  (side "T") sitting in the book.
        std::string counter =
            "GTC " + std::to_string(syntheticId_) +
            (isBuy ? " F " : " T ") +
            std::to_string(cents) + " " + std::to_string(qty);
        engine_.process(counter);

        // Fire the market order against it.
        std::string mkt =
            "MARKET " + std::string(isBuy ? "T" : "F") +
            " " + std::to_string(qty);
        std::string result = engine_.process(mkt);

        // Cancel any residual (full fill → REJECTED, partial fill → CANCELLED).
        engine_.process("CANCEL " + std::to_string(syntheticId_));
        ++syntheticId_;

        return parse(result);
    }

private:
    OrderMatchingEngine engine_;
    int syntheticId_ = 1'000'000;  // high range to avoid ID clashes

    static FillResult parse(const std::string& result) {
        std::istringstream iss(result);
        std::string tag;
        int avgCents = 0, filledQty = 0;
        iss >> tag >> avgCents >> filledQty;
        if (filledQty == 0) return {0, 0.0};
        return {filledQty, avgCents / 100.0};
    }
};

// ── OrderBook public methods ──────────────────────────────────────────────────

OrderBook::OrderBook(int maxLevel) : impl_(new Impl(maxLevel)) {}
OrderBook::~OrderBook() { delete impl_; }

FillResult OrderBook::fill(bool isBuy, int qty, double marketPrice) {
    return impl_->fill(isBuy, qty, marketPrice);
}
