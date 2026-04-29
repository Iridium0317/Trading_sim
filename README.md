# Quantitative Trading Simulator

A high-performance, automated quantitative trading simulator built in C++17. The system replays historical market data, executes user-defined trading strategies, and generates performance reports. It is designed as a foundation for low-latency, concurrent backtesting.

## Overview

The simulator follows a modular, event-driven architecture:

```
Market Data (CSV) → Strategy Engine → Order Execution → Portfolio Management
                                                            ↓
                                            Trade Log → Performance Analyzer
```

Users can plug in custom strategies by implementing the `Strategy` interface, making it easy to backtest different trading algorithms against real historical data.

## Features

**Implemented**

- Modular OOP architecture with separated headers and implementations
- Streak-based momentum strategy (configurable threshold for consecutive price movements)
- Multi-asset portfolio management using `std::map`-based position tracking
- Full trade history logging with per-trade metadata (time, asset, action, quantity, price)
- Post-simulation performance analysis (total return, final asset valuation)

**In Progress**

- Python data pipeline: fetch real market data via `yfinance`, export to CSV
- CSV parser in C++ for ingesting historical price data

## Planned Enhancements

| Category                | Description                                                                                                                             |
| ----------------------- | --------------------------------------------------------------------------------------------------------------------------------------- |
| **Strategy Framework**  | Abstract `Strategy` base class with polymorphism, allowing users to implement and swap custom strategies without modifying core engine  |
| **Concurrency**         | Multi-threaded strategy evaluation across multiple assets using a thread pool                                                           |
| **Low Latency**         | Lock-free order queue between strategy and execution layers, pre-allocated memory pool for trade records                                |
| **Bitmap Optimization** | Bitmap-based asset state tracking (tradable / halted / held) for batch status checks                                                    |
| **Data Pipeline**       | Python script (`yfinance` + `pandas`) → CSV → C++ `ifstream` parser                                                                     |
| **Benchmarking**        | C++ micro-benchmarks (`bench_main.cpp`) for per-component latency; Python stress tests for end-to-end throughput under high data volume |

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
