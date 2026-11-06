#pragma once
// Player.h - everything that persists: XP, level, coins, boosts, quests, progress.
#include <algorithm>
#include <fstream>
#include <set>
#include <string>
#include <vector>

enum QuestKind { QK_CORRECT, QK_MODULES, QK_STREAK, QK_CODE };

struct Quest {
    int kind;
    int target;
    int reward;     // coins
    int progress;
};

class Player {
public:
    int xp = 0;
    int coins = 0;
    int hints = 2;        // hint tokens
    int boost = 0;        // permanent coin boost level (+25% each)
    int extraLife = 0;    // bonus hearts for the next lesson
    bool hardMode = false;   // modules end with a hard (LeetCode style) problem
    int gearHidden = 0;      // bit set = that unlocked gear item is switched off (see DrawAvatar)
    int outfit = 0;          // outfit colour index 0-4
    std::set<std::string> done;   // finished modules / tests
    std::set<int> unlocked;       // topics bought with coins
    std::vector<Quest> quests;

    Player() { Reset(); Load(); }

    void Reset() {
        quests = { {QK_CORRECT, 10, 10, 0}, {QK_MODULES, 2, 15, 0},
                   {QK_STREAK, 3, 10, 0},   {QK_CODE, 1, 12, 0} };
        unlocked = { 0 };
    }

    int   Level() const { return xp / 100 + 1; }
    int   XpInLevel() const { return xp % 100; }
    float CoinMult() const { return 1.0f + 0.25f * boost; }

    // Returns how many levels were gained. Each level-up pays 10 coins.
    int AddXP(int n) {
        int before = Level();
        xp += n;
        int gained = Level() - before;
        coins += 10 * gained;
        return gained;
    }

    void Note(int kind, int value) {
        for (auto& q : quests) {
            if (q.kind != kind) continue;
            if (kind == QK_STREAK) q.progress = std::max(q.progress, value);
            else q.progress += value;
        }
    }

    bool CanClaim(int i) const { return quests[i].progress >= quests[i].target; }

    void Claim(int i) {
        if (!CanClaim(i)) return;
        coins += quests[i].reward;
        hints += 1;                       // every quest also gifts a hint
        quests[i].target *= 2;            // next tier is harder...
        quests[i].reward *= 2;            // ...and pays more
        Save();
    }

    static std::string QuestText(const Quest& q) {
        std::string n = std::to_string(q.target);
        switch (q.kind) {
        case QK_CORRECT: return "Answer " + n + " questions correctly";
        case QK_MODULES: return "Complete " + n + " modules";
        case QK_STREAK:  return "Reach a streak of " + n + " correct answers";
        default:         return "Solve " + n + " code challenges";
        }
    }

    // ------------------------------------------------------------ save/load
    void Save() const {
        std::ofstream f("javaquest_save.dat");
        if (!f) return;
        f << xp << ' ' << coins << ' ' << hints << ' ' << boost << ' ' << extraLife << '\n';
        f << quests.size();
        for (auto& q : quests) f << ' ' << q.target << ' ' << q.reward << ' ' << q.progress;
        f << '\n' << unlocked.size();
        for (int u : unlocked) f << ' ' << u;
        f << '\n' << done.size();
        for (auto& d : done) f << ' ' << d;
        f << '\n' << (hardMode ? 1 : 0) << ' ' << gearHidden << ' ' << outfit << '\n';
    }

    void Load() {
        std::ifstream f("javaquest_save.dat");
        if (!f) return;
        size_t n = 0;
        f >> xp >> coins >> hints >> boost >> extraLife >> n;
        if (n == quests.size())
            for (auto& q : quests) f >> q.target >> q.reward >> q.progress;
        f >> n; unlocked.clear();
        for (size_t i = 0; i < n; i++) { int u; f >> u; unlocked.insert(u); }
        unlocked.insert(0);
        f >> n; done.clear();
        for (size_t i = 0; i < n; i++) { std::string d; f >> d; done.insert(d); }
        int hm = 0; if (f >> hm) hardMode = hm != 0;      // old saves: stays off
        int gh = 0, oc = 0; if (f >> gh >> oc) { gearHidden = gh; outfit = ((oc % 5) + 5) % 5; }
    }
};