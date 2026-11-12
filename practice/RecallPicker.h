#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "RecallProgress.h"

struct RecallPick {
    int index = -1;
    bool earlyFreeplay = false;
};

// Pure selection policy: due reviews first, then new cards, then optional early
// freeplay; prefer a different skill and avoid immediate retries after mistakes.
class RecallPicker {
public:
    static RecallPick Choose(const RecallProgress& progress, std::int64_t today,
                             int round, const std::vector<int>& retryAfter,
                             int lastIndex, const std::string& lastSkill, unsigned randomValue,
                             const std::vector<unsigned>& shownThisRun = {}) {
        const auto& cards = RecallDeck::Cards();
        int best = 99999;
        std::vector<int> ties;
        for (int pass = 0; pass < 2; ++pass) {
            best = 99999;
            ties.clear();
            for (std::size_t i = 0; i < cards.size(); ++i) {
                if (cards[i].tier > progress.UnlockedTier()) continue;
                if (pass == 0 && i < retryAfter.size() && round < retryAfter[i]) continue;
                const auto& memory = progress.At(i);
                const int band = memory.attempts == 0 ? 10 : (memory.dueDay <= today ? 0 : 20);
                // Within the same review priority band, prefer cards not yet
                // shown in this run. Due reviews still outrank brand-new cards.
                const int runPenalty = i < shownThisRun.size()
                    ? static_cast<int>(std::min(3u, shownThisRun[i])) * 4 : 0;
                const int rank = band * 100 + runPenalty
                                      + (lastSkill == cards[i].skill ? 2 : 0)
                                      + (lastIndex == static_cast<int>(i) ? 5 : 0);
                if (rank < best) { best = rank; ties.clear(); }
                if (rank == best) ties.push_back(static_cast<int>(i));
            }
            if (!ties.empty()) break;
        }
        if (ties.empty()) return {};
        return { ties[randomValue % ties.size()], best >= 2000 };
    }
};
