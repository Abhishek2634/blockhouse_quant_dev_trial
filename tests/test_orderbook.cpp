#include "../include/orderbook.h"
#include <cassert>
#include <iostream>

void test_add_order() {
    OrderBook book;
    MBOEntry entry = {"2025-07-29T10:00:00Z", 12345, 100.50, 1000, 'A', Side::BID};
    book.process_mbo(entry);
    
    auto snapshot = book.get_mbp_snapshot("2025-07-29T10:00:00Z");
    assert(snapshot.bids[0].price == 100.50);
    assert(snapshot.bids[0].size == 1000);
    assert(snapshot.bids[0].count == 1);
    std::cout << "✅ Add order test passed" << std::endl;
}

void test_trade_logic() {
    OrderBook book;
    
    // Add BID order
    MBOEntry add_bid = {"", 12345, 100.50, 1000, 'A', Side::BID};
    book.process_mbo(add_bid);
    
    // Execute trade (T action on ASK side should reduce BID side)
    MBOEntry trade = {"", 0, 100.50, 200, 'T', Side::ASK};
    book.process_mbo(trade);
    
    auto snapshot = book.get_mbp_snapshot("");
    assert(snapshot.bids[0].size == 800);  // 1000 - 200
    std::cout << "✅ Trade logic test passed" << std::endl;
}

void test_price_ordering() {
    OrderBook book;
    
    // Add multiple bids
    book.process_mbo({"", 1, 100.00, 100, 'A', Side::BID});
    book.process_mbo({"", 2, 101.00, 100, 'A', Side::BID});
    book.process_mbo({"", 3, 99.00, 100, 'A', Side::BID});
    
    auto snapshot = book.get_mbp_snapshot("");
    assert(snapshot.bids[0].price == 101.00);  // Highest bid first
    assert(snapshot.bids[1].price == 100.00);
    assert(snapshot.bids[2].price == 99.00);
    std::cout << "✅ Price ordering test passed" << std::endl;
}

int main() {
    test_add_order();
    test_trade_logic();
    test_price_ordering();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
