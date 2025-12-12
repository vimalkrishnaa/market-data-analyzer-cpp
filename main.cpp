#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

struct Record {
    std::string timestamp;
    double price{};
    double volume{};
};

using Records = std::vector<Record>;

Records loadData(const std::string &filename) {
    Records records;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    std::string line;
    records.reserve(1024); // reserve a reasonable default to minimize reallocations
    
    // Skip header line
    if (std::getline(file, line)) {
        if (line.find("timestamp") != std::string::npos) {
            // Header line, skip it
        }
    }
    
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        std::istringstream ss(line);
        Record rec;
        std::string priceStr, volumeStr;
        if (!std::getline(ss, rec.timestamp, ',')) {
            continue;
        }
        if (!std::getline(ss, priceStr, ',')) {
            continue;
        }
        if (!std::getline(ss, volumeStr, ',')) {
            continue;
        }
        try {
            rec.price = std::stod(priceStr);
            rec.volume = std::stod(volumeStr);
            records.push_back(rec);
        } catch (const std::exception&) {
            // Skip invalid lines
            continue;
        }
    }
    return records;
}

std::vector<double> calcMovingAverage(const std::vector<double> &prices, std::size_t period) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> ma;
    ma.reserve(prices.size());
    double windowSum = 0.0;

    for (std::size_t i = 0; i < prices.size(); ++i) {
        windowSum += prices[i];
        if (i >= period) {
            windowSum -= prices[i - period];
        }
        if (i + 1 >= period) {
            ma.push_back(windowSum / static_cast<double>(period));
        } else {
            ma.push_back(nan);
        }
    }
    return ma;
}

std::vector<double> calcReturns(const std::vector<double> &prices) {
    std::vector<double> rets;
    rets.reserve(prices.size());
    rets.push_back(std::numeric_limits<double>::quiet_NaN());
    for (std::size_t i = 1; i < prices.size(); ++i) {
        const double prev = prices[i - 1];
        const double curr = prices[i];
        rets.push_back((curr - prev) / prev);
    }
    return rets;
}

std::vector<double> calcVolatility(const std::vector<double> &returns, std::size_t period) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> vol;
    vol.reserve(returns.size());
    double sum = 0.0;
    double sumSq = 0.0;

    for (std::size_t i = 0; i < returns.size(); ++i) {
        const double r = returns[i];
        if (std::isnan(r)) {
            vol.push_back(nan);
            continue;
        }

        sum += r;
        sumSq += r * r;
        if (i >= period) {
            const double old = returns[i - period];
            sum -= old;
            sumSq -= old * old;
        }

        if (i + 1 >= period) {
            const double mean = sum / static_cast<double>(period);
            const double variance = (sumSq / static_cast<double>(period)) - (mean * mean);
            vol.push_back(std::sqrt(std::max(variance, 0.0)));
        } else {
            vol.push_back(nan);
        }
    }

    return vol;
}

struct AbsReturnCmp {
    bool operator()(const std::pair<double, std::size_t> &lhs,
                    const std::pair<double, std::size_t> &rhs) const {
        return std::abs(lhs.first) < std::abs(rhs.first);
    }
};

void detectPatterns(const Records &records,
                    const std::vector<double> &ma5,
                    const std::vector<double> &ma10,
                    const std::vector<double> &returns,
                    std::ostringstream &out) {
    std::map<std::string, int> patternCounts;
    std::priority_queue<std::pair<double, std::size_t>,
                        std::vector<std::pair<double, std::size_t>>, AbsReturnCmp>
        topMoves;

    for (std::size_t i = 1; i < records.size(); ++i) {
        const double priceChange = (records[i].price - records[i - 1].price) / records[i - 1].price;
        topMoves.emplace(priceChange, i);

        if (priceChange > 0.03) {
            out << records[i].timestamp << " Price Spike Detected (" << priceChange * 100.0
                << "%)\n";
            ++patternCounts["Price Spike Detected"];
        }
        if (!std::isnan(ma5[i]) && !std::isnan(ma10[i]) && ma5[i] > ma10[i] &&
            ma5[i - 1] <= ma10[i - 1]) {
            out << records[i].timestamp << " Bullish Crossover\n";
            ++patternCounts["Bullish Crossover"];
        }
    }

    out << "\nTop absolute moves (by return):\n";
    const std::size_t maxItems = std::min<std::size_t>(5, topMoves.size());
    for (std::size_t i = 0; i < maxItems; ++i) {
        const auto &top = topMoves.top();
        const double ret = top.first;
        const std::size_t idx = top.second;
        out << records[idx].timestamp << " return=" << std::fixed << std::setprecision(2)
            << ret * 100.0 << "% price=" << records[idx].price << "\n";
        topMoves.pop();
    }

    out << "\nPattern counts:\n";
    for (const auto &kv : patternCounts) {
        out << kv.first << ": " << kv.second << "\n";
    }
}

int main() {
    try {
        const std::string inputFile = "sample_data.csv";
        const std::string outputFile = "results.txt";

        Records records = loadData(inputFile);
        if (records.empty()) {
            std::cerr << "No data loaded from " << inputFile << "\n";
            return 1;
        }

        std::vector<double> prices;
        prices.reserve(records.size());
        for (const auto &rec : records) {
            prices.push_back(rec.price);
        }

        auto ma5 = calcMovingAverage(prices, 5);
        auto ma10 = calcMovingAverage(prices, 10);
        auto returns = calcReturns(prices);
        auto vol5 = calcVolatility(returns, 5);

        std::ostringstream out;
        out << "Market Data Analysis\n";
        out << "Rows: " << records.size() << "\n\n";
        out << "timestamp,price,volume,MA5,MA10,return,vol5\n";
        for (std::size_t i = 0; i < records.size(); ++i) {
            out << records[i].timestamp << "," << records[i].price << "," << records[i].volume
                << ",";
            out << std::fixed << std::setprecision(4);
            out << (std::isnan(ma5[i]) ? std::string("nan") : std::to_string(ma5[i])) << ",";
            out << (std::isnan(ma10[i]) ? std::string("nan") : std::to_string(ma10[i])) << ",";
            out << (std::isnan(returns[i]) ? std::string("nan") : std::to_string(returns[i]))
                << ",";
            out << (std::isnan(vol5[i]) ? std::string("nan") : std::to_string(vol5[i])) << "\n";
        }

        out << "\nPattern detection:\n";
        detectPatterns(records, ma5, ma10, returns, out);

        std::ofstream fout(outputFile);
        fout << out.str();
        std::cout << out.str();
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}

