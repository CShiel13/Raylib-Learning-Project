#pragma once
// Game.h - the Game class: owns the Player + content and runs every screen.
#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>
#include "raylib.h"
#include "Content.h"
#include "Player.h"
#include "UI.h"

enum class Screen { Map, Lesson, Result, Shop, Quests };

class Game {
public:
    Game() : topics(BuildTopics()) {}
    void Frame();                                   // call once per frame (inside BeginDrawing)
    void SaveNow() const { p.Save(); }

private:
    // ----------------------------------------------------------- state
    Player p;
    std::vector<Topic> topics;
    Screen screen = Screen::Map;
    int selTopic = 0;

    // lesson state
    struct Item { bool isCode = false; int qi = 0; std::vector<int> order; };
    const Module* mod = nullptr;
    int modTopic = 0, modIdx = 0;                   // modIdx -1 == topic test
    std::vector<Item> items;
    int cur = 0, lives = 3, correct = 0, streak = 0, bestStreak = 0;
    bool answered = false, wasRight = false, showCodeHint = false;
    int picked = -1;
    bool hidden[8] = {false};
    std::string input, feedback;
    int resXp = 0, resCoins = 0, levelUps = 0;
    bool passed = false, firstClear = false;

    std::string toast;
    float toastT = 0;

    static constexpr int W = 960, H = 640;

    // ----------------------------------------------------------- helpers
    static std::string Key(int t, int m) {
        return m < 0 ? "t" + std::to_string(t) + "test" : "t" + std::to_string(t) + "m" + std::to_string(m);
    }
    bool TopicOpen(int t) const { return p.unlocked.count(t) || (t > 0 && p.done.count(Key(t - 1, -1))); }
    bool ModuleDone(int t, int m) const { return p.done.count(Key(t, m)) > 0; }
    bool NodeOpen(int t, int i) const {             // i == modules.size() -> the test
        if (i < (int)topics[t].modules.size()) return i == 0 || ModuleDone(t, i - 1);
        for (int m = 0; m < (int)topics[t].modules.size(); m++) if (!ModuleDone(t, m)) return false;
        return true;
    }
    void Toast(const std::string& s) { toast = s; toastT = 2.5f; }

    static const char* LevelTitle(int lvl) {
        static const char* names[] = {"Rookie", "Coder", "Scripter", "Developer", "Engineer",
                                      "Architect", "Wizard", "Guru", "Legend"};
        return names[std::min(lvl - 1, 8)];
    }

    // ----------------------------------------------------------- lesson flow
    void StartLesson(int t, int m) {
        modTopic = t; modIdx = m;
        mod = (m < 0) ? &topics[t].test : &topics[t].modules[m];
        items.clear();
        for (int i = 0; i < (int)mod->questions.size(); i++) {
            Item it; it.qi = i;
            it.order.resize(mod->questions[i].options.size());
            std::iota(it.order.begin(), it.order.end(), 0);
            for (int k = (int)it.order.size() - 1; k > 0; k--)          // Fisher-Yates shuffle
                std::swap(it.order[k], it.order[GetRandomValue(0, k)]);
            items.push_back(it);
        }
        if (mod->hasCode) { Item c; c.isCode = true; items.push_back(c); }
        cur = 0; lives = 3 + p.extraLife; p.extraLife = 0;
        correct = streak = bestStreak = 0;
        resXp = resCoins = levelUps = 0;
        ResetItem();
        screen = Screen::Lesson;
    }

    void ResetItem() {
        answered = wasRight = showCodeHint = false;
        picked = -1; input.clear(); feedback.clear();
        for (bool& h : hidden) h = false;
    }

    static std::string Norm(const std::string& s) {
        std::string r;
        for (char c : s) if (c != ' ' && c != '\t' && c != '\n') r += c;
        return r;
    }

    void Grade(bool right, bool isCode, const std::string& wrongInfo, const std::string& expl) {
        answered = true; wasRight = right;
        if (right) {
            correct++; streak++; bestStreak = std::max(bestStreak, streak);
            float mult = p.CoinMult() * (streak >= 3 ? 2.0f : 1.0f);   // combo boost at 3 in a row
            int c = (int)std::lround((isCode ? 5 : 2) * mult);
            int x = isCode ? 20 : 10;
            p.coins += c; resCoins += c; resXp += x;
            levelUps += p.AddXP(x);
            p.Note(QK_CORRECT, 1); p.Note(QK_STREAK, streak);
            if (isCode) p.Note(QK_CODE, 1);
            feedback = "+" + std::to_string(x) + " XP, +" + std::to_string(c) + " coins. " + expl;
        } else {
            lives--; streak = 0;
            feedback = wrongInfo + " " + expl;
        }
    }

    void AnswerMC(int disp) {
        const Item& it = items[cur];
        const Question& q = mod->questions[it.qi];
        picked = disp;
        bool right = it.order[disp] == q.correct;
        Grade(right, false, "Answer: " + q.options[q.correct] + ".", q.explain);
    }

    void SubmitCode() {
        const CodeTask& c = mod->code;
        bool right = false;
        for (auto& a : c.accepted) if (Norm(a) == Norm(input)) right = true;
        Grade(right, true, "Answer: " + c.accepted[0], c.explain);
    }

    void UseHint() {
        if (answered || p.hints <= 0) return;
        const Item& it = items[cur];
        if (it.isCode) { showCodeHint = true; }
        else {
            const Question& q = mod->questions[it.qi];
            int removed = 0;
            for (int i = 0; i < (int)it.order.size() && removed < 2; i++)
                if (it.order[i] != q.correct && !hidden[i]) { hidden[i] = true; removed++; }
        }
        p.hints--;
    }

    void NextItem() {
        if (lives <= 0) { Finish(false); return; }
        cur++;
        if (cur >= (int)items.size()) Finish(true);
        else ResetItem();
    }

    void Finish(bool ok) {
        passed = ok; firstClear = false;
        if (ok) {
            std::string k = Key(modTopic, modIdx);
            firstClear = !p.done.count(k);
            if (firstClear) {
                bool test = modIdx < 0;
                int bx = test ? 50 : 20, bc = test ? 20 : 5;
                resXp += bx; resCoins += bc; p.coins += bc;
                levelUps += p.AddXP(bx);
                p.done.insert(k);
                p.Note(QK_MODULES, 1);
            }
        }
        p.Save();
        screen = Screen::Result;
    }

    // ----------------------------------------------------------- shared UI
    void TopBar() {
        DrawRectangle(0, 0, W, 64, WHITE);
        DrawRectangle(0, 64, W, 2, cLine);
        DrawCircle(36, 32, 22, cGreen);
        TextCentered(TextFormat("%d", p.Level()), 36, 21, 24, WHITE);
        DrawRectangleRounded({76, 22, 220, 16}, 1.0f, 8, cLine);
        if (p.XpInLevel() > 0)
            DrawRectangleRounded({76, 22, 220.0f * p.XpInLevel() / 100.0f, 16}, 1.0f, 8, cGreen);
        DrawText(TextFormat("%d / 100 XP", p.XpInLevel()), 76, 42, 14, cSoft);
        DrawCircle(350, 32, 13, cGold);
        DrawText("$", 345, 23, 18, WHITE);
        DrawText(TextFormat("%d", p.coins), 372, 20, 24, cText);
        DrawCircle(470, 32, 13, cBlue);
        DrawText("?", 465, 23, 18, WHITE);
        DrawText(TextFormat("%d", p.hints), 492, 20, 24, cText);
        if (p.boost > 0) DrawText(TextFormat("x%.2f coins", p.CoinMult()), 545, 24, 18, Color{255, 140, 0, 255});
    }

    // ----------------------------------------------------------- MAP SCREEN
    void DrawMap() {
        TopBar();
        int claimable = 0;
        for (int i = 0; i < (int)p.quests.size(); i++) if (p.CanClaim(i)) claimable++;
        if (Button({ 700, 12, 110, 40 }, "Shop", cBlue, true, 20)) screen = Screen::Shop;
        if (Button({ 822, 12, 126, 40 }, claimable ? TextFormat("Quests (%d)", claimable) : "Quests",
            claimable ? cRed : Color{ 255, 150, 0, 255 }, true, 20)) screen = Screen::Quests;

        // ---- topic tabs (wrap into rows of 9) ----------------------------
        const int perRow = 9;
        int tRows = ((int)topics.size() + perRow - 1) / perRow;
        if (tRows < 1) tRows = 1;
        float tabH = tRows > 1 ? 28.0f : 38.0f;
        float tabY = 78.0f;
        float panelTop = std::max(130.0f, tabY + tRows * (tabH + 4.0f) + 4.0f);
        float panelH = 620.0f - panelTop;
        float dy = panelTop - 130.0f;          // 0 for <=9 topics, 16 for 18

        for (int t = 0; t < (int)topics.size(); t++) {
            int row = t / perRow, col = t % perRow;
            Rectangle r = { 12.0f + col * 104, tabY + row * (tabH + 4.0f), 100, tabH };
            bool sel = t == selTopic;
            Color c = TopicOpen(t) ? topics[t].color : cLock;
            if (sel) DrawRectangleRounded(r, 0.3f, 8, c);
            else     DrawRectangleRoundedLinesFallback(r, c);
            int fs = tRows > 1 ? 15 : 17;
            TextCentered(topics[t].shortName.c_str(),
                (int)(r.x + r.width / 2), (int)(r.y + (tabH - fs) / 2), fs, sel ? WHITE : cText);
            if (CheckCollisionPointRec(GetMousePosition(), r) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) selTopic = t;
        }

        // ---- character panel ---------------------------------------------
        DrawRectangleRounded({ 20, panelTop, 250, panelH }, 0.06f, 8, WHITE);
        DrawAvatar(145, 235, 1.3f, p.Level());
        TextCentered(TextFormat("Level %d  -  %s", p.Level(), LevelTitle(p.Level())), 145, 350, 20, cText);
        DrawText("Unlocked items", 40, 390, 18, cSoft);
        struct Gear { const char* name; int lvl; };
        static const Gear gear[] = { {"Hat", 2}, {"Glasses", 3}, {"Scarf", 5}, {"Cape", 7}, {"Crown", 10} };
        for (int i = 0; i < 5; i++) {
            bool got = p.Level() >= gear[i].lvl;
            DrawText(TextFormat("%s  (LVL %d)", gear[i].name, gear[i].lvl), 56, 418 + i * 26, 18, got ? cText : cLock);
            DrawCircle(44, 427 + i * 26, 5, got ? cGreen : cLine);
        }
        DrawText(TextFormat("Modules cleared: %d", (int)p.done.size()), 40, 580, 18, cText);

        // ---- topic panel --------------------------------------------------
        DrawRectangleRounded({ 290, panelTop, 650, panelH }, 0.04f, 8, WHITE);
        const Topic& T = topics[selTopic];
        DrawText(T.name.c_str(), 316, (int)(150 + dy), 32, T.color);
        DrawWrapped(T.blurb, 316, (int)(192 + dy), 600, 20, cSoft);

        if (!TopicOpen(selTopic)) {
            TextCentered("This topic is locked", 615, (int)(300 + dy), 30, cText);
            TextCentered("Pass the previous topic test - or unlock it with coins", 615, (int)(345 + dy), 20, cSoft);
            bool afford = p.coins >= T.cost;
            if (Button({ 515, (float)(400 + dy), 200, 56 }, TextFormat("Unlock  (%d)", T.cost), cGold, afford, 24)) {
                p.coins -= T.cost; p.unlocked.insert(selTopic); p.Save();
                Toast("Topic unlocked!");
            }
            return;
        }

        int count = (int)T.modules.size() + 1;
        for (int i = 0; i < count; i++) {
            bool isTest = i == (int)T.modules.size();
            int m = isTest ? -1 : i;
            float cx = 290 + 65 + (i % 5) * 130;
            float cy = 290 + dy + (i / 5) * 150;
            bool done = p.done.count(Key(selTopic, m)) > 0;
            bool open = NodeOpen(selTopic, i);
            float rad = isTest ? 46.0f : 40.0f;
            Color base = done ? cGold : (open ? T.color : cLock);
            bool hover = CheckCollisionPointCircle(GetMousePosition(), { cx, cy }, rad);
            DrawCircleV({ cx, cy + 6 }, rad, ColorBrightness(base, -0.25f));
            DrawCircleV({ cx, cy + ((hover && open && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) ? 4.0f : 0.0f) }, rad,
                (hover && open) ? ColorBrightness(base, 0.08f) : base);
            if (done) {
                DrawLineEx({ cx - 14, cy }, { cx - 4, cy + 11 }, 6, WHITE);
                DrawLineEx({ cx - 4, cy + 11 }, { cx + 16, cy - 12 }, 6, WHITE);
            }
            else if (!open) {
                DrawRectangleRounded({ cx - 12, cy - 4, 24, 20 }, 0.2f, 6, WHITE);
                DrawRing({ cx, cy - 6 }, 6, 9, 180, 360, 12, WHITE);
            }
            else {
                TextCentered(isTest ? "TEST" : TextFormat("%d", i + 1),
                    (int)cx, (int)cy - (isTest ? 12 : 14), isTest ? 24 : 30, WHITE);
            }
            const std::string& title = isTest ? T.test.title : T.modules[i].title;
            TextCentered(title.c_str(), (int)cx, (int)(cy + rad + 14), 16, open ? cText : cSoft);
            if (hover && open && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) StartLesson(selTopic, m);
        }
        DrawText("Tip: 3 correct answers in a row gives a COMBO - double coins!", 316, (int)(580 + dy), 18, cSoft);
    }
    static void DrawRectangleRoundedLinesFallback(Rectangle r, Color c) {   // outline without version-specific API
        DrawRectangleRounded(r, 0.3f, 8, c);
        DrawRectangleRounded({r.x + 2, r.y + 2, r.width - 4, r.height - 4}, 0.3f, 8, WHITE);
    }

    // --------------------------------------------------------- LESSON SCREEN
    void DrawLesson() {
        const Topic& T = topics[modTopic];
        Item& it = items[cur];

        // header: quit, progress, lives
        if (Button({16, 14, 42, 38}, "X", cLock, true, 22)) { p.Save(); screen = Screen::Map; return; }
        DrawRectangleRounded({76, 26, 640, 16}, 1.0f, 8, cLine);
        float frac = (float)(cur + (answered ? 1 : 0)) / (float)items.size();
        if (frac > 0) DrawRectangleRounded({76, 26, 640.0f * frac, 16}, 1.0f, 8, T.color);
        for (int i = 0; i < lives; i++) DrawHeart(752.0f + i * 40, 30, 11, cRed);
        if (streak >= 3) DrawText(TextFormat("COMBO x2 coins!  (%d)", streak), 700, 58, 18, Color{255, 140, 0, 255});
        DrawText((modIdx < 0 ? T.test.title : T.modules[modIdx].title).c_str(), 80, 52, 16, cSoft);

        if (!it.isCode) {
            const Question& q = mod->questions[it.qi];
            DrawText("Choose the correct answer", 80, 92, 18, cSoft);
            int h = DrawWrapped(q.prompt, 80, 124, 800, 28, cText);
            int oy = 124 + h + 24;
            int n = (int)q.options.size();
            for (int i = 0; i < n; i++) {
                Rectangle r = {80, (float)(oy + i * 62), 800, 52};
                bool hid = hidden[i];
                bool isCorrect = it.order[i] == q.correct;
                bool hover = !answered && !hid && CheckCollisionPointRec(GetMousePosition(), r);
                Color fill = WHITE, edge = cLine;
                if (hid) { fill = Color{238, 238, 241, 255}; edge = fill; }
                else if (answered && isCorrect) { fill = Color{215, 255, 184, 255}; edge = cGreen; }
                else if (answered && i == picked) { fill = Color{255, 223, 224, 255}; edge = cRed; }
                else if (hover) { fill = Color{221, 244, 255, 255}; edge = cBlue; }
                DrawRectangleRounded({r.x - 2, r.y - 2, r.width + 4, r.height + 4}, 0.25f, 8, edge);
                DrawRectangleRounded(r, 0.25f, 8, fill);
                DrawCircle((int)r.x + 28, (int)r.y + 26, 14, hid ? cLine : cBG);
                TextCentered(TextFormat("%d", i + 1), (int)r.x + 28, (int)r.y + 17, 18, cSoft);
                DrawText(q.options[it.order[i]].c_str(), (int)r.x + 60, (int)r.y + 15, 22, hid ? cLock : cText);
                bool key = !answered && !hid && IsKeyPressed(KEY_ONE + i);
                if (!answered && !hid && ((hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) || key)) AnswerMC(i);
            }
        } else {
            const CodeTask& c = mod->code;
            DrawText("Small code challenge", 80, 92, 18, cSoft);
            int h = DrawWrapped(c.prompt, 80, 124, 800, 28, cText);
            int by = 124 + h + 30;
            Rectangle box = {80, (float)by, 800, 58};
            DrawRectangleRounded({box.x - 2, box.y - 2, box.width + 4, box.height + 4}, 0.2f, 8,
                                 answered ? (wasRight ? cGreen : cRed) : cBlue);
            DrawRectangleRounded(box, 0.2f, 8, Color{30, 32, 40, 255});
            std::string shown = input;
            if (!answered && ((int)(GetTime() * 2) % 2 == 0)) shown += "_";
            DrawText(shown.c_str(), (int)box.x + 16, by + 16, 26, Color{190, 255, 170, 255});
            if (showCodeHint) DrawText(("Hint: " + c.hint).c_str(), 80, by + 76, 20, cBlue);
            DrawText("Type your answer, then press Enter. Spaces don't matter.", 80, by + 108, 18, cSoft);

            if (!answered) {
                int ch = GetCharPressed();
                while (ch > 0) {
                    if (ch >= 32 && ch <= 126 && input.size() < 70) input += (char)ch;
                    ch = GetCharPressed();
                }
                if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && !input.empty()) input.pop_back();
            }
        }

        // bottom bar
        if (!answered) {
            DrawRectangle(0, 540, W, 100, WHITE);
            DrawRectangle(0, 540, W, 2, cLine);
            bool canHint = p.hints > 0;
            if (Button({40, 566, 190, 52}, TextFormat("Hint  (%d)", p.hints), cBlue, canHint, 22)) UseHint();
            if (it.isCode) {
                bool ok = !input.empty();
                if (Button({730, 566, 190, 52}, "Submit", cGreen, ok, 24) || (ok && IsKeyPressed(KEY_ENTER))) SubmitCode();
            }
        } else {
            Color bg = wasRight ? Color{215, 255, 184, 255} : Color{255, 223, 224, 255};
            Color fg = wasRight ? Color{70, 160, 0, 255} : cRed;
            DrawRectangle(0, 520, W, 120, bg);
            DrawText(wasRight ? "Correct!" : "Not quite", 40, 534, 30, fg);
            DrawWrapped(feedback, 40, 576, 660, 18, fg);
            const char* label = (lives <= 0 && !wasRight) ? "Finish" : "Continue";
            if (Button({730, 560, 190, 56}, label, wasRight ? cGreen : cRed, true, 24) || IsKeyPressed(KEY_ENTER)) NextItem();
        }
    }

    // --------------------------------------------------------- RESULT SCREEN
    void DrawResult() {
        const Topic& T = topics[modTopic];
        DrawAvatar(480, 150, 1.6f, p.Level(), passed);
        TextCentered(passed ? "Module complete!" : "Out of hearts!", 480, 250, 44, passed ? cGreen : cRed);
        TextCentered(passed ? (firstClear ? "First clear bonus applied" : "Replay - practice makes perfect")
                            : "Review the topic and try again", 480, 302, 20, cSoft);
        DrawRectangleRounded({290, 340, 380, 120}, 0.15f, 8, WHITE);
        DrawText(TextFormat("XP earned:       +%d", resXp), 320, 356, 24, cText);
        DrawText(TextFormat("Coins earned:   +%d", resCoins), 320, 388, 24, cText);
        DrawText(TextFormat("Best streak:      %d", bestStreak), 320, 420, 24, cText);
        if (levelUps > 0) TextCentered(TextFormat("LEVEL UP!  You are now level %d  (+%d coins)", p.Level(), levelUps * 10), 480, 480, 26, Color{255, 140, 0, 255});
        if (passed && modIdx < 0 && firstClear && modTopic + 1 < (int)topics.size())
            TextCentered(TextFormat("Next topic unlocked: %s", topics[modTopic + 1].name.c_str()), 480, 520, 22, T.color);
        if (Button({380, 560, 200, 56}, "Continue", cGreen, true, 26) || IsKeyPressed(KEY_ENTER)) screen = Screen::Map;
    }

    // ------------------------------------------------------------ SHOP SCREEN
    void DrawShop() {
        TopBar();
        if (Button({16, 78, 110, 40}, "< Back", cLock, true, 20)) screen = Screen::Map;
        DrawText("Shop", 150, 80, 36, cText);

        struct Row { const char* name; std::string desc; int cost; bool enabled; };
        int boostCost = 60 * (p.boost + 1);
        int boostLvlReq = 3 + 2 * p.boost;
        Row rows[3] = {
            {"Hint token",   "Removes 2 wrong answers, or reveals a code hint.", 15, true},
            {"Extra heart",  "Start your next lesson with 4 hearts.", 25, true},
            {"Coin boost",   "+25% coins on every answer (needs level " + std::to_string(boostLvlReq) + "). Owned: " + std::to_string(p.boost) + "/4", boostCost,
                             p.boost < 4 && p.Level() >= boostLvlReq},
        };
        for (int i = 0; i < 3; i++) {
            Rectangle r = {80, 150.0f + i * 130, 800, 112};
            DrawRectangleRounded(r, 0.12f, 8, WHITE);
            DrawText(rows[i].name, 110, (int)r.y + 20, 30, cText);
            DrawText(rows[i].desc.c_str(), 110, (int)r.y + 64, 20, cSoft);
            bool afford = p.coins >= rows[i].cost && rows[i].enabled;
            if (Button({700, r.y + 28, 150, 56}, TextFormat("Buy  (%d)", rows[i].cost), cGold, afford, 24)) {
                p.coins -= rows[i].cost;
                if (i == 0) p.hints++; else if (i == 1) p.extraLife++; else p.boost++;
                p.Save(); Toast("Purchased!");
            }
        }
        DrawText("Earn coins from answers, quests and level-ups.", 80, 560, 20, cSoft);
    }

    // ----------------------------------------------------------- QUEST SCREEN
    void DrawQuests() {
        TopBar();
        if (Button({16, 78, 110, 40}, "< Back", cLock, true, 20)) screen = Screen::Map;
        DrawText("Quests", 150, 80, 36, cText);
        for (int i = 0; i < (int)p.quests.size(); i++) {
            const Quest& q = p.quests[i];
            Rectangle r = {80, 140.0f + i * 112, 800, 98};
            DrawRectangleRounded(r, 0.12f, 8, WHITE);
            DrawText(Player::QuestText(q).c_str(), 104, (int)r.y + 16, 24, cText);
            float frac = std::min(1.0f, (float)q.progress / (float)q.target);
            DrawRectangleRounded({104, r.y + 56, 450, 18}, 1.0f, 8, cLine);
            if (frac > 0) DrawRectangleRounded({104, r.y + 56, 450.0f * frac, 18}, 1.0f, 8, cGreen);
            DrawText(TextFormat("%d / %d", std::min(q.progress, q.target), q.target), 570, (int)r.y + 55, 20, cSoft);
            DrawText(TextFormat("Reward: %d coins + 1 hint", q.reward), 104, (int)r.y + 78, 14, cSoft);
            if (Button({700, r.y + 22, 150, 54}, "Claim", cGreen, p.CanClaim(i), 24)) { p.Claim(i); Toast("Reward claimed!"); }
        }
    }

public:
    void FrameImpl() {
        if (toastT > 0) toastT -= GetFrameTime();
        ClearBackground(cBG);
        if (IsKeyPressed(KEY_ESCAPE) && screen != Screen::Map) { p.Save(); screen = Screen::Map; }
        switch (screen) {
            case Screen::Map:    DrawMap(); break;
            case Screen::Lesson: DrawLesson(); break;
            case Screen::Result: DrawResult(); break;
            case Screen::Shop:   DrawShop(); break;
            case Screen::Quests: DrawQuests(); break;
        }
        if (toastT > 0) {
            int w = MeasureText(toast.c_str(), 22) + 40;
            DrawRectangleRounded({(float)(W - w) / 2, 580, (float)w, 44}, 0.4f, 8, Color{40, 40, 50, 230});
            TextCentered(toast.c_str(), W / 2, 590, 22, WHITE);
        }
    }
};

inline void Game::Frame() { FrameImpl(); }
