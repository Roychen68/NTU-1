// Compile this file by itself. It includes the actual solution, disabling
// only the solution's main so that the test runner can provide its own main.
#define CUMULATIVE_SUM_TEST
#include "../main.cpp"

#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

const Integer maxInput = static_cast<Integer>(
    std::numeric_limits<long long>::max());
std::size_t validChecks = 0;
std::size_t invalidChecks = 0;
std::size_t functionChecks = 0;

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// Independent reference: construct the integer boundaries of each interval,
// then check every value against each interval. This does not call binIndex
// or multiply a value by the bin count, even for very large input values.
std::vector<Integer> referenceCounts(Integer a, Integer b,
                                     const std::vector<Integer>& values,
                                     std::size_t bins) {
    const Integer binCount = static_cast<Integer>(bins);
    const Integer span = b - a;
    const Integer step = span / binCount;
    const Integer fractionalStep = span % binCount;
    std::vector<Integer> boundaries(bins + 1, a);
    Integer whole = a;
    Integer remainder = 0;
    for (std::size_t i = 1; i <= bins; ++i) {
        whole += step;
        remainder += fractionalStep;
        if (remainder >= binCount) {
            ++whole;
            remainder -= binCount;
        }
        // An integer belongs above a fractional boundary starting at its ceil.
        boundaries[i] = whole + (remainder != 0 ? 1ULL : 0ULL);
    }

    std::vector<Integer> counts(bins, 0);
    for (std::size_t i = 0; i < bins; ++i) {
        for (std::size_t j = 0; j < values.size(); ++j) {
            if (boundaries[i] <= values[j] && values[j] < boundaries[i + 1]) {
                ++counts[i];
            }
        }
    }
    return counts;
}

std::string expectedOutput(const std::vector<Integer>& counts) {
    std::ostringstream output;
    for (std::size_t i = 0; i < counts.size(); ++i) {
        output << (i == 0 ? "" : " ") << counts[i];
    }
    output << '\n';
    Integer total = 0;
    for (std::size_t i = 0; i < counts.size(); ++i) {
        total += counts[i];
        output << (i == 0 ? "" : " ") << total;
    }
    output << '\n';
    return output.str();
}

std::string inputText(Integer a, Integer b,
                      const std::vector<Integer>& values, std::size_t bins) {
    std::ostringstream input;
    input << a << '\n' << b << '\n' << values.size() << '\n';
    for (std::size_t i = 0; i < values.size(); ++i) {
        input << (i == 0 ? "" : " ") << values[i];
    }
    input << '\n' << bins << '\n';
    return input.str();
}

void checkData(const std::string& name, const std::string& data,
               const std::string& expected) {
    std::istringstream input(data);
    std::ostringstream output;
    std::ostringstream error;
    const int result = solve(input, output, error);
    require(result == 0 && output.str() == expected && error.str().empty(),
            name + " failed\nInput:\n" + data + "Expected:\n" + expected
            + "Actual:\n" + output.str() + "Errors:\n" + error.str());
    ++validChecks;
}

void checkCase(const std::string& name, Integer a, Integer b,
               const std::vector<Integer>& values, std::size_t bins) {
    checkData(name, inputText(a, b, values, bins),
              expectedOutput(referenceCounts(a, b, values, bins)));
}

Integer randomBetween(std::mt19937_64& generator, Integer low, Integer high) {
    return std::uniform_int_distribution<Integer>(low, high)(generator);
}

void testFunctions() {
    // Call the actual C++ functions directly, independently of input parsing.
    const Integer offsets[] = {0, 24, 25, 49, 50, 74, 75, 99};
    const Integer expectedBins[] = {0, 0, 1, 1, 2, 2, 3, 3};
    for (std::size_t i = 0; i < 8; ++i) {
        require(binIndex(offsets[i], 100, 4) == expectedBins[i],
                "binIndex: exact boundary test failed");
        ++functionChecks;
    }
    // Huge bin counts can be tested directly without allocating huge vectors.
    const Integer largeOffsets[] = {0, 1, maxInput / 2,
                                    maxInput - 2, maxInput - 1};
    for (std::size_t i = 0; i < 5; ++i) {
        const Integer expected = largeOffsets[i] == 0 ? 0 : largeOffsets[i] - 1;
        require(binIndex(largeOffsets[i], maxInput, maxInput - 1) == expected,
                "binIndex: large multiplication test failed");
        ++functionChecks;
    }

    const std::vector<Integer> values = {10, 14, 15, 19, 20, 29};
    const std::vector<Integer> counts = {2, 2, 1, 1};
    require(countBins(values, 10, 30, 4) == counts,
            "countBins: nonzero lower boundary test failed");
    ++functionChecks;

    std::ostringstream output;
    printLine(counts, false, output);
    require(output.str() == "2 2 1 1\n", "printLine: count output failed");
    ++functionChecks;
    output.str("");
    output.clear();
    printLine(counts, true, output);
    require(output.str() == "2 4 5 6\n", "printLine: cumulative output failed");
    ++functionChecks;
}

void testValidCases() {
    checkData("image sample", "0\n100\n8\n3 25 83 94 39 23 16 56\n4\n",
              "3 2 1 2\n3 5 6 8\n");
    checkData("nonzero boundaries", "10\n30\n6\n10 14 15 19 20 29\n4\n",
              "2 2 1 1\n2 4 5 6\n");
    checkData("fractional width", "10\n20\n10\n10 11 12 13 14 15 16 17 18 19\n3\n",
              "4 3 3\n4 7 10\n");
    checkCase("one bin", 7, 20, {7, 9, 19, 7}, 1);
    checkCase("no values", 0, 100, {}, 4);
    checkCase("exact boundaries", 0, 100, {0, 24, 25, 49, 50, 74, 75, 99}, 4);
    checkCase("all equal", 0, 100, std::vector<Integer>(20, 25), 4);
    checkCase("empty bins", 10, 110, {10, 109}, 10);
    checkCase("more bins than integers", 0, 3, {0, 1, 2}, 5);
    checkCase("smallest range", 5, 6, {5, 5, 5}, 1);
    checkCase("many empty fractional bins", 5, 6, {5, 5, 5}, 10);
    std::vector<Integer> nearLimit;
    for (Integer value = maxInput - 10; value < maxInput; ++value) {
        nearLimit.push_back(value);
    }
    checkCase("near signed limit", maxInput - 10, maxInput, nearLimit, 3);
    checkCase("large integer width", 0, maxInput - 1,
              {0, (maxInput - 1) / 2 - 1, (maxInput - 1) / 2, maxInput - 2}, 2);
    checkCase("multiplication exceeds 64 bits", 0, maxInput,
              {0, maxInput / 3, maxInput / 3 + 1, 2 * maxInput / 3,
               2 * maxInput / 3 + 1, maxInput - 1}, 3);
    checkCase("large range and many bins", 0, maxInput,
              {0, 1, maxInput / 2, maxInput - 2, maxInput - 1}, 31);
    checkData("whitespace independent", " \t0\t20 4\t0  5\t10 19 4\n",
              "1 1 1 1\n1 2 3 4\n");

    std::mt19937_64 generator(20261007ULL);
    for (std::size_t index = 0; index < 1000; ++index) {
        Integer a = 0;
        Integer b = 0;
        switch (index % 4) {
            case 0:
                a = randomBetween(generator, 0, 999);
                b = a + randomBetween(generator, 1, 500);
                break;
            case 1:
                b = maxInput - randomBetween(generator, 0, 9999);
                break;
            case 2:
                a = maxInput - randomBetween(generator, 1, 10000);
                b = randomBetween(generator, a + 1, maxInput);
                break;
            default:
                a = randomBetween(generator, 0, maxInput / 3 - 1);
                b = randomBetween(generator, a + 1, maxInput);
                break;
        }
        const std::size_t bins = static_cast<std::size_t>(
            randomBetween(generator, 1, 40));
        std::vector<Integer> values(static_cast<std::size_t>(
            randomBetween(generator, 0, 70)));
        for (std::size_t j = 0; j < values.size(); ++j) {
            values[j] = randomBetween(generator, a, b - 1);
        }
        checkCase("random case " + std::to_string(index), a, b, values, bins);
    }

    // Translation preserves the histogram; repetition doubles every count.
    for (std::size_t index = 0; index < 20; ++index) {
        const Integer a = randomBetween(generator, 0, 99);
        const Integer b = a + randomBetween(generator, 1, 200);
        const std::size_t bins = static_cast<std::size_t>(
            randomBetween(generator, 1, 30));
        std::vector<Integer> values(static_cast<std::size_t>(
            randomBetween(generator, 0, 40)));
        for (std::size_t j = 0; j < values.size(); ++j) {
            values[j] = randomBetween(generator, a, b - 1);
        }
        std::vector<Integer> counts = referenceCounts(a, b, values, bins);
        const Integer shift = randomBetween(generator, 0, 9999);
        std::vector<Integer> shifted = values;
        for (std::size_t j = 0; j < shifted.size(); ++j) {
            shifted[j] += shift;
        }
        checkData("translated case", inputText(a + shift, b + shift, shifted, bins),
                  expectedOutput(counts));
        std::vector<Integer> repeated = values;
        repeated.insert(repeated.end(), values.begin(), values.end());
        for (std::size_t j = 0; j < counts.size(); ++j) {
            counts[j] *= 2;
        }
        checkData("repeated case", inputText(a, b, repeated, bins),
                  expectedOutput(counts));
    }
}

void testInvalidCases() {
    std::ostringstream outOfRange;
    outOfRange << "0 " << maxInput + 1 << " 0 1";
    const std::pair<std::string, std::string> cases[] = {
        {"empty input", ""},
        {"negative a", "-1 10 0 2"},
        {"equal endpoints", "5 5 0 1"},
        {"reversed endpoints", "10 5 0 1"},
        {"negative n", "0 10 -1 2"},
        {"nonnumeric header", "hello 10 0 1"},
        {"missing value", "0 10 3 1 2"},
        {"value below a", "5 10 1 4 2"},
        {"excluded upper boundary", "0 10 1 10 2"},
        {"negative value", "0 10 1 -1 2"},
        {"zero bins", "0 10 0 0"},
        {"negative bins", "0 10 0 -1"},
        {"missing bins", "0 10 0"},
        {"integer outside supported range", outOfRange.str()}
    };
    for (std::size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        std::istringstream input(cases[i].second);
        std::ostringstream output;
        std::ostringstream error;
        const int result = solve(input, output, error);
        require(result != 0 && output.str().empty()
                && error.str().find("Input error:") != std::string::npos,
                "Invalid case was not rejected: " + cases[i].first);
        ++invalidChecks;
    }
}

}  // namespace

int main() {
    try {
        testFunctions();
        testValidCases();
        testInvalidCases();
        std::cout << "PASS: " << validChecks << " valid cases, "
                  << invalidChecks << " invalid-input checks, and "
                  << functionChecks << " direct function checks.\n";
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
