#pragma once

#include <algorithm>
#include <array>
#include <string>
#include "PracticeActivity.h"
#include "PracticeQuestions.h"
#include "../ui/Theme.h"
#include "../ui/Text.h"
#include "../ui/Shapes.h"
#include "../ui/Effects.h"
#include "../ui/Widgets.h"

// A compact multiple-choice loop shared by the three arcade-style learning games.
// Question content and progression are independent of the C++ code-execution runner.
class PracticeQuizSession : public IPracticeActivity {
    PracticeMiniGameKind kind;
    int questionIndex = -1;
    int answered = 0;
    int score = 0;
    int streak = 0;
    int bestStreak = 0;
    int selected = -1;
    std::array<int, 4> optionOrder{{ 0, 1, 2, 3 }};
    bool started = false;

    const PracticeQuestion& Question() const {
        return PracticeQuestionBank::For(kind).at((size_t)questionIndex);
    }

    void PickQuestion() {
        const auto& bank = PracticeQuestionBank::For(kind);
        const int difficulty = std::min(3, 1 + answered / 2);
        int candidates[16] = {};
        int count = 0;
        for (int i = 0; i < (int)bank.size() && count < 16; ++i)
            if (bank[(size_t)i].difficulty == difficulty) candidates[count++] = i;
        if (count == 0) { questionIndex = 0; return; }
        int alternatives[16] = {};
        int alternativeCount = 0;
        for (int i = 0; i < count; ++i)
            if (candidates[i] != questionIndex) alternatives[alternativeCount++] = candidates[i];
        questionIndex = alternatives[GetRandomValue(0, alternativeCount > 0 ? alternativeCount - 1 : 0)];
        for (int i = 0; i < 4; ++i) optionOrder[(size_t)i] = i;
        for (int i = 3; i > 0; --i) {
            int j = GetRandomValue(0, i);
            std::swap(optionOrder[(size_t)i], optionOrder[(size_t)j]);
        }
    }

    void DrawCode(const PracticeQuestion& q, float x, float y) const {
        size_t start = 0;
        int line = 0;
        while (start <= q.code.size() && line < 5) {
            size_t end = q.code.find('\n', start);
            std::string source = q.code.substr(start, end == std::string::npos ? end : end - start);
            const std::size_t gapAt = kind == PracticeMiniGameKind::LogicLocks ? source.find("???") : std::string::npos;
            if (gapAt == std::string::npos) {
                Txt(source, x, y + line * 22.0f, 15, kText, true);
            } else {
                const std::string prefix = source.substr(0, gapAt);
                const std::string suffix = source.substr(gapAt + 3);
                const float yy = y + line * 22.0f;
                const float xx = x + TxtW(prefix, 15, true);
                const std::string label = "[ condition ]";
                Txt(prefix, x, yy, 15, kText, true);
                Box({ xx - 3, yy - 2, TxtW(label, 15, true) + 6, 21 }, 4, Al(kViolet, 0.16f));
                Txt(label, xx, yy, 15, kViolet, true);
                Txt(suffix, xx + TxtW(label, 15, true), yy, 15, kText, true);
            }
            ++line;
            if (end == std::string::npos) break;
            start = end + 1;
        }
    }

    void Answer(int choice) {
        if (selected >= 0) return;
        selected = choice;
        ++answered;
        if (optionOrder[(size_t)choice] == Question().answer) {
            ++streak;
            bestStreak = std::max(bestStreak, streak);
            score += 10 * Question().difficulty;
            FxRing(550, 480, kMint, 74);
            FxSparks(550, 480, kMint, 9);
        } else {
            streak = 0;
            FxRing(550, 480, kCoral, 66);
        }
    }

public:
    explicit PracticeQuizSession(PracticeMiniGameKind selectedKind) : kind(selectedKind) {}

    void Start() override {
        if (started) return;
        started = true;
        PickQuestion();
    }

    PracticeAction Draw(float) override {
        DrawHeaderBackground();
        if (DrawHeaderBackButton()) return PracticeAction::Back;
        DrawHeaderTitle(PracticeQuestionBank::Title(kind), true);
        const PracticeQuestion& q = Question();
        const Color accent = kind == PracticeMiniGameKind::BugHunt ? kCoral :
            (kind == PracticeMiniGameKind::OutputOracle ? kSky : kViolet);

        Txt(PracticeQuestionBank::Subtitle(kind), 84, 47, 14, kMuted);
        Txt("Score " + std::to_string(score), 720, 47, 14, kMint);
        Txt("Streak " + std::to_string(streak), 850, 47, 14, kAmber);
        Txt("Level " + std::to_string(q.difficulty), 990, 47, 14, kMuted);

        Rectangle prompt{ 42, 94, 1016, 104 };
        Card3D(prompt, 18, kSurface, Mix(kBorder, accent, 0.55f), 4);
        Txt(PracticeQuestionBank::PromptLabel(kind), prompt.x + 22, prompt.y + 14, 14, accent);
        Wrap(q.prompt, prompt.x + 22, prompt.y + 42, prompt.width - 44, 19, kText, true, false, nullptr, 1.2f);

        Rectangle code{ 42, 214, 1016, 164 };
        Card3D(code, 18, kSurface, kBorder, 4);
        Txt(PracticeQuestionBank::SnippetInstruction(kind), code.x + 22, code.y + 13, 14, kMuted);
        DrawCode(q, code.x + 24, code.y + 42);

        Txt(PracticeQuestionBank::AnswerInstruction(kind), 48, 392, 15, kMuted);
        for (int i = 0; i < 4; ++i) {
            const int col = i % 2, row = i / 2;
            Rectangle option{ 42.0f + col * 514.0f, 416.0f + row * 62.0f, 500, 52 };
            Color optionColour = accent;
            if (selected >= 0 && optionOrder[(size_t)i] == q.answer) optionColour = kMint;
            else if (selected == i) optionColour = kCoral;
            bool enabled = selected < 0;
            std::string label = std::string(1, char('A' + i)) + ". " + q.choices[(size_t)optionOrder[(size_t)i]];
            if (Button(option, label, optionColour, enabled, 16, true)) Answer(i);
        }

        if (selected >= 0) {
            const bool correct = optionOrder[(size_t)selected] == q.answer;
            Rectangle feedback{ 42, 552, 760, 96 };
            Box(feedback, 15, Al(correct ? kMint : kCoral, 0.12f));
            Txt(correct ? "Correct!" : "Not quite — here's why", feedback.x + 18, feedback.y + 12, 17, correct ? kMint : kCoral);
            std::string detail = correct ? q.explanation : "Answer: " + q.choices[(size_t)q.answer] + ". " + q.explanation;
            Wrap(detail, feedback.x + 18, feedback.y + 40, feedback.width - 36, 13, kText, true, false, nullptr, 1.12f);
            if (Button({ 824, 570, 234, 58 }, "Next challenge", accent, true, 18, true)) {
                PickQuestion();
                selected = -1;
            }
        } else {
            Txt("Correct answers earn " + std::to_string(10 * q.difficulty) + " points. Difficulty rises every two questions.",
                48, 570, 14, kMuted);
            Txt("Best streak: " + std::to_string(bestStreak), 48, 598, 13, kLock);
        }
        return PracticeAction::None;
    }
};
