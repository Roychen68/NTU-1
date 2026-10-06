#include <cstddef>
#include <iostream>
#include <limits>
#include <new>
#include <vector>

namespace {

typedef unsigned long long Integer;

// Return floor(offset * bins / span), without floating-point rounding.
// Preconditions: offset < span, span > 0, bins > 0, and span <= LLONG_MAX.
Integer binIndex(Integer offset, Integer span, Integer bins) {
    // The usual case in this problem: all bins have an integer width.
    if (span % bins == 0) {
        return offset / (span / bins);
    }

    // Use the direct formula whenever its multiplication fits.
    if (offset <= std::numeric_limits<Integer>::max() / bins) {
        return (offset * bins) / span;
    }

    // For very large inputs, multiply one bit at a time while keeping
    // quotient and remainder separate. This avoids a large intermediate
    // product. Since span <= LLONG_MAX, doubling a remainder and adding
    // offset both fit in unsigned long long.
    Integer quotient = 0;
    Integer remainder = 0;
    for (int bit = std::numeric_limits<Integer>::digits - 1; bit >= 0; --bit) {
        quotient *= 2;
        remainder *= 2;
        quotient += remainder / span;
        remainder %= span;

        if (((bins >> bit) & 1ULL) != 0) {
            remainder += offset;
            quotient += remainder / span;
            remainder %= span;
        }
    }
    return quotient;
}

std::vector<Integer> countBins(const std::vector<Integer>& values,
                               Integer a, Integer b, std::size_t bins) {
    std::vector<Integer> counts(bins, 0);
    const Integer span = b - a;

    for (std::size_t i = 0; i < values.size(); ++i) {
        const Integer index = binIndex(values[i] - a, span,
                                       static_cast<Integer>(bins));
        ++counts[static_cast<std::size_t>(index)];
    }
    return counts;
}

// Print either the bin counts or their running (cumulative) sums.
void printLine(const std::vector<Integer>& counts, bool cumulative,
               std::ostream& output) {
    Integer runningSum = 0;
    for (std::size_t i = 0; i < counts.size(); ++i) {
        if (i != 0) {
            output << ' ';
        }
        runningSum += counts[i];
        output << (cumulative ? runningSum : counts[i]);
    }
    output << '\n';
}

int inputError(std::ostream& error, const char* message) {
    // Diagnostics go to stderr, so valid judge output contains only numbers.
    error << "Input error: " << message << '\n';
    return 1;
}

// Streams let the same functions work with keyboard input or C++ test data.
int solve(std::istream& input, std::ostream& output, std::ostream& error) {
    try {
        long long a = 0;
        long long b = 0;
        long long n = 0;
        if (!(input >> a >> b >> n)) {
            return inputError(error, "expected a, b, and n.");
        }
        if (a < 0 || b <= a || n < 0) {
            return inputError(error, "require 0 <= a < b and n >= 0.");
        }

        // m comes AFTER the values, so keep the values until m is read.
        std::vector<Integer> values;
        if (static_cast<Integer>(n) > values.max_size()) {
            return inputError(error, "n is too large for this platform.");
        }
        values.resize(static_cast<std::size_t>(n));
        for (std::size_t i = 0; i < values.size(); ++i) {
            long long value = 0;
            if (!(input >> value)) {
                return inputError(error, "expected n integer values.");
            }
            if (value < a || value >= b) {
                return inputError(error, "each value must satisfy a <= value < b.");
            }
            values[i] = static_cast<Integer>(value);
        }

        long long m = 0;
        if (!(input >> m) || m <= 0) {
            return inputError(error, "expected a positive bin count m.");
        }
        if (static_cast<Integer>(m) > values.max_size()) {
            return inputError(error, "m is too large for this platform.");
        }

        const std::vector<Integer> counts = countBins(
            values, static_cast<Integer>(a), static_cast<Integer>(b),
            static_cast<std::size_t>(m));
        printLine(counts, false, output);
        printLine(counts, true, output);
    } catch (const std::bad_alloc&) {
        return inputError(error, "not enough memory for the requested n or m.");
    }
    return 0;
}

}  // namespace

// The C++ test file supplies its own main; normal judge builds use this main.
#ifndef CUMULATIVE_SUM_TEST
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);
    return solve(std::cin, std::cout, std::cerr);
}
#endif
