#pragma once

#include <array>
#include <string>
#include <vector>

// Content-only model. Kept free of raylib so question data can be tested alone.
enum class PracticeMiniGameKind { BugHunt, OutputOracle, LogicLocks };

struct PracticeQuestion {
    int difficulty = 1;
    std::string prompt;
    std::string code;
    std::array<std::string, 4> choices;
    int answer = 0;
    std::string explanation;
};

class PracticeQuestionBank {
public:
    static const std::vector<PracticeQuestion>& For(PracticeMiniGameKind kind) {
        static const std::vector<PracticeQuestion> bugHunt = {
            {1, "This loop reads past the end of the array. Which fix is correct?",
             "int values[5] = {1, 2, 3, 4, 5};\nfor (int i = 0; i <= 5; ++i)\n    std::cout << values[i];",
             {"Change <= to <", "Change i to 1", "Change ++i to --i", "Remove the loop condition"}, 0,
             "Array indexes run from 0 to 4. Using i < 5 visits all five elements without reading index 5."},
            {1, "The program should compare score with 10. What is the bug fix?",
             "int score = 7;\nif (score = 10) {\n    std::cout << \"Perfect\";\n}",
             {"Use score == 10", "Use score >= 10", "Use score != 10", "Remove score"}, 0,
             "A single = assigns a value. The == operator compares two values."},
            {2, "The total should start at zero before values are added. Choose the fix.",
             "int total;\nfor (int i = 1; i <= 4; ++i)\n    total += i;",
             {"int total = 0;", "int total = 1;", "int total == 0;", "total = nullptr;"}, 0,
             "A local int without an initializer has an indeterminate value. Start the accumulator at 0."},
            {2, "The loop should add the numbers 1 through 10. Which header is right?",
             "int sum = 0;\nfor (int i = 0; i < 10; ++i)\n    sum += i;",
             {"for (int i = 1; i <= 10; ++i)", "for (int i = 1; i < 10; ++i)", "for (int i = 0; i <= 10; ++i)", "for (int i = 10; i < 1; ++i)"}, 0,
             "Start at 1 and use <= 10 so both endpoints are included."},
            {3, "A function must return the larger value. What should replace the condition?",
             "int largest(int a, int b) {\n    if (a < b) return a;\n    return b;\n}",
             {"Change a < b to a > b", "Change return a to return 0", "Change int to void", "Remove the if"}, 0,
             "When a is greater than b, a is the largest. Otherwise b is returned."},
            {3, "This loop never finishes for positive n. Which fix makes progress toward zero?",
             "while (n > 0) {\n    ++n;\n}",
             {"Change ++n to --n", "Change > to <", "Remove n", "Change while to if"}, 0,
             "The loop condition is n > 0, so n must decrease each iteration to eventually become zero."}
        };

        static const std::vector<PracticeQuestion> outputOracle = {
            {1, "What does this program print?",
             "int x = 3;\nstd::cout << x + 2;",
             {"5", "32", "6", "3"}, 0,
             "The + operator adds the integers: 3 + 2 is 5."},
            {1, "What is printed by this loop?",
             "for (int i = 1; i <= 3; ++i)\n    std::cout << i;",
             {"123", "0123", "321", "111"}, 0,
             "The loop visits 1, then 2, then 3. There are no spaces or newlines between them."},
            {2, "What is the exact output?",
             "int x = 3;\nstd::cout << x << \" \";\n++x;\nstd::cout << x;",
             {"3 4", "4 4", "3 3", "4 3"}, 0,
             "The first output reads x while it is 3. Then ++x increases it to 4 for the second output."},
            {2, "Which word is printed?",
             "int n = 7;\nif (n % 2 == 0)\n    std::cout << \"even\";\nelse\n    std::cout << \"odd\";",
             {"odd", "even", "7", "0"}, 0,
             "7 % 2 is 1, so the even condition is false and the else branch runs."},
            {3, "What number is printed after the nested loops finish?",
             "int count = 0;\nfor (int r = 0; r < 2; ++r)\n    for (int c = 0; c < 3; ++c)\n        ++count;\nstd::cout << count;",
             {"6", "5", "3", "2"}, 0,
             "The inner loop runs 3 times for each of 2 outer-loop passes: 2 × 3 = 6."},
            {3, "What is printed?",
             "int n = 5;\nwhile (n > 2) {\n    --n;\n}\nstd::cout << n;",
             {"2", "3", "1", "5"}, 0,
             "The loop decreases n from 5 to 4, 3, then 2. It stops when n > 2 becomes false."}
        };

        static const std::vector<PracticeQuestion> logicLocks = {
            {1, "Choose the condition that unlocks only when n is even.",
             "int n = 12;\nif ( ??? )\n    unlock();",
             {"n % 2 == 0", "n / 2 == 0", "n % 2 == 1", "n > 2"}, 0,
             "An even integer has a remainder of 0 when divided by 2."},
            {1, "The door opens when lives is positive. Which condition works?",
             "int lives = 3;\nif ( ??? )\n    openDoor();",
             {"lives > 0", "lives < 0", "lives == 0", "lives = 0"}, 0,
             "The comparison lives > 0 is true whenever at least one life remains."},
            {2, "Unlock only when score is at least 50 AND the player is alive.",
             "int score = 65;\nbool alive = true;\nif ( ??? ) unlock();",
             {"score >= 50 && alive", "score >= 50 || alive", "score < 50 && alive", "!alive"}, 0,
             "&& requires both conditions to be true: a qualifying score and alive == true."},
            {2, "Choose a condition that accepts x from 1 through 10, including both ends.",
             "int x = 7;\nif ( ??? )\n    collect();",
             {"x >= 1 && x <= 10", "x > 1 && x < 10", "x <= 1 && x >= 10", "x >= 1 || x <= 10"}, 0,
             "Use >= and <= to include 1 and 10. && keeps the value inside both bounds."},
            {3, "The switch opens for positive multiples of 3. Choose the exact condition.",
             "int n = 18;\nif ( ??? )\n    open();",
             {"n > 0 && n % 3 == 0", "n > 0 || n % 3 == 0", "n < 0 && n % 3 == 0", "n % 3 == 1"}, 0,
             "Both parts matter: n must be positive and its remainder when divided by 3 must be zero."},
            {3, "The bonus applies when the player is ready OR has a key and enough coins. Pick the expression.",
             "bool ready = false, hasKey = true;\nint coins = 12;\nif ( ??? ) bonus();",
             {"ready || (hasKey && coins >= 10)", "ready && hasKey && coins >= 10", "ready || hasKey || coins < 10", "!ready && !hasKey"}, 0,
             "The parentheses group the key-and-coins requirement. Being ready alone is also enough."}
        };

        switch (kind) {
        case PracticeMiniGameKind::BugHunt: return bugHunt;
        case PracticeMiniGameKind::OutputOracle: return outputOracle;
        default: return logicLocks;
        }
    }

    static const char* Title(PracticeMiniGameKind kind) {
        switch (kind) {
        case PracticeMiniGameKind::BugHunt: return "Bug Hunt";
        case PracticeMiniGameKind::OutputOracle: return "Output Oracle";
        default: return "Logic Locks";
        }
    }

    static const char* Subtitle(PracticeMiniGameKind kind) {
        switch (kind) {
        case PracticeMiniGameKind::BugHunt: return "Find the mistake. Pick the cleanest fix.";
        case PracticeMiniGameKind::OutputOracle: return "Trace the code. Predict exactly what it prints.";
        default: return "Combine comparisons and boolean operators to open each lock.";
        }
    }

    static const char* AnswerInstruction(PracticeMiniGameKind kind) {
        switch (kind) {
        case PracticeMiniGameKind::BugHunt: return "Select the change that fixes the bug.";
        case PracticeMiniGameKind::OutputOracle: return "Select exactly what the program prints.";
        default: return "Select the condition to put in the highlighted gap.";
        }
    }

    static const char* SnippetInstruction(PracticeMiniGameKind kind) {
        switch (kind) {
        case PracticeMiniGameKind::BugHunt: return "C++ snippet - look for the faulty line";
        case PracticeMiniGameKind::OutputOracle: return "C++ snippet - follow the statements in order";
        default: return "C++ snippet - the highlighted gap needs a condition";
        }
    }

    static const char* PromptLabel(PracticeMiniGameKind kind) {
        switch (kind) {
        case PracticeMiniGameKind::BugHunt: return "Choose the fix";
        case PracticeMiniGameKind::OutputOracle: return "Predict the output";
        default: return "Choose the condition";
        }
    }
};
