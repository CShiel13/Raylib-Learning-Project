#pragma once
// Quiz rules, answer shuffling, lifelines and milestones: no raylib dependency.
#include <algorithm>
#include <array>
#include <cstdint>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "ProgrammerQuizBank.h"

namespace programmer_quiz {

class Session {
public:
    enum class Phase { Welcome, Asking, Revealed, Finished };
    enum class Outcome { None, Walked, WrongAnswer, Jackpot };

    explicit Session(uint32_t seed = std::random_device{}()) : rng(seed) {}

    void Start() {
        used.assign(Questions().size(), false);
        round = 1; completed = 0; selected = -1;
        usedFifty = usedDuck = usedSwap = false;
        correct = false; phase = Phase::Asking; outcome = Outcome::None;
        PickQuestion();
    }

    Phase State() const { return phase; }
    Outcome Result() const { return outcome; }
    int Round() const { return round; }
    int Completed() const { return completed; }
    int Guaranteed() const { return completed >= 10 ? 10 : completed >= 5 ? 5 : 0; }
    int AwardedRound() const { return outcome == Outcome::WrongAnswer ? Guaranteed() : completed; }
    int Selected() const { return selected; }
    bool WasCorrect() const { return correct; }
    bool UsedFifty() const { return usedFifty; }
    bool UsedDuck() const { return usedDuck; }
    bool UsedSwap() const { return usedSwap; }
    bool Hidden(int position) const { return position >= 0 && position < 4 && hidden[position]; }
    int CorrectPosition() const {
        for (int i = 0; i < 4; ++i) if (order[i] == 0) return i;
        return -1;
    }
    int CurrentQuestionIndex() const { return questionIndex; }
    const Question& Current() const { return Questions().at(static_cast<size_t>(questionIndex)); }
    const std::string& Answer(int position) const {
        return Current().answers.at(static_cast<size_t>(order.at(static_cast<size_t>(position))));
    }
    const std::string& Explanation() const { return Current().explanation; }
    const std::string& DuckHint() const { return Current().duckHint; }

    bool Choose(int position) {
        if (phase != Phase::Asking || position < 0 || position >= 4 || hidden[position]) return false;
        selected = position; return true;
    }
    bool LockIn() {
        if (phase != Phase::Asking || selected < 0) return false;
        correct = selected == CorrectPosition();
        if (correct) ++completed;
        phase = Phase::Revealed;
        return true;
    }
    void Next() {
        if (phase != Phase::Revealed) return;
        if (!correct) { outcome = Outcome::WrongAnswer; phase = Phase::Finished; return; }
        if (completed >= 15) { outcome = Outcome::Jackpot; phase = Phase::Finished; return; }
        round = completed + 1;
        PickQuestion(); phase = Phase::Asking;
    }
    void WalkAway() {
        if (phase != Phase::Asking) return;
        outcome = Outcome::Walked; phase = Phase::Finished;
    }
    bool FiftyFifty() {
        if (phase != Phase::Asking || usedFifty) return false;
        usedFifty = true; ApplyFifty(); return true;
    }
    bool AskDuck() {
        if (phase != Phase::Asking || usedDuck) return false;
        usedDuck = true; return true;
    }
    bool SwapQuestion() {
        if (phase != Phase::Asking || usedSwap) return false;
        // Only consume the lifeline if there is another unused question in the band.
        std::vector<int> choices = Candidates(DifficultyForRound(round));
        if (choices.empty()) return false;
        usedSwap = true;
        PickQuestion();
        if (usedFifty) ApplyFifty();
        return true;
    }

private:
    std::mt19937 rng;
    std::vector<bool> used;
    int round = 1, completed = 0, selected = -1, questionIndex = -1;
    std::array<int, 4> order{{0, 1, 2, 3}};
    std::array<bool, 4> hidden{{false, false, false, false}};
    bool usedFifty = false, usedDuck = false, usedSwap = false;
    bool correct = false;
    Phase phase = Phase::Welcome;
    Outcome outcome = Outcome::None;

    std::vector<int> Candidates(Difficulty difficulty) const {
        std::vector<int> result;
        const auto& questions = Questions();
        for (size_t i = 0; i < questions.size(); ++i)
            if (questions[i].difficulty == difficulty && !used[i]) result.push_back(static_cast<int>(i));
        return result;
    }
    void PickQuestion() {
        std::vector<int> candidates = Candidates(DifficultyForRound(round));
        if (candidates.empty()) return; // bank invariant verified in the tests
        std::uniform_int_distribution<size_t> randomIndex(0, candidates.size() - 1);
        questionIndex = candidates[randomIndex(rng)];
        used[static_cast<size_t>(questionIndex)] = true;
        std::iota(order.begin(), order.end(), 0);
        std::shuffle(order.begin(), order.end(), rng);
        hidden.fill(false); selected = -1; correct = false;
    }
    void ApplyFifty() {
        std::vector<int> wrong;
        for (int i = 0; i < 4; ++i) if (i != CorrectPosition()) wrong.push_back(i);
        std::shuffle(wrong.begin(), wrong.end(), rng);
        hidden.fill(false);
        hidden[wrong[0]] = hidden[wrong[1]] = true;
        if (selected >= 0 && hidden[selected]) selected = -1;
    }
};

} // namespace programmer_quiz
