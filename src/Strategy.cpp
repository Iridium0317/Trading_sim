#include "Strategy.h"
#include "Types.h"
using namespace std;


Strategy:: Strategy(int t) {
    lastPrice = 0;
    streak = 0;
    threshold = t;
}

Signal Strategy::decide(double currentPrice) {
    // 更新 streak
    if (currentPrice > lastPrice) {
        if (streak >= 0) streak++;
        else streak = 1;
    } else if (currentPrice < lastPrice) {
        if (streak <= 0) streak--;
        else streak = -1;
    } else {
        streak = 0;
    }

    // 更新 lastPrice
    lastPrice = currentPrice;

    // 第三步：判断信号
    if (streak >= threshold)  return SELL;
    if (streak <= -threshold) return BUY;
    return HOLD;
}
