#pragma once
// Additional C++ topic-test bosses. Each problem has at least one complete
// reference implementation. The current TextJudge does NOT execute code;
// these examples are learning references, not reported as passed unit tests.
#include <string>
#include <utility>
#include <vector>
#include "../../hard/HardRegistry.h"

namespace cpp_hard {
inline void RegisterMissingTopicBosses() {
    auto add = [](int topic, const char* title, const char* difficulty,
                  std::string statement, std::vector<HardExample> examples,
                  std::vector<std::string> constraints, std::string starter,
                  std::vector<std::string> references, std::vector<std::string> hints,
                  int xp, int coins) {
        HardProblem p;
        p.title = title; p.difficulty = difficulty;
        p.statement = std::move(statement); p.examples = std::move(examples);
        p.constraints = std::move(constraints); p.starter = std::move(starter);
        p.solutions = std::move(references); p.hints = std::move(hints);
        p.xp = xp; p.coins = coins;
        AddHard("C++", topic, -1, std::move(p));
    };

    add(0, "Hello, Terminal", "Easy",
        "Complete greet() so it prints Hello followed by a newline. Write only inside the provided function.",
        {{"greet()", "Hello\\n", "The newline is part of the output."}},
        {"Use std::cout", "Do not print additional spaces"},
        "#include <iostream>\n\nvoid greet() {\n    // your code here\n}\n",
        {"#include <iostream>\nvoid greet() { std::cout << \"Hello\\n\"; }",
         "#include <iostream>\nvoid greet() { std::cout << \"Hello\" << '\\n'; }",
         "#include <iostream>\nvoid greet() { std::cout << \"Hello\" << std::endl; }"},
        {"Use the output stream and << insertion operator.", "A newline may be written as \\n inside a string or as the character '\\n'."}, 35, 12);

    add(1, "Weeks to Days", "Easy",
        "Complete daysInWeeks() to convert a nonnegative whole number of weeks to days.",
        {{"daysInWeeks(0)", "0", ""}, {"daysInWeeks(3)", "21", ""}},
        {"0 <= weeks <= 1000"},
        "int daysInWeeks(int weeks) {\n    // your code here\n}\n",
        {"int daysInWeeks(int weeks) { return weeks * 7; }",
         "int daysInWeeks(int weeks) { return 7 * weeks; }",
         "int daysInWeeks(int weeks) { int days = weeks * 7; return days; }"},
        {"A week always contains seven days.", "Use the int parameter in an arithmetic expression returned by the function."}, 38, 14);

    add(2, "Print Two Scores", "Easy",
        "Complete printScores() so it prints the two values separated by a colon and ends with a newline.",
        {{"printScores(4, 9)", "4:9\\n", ""}, {"printScores(0, -2)", "0:-2\\n", ""}},
        {"No spaces around the colon"},
        "#include <iostream>\n\nvoid printScores(int a, int b) {\n    // your code here\n}\n",
        {"#include <iostream>\nvoid printScores(int a, int b) { std::cout << a << ':' << b << '\\n'; }",
         "#include <iostream>\nvoid printScores(int a, int b) { std::cout << a << \":\" << b << \"\\n\"; }",
         "#include <iostream>\nvoid printScores(int a, int b) { std::cout << a << \":\" << b << std::endl; }"},
        {"Use several << operators in the same statement.", "A colon can be a character literal ':' or a string literal \":\"."}, 40, 15);

    add(8, "Count Vowels", "Medium",
        "Complete countVowels() to count lowercase English vowels (a, e, i, o, u) in a string.",
        {{"countVowels(\"code\")", "2", ""}, {"countVowels(\"xyz\")", "0", ""},
         {"countVowels(\"queue\")", "4", "Repeated vowels count."}},
        {"Input contains only lowercase a-z", "Empty strings are allowed"},
        "#include <string>\n\nint countVowels(const std::string& s) {\n    // your code here\n}\n",
        {"#include <string>\nint countVowels(const std::string& s) { int count = 0; for (char c : s) { if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') { ++count; } } return count; }",
         "#include <string>\nint countVowels(const std::string& s) { int n = 0; for (char c : s) { if (std::string(\"aeiou\").find(c) != std::string::npos) { ++n; } } return n; }"},
        {"Loop through each character in the string.", "Check whether each character belongs to the set a, e, i, o, u."}, 65, 28);

    add(10, "Sorted Copy", "Medium",
        "Complete sortedCopy() to return a sorted copy of a vector in ascending order. The caller's vector must not change.",
        {{"sortedCopy({3, 1, 2})", "{1, 2, 3}", ""}, {"sortedCopy({})", "{}", ""},
         {"sortedCopy({5, 5})", "{5, 5}", ""}},
        {"0 <= v.size() <= 100000", "Use std::sort"},
        "#include <vector>\n#include <algorithm>\n\nstd::vector<int> sortedCopy(std::vector<int> v) {\n    // your code here\n}\n",
        {"#include <vector>\n#include <algorithm>\nstd::vector<int> sortedCopy(std::vector<int> v) { std::sort(v.begin(), v.end()); return v; }"},
        {"The parameter is passed by value, so it is already a copy.", "std::sort accepts an iterator pair: begin() and end()."}, 65, 28);

    add(11, "Rectangle Class", "Medium",
        "Implement Rectangle with public int width and height, and a const area() member returning their product.",
        {{"Rectangle{3, 4}.area()", "12", ""}, {"Rectangle{0, 8}.area()", "0", ""}},
        {"Keep members public", "area() must be const"},
        "// Write a struct named Rectangle here.\n",
        {"struct Rectangle { int width; int height; int area() const { return width * height; } };",
         "struct Rectangle { int width, height; int area() const { return width * height; } };"},
        {"A struct has public members by default.", "A const member function cannot modify non-mutable fields."}, 70, 30);

    add(12, "Scope Guard", "Hard",
        "Implement CounterGuard: construction increments an external int counter; destruction decrements it. Store a reference so both operations affect the original counter.",
        {{"int c=0; { CounterGuard g(c); }", "c == 0", "The destructor restores the counter."},
         {"int c=4; CounterGuard g(c);", "c == 5 while g lives", ""}},
        {"No owning heap allocation", "Use RAII and a destructor"},
        "// Write struct CounterGuard here.\n",
        {"struct CounterGuard { int& count; CounterGuard(int& c) : count(c) { ++count; } ~CounterGuard() { --count; } };",
         "class CounterGuard { int& count; public: CounterGuard(int& c) : count(c) { ++count; } ~CounterGuard() { --count; } };"},
        {"Store an int& member and initialize it in the constructor initializer list.", "A destructor named ~CounterGuard runs automatically at the end of the object's lifetime."}, 80, 35);

    add(13, "Unique Score", "Medium",
        "Complete makeScore() to return a unique_ptr<int> owning a newly created integer equal to value.",
        {{"*makeScore(42)", "42", ""}, {"*makeScore(-5)", "-5", ""}},
        {"Use std::make_unique", "Do not return a pointer to a local variable"},
        "#include <memory>\n\nstd::unique_ptr<int> makeScore(int value) {\n    // your code here\n}\n",
        {"#include <memory>\nstd::unique_ptr<int> makeScore(int value) { return std::make_unique<int>(value); }",
         "#include <memory>\nstd::unique_ptr<int> makeScore(int value) { auto p = std::make_unique<int>(value); return p; }"},
        {"unique_ptr expresses exclusive ownership.", "std::make_unique<int>(value) creates both the integer and its owner."}, 70, 30);

    add(14, "Virtual Speak", "Hard",
        "Complete Dog so that it publicly inherits Animal and overrides the virtual speak() method to return 1.",
        {{"Dog d; d.speak()", "1", ""}, {"Animal& a = d; a.speak()", "1", "Dynamic dispatch."}},
        {"Keep the base declaration unchanged", "Mark the override"},
        "struct Animal { virtual int speak() const = 0; virtual ~Animal() = default; };\n\n// Write Dog here.\n",
        {"struct Animal { virtual int speak() const = 0; virtual ~Animal() = default; };\nstruct Dog : public Animal { int speak() const override { return 1; } };",
         "struct Animal { virtual int speak() const = 0; virtual ~Animal() = default; };\nclass Dog : public Animal { public: int speak() const override { return 1; } };"},
        {"Dog must inherit publicly so it can be treated as an Animal.", "Keep the const qualifier and add override to match the virtual base method."}, 85, 36);

    add(15, "Generic Maximum", "Hard",
        "Implement a C++ function template maximum(T a, T b) that returns the larger value. Assume T supports operator<.",
        {{"maximum(2, 5)", "5", ""}, {"maximum(-2, -7)", "-2", ""}},
        {"Do not use a concrete type such as int", "Supports comparable numeric types"},
        "// Write a function template maximum below.\n",
        {"template <typename T> T maximum(T a, T b) { return a < b ? b : a; }",
         "template <typename T> T maximum(T a, T b) { if (a < b) return b; return a; }",
         "template <class T> T maximum(T a, T b) { return a < b ? b : a; }"},
        {"Start with template <typename T>.", "Compare a and b; return b when a < b, otherwise a."}, 85, 36);

    add(16, "Safe Vector Read", "Hard",
        "Complete readOrMinusOne() to read v.at(index). Return -1 if std::out_of_range is thrown.",
        {{"readOrMinusOne({8, 9}, 1)", "9", ""}, {"readOrMinusOne({8}, 2)", "-1", ""}},
        {"Catch std::out_of_range by const reference", "Negative indices are not used"},
        "#include <vector>\n#include <stdexcept>\n\nint readOrMinusOne(const std::vector<int>& v, std::size_t index) {\n    // your code here\n}\n",
        {"#include <vector>\n#include <stdexcept>\nint readOrMinusOne(const std::vector<int>& v, std::size_t index) { try { return v.at(index); } catch (const std::out_of_range&) { return -1; } }",
         "#include <vector>\n#include <stdexcept>\nint readOrMinusOne(const std::vector<int>& v, std::size_t index) { try { return v.at(index); } catch (const std::out_of_range& e) { return -1; } }"},
        {"v.at() checks bounds and throws std::out_of_range.", "Use try { ... } catch (const std::out_of_range&) { ... }."}, 88, 38);

    add(17, "First Even", "Hard",
        "Complete firstEven() to return an optional holding the first even number in v, or std::nullopt if there is none.",
        {{"firstEven({3, 8, 2})", "8", ""}, {"firstEven({1, 3})", "std::nullopt", ""},
         {"firstEven({})", "std::nullopt", ""}},
        {"Use std::optional<int>", "Order matters: return the first even element"},
        "#include <optional>\n#include <vector>\n\nstd::optional<int> firstEven(const std::vector<int>& v) {\n    // your code here\n}\n",
        {"#include <optional>\n#include <vector>\nstd::optional<int> firstEven(const std::vector<int>& v) { for (int x : v) { if (x % 2 == 0) return x; } return std::nullopt; }",
         "#include <optional>\n#include <vector>\nstd::optional<int> firstEven(const std::vector<int>& v) { for (const auto& x : v) { if (x % 2 == 0) { return x; } } return std::nullopt; }"},
        {"Use a range-for loop and return when you first find an even value.", "When the loop finishes without finding one, return std::nullopt."}, 95, 40);
}
// Lighter, module-specific coding checkpoints. Unlike topic-test bosses,
// these are deliberately short syntax-retrieval challenges based on the
// module's own learning objective, not purportedly executable LeetCode tests.
inline void RegisterModuleCheckpoints(const std::vector<Topic>& topics) {
    for (int t = 0; t < (int)topics.size(); ++t) {
        const auto& topic = topics[t];
        for (int m = 0; m < (int)topic.modules.size(); ++m) {
            if (FindHard("C++", t, m)) continue;
            const Module& mod = topic.modules[m];
            const CodeTask* task = mod.hasCode && !mod.code.accepted.empty() ? &mod.code : nullptr;
            // Only two introductory modules lack a code task. Revisit the nearest
            // module's concrete C++ objective instead of inventing invalid code.
            if (!task) {
                for (int offset = 1; offset < (int)topic.modules.size(); ++offset) {
                    int left = m - offset, right = m + offset;
                    if (left >= 0 && topic.modules[left].hasCode && !topic.modules[left].code.accepted.empty()) {
                        task = &topic.modules[left].code; break;
                    }
                    if (right < (int)topic.modules.size() && topic.modules[right].hasCode && !topic.modules[right].code.accepted.empty()) {
                        task = &topic.modules[right].code; break;
                    }
                }
            }
            if (!task) continue;
            HardProblem p;
            p.title = mod.title + " - Code Checkpoint";
            p.difficulty = t < 6 ? "Easy" : (t < 12 ? "Medium" : "Hard");
            p.statement = "Write the requested C++ snippet from memory: " + task->prompt;
            p.constraints = {"Write only the requested snippet; do not add a main() wrapper.",
                             "This is a syntax checkpoint, not an executable program."};
            p.starter = "// Write the requested C++ snippet below.\n";
            p.solutions = task->accepted;
            p.hints = {task->hint, "Review the module's worked example, then try again from memory."};
            p.xp = 18 + std::min(t / 5, 3) * 4;
            p.coins = 6 + std::min(t / 5, 3) * 2;
            AddHard("C++", t, m, std::move(p));
        }
    }
}
} // namespace cpp_hard
