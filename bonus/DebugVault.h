#pragma once
// Debug Vault: a deduction game with generated, non-repeating 4-digit access codes.
// Logic is separate from the raylib screen so it can be tested in isolation.
#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

namespace debug_vault {
struct Clue {
    std::string guess;
    int exact = 0, misplaced = 0;
};
enum class Phase { Guessing, Unlocked, LockedOut };

class Session {
public:
    explicit Session(uint32_t seed = std::random_device{}()) : rng_(seed) { NewCode(); }
    int Round() const { return round_; }
    int Wins() const { return wins_; }
    int Limit() const { return round_ < 3 ? 8 : (round_ < 6 ? 7 : 6); }
    Phase State() const { return phase_; }
    const std::string& Entry() const { return entry_; }
    const std::vector<Clue>& History() const { return history_; }
    // Code is revealed only at the end; returning an empty string during play prevents UI spoilers.
    std::string RevealedCode() const { return phase_ == Phase::Guessing ? "" : secret_; }

    bool Enter(char digit) {
        if (phase_ != Phase::Guessing || digit < '1' || digit > '6' || entry_.size() == 4 ||
            entry_.find(digit) != std::string::npos) return false;
        entry_ += digit; return true;
    }
    void Delete() { if (phase_ == Phase::Guessing && !entry_.empty()) entry_.pop_back(); }
    void Clear() { if (phase_ == Phase::Guessing) entry_.clear(); }
    bool Submit() {
        if (phase_ != Phase::Guessing || entry_.size() != 4) return false;
        Clue clue; clue.guess = entry_;
        for (int i = 0; i < 4; ++i) {
            if (entry_[static_cast<size_t>(i)] == secret_[static_cast<size_t>(i)]) ++clue.exact;
            else if (secret_.find(entry_[static_cast<size_t>(i)]) != std::string::npos) ++clue.misplaced;
        }
        history_.push_back(clue);
        entry_.clear();
        if (clue.exact == 4) { phase_ = Phase::Unlocked; ++wins_; }
        else if (static_cast<int>(history_.size()) >= Limit()) phase_ = Phase::LockedOut;
        return true;
    }
    void NewCode() {
        if (phase_ == Phase::Unlocked) ++round_;
        std::array<char, 6> alphabet{{'1','2','3','4','5','6'}};
        std::shuffle(alphabet.begin(), alphabet.end(), rng_);
        secret_.assign(alphabet.begin(), alphabet.begin() + 4);
        entry_.clear(); history_.clear(); phase_ = Phase::Guessing;
    }

private:
    std::mt19937 rng_;
    std::string secret_, entry_;
    std::vector<Clue> history_;
    Phase phase_ = Phase::Guessing;
    int round_ = 1, wins_ = 0;
};
} // namespace debug_vault
