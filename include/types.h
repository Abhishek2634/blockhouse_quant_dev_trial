#pragma once

#include <cstdint>
#include <string>
#include <vector>
using namespace std;

// Enum for order side
enum class Side { BID, ASK, NONE };

// Represents a single MBO message from the input CSV
struct MBOEntry {
    string ts_recv;
    uint64_t order_id;
    double price;
    uint64_t size;
    char action;
    Side side;
};

// Represents a single order stored in our order book for fast lookup
struct Order {
    double price;
    uint64_t size;
    Side side;
};

// Represents an aggregated price level in the book
struct PriceLevel {
    uint64_t total_size = 0;
    uint64_t order_count = 0;
};

// Represents one level of the MBP book for output
struct MBPLevel {
    double price = 0.0;
    uint64_t size = 0;
    uint64_t count = 0;
};

// Represents a full snapshot of the MBP-10 book at a point in time
struct OrderBookSnapshot {
    string ts_recv;
    vector<MBPLevel> bids;
    vector<MBPLevel> asks;

    OrderBookSnapshot() : bids(10), asks(10) {}
};
