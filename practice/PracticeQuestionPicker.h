#pragma once

#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>
#include "PracticeQuestions.h"

// No raylib dependency: quizzes can be tested without opening a window.
// Shows all eligible questions once before recycling them; newly unlocked
// difficulties enter the same pool. Never repeats the previous question if
// another eligible question exists. Every session gets its own seeded picker.
class PracticeQuestionPicker {
    std::mt19937 random_;
    std::vector<unsigned char> seen_;
    int last_ = -1;

public:
    explicit PracticeQuestionPicker(std::uint32_t seed) : random_(seed) {}

    int Next(const std::vector<PracticeQuestion>& bank, int maxDifficulty) {
        if (bank.empty()) return -1;
        if (seen_.size() != bank.size()) { seen_.assign(bank.size(), 0); last_ = -1; }
        std::vector<int> eligible;
        std::vector<int> unused;
        for (std::size_t i = 0; i < bank.size(); ++i) {
            if (bank[i].difficulty > maxDifficulty) continue;
            eligible.push_back(static_cast<int>(i));
            if (!seen_[i]) unused.push_back(static_cast<int>(i));
        }
        if (eligible.empty()) return -1;
        if (unused.empty()) {
            for (int index : eligible) seen_[static_cast<std::size_t>(index)] = 0;
            unused = eligible;
        }
        // If this is a newly shuffled cycle, avoid a boundary repeat.
        if (unused.size() > 1) {
            unused.erase(std::remove(unused.begin(), unused.end(), last_), unused.end());
        }
        std::uniform_int_distribution<std::size_t> choice(0, unused.size() - 1);
        const int selected = unused[choice(random_)];
        seen_[static_cast<std::size_t>(selected)] = 1;
        last_ = selected;
        return selected;
    }
};
