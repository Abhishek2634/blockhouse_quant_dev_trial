#include "orderbook.h"
#include <iostream>

// Uncomment the next line to enable debug output for trade processing
// #define DEBUG_TRADES

void OrderBook::clear_book() {
    bids_.clear();
    asks_.clear();
    orders_.clear();
}

void OrderBook::process_mbo(const MBOEntry& entry) {
    switch (entry.action) {
        case 'A':
            add_order(entry);
            break;
        case 'C':
            cancel_order(entry);
            break;
        case 'T':
            execute_trade(entry); 
            break;
        case 'R':
            clear_book();
            break;
        case 'F':
            break;
        default:
            break;
    }
}

void OrderBook::add_order(const MBOEntry& entry) {
    if (entry.order_id == 0) return;
    orders_[entry.order_id] = {entry.price, entry.size, entry.side};
    if (entry.side == Side::BID) {
        bids_[entry.price].total_size += entry.size;
        bids_[entry.price].order_count++;
    } else if (entry.side == Side::ASK) {
        asks_[entry.price].total_size += entry.size;
        asks_[entry.price].order_count++;
    }
}

void OrderBook::cancel_order(const MBOEntry& entry) {
    auto it = orders_.find(entry.order_id);
    if (it == orders_.end()) return;

    Order& order = it->second;
    uint64_t cancel_size = entry.size;
    bool is_full_cancel = false;
    
    // Store the side and price before potentially erasing the order
    Side order_side = order.side;
    double order_price = order.price;

    if (cancel_size >= order.size) {
        cancel_size = order.size;
        is_full_cancel = true;
        orders_.erase(it);
    } else {
        order.size -= cancel_size;
    }

    // Use stored values instead of potentially invalid order reference
    if (order_side == Side::BID) {
        auto& bid_level = bids_[order_price];
        bid_level.total_size -= cancel_size;
        if (is_full_cancel) bid_level.order_count--;
        if (bid_level.order_count == 0) bids_.erase(order_price);
    } else if (order_side == Side::ASK) {
        auto& ask_level = asks_[order_price];
        ask_level.total_size -= cancel_size;
        if (is_full_cancel) ask_level.order_count--;
        if (ask_level.order_count == 0) asks_.erase(order_price);
    }
}

void OrderBook::execute_trade(const MBOEntry& entry) {
    if (entry.side == Side::NONE) return;

#ifdef DEBUG_TRADES
    std::cerr << "DEBUG TRADE: action=" << entry.action 
              << " side=" << (entry.side == Side::BID ? "B" : (entry.side == Side::ASK ? "A" : "N"))
              << " price=" << entry.price 
              << " size=" << entry.size 
              << " order_id=" << entry.order_id << std::endl;
#endif


    // This handles the T->F->C sequence requirement
    if (entry.side == Side::ASK) {
        auto bid_it = bids_.find(entry.price);
        if (bid_it != bids_.end()) {
#ifdef DEBUG_TRADES
            std::cerr << "DEBUG: Found BID level at " << entry.price 
                      << " with size " << bid_it->second.total_size << std::endl;
#endif
            bid_it->second.total_size -= entry.size;
#ifdef DEBUG_TRADES
            std::cerr << "DEBUG: After trade, BID size = " << bid_it->second.total_size << std::endl;
#endif
            if (bid_it->second.total_size <= 0) {
#ifdef DEBUG_TRADES
                std::cerr << "DEBUG: Removing empty BID level" << std::endl;
#endif
                bids_.erase(bid_it);
            }
        }
#ifdef DEBUG_TRADES
        else {
            std::cerr << "DEBUG: WARNING - No BID level found at " << entry.price << std::endl;
        }
#endif
    } else if (entry.side == Side::BID) {
        auto ask_it = asks_.find(entry.price);
        if (ask_it != asks_.end()) {
#ifdef DEBUG_TRADES
            std::cerr << "DEBUG: Found ASK level at " << entry.price 
                      << " with size " << ask_it->second.total_size << std::endl;
#endif
            ask_it->second.total_size -= entry.size;
#ifdef DEBUG_TRADES
            std::cerr << "DEBUG: After trade, ASK size = " << ask_it->second.total_size << std::endl;
#endif
            if (ask_it->second.total_size <= 0) {
#ifdef DEBUG_TRADES
                std::cerr << "DEBUG: Removing empty ASK level" << std::endl;
#endif
                asks_.erase(ask_it);
            }
        }
#ifdef DEBUG_TRADES
        else {
            std::cerr << "DEBUG: WARNING - No ASK level found at " << entry.price << std::endl;
        }
#endif
    }
}

OrderBookSnapshot OrderBook::get_mbp_snapshot(const std::string& ts_recv) const {
    OrderBookSnapshot snapshot;
    snapshot.ts_recv = ts_recv;

    int level = 0;
    for (auto const& [price, price_level] : asks_) {
        if (level >= 10) break;
        snapshot.asks[level] = {price, price_level.total_size, price_level.order_count};
        level++;
    }

    level = 0;
    for (auto const& [price, price_level] : bids_) {
        if (level >= 10) break;
        snapshot.bids[level] = {price, price_level.total_size, price_level.order_count};
        level++;
    }

    return snapshot;
}
