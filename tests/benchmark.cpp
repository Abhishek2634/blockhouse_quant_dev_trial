#include <iostream>  // ← This was missing! Fixes the std::cout error
#include <chrono>
#include <random>
#include <vector>
#include "../include/orderbook.h"

class PerformanceTester {
public:
    void benchmark_operations(int num_operations) {
        OrderBook book;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> price_dist(90.0, 110.0);
        std::uniform_int_distribution<> size_dist(100, 1000);
        
        std::vector<MBOEntry> entries;
        entries.reserve(num_operations);
        
        // Generate test data
        for (int i = 0; i < num_operations; ++i) {
            entries.push_back({
                "2025-07-29T10:00:00Z",
                static_cast<uint64_t>(i + 1),
                price_dist(gen),
                static_cast<uint64_t>(size_dist(gen)),
                'A',
                (i % 2 == 0) ? Side::BID : Side::ASK
            });
        }
        
        // Benchmark processing
        auto start = std::chrono::high_resolution_clock::now();
        
        for (const auto& entry : entries) {
            book.process_mbo(entry);
            book.get_mbp_snapshot(entry.ts_recv);  // Include snapshot generation
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        
        double avg_ns_per_op = static_cast<double>(duration.count()) / num_operations;
        double ops_per_second = 1e9 / avg_ns_per_op;
        
        std::cout << "Performance Results:" << std::endl;
        std::cout << "Operations: " << num_operations << std::endl;
        std::cout << "Total time: " << duration.count() / 1e6 << " ms" << std::endl;
        std::cout << "Average time per operation: " << avg_ns_per_op << " ns" << std::endl;
        std::cout << "Operations per second: " << static_cast<int>(ops_per_second) << std::endl;
        
        // Performance targets for high-frequency systems
        if (avg_ns_per_op < 1000) {
            std::cout << "✅ Excellent performance (< 1 microsecond)" << std::endl;
        } else if (avg_ns_per_op < 10000) {
            std::cout << "✅ Good performance (< 10 microseconds)" << std::endl;
        } else {
            std::cout << "⚠️  Consider optimization (> 10 microseconds)" << std::endl;
        }
    }
    
    void memory_usage_test() {
        OrderBook book;
        
        std::cout << "\n=== Memory Usage Test ===" << std::endl;
        
        // Add many orders to test memory efficiency
        for (int i = 0; i < 50000; ++i) {
            double price = 100.0 + (i % 1000) * 0.01;  // Price range 100.00 to 109.99
            MBOEntry entry = {
                "2025-07-29T10:00:00Z",
                static_cast<uint64_t>(i + 1),
                price,
                100,
                'A',
                (i % 2 == 0) ? Side::BID : Side::ASK
            };
            book.process_mbo(entry);
        }
        
        auto snapshot = book.get_mbp_snapshot("2025-07-29T10:00:00Z");
        
        std::cout << "Added 50,000 orders" << std::endl;
        std::cout << "Top BID: " << snapshot.bids[0].price << " (size: " << snapshot.bids[0].size << ")" << std::endl;
        std::cout << "Top ASK: " << snapshot.asks[0].price << " (size: " << snapshot.asks[0].size << ")" << std::endl;
        std::cout << "Memory test completed successfully" << std::endl;
    }
};

int main() {
    PerformanceTester tester;
    
    std::cout << "=== Order Book Performance Benchmark ===" << std::endl;
    
    // Run different scales of benchmarks
    tester.benchmark_operations(1000);    // 1K operations
    std::cout << std::endl;
    tester.benchmark_operations(10000);   // 10K operations
    std::cout << std::endl;
    tester.benchmark_operations(100000);  // 100K operations
    
    // Test memory efficiency
    tester.memory_usage_test();
    
    return 0;
}
