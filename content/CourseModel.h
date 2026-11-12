#pragma once
// ---------------------------------------------------------------------------
// Content.h  -  ALL quiz content lives here. Add topics/modules/questions by
// adding more Q(...) / M(...) lines; nothing else in the code has to change.
//
//  Q(prompt, {options}, explanation)  -> the FIRST option is the correct one
//                                        (options are shuffled at runtime).
//  M(title, {questions})              -> a module (a 5 minute mini quiz)
//  WithCode(module, ...)              -> adds a typed "small code question"
//  WithLesson(module, {cards})        -> adds "learn first" cards before a module
//  MakeTopic(...)                     -> groups modules + a topic test
// ---------------------------------------------------------------------------
#include <string>
#include <utility>
#include <vector>
#include "raylib.h"

struct Question {
    std::string prompt;
    std::vector<std::string> options;
    int correct = 0;          // index into options (always 0 in the data)
    std::string explain;
};

struct CodeTask {
    std::string prompt;
    std::string hint;
    std::vector<std::string> accepted;  // whitespace-insensitive matches
    std::string explain;
    int mode = -1;   // -1 auto (by topic position), 0 build from pieces, 1 fill the blank, 2 type it
};

struct LessonCard {
    std::string title;
    std::string text;
    std::string code;
    std::vector<std::pair<std::string, std::string>> parts;
    // Optional active-recall prompt. The model answer is hidden until requested.
    std::string recallPrompt;
    std::string recallAnswer;
};

struct Module {
    std::string title;
    std::vector<Question> questions;
    bool hasCode = false;
    CodeTask code;
    std::vector<LessonCard> lesson;
};

struct Topic {
    std::string name, shortName, blurb;
    Color color{};
    int cost = 0;
    std::vector<Module> modules;
    Module test;
};

inline Question Q(const std::string& p, std::vector<std::string> o, const std::string& e) {
    Question q; q.prompt = p; q.options = std::move(o); q.correct = 0; q.explain = e; return q;
}
inline Module M(const std::string& title, std::vector<Question> qs) {
    Module m; m.title = title; m.questions = std::move(qs); return m;
}
inline Module WithLesson(Module m, std::vector<LessonCard> cards) { m.lesson = std::move(cards); return m; }
inline Module WithCodeMode(Module m, int mode) { m.code.mode = mode; return m; }
inline Module WithCode(Module m, const std::string& prompt, const std::string& hint,
    std::vector<std::string> accepted, const std::string& explain) {
    m.hasCode = true; m.code = { prompt, hint, std::move(accepted), explain }; return m;
}
inline Topic MakeTopic(const std::string& name, const std::string& shortName, const std::string& blurb,
    Color c, int cost, std::vector<Module> mods, Module test) {
    Topic t; t.name = name; t.shortName = shortName; t.blurb = blurb; t.color = c;
    t.cost = cost; t.modules = std::move(mods); t.test = std::move(test); t.test.title = "Topic Test";
    return t;
}

struct Course {
    std::string name, tagline;
    Color color{};
    std::vector<Topic> topics;

    bool Ready() const { for (auto& t : topics) if (!t.modules.empty()) return true; return false; }
    int  TotalNodes() const {
        int n = 0;
        for (auto& t : topics) if (!t.modules.empty()) n += (int)t.modules.size() + 1;
        return n;
    }
};

struct OutlineRow { const char* name; const char* shortName; const char* blurb; };

inline std::vector<Topic> OutlineTopics(const std::vector<OutlineRow>& rows, Color c) {
    std::vector<Topic> v;
    for (size_t i = 0; i < rows.size(); i++)
        v.push_back(MakeTopic(rows[i].name, rows[i].shortName, rows[i].blurb, c,
            i == 0 ? 0 : 30 + (int)i * 40, {}, Module{}));
    return v;
}
