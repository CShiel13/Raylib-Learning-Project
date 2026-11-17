#pragma once

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

// Cycle through each pattern family in randomized order, without repeats
// within a cycle or across a shuffle boundary.
class PracticeStyleBag {
    std::mt19937 random_;
    std::vector<int> order_;
    std::size_t next_ = 0;
    int last_ = -1;

public:
    PracticeStyleBag(int styleCount, std::uint32_t seed) : random_(seed) {
        if (styleCount > 0) {
            order_.resize(static_cast<std::size_t>(styleCount));
            std::iota(order_.begin(), order_.end(), 0);
        }
    }

    int Next() {
        if (order_.empty()) return -1;
        if (next_ == order_.size()) next_ = 0;
        if (next_ == 0) {
            std::shuffle(order_.begin(), order_.end(), random_);
            // The last item in one shuffle should not also start the next.
            if (order_.size() > 1 && order_.front() == last_) {
                std::uniform_int_distribution<std::size_t> other(1, order_.size() - 1);
                std::swap(order_.front(), order_[other(random_)]);
            }
        }
        last_ = order_[next_++];
        return last_;
    }
};

