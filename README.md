# MarketDataAnalyzer

A high-performance C++17 console application for analyzing financial market data. This tool processes CSV files containing timestamped price and volume data, computes technical indicators, detects trading patterns, and generates comprehensive analysis reports.

## 📋 Overview

MarketDataAnalyzer is a pure C++17 implementation that uses only the Standard Template Library (STL) to perform financial data analysis. It reads market data from CSV files, calculates moving averages, returns, and volatility, then identifies significant price movements and trend patterns. All results are displayed on the console and saved to a text file for further analysis.

## ✨ Features

### Data Processing
- **CSV Parsing**: Robust parsing of timestamp, price, and volume data using STL only
- **Error Handling**: Gracefully handles invalid data lines and missing values
- **Memory Efficient**: Pre-allocated containers to minimize reallocations
- **Header Detection**: Automatically skips CSV header rows

### Technical Indicators
- **Moving Averages**: Computes 5-period and 10-period simple moving averages (SMA)
- **Returns Calculation**: Calculates percentage returns between consecutive price points
- **Rolling Volatility**: Computes 5-period rolling standard deviation of returns
- **NaN Handling**: Properly handles insufficient data points with NaN values

### Pattern Detection
- **Price Spikes**: Identifies sudden price increases exceeding 3% threshold
- **Bullish Crossovers**: Detects when the 5-period MA crosses above the 10-period MA
- **Top Moves**: Tracks and reports the top 5 absolute return movements
- **Pattern Statistics**: Counts occurrences of each detected pattern type

### Performance Optimizations
- **Sliding Window Algorithms**: O(1) updates for moving averages and volatility
- **Reference Passing**: Avoids unnecessary data copying
- **Memory Pre-allocation**: Uses `reserve()` to minimize vector reallocations
- **Efficient Loops**: Single-pass algorithms where possible

## 🏗️ Project Structure

```
market-data-analyzer-cpp/
├── main.cpp              # Core implementation (233 lines)
├── sample_data.csv       # Sample input data (2000+ entries)
├── README.md            # This file
├── .gitignore           # Git ignore rules
└── results.txt          # Generated output (created at runtime)
```

## 📦 Requirements

- **Compiler**: GCC 7+ or Clang 5+ with C++17 support
- **Standard Library**: C++17 STL
- **Operating System**: Windows, Linux, or macOS
- **Dependencies**: None (pure STL implementation)

## 🔨 Building

### Compilation

```bash
g++ -std=c++17 -O2 -o MarketDataAnalyzer main.cpp
```

**Flags Explanation:**
- `-std=c++17`: Enables C++17 standard features
- `-O2`: Optimizes for performance
- `-o MarketDataAnalyzer`: Specifies output executable name

### Alternative Compilers

```bash
# Using Clang
clang++ -std=c++17 -O2 -o MarketDataAnalyzer main.cpp

# Using MSVC (Windows)
cl /std:c++17 /O2 main.cpp /Fe:MarketDataAnalyzer.exe
```

## 🚀 Usage

### Basic Usage

```bash
./MarketDataAnalyzer
```

The program will:
1. Read `sample_data.csv` from the current directory
2. Process all data and compute indicators
3. Display results on the console
4. Save results to `results.txt`

### Input File Format

The CSV file must follow this format:

```csv
timestamp,price,volume
2025-01-02 09:30,100.00,1500
2025-01-02 09:31,101.20,1200
2025-01-02 09:32,102.50,1350
...
```

**Requirements:**
- First line can be a header (will be automatically skipped)
- Columns: `timestamp,price,volume` (comma-separated)
- Timestamp format: `YYYY-MM-DD HH:MM` (flexible, stored as string)
- Price: Decimal number
- Volume: Integer or decimal number

## 📊 Output Format

The program generates a comprehensive report with the following sections:

### 1. Summary Header
```
Market Data Analysis
Rows: 2000
```

### 2. Data Table
CSV-formatted table with all computed indicators:
```
timestamp,price,volume,MA5,MA10,return,vol5
2025-01-02 09:30,100.00,1500,nan,nan,nan,nan
2025-01-02 09:31,101.20,1200,nan,nan,0.012000,nan
...
```

**Columns:**
- `timestamp`: Original timestamp from input
- `price`: Original price value
- `volume`: Original volume value
- `MA5`: 5-period moving average (NaN until 5 data points available)
- `MA10`: 10-period moving average (NaN until 10 data points available)
- `return`: Percentage return from previous price
- `vol5`: 5-period rolling volatility (NaN until 5 returns available)

### 3. Pattern Detection
```
Pattern detection:
2025-01-02 09:34 Price Spike Detected (3.21%)
2025-01-02 09:39 Bullish Crossover
...
```

### 4. Top Moves
```
Top absolute moves (by return):
2025-01-02 09:39 return=3.29% price=110.00
2025-01-02 09:34 return=3.21% price=103.00
...
```

### 5. Pattern Statistics
```
Pattern counts:
Price Spike Detected: 2
Bullish Crossover: 113
```

## 🔍 Algorithm Details

### Moving Average Calculation
Uses a sliding window approach:
- Maintains a running sum of the current window
- Adds new values and subtracts old values as the window slides
- Time complexity: O(1) per data point (amortized)

### Volatility Calculation
Computes rolling standard deviation using:
- Running sum and sum of squares
- Formula: `σ = √(E[X²] - E[X]²)`
- Efficiently updates as the window moves forward

### Pattern Detection Logic

**Price Spike:**
```cpp
priceChange = (current_price - previous_price) / previous_price
if (priceChange > 0.03)  // 3% threshold
    → Price Spike Detected
```

**Bullish Crossover:**
```cpp
if (MA5[i] > MA10[i] && MA5[i-1] <= MA10[i-1])
    → Bullish Crossover
```

## 🛠️ STL Components Used

- **`std::vector`**: Primary data storage and calculations
- **`std::map`**: Pattern counting and statistics
- **`std::priority_queue`**: Top moves tracking with custom comparator
- **`std::string`**: Timestamp and string manipulation
- **`std::ifstream` / `std::ofstream`**: File I/O operations
- **`std::istringstream`**: CSV parsing
- **`std::ostringstream`**: Efficient string building
- **Custom Comparator**: `AbsReturnCmp` for priority queue ordering

## 🎯 Customization

### Adjusting Detection Thresholds

Edit `main.cpp` to modify thresholds:

```cpp
// Price spike threshold (currently 3%)
if (priceChange > 0.03) {  // Change 0.03 to desired threshold
    // ...
}

// Moving average periods
auto ma5 = calcMovingAverage(prices, 5);   // Change 5 to desired period
auto ma10 = calcMovingAverage(prices, 10); // Change 10 to desired period
```

### Changing Output File

Modify the `outputFile` variable in `main()`:

```cpp
const std::string outputFile = "results.txt";  // Change filename here
```

### Using Custom Data

1. Replace `sample_data.csv` with your own CSV file
2. Ensure the format matches: `timestamp,price,volume`
3. Run the program - it will automatically use the new file

## 📈 Example Use Cases

- **Technical Analysis**: Identify trends and patterns in historical market data
- **Backtesting**: Test trading strategies on historical data
- **Data Processing**: Clean and analyze large CSV datasets
- **Educational**: Learn C++17 STL and financial algorithms
- **Research**: Study market behavior and volatility patterns

## 🐛 Error Handling

The program includes robust error handling:
- **File Not Found**: Displays error message and exits gracefully
- **Invalid Data Lines**: Skips malformed rows and continues processing
- **Empty Files**: Detects and reports empty input files
- **NaN Values**: Properly handles insufficient data for calculations

## 📝 Notes

- The program processes data sequentially and requires sufficient memory for the entire dataset
- Large files (100,000+ rows) may take several seconds to process
- Results are appended to `results.txt` - previous results are overwritten
- The executable (`MarketDataAnalyzer.exe` on Windows) is excluded from git via `.gitignore`

## 🔗 Repository

GitHub: [https://github.com/vimalkrishnaa/market-data-analyzer-cpp](https://github.com/vimalkrishnaa/market-data-analyzer-cpp)

## 📄 License

This project is open source and available for educational and commercial use.

---

**Built with C++17 • Pure STL • No External Dependencies**
