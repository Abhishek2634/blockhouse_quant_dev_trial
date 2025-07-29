CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -march=native
INCLUDES = -Iinclude

# Main program
TARGET = orderbook
SOURCES = src/main.cpp src/orderbook.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Test programs
TEST_TARGET = run_tests
BENCH_TARGET = benchmark_exe

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run the main program
run: $(TARGET)
	./$(TARGET) data/mbo.csv

# Build and run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): tests/test_orderbook.cpp src/orderbook.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $(TEST_TARGET)

# Build and run benchmarks (fixed naming to avoid conflicts)
benchmark: $(BENCH_TARGET)
	./$(BENCH_TARGET)

$(BENCH_TARGET): tests/benchmark.cpp src/orderbook.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $(BENCH_TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET) $(BENCH_TARGET)

.PHONY: all run test benchmark clean
