#pragma once
// Bonus-game presentation. The quiz rules and question bank do not depend on raylib.
#include <string>
#include "ProgrammerQuizSession.h"
#include "ui/Widgets.h"

class ProgrammerMillionaireScreen {
public:
    void Reset() { quiz = programmer_quiz::Session{}; showDuckHint = false; }

    void Draw() {
        using Phase = programmer_quiz::Session::Phase;
        switch (quiz.State()) {
        case Phase::Welcome: DrawWelcome(); break;
        case Phase::Asking:
        case Phase::Revealed: DrawQuestion(); break;
        case Phase::Finished: DrawFinished(); break;
        }
    }

private:
    programmer_quiz::Session quiz;
    bool showDuckHint = false;

    static void Panel(Rectangle r, Color accent) {
        Card3D(r, 20, kSurface, Mix(kBorder, accent, 0.42f), 5);
    }

    void DrawWelcome() {
        Panel({55, 113, 990, 530}, kViolet);
        TxtC("WHO WANTS TO BE A C++ PROGRAMMER?", 550, 145, 29, kText);
        TxtC("15 interviews. One legendary job offer.", 550, 197, 20, kAmber);
        TxtC("An entirely fictional career ladder - no real jobs, rewards or XP.", 550, 240, 15, kMuted);
        DrawLine(140, 289, 960, 289, kBorder);
        Txt("THE INTERVIEW RULES", 140, 311, 21, kMint);
        Wrap("Answer four-choice C++ questions. Levels 1-3 are silly and easy, 4-9 are moderate, 10-14 test deeper C++, and 15 is the dream-job finale.",
             140, 348, 800, 17, kText, true, false, nullptr, 1.5f);
        Wrap("Select an answer and lock it in. You can walk away with your current offer, or keep climbing. A wrong answer drops you to your last guaranteed checkpoint (5 or 10).",
             140, 427, 800, 16, kMuted, true, false, nullptr, 1.5f);
        TxtC("Lifelines: 50/50   |   Ask the Rubber Duck   |   Swap Question", 550, 524, 18, kSky);
        if (Button({372, 567, 356, 59}, "Begin the interview", kMint, true, 22)) quiz.Start();
    }

    void DrawLadder() {
        Panel({783, 121, 288, 525}, kAmber);
        TxtC("CAREER LADDER", 927, 142, 19, kAmber);
        const auto& jobs = programmer_quiz::CareerLadder();
        for (int job = 15; job >= 1; --job) {
            int row = 15 - job;
            float y = 178.0f + row * 29.8f;
            bool active = job == quiz.Round();
            bool earned = job <= quiz.Completed();
            bool safe = job == 5 || job == 10 || job == 15;
            if (active) Box({797, y - 1, 258, 27}, 7, Mix(kSurface2, kViolet, 0.43f));
            Color color = active ? kText : (earned ? kMint : safe ? kAmber : kMuted);
            TxtR(std::to_string(job), 826, y + 2, 13, color, true);
            Txt(Fit(jobs[static_cast<size_t>(job - 1)], 210, 13), 840, y + 2, 13, color);
            if (safe) DrawCircleV({1043, y + 11}, 3.5f, kAmber);
        }
        TxtC("Gold dots = guaranteed offers", 927, 628, 11, kMuted);
    }

    void AnswerCard(int position, Rectangle r) {
        bool revealed = quiz.State() == programmer_quiz::Session::Phase::Revealed;
        bool hidden = quiz.Hidden(position);
        bool selected = quiz.Selected() == position;
        bool correct = position == quiz.CorrectPosition();
        Color border = selected ? kViolet : kBorder;
        if (revealed && correct) border = kMint;
        if (revealed && selected && !correct) border = kCoral;
        Vector2 mouse = Mouse();
        bool hover = !revealed && !hidden && CheckCollisionPointRec(mouse, r);
        BoxB(r, 13, hover ? Mix(kSurface2, kSky, 0.13f) : kSurface2, border, selected || revealed && correct ? 3.0f : 2.0f);
        if (hidden) {
            Txt("-- eliminated --", r.x + 28, r.y + 20, 17, kLock);
            return;
        }
        Txt(std::string(1, static_cast<char>('A' + position)) + ".", r.x + 16, r.y + 18, 21,
            selected ? kViolet : kSky, true);
        Wrap(quiz.Answer(position), r.x + 58, r.y + 11, r.width - 72, 17,
             revealed && correct ? kMint : kText, true, false, nullptr, 1.25f);
        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) quiz.Choose(position);
    }

    void DrawQuestion() {
        using Phase = programmer_quiz::Session::Phase;
        bool revealed = quiz.State() == Phase::Revealed;
        if (!revealed) {
            for (int i = 0; i < 4; ++i) {
                const int key = KEY_ONE + i; // raylib's top-row numeric keys are contiguous.
                if (IsKeyPressed(key)) quiz.Choose(i);
            }
            if (IsKeyPressed(KEY_ENTER)) quiz.LockIn();
        } else if (IsKeyPressed(KEY_ENTER)) { quiz.Next(); showDuckHint = false; }

        Txt("INTERVIEW " + std::to_string(quiz.Round()) + " / 15", 34, 93, 22, kAmber);
        TxtR(programmer_quiz::TierLabel(quiz.Round()), 750, 98, 16, kMuted);
        Panel({30, 130, 725, 158}, kViolet);
        Txt("THE QUESTION", 52, 145, 13, kSky);
        Wrap(quiz.Current().prompt, 52, 177, 678, 23, kText, true, false, nullptr, 1.4f);
        AnswerCard(0, {30, 308, 351, 73});
        AnswerCard(1, {402, 308, 353, 73});
        AnswerCard(2, {30, 394, 351, 73});
        AnswerCard(3, {402, 394, 353, 73});
        DrawLadder();

        BoxB({30, 482, 725, 66}, 14, kSurface, kBorder);
        if (revealed) {
            Txt(quiz.WasCorrect() ? "CORRECT - OFFER UNLOCKED" : "NOT QUITE - INTERVIEW OVER",
                46, 490, 15, quiz.WasCorrect() ? kMint : kCoral);
            Wrap(quiz.Explanation(), 46, 512, 690, 14, kText, true, false, nullptr, 1.2f);
        } else if (showDuckHint) {
            Txt("RUBBER DUCK HINT", 46, 490, 15, kSky);
            Wrap(quiz.DuckHint(), 46, 512, 690, 14, kText, true, false, nullptr, 1.2f);
        } else {
            Txt("SELECT A, B, C OR D", 46, 491, 16, kText);
            Txt("Click an answer or press 1-4, then lock it in. There is no timer.", 46, 516, 14, kMuted);
        }
        if (!revealed) {
            if (Button({30, 565, 132, 44}, "50 / 50", kViolet, !quiz.UsedFifty(), 17, true)) quiz.FiftyFifty();
            if (Button({172, 565, 202, 44}, "Ask Rubber Duck", kSky, !quiz.UsedDuck(), 16, true)) {
                quiz.AskDuck(); showDuckHint = true;
            }
            if (Button({384, 565, 178, 44}, "Swap Question", kAmber, !quiz.UsedSwap(), 16, true)) {
                if (quiz.SwapQuestion()) showDuckHint = false;
            }
            if (Button({573, 565, 182, 44}, "Walk Away", kSurface2, true, 16, true)) quiz.WalkAway();
            if (Button({247, 623, 286, 57}, "Lock in answer", kMint, quiz.Selected() >= 0, 22)) quiz.LockIn();
        } else {
            if (Button({247, 617, 286, 61}, quiz.WasCorrect() && quiz.Completed() < 15 ? "Next interview" : "View job offer", kMint, true, 21)) {
                quiz.Next(); showDuckHint = false;
            }
        }
    }

    void DrawFinished() {
        using Outcome = programmer_quiz::Session::Outcome;
        Panel({75, 118, 950, 526}, kAmber);
        bool jackpot = quiz.Result() == Outcome::Jackpot;
        TxtC(jackpot ? "YOU GOT THE DREAM JOB!" : "YOUR INTERVIEW IS OVER", 550, 160, 30,
             jackpot ? kMint : kAmber);
        std::string explanation;
        if (quiz.Result() == Outcome::WrongAnswer)
            explanation = "That answer was incorrect. You keep only your last guaranteed offer.";
        else if (quiz.Result() == Outcome::Walked)
            explanation = "You walked away and kept your most recent fictional job offer.";
        else explanation = "All 15 interviews passed. The board is impressed (and slightly afraid).";
        Wrap(explanation, 180, 231, 750, 18, kText, true, false, nullptr, 1.4f);
        BoxB({175, 311, 750, 142}, 22, kSurface2, kBorder);
        const int awarded = quiz.AwardedRound();
        TxtC("YOUR FICTIONAL CAREER PRIZE", 550, 331, 17, kMuted);
        TxtC(awarded > 0 ? programmer_quiz::CareerLadder()[static_cast<size_t>(awarded - 1)] : "No offer - keep practising!",
             550, 374, 26, awarded > 0 ? kMint : kCoral);
        TxtC("Answered " + std::to_string(quiz.Completed()) + " of 15 interviews", 550, 466, 17, kMuted);
        if (Button({245, 547, 274, 62}, "Play again", kViolet, true, 21)) {
            quiz.Start(); showDuckHint = false;
        }
        Txt("Back button or Esc: return to Bonus Languages", 533, 623, 13, kMuted);
    }
};
