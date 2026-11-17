#pragma once
// UI adapters: return XP on the first Collect click only; caller owns the player/rewards.
#include <algorithm>
#include <string>
#include <sstream>
#include "TimedGames.h"
#include "ui/Widgets.h"

namespace engagement {
class RapidReviewScreen {
public:
    void Reset() { session_.Reset(); claimed_ = false; }
    int Draw(float dt) {
        if (!session_.Running() && !session_.Finished() && IsKeyPressed(KEY_ENTER)) session_.Start();
        session_.Tick(dt);
        Txt("RAPID REVIEW", 42, 95, 31, kText);
        Txt("60 seconds. Recall C++ concepts quickly. Wrong choices cost 2 seconds.", 42, 137, 16, kMuted);
        Card3D({35, 176, 1030, 466}, 22, kSurface, kBorder, 5);
        if (!session_.Running() && !session_.Finished()) {
            TxtC("Ready for a speed round?", 550, 282, 31, kText);
            TxtC("Mixed Bug Hunt, Output Oracle and Logic Locks questions.", 550, 335, 18, kMuted);
            if (Button({410, 420, 280, 64}, "Start 60-second run", kMint, true, 21)) session_.Start();
            return 0;
        }
        if (session_.Finished()) {
            TxtC("TIME!", 550, 227, 38, kAmber);
            TxtC(std::to_string(session_.Correct()) + " correct out of " + std::to_string(session_.Attempts()), 550, 306, 30, kText);
            TxtC("XP is awarded only once when you collect your score.", 550, 360, 17, kMuted);
            int reward = session_.RewardXP();
            if (!claimed_ && Button({407, 430, 285, 60}, "Collect " + std::to_string(reward) + " XP", kMint, true, 22)) {
                claimed_ = true; return reward;
            }
            if (claimed_ && Button({407, 430, 285, 60}, "Play again", kSky, true, 22)) Reset();
            return 0;
        }
        Txt("TIME LEFT: " + std::to_string(session_.TimeLeft()) + "s", 63, 196, 23, session_.TimeLeft() < 10 ? kCoral : kAmber);
        TxtR("Correct: " + std::to_string(session_.Correct()), 1029, 197, 19, kMint);
        const auto& q = session_.Question();
        Wrap(q.prompt, 65, 243, 950, 23, kText);
        if (!q.code.empty()) {
            Box({63, 323, 974, 93}, 11, Color{10,14,23,255});
            std::istringstream lines(q.code); std::string line;
            for (int n = 0; n < 5 && std::getline(lines, line); ++n)
                Txt(Fit(line, 939, 15, true), 82, 329.0f + n * 17.2f, 15, kSky, true);
        }
        for (int i=0; i<4; ++i) {
            const float x=65.0f, y=425.0f + i*52.0f;
            if (Button({x,y,968,47}, std::string(1,'1'+i) + ". " + q.choices[session_.Options()[i]],
                       kSky, true, 18, true) || IsKeyPressed(KEY_ONE+i)) session_.Answer(i);
        }
        return 0;
    }
private:
    RapidReview session_; bool claimed_ = false;
};

class MatchMadnessScreen {
public:
    void Reset() { session_.Reset(); claimed_ = false; }
    int Draw(float dt) {
        if (!session_.Running() && !session_.Finished() && IsKeyPressed(KEY_ENTER)) session_.Start();
        session_.Tick(dt);
        Txt("MATCH MADNESS", 42, 95, 31, kText);
        Txt("Match each C++ term to its meaning. 12 pairs, 60 seconds.", 42, 137, 16, kMuted);
        Card3D({35, 176, 1030, 473}, 22, kSurface, kBorder, 5);
        if (!session_.Running() && !session_.Finished()) {
            TxtC("Connect concepts at speed", 550, 281, 30, kText);
            TxtC("Select one term and one definition. Mistakes cost 2 seconds.", 550, 341, 18, kMuted);
            if (Button({410, 420, 280, 64}, "Start matching", kMint, true, 21)) session_.Start();
            return 0;
        }
        if (session_.Finished()) {
            TxtC("ROUND COMPLETE", 550, 226, 35, kAmber);
            TxtC(std::to_string(session_.Matched()) + "/12 pairs matched", 550, 307, 30, kText);
            TxtC("Mistakes: " + std::to_string(session_.Misses()), 550, 355, 18, kMuted);
            const int xp = session_.RewardXP();
            if (!claimed_ && Button({405, 427, 290, 62}, "Collect " + std::to_string(xp) + " XP", kMint, true, 22)) { claimed_ = true; return xp; }
            if (claimed_ && Button({405, 427, 290, 62}, "Play again", kSky, true, 22)) Reset();
            return 0;
        }
        Txt("TIME LEFT: " + std::to_string(session_.TimeLeft()) + "s", 63, 194, 23, session_.TimeLeft() < 10 ? kCoral : kAmber);
        TxtR("Matched " + std::to_string(session_.Matched()) + "/12", 1029, 194, 20, kMint);
        for (int i=0;i<8;++i) {
            const auto& tile = session_.Tiles()[i];
            const float x=64+(i%2)*496.0f, y=246+(i/2)*91.0f;
            if (tile.matched) {
                Box({x,y,469,70}, 14, Mix(kSurface2,kMint,0.17f));
                TxtC("Matched",x+234.5f,y+21,19,kMint);
            } else {
                const Color tone = session_.First()==i ? kAmber : kSky;
                if (Button({x,y,469,70},session_.Label(i),tone,true,19,true) || IsKeyPressed(KEY_ONE+i)) session_.Choose(i);
            }
        }
        return 0;
    }
private:
    MatchMadness session_; bool claimed_ = false;
};
} // namespace engagement
