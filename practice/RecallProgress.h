#pragma once

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "RecallDeck.h"

// No raylib, Player or compiler dependencies: this model is unit-testable.
// The file is intentionally separate from course saves and carries no coins/XP.
class RecallProgress {
public:
    struct Memory {
        int box = 0;                  // Unassisted recalls completed consecutively (0..5).
        std::int64_t dueDay = 0;      // UTC day; 0 for new cards.
        unsigned attempts = 0;
        bool recalledUnaided = false;
    };

    explicit RecallProgress(std::string file = "cppquest_recall.dat")
        : path_(std::move(file)), memory_(RecallDeck::Cards().size()) {}

    static std::vector<std::string> Tokens(const std::string& text) {
        // Token-based matching tolerates formatting, NOT arbitrary different C++.
        // Quotes/escapes and identifier boundaries are preserved.
        std::vector<std::string> tokens;
        for (std::size_t i = 0; i < text.size();) {
            const unsigned char c = static_cast<unsigned char>(text[i]);
            if (std::isspace(c)) { ++i; continue; }
            const std::size_t begin = i;
            if (std::isalpha(c) || c == '_') {
                while (++i < text.size() && (std::isalnum(static_cast<unsigned char>(text[i])) || text[i] == '_')) {}
            } else if (std::isdigit(c)) {
                while (++i < text.size() && std::isdigit(static_cast<unsigned char>(text[i]))) {}
            } else if (c == '\'' || c == '"') {
                const char quote = text[i++];
                while (i < text.size()) {
                    if (text[i] == '\\' && i + 1 < text.size()) { i += 2; continue; }
                    if (text[i++] == quote) break;
                }
            } else {
                // Adjacent operator characters are not interchangeable with separated ones.
                static const char* compounds[] = {"==", "!=", "<=", ">=", "&&", "||", "++", "--", "+=", "-=", "<<", ">>", "->", "*=", "/=", "%="};
                bool compound = false;
                for (const char* op : compounds) if (text.compare(i, 2, op) == 0) {
                    i += 2; compound = true; break;
                }
                if (!compound) ++i;
            }
            tokens.emplace_back(text.substr(begin, i - begin));
        }
        return tokens;
    }

    static bool Matches(const RecallCard& card, const std::string& input) {
        const auto typed = Tokens(input);
        if (typed.empty() || input.size() > 180) return false;
        for (const auto& answer : card.accepted)
            if (typed == Tokens(answer)) return true;
        return false;
    }

    const Memory& At(std::size_t index) const { return memory_.at(index); }
    std::size_t Size() const { return memory_.size(); }

    static std::int64_t IntervalDays(int box) {
        static const std::int64_t intervals[] = {0, 1, 3, 7, 14, 30};
        return intervals[std::max(0, std::min(5, box))];
    }

    void Record(std::size_t index, bool correct, bool usedHint, std::int64_t today) {
        Memory& m = memory_.at(index);
        ++m.attempts;
        if (correct && !usedHint) {
            m.box = std::min(5, m.box + 1);
            m.recalledUnaided = true;
            m.dueDay = today + IntervalDays(m.box);
        } else if (correct) {
            // Assisted success is useful practice, but not evidence of unaided recall.
            m.box = 0;
            m.dueDay = today + 1;
        } else {
            m.box = 0;
            m.dueDay = today; // retry after intervening cards in this session
        }
    }

    int UnlockedTier() const {
        int learned = 0;
        for (const auto& m : memory_) if (m.recalledUnaided) ++learned;
        return learned >= 14 ? 3 : (learned >= 6 ? 2 : 1);
    }

    int CountDue(std::int64_t today) const {
        int n = 0;
        for (std::size_t i = 0; i < memory_.size(); ++i)
            if (RecallDeck::Cards()[i].tier <= UnlockedTier() && memory_[i].attempts && memory_[i].dueDay <= today) ++n;
        return n;
    }
    int CountNew() const {
        int n = 0;
        for (std::size_t i = 0; i < memory_.size(); ++i)
            if (RecallDeck::Cards()[i].tier <= UnlockedTier() && !memory_[i].attempts) ++n;
        return n;
    }
    int CountLearned() const {
        int n = 0;
        for (const auto& m : memory_) if (m.recalledUnaided) ++n;
        return n;
    }

    bool Load() {
        memory_.assign(RecallDeck::Cards().size(), Memory{});
        std::ifstream f(path_, std::ios::binary);
        if (!f) return false;
        f.seekg(0, std::ios::end);
        const auto length = f.tellg();
        if (length < 0 || length > 16384) return false;
        f.seekg(0);
        std::string magic;
        int count = 0;
        if (!(f >> magic >> count) || magic != "RECALL1" || count < 0 || count > 128) return false;
        const auto& cards = RecallDeck::Cards();
        std::vector<Memory> loaded(cards.size());
        for (int n = 0; n < count; ++n) {
            std::string id; Memory m; int recalled = 0;
            if (!(f >> id >> m.box >> m.dueDay >> m.attempts >> recalled) ||
                id.size() > 64 || m.box < 0 || m.box > 5 || m.dueDay < 0 ||
                m.dueDay > 5000000 || m.attempts > 1000000 || (recalled != 0 && recalled != 1)) return false;
            for (std::size_t i = 0; i < cards.size(); ++i) if (id == cards[i].id) {
                m.recalledUnaided = recalled != 0;
                loaded[i] = m;
                break;
            }
        }
        memory_ = std::move(loaded);
        return true;
    }

    bool Save() const {
        std::ofstream f(path_, std::ios::binary | std::ios::trunc);
        if (!f) return false;
        const auto& cards = RecallDeck::Cards();
        f << "RECALL1 " << cards.size() << '\n';
        for (std::size_t i = 0; i < cards.size(); ++i) {
            const auto& m = memory_[i];
            f << cards[i].id << ' ' << m.box << ' ' << m.dueDay << ' ' << m.attempts
              << ' ' << (m.recalledUnaided ? 1 : 0) << '\n';
        }
        f.flush();
        return f.good();
    }

private:
    std::string path_;
    std::vector<Memory> memory_;
};
