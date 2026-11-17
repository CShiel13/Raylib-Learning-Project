#pragma once
// Pure C++ game logic; UI and XP awarding are handled by Game.
#include <algorithm>
#include <array>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "practice/PracticeQuestions.h"

namespace engagement {
class RapidReview {
public:
    RapidReview() { Reset(); }
    void Reset() {
        std::random_device rd; gen_.seed(rd());
        deck_.clear();
        for (int kind = 0; kind < 3; ++kind) {
            auto k = static_cast<PracticeMiniGameKind>(kind);
            auto& v = PracticeQuestionBank::For(k);
            for (size_t i = 0; i < v.size(); ++i) deck_.push_back({kind, static_cast<int>(i)});
        }
        std::shuffle(deck_.begin(), deck_.end(), gen_);
        cursor_ = correct_ = attempts_ = 0; seconds_ = 60.0f;
        running_ = finished_ = false;
        ShuffleOptions();
    }
    void Start() { if (!finished_) running_ = true; }
    void Tick(float dt) {
        if (!running_) return;
        seconds_ = std::max(0.0f, seconds_ - std::max(0.0f, dt));
        if (seconds_ <= 0 || cursor_ >= static_cast<int>(deck_.size())) Stop();
    }
    bool Answer(int slot) {
        if (!running_ || slot < 0 || slot >= 4 || cursor_ >= static_cast<int>(deck_.size())) return false;
        bool ok = order_[slot] == Question().answer;
        ++attempts_; if (ok) ++correct_;
        else seconds_ = std::max(0.0f, seconds_ - 2.0f); // gentle time penalty
        ++cursor_;
        if (seconds_ <= 0 || cursor_ >= static_cast<int>(deck_.size())) Stop();
        else ShuffleOptions();
        return ok;
    }
    void Stop() { running_ = false; finished_ = true; }
    const PracticeQuestion& Question() const {
        const auto& ref = deck_[std::min(cursor_, static_cast<int>(deck_.size()) - 1)];
        return PracticeQuestionBank::For(static_cast<PracticeMiniGameKind>(ref[0]))[ref[1]];
    }
    const std::array<int,4>& Options() const { return order_; }
    bool Running() const { return running_; }
    bool Finished() const { return finished_; }
    int Correct() const { return correct_; }
    int Attempts() const { return attempts_; }
    int TimeLeft() const { return static_cast<int>(seconds_ + 0.999f); }
    int RewardXP() const { return std::min(75, correct_ * 4); }
private:
    void ShuffleOptions() { order_ = {0,1,2,3}; std::shuffle(order_.begin(), order_.end(), gen_); }
    std::mt19937 gen_;
    std::vector<std::array<int,2>> deck_;
    std::array<int,4> order_{};
    int cursor_ = 0, correct_ = 0, attempts_ = 0;
    float seconds_ = 60;
    bool running_ = false, finished_ = false;
};

struct MatchPair { const char* term; const char* meaning; };
inline const std::array<MatchPair,24>& MatchBank() {
    static const std::array<MatchPair,24> bank{{
        {"const", "Cannot be reassigned"}, {"if", "Conditional branch"},
        {"else", "Alternative branch"}, {"for", "Counted loop"},
        {"while", "Condition-based loop"}, {"break", "Exit nearest loop"},
        {"continue", "Skip to next iteration"}, {"return", "Exit a function"},
        {"bool", "True or false type"}, {"int", "Integer type"},
        {"double", "Floating-point type"}, {"char", "Character type"},
        {"std::vector", "Resizable sequence"}, {"std::string", "Text sequence"},
        {"std::cout", "Standard output stream"}, {"std::cin", "Standard input stream"},
        {"==", "Equality comparison"}, {"!=", "Inequality comparison"},
        {"&&", "Logical AND"}, {"||", "Logical OR"},
        {"%", "Remainder operator"}, {"++i", "Prefix increment"},
        {"i++", "Postfix increment"}, {"nullptr", "Null pointer literal"}
    }};
    return bank;
}
class MatchMadness {
public:
    struct Tile { int pair = 0; bool meaning = false; bool matched = false; };
    MatchMadness() { Reset(); }
    void Reset() {
        std::random_device rd; gen_.seed(rd());
        indices_.resize(MatchBank().size());
        std::iota(indices_.begin(), indices_.end(), 0);
        std::shuffle(indices_.begin(), indices_.end(), gen_);
        board_ = 0; matched_ = 0; misses_ = 0; first_ = -1; seconds_ = 60;
        running_ = finished_ = false; MakeBoard();
    }
    void Start() { if (!finished_) running_ = true; }
    void Tick(float dt) {
        if (!running_) return;
        seconds_ = std::max(0.0f, seconds_ - std::max(0.0f, dt));
        if (seconds_ <= 0) Stop();
    }
    void Choose(int index) {
        if (!running_ || index < 0 || index >= 8 || tiles_[index].matched) return;
        if (first_ < 0) { first_ = index; return; }
        if (first_ == index) { first_ = -1; return; }
        if (tiles_[first_].pair == tiles_[index].pair && tiles_[first_].meaning != tiles_[index].meaning) {
            tiles_[first_].matched = tiles_[index].matched = true;
            ++matched_;
            if (matched_ % 4 == 0) {
                ++board_;
                if (board_ >= 3) Stop();
                else MakeBoard();
            }
        } else {
            ++misses_; seconds_ = std::max(0.0f, seconds_ - 2.0f);
        }
        first_ = -1;
        if (seconds_ <= 0) Stop();
    }
    bool Running() const { return running_; }
    bool Finished() const { return finished_; }
    int First() const { return first_; }
    int Matched() const { return matched_; }
    int Misses() const { return misses_; }
    int Board() const { return board_; }
    int TimeLeft() const { return static_cast<int>(seconds_ + 0.999f); }
    int RewardXP() const { return std::min(60, matched_ * 5); }
    const std::array<Tile,8>& Tiles() const { return tiles_; }
    std::string Label(int slot) const {
        if (slot < 0 || slot >= 8) return {};
        const auto& p = MatchBank()[tiles_[slot].pair];
        return tiles_[slot].meaning ? p.meaning : p.term;
    }
    void Stop() { running_ = false; finished_ = true; first_ = -1; }
private:
    void MakeBoard() {
        for (int i = 0; i < 4; ++i) {
            tiles_[i] = {indices_[board_ * 4 + i], false, false};
            tiles_[i+4] = {indices_[board_ * 4 + i], true, false};
        }
        std::shuffle(tiles_.begin(), tiles_.end(), gen_);
    }
    std::mt19937 gen_;
    std::vector<int> indices_;
    std::array<Tile,8> tiles_{};
    int board_ = 0, matched_ = 0, misses_ = 0, first_ = -1;
    float seconds_ = 60;
    bool running_ = false, finished_ = false;
};
} // namespace engagement
