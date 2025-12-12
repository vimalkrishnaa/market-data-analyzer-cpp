# MarketDataAnalyzer

Simple C++17 console utility that loads intraday price/volume data from a CSV file, calculates moving averages, returns, rolling volatility, and flags basic patterns (price spikes and bullish crossovers). Results are printed to the console and written to `results.txt`.

## Files
- `main.cpp` — core implementation.
- `sample_data.csv` — example input data.
- `results.txt` — generated output after running the program.

## Building
```bash
g++ -std=c++17 -O2 -o MarketDataAnalyzer main.cpp
```

## Running
```bash
./MarketDataAnalyzer    # uses sample_data.csv in the working directory
```

## What it does
- Parses CSV rows: `timestamp,price,volume` using only the C++ STL.
- Computes 5- and 10-period moving averages, per-row returns, and rolling 5-period volatility of returns.
- Detects:
  - Price spikes (>3% jump versus previous price).
  - Bullish crossovers (MA5 crossing above MA10).
- Summarizes top absolute return moves with a priority queue and custom comparator.
- Uses STL containers (`vector`, `map`, `priority_queue`) with small optimizations (`reserve`, references, compact loops).

## Customizing
- Replace `sample_data.csv` with your own file, keeping the same column order.
- Adjust periods or detection thresholds directly in `main.cpp`.

