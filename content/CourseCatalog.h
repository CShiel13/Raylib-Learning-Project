#pragma once
// Composition root for the bundled language catalogues.
#include "CourseModel.h"
#include "catalog/JavaCourse.h"
#include "catalog/CppPlaceholderCourse.h"
#include "catalog/PythonCourse.h"
#include "catalog/CSharpCourse.h"
#include "catalog/JavaScriptCourse.h"

inline std::vector<Course> BuildCourses() {
    std::vector<Course> courses;
    courses.push_back({ "Java", "Classes, OOP, collections, streams, Spring Boot and more.",
                        Color{255,150,0,255}, BuildJavaTopics() });
    courses.push_back({ "C++", "Systems programming: memory, pointers, templates and the STL.",
                        Color{60,120,220,255}, BuildCppTopics() });
    courses.push_back({ "Python", "Readable, powerful scripting for beginners and pros.",
                        Color{53,114,165,255}, BuildPythonTopics() });
    courses.push_back({ "C#", "Modern .NET: OOP, LINQ and async programming.",
                        Color{155,79,150,255}, BuildCSharpTopics() });
    courses.push_back({ "JavaScript", "The language of the web: DOM, async and modern ES6+.",
                        Color{230,180,0,255}, BuildJavaScriptTopics() });
    return courses;
}
