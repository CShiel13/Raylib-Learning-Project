#pragma once
#include <cctype>
#include <string>
#include <vector>


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
        else if (s[i] == '"' || s[i] == '\'') { // preserve string AND char literal contents
            const char quote = s[i];
            r += s[i++];
            while (i < n) {
                const char ch = s[i++];
                r += ch;
                if (ch == '\\' && i < n) r += s[i++]; // escaped quote/backslash
                else if (ch == quote) break;
            }
        }
        else { if (!std::isspace((unsigned char)s[i])) r += s[i]; i++; }
    }
    return r;
}

// Built-in reference matcher: never executes submissions or example tests.
// It recognizes a limited set of examples, NOT arbitrary semantically correct code.
// Keep tests[] as -1 (not run) to avoid implying real test execution.
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
            v.accepted = true;
            v.message = "Reference matched. Examples were not executed.";
            return v;
        }
        v.message = "No reference match. Other correct solutions may be rejected by this text-only checker.";
        return v;
    }
};

// ------------------------------------------------------------------ registry
