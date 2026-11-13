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
             "The loop condition is n > 0, so n must decrease each iteration to eventually become zero."},
            {1, "The total should be the sum of 1, 2 and 3. What change fixes the code?",
             "int total = 0;\nfor (int i = 1; i <= 3; ++i)\n    total = i;",
             {"Use total += i", "Start total at 1", "Use i--", "Change total to bool"}, 0,
             "total = i replaces the current total. total += i adds each value instead."},
            {1, "The message should appear only when the password matches. What is wrong?",
             "int password = 4;\nif (password != 4)\n    std::cout << \"Welcome\";",
             {"Replace != with ==", "Replace != with =", "Change password to 0", "Remove the if"}, 0,
             "!= means not equal. == checks whether the password matches."},
            {2, "This should print the last item in the array without reading past it.",
             "int numbers[3] = {7, 8, 9};\nstd::cout << numbers[3];",
             {"Use numbers[2]", "Use numbers[4]", "Use numbers[-1]", "Use numbers[0]"}, 0,
             "The three elements have indices 0, 1, and 2. The last valid index is 2."},
            {2, "The button should activate only when ready is true. Choose the fix.",
             "bool ready = false;\nif (ready = true)\n    activate();",
             {"Use if (ready)", "Use if (!ready)", "Use if (ready = false)", "Remove the if"}, 0,
             "Assignment sets ready to true. Testing if (ready) reads its current value."},
            {3, "The function computes double a number, but does not return it. Fix it.",
             "int doubled(int n) {\n    int result = n * 2;\n}",
             {"Add return result;", "Change int to bool", "Add result++;", "Remove n * 2"}, 0,
             "An int-returning function must return the computed value."},
            {3, "The average of two ints should keep halves such as 2.5. What change works?",
             "double average(int a, int b) {\n    return (a + b) / 2;\n}",
             {"Divide by 2.0 instead of 2", "Change double to int", "Divide by 1", "Remove return"}, 0,
             "Dividing by int 2 uses integer division first. 2.0 makes the division floating-point."},
            {1, "A classmate says i++ is invalid in a for-loop update. What is the correct fix?",
             "for (int i = 0; i < 3; i++)\n    std::cout << i;",
             {"No fix needed: i++ is valid here", "Replace i++ with i--", "Only ++i is legal C++", "Use i + 1 without assigning"}, 0,
             "Both i++ and ++i are valid on an int. Because a for-loop update discards its expression's value, both visit 0, 1 and 2."},
            {3, "A function should return the old count and then increment it. Which change fixes the code?",
             "int nextTicket(int& count) {\n    return ++count;\n}",
             {"Change ++count to count++", "Change ++count to --count", "Return count without incrementing", "Change int to void"}, 0,
             "Postfix count++ gives the caller the previous value while increasing count. Prefix ++count would return the new value."}
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
             "The loop decreases n from 5 to 4, 3, then 2. It stops when n > 2 becomes false."},
            {1, "What value is printed after multiplying x by itself?",
             "int x = 4;\nstd::cout << x * x;",
             {"16", "8", "44", "4"}, 0,
             "4 multiplied by 4 is 16."},
            {1, "What does the subtraction print?",
             "int a = 9;\nstd::cout << a - 4;",
             {"5", "13", "94", "9"}, 0,
             "a starts at 9, and 9 minus 4 is 5."},
            {2, "What is printed after updating n?",
             "int n = 4;\nn += 3;\nstd::cout << n;",
             {"7", "4", "3", "43"}, 0,
             "The += operator adds 3 to the stored value, so n becomes 7."},
            {2, "What characters are printed by this loop, with no spaces?",
             "for (int i = 0; i < 4; i += 2)\n    std::cout << i;",
             {"02", "0123", "024", "04"}, 0,
             "The loop prints 0 then 2. Adding 2 makes i equal 4, so the loop stops."},
            {3, "What does the accumulator print after the loop?",
             "int total = 0;\nfor (int i = 0; i < 4; ++i)\n    total += i;\nstd::cout << total;",
             {"6", "4", "10", "3"}, 0,
             "The loop adds 0 + 1 + 2 + 3, which is 6."},
            {3, "What number does the conditional expression print?",
             "int a = 2, b = 3;\nstd::cout << (a > b ? a : b);",
             {"3", "2", "1", "0"}, 0,
             "a > b is false, so the expression chooses b, which is 3."},
            {1, "What is printed, before the variable changes?",
             "int i = 4;\nstd::cout << i++;",
             {"4", "5", "45", "A compiler error"}, 0,
             "i++ yields the old value (4) for printing, then changes i to 5."},
            {2, "What is printed when prefix increment is used?",
             "int i = 4;\nstd::cout << ++i;",
             {"5", "4", "45", "A compiler error"}, 0,
             "++i updates i to 5 first and yields that updated value to std::cout."},
            {3, "What does this sequence print, separated by one space?",
             "int i = 3;\nint oldValue = i++;\nint newValue = ++i;\nstd::cout << oldValue << " " << newValue;",
             {"3 5", "4 5", "3 4", "5 5"}, 0,
             "oldValue gets 3, then i becomes 4. ++i makes i 5 and newValue gets 5."}
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
             "The parentheses group the key-and-coins requirement. Being ready alone is also enough."},
            {1, "Unlock only when points has reached at least 10.",
             "int points = 10;\nif ( ??? ) unlock();",
             {"points >= 10", "points > 10", "points < 10", "points = 10"}, 0,
             ">= includes 10 itself, whereas > would require more than 10."},
            {1, "The chest should open only if the player has a key.",
             "bool hasKey = true;\nif ( ??? ) openChest();",
             {"hasKey", "!hasKey", "hasKey = false", "false"}, 0,
             "A bool can be used directly as a condition: hasKey is true when the key is held."},
            {2, "The gate requires an adult who has signed the form. Choose the condition.",
             "int age = 18;\nbool signedForm = true;\nif ( ??? ) enter();",
             {"age >= 18 && signedForm", "age > 18 || signedForm", "age < 18 && signedForm", "age >= 18 || signedForm"}, 0,
             "&& requires both being at least 18 and having signed the form."},
            {2, "The alarm sounds when the temperature is at or below freezing.",
             "int temp = 0;\nif ( ??? ) alarm();",
             {"temp <= 0", "temp < 0", "temp >= 0", "temp == 1"}, 0,
             "<= includes 0 and any colder temperature."},
            {3, "Unlock for even values from 10 through 20, including both limits.",
             "int n = 20;\nif ( ??? ) unlock();",
             {"n >= 10 && n <= 20 && n % 2 == 0", "n > 10 && n < 20 && n % 2 == 0", "n >= 10 || n <= 20", "n % 2 == 1 && n <= 20"}, 0,
             "All three requirements must hold: the two inclusive bounds and even remainder."},
            {3, "Access is allowed to admins or owners, but never while locked.",
             "bool admin = false, owner = true, locked = false;\nif ( ??? ) enter();",
             {"(admin || owner) && !locked", "admin && owner && locked", "admin || owner || locked", "admin && !owner"}, 0,
             "An admin or owner is allowed, but && !locked prevents access while locked."}
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
