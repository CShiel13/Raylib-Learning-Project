#pragma once
// Game.h - owns the Player + content and runs every screen (dark theme, animated).
#include <algorithm>
#include <cctype>
#include <memory>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>
#include "raylib.h"
#include "Content.h"
#include "Player.h"
#include "UI.h"
#include "Hard.h"
#include "CppCourse.h"

// Optional easter egg: if Sh2Mode.h exists it is compiled in; delete the file and the game builds without it.
#if defined(__has_include)
#  if __has_include("Sh2Mode.h")
#    include "Sh2Mode.h"
#    define JQ_SH2 1
#  endif
#endif

enum class Screen { Menu, Bonus, Map, Lesson, Result, Shop, Quests, Hard, Character };

class Game {
public:
    Game() {
        BuildHard();
        courses = BuildCourses();
        InstallCppCourse(courses);        // the full C++ course replaces the outline from Content.h
        topics = courses[0].topics;
        dispXp = (float)p.xp; dispCoins = (float)p.coins;
    }
    void SaveNow() const { p.Save(); }

    void Frame() {
        float dt = GetFrameTime(); if (dt > 0.05f) dt = 0.05f;
#ifdef JQ_SH2
        if (sh2mode) {                                       // easter egg owns the whole frame while it runs
            if (!sh2mode->Frame(dt)) { sh2mode.reset(); fade = 1.0f; }
            return;
        }
#endif
        time += dt;
        if (toastT > 0) toastT -= dt;
        fade = std::max(0.0f, fade - dt * 4.0f);
        dispXp = Smooth(dispXp, (float)p.xp, 7, dt);
        dispCoins = Smooth(dispCoins, (float)p.coins, 9, dt);

        DrawBackground();
        if (IsKeyPressed(KEY_ESCAPE)) {
            p.Save();
            if (screen == Screen::Map) Go(mapFrom);
            else if (screen == Screen::Bonus) Go(Screen::Menu);
            else if (screen == Screen::Shop || screen == Screen::Quests || screen == Screen::Character) Go(from);
            else if (screen == Screen::Lesson || screen == Screen::Result) Go(Screen::Map);
            else if (screen == Screen::Hard) EnterResult();
        }
        switch (screen) {
        case Screen::Menu:   DrawMenu(); break;
        case Screen::Bonus:  DrawBonus(); break;
        case Screen::Map:    DrawMap(); break;
        case Screen::Lesson: DrawLesson(dt); break;
        case Screen::Result: DrawResult(dt); break;
        case Screen::Shop:   DrawShop(); break;
        case Screen::Quests: DrawQuests(); break;
        case Screen::Hard:   DrawHard(dt); break;
        case Screen::Character: DrawCharacter(); break;
        }
        FxUpdate(dt); FxDraw();

        if (toastT > 0) {                                   // toast pill
            float k = EaseOut(std::min(1.0f, (2.5f - toastT) / 0.25f));
            float a = std::min(1.0f, toastT / 0.3f);
            float w = TxtW(toast, 20) + 56;
            Rectangle r{ (W - w) / 2, H - 70 + (1 - k) * 30, w, 44 };
            Shadow(r, 22, 5, 0.4f * a);
            BoxB(r, 22, Al(kSurface2, a), Al(kMint, a), 2);
            Icon(IC_CHECK, r.x + 24, r.y + 22, 8, Al(kMint, a));
            Txt(toast, r.x + 42, r.y + 10, 20, Al(kText, a));
        }
        if (fade > 0.001f) DrawRectangle(-3000, -3000, 7100, 6700, Al(kBg, fade));
    }

private:
    static constexpr float W = 1100, H = 700;

    // ----------------------------------------------------------- state
    Player p;
    std::vector<Course> courses;
    std::vector<Topic> topics;
    int course = 0, selTopic = 0;
    Screen screen = Screen::Menu, from = Screen::Menu, mapFrom = Screen::Menu;
    float time = 0, fade = 0, toastT = 0;
    std::string toast;
    float dispXp = 0, dispCoins = 0;

    // menu / map animation
    float cardHover[8] = { 0 }, rowHover[40] = { 0 }, nodeHover[40] = { 0 };
    Scroller scMenu, scBonus, scMap, scSide, scShop, scQuest, scCards, scHard;
    // hard mode
    const HardProblem* hard = nullptr;
    CodeEditor editor;
    TextJudge judge;
    Verdict lastV;
    bool hardRan = false, hardSolved = false;
    int hintsShown = 0;
    std::string hardReward;
    float hardKnob = 0;
    float charBounce = 99, gearHover[5] = { 0 };   // character screen animation
#ifdef JQ_SH2
    sh2::Konami konami;                          // easter egg (see Sh2Mode.h)
    std::unique_ptr<sh2::Mode> sh2mode;
#endif

    // lesson
    struct Item {
        bool isCode = false; int qi = 0; std::vector<int> order;
        // code questions: 0 build from pieces, 1 fill the blank, 2 type it
        int mode = 2;
        std::vector<std::string> tokens;           // build: pieces in correct order (`order` = tray shuffle)
        std::vector<int> placed;                   // build: token indices the player has placed, in order
        std::string before, mid, after;            // fill: accepted[0] split around the blank
        std::vector<std::string> choices;          // fill: 3 buttons
        int correctChoice = 0;
    };
    const Module* mod = nullptr;
    int modTopic = 0, modIdx = 0;                  // modIdx -1 == topic test
    std::vector<Item> items;
    int cur = 0, lives = 3, correct = 0, streak = 0, bestStreak = 0;
    bool answered = false, wasRight = false, showCodeHint = false;
    int picked = -1;
    bool hidden[8] = { false };
    std::string input, feedback;
    int resXp = 0, resCoins = 0, levelUps = 0;
    bool passed = false, firstClear = false;
    float itemT = 0, answerT = 0, hurtT = 0, progShown = 0, optHover[8] = { 0 };
    Vector2 fxAt{ 550, 300 };
    // result
    float resT = 0; bool resBurst = false;
    // lesson cards (shown before a module's questions)
    bool inCards = false; int cardIdx = 0, selPart = -1; float cardT = 0;

    void Go(Screen s) { screen = s; fade = 1.0f; }
    void Toast(const std::string& s) { toast = s; toastT = 2.5f; }

    // ----------------------------------------------------------- helpers
    std::string Key(int t, int m) const {
        std::string pre = course == 0 ? "" : "c" + std::to_string(course);
        return pre + (m < 0 ? "t" + std::to_string(t) + "test" : "t" + std::to_string(t) + "m" + std::to_string(m));
    }
    int UnlockId(int t) const { return course * 100 + t; }
    int CourseDone(int c) const {
        std::string pre = c == 0 ? "t" : "c" + std::to_string(c) + "t";
        int n = 0;
        for (auto& k : p.done) if (k.compare(0, pre.size(), pre) == 0 && (k.size() < 4 || k.compare(k.size() - 4, 4, "hard") != 0)) n++;
        return n;
    }
    bool TopicOpen(int t) const { return t == 0 || p.unlocked.count(UnlockId(t)) || (t > 0 && p.done.count(Key(t - 1, -1))); }
    bool ModuleDone(int t, int m) const { return p.done.count(Key(t, m)) > 0; }
    bool NodeOpen(int t, int i) const {
        if (i < (int)topics[t].modules.size()) return i == 0 || ModuleDone(t, i - 1);
        for (int m = 0; m < (int)topics[t].modules.size(); m++) if (!ModuleDone(t, m)) return false;
        return true;
    }
    void TopicProgress(int t, int& done, int& total) const {
        done = 0; total = (int)topics[t].modules.size();
        for (int m = 0; m < total; m++) if (ModuleDone(t, m)) done++;
        if (total > 0) { total++; if (p.done.count(Key(t, -1))) done++; }
    }
    static const char* LevelTitle(int lvl) {
        static const char* names[] = { "Rookie", "Coder", "Scripter", "Developer", "Engineer",
                                      "Architect", "Wizard", "Guru", "Legend" };
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
            for (int k = (int)it.order.size() - 1; k > 0; k--) std::swap(it.order[k], it.order[GetRandomValue(0, k)]);
            items.push_back(it);
        }
        if (mod->hasCode) { Item c; c.isCode = true; BuildCodeItem(c); items.push_back(c); }
        cur = 0; lives = 3 + p.extraLife; p.extraLife = 0;
        correct = streak = bestStreak = 0;
        resXp = resCoins = levelUps = 0; progShown = 0; hurtT = 0;
        ResetItem();
        inCards = !mod->lesson.empty(); cardIdx = 0; selPart = -1; cardT = 0; scCards.Reset();
        Go(Screen::Lesson);
    }
    // ----------------------------------------------------------- code question modes
    // The code question is a ladder: build from pieces -> fill the blank -> type it.
    // Mode comes from CodeTask::mode, or automatically from the topic's position in the course.
    struct Token { size_t pos = 0, len = 0; bool ok = false; };
    static bool IsIdCh(char c) { return std::isalnum((unsigned char)c) || c == '_'; }

    static std::vector<std::string> Tokenize(const std::string& s) {      // covers every non-space character
        static const char* two[] = { "==", "!=", "<=", ">=", "&&", "||", "++", "--", "->", "::", "+=", "-=", "*=", "/=" };
        std::vector<std::string> out; size_t n = s.size(), i = 0;
        while (i < n) {
            unsigned char ch = (unsigned char)s[i];
            if (std::isspace(ch)) { i++; continue; }
            if (ch == '"') {
                size_t j = i + 1;
                while (j < n && s[j] != '"') { if (s[j] == '\\' && j + 1 < n) j++; j++; }
                j = std::min(j + 1, n); out.push_back(s.substr(i, j - i)); i = j;
            }
            else if (ch == '\'') {                                   // char literal such as '\n' stays in one piece
                size_t j = i + 1;
                while (j < n && s[j] != '\'') { if (s[j] == '\\' && j + 1 < n) j++; j++; }
                j = std::min(j + 1, n); out.push_back(s.substr(i, j - i)); i = j;
            }
            else if (std::isalnum(ch) || ch == '_') {
                size_t j = i;
                while (j < n && (IsIdCh(s[j]) || (s[j] == '.' && j + 1 < n && IsIdCh(s[j + 1])))) j++;
                out.push_back(s.substr(i, j - i)); i = j;
            }
            else {
                bool m = false;
                if (i + 1 < n) { std::string d = s.substr(i, 2); for (auto t : two) if (d == t) { out.push_back(d); i += 2; m = true; break; } }
                if (!m) { out.push_back(std::string(1, s[i])); i++; }
            }
        }
        return out;
    }
    // Token to blank in fill mode: last "string", else last number, else last identifier before ; } ) {
    static Token FindToken(const std::string& s) {
        Token t; size_t n = s.size();
        for (size_t i = 0; i < n;) {
            if (s[i] == '"') {
                size_t j = i + 1;
                while (j < n && s[j] != '"') { if (s[j] == '\\' && j + 1 < n) j++; j++; }
                if (j >= n) break;
                t.pos = i; t.len = j - i + 1; t.ok = true; i = j + 1;
            }
            else i++;
        }
        if (t.ok) return t;
        for (size_t i = 0; i < n;) {
            if (std::isdigit((unsigned char)s[i]) && (i == 0 || !IsIdCh(s[i - 1]))) {
                size_t j = i;
                while (j < n && (std::isdigit((unsigned char)s[j]) || (s[j] == '.' && j + 1 < n && std::isdigit((unsigned char)s[j + 1])))) j++;
                if (j >= n || !IsIdCh(s[j])) { t.pos = i; t.len = j - i; t.ok = true; }
                i = j;
            }
            else i++;
        }
        if (t.ok) return t;
        for (size_t i = 0; i < n;) {
            if (std::isalpha((unsigned char)s[i]) || s[i] == '_') {
                size_t j = i; while (j < n && IsIdCh(s[j])) j++;
                size_t k = j; while (k < n && (s[k] == ' ' || s[k] == '\t')) k++;
                if (k < n && (s[k] == ';' || s[k] == '}' || s[k] == ')' || s[k] == '{')) { t.pos = i; t.len = j - i; t.ok = true; }
                i = j;
            }
            else i++;
        }
        return t;
    }
    static int TokCat(const std::string& t) {            // 0 string, 1 number, 2 identifier
        if (!t.empty() && t[0] == '"') return 0;
        if (!t.empty() && (std::isdigit((unsigned char)t[0]) || t[0] == '-')) return 1;
        return 2;
    }

    void BuildCodeItem(Item& it) {
        it.mode = 2;
        if (!mod || mod->code.accepted.empty()) return;
        const CodeTask& c = mod->code;
        const std::string& acc = c.accepted[0];
        float frac = topics.size() > 1 ? (float)modTopic / (float)topics.size() : 1.0f;   // position in the course
        int want = c.mode >= 0 ? c.mode : (frac < 0.25f ? 0 : (frac < 0.5f ? 1 : 2));     // build -> fill -> type
        if (want == 0) {
            std::vector<std::string> toks = Tokenize(acc);
            if (toks.size() >= 2 && toks.size() <= 16) {
                it.tokens = toks;
                it.order.resize(toks.size());
                std::iota(it.order.begin(), it.order.end(), 0);
                for (int k = (int)it.order.size() - 1; k > 0; k--) std::swap(it.order[k], it.order[GetRandomValue(0, k)]);
                bool sorted = true;
                for (size_t k = 0; k < it.order.size(); k++) if (it.order[k] != (int)k) sorted = false;
                if (sorted) std::swap(it.order[0], it.order[1]);       // never hand over the answer pre-assembled
                it.mode = 0; return;
            }
            want = 1;                                                  // too long to build: fall back to fill
        }
        if (want == 1) {
            Token t = FindToken(acc);
            if (!t.ok) return;
            it.before = acc.substr(0, t.pos); it.mid = acc.substr(t.pos, t.len); it.after = acc.substr(t.pos + t.len);
            int cat = TokCat(it.mid);
            std::vector<std::string> pool;                             // distractors from other code tasks
            auto consider = [&](const Module& m) {
                if (&m == mod || !m.hasCode || m.code.accepted.empty()) return;
                Token d = FindToken(m.code.accepted[0]);
                if (!d.ok) return;
                std::string txt = m.code.accepted[0].substr(d.pos, d.len);
                if (TokCat(txt) != cat || txt == it.mid) return;
                if (std::find(pool.begin(), pool.end(), txt) == pool.end()) pool.push_back(txt);
                };
            for (const Topic& T : topics) { for (const Module& m : T.modules) consider(m); consider(T.test); }
            for (int k = (int)pool.size() - 1; k > 0; k--) std::swap(pool[k], pool[GetRandomValue(0, k)]);
            std::vector<std::string> ch{ it.mid };
            for (auto& s : pool) { if (ch.size() >= 3) break; ch.push_back(s); }
            const std::string pad = cat == 0 ? "\"?\"" : (cat == 1 ? "0" : "foo");
            while (ch.size() < 3) {
                std::string s = pad;
                while (std::find(ch.begin(), ch.end(), s) != ch.end()) s += "x";
                ch.push_back(s);
            }
            for (int k = 2; k > 0; k--) std::swap(ch[k], ch[GetRandomValue(0, k)]);
            it.choices = ch;
            for (int k = 0; k < 3; k++) if (ch[k] == it.mid) it.correctChoice = k;
            it.mode = 1;
        }
    }
    void SubmitBuild(Item& it) {
        std::string joined;
        for (int k : it.placed) joined += it.tokens[k];
        bool right = false;
        for (auto& a : mod->code.accepted) if (Norm(a) == Norm(joined)) right = true;
        Grade(right, true, "Answer: " + mod->code.accepted[0], mod->code.explain);
    }

    // chips (build mode)
    static std::vector<Rectangle> FlowRects(const std::vector<std::string>& labels, float maxW, float fs, float& totalH) {
        std::vector<Rectangle> out; float cx = 0, cy = 0; const float h = 42, gap = 10;
        for (auto& l : labels) {
            float w = std::max(44.0f, TxtW(l, fs, true) + 30);
            if (cx > 0 && cx + w > maxW) { cx = 0; cy += h + gap; }
            out.push_back({ cx, cy, w, h }); cx += w + gap;
        }
        totalH = labels.empty() ? 0.0f : cy + h;
        return out;
    }
    bool Chip(Rectangle r, const std::string& label, Color tone, bool faded, float fs) {
        Vector2 m = Mouse();
        if (faded) { Box(r, 12, Al(kSurface2, 0.5f)); Txt(label, r.x + 15, r.y + (r.height - fs * 1.2f) / 2, fs, Al(kLock, 0.6f), true); return false; }
        bool hover = CheckCollisionPointRec(m, r);
        Rectangle d = r; if (hover) d.y -= 2;
        Shadow(d, 12, 3, 0.3f);
        BoxB(d, 12, Mix(kSurface2, tone, hover ? 0.3f : 0.12f), Mix(kBorder, tone, hover ? 1.0f : 0.4f), 2);
        Txt(label, d.x + 15, d.y + (d.height - fs * 1.2f) / 2, fs, kText, true);
        bool click = hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        if (click) { FxRing(m.x, m.y, tone, 36); FxSparks(m.x, m.y, tone, 6); }
        return click;
    }
    float HintPanel(float x, float y, float w) {
        float hh = Wrap("Hint: " + mod->code.hint, x + 52, y + 14, w - 80, 17, kSky, false);
        Box({ x, y, w, hh + 28 }, 14, Al(kSky, 0.12f));
        Icon(IC_BULB, x + 26, y + 14 + hh / 2, 10, kSky);
        Wrap("Hint: " + mod->code.hint, x + 52, y + 14, w - 80, 17, kSky);
        return hh + 28;
    }

    void DrawBuild(Item& it, float contentTop, float enter) {
        const CodeTask& c = mod->code;
        while (GetCharPressed() > 0) {}
        float ph = Wrap(c.prompt, 150, contentTop, 800, 28, Al(kText, enter), true, false, nullptr, 1.3f);
        const float fs = 20;
        std::vector<std::string> pl;
        for (int k : it.placed) pl.push_back(it.tokens[k]);
        float hp = 0; std::vector<Rectangle> pr = FlowRects(pl, 768, fs, hp);
        Rectangle box{ 150, contentTop + ph + 22, 800, std::max(74.0f, hp + 32) };
        Color edge = answered ? (wasRight ? kMint : kCoral) : kBorder;
        if (answered && !wasRight) box.x += std::sin(answerT * 48) * 12 * std::exp(-answerT * 7);
        Inset(box, 16, Color{ 9, 12, 20, 255 }, edge, 2);
        if (pl.empty()) Txt("Tap the pieces below to build the line", box.x + 24, box.y + box.height / 2 - 10, 17, kMuted);
        int removeAt = -1;
        for (size_t i = 0; i < pr.size(); i++) {
            Rectangle r{ pr[i].x + box.x + 16, pr[i].y + box.y + (box.height - hp) / 2, pr[i].width, pr[i].height };
            if (Chip(r, pl[i], answered ? edge : kSky, false, fs) && !answered) removeAt = (int)i;
        }
        if (removeAt >= 0) it.placed.erase(it.placed.begin() + removeAt);

        float ty = box.y + box.height + 26;
        Txt("Pieces", 150, ty, 14, kMuted);
        std::vector<std::string> tl;
        for (int o : it.order) tl.push_back(it.tokens[o]);
        float ht = 0; std::vector<Rectangle> tr = FlowRects(tl, 800, fs, ht);
        for (size_t i = 0; i < tr.size(); i++) {
            bool used = std::find(it.placed.begin(), it.placed.end(), it.order[i]) != it.placed.end();
            Rectangle r{ tr[i].x + 150, tr[i].y + ty + 26, tr[i].width, tr[i].height };
            if (Chip(r, tl[i], kViolet, used || answered, fs) && !answered) it.placed.push_back(it.order[i]);
        }
        if (showCodeHint) HintPanel(150, ty + 26 + ht + 20, 800);
        fxAt = { box.x + box.width - 140, box.y + box.height / 2 };
    }

    void DrawFill(Item& it, float contentTop, float enter) {
        const CodeTask& c = mod->code;
        while (GetCharPressed() > 0) {}
        float ph = Wrap(c.prompt, 150, contentTop, 800, 28, Al(kText, enter), true, false, nullptr, 1.3f);
        Rectangle box{ 150, contentTop + ph + 26, 800, 74 };
        if (answered && !wasRight) box.x += std::sin(answerT * 48) * 12 * std::exp(-answerT * 7);
        Inset(box, 16, Color{ 9, 12, 20, 255 }, answered ? (wasRight ? kMint : kCoral) : kBorder, 2);
        std::string mid = answered ? it.mid : "___";
        Color midCol = answered ? kMint : kSky;
        const Color codeCol{ 190, 255, 220, 255 };
        float fs = 24;
        while (fs > 14 && TxtW(it.before + mid + it.after, fs, true) > box.width - 90) fs -= 1;
        float ty = box.y + (box.height - fs * 1.25f) / 2, x = box.x + 50;
        float wb = TxtW(it.before, fs, true), wm = TxtW(mid, fs, true);
        Txt(">", box.x + 22, box.y + 22, 24, kViolet, true);
        Txt(it.before, x, ty, fs, codeCol, true);
        Box({ x + wb - 5, ty - 3, wm + 10, fs * 1.25f + 6 }, 7, Al(midCol, 0.22f));
        Txt(mid, x + wb, ty, fs, midCol, true);
        Txt(it.after, x + wb + wm, ty, fs, codeCol, true);

        float yy = box.y + box.height + 24;
        Txt("Pick the piece that belongs in the blank:", box.x, yy, 15, kMuted);
        for (int i = 0; i < 3; i++) {
            Rectangle r{ 170.0f + i * 260, yy + 34, 240, 58 };
            bool isC = i == it.correctChoice;
            if (!answered) {
                if (Button(r, it.choices[i], kSky, true, 20, true) || IsKeyPressed(KEY_ONE + i)) {
                    picked = i; fxAt = { r.x + 120, r.y + 29 };
                    Grade(isC, true, "Answer: " + c.accepted[0], c.explain);
                }
            }
            else {
                Color tone = isC ? kMint : (i == picked ? kCoral : kLock);
                BoxB(r, 16, Mix(kSurface, tone, isC || i == picked ? 0.16f : 0.0f), tone, 2);
                TxtC(Fit(it.choices[i], 216, 20), r.x + 120, r.y + 17, 20, isC || i == picked ? tone : kMuted);
            }
        }
        if (showCodeHint) HintPanel(box.x, yy + 110, box.width);
    }

    // ----------------------------------------------------------- lesson cards
    void DrawCards(float dt) {
        cardT += dt;
        const std::vector<LessonCard>& cards = mod->lesson;
        const LessonCard& c = cards[cardIdx];
        bool last = cardIdx + 1 >= (int)cards.size();
        bool replay = p.done.count(Key(modTopic, modIdx)) > 0;
        float enter = EaseOut(cardT / 0.35f);
        Vector2 m = Mouse();
        static const Color pal[5] = { kSky, kMint, kAmber, kViolet, kCoral };

        if (Button({ 24, 20, 48, 48 }, "", kSurface2, true, 20, true)) { p.Save(); Go(Screen::Map); return; }
        Icon(IC_CROSS, 48, 44, 9, kMuted);
        Txt("LEARN FIRST", 92, 24, 13, kSky);
        Txt(Fit(modIdx < 0 ? topics[modTopic].test.title : topics[modTopic].modules[modIdx].title, 600, 22), 92, 40, 22, kText);
        if (replay && Button({ W - 24 - 120, 20, 120, 44 }, "Skip", kSky, true, 19, true)) { inCards = false; itemT = 0; return; }

        float th0 = Wrap(c.text, 150, 0, 800, 21, kText, false, false, nullptr, 1.4f);
        float chc = std::max(H - 74 - 110, 112 + 58 + th0 + 26 + (c.code.empty() ? 0.0f : 74 + 18 + 130) + 30 - 74);
        scCards.Begin({ 0, 74, W, H - 74 - 110 }, chc, dt);
        m = Mouse();
        float y = 112;
        Txt(Fit(c.title, 800, 36), 150, y + 12 * (1 - enter), 36, Al(kText, enter));
        y += 58;
        y += Wrap(c.text, 150, y, 800, 21, Al(kText, 0.92f * enter), true, false, nullptr, 1.4f) + 26;

        if (!c.code.empty()) {
            Rectangle box{ 150, y, 800, 74 };
            Inset(box, 16, Color{ 9, 12, 20, 255 }, kBorder, 2);
            float fs = 26;
            while (fs > 14 && TxtW(c.code, fs, true) > box.width - 90) fs -= 1;
            float ty = box.y + (box.height - fs * 1.25f) / 2, x = box.x + 50;
            const Color codeCol{ 190, 255, 220, 255 };
            Txt(">", box.x + 22, box.y + 22, 24, kViolet, true);
            size_t pos = 0;
            auto gap = [&](size_t a, size_t b) {
                if (b > a) Txt(c.code.substr(a, b - a), x + TxtW(c.code.substr(0, a), fs, true), ty, fs, codeCol, true);
                };
            for (int pi = 0; pi < (int)c.parts.size(); pi++) {
                size_t f = c.code.find(c.parts[pi].first, pos);
                if (f == std::string::npos || c.parts[pi].first.empty()) continue;
                gap(pos, f);
                std::string sub = c.parts[pi].first;
                float px = x + TxtW(c.code.substr(0, f), fs, true), pw = TxtW(sub, fs, true);
                Color pc = pal[pi % 5];
                Rectangle hr{ px - 5, ty - 4, pw + 10, fs * 1.25f + 8 };
                bool hov = CheckCollisionPointRec(m, hr), sel = pi == selPart;
                Box(hr, 8, Al(pc, sel ? 0.38f : (hov ? 0.28f : 0.14f)));
                if (sel) Box({ hr.x, hr.y + hr.height - 3, hr.width, 3 }, 1.5f, pc);
                Txt(sub, px, ty, fs, pc, true);
                if (hov && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { selPart = pi; FxRing(m.x, m.y, pc, 40); }
                pos = f + sub.size();
            }
            gap(pos, c.code.size());

            float y2 = box.y + box.height + 18;
            if (!c.parts.empty()) {
                if (selPart >= 0 && selPart < (int)c.parts.size()) {
                    Color tone = pal[selPart % 5];
                    float lh = Wrap(c.parts[selPart].second, box.x + 24, y2 + 44, 752, 19, kText, false);
                    Box({ box.x, y2, box.width, lh + 58 }, 14, Al(tone, 0.14f));
                    Txt(c.parts[selPart].first, box.x + 24, y2 + 12, 20, tone, true);
                    Wrap(c.parts[selPart].second, box.x + 24, y2 + 44, 752, 19, kText);
                }
                else Txt("Tap any highlighted part of the code to see what it does.", box.x, y2, 16, kMuted);
            }
        }

        scCards.End();
        DrawRectangle(-1500, (int)(H - 110), 4100, 110, kSurface);
        DrawRectangle(-1500, (int)(H - 110), 4100, 2, kBorder);
        int n = (int)cards.size();
        for (int i = 0; i < n; i++)
            DrawCircleV({ W / 2 - (n - 1) * 14.0f + i * 28, H - 56 }, i == cardIdx ? 7.0f : 5.0f, i == cardIdx ? kSky : kBorder);
        if (cardIdx > 0 && Button({ 150, H - 84, 150, 56 }, "Back", kSky, true, 21, true)) { cardIdx--; cardT = 0; selPart = -1; scCards.Reset(); }
        if (Button({ W - 150 - 200, H - 84, 200, 56 }, last ? "Start quiz" : "Next", kMint, true, 23) || IsKeyPressed(KEY_ENTER)) {
            if (last) { inCards = false; itemT = 0; }
            else { cardIdx++; cardT = 0; selPart = -1; scCards.Reset(); }
        }
    }

    void ResetItem() {
        answered = wasRight = showCodeHint = false;
        picked = -1; input.clear(); feedback.clear();
        itemT = 0; answerT = 0;
        for (bool& h : hidden) h = false;
        for (float& o : optHover) o = 0;
    }
    static std::string Norm(const std::string& s) {
        std::string r;
        for (char c : s) if (c != ' ' && c != '\t' && c != '\n') r += c;
        return r;
    }
    void Grade(bool right, bool isCode, const std::string& wrongInfo, const std::string& expl) {
        answered = true; wasRight = right; answerT = 0;
        if (right) {
            correct++; streak++; bestStreak = std::max(bestStreak, streak);
            float mult = p.CoinMult() * (streak >= 3 ? 2.0f : 1.0f);
            int c = (int)std::lround((isCode ? 5 : 2) * mult);
            int x = isCode ? 20 : 10;
            p.coins += c; resCoins += c; resXp += x;
            int lv = p.AddXP(x); levelUps += lv;
            p.Note(QK_CORRECT, 1); p.Note(QK_STREAK, streak);
            if (isCode) p.Note(QK_CODE, 1);
            feedback = expl;
            FxBurst(fxAt.x, fxAt.y, isCode ? 70 : 34);
            FxRing(fxAt.x, fxAt.y, kMint, 90);
            FxFloat(fxAt.x + 40, fxAt.y - 30, "+" + std::to_string(x) + " XP", kMint);
            FxFloat(fxAt.x + 40, fxAt.y + 4, "+" + std::to_string(c) + " coins", kAmber, 22);
            if (lv > 0) { FxBurst(W / 2, 120, 60); Toast("Level up! You are now level " + std::to_string(p.Level())); }
        }
        else {
            lives--; streak = 0; hurtT = 0.6f;
            feedback = wrongInfo + "  " + expl;
            FxRing(fxAt.x, fxAt.y, kCoral, 80);
            FxSparks(fxAt.x, fxAt.y, kCoral, 14);
        }
    }
    void AnswerMC(int disp) {
        const Item& it = items[cur];
        const Question& q = mod->questions[it.qi];
        picked = disp;
        Grade(it.order[disp] == q.correct, false, "Answer: " + q.options[q.correct] + ".", q.explain);
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
        if (it.isCode) showCodeHint = true;
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
        if (cur >= (int)items.size()) Finish(true); else ResetItem();
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
        hard = (ok && p.hardMode) ? FindHard(courses[course].name, modTopic, modIdx) : nullptr;
        if (hard) StartHard(); else EnterResult();
    }
    void EnterResult() { resT = 0; resBurst = false; Go(Screen::Result); }

    // ----------------------------------------------------------- hard mode
    void StartHard() {
        editor.SetText(hard->starter);
        lastV = Verdict(); hardRan = false; hardSolved = false; hintsShown = 0; hardReward.clear();
        scHard.Reset();
        Go(Screen::Hard);
    }
    void RunHard() {
        lastV = judge.Run(*hard, editor.Text());
        hardRan = true;
        if (!lastV.accepted || hardSolved) return;
        hardSolved = true;
        std::string hk = Key(modTopic, modIdx) + "hard";
        if (!p.done.count(hk)) {
            p.done.insert(hk);
            resXp += hard->xp; resCoins += hard->coins; p.coins += hard->coins;
            levelUps += p.AddXP(hard->xp);
            p.Note(QK_CODE, 1);
            hardReward = "+" + std::to_string(hard->xp) + " XP   +" + std::to_string(hard->coins) + " coins  (first solve)";
        }
        else hardReward = "Solved again. The first-solve bonus was already claimed.";
        p.Save();
        FxBurst(770, 300, 70); FxRing(770, 300, kMint, 120);
    }

    // ----------------------------------------------------------- shared top bar
    void Pill(Rectangle r, int icon, Color c, const std::string& text) {
        Box(r, 14, kSurface2);
        Icon(icon, r.x + 24, r.y + 22, 12, c);
        Txt(text, r.x + 46, r.y + 10, 22, kText);
    }
    // returns true when the back button was clicked
    bool TopBar(const std::string& title, bool back, bool nav) {
        DrawRectangle(-1500, 0, 4100, 72, kSurface);
        DrawRectangle(-1500, 72, 4100, 2, kBorder);
        DrawRectangleGradientV(-1500, 74, 4100, 12, Al(Color{ 0, 0, 0, 255 }, 0.35f), Al(Color{ 0, 0, 0, 255 }, 0.0f));
        bool backClicked = false;
        float x;
        if (back) {
            backClicked = Button({ 24, 14, 44, 44 }, "", kSurface2, true, 24, true);
            DrawLineEx({ 50, 26 }, { 40, 36 }, 3, kText); DrawLineEx({ 40, 36 }, { 50, 46 }, 3, kText);
            x = 84;
        }
        else {
            Box({ 24, 16, 40, 40 }, 12, kViolet);
            TxtC("{}", 44, 22, 22, Color{ 255, 255, 255, 255 }, true);
            x = 76;
        }
        Txt(Fit(title, 300, 28), x, 18, 28, kText);

        float cx = W - 24;
        if (nav) {
            int claimable = 0;
            for (int i = 0; i < (int)p.quests.size(); i++) if (p.CanClaim(i)) claimable++;
            std::string ql = claimable ? "Quests (" + std::to_string(claimable) + ")" : "Quests";
            if (Button({ cx - 150, 14, 150, 44 }, ql, claimable ? kCoral : kAmber, true, 19)) { from = screen; Go(Screen::Quests); }
            cx -= 160;
            if (Button({ cx - 100, 14, 100, 44 }, "Shop", kSky, true, 19)) { from = screen; Go(Screen::Shop); }
            cx -= 112;
        }
        Pill({ cx - 92, 14, 92, 44 }, IC_BULB, kSky, std::to_string(p.hints)); cx -= 102;
        Pill({ cx - 124, 14, 124, 44 }, IC_COIN, kAmber, std::to_string((int)std::lround(dispCoins))); cx -= 134;
        Rectangle lp{ cx - 210, 14, 210, 44 };
        bool lpHover = screen != Screen::Character && CheckCollisionPointRec(Mouse(), lp);
        Box(lp, 14, lpHover ? Mix(kSurface2, kMint, 0.14f) : kSurface2);
        if (lpHover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { from = screen; Go(Screen::Character); }
        DrawCircleV({ lp.x + 24, lp.y + 22 }, 17, kMint);
        TxtC(std::to_string(p.Level()), lp.x + 24, lp.y + 10, 20, kBg);
        float inLvl = dispXp - 100.0f * std::floor(dispXp / 100.0f);
        Txt("XP " + std::to_string((int)inLvl) + " / 100", lp.x + 50, lp.y + 5, 13, kMuted);
        Bar({ lp.x + 50, lp.y + 27, 148, 9 }, inLvl / 100.0f, kMint);
        return backClicked;
    }

    // ----------------------------------------------------------- MAIN MENU / BONUS
    int CppIndex() const { for (int i = 0; i < (int)courses.size(); i++) if (courses[i].name == "C++") return i; return 0; }
    bool IsBonus(int i) const { return i != CppIndex(); }
    static std::string Glyph(const std::string& n) {
        if (n == "Java") return "J";
        if (n == "Python") return "Py";
        if (n == "C#") return "C#";
        if (n == "JavaScript") return "JS";
        return n.substr(0, 1);
    }

    void OpenCourse(int i, Screen back) {
        course = i; topics = courses[i].topics; mapFrom = back;
        selTopic = 0;                                                   // resume at the first unfinished open topic
        for (int t = 0; t < (int)topics.size(); t++) {
            if (topics[t].modules.empty() || !TopicOpen(t)) continue;
            int d, tot; TopicProgress(t, d, tot);
            if (d < tot) { selTopic = t; break; }
        }
        scSide.Reset(); scMap.Reset();
        scSide.target = std::max(0.0f, selTopic * 66.0f - 120);
        Go(Screen::Map);
    }

    void DrawMenu() {
        TopBar("JavaQuest", false, true);
        scMenu.Begin({ 0, 74, W, H - 74 }, std::max(H - 74, 640.0f - 74), GetFrameTime());
        float dt = GetFrameTime();
        Vector2 m = Mouse();
        int ci = CppIndex();
        const Course& cc = courses[ci];

        Txt("Learn C++", 35, 90, 40, kText);
        Txt("From your first program to templates and modern C++. Short lessons, quick quizzes, real progress.", 35, 142, 17, kMuted);

        // ---- hero card: the C++ course
        Rectangle base{ 35, 184, 1030, 244 };
        bool hov = CheckCollisionPointRec(m, base);
        cardHover[0] = Smooth(cardHover[0], hov ? 1.0f : 0.0f, 16, dt);
        float h = cardHover[0];
        Rectangle r{ base.x, base.y - 6 * h, base.width, base.height };
        Card3D(r, 26, Mix(kSurface, kSurface2, h), Mix(kBorder, cc.color, h), 9);
        Rectangle ic{ r.x + 34, r.y + 34, 136, 136 };
        Card3D(ic, 32, cc.color, Mix(cc.color, Color{ 255, 255, 255, 255 }, 0.25f), 6);
        TxtC("C++", ic.x + 68, ic.y + 40, 52, OnColor(cc.color));
        Txt("The C++ Course", r.x + 204, r.y + 30, 34, kText);
        int nTopics = (int)cc.topics.size(), total = std::max(1, cc.TotalNodes()), done = CourseDone(ci);
        Txt(std::to_string(nTopics) + " topics  -  " + std::to_string(total) + " modules and tests  -  hard-mode boss problems", r.x + 204, r.y + 76, 16, kMuted);
        Wrap(cc.tagline, r.x + 204, r.y + 108, 500, 17, kText, true, false, nullptr, 1.35f);
        Bar({ r.x + 204, r.y + 178, 520, 14 }, (float)done / total, cc.color);
        Txt(std::to_string(done) + " / " + std::to_string(total) + " modules   -   " + std::to_string(100 * done / total) + "%", r.x + 204, r.y + 202, 15, kMuted);
        Rectangle go{ r.x + r.width - 34 - 250, r.y + (r.height - 64) / 2, 250, 64 };
        if (Button(go, done > 0 ? "Continue" : "Start learning", kMint, true, 24)) OpenCourse(ci, Screen::Menu);
        if (hov && !CheckCollisionPointRec(m, go) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { FxRing(m.x, m.y, cc.color, 70); OpenCourse(ci, Screen::Menu); }

        // ---- bonus card
        Rectangle bb0{ 35, 462, 1030, 140 };
        bool bhov = CheckCollisionPointRec(m, bb0);
        cardHover[1] = Smooth(cardHover[1], bhov ? 1.0f : 0.0f, 16, dt);
        float bh = cardHover[1];
        Rectangle br{ bb0.x, bb0.y - 5 * bh, bb0.width, bb0.height };
        Card3D(br, 24, Mix(kSurface, kSurface2, bh), Mix(kBorder, kViolet, bh), 8);
        Rectangle bi{ br.x + 30, br.y + 30, 80, 80 };
        Card3D(bi, 22, kViolet, Mix(kViolet, Color{ 255, 255, 255, 255 }, 0.25f), 5);
        Icon(IC_STAR, bi.x + 40, bi.y + 40, 24, OnColor(kViolet));
        Txt("Bonus languages", br.x + 136, br.y + 28, 30, kText);
        Txt("Java, Python, C# and JavaScript. Extra practice with the same XP, coins and quests.", br.x + 136, br.y + 72, 16, kMuted);
        int bdone = 0, btotal = 0;
        float chipX = br.x + br.width - 40 - 4 * 76;
        int k = 0;
        for (int i = 0; i < (int)courses.size(); i++) {
            if (!IsBonus(i)) continue;
            bdone += CourseDone(i); btotal += courses[i].TotalNodes();
            Rectangle chip{ chipX + k * 76, br.y + 36, 64, 64 };
            Card3D(chip, 16, courses[i].color, Mix(courses[i].color, Color{ 255, 255, 255, 255 }, 0.25f), 4);
            std::string g = Glyph(courses[i].name);
            TxtC(g, chip.x + 32, chip.y + 18, g.size() > 2 ? 22.0f : 26.0f, OnColor(courses[i].color));
            k++;
        }
        Txt(std::to_string(bdone) + " modules done in bonus courses", br.x + 136, br.y + 100, 14, kMuted);
        if (bhov && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { FxRing(m.x, m.y, kViolet, 70); scBonus.Reset(); Go(Screen::Bonus); }
        scMenu.End();
    }

    void DrawBonus() {
#ifdef JQ_SH2
        if (konami.Feed()) { sh2mode = std::make_unique<sh2::Mode>(); return; }   // Up Up Down Down Left Right Left Right B A
#endif
        if (TopBar("Bonus languages", true, true)) { Go(Screen::Menu); return; }
        std::vector<int> idx;
        for (int i = 0; i < (int)courses.size(); i++) if (IsBonus(i)) idx.push_back(i);
        int rows = ((int)idx.size() + 2) / 3;
        scBonus.Begin({ 0, 74, W, H - 74 }, std::max(H - 74, 190.0f + rows * 235 + 10 - 74), GetFrameTime());
        float dt = GetFrameTime();
        Vector2 m = Mouse();
        Txt("Bonus languages", 35, 92, 36, kText);
        Txt("Extra practice outside the main C++ course. Progress, XP and coins are shared.", 35, 140, 17, kMuted);
        for (int k = 0; k < (int)idx.size() && k < 8; k++) {
            int i = idx[k];
            const Course& c = courses[i];
            Rectangle base{ 35.0f + (k % 3) * 350, 190.0f + (k / 3) * 235, 330, 215 };
            bool hover = CheckCollisionPointRec(m, base);
            cardHover[2 + k] = Smooth(cardHover[2 + k], hover ? 1.0f : 0.0f, 16, dt);
            float h = cardHover[2 + k];
            Rectangle r{ base.x, base.y - 6 * h, base.width, base.height };
            Card3D(r, 22, Mix(kSurface, kSurface2, h), Mix(kBorder, c.color, h), 8);
            Rectangle ic{ r.x + 22, r.y + 22, 64, 64 };
            Card3D(ic, 18, c.color, Mix(c.color, Color{ 255, 255, 255, 255 }, 0.25f), 4);
            std::string g = Glyph(c.name);
            TxtC(g, ic.x + 32, ic.y + 15, g.size() > 2 ? 26.0f : 32.0f, OnColor(c.color));
            Txt(c.name, r.x + 104, r.y + 24, 28, kText);
            Txt(std::to_string(c.topics.size()) + " topics", r.x + 104, r.y + 58, 15, kMuted);
            Wrap(c.tagline, r.x + 22, r.y + 102, r.width - 44, 15, kMuted, true, false, nullptr, 1.3f);
            if (c.Ready()) {
                int done = CourseDone(i), total = std::max(1, c.TotalNodes());
                Bar({ r.x + 22, r.y + 166, r.width - 44, 10 }, (float)done / total, c.color);
                Txt(std::to_string(done) + " / " + std::to_string(total) + " modules", r.x + 22, r.y + 184, 14, kMuted);
                TxtR(std::to_string(100 * done / total) + "%", r.x + r.width - 22, r.y + 184, 14, kText);
            }
            else {
                Txt("Coming soon - roadmap preview", r.x + 22, r.y + 180, 14, kMuted);
            }
            if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { FxRing(m.x, m.y, c.color, 70); OpenCourse(i, Screen::Bonus); }
        }
        scBonus.End();
    }

    // ----------------------------------------------------------- MAP
    void DrawMap() {
        if (TopBar(courses[course].name, true, true)) { p.Save(); Go(mapFrom); return; }
        float dt = GetFrameTime();
        Vector2 m = Mouse();

        // sidebar
        DrawRectangle(-1500, 74, 1810, 3000, kSurface);
        DrawRectangle(310, 74, 2, 3000, kBorder);
        Txt("TOPICS", 26, 90, 14, kMuted);
        const float listTop = 116, listH = H - 116 - 142;
        int n = (int)topics.size();
        scSide.Begin({ 0, listTop, 310, listH }, n * 66.0f, dt);
        Vector2 ms = Mouse();
        for (int t = 0; t < n; t++) {
            Rectangle rr{ 14, listTop + t * 66, 268, 58 };
            bool open = TopicOpen(t), sel = t == selTopic;
            bool hov = CheckCollisionPointRec(ms, rr);
            if (t < 40) rowHover[t] = Smooth(rowHover[t], hov ? 1.0f : 0.0f, 16, dt);
            Color tc = topics[t].color;
            if (sel) { Card3D(rr, 14, Mix(kSurface2, tc, 0.2f), tc, 4); }
            else Box(rr, 14, Mix(kSurface, kSurface2, t < 40 ? rowHover[t] : 0));
            float cy = rr.y + 29;
            DrawCircleV({ rr.x + 32, cy }, 18, open ? tc : kLock);
            if (open) TxtC(std::to_string(t + 1), rr.x + 32, cy - 11, 20, OnColor(tc));
            else Icon(IC_LOCK, rr.x + 32, cy - 2, 8, kText);
            Txt(Fit(topics[t].name, 268 - 62 - 34, 17), rr.x + 62, rr.y + 7, 17, open ? kText : kMuted);
            int d, tot; TopicProgress(t, d, tot);
            std::string sub = topics[t].modules.empty() ? "Coming soon" : (open ? std::to_string(d) + " / " + std::to_string(tot) + " done" : "Locked");
            Txt(sub, rr.x + 62, rr.y + 33, 14, kMuted);
            if (tot > 0 && d == tot) Icon(IC_CHECK, rr.x + rr.width - 24, cy, 8, kMint);
            if (hov && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { selTopic = t; scMap.Reset(); FxRing(ms.x, ms.y, tc, 40); }
        }
        scSide.End();

        // character card
        Rectangle ac{ 16, H - 134, 278, 118 };
        Card3D(ac, 18, kSurface2, kBorder, 6);
        DrawAvatar(66, ac.y + 58 + std::sin(time * 2.2f) * 2.5f, 0.6f, p.Level(), true, p.gearHidden, p.outfit);
        Txt("Level " + std::to_string(p.Level()), 122, ac.y + 20, 22, kText);
        Txt(LevelTitle(p.Level()), 122, ac.y + 50, 16, kMint);
        Txt(std::to_string(CourseDone(course)) + " modules cleared", 122, ac.y + 78, 14, kMuted);
        if (CheckCollisionPointRec(m, ac)) {
            Txt("View character >", 122, ac.y + 98, 13, kSky);
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { from = Screen::Map; Go(Screen::Character); }
        }

        // main area
        const Topic& T = topics[selTopic];
        const float x0 = 350, cw = 716;
        {
            float bh0 = Wrap(T.blurb, x0, 138, cw, 17, kMuted, false);
            int cnt0 = (int)T.modules.size() + 1;
            float ch0 = (T.modules.empty() || !TopicOpen(selTopic)) ? H - 74
                : std::max(H - 74, std::max(250.0f, 138 + bh0 + 70) + ((cnt0 + 5) / 6) * 118 + 100 - 74);
            scMap.Begin({ 312, 74, W - 312, H - 74 }, ch0, dt);
            m = Mouse();
        }
        Txt(Fit(T.name, cw - 210, 34), x0, 92, 34, T.color);
        float bh = Wrap(T.blurb, x0, 138, cw, 17, kMuted);
        {   // hard-mode switch
            Rectangle sw{ x0 + cw - 62, 100, 56, 30 };
            bool hov = CheckCollisionPointRec(m, { x0 + cw - 190, 94, 190, 42 });
            hardKnob = Smooth(hardKnob, p.hardMode ? 1.0f : 0.0f, 14, dt);
            Txt("Hard mode", x0 + cw - 186, 104, 17, p.hardMode ? kCoral : kMuted);
            Inset(sw, 15, Mix(kSurface2, kCoral, hardKnob * 0.6f), Mix(kBorder, kCoral, hardKnob));
            DrawCircleV({ sw.x + 15 + hardKnob * 26, sw.y + 17 }, 10.5f, Mix(kMuted, Color{ 255, 255, 255, 255 }, hardKnob));
            DrawCircleV({ sw.x + 15 + hardKnob * 26, sw.y + 14 }, 10.5f, Mix(Color{ 190, 198, 220, 255 }, Color{ 255, 255, 255, 255 }, hardKnob));
            if (hov && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { p.hardMode = !p.hardMode; p.Save(); Toast(p.hardMode ? "Hard mode on: modules end with a boss problem" : "Hard mode off"); }
        }

        if (T.modules.empty()) {
            Rectangle r{ x0 + 100, 280, cw - 200, 190 };
            BoxB(r, 22, kSurface, kBorder, 2);
            TxtC("Content coming soon", r.x + r.width / 2, r.y + 50, 30, kText);
            TxtC("Add modules for this topic in Content.h", r.x + r.width / 2, r.y + 100, 17, kMuted);
            scMap.End(); return;
        }
        if (!TopicOpen(selTopic)) {
            Rectangle r{ x0 + 100, 250, cw - 200, 270 };
            BoxB(r, 22, kSurface, kBorder, 2);
            Icon(IC_LOCK, r.x + r.width / 2, r.y + 62, 30, kMuted);
            TxtC("This topic is locked", r.x + r.width / 2, r.y + 110, 28, kText);
            TxtC("Pass the previous topic test, or unlock it with coins.", r.x + r.width / 2, r.y + 150, 16, kMuted);
            bool afford = p.coins >= T.cost;
            if (Button({ r.x + r.width / 2 - 110, r.y + 196, 220, 52 }, "Unlock  -  " + std::to_string(T.cost), kAmber, afford, 20)) {
                p.coins -= T.cost; p.unlocked.insert(UnlockId(selTopic)); p.Save();
                FxBurst(r.x + r.width / 2, r.y + 220, 40); Toast("Topic unlocked!");
            }
            scMap.End(); return;
        }

        // module path (snake layout)
        int count = (int)T.modules.size() + 1;
        const int cols = 6;
        float top = std::max(250.0f, 138 + bh + 70);
        float stepX = cw / cols, stepY = 118;
        int firstOpen = -1;
        for (int i = 0; i < count; i++) {
            int mm = i == (int)T.modules.size() ? -1 : i;
            if (NodeOpen(selTopic, i) && !p.done.count(Key(selTopic, mm))) { firstOpen = i; break; }
        }
        auto nodePos = [&](int i) {
            int row = i / cols, c = i % cols;
            if (row % 2 == 1) c = cols - 1 - c;
            return Vector2{ x0 + stepX * (c + 0.5f), top + row * stepY };
            };
        for (int i = 0; i + 1 < count; i++) {                      // connectors
            Vector2 a = nodePos(i), b = nodePos(i + 1);
            int mm = i == (int)T.modules.size() ? -1 : i;
            bool d = p.done.count(Key(selTopic, mm)) > 0;
            DrawLineEx(a, b, 6, d ? Al(kMint, 0.55f) : kSurface2);
        }
        for (int i = 0; i < count; i++) {
            bool isTest = i == (int)T.modules.size();
            int mm = isTest ? -1 : i;
            Vector2 c = nodePos(i);
            bool done = p.done.count(Key(selTopic, mm)) > 0;
            bool open = NodeOpen(selTopic, i);
            float rad = isTest ? 42.0f : 36.0f;
            bool hover = CheckCollisionPointCircle(m, c, rad + 4);
            float& hv = nodeHover[std::min(i, 39)];
            hv = Smooth(hv, (hover && open) ? 1.0f : 0.0f, 16, dt);
            bool down = hover && open && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
            if (down) c.y += 6;                                      // pressed: sinks into its base
            float s = 1 + 0.08f * hv - (down ? 0.12f : 0.0f);
            if (i == firstOpen) {                                   // pulsing "next up" glow
                float pu = 0.5f + 0.5f * std::sin(time * 3.2f);
                DrawCircleV(c, (rad + 10 + 6 * pu) * s, Al(T.color, 0.18f + 0.12f * pu));
            }
            Color base = done ? kMint : (open ? T.color : kLock);
            DrawCircleV({ c.x, c.y + 11 }, rad * s, Mix(base, kBg, 0.62f));
            DrawCircleV({ c.x, c.y + 6 }, rad * s, Mix(base, kBg, 0.38f));
            DrawCircleV(c, rad * s, base);
            DrawCircleV({ c.x - rad * 0.25f * s, c.y - rad * 0.3f * s }, rad * 0.45f * s, Al(Color{ 255, 255, 255, 255 }, 0.12f));
            if (done) Icon(IC_CHECK, c.x, c.y, 15 * s, kBg);
            else if (!open) Icon(IC_LOCK, c.x, c.y - 2, 13 * s, kText);
            else if (isTest) Icon(IC_STAR, c.x, c.y, 20 * s, OnColor(base));
            else TxtC(std::to_string(i + 1), c.x, c.y - 15 * s, 30 * s, OnColor(base));
            const std::string& title = isTest ? T.test.title : T.modules[i].title;
            TxtC(Fit(title, stepX - 8, 14), c.x, c.y + rad + 14, 14, open ? kText : kMuted);
            if (p.hardMode && FindHard(courses[course].name, selTopic, mm)) {             // hard-mode badge
                bool hs = p.done.count(Key(selTopic, mm) + "hard") > 0;
                Vector2 bp{ c.x + rad * 0.72f, c.y - rad * 0.72f };
                DrawCircleV({ bp.x, bp.y + 3 }, 13, Mix(hs ? kMint : kCoral, kBg, 0.5f));
                DrawCircleV(bp, 13, hs ? kMint : kCoral);
                Icon(hs ? IC_CHECK : IC_FLAME, bp.x, bp.y, 6.5f, kBg);
            }
            if (hover && open && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { FxRing(c.x, c.y, T.color, 80); StartLesson(selTopic, mm); }
        }
        Icon(IC_FLAME, x0 + 8, H - 26, 8, kAmber);
        Txt("3 correct in a row starts a COMBO: double coins!", x0 + 24, H - 36, 15, kMuted);
        scMap.End();
    }

    // ----------------------------------------------------------- LESSON
    void DrawLesson(float dt) {
        if (inCards) { DrawCards(dt); return; }
        if (answered) answerT += dt; else answerT = 0;
        itemT += dt; if (hurtT > 0) hurtT -= dt;
        const Topic& T = topics[modTopic];
        Item& it = items[cur];
        Vector2 m = Mouse();

        // header
        if (Button({ 24, 20, 48, 48 }, "", kSurface2, true, 20, true)) { p.Save(); Go(Screen::Map); return; }
        Icon(IC_CROSS, 48, 44, 9, kMuted);
        float target = (float)(cur + (answered && wasRight ? 1 : 0)) / (float)items.size();
        progShown = Smooth(progShown, target, 8, dt);
        Bar({ 92, 34, 690, 20 }, progShown, T.color);
        for (int i = 0; i < 4; i++) {
            if (i >= lives && !(i == lives && hurtT > 0)) continue;
            float k = i == lives ? hurtT / 0.6f : 0;                 // heart being lost fades and shakes
            float sh = k > 0 ? std::sin(time * 60) * 4 * k : 0;
            DrawHeart(836 + i * 46 + sh, 38, 12 * (k > 0 ? 1 + 0.4f * (1 - k) : 1), k > 0 ? Al(kCoral, k) : kCoral);
        }
        std::string head = (modIdx < 0 ? T.test.title : T.modules[modIdx].title) + "   -   " +
            (it.isCode ? (it.mode == 0 ? "Build the code" : it.mode == 1 ? "Fill in the blank" : "Code challenge") : "Question " + std::to_string(cur + 1) + " of " + std::to_string(items.size()));
        Txt(Fit(head, 600, 15), 150, 86, 15, it.isCode ? kViolet : kMuted);
        if (streak >= 3) {
            float pu = 1 + 0.05f * std::sin(time * 8);
            Rectangle chip = ScaleRect({ W - 24 - 210, 76, 210, 30 }, pu);
            Box(chip, 15, Al(kAmber, 0.2f));
            Icon(IC_FLAME, chip.x + 18, chip.y + 14, 8, kAmber);
            Txt("COMBO x2 coins  (" + std::to_string(streak) + ")", chip.x + 34, chip.y + 5, 16, kAmber);
        }

        float enter = EaseOut(itemT / 0.3f);
        float contentTop = 118;

        if (!it.isCode) {
            const Question& q = mod->questions[it.qi];
            float ph = Wrap(q.prompt, 150, contentTop, 800, 30, Al(kText, enter), true, false, nullptr, 1.3f);
            float oy = contentTop + ph + 22;
            int n = (int)q.options.size();
            float fs = 23; std::vector<float> hs(n);
            for (int pass = 0; pass < 2; pass++) {
                float total = 0;
                for (int i = 0; i < n; i++) {
                    float th = Wrap(q.options[it.order[i]], 0, 0, 800 - 110, fs, kText, false);
                    hs[i] = std::max(60.0f, th + 28); total += hs[i] + 12;
                }
                if (oy + total < H - 150 || pass == 1) break;
                fs = 18; oy = contentTop + ph + 14;
            }
            float y = oy;
            for (int i = 0; i < n; i++) {
                float k = EaseOut((itemT - 0.06f * i) / 0.3f);
                Rectangle base{ 150 + (1 - k) * 40, y, 800, hs[i] };
                y += hs[i] + 12;
                bool hid = hidden[i];
                bool isCorrect = it.order[i] == q.correct;
                bool hover = !answered && !hid && CheckCollisionPointRec(m, base);
                optHover[i] = Smooth(optHover[i], hover ? 1.0f : 0.0f, 18, dt);
                Rectangle r = base; r.x += 8 * optHover[i];
                float sc = 1.0f;
                Color fill = kSurface, edge = kBorder, txt = kText;
                if (hid) { fill = kBg; edge = kBg; txt = kLock; }
                else if (answered && isCorrect) {
                    fill = Mix(kSurface, kMint, 0.18f); edge = kMint;
                    sc = 1 + 0.035f * std::sin(Clamp01(answerT / 0.4f) * 3.14159f);
                }
                else if (answered && i == picked) {
                    fill = Mix(kSurface, kCoral, 0.18f); edge = kCoral;
                    r.x += std::sin(answerT * 48) * 12 * std::exp(-answerT * 7);
                }
                else if (answered) txt = kMuted;
                else { fill = Mix(kSurface, kSurface2, optHover[i]); edge = Mix(kBorder, kSky, optHover[i]); }
                r = ScaleRect(r, sc);
                if (hid) BoxB(r, 16, Al(fill, k), Al(edge, k), 2);
                else Card3D(r, 16, fill, edge, 5);
                Color badge = (answered && isCorrect) ? kMint : (answered && i == picked) ? kCoral : kSurface2;
                DrawCircleV({ r.x + 34, r.y + r.height / 2 }, 16, Al(badge, k));
                if (answered && isCorrect) Icon(IC_CHECK, r.x + 34, r.y + r.height / 2, 7, kBg);
                else if (answered && i == picked) Icon(IC_CROSS, r.x + 34, r.y + r.height / 2, 6, kBg);
                else TxtC(std::to_string(i + 1), r.x + 34, r.y + r.height / 2 - 10, 18, Al(hid ? kLock : kMuted, k));
                float th = Wrap(q.options[it.order[i]], 0, 0, 800 - 110, fs, txt, false);
                Wrap(q.options[it.order[i]], r.x + 68, r.y + (r.height - th) / 2 + 1, 800 - 110, fs, Al(txt, k));
                bool key = !answered && !hid && IsKeyPressed(KEY_ONE + i);
                if (!answered && !hid && ((hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) || key)) {
                    fxAt = { r.x + r.width - 120, r.y + r.height / 2 };
                    AnswerMC(i);
                }
            }
        }
        else if (it.mode == 0) {
            DrawBuild(it, contentTop, enter);
        }
        else if (it.mode == 1) {
            DrawFill(it, contentTop, enter);
        }
        else {
            const CodeTask& c = mod->code;
            float ph = Wrap(c.prompt, 150, contentTop, 800, 28, Al(kText, enter), true, false, nullptr, 1.3f);
            Rectangle box{ 150, contentTop + ph + 26, 800, 74 };
            Color edge = answered ? (wasRight ? kMint : kCoral) : Mix(kBorder, kViolet, 0.5f + 0.5f * std::sin(time * 3));
            if (answered && !wasRight) box.x += std::sin(answerT * 48) * 12 * std::exp(-answerT * 7);
            Inset(box, 16, Color{ 9, 12, 20, 255 }, edge, 2);
            std::string shown = input;
            while (shown.size() > 1 && TxtW(shown, 24, true) > box.width - 90) shown.erase(0, 1);
            if (!answered && (int)(time * 2) % 2 == 0) shown += "|";
            Txt(">", box.x + 22, box.y + 22, 24, kViolet, true);
            Txt(shown, box.x + 50, box.y + 22, 24, Color{ 190, 255, 220, 255 }, true);
            float yy = box.y + box.height + 20;
            if (showCodeHint) {
                float hh = Wrap("Hint: " + c.hint, box.x + 52, yy + 14, 700, 17, kSky, false);
                Box({ box.x, yy, box.width, hh + 28 }, 14, Al(kSky, 0.12f));
                Icon(IC_BULB, box.x + 26, yy + 14 + hh / 2, 10, kSky);
                Wrap("Hint: " + c.hint, box.x + 52, yy + 14, 700, 17, kSky);
                yy += hh + 48;
            }
            Txt("Type your answer and press Enter. Spaces don't matter.", box.x, yy, 15, kMuted);
            if (!answered) {
                int ch = GetCharPressed();
                while (ch > 0) { if (ch >= 32 && ch <= 126 && input.size() < 90) input += (char)ch; ch = GetCharPressed(); }
                if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && !input.empty()) input.pop_back();
            }
            fxAt = { box.x + box.width - 140, box.y + box.height / 2 };
        }

        // bottom bar
        if (!answered) {
            Rectangle bar{ 0, H - 110, W, 110 };
            DrawRectangle(-1500, (int)bar.y, 4100, (int)bar.height, kSurface);
            DrawRectangle(-1500, (int)bar.y, 4100, 2, kBorder);
            if (Button({ 150, H - 84, 170, 56 }, "Hint  (" + std::to_string(p.hints) + ")", kSky, p.hints > 0, 21, true)) UseHint();
            if (it.isCode && it.mode != 1) {
                bool ok = it.mode == 0 ? !it.placed.empty() : !input.empty();
                if (Button({ W - 150 - 200, H - 84, 200, 56 }, "Check", kMint, ok, 23) || (ok && IsKeyPressed(KEY_ENTER))) {
                    if (it.mode == 0) SubmitBuild(it); else SubmitCode();
                }
            }
        }
        else {
            float panelH = 150, k = EaseOut(answerT / 0.28f);
            float y = H - panelH * k;
            Color tone = wasRight ? kMint : kCoral;
            DrawRectangle(-1500, (int)y, 4100, (int)panelH + 4, Mix(kBg, tone, 0.14f));
            DrawRectangle(-1500, (int)y, 4100, 3, tone);
            float ic = 150 + 28;
            float pop = EaseOutBack(answerT / 0.35f);
            DrawCircleV({ ic, y + 52 }, 28 * pop, tone);
            Icon(wasRight ? IC_CHECK : IC_CROSS, ic, y + 52, 12 * pop, kBg);
            Txt(wasRight ? "Correct!" : "Not quite", 150 + 72, y + 18, 30, tone);
            int lines = 0; float fs = 17;
            Wrap(feedback, 0, 0, 560, fs, kText, false, false, &lines);
            if (lines > 3) fs = 14.5f;
            Wrap(feedback, 150 + 72, y + 62, 560, fs, Al(kText, 0.92f));
            const char* label = (lives <= 0 && !wasRight) ? "Finish" : "Continue";
            if (Button({ W - 150 - 200, y + 46, 200, 58 }, label, tone, true, 23) || IsKeyPressed(KEY_ENTER)) NextItem();
        }
    }

    // ----------------------------------------------------------- HARD MODE SCREEN
    void DrawHard(float dt) {
        const HardProblem& H0 = *hard;
        DrawRectangle(-1500, 0, 4100, 72, kSurface);
        DrawRectangle(-1500, 72, 4100, 2, kBorder);
        DrawRectangleGradientV(-1500, 74, 4100, 12, Al(Color{ 0, 0, 0, 255 }, 0.35f), Al(Color{ 0, 0, 0, 255 }, 0.0f));
        Box({ 24, 16, 330, 40 }, 20, Al(kCoral, 0.18f));
        Icon(IC_FLAME, 48, 36, 9, kCoral);
        Txt("HARD MODE  -  Boss challenge", 68, 24, 16, kCoral);
        if (Button({ W - 24 - 160, 14, 160, 46 }, hardSolved ? "Continue" : "Skip", hardSolved ? kMint : kCoral, true, 20, !hardSolved)) { EnterResult(); return; }

        // ---- left: problem statement
        Rectangle lp{ 24, 94, 436, H - 120 };
        Card3D(lp, 20, kSurface, kBorder, 6);
        Txt(Fit(H0.title, 290, 28), 46, 108, 28, kText);
        Color dc = H0.difficulty == "Hard" ? kCoral : (H0.difficulty == "Medium" ? kAmber : kMint);
        Box({ lp.x + lp.width - 24 - 104, 112, 104, 30 }, 15, Al(dc, 0.2f));
        TxtC(H0.difficulty, lp.x + lp.width - 24 - 52, 117, 16, dc);

        const float vx = 36, vy = 158, vw = 412, vh = H - 158 - 40;
        auto content = [&](bool draw) -> float {
            float y = 0; const float x = 46, w = 376;
            y += Wrap(H0.statement, x, vy + y, w, 17, kText, draw, false, nullptr, 1.4f) + 20;
            if (!H0.examples.empty()) {
                if (draw) Txt("EXAMPLES", x, vy + y, 14, kMuted);
                y += 26;
                for (size_t i = 0; i < H0.examples.size(); i++) {
                    const HardExample& ex = H0.examples[i];
                    std::string t = "Input:  " + ex.input + "\nOutput: " + ex.output + (ex.note.empty() ? "" : "\n" + ex.note);
                    float th = Wrap(t, 0, 0, w - 52, 15, kText, false, true);
                    if (draw) {
                        Rectangle b{ x, vy + y, w, th + 24 };
                        Inset(b, 12, Color{ 9, 12, 20, 255 }, kBorder);
                        Wrap(t, b.x + 14, b.y + 12, w - 52, 15, Color{ 190, 255, 220, 255 }, true, true);
                        if (hardRan && i < lastV.tests.size() && lastV.tests[i] != -1)
                            Icon(lastV.tests[i] == 1 ? IC_CHECK : IC_CROSS, b.x + b.width - 22, b.y + 20, 7, lastV.tests[i] == 1 ? kMint : kCoral);
                    }
                    y += th + 24 + 12;
                }
                y += 6;
            }
            if (!H0.constraints.empty()) {
                if (draw) Txt("CONSTRAINTS", x, vy + y, 14, kMuted);
                y += 26;
                for (auto& c : H0.constraints) y += Wrap("-  " + c, x, vy + y, w, 16, kMuted, draw, true, nullptr, 1.4f) + 4;
                y += 14;
            }
            if (hintsShown > 0) {
                if (draw) Txt("HINTS", x, vy + y, 14, kSky);
                y += 26;
                for (int i = 0; i < hintsShown && i < (int)H0.hints.size(); i++) {
                    float th = Wrap(H0.hints[i], 0, 0, w - 52, 16, kText, false);
                    if (draw) {
                        Box({ x, vy + y, w, th + 24 }, 12, Al(kSky, 0.12f));
                        Icon(IC_BULB, x + 22, vy + y + 12 + th / 2, 9, kSky);
                        Wrap(H0.hints[i], x + 44, vy + y + 12, w - 62, 16, kText);
                    }
                    y += th + 24 + 10;
                }
            }
            return y + 20;
            };
        scHard.Begin({ vx, vy, vw, vh }, content(false), dt);
        content(true);
        scHard.End();

        // ---- right: editor, console, buttons
        std::string ext = ".txt", cn = courses[course].name;
        if (cn == "Java") ext = ".java"; else if (cn == "C++") ext = ".cpp"; else if (cn == "Python") ext = ".py";
        else if (cn == "C#") ext = ".cs"; else if (cn == "JavaScript") ext = ".js";
        Rectangle ed{ 476, 94, 600, 420 };
        Card3D(ed, 18, kSurface, kBorder, 6);
        DrawCircleV({ 500, 116 }, 6, kCoral); DrawCircleV({ 520, 116 }, 6, kAmber); DrawCircleV({ 540, 116 }, 6, kMint);
        Txt("Solution" + ext, 566, 106, 16, kMuted);
        Rectangle view{ 488, 134, 576, 368 };
        editor.Update(view, !hardSolved);
        editor.Draw(view, !hardSolved, time);
        if (editor.wantRun && !hardSolved) RunHard();

        Rectangle con{ 476, 532, 600, 92 };
        Card3D(con, 18, kSurface, kBorder, 5);
        if (!hardRan) {
            Txt("CONSOLE", con.x + 22, con.y + 14, 13, kMuted);
            Txt("Press Run (or Ctrl+Enter) to test your solution.", con.x + 22, con.y + 42, 17, kMuted);
        }
        else {
            Color vc = lastV.accepted ? kMint : kCoral;
            Icon(lastV.accepted ? IC_CHECK : IC_CROSS, con.x + 34, con.y + 30, 10, vc);
            Txt(lastV.accepted ? "Accepted" : "Not accepted", con.x + 62, con.y + 14, 22, vc);
            Wrap(Fit(lastV.message, 540, 15), con.x + 62, con.y + 46, 520, 15, kText);
            if (!hardReward.empty()) Txt(Fit(hardReward, 540, 15), con.x + 62, con.y + 66, 15, kAmber);
        }
        if (Button({ 476, 640, 200, 50 }, "Run", kMint, !hardSolved, 21)) RunHard();
        if (Button({ 688, 640, 120, 50 }, "Reset", kSky, !hardSolved, 19, true)) { editor.SetText(H0.starter); hardRan = false; }
        bool canHint = !hardSolved && hintsShown < (int)H0.hints.size() && p.hints > 0;
        std::string hl = hintsShown >= (int)H0.hints.size() ? "No more hints" : "Hint  (" + std::to_string(p.hints) + " left)";
        if (Button({ 820, 640, 256, 50 }, hl, kSky, canHint, 19, true)) { p.hints--; hintsShown++; p.Save(); }
    }

    // ----------------------------------------------------------- RESULT
    void DrawResult(float dt) {
        resT += dt;
        const Topic& T = topics[modTopic];
        float cx = W / 2;
        if (passed && !resBurst && resT > 0.2f) {
            resBurst = true;
            FxBurst(cx, 220, 80); FxBurst(cx - 260, 330, 40); FxBurst(cx + 260, 330, 40);
        }
        float bob = passed ? std::sin(resT * 5) * 4 * std::exp(-resT * 0.8f) : 0;
        DrawAvatar(cx, 200 + bob - 10 * std::fabs(std::sin(resT * 6)) * std::exp(-resT * 1.2f), 1.5f, p.Level(), passed, p.gearHidden, p.outfit);
        float k = EaseOutBack(resT / 0.5f);
        TxtC(passed ? "Module complete!" : "Out of hearts", cx, 318, 44 * k, passed ? kMint : kCoral);
        TxtC(passed ? (firstClear ? "First-clear bonus added." : "Replay finished. Practice pays off.")
            : "Review the topic and try again.", cx, 372, 18, kMuted);

        Rectangle card{ cx - 270, 412, 540, 120 };
        Card3D(card, 22, kSurface, kBorder, 6);
        float cnt = EaseOut(resT / 0.9f);
        struct Stat { int icon; Color c; int val; const char* label; };
        Stat st[3] = { {IC_BOLT, kMint, resXp, "XP earned"}, {IC_COIN, kAmber, resCoins, "Coins"}, {IC_FLAME, kCoral, bestStreak, "Best streak"} };
        for (int i = 0; i < 3; i++) {
            float sx = card.x + card.width / 6 * (1 + 2 * i);
            Icon(st[i].icon, sx, card.y + 26, 11, st[i].c);
            TxtC(std::to_string((int)(st[i].val * (i == 2 ? 1.0f : cnt))), sx, card.y + 44, 34, kText);
            TxtC(st[i].label, sx, card.y + 90, 14, kMuted);
        }
        float yy = 552;
        if (levelUps > 0) {
            float pu = 1 + 0.04f * std::sin(resT * 7);
            TxtC("LEVEL UP!  You reached level " + std::to_string(p.Level()) + "  (+" + std::to_string(levelUps * 10) + " coins)", cx, yy, 24 * pu, kAmber);
            yy += 34;
        }
        if (passed && modIdx < 0 && firstClear && modTopic + 1 < (int)topics.size()) {
            TxtC("Next topic unlocked: " + topics[modTopic + 1].name, cx, yy, 18, T.color);
            yy += 28;
        }
        if (Button({ cx - 110, H - 76, 220, 56 }, "Continue", passed ? kMint : kViolet, true, 23) || IsKeyPressed(KEY_ENTER)) Go(Screen::Map);
    }

    // ----------------------------------------------------------- CHARACTER
    static void DrawGearIcon(int id, float cx, float cy, float s, Color c) {
        switch (id) {
        case 0: Box({ cx - 22 * s, cy + 2 * s, 44 * s, 8 * s }, 3 * s, c); Box({ cx - 13 * s, cy - 18 * s, 26 * s, 22 * s }, 4 * s, c); break;   // hat
        case 1: DrawRing({ cx - 11 * s, cy }, 7 * s, 9 * s, 0, 360, 24, c); DrawRing({ cx + 11 * s, cy }, 7 * s, 9 * s, 0, 360, 24, c);
            DrawLineEx({ cx - 3 * s, cy }, { cx + 3 * s, cy }, 2 * s, c); break;                                                              // glasses
        case 2: Box({ cx - 24 * s, cy - 8 * s, 48 * s, 12 * s }, 6 * s, c); Box({ cx + 6 * s, cy, 10 * s, 24 * s }, 4 * s, c); break;            // scarf
        case 3: Tri({ cx - 24 * s, cy - 16 * s }, { cx + 24 * s, cy - 16 * s }, { cx, cy + 24 * s }, c); break;                                   // cape
        case 4: DrawRectangleRec({ cx - 20 * s, cy + 2 * s, 40 * s, 10 * s }, c);
            for (int i = -1; i <= 1; i++) Tri({ cx + i * 14 * s - 6 * s, cy + 2 * s }, { cx + i * 14 * s + 6 * s, cy + 2 * s }, { cx + i * 14 * s, cy - 16 * s }, c);
            break;                                                                                                                       // crown
        }
    }

    void DrawCharacter() {
        if (TopBar("Character", true, false)) { p.Save(); Go(from); return; }
        float dt = GetFrameTime();
        Vector2 m = Mouse();
        static const char* gearName[5] = { "Hat", "Glasses", "Scarf", "Cape", "Crown" };
        static const int gearLvl[5] = { 2, 3, 5, 7, 10 };
        Color accent = OutfitColor(p.outfit);

        // ---- stage
        Rectangle st{ 40, 96, 440, 584 };
        Card3D(st, 26, kSurface, kBorder, 7);
        float cx = st.x + st.width / 2;
        DrawCircleV({ cx, 330 }, 205, Al(accent, 0.06f));
        DrawCircleV({ cx, 330 }, 145, Al(accent, 0.08f));
        charBounce += dt;
        float jump = -std::fabs(std::sin(charBounce * 9.0f)) * 46.0f * std::exp(-charBounce * 3.2f);
        float bob = std::sin(time * 2.2f) * 4.0f;
        Box({ cx - 105, 548, 210, 18 }, 9, Al(Color{ 0, 0, 0, 255 }, 0.35f));
        Rectangle hit{ cx - 130, 160, 260, 400 };
        if (CheckCollisionPointRec(m, hit) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            charBounce = 0; FxRing(m.x, m.y, accent, 70); FxSparks(m.x, m.y, accent, 12);
        }
        DrawAvatar(cx, 330 + bob + jump, 2.2f, p.Level(), true, (unsigned)p.gearHidden, p.outfit);
        TxtC("Level " + std::to_string(p.Level()), cx, 566, 34, kText);
        TxtC(LevelTitle(p.Level()), cx, 606, 20, kMint);
        int into = p.XpInLevel();
        Bar({ st.x + 50, 640, 340, 14 }, (float)into / 100.0f, kMint);
        TxtC(std::to_string(into) + " / 100 XP   -   " + std::to_string(100 - into) + " XP to level " + std::to_string(p.Level() + 1), cx, 658, 14, kMuted);

        // ---- gear
        Rectangle gp{ 510, 96, 550, 306 };
        Card3D(gp, 22, kSurface, kBorder, 6);
        Txt("Gear", 534, 110, 26, kText);
        Txt("Gear unlocks as you level up. Tap an unlocked item to switch it on or off.", 534, 146, 14, kMuted);
        for (int i = 0; i < 5; i++) {
            bool unlocked = p.Level() >= gearLvl[i];
            bool on = unlocked && !(p.gearHidden & (1 << i));
            Rectangle r{ 524.0f + i * 106, 176, 98, 136 };
            bool hov = unlocked && CheckCollisionPointRec(m, r);
            gearHover[i] = Smooth(gearHover[i], hov ? 1.0f : 0.0f, 16, dt);
            r.y -= 4 * gearHover[i];
            Card3D(r, 16, unlocked ? Mix(kSurface2, accent, on ? 0.14f : 0.0f) : kSurface, on ? accent : kBorder, 4);
            DrawGearIcon(i, r.x + 49, r.y + 48, 1.15f, unlocked ? (on ? accent : kMuted) : kLock);
            if (!unlocked) Icon(IC_LOCK, r.x + 49, r.y + 40, 11, Al(kText, 0.8f));
            TxtC(gearName[i], r.x + 49, r.y + 88, 16, unlocked ? kText : kMuted);
            TxtC(unlocked ? (on ? "On" : "Off") : "Level " + std::to_string(gearLvl[i]), r.x + 49, r.y + 110, 14, unlocked ? (on ? accent : kMuted) : kMuted);
            if (hov && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { p.gearHidden ^= (1 << i); p.Save(); FxRing(m.x, m.y, accent, 40); }
        }
        Txt("Outfit colour", 534, 330, 16, kMuted);
        for (int i = 0; i < 5; i++) {
            Vector2 sc{ 556.0f + i * 56, 372 };
            bool hov = CheckCollisionPointRec(m, { sc.x - 22, sc.y - 22, 44, 44 });
            DrawCircleV({ sc.x, sc.y + 4 }, 18, Mix(OutfitColor(i), kBg, 0.5f));
            DrawCircleV(sc, hov ? 20.0f : 18.0f, OutfitColor(i));
            if (p.outfit == i) DrawRing(sc, 22, 25, 0, 360, 32, kText);
            if (hov && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { p.outfit = i; p.Save(); FxRing(sc.x, sc.y, OutfitColor(i), 50); }
        }

        // ---- stats
        Rectangle sp{ 510, 426, 550, 254 };
        Card3D(sp, 22, kSurface, kBorder, 6);
        Txt("Stats", 534, 440, 24, kText);
        int modules = 0; for (int c = 0; c < (int)courses.size(); c++) modules += CourseDone(c);
        int boss = 0; for (auto& k : p.done) if (k.size() >= 4 && k.compare(k.size() - 4, 4, "hard") == 0) boss++;
        std::string next = "All unlocked";
        for (int i = 0; i < 5; i++) if (p.Level() < gearLvl[i]) { next = std::string(gearName[i]) + " (Lv " + std::to_string(gearLvl[i]) + ")"; break; }
        struct Tile { const char* label; std::string value; Color c; float fs; };
        Tile tiles[8] = {
            {"Level", std::to_string(p.Level()), kMint, 28}, {"Total XP", std::to_string(p.xp), kSky, 28},
            {"Coins", std::to_string(p.coins), kAmber, 28}, {"Hints", std::to_string(p.hints), kSky, 28},
            {"Coin boost", std::to_string((int)std::lround(p.CoinMult() * 100)) + "%", kAmber, 28}, {"Modules done", std::to_string(modules), kMint, 28},
            {"Boss problems", std::to_string(boss), kCoral, 28}, {"Next unlock", next, kViolet, 15},
        };
        for (int i = 0; i < 8; i++) {
            Rectangle r{ 530.0f + (i % 4) * 126, 484.0f + (i / 4) * 90, 116, 78 };
            Box({ r.x, r.y + 3, r.width, r.height }, 14, Al(Color{ 0, 0, 0, 255 }, 0.3f));
            Box(r, 14, kSurface2);
            Box({ r.x, r.y + 14, 4, r.height - 28 }, 2, tiles[i].c);
            Txt(tiles[i].label, r.x + 14, r.y + 10, 13, kMuted);
            Wrap(tiles[i].value, r.x + 14, r.y + (tiles[i].fs > 20 ? 32 : 36), r.width - 22, tiles[i].fs, tiles[i].c, true, false, nullptr, 1.15f);
        }
    }

    // ----------------------------------------------------------- SHOP
    void DrawShop() {
        if (TopBar("Shop", true, false)) { p.Save(); Go(from); return; }
        scShop.Begin({ 0, 74, W, H - 74 }, std::max(H - 74, 110.0f + 3 * 148 + 70 - 74), GetFrameTime());
        struct Row { const char* name; std::string desc; int cost; bool enabled; int icon; Color c; };
        int boostCost = 60 * (p.boost + 1);
        int boostReq = 3 + 2 * p.boost;
        Row rows[3] = {
            {"Hint token", "Removes two wrong answers, or reveals a code hint.  You own " + std::to_string(p.hints) + ".", 15, true, IC_BULB, kSky},
            {"Extra heart", "Start your next lesson with 4 hearts.  Ready: " + std::to_string(p.extraLife) + ".", 25, true, IC_FLAME, kCoral},
            {"Coin boost", "+25% coins on every answer.  Needs level " + std::to_string(boostReq) + ".  Owned: " + std::to_string(p.boost) + " / 4.",
                           boostCost, p.boost < 4 && p.Level() >= boostReq, IC_COIN, kAmber},
        };
        Vector2 m = Mouse();
        for (int i = 0; i < 3; i++) {
            Rectangle r{ 150, 110.0f + i * 148, 800, 128 };
            bool hov = CheckCollisionPointRec(m, r);
            cardHover[i] = Smooth(cardHover[i], hov ? 1.0f : 0.0f, 14, GetFrameTime());
            r.y -= 4 * cardHover[i];
            Card3D(r, 20, Mix(kSurface, kSurface2, cardHover[i]), Mix(kBorder, rows[i].c, cardHover[i]), 6);
            DrawCircleV({ r.x + 64, r.y + 64 }, 36, Al(rows[i].c, 0.18f));
            Icon(rows[i].icon, r.x + 64, r.y + 64, 20, rows[i].c);
            Txt(rows[i].name, r.x + 120, r.y + 24, 27, kText);
            Wrap(rows[i].desc, r.x + 120, r.y + 64, 400, 16, kMuted);
            bool afford = p.coins >= rows[i].cost && rows[i].enabled;
            if (Button({ r.x + r.width - 210, r.y + 36, 180, 56 }, "Buy  " + std::to_string(rows[i].cost), kAmber, afford, 21)) {
                p.coins -= rows[i].cost;
                if (i == 0) p.hints++; else if (i == 1) p.extraLife++; else p.boost++;
                p.Save(); Toast("Purchased!");
                FxBurst(r.x + r.width - 120, r.y + 64, 30);
            }
        }
        Txt("Earn coins from correct answers, quests and level-ups.", 150, H - 52, 16, kMuted);
        scShop.End();
    }

    // ----------------------------------------------------------- QUESTS
    void DrawQuests() {
        if (TopBar("Quests", true, false)) { p.Save(); Go(from); return; }
        scQuest.Begin({ 0, 74, W, H - 74 }, std::max(H - 74, 106.0f + (float)p.quests.size() * 134 + 30 - 74), GetFrameTime());
        Vector2 m = Mouse();
        static const Color qc[4] = { kMint, kViolet, kCoral, kSky };
        for (int i = 0; i < (int)p.quests.size(); i++) {
            const Quest& q = p.quests[i];
            Rectangle r{ 150, 106.0f + i * 134, 800, 118 };
            bool hov = CheckCollisionPointRec(m, r);
            cardHover[i] = Smooth(cardHover[i], hov ? 1.0f : 0.0f, 14, GetFrameTime());
            r.y -= 3 * cardHover[i];
            bool can = p.CanClaim(i);
            Color col = qc[q.kind % 4];
            Color edge = can ? Mix(kBorder, kMint, 0.5f + 0.5f * std::sin(time * 4)) : Mix(kBorder, col, cardHover[i]);
            Card3D(r, 20, kSurface, edge, 6);
            DrawCircleV({ r.x + 58, r.y + 59 }, 30, Al(col, 0.18f));
            Icon(can ? IC_STAR : IC_BOLT, r.x + 58, r.y + 59, 15, col);
            Txt(Fit(Player::QuestText(q), 480, 22), r.x + 110, r.y + 20, 22, kText);
            float frac = std::min(1.0f, (float)q.progress / (float)q.target);
            Bar({ r.x + 110, r.y + 62, 400, 14 }, frac, col);
            Txt(std::to_string(std::min(q.progress, q.target)) + " / " + std::to_string(q.target), r.x + 524, r.y + 58, 17, kMuted);
            Txt("Reward: " + std::to_string(q.reward) + " coins + 1 hint", r.x + 110, r.y + 86, 14, kAmber);
            if (Button({ r.x + r.width - 190, r.y + 30, 160, 56 }, "Claim", kMint, can, 21)) {
                p.Claim(i); Toast("Reward claimed!");
                FxBurst(r.x + r.width - 110, r.y + 58, 40);
            }
        }
        scQuest.End();
    }
};