CXX = c++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra -Wpedantic
TARGET = cumulative_sum
TEST_TARGET = cumulative_sum_tests

.PHONY: all test clean

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

$(TEST_TARGET): tests/test_cumulative_sum.cpp main.cpp
	$(CXX) $(CXXFLAGS) tests/test_cumulative_sum.cpp -o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	$(RM) $(TARGET) $(TARGET).exe $(TEST_TARGET) $(TEST_TARGET).exe
