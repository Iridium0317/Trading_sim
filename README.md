# Quantitative Trading Simulator

A high-performance, automated quantitative trading simulator built in C++17. The system replays historical market data, executes user-defined trading strategies, and generates performance reports. It is designed as a foundation for low-latency, concurrent backtesting.

## Overview

The simulator follows a modular, event-driven architecture:

```
Market Data (CSV) → Strategy Engine → Order Execution → Portfolio Management
                                                            ↓
                                            Trade Log → Performance Analyzer
```

Users can use custom strategies by implementing the `Strategy` interface, to backtest different trading algorithms against real historical data.

## Features

**Implemented**

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
g++ -std=c++17 -Iinclude -o trading_sim src/*.cpp

# Run
./trading_sim
```

## Project Structure

```
trading_sim/
├── include/
│   ├── Types.h          # Shared types: Signal enum, MarketData, TradeRecord
│   ├── Market.h         # Market data feed
│   ├── Strategy.h       # Trading strategy engine
│   ├── Account.h        # Portfolio and cash management
│   ├── TradeLog.h       # Trade history recorder
│   └── Analyzer.h       # Post-simulation performance analysis
├── src/
│   ├── Market.cpp
│   ├── Strategy.cpp
│   ├── Account.cpp
│   ├── TradeLog.cpp
│   ├── Analyzer.cpp
│   └── main.cpp         # Simulation entry point and main loop
├── data/                 # (planned) CSV market data from Python pipeline
├── bench/
│   └── bench_main.cpp    # C++ micro-benchmarks
├── scripts/
│   ├── fetch_data.py     # market data pipeline
│   └── stress_test.py    # end-to-end stress test
└── README.md
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
