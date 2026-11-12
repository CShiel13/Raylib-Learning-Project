#pragma once
#include <algorithm>
#include <array>
#include <cstddef>

namespace sh2 {
// Last-ten-keys recogniser: a mistaken or extra key does not require leaving
// the screen, and the last complete occurrence of the code still works.
class KonamiCode {
public:
    explicit KonamiCode(std::array<int, 10> sequence) : sequence_(sequence) {}
    void Reset() { size_ = 0; }
    bool Feed(int key) {
        if (key == 0) return false;
        if (size_ < history_.size()) history_[size_++] = key;
        else {
            std::move(history_.begin() + 1, history_.end(), history_.begin());
            history_.back() = key;
        }
        if (size_ != sequence_.size() || history_ != sequence_) return false;
        Reset();
        return true;
    }
private:
    std::array<int, 10> sequence_{};
    std::array<int, 10> history_{};
    std::size_t size_ = 0;
};
} // namespace sh2
