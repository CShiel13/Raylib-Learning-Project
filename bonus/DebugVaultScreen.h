#pragma once
// Visual deduction game: accessible feedback instead of unexplained coloured pegs.
#include <string>
#include "DebugVault.h"
#include "ui/Widgets.h"

class DebugVaultScreen {
public:
    void Reset() { session_ = debug_vault::Session{}; hint_ = "Digits 1-6; every digit can appear only once."; }
    void Draw() {
        using debug_vault::Phase;
        if (session_.State() == Phase::Guessing) {
            for (int key = KEY_ONE; key <= KEY_SIX; ++key)
                if (IsKeyPressed(key)) AddDigit(static_cast<char>('1' + key - KEY_ONE));
            if (IsKeyPressed(KEY_BACKSPACE)) session_.Delete();
            if (IsKeyPressed(KEY_ENTER)) Submit();
        } else if (IsKeyPressed(KEY_ENTER)) session_.NewCode();

        Txt("DEBUG VAULT", 35, 89, 30, kText);
        Txt("Crack a hidden 4-digit code using logical clues.", 35, 132, 16, kMuted);
        Card3D({30, 166, 496, 449}, 22, kSurface, kBorder, 5);
        Card3D({549, 166, 522, 449}, 22, kSurface, kBorder, 5);
        DrawEntry();
        DrawHistory();
    }
private:
    debug_vault::Session session_;
    std::string hint_ = "Digits 1-6; every digit can appear only once.";

    void AddDigit(char digit) {
        if (!session_.Enter(digit)) hint_ = "Use a different digit (1-6). No duplicates.";
        else hint_ = "Press Enter or 'Check code' when all four slots are filled.";
    }
    void Submit() {
        if (!session_.Submit()) hint_ = "Enter four different digits to check the code.";
        else hint_ = "Compare exact and misplaced counts to narrow it down.";
    }
    void DrawEntry() {
        using debug_vault::Phase;
        Txt("THE LOCK", 50, 185, 17, kAmber);
        Txt("Attempt " + std::to_string(std::min(static_cast<int>(session_.History().size()) + 1, session_.Limit())) + " / " + std::to_string(session_.Limit()), 51, 216, 15, kMuted);
        const bool playing = session_.State() == Phase::Guessing;
        const std::string visible = playing ? session_.Entry() : session_.RevealedCode();
        for (int i = 0; i < 4; ++i) {
            float x = 55.0f + i * 112.0f;
            bool entered = static_cast<size_t>(i) < visible.size();
            Color edge = entered ? (playing ? kSky : session_.State() == Phase::Unlocked ? kMint : kCoral) : kBorder;
            BoxB({x, 252, 93, 69}, 13, entered ? kSurface2 : kSurface, edge, 2);
            TxtC(entered ? std::string(1, visible[static_cast<size_t>(i)]) : "_", x + 46, 271, 27,
                 entered ? kText : kMuted, true);
        }
        if (playing) {
            for (int row = 0; row < 2; ++row) for (int col = 0; col < 3; ++col) {
                int digit = row * 3 + col + 1;
                if (Button({54.0f + col * 150.0f, 350.0f + row * 66.0f, 133, 54},
                           std::to_string(digit), kViolet, true, 22)) AddDigit(static_cast<char>('0' + digit));
            }
            if (Button({53, 493, 135, 53}, "Delete", kAmber, !session_.Entry().empty(), 18, true)) session_.Delete();
            if (Button({202, 493, 300, 53}, "Check code [Enter]", kMint, session_.Entry().size() == 4, 19)) Submit();
        } else {
            Color award = session_.State() == Phase::Unlocked ? kMint : kCoral;
            TxtC(session_.State() == Phase::Unlocked ? "VAULT UNLOCKED" : "ACCESS DENIED", 278, 373, 27, award);
            TxtC("Secret code: " + session_.RevealedCode(), 278, 425, 18, kText, true);
            if (Button({107, 492, 343, 57}, "New code [Enter]", kMint, true, 19)) session_.NewCode();
        }
        Wrap(hint_, 52, 565, 446, 14, kMuted, true, false, nullptr, 1.2f);
    }
    void DrawHistory() {
        Txt("DEDUCTION LOG", 571, 184, 17, kSky);
        Txt("Exact = right digit AND position", 571, 217, 15, kMint);
        Txt("Misplaced = right digit, wrong position", 571, 246, 15, kAmber);
        const auto& history = session_.History();
        if (history.empty()) {
            Wrap("Example: if the secret is 1234, guessing 1356 gives 1 exact and 1 misplaced. Each clue narrows your next guess.",
                 575, 311, 456, 17, kMuted, true, false, nullptr, 1.35f);
        }
        for (size_t i = 0; i < history.size(); ++i) {
            float y = 292.0f + static_cast<float>(i) * 36.0f;
            Box({569, y, 477, 31}, 7, i % 2 ? kSurface : kSurface2);
            Txt(std::to_string(i + 1) + ".  " + history[i].guess, 584, y + 4, 17, kText, true);
            Txt("Exact: " + std::to_string(history[i].exact), 763, y + 6, 14, kMint);
            Txt("Misplaced: " + std::to_string(history[i].misplaced), 880, y + 6, 14, kAmber);
        }
        Txt("Unlocked vaults: " + std::to_string(session_.Wins()), 572, 574, 17, kSky);
    }
};
