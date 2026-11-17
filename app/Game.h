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
#include "content/CourseCatalog.h"
#include "player/Player.h"
#include "ui/UI.h"
#include "avatar/AvatarRenderer.h"
#include "hard/Hard.h"
#include "practice/Practice.h"
#include "bonus/ProgrammerMillionaireScreen.h"
#include "bonus/RobotRoutinesScreen.h"
#include "bonus/DebugVaultScreen.h"
#include "engagement/Rewards.h"
#include "engagement/TimedScreens.h"
#include "content/cpp/CppCourse.h"
#include "app/LessonRetryPolicy.h"
#include "labs/LabRunner.h"
#include "labs/LabDrafts.h"

// Optional easter egg: if ee/mode.h exists it is compiled in; delete the folder and the game builds without it.
#if defined(__has_include)
#  if __has_include("ee/mode.h")
#    include "ee/mode.h"
#    define JQ_SH2 1
#  endif
#endif

enum class Screen { Menu, Learn, Labs, LabTask, Bonus, Games, Map, Lesson, Result, Shop, Quests, Hard, Character, Practice, Millionaire, RobotRoutines, DebugVault, RapidReview, MatchMadness };

class Game {
public:
    Game() {
        BuildHard();
        courses = BuildCourses();
        InstallCppCourse(courses);        // the full C++ course replaces the outline from Content.h
        topics = courses[0].topics;
        dispXp = (float)p.xp; dispCoins = (float)p.coins;
        labDrafts = questlabs::LoadDrafts();
    }
    void SaveNow() const { if (screen != Screen::Practice) { p.Save(); rewards.Save(); }
        auto drafts=labDrafts; if(screen==Screen::LabTask && labIndex>=0 && labIndex<(int)drafts.size()) drafts[labIndex]=labEditor.Text();
        questlabs::SaveDrafts(drafts); }

    void Frame() {
        float dt = GetFrameTime(); if (dt > 0.05f) dt = 0.05f;
        rewards.Refresh();
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
            if (screen != Screen::Practice) p.Save();
            if (screen == Screen::Map) Go(mapFrom);
            else if (screen == Screen::Bonus || screen == Screen::Games || screen == Screen::Learn) Go(Screen::Menu);
            else if (screen == Screen::Labs) Go(Screen::Learn);
            else if (screen == Screen::LabTask) LeaveLab();
            else if (screen == Screen::Shop || screen == Screen::Quests || screen == Screen::Character) Go(from);
            else if (screen == Screen::Lesson || screen == Screen::Result) Go(Screen::Map);
            else if (screen == Screen::Hard) EnterResult();
            else if (screen == Screen::Practice && !practice.BackToChooser()) Go(practiceFrom);
            else if (screen == Screen::Millionaire || screen == Screen::RobotRoutines || screen == Screen::DebugVault || screen == Screen::RapidReview || screen == Screen::MatchMadness) Go(Screen::Games);
        }
        switch (screen) {
        case Screen::Menu:   DrawMenu(); break;
        case Screen::Learn:  DrawLearn(); break;
        case Screen::Labs:   DrawLabs(); break;
        case Screen::LabTask: DrawLabTask(dt); break;
        case Screen::Bonus:  DrawBonus(); break;
        case Screen::Games:  DrawGames(); break;
        case Screen::Map:    DrawMap(); break;
        case Screen::Lesson: DrawLesson(dt); break;
        case Screen::Result: DrawResult(dt); break;
        case Screen::Shop:   DrawShop(); break;
        case Screen::Quests: DrawQuests(); break;
        case Screen::Hard:   DrawHard(dt); break;
        case Screen::Character: DrawCharacter(); break;
        case Screen::Practice:
            if (practice.InChooser()) {
                if (TopBar("Practice C++", true, true)) { Go(practiceFrom); break; }
                // TopBar can also navigate to Shop/Quests/Character.
                if (screen != Screen::Practice) break;
            }
            if (practice.Draw(dt) == PracticeAction::Back) Go(practiceFrom);
            break;
        case Screen::Millionaire: DrawMillionaire(); break;
        case Screen::RobotRoutines: DrawRobotRoutines(dt); break;
        case Screen::DebugVault: DrawDebugVault(); break;
        case Screen::RapidReview: DrawRapidReview(dt); break;
        case Screen::MatchMadness: DrawMatchMadness(dt); break;
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
    Screen practiceFrom = Screen::Menu;
    EndlessPractice practice;
    ProgrammerMillionaireScreen millionaire;
    RobotRoutinesScreen robotRoutines;
    DebugVaultScreen debugVault;
    engagement::Rewards rewards;
    engagement::RapidReviewScreen rapidReview;
    engagement::MatchMadnessScreen matchMadness;
    int labGroup = 0, labIndex = 0, labHints = 0;
    bool labChecked = false, labSolved = false;
    questlabs::Result labResult;
    CodeEditor labEditor;
    std::string labResultNote;
    std::vector<std::string> labDrafts = std::vector<std::string>(questlabs::All().size());
    bool editingFriendCode = false;
    std::string incomingFriendCode;
    std::string friendStatus;
    float time = 0, fade = 0, toastT = 0;
    std::string toast;
    float dispXp = 0, dispCoins = 0;

    // menu / map animation
    float cardHover[8] = { 0 }, rowHover[40] = { 0 }, nodeHover[40] = { 0 };
    Scroller scMenu, scBonus, scMap, scSide, scShop, scQuest, scCards, scHard, scLabs, scLabInfo;
    // hard mode
    const HardProblem* hard = nullptr;
    CodeEditor editor;
    TextJudge judge;
    Verdict lastV;
    bool hardRan = false, hardSolved = false, hardFromMap = false;
    int hintsShown = 0;
    std::string hardReward;
    float hardKnob = 0;
    float charBounce = 99, avCellHover[12] = { 0 };
    int avTab = 0;
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
    int resXp = 0, resCoins = 0, levelUps = 0, resLevelCoins = 0;
    bool passed = false, firstClear = false;
    float itemT = 0, answerT = 0, hurtT = 0, progShown = 0, optHover[8] = { 0 };
    Vector2 fxAt{ 550, 300 };
    // result
    float resT = 0; bool resBurst = false;
    // lesson cards (shown before a module's questions)
    bool inCards = false; int cardIdx = 0, selPart = -1; float cardT = 0;
    bool reviewing = false;                        // lesson cards re-opened from inside a quiz
    bool recallRevealed = false;
    bool flashRated = false;
    bool flashRevisit = false;
    std::vector<int> flashMissed;
    size_t flashRevisitAt = 0;                   // prompt answer remains hidden until requested

    void Go(Screen s) {
#ifdef JQ_SH2
        if (screen == Screen::Bonus && s != Screen::Bonus) konami.Reset();
#endif
        if (s == Screen::Games || s == Screen::Bonus) scBonus.Reset();
        screen = s; fade = 1.0f;
    }
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
        hardFromMap = false;
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
        if (course == CppIndex() && m < 0) { // Mixed topic tests: avoid always ending with two obvious review questions.
            for (int k = (int)items.size() - 1; k > 0; --k)
                std::swap(items[k], items[GetRandomValue(0, k)]);
        }
        if (mod->hasCode) { Item c; c.isCode = true; BuildCodeItem(c); items.push_back(c); }
        cur = 0; lives = 3 + p.extraLife; p.extraLife = 0;
        correct = streak = bestStreak = 0;
        resXp = resCoins = levelUps = resLevelCoins = 0; progShown = 0; hurtT = 0;
        ResetItem();
        inCards = !mod->lesson.empty(); reviewing = false; cardIdx = 0; selPart = -1; cardT = 0; recallRevealed = false; flashRated = false; flashRevisit = false; flashMissed.clear(); flashRevisitAt = 0; scCards.Reset();
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
        std::string mid = answered && wasRight ? it.mid : "___";
        Color midCol = answered && wasRight ? kMint : (answered ? kCoral : kSky);
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
                Color tone = (wasRight && isC) ? kMint : (i == picked ? kCoral : kLock);
                bool highlighted = (wasRight && isC) || i == picked;
                BoxB(r, 16, Mix(kSurface, tone, highlighted ? 0.16f : 0.0f), tone, 2);
                TxtC(Fit(it.choices[i], 216, 20), r.x + 120, r.y + 17, 20, highlighted ? tone : kMuted);
            }
        }
        if (showCodeHint) HintPanel(box.x, yy + 110, box.width);
    }

    // ----------------------------------------------------------- lesson cards
    void DrawCards(float dt) {
        cardT += dt;
        const std::vector<LessonCard>& cards = mod->lesson;
        const LessonCard& c = cards[cardIdx];
        bool last = flashRevisit ? flashRevisitAt + 1 >= flashMissed.size() : cardIdx + 1 >= (int)cards.size();
        bool replay = p.done.count(Key(modTopic, modIdx)) > 0;
        float enter = EaseOut(cardT / 0.35f);
        Vector2 m = Mouse();
        static const Color pal[5] = { kSky, kMint, kAmber, kViolet, kCoral };

        if (Button({ 24, 20, 48, 48 }, "", kSurface2, true, 20, true)) {
            if (reviewing) { inCards = false; reviewing = false; itemT = 1; return; }
            p.Save(); Go(Screen::Map); return;
        }
        Icon(IC_CROSS, 48, 44, 9, kMuted);
        Txt(reviewing ? "REVIEW" : "LEARN FIRST", 92, 24, 13, kSky);
        Txt(Fit(modIdx < 0 ? topics[modTopic].test.title : topics[modTopic].modules[modIdx].title, 600, 22), 92, 40, 22, kText);
        if ((replay || reviewing) && Button({ W - 24 - 170, 20, 170, 44 }, reviewing ? "Back to quiz" : "Skip", kSky, true, 19, true)) {
            itemT = reviewing ? 1.0f : 0.0f; inCards = false; reviewing = false; return;
        }

        float th0 = Wrap(c.text, 150, 0, 800, 21, kText, false, false, nullptr, 1.4f);
        const std::string flashFront = c.recallPrompt.empty() ? "Without looking above, explain: " + c.title : c.recallPrompt;
        const std::string flashBack = c.recallAnswer.empty() ? c.text : c.recallAnswer;
        float promptH = 36 + Wrap(flashFront, 0, 0, 750, 19, kText, false) + 74;
        float answerH = recallRevealed ? 30 + Wrap(flashBack, 0, 0, 744, 18, kText, false) + 85 : 0.0f;
        float chc = std::max(H - 74 - 110, 112 + 58 + th0 + 26 +
            (c.code.empty() ? 0.0f : 74 + 18 + (c.parts.empty() ? 0.0f : 130)) +
            promptH + answerH + 48 - 74);
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
            y = y + 74 + 18 + (c.parts.empty() ? 0.0f : 130);
        }

        // Flashcard: retrieve from memory, flip to reveal, then self-rate.
        // Cards marked "Review again" return once at the end of the lesson.
        {
            const float pHeight = Wrap(flashFront, 0, 0, 744, 19, kText, false);
            Rectangle flashBox{150, y + 8, 800, 36 + pHeight + 18};
            Box(flashBox, 14, Al(kSky, 0.12f));
            Txt(flashRevisit ? "FLASHCARD - SECOND TRY" : "FLASHCARD - RECALL WITHOUT LOOKING ABOVE", flashBox.x + 16, flashBox.y + 10, 13, kSky);
            Wrap(flashFront, flashBox.x + 16, flashBox.y + 36, 744, 19, kText);
            y += flashBox.height + 18;
            if (recallRevealed) {
                const float aHeight = Wrap(flashBack, 0, 0, 744, 18, kText, false);
                Box({150, y, 800, aHeight + 28}, 14, Al(kMint, 0.10f));
                Wrap(flashBack, 166, y + 13, 744, 18, kText);
                y += aHeight + 43;
                if (!flashRated) {
                    if (Button({150, y, 246, 47}, "I remembered", kMint, true, 18)) flashRated = true;
                    if (Button({414, y, 246, 47}, "Review again", kAmber, true, 18, true)) {
                        flashRated = true;
                        if (!flashRevisit && std::find(flashMissed.begin(), flashMissed.end(), cardIdx) == flashMissed.end())
                            flashMissed.push_back(cardIdx);
                    }
                } else Txt("Card rated. Continue when you're ready.", 150, y + 10, 16, kMuted);
            } else if (Button({150, y, 230, 47}, "Flip flashcard", kViolet, true, 18, true)) {
                recallRevealed = true;
            }
        }

        scCards.End();
        DrawRectangle(-1500, (int)(H - 110), 4100, 110, kSurface);
        DrawRectangle(-1500, (int)(H - 110), 4100, 2, kBorder);
        int n = flashRevisit ? (int)flashMissed.size() : (int)cards.size();
        int dot = flashRevisit ? (int)flashRevisitAt : cardIdx;
        for (int i = 0; i < n; i++)
            DrawCircleV({ W / 2 - (n - 1) * 14.0f + i * 28, H - 56 }, i == dot ? 7.0f : 5.0f, i == dot ? kSky : kBorder);
        if (!flashRevisit && cardIdx > 0 && Button({ 150, H - 84, 150, 56 }, "Back", kSky, true, 21, true)) {
            cardIdx--; cardT = 0; selPart = -1; recallRevealed = false; flashRated = false; scCards.Reset();
        }
        std::string nextLabel = last ? (!flashRevisit && !flashMissed.empty() ? "Review missed" : (reviewing ? "Back to quiz" : "Start quiz")) : "Next";
        if (Button({ W - 150 - 200, H - 84, 200, 56 }, nextLabel, kMint, flashRated, 22)
            || (flashRated && IsKeyPressed(KEY_ENTER))) {
            if (last) {
                if (!flashRevisit && !flashMissed.empty()) {
                    flashRevisit = true; flashRevisitAt = 0; cardIdx = flashMissed[0];
                } else { itemT = reviewing ? 1.0f : 0.0f; inCards = false; reviewing = false; }
            } else {
                if (flashRevisit) { ++flashRevisitAt; cardIdx = flashMissed[flashRevisitAt]; }
                else ++cardIdx;
            }
            cardT = 0; selPart = -1; recallRevealed = false; flashRated = false; scCards.Reset();
        }
        if (!flashRated) Txt("Flip the card, then mark your recall to continue.", 390, H - 96, 14, kMuted);
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
    int AwardXP(int amount) {
        int before = p.Level();
        int actual = amount * rewards.Multiplier();
        int gained = p.AddXP(actual);
        rewards.RecordXp(actual);
        levelUps += gained;
        resLevelCoins += p.lastLevelCoins;
        if (gained > 0) {
            std::string unlock = AvUnlockText(before, p.Level());
            Toast(unlock.empty() ? "Level up! You are now level " + std::to_string(p.Level()) : unlock);
        }
        return gained;
    }
    void Grade(bool right, bool isCode, const std::string& wrongInfo, const std::string& expl) {
        if (answered) return; // One judgement per attempt, regardless of overlapping input.
        answered = true; wasRight = right; answerT = 0;
        rewards.RecordAnswer(right);
        if (right) {
            correct++; streak++; bestStreak = std::max(bestStreak, streak);
            float mult = p.CoinMult() * (streak >= 3 ? 2.0f : 1.0f);
            int c = (int)std::lround((isCode ? 5 : 2) * mult);
            int x = isCode ? 20 : 10;
            p.coins += c; resCoins += c; resXp += x * rewards.Multiplier();
            int lv = AwardXP(x);
            p.Note(QK_CORRECT, 1); p.Note(QK_STREAK, streak);
            if (isCode) p.Note(QK_CODE, 1);
            feedback = expl;
            FxBurst(fxAt.x, fxAt.y, isCode ? 70 : 34);
            FxRing(fxAt.x, fxAt.y, kMint, 90);
            FxFloat(fxAt.x + 40, fxAt.y - 30, "+" + std::to_string(x * rewards.Multiplier()) + " XP", kMint);
            FxFloat(fxAt.x + 40, fxAt.y + 4, "+" + std::to_string(c) + " coins", kAmber, 22);
            if (lv > 0) FxBurst(W / 2, 120, 60);
        }
        else {
            lives = std::max(0, lives - 1); streak = 0; hurtT = 0.6f;
            // Don't reveal the exact solution until all hearts are gone: let the
            // learner retrieve it on the next attempt, with a targeted hint.
            feedback = lives == 0 ? wrongInfo + "  " + expl
                : (isCode ? "Try again. Hint: " + mod->code.hint
                          : "Try another option. You can use a hint or review the lesson.");
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
        switch (AfterLessonAnswer(wasRight, lives, cur, (int)items.size())) {
        case LessonNextStep::OutOfHearts: Finish(false); break;
        case LessonNextStep::Retry:
            // Keep the current question, selected code pieces, typed answer,
            // and any paid-for hints; only reset the attempt/feedback UI.
            answered = wasRight = false;
            picked = -1;
            feedback.clear(); answerT = 0; itemT = 0;
            break;
        case LessonNextStep::Complete: Finish(true); break;
        case LessonNextStep::Advance: ++cur; ResetItem(); break;
        }
    }
    void Finish(bool ok) {
        passed = ok; firstClear = false;
        if (ok) {
            std::string k = Key(modTopic, modIdx);
            firstClear = !p.done.count(k);
            if (firstClear) {
                bool test = modIdx < 0;
                int bx = test ? 50 : 20, bc = test ? 20 : 5;
                resXp += bx * rewards.Multiplier(); resCoins += bc; p.coins += bc;
                AwardXP(bx);
                p.done.insert(k);
                p.Note(QK_MODULES, 1);
            }
        }
        if (ok) rewards.RecordLesson();
        p.Save();
        hard = (ok && p.hardMode) ? FindHard(courses[course].name, modTopic, modIdx) : nullptr;
        hardFromMap = false;
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
            resXp += hard->xp * rewards.Multiplier(); resCoins += hard->coins; p.coins += hard->coins;
            AwardXP(hard->xp);
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
        DrawHeaderBackground();
        bool backClicked = false;
        float x;
        if (back) {
            backClicked = DrawHeaderBackButton();
            x = 84;
        }
        else {
            Box({ 24, 16, 40, 40 }, 12, kViolet);
            TxtC("{}", 44, 22, 22, Color{ 255, 255, 255, 255 }, true);
            x = 76;
        }
        if (back) DrawHeaderTitle(title, true);
        else {
            // Preserve the existing logo in the no-back top bar.
            Txt(Fit(title, 300, 28), x, 18, 28, kText);
        }

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
        Txt("XP " + std::to_string((int)inLvl) + " / 100" + (rewards.Active() ? " (2x)" : ""),
            lp.x + 50, lp.y + 5, 13, rewards.Active() ? kMint : kMuted);
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

    // One large click target per destination; the label makes the result explicit.
    // This is shared by the main menu and bonus-game shortcuts.
    bool DrawDestinationCard(Rectangle r, const std::string& title,
                             const std::string& description, const std::string& action,
                             Color accent, int icon, int hoverIndex) {
        const bool hover = CheckCollisionPointRec(Mouse(), r);
        cardHover[hoverIndex] = Smooth(cardHover[hoverIndex], hover ? 1.0f : 0.0f, 16, GetFrameTime());
        const float h = cardHover[hoverIndex];
        Rectangle face{r.x, r.y - 4 * h, r.width, r.height};
        Card3D(face, 22, Mix(kSurface, kSurface2, h * 0.7f), Mix(kBorder, accent, h), 6);
        Box({face.x + 20, face.y + 27, 65, 65}, 18, accent);
        Icon(icon, face.x + 52, face.y + 59, 18, OnColor(accent));
        Txt(title, face.x + 106, face.y + 19, 25, kText);
        Wrap(description, face.x + 106, face.y + 57, face.width - 133, 15, kMuted,
             true, false, nullptr, 1.25f);
        Txt(action + "  >", face.x + 106, face.y + face.height - 34, 17, accent);
        return hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    }

    void OpenPractice(Screen source) {
        practice.Start();
        practiceFrom = source;
        Go(Screen::Practice);
    }

    void DrawMenu() {
        TopBar("C++ Quest", false, true);
        scMenu.Begin({0, 74, W, H - 74}, H - 74, GetFrameTime());
        const int ci = CppIndex();
        const Course& cpp = courses[ci];
        const int total = std::max(1, cpp.TotalNodes());
        const int done = CourseDone(ci);

        Txt("What would you like to do?", 35, 94, 33, kText);
        Txt("Choose a path. You can switch activities at any time.", 35, 139, 16, kMuted);

        // Primary next step: continue the course with visible saved progress.
        Rectangle learn{35, 180, 1030, 198};
        const bool hovered = CheckCollisionPointRec(Mouse(), learn);
        cardHover[0] = Smooth(cardHover[0], hovered ? 1.0f : 0.0f, 16, GetFrameTime());
        Card3D(learn, 24, Mix(kSurface, kSurface2, 0.7f * cardHover[0]),
               Mix(kBorder, cpp.color, cardHover[0]), 8);
        Card3D({learn.x + 25, learn.y + 33, 124, 124}, 28, cpp.color, cpp.color, 5);
        TxtC("C++", learn.x + 87, learn.y + 72, 43, OnColor(cpp.color));
        Txt("LEARN C++", learn.x + 177, learn.y + 21, 15, kMint);
        Txt(done ? "Keep learning C++" : "Start your C++ journey", learn.x + 177, learn.y + 47, 28, kText);
        Txt(std::to_string(cpp.topics.size()) + " topics - lessons, quizzes and coding challenges",
            learn.x + 177, learn.y + 87, 15, kMuted);
        Bar({learn.x + 177, learn.y + 126, 505, 11}, (float)done / total, cpp.color);
        Txt(std::to_string(done) + " of " + std::to_string(total) + " completed (" +
            std::to_string(100 * done / total) + "%)", learn.x + 177, learn.y + 150, 15, kMuted);
        Rectangle start{learn.x + 770, learn.y + 68, 222, 62};
        if (Button(start, "Choose learning path", kMint, true, 21) ||
            (hovered && !CheckCollisionPointRec(Mouse(), start) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))) {
            Go(Screen::Learn);
        }

        // Equal-weight alternatives: practising and playing are not hidden under courses.
        if (DrawDestinationCard({35, 401, 499, 163}, "Practice C++",
                "Six quick and coding games. Get better through hands-on challenges.",
                "Choose a practice game", kSky, IC_BOLT, 1)) {
            OpenPractice(Screen::Menu);
        }
        if (DrawDestinationCard({556, 401, 509, 163}, "Bonus games",
                "Try the career game show, robot mazes, and a codebreaking vault.",
                "Explore bonus games", kViolet, IC_STAR, 2)) {
            Go(Screen::Games);
        }

        // Other language courses are still one click away, without hiding the games.
        Rectangle other{35, 584, 1030, 76};
        const bool hoverOther = CheckCollisionPointRec(Mouse(), other);
        Card3D(other, 17, hoverOther ? kSurface2 : kSurface,
               hoverOther ? kViolet : kBorder, 4);
        Txt("Other languages", other.x + 23, other.y + 12, 22, kText);
        Txt("Explore Java, Python, C# and JavaScript", other.x + 23, other.y + 43, 15, kMuted);
        TxtR("Browse courses  >", other.x + other.width - 27, other.y + 29, 18, kViolet);
        if (hoverOther && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            scBonus.Reset(); Go(Screen::Bonus);
        }
        scMenu.End();
    }

    // ----------------------------------------------------------- C++ LEARNING PATHS / LABS
    void DrawLearn() {
        if (TopBar("Learn C++", true, true)) { Go(Screen::Menu); return; }
        if (screen != Screen::Learn) return;
        HubMenu::Heading("Choose your C++ learning path", "Follow the guided course or solve coding labs with progressive hints and real test cases.");
        int ci = CppIndex();
        if (HubMenu::Card(HubMenu::CardRect(0), "Guided C++ course", "Learn through short lessons, quizzes and reviews.",
                          std::to_string(CourseDone(ci)) + " / " + std::to_string(courses[ci].TotalNodes()) + " COMPLETE",
                          "Continue course", kMint, IC_BOLT, "C++")) {
            OpenCourse(ci, Screen::Learn); return;
        }
        int count = 0;
        for (const auto& l : questlabs::All()) if (p.done.count(questlabs::SaveKey(l.id))) ++count;
        if (HubMenu::Card(HubMenu::CardRect(1), "Hands-on coding labs", std::to_string(questlabs::All().size())+" function-writing challenges based on your notes.",
                          std::to_string(count) + " / "+std::to_string(questlabs::All().size())+" COMPLETED  |  3 HINTS PER LAB",
                          "Browse labs", kSky, IC_BULB, "{ }") ) { scLabs.Reset(); Go(Screen::Labs); }
    }

    void OpenLab(int index) {
        labIndex=index; labHints=0;labChecked=false;labSolved=p.done.count(questlabs::SaveKey(index))>0;
        const auto& task=questlabs::All().at(index);
        labEditor.SetText(labDrafts[index].empty()?task.starter:labDrafts[index]);
        labResult={}; labResultNote=""; scLabInfo.Reset(); Go(Screen::LabTask);
    }
    void LeaveLab() {
        if (labIndex>=0 && labIndex<(int)labDrafts.size()) labDrafts[labIndex]=labEditor.Text();
        questlabs::SaveDrafts(labDrafts);
        Go(Screen::Labs);
    }
    void DrawLabs() {
        if (TopBar("C++ Labs", true, true)) { Go(Screen::Learn); return; }
        if (screen != Screen::Labs) return;
        HubMenu::Heading("Hands-on coding labs", "Read the short concept guide, write a function, then run tests. Wrong attempts do not remove hearts.");
        const float tabW=1030.0f / (float)questlabs::Groups().size();
        for (int group=0;group<(int)questlabs::Groups().size();group++) {
            Rectangle r{35+group*tabW,174,tabW-5,37};
            bool selected=labGroup==group;
            if(Button(r,questlabs::Groups()[group].title,selected?kMint:kSurface2,true,14,!selected)) {
                if(labGroup!=group){labGroup=group;scLabs.Reset();}
            }
        }
        std::vector<int> rows;
        for (const auto& l : questlabs::All()) if (l.group==labGroup) rows.push_back(l.id);
        const float lastBottom=rows.empty()?H:(HubMenu::CardRect((int)rows.size()-1).y+40+HubMenu::CardHeight+25);
        scLabs.Begin({0,218,W,H-218},std::max(H-218,lastBottom-218),GetFrameTime());
        int chosen=-1;
        for (int i=0;i<(int)rows.size();i++){
            const auto& l=questlabs::All()[rows[i]];
            Rectangle r=HubMenu::CardRect(i);r.y+=40;
            bool done=p.done.count(questlabs::SaveKey(l.id))!=0;
            std::string details=done?"COMPLETED  |  REPLAY ANYTIME":"NEW LAB  |  3 PROGRESSIVE HINTS";
            if (HubMenu::Card(r,std::to_string(l.id+1)+". "+l.title,l.summary,details,
                done?"Review solution":"Write solution",done?kMint:kSky,done?IC_CHECK:IC_BULB)) chosen=l.id;
        }
        scLabs.End();
        if(chosen>=0)OpenLab(chosen);
    }
    void SubmitLab(){
        const auto& l=questlabs::All()[labIndex];
        labDrafts[labIndex]=labEditor.Text();questlabs::SaveDrafts(labDrafts);
        labResult=questlabs::Run(l,labDrafts[labIndex]);labChecked=true;
        if(labResult.accepted) {
            bool first=p.done.insert(questlabs::SaveKey(l.id)).second;
            labSolved=true;
            if(first) {
                int earned=25*rewards.Multiplier();
                p.AddXP(earned);p.Note(QK_CODE,1);
                rewards.RecordAnswer(true);rewards.RecordXp(earned);
                p.Save();rewards.Save();labResultNote="Earned "+std::to_string(earned)+" XP";
                FxBurst(768,350,35);
            }else labResultNote="Already completed. No duplicate XP awarded.";
        }else labResultNote="No hearts lost. Adjust your code and try again.";
    }
    void DrawLabTask(float dt) {
        const auto& l=questlabs::All()[labIndex];
        if (TopBar("Lab "+std::to_string(l.id+1)+" of "+std::to_string(questlabs::All().size()),true,true)) {LeaveLab();return;}
        if(screen!=Screen::LabTask)return;
        Card3D({20,88,432,536},18,kSurface,kBorder,5);
        Txt(Fit(l.title,400,25),36,98,25,kText);
        Txt("READ  -  PLAN  -  WRITE  -  TEST",36,134,13,kSky);
        const float leftX=36, top=168, lw=399, lh=420;
        auto info=[&](bool draw)->float {
            float y=0;
            if(draw)Txt("CONCEPT FROM YOUR NOTES",leftX+10,top+y,14,kMint);
            y+=27;
            y+=Wrap(l.summary,leftX+10,top+y,lw-24,16,kText,draw,false,nullptr,1.3f)+17;
            if(draw)Txt("YOUR TASK",leftX+10,top+y,14,kSky);
            y+=27;
            y+=Wrap(l.prompt,leftX+10,top+y,lw-24,17,kText,draw,false,nullptr,1.35f)+20;
            if(draw)Txt("FUNCTION SIGNATURE",leftX+10,top+y,14,kMuted);
            y+=26;
            y+=Wrap(l.signature,leftX+10,top+y,lw-24,16,kAmber,draw,false,nullptr,1.3f)+18;
            if(draw)Txt("EXAMPLE",leftX+10,top+y,14,kMuted);
            y+=25;
            y+=Wrap(l.example,leftX+10,top+y,lw-24,16,kText,draw,false,nullptr,1.3f)+25;
            if(draw)Txt("PROGRESSIVE HINTS",leftX+10,top+y,14,kMuted);
            y+=26;
            for(int j=0;j<labHints;j++) {
                const float hh=Wrap(l.hints[j],leftX+21,top+y+10,lw-50,16,kSky,false);
                if(draw){Box({leftX+6,top+y,lw-20,hh+20},10,Al(kSky,0.12f));
                    Wrap(l.hints[j],leftX+21,top+y+10,lw-50,16,kSky);}
                y+=hh+28;
            }
            return y+20;
        };
        scLabInfo.Begin({leftX,top,lw,lh},std::max(lh,info(false)),dt);
        info(true);scLabInfo.End();
        const bool hintLeft=labHints<(int)l.hints.size()&&p.hints>0;
        if (Button({36,634,202,50},hintLeft?"Hint ("+std::to_string(p.hints)+" left)":"No hint tokens",kSky,hintLeft,18,true)) {
            ++labHints; --p.hints;p.Save();scLabInfo.target=scLabInfo.maxS;
        }
        if(Button({247,634,184,50},"Back to labs",kViolet,true,18,true)){LeaveLab();return;}

        Card3D({470,88,610,451},18,kSurface,kBorder,6);
        Txt("Solution.cpp",488,100,17,kMuted);
        Txt(questlabs::RuntimeReady()?"SAFE CODE TESTS READY":"WASI SDK + WASMTIME REQUIRED",739,100,14,questlabs::RuntimeReady()?kMint:kAmber);
        Rectangle view{482,132,585,394};
        labEditor.Update(view,true);
        if(labChecked && labEditor.Text()!=labDrafts[labIndex])labChecked=false;
        labEditor.Draw(view,true,time);
        if(labEditor.wantRun)SubmitLab();
        Card3D({470,551,610,75},15,kSurface,kBorder,4);
        if(!labChecked) {
            Txt("Write the function, then press Run tests.",489,563,17,kText);
            Txt("Ctrl+Enter to test  |  Code runs only in the isolated WASI runner.",489,592,14,kMuted);
        }else {
            Color c=labResult.accepted?kMint:labResult.compileError?kCoral:kAmber;
            Txt(labResult.accepted?"PASSED":labResult.compileError?"COMPILATION ERROR":"KEEP TRYING",489,558,16,c);
            Txt(Fit(labResult.message,558,13),489,583,13,kText);
            Txt(Fit(labResultNote,558,12),489,605,12,c);
        }
        if (Button({482,640,202,48},"Run tests",kMint,true,20)) SubmitLab();
        if (Button({695,640,170,48},"Reset starter",kSky,true,17,true)) {
            labEditor.SetText(l.starter);labDrafts[labIndex]=l.starter;questlabs::SaveDrafts(labDrafts);labChecked=false;
        }
        if (Button({875,640,192,48},"Copy code",kViolet,true,17,true)){
            std::string code=labEditor.Text();SetClipboardText(code.c_str());Toast("Code copied");
        }
    }

    void DrawMillionaire() {
        if (TopBar("C++ Programmer?", true, false)) { Go(Screen::Games); return; }
        millionaire.Draw();
    }

    void DrawRobotRoutines(float dt) {
        if (TopBar("Robot Routines", true, false)) { Go(Screen::Games); return; }
        robotRoutines.Draw(dt);
    }

    void DrawDebugVault() {
        if (TopBar("Debug Vault", true, false)) { Go(Screen::Games); return; }
        debugVault.Draw();
    }

    void DrawRapidReview(float dt) {
        if (TopBar("Rapid Review", true, false)) { Go(Screen::Games); return; }
        const int reward = rapidReview.Draw(dt);
        if (reward > 0) { AwardXP(reward); p.Save(); Toast("Rapid Review: +" + std::to_string(reward * rewards.Multiplier()) + " XP"); }
    }
    void DrawMatchMadness(float dt) {
        if (TopBar("Match Madness", true, false)) { Go(Screen::Games); return; }
        const int reward = matchMadness.Draw(dt);
        if (reward > 0) { AwardXP(reward); p.Save(); Toast("Match Madness: +" + std::to_string(reward * rewards.Multiplier()) + " XP"); }
    }

    // Shared hub layout: 2-column cards across Practice, Bonus Games and Other Languages.
    void DrawGames() {
        if (TopBar("Bonus games", true, true)) { Go(Screen::Menu); return; }
        if (screen != Screen::Games) return;
        struct Tile {
            const char* title;
            const char* desc;
            const char* detail;
            Color accent;
            int icon;
            Screen target;
        };
        const Tile tiles[] = {
            {"Who Wants to Be a C++ Programmer?", "A 15-question programming game show.",
             "TRIVIA  |  Lifelines and career ladder", kAmber, IC_STAR, Screen::Millionaire},
            {"Robot Routines", "Program a robot to escape a maze.",
             "PUZZLE  |  Random mazes", kSky, IC_BOLT, Screen::RobotRoutines},
            {"Debug Vault", "Crack a secret code using logic clues.",
             "PUZZLE  |  Fresh code every round", kViolet, IC_LOCK, Screen::DebugVault},
            {"Rapid Review", "Answer C++ questions in just 60 seconds.",
             "TIMED  |  Earn course XP", kMint, IC_BOLT, Screen::RapidReview},
            {"Match Madness", "Match C++ terms with their definitions.",
             "TIMED  |  Earn course XP", kCoral, IC_STAR, Screen::MatchMadness}
        };
        scBonus.Begin({0, 74, W, H - 74}, HubMenu::ScrollHeight(5), GetFrameTime());
        HubMenu::Heading("Bonus games", "Play short challenges, puzzles and timed C++ games.");
        Screen chosen = Screen::Games;
        for (int i = 0; i < 5; ++i) {
            const Tile& t = tiles[i];
            if (HubMenu::Card(HubMenu::CardRect(i), t.title, t.desc, t.detail,
                              "Play game", t.accent, t.icon)) chosen = t.target;
        }
        scBonus.End(); // close the scissor before switching screens
        if (chosen == Screen::Games) return;
        if (chosen == Screen::Millionaire) millionaire.Reset();
        if (chosen == Screen::RobotRoutines) robotRoutines.Reset();
        if (chosen == Screen::DebugVault) debugVault.Reset();
        if (chosen == Screen::RapidReview) rapidReview.Reset();
        if (chosen == Screen::MatchMadness) matchMadness.Reset();
        Go(chosen);
    }

    // Other Languages is deliberately the only Konami-code entrance to Silent Localhost.
    void DrawBonus() {
#ifdef JQ_SH2
        if (konami.Feed()) { sh2mode = std::make_unique<sh2::Mode>(&courses[CppIndex()].topics); return; }
#endif
        if (TopBar("Other languages", true, true)) { Go(Screen::Menu); return; }
        if (screen != Screen::Bonus) return;
        std::vector<int> idx;
        for (int i = 0; i < (int)courses.size(); ++i) if (IsBonus(i)) idx.push_back(i);
        scBonus.Begin({0, 74, W, H - 74}, HubMenu::ScrollHeight((int)idx.size()), GetFrameTime());
        HubMenu::Heading("Other languages", "Explore another language. Your XP, coins and character carry across courses.");
        int selected = -1;
        for (int k = 0; k < (int)idx.size(); ++k) {
            const int i = idx[k];
            const Course& c = courses[i];
            std::string detail = std::to_string(c.topics.size()) + " TOPICS";
            if (c.Ready()) {
                detail += "  |  " + std::to_string(CourseDone(i)) + " / " +
                          std::to_string(c.TotalNodes()) + " complete";
            } else detail += "  |  Preview";
            if (HubMenu::Card(HubMenu::CardRect(k), c.name, c.tagline, detail,
                              c.Ready() ? "Open course" : "Preview topics",
                              c.color, IC_STAR, Glyph(c.name))) selected = i;
        }
        scBonus.End();
        if (selected >= 0) {
            FxRing(Mouse().x, Mouse().y, courses[selected].color, 70);
            OpenCourse(selected, Screen::Bonus);
        }
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
        DrawAvatar(66, ac.y + 58 + std::sin(time * 2.2f) * 2.5f, 0.6f, p.look, true, time);
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
            if (hov && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { p.hardMode = !p.hardMode; p.Save(); Toast(p.hardMode ? "Hard mode on: coding challenge after each lesson" : "Hard mode off"); }
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
            if (hover && open && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                FxRing(c.x, c.y, T.color, 80);
                // An already-completed topic test must not require replaying its
                // full quiz merely to reach the Hard Mode boss challenge.
                const HardProblem* challenge = p.hardMode && done
                    ? FindHard(courses[course].name, selTopic, mm) : nullptr;
                if (challenge) {
                    modTopic = selTopic; modIdx = mm;
                    mod = isTest ? &T.test : &T.modules[mm];
                    hard = challenge; hardFromMap = true;
                    passed = true; firstClear = false;
                    resXp = resCoins = levelUps = resLevelCoins = bestStreak = 0;
                    StartHard();
                } else StartLesson(selTopic, mm);
            }
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
                else if (answered && wasRight && isCorrect) {
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
                Color badge = (answered && wasRight && isCorrect) ? kMint : (answered && i == picked) ? kCoral : kSurface2;
                DrawCircleV({ r.x + 34, r.y + r.height / 2 }, 16, Al(badge, k));
                if (answered && wasRight && isCorrect) Icon(IC_CHECK, r.x + 34, r.y + r.height / 2, 7, kBg);
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
            if (!mod->lesson.empty() && Button({ 336, H - 84, 190, 56 }, "Review lesson", kViolet, true, 19, true)) {
                reviewing = true; inCards = true; cardIdx = 0; cardT = 0; selPart = -1;
                recallRevealed = false; flashRated = false; flashRevisit = false;
                flashMissed.clear(); flashRevisitAt = 0; scCards.Reset();
            }
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
            Txt(wasRight ? "Correct!" : (lives > 0 ? "Not quite - retry!" : "Out of hearts"), 150 + 72, y + 18, 30, tone);
            int lines = 0; float fs = 17;
            Wrap(feedback, 0, 0, 560, fs, kText, false, false, &lines);
            if (lines > 3) fs = 14.5f;
            Wrap(feedback, 150 + 72, y + 62, 560, fs, Al(kText, 0.92f));
            const char* label = lives <= 0 ? "Finish" : (wasRight ? "Continue" : "Try again");
            if (Button({ W - 150 - 200, y + 46, 200, 58 }, label, tone, true, 23) || IsKeyPressed(KEY_ENTER)) NextItem();
        }
    }

    // ----------------------------------------------------------- HARD MODE SCREEN
    void DrawHard(float dt) {
        const HardProblem& H0 = *hard;
        DrawHeaderBackground();
        Box({ 24, 16, 330, 40 }, 20, Al(kCoral, 0.18f));
        Icon(IC_FLAME, 48, 36, 9, kCoral);
        Txt(modIdx < 0 ? "HARD MODE  -  Topic boss" : "HARD MODE  -  Code checkpoint", 68, 24, 16, kCoral);
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
            Txt("REFERENCE CHECK (not code execution)", con.x + 22, con.y + 14, 13, kMuted);
            Txt("Run compares your answer with known solutions; other valid code may fail.", con.x + 22, con.y + 42, 14, kMuted);
        }
        else {
            Color vc = lastV.accepted ? kMint : kCoral;
            Icon(lastV.accepted ? IC_CHECK : IC_CROSS, con.x + 34, con.y + 30, 10, vc);
            Txt(lastV.accepted ? "Reference matched" : "No reference match", con.x + 62, con.y + 14, 21, vc);
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
        if ((hardFromMap ? hardSolved : passed) && !resBurst && resT > 0.2f) {
            resBurst = true;
            FxBurst(cx, 220, 80); FxBurst(cx - 260, 330, 40); FxBurst(cx + 260, 330, 40);
        }
        float bob = (hardFromMap ? hardSolved : passed) ? std::sin(resT * 5) * 4 * std::exp(-resT * 0.8f) : 0;
        DrawAvatar(cx, 200 + bob - 10 * std::fabs(std::sin(resT * 6)) * std::exp(-resT * 1.2f), 1.5f, p.look, hardFromMap ? hardSolved : passed, resT);
        float k = EaseOutBack(resT / 0.5f);
        const bool challengeOnly = hardFromMap;
        const bool celebration = challengeOnly ? hardSolved : passed;
        TxtC(challengeOnly ? (hardSolved ? "Challenge solved!" : "Challenge skipped")
                  : (passed ? "Module complete!" : "Out of hearts"),
             cx, 318, 44 * k, celebration ? kMint : kCoral);
        TxtC(challengeOnly ? (hardSolved ? "Well done - your reward was saved." : "You can return to this challenge from the map.")
                 : (passed ? (firstClear ? "First-clear bonus added." : "Replay finished. Practice pays off.")
                           : "Review the topic and try again."),
             cx, 372, 18, kMuted);

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
            TxtC("LEVEL UP!  You reached level " + std::to_string(p.Level()) + "  (+" + std::to_string(resLevelCoins) + " coins)", cx, yy, 24 * pu, kAmber);
            yy += 34;
        }
        if (passed && modIdx < 0 && firstClear && modTopic + 1 < (int)topics.size()) {
            TxtC("Next topic unlocked: " + topics[modTopic + 1].name, cx, yy, 18, T.color);
            yy += 28;
        }
        if (Button({ cx - 110, H - 76, 220, 56 }, "Continue", passed ? kMint : kViolet, true, 23) || IsKeyPressed(KEY_ENTER)) Go(Screen::Map);
    }

    // ----------------------------------------------------------- CHARACTER
    // ----------------------------------------------------------- CHARACTER (wardrobe)
    void DrawCharacter() {
        if (TopBar("Character", true, false)) { p.Save(); Go(from); return; }
        float dt = GetFrameTime();
        Vector2 m = Mouse();
        AvatarLook& L = p.look;
        // Apply tab input before reading its item count. Otherwise a click that
        // switches from a large category to a smaller one can leave this frame
        // iterating with the old count and indexing past the new category.
        for (int f = 0; f < AV_FIELDS; f++) {
            Rectangle tb{ 524.0f + (f % 7) * 76, 108.0f + (f / 7) * 38, 72, 32 };
            if (f != avTab && CheckCollisionPointRec(m, tb) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                avTab = f;
                std::fill(avCellHover, avCellHover + 12, 0.0f);
                FxRing(m.x, m.y, AvPal(AV_OUTC, L.v[AV_OUTC]), 30);
                break;
            }
        }
        const Color accent = AvPal(AV_OUTC, L.v[AV_OUTC]);
        const int n = AvCount(avTab);
        const bool pal = AvIsPalette(avTab);

        // grid cells for the current tab (colour swatches or item cards)
        auto cell = [&](int i) {
            return pal ? Rectangle{ 524.0f + (i % 8) * 64, 214.0f + (i / 8) * 64, 56, 56 }
                       : Rectangle{ 524.0f + (i % 4) * 134, 190.0f + (i / 4) * 124, 126, 116 };
        };
        int hoverId = -1;                                              // item under the mouse = live try-on
        for (int i = 0; i < n; i++) if (CheckCollisionPointRec(m, cell(i))) hoverId = i;
        AvatarLook pv = L;
        if (hoverId >= 0) pv.v[avTab] = hoverId;

        // ---- stage
        Rectangle st{ 40, 96, 440, 584 };
        Card3D(st, 26, kSurface, kBorder, 7);
        float cx = st.x + st.width / 2;
        Rectangle in{ st.x + 8, st.y + 8, st.width - 16, st.height - 16 };
        ScissorOn(in.x, in.y, in.width, in.height);
        DrawAvatarBackdrop(in, pv.v[AV_BG], time, accent);
        charBounce += dt;
        float jump = -std::fabs(std::sin(charBounce * 9.0f)) * 46.0f * std::exp(-charBounce * 3.2f);
        float bob = std::sin(time * 2.2f) * 4.0f;
        Box({ cx - 105, 548, 210, 18 }, 9, Al(Color{ 0, 0, 0, 255 }, 0.35f));
        DrawPet(st.x + 84, 548, 1.7f, pv.v[AV_PET], time);
        DrawAvatar(cx, 330 + bob + jump, 2.2f, pv, true, time);
        EndScissorMode();
        Rectangle hit{ cx - 130, 160, 260, 400 };
        if (CheckCollisionPointRec(m, hit) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            charBounce = 0; FxRing(m.x, m.y, accent, 70); FxSparks(m.x, m.y, accent, 12);
        }
        Box({ st.x + 24, 554, st.width - 48, 118 }, 18, Al(kBg, 0.72f));
        TxtC("Level " + std::to_string(p.Level()), cx, 560, 30, kText);
        TxtC(LevelTitle(p.Level()), cx, 598, 18, kMint);
        int into = p.XpInLevel();
        Bar({ st.x + 50, 628, 340, 12 }, (float)into / 100.0f, kMint);
        TxtC(std::to_string(into) + " / 100 XP   -   " + std::to_string(100 - into) + " XP to level " + std::to_string(p.Level() + 1), cx, 646, 13, kMuted);

        // ---- wardrobe panel: tabs
        Rectangle wp{ 510, 96, 550, 464 };
        Card3D(wp, 22, kSurface, kBorder, 6);
        for (int f = 0; f < AV_FIELDS; f++) {
            Rectangle tb{ 524.0f + (f % 7) * 76, 108.0f + (f / 7) * 38, 72, 32 };
            bool sel = f == avTab, hv = CheckCollisionPointRec(m, tb);
            Box(tb, 10, sel ? accent : Mix(kSurface2, accent, hv ? 0.25f : 0.0f));
            TxtC(Fit(AvFieldName(f), 66, 13), tb.x + 36, tb.y + 8, 13, sel ? OnColor(accent) : kText);
        }
        if (pal) Txt("Colours are free - tap one to wear it.", 524, 184, 14, kMuted);

        // ---- wardrobe panel: swatches / item cards
        for (int i = 0; i < n; i++) {
            Rectangle r = cell(i);
            const bool hv = i == hoverId, on = L.v[avTab] == i;
            avCellHover[i] = Smooth(avCellHover[i], hv ? 1.0f : 0.0f, 16, dt);
            r.y -= 3 * avCellHover[i];
            if (pal) {
                Color c = AvPal(avTab, i);
                DrawCircleV({ r.x + 28, r.y + 32 }, 22, Mix(c, kBg, 0.5f));
                DrawCircleV({ r.x + 28, r.y + 28 }, hv ? 24.0f : 22.0f, c);
                if (on) DrawRing({ r.x + 28, r.y + 28 }, 26, 29, 0, 360, 32, kText);
                if (hv && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { L.v[avTab] = i; p.Save(); FxRing(r.x + 28, r.y + 28, c, 40); }
                continue;
            }
            const AvItem& it = AvItemAt(avTab, i);
            const bool un = AvUnlocked(avTab, i, p.Level(), p.owned), buy = AvCanBuy(avTab, i, p.Level(), p.owned);
            Card3D(r, 14, on ? Mix(kSurface2, accent, 0.18f) : kSurface2, on ? accent : kBorder, 4);

            // thumbnail: the avatar wearing just this item
            AvatarLook mv = L; mv.v[avTab] = i;
            if (avTab == AV_PET) DrawPet(r.x + 63, r.y + 72, 0.55f, i, time);
            else if (avTab == AV_BG) {
                Rectangle tr{ r.x + 6, r.y + 6, r.width - 12, 66 };
                ScissorOn(tr.x, tr.y, tr.width, tr.height);
                DrawAvatarBackdrop(tr, i, time, accent);
                EndScissorMode();
            }
            else DrawAvatar(r.x + 63, r.y + 46, 0.33f, mv, true, 0);

            if (!un && !buy) { Box(r, 14, Al(kBg, 0.45f)); Icon(IC_LOCK, r.x + 63, r.y + 40, 11, Al(kText, 0.8f)); }
            TxtC(Fit(it.name, 116, 13), r.x + 63, r.y + 80, 13, (un || buy) ? kText : kMuted);
            if (on) TxtC("Equipped", r.x + 63, r.y + 98, 12, kMint);
            else if (un) TxtC("Tap to wear", r.x + 63, r.y + 98, 12, kMuted);
            else if (buy) {
                Icon(IC_COIN, r.x + 44, r.y + 105, 7, kAmber);
                Txt(std::to_string(it.price), r.x + 55, r.y + 98, 13, p.coins >= it.price ? kAmber : kCoral);
            }
            else {
                Icon(IC_LOCK, r.x + 40, r.y + 105, 6, kMuted);
                Txt("Level " + std::to_string(it.level), r.x + 50, r.y + 98, 12, kMuted);
            }

            if (hv && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                if (un) { L.v[avTab] = i; p.Save(); FxRing(m.x, m.y, accent, 40); }
                else if (buy) {
                    if (p.coins >= it.price) {
                        p.coins -= it.price; p.owned.insert(AvKey(avTab, i)); L.v[avTab] = i; p.Save();
                        Toast(std::string("Bought ") + it.name); FxBurst(m.x, m.y, 30);
                    }
                    else Toast("Not enough coins");
                }
                else Toast("Unlocks at level " + std::to_string(it.level));
            }
        }

        // ---- stats + upcoming rewards
        Rectangle sp{ 510, 576, 550, 104 };
        Card3D(sp, 20, kSurface, kBorder, 5);
        int modules = 0; for (int c = 0; c < (int)courses.size(); c++) modules += CourseDone(c);
        struct Tile { const char* label; std::string value; Color c; };
        Tile tiles[3] = { {"Level", std::to_string(p.Level()), kMint}, {"Coins", std::to_string(p.coins), kAmber}, {"Modules", std::to_string(modules), kSky} };
        for (int i = 0; i < 3; i++) {
            Rectangle r{ 526.0f + i * 104, 592, 96, 72 };
            Box(r, 12, kSurface2);
            Box({ r.x, r.y + 12, 4, r.height - 24 }, 2, tiles[i].c);
            Txt(tiles[i].label, r.x + 14, r.y + 8, 12, kMuted);
            Txt(tiles[i].value, r.x + 14, r.y + 30, 26, kText);
        }
        Txt("Next rewards", 852, 586, 14, kMuted);
        auto up = AvUpcoming(p.Level(), 3);
        for (size_t i = 0; i < up.size(); i++) {
            Txt("Lv " + std::to_string(up[i].first), 852, 610 + i * 20.0f, 13, kViolet);
            Txt(Fit(up[i].second, 150, 13), 892, 610 + i * 20.0f, 13, kText);
        }
        if (up.empty()) Txt("Everything unlocked!", 852, 612, 14, kMint);
    }
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

    // Weekly quests exchange local progress codes; no online account or server is implied.
    void DrawRewardsHub() {
        Txt("DAILY QUESTS AND BOOSTS", 150, 685, 21, kAmber);
        Rectangle boost{150, 720, 800, 104};
        Card3D(boost, 18, kSurface, kBorder, 4);
        Txt("2x XP for 15 minutes", 175, 740, 21, kText);
        Txt(rewards.Active() ? "ACTIVE: " + std::to_string(rewards.SecondsRemaining() / 60) + "m " +
            std::to_string(rewards.SecondsRemaining() % 60) + "s remaining" :
            std::to_string(rewards.Tokens()) + " boost tokens available", 175, 777, 17, rewards.Active() ? kMint : kMuted);
        Txt("Study streak: " + std::to_string(rewards.DayStreak()) + " days (milestones: 3, 7, 14, 30)",
            175, 801, 13, kMuted);
        if (Button({732, 744, 185, 56}, rewards.Active() ? "Boost active" : "Activate 2x", kMint,
                   !rewards.Active() && rewards.Tokens() > 0, 19)) {
            if (rewards.Activate()) Toast("2x XP active for 15 minutes!");
        }
        static const char* dailyLabels[3] = {
            "Answer 5 questions correctly", "Complete 1 course lesson", "Earn 100 XP today"
        };
        for (int i = 0; i < 3; ++i) {
            float y = 838.0f + i * 98.0f;
            Card3D({150,y,800,87},16,kSurface,kBorder,4);
            Txt(dailyLabels[i],174,y+9,20,kText);
            Txt(std::to_string(std::min(rewards.DailyProgress(i), rewards.DailyTarget(i))) + "/" +
                std::to_string(rewards.DailyTarget(i)) + "  |  Reward: 1 boost token", 174, y+48,15,kMuted);
            bool ready = rewards.CanClaimDaily(i);
            if (Button({752,y+18,172,51}, rewards.DailyClaimed(i) ? "Claimed" : "Claim", kMint,
                       ready,18) && rewards.ClaimDaily(i)) Toast("Daily quest: +1 XP boost token!");
        }
        Txt("TREASURE CHESTS",150,1151,21,kAmber);
        for (int i=0;i<2;++i) {
            float x = 150.0f + i*412.0f;
            Card3D({x,1187,388,107},16,kSurface,kBorder,4);
            Txt(i==0 ? "Early Bird" : "Night Owl",x+16,1203,22,kText);
            Txt(i==0 ? "Finish a lesson before noon" : "Finish a lesson after 18:00",x+16,1237,15,kMuted);
            if (Button({x+224,1246,145,40},"Open chest",kAmber,rewards.CanClaimChest(i),15)
                && rewards.ClaimChest(i)) Toast("Treasure chest: +1 boost token!");
        }
        Txt("FRIENDS QUEST - WEEKLY",150,1323,22,kSky);
        Card3D({150,1362,800,446},19,kSurface,kBorder,5);
        Txt("Team goal: earn 300 combined XP this week",171,1385,20,kText);
        Txt("You: " + std::to_string(rewards.WeeklySelfXp()) + " XP   |   Friend: " +
            std::to_string(rewards.WeeklyFriendXp()) + " XP",171,1426,18,kMuted);
        Bar({171,1460,759,14},(float)(rewards.WeeklySelfXp()+rewards.WeeklyFriendXp()) / rewards.WeeklyTarget(),kSky);
        Txt("Offline co-op: exchange progress codes with a friend (no server or automatic sync).",171,1492,15,kMuted);
        Txt("My weekly code:",171,1529,16,kText);
        const std::string myCode = rewards.ShareCode();
        Txt(Fit(myCode,490,17,true),171,1557,17,kSky,true);
        if (Button({748,1535,172,43},"Copy my code",kSky,true,16,true)) {
            SetClipboardText(myCode.c_str()); Toast("Progress code copied!");
        }
        Txt("Paste or type your friend's code:",171,1608,17,kText);
        Rectangle inputRect{171,1644,536,49};
        BoxB(inputRect,10,kBg,editingFriendCode?kSky:kBorder,2);
        Txt(Fit(incomingFriendCode.empty() ? "Click here, then Ctrl+V" : incomingFriendCode,510,16,true),182,1657,16,
            incomingFriendCode.empty()?kMuted:kText,true);
        if (CheckCollisionPointRec(Mouse(),inputRect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) editingFriendCode=true;
        if (editingFriendCode) {
            for (int c=GetCharPressed(); c>0; c=GetCharPressed()) {
                if (c>=33 && c<=126 && incomingFriendCode.size()<90) incomingFriendCode+=(char)c;
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !incomingFriendCode.empty()) incomingFriendCode.pop_back();
            if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
                const char* clip=GetClipboardText(); if (clip) incomingFriendCode=std::string(clip).substr(0,90);
            }
            if (IsKeyPressed(KEY_ENTER)) editingFriendCode=false;
        }
        if (Button({725,1644,192,49},"Import code",kSky,!incomingFriendCode.empty(),17)
            ) {
            friendStatus = rewards.Import(incomingFriendCode) ? "Friend progress updated!" : "Invalid, expired, or different partner code.";
            incomingFriendCode.clear(); editingFriendCode=false;
        }
        Txt(Fit(friendStatus.empty()?rewards.PartnerLabel():friendStatus,745,16),171,1706,16,kMuted);
        if (Button({678,1731,242,55},rewards.WeeklyClaimed()?"Reward claimed":"Claim team reward",kMint,
                   rewards.CanClaimWeekly(),18) && rewards.ClaimWeekly()) {
            Toast("Team quest complete: +2 boost tokens!");
        }
        Txt("Codes detect typos but do not verify identity. Share only with someone you trust.",171,1765,13,kMuted);
    }

    // ----------------------------------------------------------- QUESTS
    void DrawQuests() {
        if (TopBar("Quests", true, false)) { p.Save(); Go(from); return; }
        scQuest.Begin({ 0, 74, W, H - 74 }, 1850.0f, GetFrameTime());
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
        DrawRewardsHub();
        scQuest.End();
    }
};
