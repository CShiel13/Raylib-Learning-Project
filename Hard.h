#pragma once
// ---------------------------------------------------------------------------
// Hard.h - foundation for "hard mode": one LeetCode / HackerRank style problem per module.
//
//  * HardProblem  : the problem (statement, examples, constraints, starter code, hints, rewards)
//  * IJudge       : how a submission is judged. TextJudge (below) is the built-in one.
//                   To run real code later, write a judge that writes the player's code to a
//                   temp file, runs `javac` / `python` / `g++` on each test case, and returns a
//                   Verdict - then swap it in where Game creates `judge`. Nothing else changes.
//  * CodeEditor   : multi-line editor (typing, Enter auto-indent, arrows, click-to-place caret,
//                   Tab, Home/End, Ctrl+V, scrolling, line numbers, basic syntax colours).
//  * AddHard(...) : register a problem for  course name + topic index + module index
//                   (module index -1 = the topic test). See BuildHard() for examples.
// ---------------------------------------------------------------------------
#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <vector>
#include "UI.h"

struct HardExample { std::string input, output, note; };

struct HardProblem {
    std::string title;
    std::string difficulty = "Easy";                 // Easy / Medium / Hard (just a colour + label)
    std::string statement;
    std::vector<HardExample> examples;
    std::vector<std::string> constraints;
    std::string starter;                             // initial editor text ('\n' separated)
    std::vector<std::string> solutions;              // reference solutions (TextJudge compares to these)
    std::vector<std::string> mustContain;            // tokens a valid answer needs (gives nicer feedback)
    std::vector<std::string> hints;                  // revealed one at a time, each costs a hint token
    int xp = 40, coins = 15;                         // first-solve bonus
};

struct Verdict {
    bool accepted = false;
    std::string message;
    std::vector<int> tests;                          // per example: 1 pass, 0 fail, -1 not run
};

class IJudge {
public:
    virtual ~IJudge() {}
    virtual Verdict Run(const HardProblem& p, const std::string& code) = 0;
};

// Removes // and /* */ comments and all whitespace, so formatting differences don't matter.
inline std::string NormCode(const std::string& s) {
    std::string r; size_t n = s.size();
    for (size_t i = 0; i < n;) {
        if (s[i] == '/' && i + 1 < n && s[i + 1] == '/') { while (i < n && s[i] != '\n') i++; }
        else if (s[i] == '/' && i + 1 < n && s[i + 1] == '*') { i += 2; while (i + 1 < n && !(s[i] == '*' && s[i + 1] == '/')) i++; i += 2; }
        else if (s[i] == '"') {                                   // keep string contents verbatim
            size_t j = i + 1; r += '"';
            while (j < n && s[j] != '"') { if (s[j] == '\\' && j + 1 < n) r += s[j++]; r += s[j++]; }
            if (j < n) r += '"';
            i = j + 1;
        }
        else { if (!std::isspace((unsigned char)s[i])) r += s[i]; i++; }
    }
    return r;
}

// Built-in judge: no code is executed. It checks required tokens, then compares the
// normalised submission with the reference solutions.
class TextJudge : public IJudge {
public:
    Verdict Run(const HardProblem& p, const std::string& code) override {
        Verdict v; v.tests.assign(p.examples.size(), -1);
        std::string n = NormCode(code);
        if (n.empty()) { v.message = "The editor is empty. Write your solution first."; return v; }
        std::string missing;
        for (auto& t : p.mustContain) if (n.find(NormCode(t)) == std::string::npos) missing += (missing.empty() ? "" : ", ") + t;
        if (!missing.empty()) { v.message = "Your solution should use: " + missing + "."; return v; }
        for (auto& s : p.solutions) if (NormCode(s) == n) {
            v.accepted = true; v.message = "Accepted. Your solution matches the expected behaviour.";
            v.tests.assign(p.examples.size(), 1); return v;
        }
        v.message = "Not accepted yet. Compare your logic with the examples and check edge cases.";
        v.tests.assign(p.examples.size(), 0);
        return v;
    }
};

// ------------------------------------------------------------------ registry
inline std::map<std::string, HardProblem>& HardRegistry() { static std::map<std::string, HardProblem> m; return m; }
inline std::string HardKey(const std::string& course, int topic, int module) { return course + ":" + std::to_string(topic) + ":" + std::to_string(module); }
inline void AddHard(const std::string& course, int topic, int module, HardProblem p) { HardRegistry()[HardKey(course, topic, module)] = std::move(p); }
inline const HardProblem* FindHard(const std::string& course, int topic, int module) {
    auto& m = HardRegistry();
    auto it = m.find(HardKey(course, topic, module));
    return it == m.end() ? nullptr : &it->second;
}

// Sample problems. Add yours the same way: AddHard("Java", topicIndex, moduleIndex, {...});
inline void BuildHard() {
    static bool done = false; if (done) return; done = true;
    {
        HardProblem p;
        p.title = "Sum of Two"; p.difficulty = "Easy";
        p.statement = "Complete the method sum so that it returns the sum of its two int parameters.";
        p.examples = { {"sum(2, 3)", "5", ""}, {"sum(-4, 10)", "6", "Negative numbers work too."} };
        p.constraints = { "-1000 <= a, b <= 1000" };
        p.starter = "public class Solution {\n    public static int sum(int a, int b) {\n        // your code here\n    }\n}\n";
        p.solutions = { "public class Solution { public static int sum(int a, int b) { return a + b; } }",
                       "public class Solution { public static int sum(int a, int b) { return b + a; } }" };
        p.mustContain = { "return" };
        p.hints = { "Methods hand back a value with the return keyword.", "Combine a and b with the + operator." };
        AddHard("Java", 0, 0, p);
    }
    {
        HardProblem p;
        p.title = "Even or Odd"; p.difficulty = "Easy";
        p.statement = "Write parity so it returns the String \"even\" when n is even and \"odd\" when it is odd.";
        p.examples = { {"parity(4)", "\"even\"", ""}, {"parity(7)", "\"odd\"", ""}, {"parity(0)", "\"even\"", "Zero is even."} };
        p.constraints = { "0 <= n <= 1,000,000" };
        p.starter = "public class Solution {\n    public static String parity(int n) {\n        // your code here\n    }\n}\n";
        p.solutions = { "public class Solution { public static String parity(int n) { return n % 2 == 0 ? \"even\" : \"odd\"; } }",
                       "public class Solution { public static String parity(int n) { if (n % 2 == 0) { return \"even\"; } return \"odd\"; } }",
                       "public class Solution { public static String parity(int n) { if (n % 2 == 0) { return \"even\"; } else { return \"odd\"; } } }" };
        p.mustContain = { "%", "return" };
        p.hints = { "The % operator gives a remainder. What is the remainder of an even number divided by 2?" };
        AddHard("Java", 0, 1, p);
    }
    {
        HardProblem p;
        p.title = "Fizz or Buzz"; p.difficulty = "Medium";
        p.statement = "Write fizz so it returns \"FizzBuzz\" if n is divisible by both 3 and 5, \"Fizz\" if divisible by 3 only, "
            "\"Buzz\" if divisible by 5 only, otherwise the number as a String.";
        p.examples = { {"fizz(15)", "\"FizzBuzz\"", "Divisible by 3 and 5."}, {"fizz(9)", "\"Fizz\"", ""}, {"fizz(10)", "\"Buzz\"", ""}, {"fizz(7)", "\"7\"", ""} };
        p.constraints = { "1 <= n <= 100,000" };
        p.starter = "public class Solution {\n    public static String fizz(int n) {\n        // your code here\n    }\n}\n";
        p.solutions = { "public class Solution { public static String fizz(int n) { if (n % 15 == 0) { return \"FizzBuzz\"; } if (n % 3 == 0) { return \"Fizz\"; } if (n % 5 == 0) { return \"Buzz\"; } return String.valueOf(n); } }",
                       "public class Solution { public static String fizz(int n) { if (n % 3 == 0 && n % 5 == 0) { return \"FizzBuzz\"; } if (n % 3 == 0) { return \"Fizz\"; } if (n % 5 == 0) { return \"Buzz\"; } return String.valueOf(n); } }" };
        p.mustContain = { "FizzBuzz", "Fizz", "Buzz", "%" };
        p.hints = { "Check the combined case (3 and 5) first, otherwise the single cases would catch it.", "String.valueOf(n) turns an int into a String." };
        p.xp = 60; p.coins = 25;
        AddHard("Java", 1, 0, p);
    }
}

// ------------------------------------------------------------------ code editor
struct CodeEditor {
    std::vector<std::string> lines{ "" };
    int line = 0, col = 0;
    float scroll = 0;
    bool wantRun = false;                            // set when Ctrl+Enter was pressed
    bool inputTruncated = false;                     // sticky until SetText; shared by course and Practice editors

    void SetText(const std::string& t) {
        lines.clear(); std::string cur;
        for (char c : t) { if (c == '\n') { lines.push_back(cur); cur.clear(); } else if (c != '\r') cur += c; }
        lines.push_back(cur);
        line = col = 0; scroll = 0; wantRun = false; inputTruncated = false;
        // put the caret on a fresh indented line under the "your code here" comment
        for (size_t i = 0; i < lines.size(); i++) if (lines[i].find("your code here") != std::string::npos) {
            size_t ind = 0; while (ind < lines[i].size() && lines[i][ind] == ' ') ind++;
            lines.insert(lines.begin() + i + 1, std::string(ind, ' '));
            line = (int)i + 1; col = (int)ind; break;
        }
    }
    std::string Text() const { std::string r; for (size_t i = 0; i < lines.size(); i++) { r += lines[i]; if (i + 1 < lines.size()) r += '\n'; } return r; }

    static constexpr float kLine = 28, kPad = 12, kGutter = 52;

    void Insert(const std::string& s) {
        for (char c : s) {
            if (c == '\n') NewLine();
            else if (c >= 32 && c <= 126) {
                if (lines[line].size() < 220) lines[line].insert(col++, 1, c);
                else inputTruncated = true;
            }
        }
    }
    void NewLine() {
        std::string rest = lines[line].substr(col); lines[line].erase(col);
        size_t ind = 0; while (ind < lines[line].size() && lines[line][ind] == ' ') ind++;
        std::string t = lines[line]; while (!t.empty() && t.back() == ' ') t.pop_back();
        std::string pre(ind + (!t.empty() && t.back() == '{' ? 4 : 0), ' ');
        lines.insert(lines.begin() + line + 1, pre + rest);
        line++; col = (int)pre.size();
    }

    // Handles input. `focused` = the editor owns the keyboard.
    void Update(Rectangle view, bool focused) {
        wantRun = false;
        Vector2 m = Mouse();
        bool hover = CheckCollisionPointRec(m, view);
        float maxS = std::max(0.0f, lines.size() * kLine + 2 * kPad - view.height);
        float wheel = GetMouseWheelMoveV().y; if (wheel == 0) wheel = GetMouseWheelMove();
        if (hover && wheel != 0) scroll -= wheel * kLine * 3;
        bool moved = false;
        if (focused) {
            bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
            auto rep = [](int k) { return IsKeyPressed(k) || IsKeyPressedRepeat(k); };
            int ch = GetCharPressed();
            while (ch > 0) { if (!ctrl && ch >= 32 && ch <= 126) { Insert(std::string(1, (char)ch)); moved = true; } ch = GetCharPressed(); }
            if (ctrl && IsKeyPressed(KEY_ENTER)) wantRun = true;
            else if (rep(KEY_ENTER) || rep(KEY_KP_ENTER)) { NewLine(); moved = true; }
            if (ctrl && IsKeyPressed(KEY_V)) { const char* c = GetClipboardText(); if (c) Insert(c); moved = true; }
            if (rep(KEY_TAB)) { Insert("    "); moved = true; }
            if (rep(KEY_BACKSPACE)) {
                if (col > 0) lines[line].erase(--col, 1);
                else if (line > 0) { col = (int)lines[line - 1].size(); lines[line - 1] += lines[line]; lines.erase(lines.begin() + line); line--; }
                moved = true;
            }
            if (rep(KEY_DELETE)) {
                if (col < (int)lines[line].size()) lines[line].erase(col, 1);
                else if (line + 1 < (int)lines.size()) { lines[line] += lines[line + 1]; lines.erase(lines.begin() + line + 1); }
                moved = true;
            }
            if (rep(KEY_LEFT)) { if (col > 0) col--; else if (line > 0) { line--; col = (int)lines[line].size(); } moved = true; }
            if (rep(KEY_RIGHT)) { if (col < (int)lines[line].size()) col++; else if (line + 1 < (int)lines.size()) { line++; col = 0; } moved = true; }
            if (rep(KEY_UP)) { if (line > 0) line--; col = std::min(col, (int)lines[line].size()); moved = true; }
            if (rep(KEY_DOWN)) { if (line + 1 < (int)lines.size()) line++; col = std::min(col, (int)lines[line].size()); moved = true; }
            if (IsKeyPressed(KEY_HOME)) { col = 0; moved = true; }
            if (IsKeyPressed(KEY_END)) { col = (int)lines[line].size(); moved = true; }
        }
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {           // click to place the caret
            int l = (int)std::floor((m.y - view.y - kPad + scroll) / kLine);
            line = std::max(0, std::min((int)lines.size() - 1, l));
            float rel = m.x - (view.x + kGutter + 10);
            int c = 0;
            while (c < (int)lines[line].size() && TxtW(lines[line].substr(0, c + 1), 19, true) - 4 < rel) c++;
            col = c; moved = true;
        }
        if (moved) {                                                      // keep the caret in view
            float top = line * kLine, bot = top + kLine;
            if (top < scroll) scroll = top;
            if (bot > scroll + view.height - 2 * kPad) scroll = bot - (view.height - 2 * kPad);
        }
        scroll = std::max(0.0f, std::min(maxS, scroll));
    }

    void Draw(Rectangle view, bool focused, float time) const {
        static const char* kw[] = { "public", "private", "protected", "static", "final", "class", "void", "int", "long", "double", "float", "boolean", "char",
                                   "String", "if", "else", "for", "while", "do", "return", "new", "true", "false", "null", "switch", "case", "break",
                                   "continue", "import", "package", "extends", "implements", "interface", "try", "catch", "finally", "throw", "throws" };
        Inset(view, 14, Color{ 9, 12, 20, 255 }, kBorder);
        ScissorOn(view.x + 3, view.y + 3, view.width - 6, view.height - 6);
        DrawRectangle((int)view.x + 2, (int)view.y + 2, (int)kGutter, (int)view.height - 4, Al(Color{ 255, 255, 255, 255 }, 0.03f));
        const float fs = 19;
        for (size_t i = 0; i < lines.size(); i++) {
            float y = view.y + kPad + i * kLine - scroll;
            if (y + kLine < view.y || y > view.y + view.height) continue;
            if ((int)i == line && focused) DrawRectangle((int)view.x + (int)kGutter + 2, (int)y - 2, (int)(view.width - kGutter - 6), (int)kLine, Al(kSky, 0.07f));
            TxtR(std::to_string(i + 1), view.x + kGutter - 10, y + 2, 15, (int)i == line ? kText : kLock, true);
            const std::string& s = lines[i];
            float x0 = view.x + kGutter + 10;
            size_t k = 0;
            while (k < s.size()) {
                size_t st = k; Color c = Color{ 205, 214, 235, 255 };
                if (s[k] == '/' && k + 1 < s.size() && s[k + 1] == '/') { k = s.size(); c = kMuted; }
                else if (s[k] == '"') { k++; while (k < s.size() && s[k] != '"') { if (s[k] == '\\') k++; k++; } k = std::min(k + 1, s.size()); c = kAmber; }
                else if (std::isalpha((unsigned char)s[k]) || s[k] == '_') {
                    while (k < s.size() && (std::isalnum((unsigned char)s[k]) || s[k] == '_')) k++;
                    std::string w = s.substr(st, k - st);
                    for (auto q : kw) if (w == q) c = kViolet;
                    if (c.r == 205 && k < s.size() && s[k] == '(') c = kSky;
                }
                else if (std::isdigit((unsigned char)s[k])) { while (k < s.size() && (std::isdigit((unsigned char)s[k]) || s[k] == '.')) k++; c = kMint; }
                else k++;
                Txt(s.substr(st, k - st), x0 + TxtW(s.substr(0, st), fs, true), y, fs, c, true);
            }
            if ((int)i == line && focused && (int)(time * 2) % 2 == 0) {
                float cx = x0 + TxtW(s.substr(0, std::min((size_t)col, s.size())), fs, true);
                DrawRectangle((int)cx, (int)y - 1, 2, (int)kLine - 4, kText);
            }
        }
        EndScissorMode();
        if (inputTruncated) {
            float warningWidth = std::min(360.0f, view.width - 16.0f);
            Rectangle warning{ view.x + 8, view.y + view.height - 30, warningWidth, 24 };
            Box(warning, 7, Al(kAmber, 0.96f));
            Txt("Text truncated: 220 character line limit", warning.x + 8, warning.y + 4, 12, kBg);
        }
    }
};
