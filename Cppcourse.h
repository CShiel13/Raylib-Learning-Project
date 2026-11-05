#pragma once
// CppCourse.h - assembles the full C++ course (18 topics) and registers its hard-mode problems.
// Game calls InstallCppCourse() once; it replaces the placeholder "C++" course from Content.h.
#include "CppPart3.h"
#include "Hard.h"

inline std::vector<Topic> BuildCppCourseTopics() {
    std::vector<Topic> all = CppTopicsA();
    for (auto& t : CppTopicsB()) all.push_back(std::move(t));
    for (auto& t : CppTopicsC()) all.push_back(std::move(t));
    return all;
}

// Boss problems, one per topic test (module index -1). Judged by TextJudge: whitespace and comments are
// ignored, but the code must match one of the reference solutions, so each lists the common variants.
inline void BuildCppHard() {
    static bool done = false; if (done) return; done = true;
    auto add = [](int topic, HardProblem p) { AddHard("C++", topic, -1, std::move(p)); };

    {
        HardProblem p; p.title = "Is Even"; p.difficulty = "Easy"; p.xp = 40; p.coins = 15;
        p.statement = "Complete isEven so it returns true when n is even and false when n is odd.";
        p.examples = { {"isEven(4)", "true", ""}, {"isEven(7)", "false", ""}, {"isEven(0)", "true", "Zero is even."} };
        p.constraints = { "0 <= n <= 1,000,000" };
        p.starter = "#include <iostream>\n\nbool isEven(int n) {\n    // your code here\n}\n";
        p.solutions = { "#include <iostream>\nbool isEven(int n) { return n % 2 == 0; }",
                       "#include <iostream>\nbool isEven(int n) { return (n % 2) == 0; }",
                       "#include <iostream>\nbool isEven(int n) { return !(n % 2); }" };
        p.mustContain = { "return", "%" };
        p.hints = { "The % operator gives a remainder. What is the remainder of an even number divided by 2?", "A comparison already produces a bool: return it directly." };
        add(3, p);
    }

    {
        HardProblem p; p.title = "Largest of Three"; p.difficulty = "Easy"; p.xp = 45; p.coins = 18;
        p.statement = "Complete largest so it returns the biggest of the three ints a, b and c.";
        p.examples = { {"largest(1, 5, 3)", "5", ""}, {"largest(9, 2, 9)", "9", "Ties are fine."}, {"largest(-4, -8, -1)", "-1", "Works with negatives."} };
        p.constraints = { "-10^9 <= a, b, c <= 10^9" };
        p.starter = "#include <algorithm>\n\nint largest(int a, int b, int c) {\n    // your code here\n}\n";
        p.solutions = { "#include <algorithm>\nint largest(int a, int b, int c) { int m = a; if (b > m) { m = b; } if (c > m) { m = c; } return m; }",
                       "#include <algorithm>\nint largest(int a, int b, int c) { return std::max(a, std::max(b, c)); }",
                       "#include <algorithm>\nint largest(int a, int b, int c) { if (a >= b && a >= c) { return a; } if (b >= c) { return b; } return c; }" };
        p.mustContain = { "return" };
        p.hints = { "Keep a variable holding the best value so far, then compare it with each of the others.", "std::max(x, y) is in <algorithm> and returns the larger of two values." };
        add(4, p);
    }

    {
        HardProblem p; p.title = "Sum to N"; p.difficulty = "Easy"; p.xp = 50; p.coins = 20;
        p.statement = "Complete sumTo so it returns 1 + 2 + ... + n. If n is 0 the answer is 0.";
        p.examples = { {"sumTo(4)", "10", "1 + 2 + 3 + 4"}, {"sumTo(1)", "1", ""}, {"sumTo(0)", "0", ""} };
        p.constraints = { "0 <= n <= 10,000" };
        p.starter = "int sumTo(int n) {\n    // your code here\n}\n";
        p.solutions = { "int sumTo(int n) { int total = 0; for (int i = 1; i <= n; ++i) { total += i; } return total; }",
                       "int sumTo(int n) { int total = 0; for (int i = 1; i <= n; i++) { total += i; } return total; }",
                       "int sumTo(int n) { int total = 0; int i = 1; while (i <= n) { total += i; ++i; } return total; }",
                       "int sumTo(int n) { return n * (n + 1) / 2; }" };
        p.mustContain = { "return" };
        p.hints = { "Start a total at 0 and add each number from 1 up to n.", "There is also a closed formula: n * (n + 1) / 2." };
        add(5, p);
    }

    {
        HardProblem p; p.title = "Factorial"; p.difficulty = "Medium"; p.xp = 60; p.coins = 25;
        p.statement = "Complete factorial so it returns n! (the product 1 * 2 * ... * n). factorial(0) is 1.";
        p.examples = { {"factorial(5)", "120", ""}, {"factorial(1)", "1", ""}, {"factorial(0)", "1", "The empty product is 1."} };
        p.constraints = { "0 <= n <= 20 (fits in long long)" };
        p.starter = "long long factorial(int n) {\n    // your code here\n}\n";
        p.solutions = { "long long factorial(int n) { long long result = 1; for (int i = 2; i <= n; ++i) { result *= i; } return result; }",
                       "long long factorial(int n) { long long result = 1; for (int i = 2; i <= n; i++) { result *= i; } return result; }",
                       "long long factorial(int n) { if (n <= 1) { return 1; } return n * factorial(n - 1); }" };
        p.mustContain = { "return" };
        p.hints = { "Multiply a running result by 2, 3, ... up to n. Start it at 1, not 0.", "Recursive idea: n! = n * (n - 1)!, with a base case for n <= 1." };
        add(6, p);
    }

    {
        HardProblem p; p.title = "Swap Two"; p.difficulty = "Easy"; p.xp = 50; p.coins = 20;
        p.statement = "Complete swapValues so that after the call, a holds b's old value and b holds a's old value. The parameters are references, so the caller's variables change.";
        p.examples = { {"a = 1, b = 2;  swapValues(a, b);", "a = 2, b = 1", ""}, {"a = 7, b = 7;  swapValues(a, b);", "a = 7, b = 7", ""} };
        p.constraints = { "Do not return anything" };
        p.starter = "#include <utility>\n\nvoid swapValues(int& a, int& b) {\n    // your code here\n}\n";
        p.solutions = { "#include <utility>\nvoid swapValues(int& a, int& b) { int temp = a; a = b; b = temp; }",
                       "#include <utility>\nvoid swapValues(int& a, int& b) { std::swap(a, b); }" };
        p.mustContain = { "a", "b" };
        p.hints = { "Save one value in a temporary variable before you overwrite it.", "std::swap(a, b) from <utility> does it in one line." };
        add(7, p);
    }

    {
        HardProblem p; p.title = "Sum of a Vector"; p.difficulty = "Medium"; p.xp = 60; p.coins = 25;
        p.statement = "Complete sumVector so it returns the sum of all elements of v. An empty vector sums to 0.";
        p.examples = { {"sumVector({1, 2, 3})", "6", ""}, {"sumVector({})", "0", "Empty vector."}, {"sumVector({-2, 5})", "3", ""} };
        p.constraints = { "0 <= v.size() <= 100,000", "The sum fits in an int" };
        p.starter = "#include <vector>\n#include <numeric>\n\nint sumVector(const std::vector<int>& v) {\n    // your code here\n}\n";
        p.solutions = { "#include <vector>\n#include <numeric>\nint sumVector(const std::vector<int>& v) { int total = 0; for (int x : v) { total += x; } return total; }",
                       "#include <vector>\n#include <numeric>\nint sumVector(const std::vector<int>& v) { int total = 0; for (const auto& x : v) { total += x; } return total; }",
                       "#include <vector>\n#include <numeric>\nint sumVector(const std::vector<int>& v) { return std::accumulate(v.begin(), v.end(), 0); }" };
        p.mustContain = { "return" };
        p.hints = { "Loop over every element and add it to a running total that starts at 0.", "std::accumulate(v.begin(), v.end(), 0) from <numeric> does exactly this." };
        add(9, p);
    }
}

// Replaces the placeholder C++ course in `courses` with the full one.
inline void InstallCppCourse(std::vector<Course>& courses) {
    BuildCppHard();
    for (auto& c : courses) {
        if (c.name != "C++") continue;
        c.topics = BuildCppCourseTopics();
        c.tagline = "The full course: from your first program to templates, RAII and modern C++.";
        c.color = Color{ 62, 224, 176, 255 };
    }
}