#include "Market.h"
#include "Types.h"
using namespace std;


Market::Market(vector<MarketData> inputData) {
    data = inputData;
    currentIndex = 0;
}

bool Market::hasNext() {
    return currentIndex != (int)data.size();
}

MarketData Market::next() {
    return data[currentIndex++];
}
