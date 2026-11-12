#pragma once

#include <algorithm>
#include <cstdint>
#include <ctime>
#include <string>
#include <vector>
#include "PracticeActivity.h"
#include "RecallPicker.h"
#include "RecallGuidance.h"
#include "RecallInlineField.h"
#include "../ui/Theme.h"
#include "../ui/Math.h"
#include "../ui/Text.h"
#include "../ui/Shapes.h"
#include "../ui/Effects.h"
#include "../ui/Widgets.h"

// Type-the-code retrieval game: no compiler, course Player, or runner dependency.
// Reviews are scheduled in UTC days. Early freeplay does not advance the schedule.
class RecallReactorSession : public IPracticeActivity {
    RecallProgress progress;
    std::vector<int> retryAfter;
    std::vector<unsigned> shownThisRun;
    bool started = false;
    bool finished = false;
    bool answered = false;
    bool hinted = false;
    bool correct = false;
    bool bonus = false;
    bool saveFailed = false;
    int current = -1;
    int lastIndex = -1;
    int round = 0;
    int score = 0;
    int correctCount = 0;
    int streak = 0;
    int bestStreak = 0;
    RecallInlineField codeInput;
    std::string lastSkill;

    static std::int64_t Today() { return static_cast<std::int64_t>(std::time(nullptr)) / 86400; }
    const RecallCard& Card() const { return RecallDeck::Cards().at(static_cast<std::size_t>(current)); }

    void SelectCard() {
        const RecallPick pick = RecallPicker::Choose(progress, Today(), round,
            retryAfter, lastIndex, lastSkill,
            static_cast<unsigned>(GetRandomValue(0, 1000000)), shownThisRun);
        if (pick.index < 0) { finished = true; current = -1; return; }
        current = pick.index;
        ++shownThisRun[static_cast<std::size_t>(current)];
        lastIndex = current;
        lastSkill = Card().skill;
        bonus = pick.earlyFreeplay;
        answered = hinted = correct = false;
        codeInput.Clear();
    }

    void NewRun() {
        round = score = correctCount = streak = bestStreak = 0;
        finished = false;
        retryAfter.assign(RecallDeck::Cards().size(), 0);
        shownThisRun.assign(RecallDeck::Cards().size(), 0);
        lastIndex = -1;
        lastSkill.clear();
        SelectCard();
    }

    void EnterAnswer() {
        if (answered || current < 0 || !codeInput.HasContent()) return;
        correct = RecallProgress::Matches(Card(), codeInput.Text());
        answered = true;
        if (correct) {
            ++correctCount;
            ++streak;
            bestStreak = std::max(bestStreak, streak);
            score += bonus ? 5 : (hinted ? 8 : 20 + 5 * (streak - 1));
            FxSparks(913, 488, kMint, 12);
            FxRing(913, 488, kMint, 62);
        } else {
            streak = 0;
            retryAfter[static_cast<std::size_t>(current)] = round + 3;
            FxRing(913, 488, kCoral, 48);
        }
        // Bonus challenges are optional early practice: don't defeat the spacing schedule.
        if (!bonus) {
            progress.Record(static_cast<std::size_t>(current), correct, hinted, Today());
            if (!progress.Save()) saveFailed = true;
        }
    }

    void NextCard() {
        ++round;
        if (round >= 8) { finished = true; return; }
        SelectCard();
    }

    // The stored cards still use ___; it is replaced visually by a real,
    // mouse-focusable, single-line editor with a width that follows the answer.
    // This never changes the question's source string or accepted answers.
    void RenderCode(const RecallCard& card, float x, float y, bool hasAnswered) {
        const std::string source = card.context;
        constexpr float fs = 17.0f;
        std::size_t start = 0;
        int line = 0;
        while (start <= source.size() && line < 5) {
            const std::size_t end = source.find('\n', start);
            const std::string text = source.substr(start, end == std::string::npos ? end : end - start);
            const float lineY = y + line * 23.0f;
            const std::size_t at = text.find("___");
            if (at == std::string::npos) {
                Txt(text, x, lineY, fs, kText, true);
            } else {
                const std::string before = text.substr(0, at);
                const std::string after = text.substr(at + 3);
                Txt(before, x, lineY, fs, kText, true);
                const float gapX = x + TxtW(before, fs, true);
                const float suffixWidth = TxtW(after, fs, true);
                // Keep both the trailing C++ and the box inside the 1016px card.
                const float available = std::max(54.0f, 1034.0f - gapX - suffixWidth - 9.0f);
                const std::string completed = correct ? codeInput.Text() : card.accepted.front();
                const float width = hasAnswered
                    ? std::min(available, std::max(126.0f, TxtW(completed, fs, true) + 20.0f))
                    : codeInput.Width(available, fs, RecallGuidance::Name(card.answerKind));
                Rectangle field{ gapX - 1.0f, lineY - 3.0f, width, 28.0f };
                if (hasAnswered) {
                    BoxB(field, 6, Color{ 9, 12, 20, 255 }, correct ? kMint : kAmber, 2);
                    Txt(Fit(completed, field.width - 14.0f, fs, true), field.x + 7.0f,
                        field.y + 3.0f, fs, correct ? kMint : kAmber, true);
                } else {
                    codeInput.DrawInline(field, RecallGuidance::Name(card.answerKind), true);
                }
                Txt(after, field.x + field.width + 3.0f, lineY, fs, kText, true);
            }
            ++line;
            if (end == std::string::npos) break;
            start = end + 1;
        }
    }

    void DrawMeter() const {
        Box({ 42, 88, 1016, 68 }, 15, kSurface);
        Txt("CHARGE", 61, 101, 14, kSky);
        for (int i = 0; i < 8; ++i) {
            Rectangle cell{ 148.0f + i * 54.0f, 105, 42, 32 };
            Box(cell, 8, i < round ? kMint : kSurface2);
            if (i < round) TxtC("+", cell.x + cell.width / 2, cell.y + 4, 18, kBg, true);
        }
        Txt("Score " + std::to_string(score), 615, 106, 16, kMint);
        Txt("Combo " + std::to_string(streak), 770, 106, 16, kAmber);
        Txt("Tier " + std::to_string(progress.UnlockedTier()), 930, 106, 16, kViolet);
    }

    PracticeAction DrawComplete() {
        DrawHeaderBackground();
        if (DrawHeaderBackButton()) return PracticeAction::Back;
        DrawHeaderTitle("Recall Reactor", true);
        Card3D({ 160, 132, 780, 460 }, 24, kSurface, kMint, 6);
        TxtC("REACTOR CHARGED", 550, 172, 34, kMint);
        TxtC("Eight code challenges complete", 550, 230, 20, kText);
        TxtC(std::to_string(correctCount) + "/8 correct    |    " + std::to_string(score) + " energy", 550, 278, 24, kAmber);
        TxtC("Best combo: " + std::to_string(bestStreak), 550, 324, 18, kMuted);
        TxtC("Unaided success schedules tomorrow, then 3, 7, 14 and 30 days.", 550, 371, 15, kText);
        TxtC("Mistakes return sooner. Early freeplay won't change review dates.", 550, 400, 14, kMuted);
        TxtC("Reviews due now: " + std::to_string(progress.CountDue(Today())) +
              "   |   New cards: " + std::to_string(progress.CountNew()), 550, 445, 16, kSky);
        if (Button({ 346, 506, 408, 60 }, "Start another run", kMint, true, 21)) NewRun();
        return PracticeAction::None;
    }

public:
    void Start() override {
        if (started) return;
        started = true;
        progress.Load();
        NewRun();
    }

    PracticeAction Draw(float) override {
        if (finished) return DrawComplete();
        DrawHeaderBackground();
        if (DrawHeaderBackButton()) return PracticeAction::Back;
        DrawHeaderTitle("Recall Reactor", true);
        Txt("Remember the code, don't just recognise it", 328, 41, 14, kMuted);
        DrawMeter();

        const RecallCard& c = Card();
        const auto& m = progress.At(static_cast<std::size_t>(current));
        Card3D({ 42, 173, 1016, 106 }, 16, kSurface, kViolet, 4);
        Txt(std::string("CIRCUIT ") + std::to_string(round + 1) + "/8  |  " + c.skill +
            (bonus ? "  |  Optional freeplay" : (m.attempts ? "  |  Due review" : "  |  New skill")),
            62, 187, 14, kViolet);
        Wrap(c.mission, 62, 217, 968, 19, kText, true, false, nullptr, 1.17f);
        Txt("Example: " + std::string(c.example), 62, 250, 14, kSky);

        Card3D({ 42, 294, 1016, 149 }, 16, kSurface, kBorder, 4);
        Txt(answered ? "WORKING C++ - THE FILLED GAP IS HIGHLIGHTED" :
            "CLICK THE BLUE CODE BOX - TYPE " + std::string(RecallGuidance::Instruction(c.answerKind)),
            62, 308, 14, kMuted);
        RenderCode(c, 66, 338, answered);

        // The inline code gap is the only answer field. The freed space provides
        // room for hints and feedback instead of a second, duplicate editor.
        const bool wasAnswered = answered;
        if (!answered && codeInput.UpdateKeyboard()) EnterAnswer();
        Rectangle status{ 42, 458, 723, 182 };
        Box(status, 15, Al(answered ? (correct ? kMint : kCoral) : kSky, 0.10f));
        if (answered) {
            Txt(correct ? (hinted ? "Correct with a clue - keep practising!" : "Correct - circuit restored!") :
                "Not quite - compare with a working answer", 61, 473, 16, correct ? kMint : kCoral);
            const std::string answerLine = correct ? "Your code: " + codeInput.Text() :
                "One answer: " + c.accepted.front();
            Txt(Fit(answerLine, 676, 15, true), 61, 507, 15, kText, true);
            if (correct) {
                Wrap(c.explanation, 61, 549, 678, 14, kText, true, false, nullptr, 1.16f);
            } else {
                Wrap(RecallGuidance::CorrectionTip(c, codeInput.Text()),
                     61, 542, 676, 13, kText, true, false, nullptr, 1.13f);
                Wrap(std::string("Why: ") + c.explanation, 61, 583, 676, 12, kMuted,
                     true, false, nullptr, 1.12f);
            }
        } else if (hinted) {
            Txt("CLUE (your next correct answer counts as practice)", 61, 474, 15, kAmber);
            Wrap(c.hint, 61, 512, 675, 15, kText, true, false, nullptr, 1.18f);
            Txt("Enter your code directly into the blue gap above.", 61, 600, 13, kMuted);
        } else {
            Txt("How to answer", 61, 474, 17, kSky);
            Wrap("Click the small blue box in the C++ code and type the missing " +
                 std::string(RecallGuidance::Name(c.answerKind)) + ". The field grows as you type "
                 "and scrolls for longer answers. Press Enter or click Check my code.",
                 61, 510, 674, 14, kMuted, true, false, nullptr, 1.2f);
        }

        if (!answered) {
            if (Button({ 789, 480, 269, 45 }, "Show a clue", kAmber, !hinted, 16, true)) hinted = true;
            if (Button({ 789, 547, 269, 52 }, "Check my code", kMint, codeInput.HasContent(), 17)) EnterAnswer();
        } else if (Button({ 789, 517, 269, 63 }, round == 7 ? "Finish run" : "Next circuit", kMint, true, 18) ||
                   (wasAnswered && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)))) {
            NextCard();
        }
        Txt("Unaided recall: tomorrow -> 3d -> 7d -> 14d -> 30d   |   " +
              std::to_string(progress.CountLearned()) + " cards learned" +
              (saveFailed ? "   |   Review history could not be saved" : ""),
            46, 661, 13, saveFailed ? kCoral : kMuted);
        return PracticeAction::None;
    }
};
