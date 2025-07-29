#pragma once

#include "types.h"
#include <map>
#include <unordered_map>
#include <string>

class OrderBook {
public:
    void process_mbo(const MBOEntry& entry);
    OrderBookSnapshot get_mbp_snapshot(const std::string& ts_recv) const;
    void clear_book(); // Handles the 'R' reset event

private:
    void add_order(const MBOEntry& entry);
    void cancel_order(const MBOEntry& entry);
    void execute_trade(const MBOEntry& entry);

    std::map<double, PriceLevel, std::greater<double>> bids_;  // Descending order
    std::map<double, PriceLevel> asks_;                       // Ascending order
    std::unordered_map<uint64_t, Order> orders_;
};
