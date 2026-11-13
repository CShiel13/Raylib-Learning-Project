#pragma once

#include <string>
#include <vector>

// Curated code-completion prompts. Stable IDs are used by the spaced-review save file.
// Keep the answer set deliberately narrow: this is a recall drill, not a C++ interpreter.
// The code gap's expected shape, used to give precise instructions without
// showing the answer. This is presentation metadata, not part of grading.
enum class RecallAnswerKind { Statement, Condition, Expression, Value, LoopClause, Parameters, Lambda };

struct RecallCard {
    const char* id;
    int tier;
    const char* skill;
    const char* mission;
    const char* context;
    const char* hint;
    const char* explanation;
    std::vector<std::string> accepted;
    RecallAnswerKind answerKind;
    const char* example; // Observable result/requirement, never the answer itself.
};

class RecallDeck {
public:
    static const std::vector<RecallCard>& Cards() {
        static const std::vector<RecallCard> cards = {
            // Tier 1: basic expressions, control flow and simple functions.
            {"increase-score",1,"Variables","Increase score by one.","int score = 5;\n___", "An increment operator uses two plus signs.",
             "++score increments the integer. score++ and score += 1 also work here.", {"++score;", "score++;", "score += 1;", "score = score + 1;"}, RecallAnswerKind::Statement, "score starts at 5 and should become 6."},
            {"positive-lives",1,"Conditions","Enter the condition for having at least one life.","int lives = 3;\nif (___) play();", "Compare lives with zero using >.",
             "A positive count satisfies lives > 0.", {"lives > 0", "0 < lives", "lives >= 1", "1 <= lives"}, RecallAnswerKind::Condition, "1 life allows play; 0 lives does not."},
            {"cout-coins",1,"Output","Print the numeric value of coins.","int coins = 9;\n___", "Use std::cout and <<.",
             "std::cout << coins prints the integer without an automatic newline.", {"std::cout << coins;"}, RecallAnswerKind::Statement, "With coins = 9, the screen should show 9."},
            {"is-even",1,"Conditions","The gate should open only for even n.","int n = 8;\nif (___) unlock();", "Even integers have a remainder of zero after / 2.",
             "The remainder operator % checks whether n is divisible by 2.", {"n % 2 == 0", "0 == n % 2"}, RecallAnswerKind::Condition, "n = 8 unlocks; n = 7 stays locked."},
            {"loop-increment",1,"Loops","Complete the loop update to move i forward by one.","for (int i = 0; i < 3; ___) {\n    draw(i);\n}", "The update runs after each pass; either position of ++ is valid here.",
             "With an int loop counter, ++i and i++ both advance one step when the update's result is ignored.", {"++i", "i++", "i += 1"}, RecallAnswerKind::LoopClause, "draw(i) runs for i = 0, 1 and 2, then the loop ends."},
            {"start-zero",1,"Loops","Fill in the starting index of a zero-based loop.","for (int i = ___; i < 4; ++i) {\n    draw(i);\n}", "A zero-based index starts at zero.",
             "Starting at 0 visits positions 0, 1, 2 and 3.", {"0"}, RecallAnswerKind::Value, "The loop should visit positions 0, 1, 2, 3."},
            {"stop-before-four",1,"Loops","Visit indices 0, 1, 2 and 3 only.","for (int i = 0; ___; ++i) {\n    draw(i);\n}", "Compare i to 4 using <.",
             "i < 4 stops before index 4, avoiding an off-by-one error.", {"i < 4", "4 > i", "i <= 3", "3 >= i"}, RecallAnswerKind::Condition, "Include index 3, but never index 4."},
            {"return-forty-two",1,"Functions","Make answer() return the number 42.","int answer() {\n    ___\n}", "The return keyword sends a value to the caller.",
             "return 42; sends the integer back from the function.", {"return 42;"}, RecallAnswerKind::Statement, "Calling answer() should give back 42."},
            {"boolean-ready",1,"Variables","Initialise ready to the boolean true value.","bool ready = ___;", "C++ boolean literals are lowercase.",
             "true is the C++ boolean literal (not the string \"true\").", {"true"}, RecallAnswerKind::Value, "ready should be a true boolean, not text."},
            {"add-two",1,"Output","Print the sum of a and b.","int a = 2, b = 3;\nstd::cout << ___;", "Use the + operator between the variables.",
             "The expression a + b evaluates to 5 before std::cout prints it.", {"a + b", "b + a"}, RecallAnswerKind::Expression, "With a = 2 and b = 3, print 5."},
            {"exit-loop",1,"Loops","Stop this loop after doing the work once.","while (true) {\n    work();\n    ___\n}", "Use the keyword that exits a loop.",
             "break; immediately exits the nearest loop.", {"break;"}, RecallAnswerKind::Statement, "work() should run once, then the loop ends."},
            {"sum-accumulator",1,"Variables","Add the current i to the running total.","int total = 0;\nfor (int i = 1; i <= 3; ++i) {\n    ___\n}", "Use += for adding to an existing variable.",
             "total += i adds each loop value to the accumulator.", {"total += i;", "total = total + i;", "total = i + total;"}, RecallAnswerKind::Statement, "Add 1, then 2, then 3; the final total is 6."},
            {"countdown",1,"Loops","Make n move toward zero on each iteration.","while (n > 0) {\n    ___\n}", "Decrement is the opposite of increment.",
             "--n makes a positive n one smaller until the loop can stop.", {"--n;", "n--;", "n -= 1;", "n = n - 1;"}, RecallAnswerKind::Statement, "n gets smaller each pass until it reaches 0."},
            // Tier 2: combine conditions, containers and helper functions.
            {"print-original",2,"Output","Print the original value, then increment i by one.","int i = 4;\nstd::cout << ___;", "Use the increment form that supplies the old value to the surrounding expression.",
             "i++ prints 4 while changing i to 5. ++i would print 5 instead.", {"i++"}, RecallAnswerKind::Expression, "Print 4; afterward i must equal 5."},
            {"print-incremented",2,"Output","Increment i first, then print its updated value.","int i = 4;\nstd::cout << ___;", "Use the increment form that supplies the new value to the surrounding expression.",
             "++i increments i to 5, then prints 5. i++ would print 4.", {"++i"}, RecallAnswerKind::Expression, "Print 5; afterward i must equal 5."},
            {"inclusive-bound",2,"Conditions","Accept a score between 1 and 10, inclusive.","int score = 6;\nif (___) reward();", "Use && to require both bounds.",
             "Both lower and upper comparisons must be true.", {"score >= 1 && score <= 10", "score <= 10 && score >= 1", "1 <= score && score <= 10"}, RecallAnswerKind::Condition, "Scores 1 and 10 pass; 0 and 11 do not."},
            {"key-and-coins",2,"Conditions","The door needs a key AND at least 5 coins.","bool key = true;\nint coins = 7;\nif (___) open();", "Combine conditions with &&.",
             "&& opens the door only when both the key and coin requirements hold.", {"key && coins >= 5", "coins >= 5 && key", "key == true && coins >= 5", "coins >= 5 && key == true"}, RecallAnswerKind::Condition, "Require both key = true and coins >= 5."},
            {"key-or-pass",2,"Conditions","Allow entry with either a key OR a pass.","bool key = false, pass = true;\nif (___) enter();", "Use || for OR.",
             "|| requires at least one operand to be true.", {"key || pass", "pass || key"}, RecallAnswerKind::Condition, "Having either the key or a pass is enough."},
            {"vector-push",2,"Containers","Append 7 to the existing vector.","std::vector<int> values;\n___", "Use push_back(value) on the vector.",
             "push_back adds an element at the end of the vector.", {"values.push_back(7);"}, RecallAnswerKind::Statement, "values should contain the number 7 afterward."},
            {"vector-size",2,"Containers","Print the number of elements in values.","std::vector<int> values{2, 4, 6};\nstd::cout << ___;", "Vectors expose a size() member function.",
             "values.size() gives the current element count (3).", {"values.size()"}, RecallAnswerKind::Expression, "The shown vector holds 3 elements; print 3."},
            {"string-size",2,"Containers","Print how many characters are in name.","std::string name = \"Ada\";\nstd::cout << ___;", "The string type has a size() member.",
             "name.size() returns 3 for the string Ada.", {"name.size()", "name.length()"}, RecallAnswerKind::Expression, "The name Ada contains 3 characters; print 3."},
            {"return-sum",2,"Functions","Complete the function so it adds its arguments.","int add(int a, int b) {\n    ___\n}", "Return an expression using a and b.",
             "The expression is evaluated and returned to the caller.", {"return a + b;", "return b + a;"}, RecallAnswerKind::Statement, "add(2, 3) should return 5."},
            {"odd-check",2,"Conditions","Detect odd integers, including negative odd values.","int n = -5;\nif (___) oddCount++;", "An odd integer's remainder after / 2 is nonzero.",
             "n % 2 != 0 handles both positive and negative odd values in C++.", {"n % 2 != 0", "0 != n % 2"}, RecallAnswerKind::Condition, "Both -5 and 5 are odd; 4 is not."},
            {"not-equal",2,"Conditions","Run only when attempt differs from target.","int attempt = 2, target = 4;\nif (___) retry();", "Use != for not-equal.",
             "!= compares values for inequality; = would assign instead.", {"attempt != target", "target != attempt"}, RecallAnswerKind::Condition, "Retry for 2 vs 4, but not for 4 vs 4."},
            {"line-break",2,"Output","Print the word go followed by a newline.","___", "Use std::cout with a newline character literal.",
             "The \\n character starts a new line after go.", {"std::cout << \"go\\n\";", "std::cout << \"go\" << '\\n';"}, RecallAnswerKind::Statement, "Print go and then move to the next line."},
            {"skip-iteration",2,"Loops","Skip just this pass of the loop.","for (int i = 0; i < 5; ++i) {\n    if (i == 2) { ___ }\n    log(i);\n}", "The keyword differs from break: it continues the loop.",
             "continue; skips the remainder of the current iteration.", {"continue;"}, RecallAnswerKind::Statement, "Skip log(2), but still run later iterations."},
            // Tier 3: more advanced course topics.
            {"vector-bounds",3,"Containers","Access element i with bounds checking.","std::vector<int> values{1, 2, 3};\nint x = ___;", "Use the member named at rather than operator[].",
             "values.at(i) checks bounds and throws std::out_of_range for an invalid index.", {"values.at(i)"}, RecallAnswerKind::Expression, "Read element i safely; detect invalid indexes."},
            {"range-for",3,"Loops","Fill in a range-for loop to read each number.","std::vector<int> nums{1, 2, 3};\nfor (___) std::cout << x;", "Use int x : nums inside the parentheses.",
             "A range-for loop visits each element without manual indexing.", {"int x : nums", "const int x : nums", "const int& x : nums", "int const& x : nums"}, RecallAnswerKind::LoopClause, "Visit each number in nums using the name x."},
            {"reference-arg",3,"Functions","Pass a string without copying and prevent mutation.","void greet(___) {\n    std::cout << name;\n}", "Combine const, std::string and &.",
             "A const reference avoids a string copy while protecting it from modification.", {"const std::string& name", "std::string const& name"}, RecallAnswerKind::Parameters, "The name argument must be read-only, with no copy."},
            {"unique-ptr",3,"Memory","Create an owning smart pointer holding 7.","#include <memory>\n___", "Use std::make_unique<int>(7).",
             "std::make_unique creates and owns the int; no manual delete is needed.", {"auto p = std::make_unique<int>(7);", "std::unique_ptr<int> p = std::make_unique<int>(7);"}, RecallAnswerKind::Statement, "Create an owned integer with the value 7."},
            {"swap-two",3,"Algorithms","Swap the values of a and b.","#include <utility>\nint a = 2, b = 9;\n___", "The standard library has std::swap.",
             "std::swap exchanges the two values.", {"std::swap(a, b);", "std::swap(b, a);"}, RecallAnswerKind::Statement, "Afterward a is 9 and b is 2."},
            {"sort-vector",3,"Algorithms","Sort the vector in ascending order.","#include <algorithm>\nstd::vector<int> v{3, 1, 2};\n___", "Use std::sort with begin() and end().",
             "std::sort(v.begin(), v.end()) sorts the range in place.", {"std::sort(v.begin(), v.end());"}, RecallAnswerKind::Statement, "Afterward v contains 1, 2, 3 in that order."},
            {"conditional-group",3,"Conditions","Require: ready OR (hasKey AND enough coins).","bool ready = false, hasKey = true;\nint coins = 12;\nif (___) open();", "Parentheses must group the key-and-coins branch.",
             "&& binds more tightly than ||, but parentheses make the rule clear.", {"ready || (hasKey && coins >= 10)", "(hasKey && coins >= 10) || ready", "ready || hasKey && coins >= 10"}, RecallAnswerKind::Condition, "Ready is enough, OR key plus 10 or more coins."},
            {"lambda-add",3,"Functions","Define a lambda that doubles its int argument.","auto twice = ___;\nstd::cout << twice(3);", "A lambda has [capture](args) { body } syntax.",
             "The lambda returns n * 2, so twice(3) is 6.", {"[](int n) { return n * 2; }", "[](int n){return 2*n;}"}, RecallAnswerKind::Lambda, "Calling twice(3) should print 6."},
            {"const-auto-ref",3,"Containers","Read vector elements without copying or changing them.","std::vector<std::string> names;\nfor (___) show(name);", "Use const auto& with a range-for loop.",
             "const auto& binds each element by read-only reference.", {"const auto& name : names", "auto const& name : names"}, RecallAnswerKind::LoopClause, "Each name is read-only and not copied."},
            {"is-empty",3,"Containers","Run the branch only when values is empty.","std::vector<int> values;\nif (___) warn();", "Vectors have an empty() method.",
             "empty() checks whether the vector has zero elements.", {"values.empty()", "values.size() == 0", "0 == values.size()"}, RecallAnswerKind::Condition, "Run warn() when values has no elements."}
        };
        return cards;
    }
};
