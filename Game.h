#pragma once
// Game.h - owns the Player + content and runs every screen (dark theme, animated).
#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>
#include "raylib.h"
#include "Content.h"
#include "Player.h"
#include "UI.h"

enum class Screen { Menu, Map, Lesson, Result, Shop, Quests };

class Game {
public:
    Game() {
        courses = BuildCourses();
        topics = courses[0].topics;
        dispXp = (float)p.xp; dispCoins = (float)p.coins;
    }
    void SaveNow() const { p.Save(); }

    void Frame() {
        float dt = GetFrameTime(); if (dt > 0.05f) dt = 0.05f;
        time += dt;
        if (toastT > 0) toastT -= dt;
        fade = std::max(0.0f, fade - dt * 4.0f);
        dispXp = Smooth(dispXp, (float)p.xp, 7, dt);
        dispCoins = Smooth(dispCoins, (float)p.coins, 9, dt);

        ClearBackground(kBg);
        if (IsKeyPressed(KEY_ESCAPE)) {
            p.Save();
            if (screen == Screen::Map) Go(Screen::Menu);
            else if (screen == Screen::Shop || screen == Screen::Quests) Go(from);
            else if (screen == Screen::Lesson || screen == Screen::Result) Go(Screen::Map);
        }
        switch (screen) {
        case Screen::Menu:   DrawMenu(); break;
        case Screen::Map:    DrawMap(); break;
        case Screen::Lesson: DrawLesson(dt); break;
        case Screen::Result: DrawResult(dt); break;
        case Screen::Shop:   DrawShop(); break;
        case Screen::Quests: DrawQuests(); break;
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
        if (fade > 0.001f) DrawRectangle(0, 0, (int)W, (int)H, Al(kBg, fade));
    }

private:
    static constexpr float W = 1100, H = 700;

    // ----------------------------------------------------------- state
    Player p;
    std::vector<Course> courses;
    std::vector<Topic> topics;
    int course = 0, selTopic = 0;
    Screen screen = Screen::Menu, from = Screen::Menu;
    float time = 0, fade = 0, toastT = 0;
    std::string toast;
    float dispXp = 0, dispCoins = 0;

    // menu / map animation
    float cardHover[8] = { 0 }, rowHover[40] = { 0 }, nodeHover[40] = { 0 };
    float listScroll = 0, listTarget = 0;

    // lesson
    struct Item { bool isCode = false; int qi = 0; std::vector<int> order; };
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
        for (auto& k : p.done) if (k.compare(0, pre.size(), pre) == 0) n++;
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
        if (mod->hasCode) { Item c; c.isCode = true; items.push_back(c); }
        cur = 0; lives = 3 + p.extraLife; p.extraLife = 0;
        correct = streak = bestStreak = 0;
        resXp = resCoins = levelUps = 0; progShown = 0; hurtT = 0;
        ResetItem();
        Go(Screen::Lesson);
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
        resT = 0; resBurst = false;
        Go(Screen::Result);
    }

    // ----------------------------------------------------------- shared top bar
    void Pill(Rectangle r, int icon, Color c, const std::string& text) {
        Box(r, 14, kSurface2);
        Icon(icon, r.x + 24, r.y + 22, 12, c);
        Txt(text, r.x + 46, r.y + 10, 22, kText);
    }
    // returns true when the back button was clicked
    bool TopBar(const std::string& title, bool back, bool nav) {
        DrawRectangle(0, 0, (int)W, 72, kSurface);
        DrawRectangle(0, 72, (int)W, 2, kBorder);
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
        Box(lp, 14, kSurface2);
        DrawCircleV({ lp.x + 24, lp.y + 22 }, 17, kMint);
        TxtC(std::to_string(p.Level()), lp.x + 24, lp.y + 10, 20, kBg);
        float inLvl = dispXp - 100.0f * std::floor(dispXp / 100.0f);
        Txt("XP " + std::to_string((int)inLvl) + " / 100", lp.x + 50, lp.y + 5, 13, kMuted);
        Bar({ lp.x + 50, lp.y + 27, 148, 9 }, inLvl / 100.0f, kMint);
        return backClicked;
    }

    // ----------------------------------------------------------- MAIN MENU
    void DrawMenu() {
        TopBar("JavaQuest", false, true);
        Txt("Choose your language", 35, 96, 36, kText);
        Txt("Short lessons, quick quizzes, real progress. Your XP and coins carry across every course.", 35, 144, 17, kMuted);
        static const char* glyph[] = { "J", "C++", "Py", "C#", "JS" };
        Vector2 m = GetMousePosition();
        float dt = GetFrameTime();
        for (int i = 0; i < (int)courses.size() && i < 8; i++) {
            const Course& c = courses[i];
            Rectangle base{ 35.0f + (i % 3) * 350, 190.0f + (i / 3) * 235, 330, 215 };
            bool hover = CheckCollisionPointRec(m, base);
            cardHover[i] = Smooth(cardHover[i], hover ? 1.0f : 0.0f, 16, dt);
            float h = cardHover[i];
            Rectangle r{ base.x, base.y - 6 * h, base.width, base.height };
            Shadow(r, 22, 8, 0.4f);
            BoxB(r, 22, Mix(kSurface, kSurface2, h), Mix(kBorder, c.color, h), 2);

            Rectangle ic{ r.x + 22, r.y + 22, 64, 64 };
            Box(ic, 18, c.color);
            std::string g = i < 5 ? glyph[i] : c.name.substr(0, 1);
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
            if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                FxRing(m.x, m.y, c.color, 70);
                course = i; topics = c.topics; selTopic = 0; listScroll = listTarget = 0;
                Go(Screen::Map);
            }
        }
    }

    // ----------------------------------------------------------- MAP
    void DrawMap() {
        if (TopBar(courses[course].name, true, true)) { p.Save(); Go(Screen::Menu); return; }
        float dt = GetFrameTime();
        Vector2 m = GetMousePosition();

        // sidebar
        DrawRectangle(0, 74, 310, (int)H - 74, kSurface);
        DrawRectangle(310, 74, 2, (int)H - 74, kBorder);
        Txt("TOPICS", 26, 90, 14, kMuted);
        const float listTop = 116, listH = H - 116 - 142;
        int n = (int)topics.size();
        float contentH = n * 66.0f;
        float maxScroll = std::max(0.0f, contentH - listH);
        bool inSide = m.x < 310 && m.y > listTop && m.y < listTop + listH;
        if (inSide) listTarget -= GetMouseWheelMove() * 66;
        listTarget = std::max(0.0f, std::min(maxScroll, listTarget));
        listScroll = Smooth(listScroll, listTarget, 14, dt);

        BeginScissorMode(0, (int)listTop, 310, (int)listH);
        for (int t = 0; t < n; t++) {
            float ry = listTop + t * 66 - listScroll;
            if (ry + 58 < listTop || ry > listTop + listH) continue;
            Rectangle rr{ 14, ry, 282, 58 };
            bool open = TopicOpen(t), sel = t == selTopic;
            bool hov = inSide && CheckCollisionPointRec(m, rr);
            if (t < 40) rowHover[t] = Smooth(rowHover[t], hov ? 1.0f : 0.0f, 16, dt);
            Color tc = topics[t].color;
            if (sel) BoxB(rr, 14, Mix(kSurface2, tc, 0.2f), tc, 2);
            else Box(rr, 14, Mix(kSurface, kSurface2, t < 40 ? rowHover[t] : 0));
            float cy = rr.y + 29;
            DrawCircleV({ rr.x + 32, cy }, 18, open ? tc : kLock);
            if (open) TxtC(std::to_string(t + 1), rr.x + 32, cy - 11, 20, OnColor(tc));
            else Icon(IC_LOCK, rr.x + 32, cy - 2, 8, kText);
            Txt(Fit(topics[t].name, 282 - 62 - 38, 17), rr.x + 62, rr.y + 8, 17, open ? kText : kMuted);
            int d, tot; TopicProgress(t, d, tot);
            std::string sub = topics[t].modules.empty() ? "Coming soon" : (open ? std::to_string(d) + " / " + std::to_string(tot) + " done" : "Locked");
            Txt(sub, rr.x + 62, rr.y + 33, 13, kMuted);
            if (tot > 0 && d == tot) Icon(IC_CHECK, rr.x + rr.width - 24, cy, 8, kMint);
            if (hov && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { selTopic = t; FxRing(m.x, m.y, tc, 40); }
        }
        EndScissorMode();
        if (maxScroll > 0) {
            float th = std::max(30.0f, listH * listH / contentH);
            float ty = listTop + (listH - th) * (listScroll / maxScroll);
            Box({ 302, ty, 4, th }, 2, kBorder);
        }

        // character card
        Rectangle ac{ 16, H - 134, 278, 118 };
        BoxB(ac, 18, kSurface2, kBorder, 2);
        DrawAvatar(66, ac.y + 58 + std::sin(time * 2.2f) * 2.5f, 0.6f, p.Level());
        Txt("Level " + std::to_string(p.Level()), 122, ac.y + 20, 22, kText);
        Txt(LevelTitle(p.Level()), 122, ac.y + 50, 16, kMint);
        Txt(std::to_string(CourseDone(course)) + " modules cleared", 122, ac.y + 78, 14, kMuted);

        // main area
        const Topic& T = topics[selTopic];
        const float x0 = 350, cw = 716;
        Txt(Fit(T.name, cw, 34), x0, 92, 34, T.color);
        float bh = Wrap(T.blurb, x0, 138, cw, 17, kMuted);

        if (T.modules.empty()) {
            Rectangle r{ x0 + 100, 280, cw - 200, 190 };
            BoxB(r, 22, kSurface, kBorder, 2);
            TxtC("Content coming soon", r.x + r.width / 2, r.y + 50, 30, kText);
            TxtC("Add modules for this topic in Content.h", r.x + r.width / 2, r.y + 100, 17, kMuted);
            return;
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
            return;
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
            float s = 1 + 0.08f * hv - (down ? 0.12f : 0.0f);
            if (i == firstOpen) {                                   // pulsing "next up" glow
                float pu = 0.5f + 0.5f * std::sin(time * 3.2f);
                DrawCircleV(c, (rad + 10 + 6 * pu) * s, Al(T.color, 0.18f + 0.12f * pu));
            }
            Color base = done ? kMint : (open ? T.color : kLock);
            DrawCircleV({ c.x, c.y + 6 }, rad * s, Mix(base, kBg, 0.55f));
            DrawCircleV(c, rad * s, base);
            DrawCircleV({ c.x - rad * 0.25f * s, c.y - rad * 0.3f * s }, rad * 0.45f * s, Al(Color{ 255, 255, 255, 255 }, 0.12f));
            if (done) Icon(IC_CHECK, c.x, c.y, 15 * s, kBg);
            else if (!open) Icon(IC_LOCK, c.x, c.y - 2, 13 * s, kText);
            else if (isTest) Icon(IC_STAR, c.x, c.y, 20 * s, OnColor(base));
            else TxtC(std::to_string(i + 1), c.x, c.y - 15 * s, 30 * s, OnColor(base));
            const std::string& title = isTest ? T.test.title : T.modules[i].title;
            TxtC(Fit(title, stepX - 8, 14), c.x, c.y + rad + 14, 14, open ? kText : kMuted);
            if (hover && open && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { FxRing(c.x, c.y, T.color, 80); StartLesson(selTopic, mm); }
        }
        Icon(IC_FLAME, x0 + 8, H - 26, 8, kAmber);
        Txt("3 correct in a row starts a COMBO: double coins!", x0 + 24, H - 36, 15, kMuted);
    }

    // ----------------------------------------------------------- LESSON
    void DrawLesson(float dt) {
        if (answered) answerT += dt; else answerT = 0;
        itemT += dt; if (hurtT > 0) hurtT -= dt;
        const Topic& T = topics[modTopic];
        Item& it = items[cur];
        Vector2 m = GetMousePosition();

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
            (it.isCode ? "Code challenge" : "Question " + std::to_string(cur + 1) + " of " + std::to_string(items.size()));
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
                if (!hid) Shadow(r, 16, 4, 0.3f * k);
                BoxB(r, 16, Al(fill, k), Al(edge, k), 2);
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
        else {
            const CodeTask& c = mod->code;
            float ph = Wrap(c.prompt, 150, contentTop, 800, 28, Al(kText, enter), true, false, nullptr, 1.3f);
            Rectangle box{ 150, contentTop + ph + 26, 800, 74 };
            Color edge = answered ? (wasRight ? kMint : kCoral) : Mix(kBorder, kViolet, 0.5f + 0.5f * std::sin(time * 3));
            if (answered && !wasRight) box.x += std::sin(answerT * 48) * 12 * std::exp(-answerT * 7);
            BoxB(box, 16, Color{ 9, 12, 20, 255 }, edge, 2);
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
            DrawRectangle(0, (int)bar.y, (int)W, (int)bar.height, kSurface);
            DrawRectangle(0, (int)bar.y, (int)W, 2, kBorder);
            if (Button({ 150, H - 84, 170, 56 }, "Hint  (" + std::to_string(p.hints) + ")", kSky, p.hints > 0, 21, true)) UseHint();
            if (it.isCode) {
                bool ok = !input.empty();
                if (Button({ W - 150 - 200, H - 84, 200, 56 }, "Check", kMint, ok, 23) || (ok && IsKeyPressed(KEY_ENTER))) SubmitCode();
            }
        }
        else {
            float panelH = 150, k = EaseOut(answerT / 0.28f);
            float y = H - panelH * k;
            Color tone = wasRight ? kMint : kCoral;
            DrawRectangle(0, (int)y, (int)W, (int)panelH + 4, Mix(kBg, tone, 0.14f));
            DrawRectangle(0, (int)y, (int)W, 3, tone);
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
        DrawAvatar(cx, 200 + bob - 10 * std::fabs(std::sin(resT * 6)) * std::exp(-resT * 1.2f), 1.5f, p.Level(), passed);
        float k = EaseOutBack(resT / 0.5f);
        TxtC(passed ? "Module complete!" : "Out of hearts", cx, 318, 44 * k, passed ? kMint : kCoral);
        TxtC(passed ? (firstClear ? "First-clear bonus added." : "Replay finished. Practice pays off.")
            : "Review the topic and try again.", cx, 372, 18, kMuted);

        Rectangle card{ cx - 270, 412, 540, 120 };
        BoxB(card, 22, kSurface, kBorder, 2);
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

    // ----------------------------------------------------------- SHOP
    void DrawShop() {
        if (TopBar("Shop", true, false)) { p.Save(); Go(from); return; }
        struct Row { const char* name; std::string desc; int cost; bool enabled; int icon; Color c; };
        int boostCost = 60 * (p.boost + 1);
        int boostReq = 3 + 2 * p.boost;
        Row rows[3] = {
            {"Hint token", "Removes two wrong answers, or reveals a code hint.  You own " + std::to_string(p.hints) + ".", 15, true, IC_BULB, kSky},
            {"Extra heart", "Start your next lesson with 4 hearts.  Ready: " + std::to_string(p.extraLife) + ".", 25, true, IC_FLAME, kCoral},
            {"Coin boost", "+25% coins on every answer.  Needs level " + std::to_string(boostReq) + ".  Owned: " + std::to_string(p.boost) + " / 4.",
                           boostCost, p.boost < 4 && p.Level() >= boostReq, IC_COIN, kAmber},
        };
        Vector2 m = GetMousePosition();
        for (int i = 0; i < 3; i++) {
            Rectangle r{ 150, 110.0f + i * 148, 800, 128 };
            bool hov = CheckCollisionPointRec(m, r);
            cardHover[i] = Smooth(cardHover[i], hov ? 1.0f : 0.0f, 14, GetFrameTime());
            r.y -= 4 * cardHover[i];
            Shadow(r, 20, 6, 0.35f);
            BoxB(r, 20, Mix(kSurface, kSurface2, cardHover[i]), Mix(kBorder, rows[i].c, cardHover[i]), 2);
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
    }

    // ----------------------------------------------------------- QUESTS
    void DrawQuests() {
        if (TopBar("Quests", true, false)) { p.Save(); Go(from); return; }
        Vector2 m = GetMousePosition();
        static const Color qc[4] = { kMint, kViolet, kCoral, kSky };
        for (int i = 0; i < (int)p.quests.size(); i++) {
            const Quest& q = p.quests[i];
            Rectangle r{ 150, 106.0f + i * 134, 800, 118 };
            bool hov = CheckCollisionPointRec(m, r);
            cardHover[i] = Smooth(cardHover[i], hov ? 1.0f : 0.0f, 14, GetFrameTime());
            r.y -= 3 * cardHover[i];
            bool can = p.CanClaim(i);
            Color col = qc[q.kind % 4];
            Shadow(r, 20, 6, 0.35f);
            Color edge = can ? Mix(kBorder, kMint, 0.5f + 0.5f * std::sin(time * 4)) : Mix(kBorder, col, cardHover[i]);
            BoxB(r, 20, kSurface, edge, 2);
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
    }
};