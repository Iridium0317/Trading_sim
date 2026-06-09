# Quantitative Trading Simulator

A high-performance, automated quantitative trading simulator built in C++17. The system replays historical market data, executes user-defined trading strategies, and generates performance reports. It is designed as a foundation for low-latency, concurrent backtesting.

## Overview

The simulator follows a modular, event-driven architecture:

```
Market Data (CSV) → Strategy Engine → OrderBook (matching) → Account (portfolio)
                                                                      ↓
                                              Trade Log → Performance Analyzer
```

Each signal from the strategy is submitted to the `OrderBook` as a market order. The order book matches it against a synthetic counter-party at the current market price, returning a `FillResult` that `Account` uses to update cash and positions. This keeps execution semantics separate from portfolio accounting.

Users can use custom strategies by implementing the `Strategy` interface, to backtest different trading algorithms against real historical data.

## Features

**Implemented**

- Order book matching engine (`OrderMatchingEngine`) supporting GTC, IOC, MARKET, CANCEL, MODIFY, BBO
- `OrderBook` module: wraps the engine, routes strategy signals through market-order matching before updating the portfolio
- Multi-asset portfolio tracking
- Trade history logging
- Post-simulation performance report (return rate, final valuation)

**In Progress**

- Python data pipeline: fetch real market data via `yfinance`, export to CSV
- CSV parser in C++ for ingesting historical price data

## TODO

- [ ] **Strategy Framework** — Base class + polymorphism, swap strategies without changing core engine
- [ ] **Concurrency** — Thread pool for multi-asset strategy evaluation
- [ ] **Low Latency** — Lock-free order queue, pre-allocated memory pool for trade records
- [ ] **Bitmap Optimization** — Bitmap-based asset state tracking (tradable / halted / held)
- [ ] **Data Pipeline** — Python (yfinance + pandas) → CSV → C++ parser
- [ ] **Benchmarking** — Per-component latency benchmarks + end-to-end stress tests

## Build & Run

```bash
# Compile
clang++ -std=c++17 -Iinclude -o trading_sim src/*.cpp

# Run
./trading_sim

# Run OrderBook unit tests
cd tests && clang++ -std=c++17 -I../include ../src/OrderBook.cpp test_orderbook.cpp -o test_orderbook && ./test_orderbook
```

## Project Structure

```
trading_sim/
├── include/
│   ├── Types.h          # Shared types: Signal enum, MarketData, TradeRecord
│   ├── Market.h         # Market data feed
│   ├── Strategy.h       # Trading strategy engine
│   ├── OrderBook.h      # Order matching interface (FillResult, OrderBook)
│   ├── Account.h        # Portfolio and cash management
│   ├── TradeLog.h       # Trade history recorder
│   └── Analyzer.h       # Post-simulation performance analysis
├── src/
│   ├── Market.cpp
│   ├── Strategy.cpp
│   ├── OrderBook.cpp    # OrderMatchingEngine (pimpl) + OrderBook methods
│   ├── Account.cpp
│   ├── TradeLog.cpp
│   ├── Analyzer.cpp
│   └── main.cpp         # Simulation entry point and main loop
├── tests/
│   └── test_orderbook.cpp  # Unit tests for OrderBook (39 assertions)
├── data/                # (planned) CSV market data from Python pipeline
├── bench/
│   └── bench_main.cpp   # C++ micro-benchmarks
├── scripts/
│   ├── fetch_data.py    # market data pipeline
│   └── stress_test.py   # end-to-end stress test
└── README.md
```

## OrderBook Module

`OrderBook` is a thin wrapper around `OrderMatchingEngine` (a price-time-priority matching engine). It is owned by `Account` and mediates every buy/sell:

```
Account::buy(id, price)
  └─ qty = floor(cash / price)
  └─ OrderBook::fill(isBuy=true, qty, price)
       ├─ inject synthetic GTC SELL at price (represents market liquidity)
       ├─ submit MARKET BUY → engine matches and returns "FILLED avg qty"
       └─ cancel residual synthetic order
  └─ cash  -= fill.qty * fill.avg
  └─ positions[id] += fill.qty
```

`OrderMatchingEngine` supports the following commands internally:

| Command | Description |
|---------|-------------|
| `GTC id side price qty` | Good-Till-Cancel limit order |
| `IOC side price qty` | Immediate-Or-Cancel limit order |
| `MARKET side qty` | Market order (sweep to fill) |
| `CANCEL id` | Cancel a resting order |
| `MODIFY id price qty` | Modify price/qty of a resting order |
| `BBO side` | Query best bid or offer |

Prices are stored as integer cents internally; `OrderBook::fill` converts from `double` dollars on the way in and back on the way out.

To run the unit tests:

```bash
cd tests
clang++ -std=c++17 -I../include ../src/OrderBook.cpp test_orderbook.cpp -o test_orderbook
./test_orderbook
```

## Customizing Strategies

The `Strategy` class determines when to buy or sell. The current implementation uses a streak-based momentum approach:

- **BUY** signal: price drops for N consecutive steps (default N=3)
- **SELL** signal: price rises for N consecutive steps
- **HOLD**: otherwise

To test different parameters, modify the threshold when constructing the strategy:

```cpp
Strategy strategy(5);  // trigger after 5 consecutive moves instead of 3
```

Future versions will support a `Strategy` base class, allowing users to implement custom algorithms:

```cpp
class MyStrategy : public Strategy {
    Signal decide(double price) override {
        // your logic here
    }
};
```

## Tech Stack

- **Language**: C++17
- **Data Source** (planned): Python 3 + yfinance + pandas
- **Build**: g++ (CMake planned)
- **Platform**: Linux / macOS
