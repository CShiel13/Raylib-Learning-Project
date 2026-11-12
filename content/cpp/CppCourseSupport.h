#pragma once
// Shared helpers for the independently compiled C++ course topic groups.
// Code answers: accepted[0] is the "model" answer used by Build/Fill modes; later entries are alternatives.
#include <string>
#include <utility>
#include <vector>
#include "../CourseModel.h"

typedef std::vector<std::pair<std::string, std::string>> Parts;
inline LessonCard L(const std::string& title, const std::string& text, const std::string& code = "", Parts parts = {}) {
    LessonCard c; c.title = title; c.text = text; c.code = code; c.parts = std::move(parts); return c;
}
inline Color CppColor(int i) {
    static const Color pal[9] = {{62,224,176,255}, {77,171,255,255}, {255,196,61,255}, {139,108,255,255}, {255,134,208,255},
                                 {255,150,60,255}, {0,205,156,255}, {255,92,110,255}, {120,200,255,255}};
    return pal[i % 9];
}
inline Topic CppTopic(int i, const std::string& name, const std::string& shortName, const std::string& blurb,
                      std::vector<Module> mods, Module test) {
    return MakeTopic(name, shortName, blurb, CppColor(i), i == 0 ? 0 : 30 + i * 12, std::move(mods), std::move(test));
}
