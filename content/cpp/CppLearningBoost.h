#pragma once
// C++ pedagogy layer. Content stays separate from raylib screens and progress.
// The lesson cards invite prediction BEFORE showing a model explanation;
// topic tests include prior-topic retrieval to encourage transfer.
#include "CppCourseSupport.h"
#include <array>
#include <stdexcept>

namespace cpp_learning {

struct Prediction {
    const char* title;
    const char* setup;
    const char* code;
    const char* question;
    const char* explanation;
};

// One fresh prediction/self-explanation task per topic. Unlike examples that simply
// display code, the answer remains hidden until the learner presses Reveal.
inline constexpr std::array<Prediction, 18> predictions{{
    {"Predict: program entry", "Look at this complete program. Nothing is printed.",
     "int main() { return 0; }", "Does the program succeed, and why is there no output?",
     "It succeeds. main returns 0 to the operating system; return does not print anything. Only an output operation such as std::cout would display text."},
    {"Predict: assignment", "Track the value of the variable after each statement.",
     "int points{4}; points = points + 3;", "What is points now? Would int points{3.5}; compile?",
     "points is 7: the new value replaces 4. Brace initialization from a fractional double to int is a narrowing conversion, so int points{3.5}; is rejected."},
    {"Predict: input and output", "Trace the two values sent to the output stream.",
     "std::cout << 4 << 2;", "Does this display 42, 6 or 4 2? Explain.",
     "It displays 42. Each insertion prints the value without automatically adding a space. To print a space you must insert one explicitly, for example << ' ' <<."},
    {"Predict: integer arithmetic", "C++ uses integer division for two integer operands.",
     "int r = 7 / 2; int s = 7 % 2;", "What values are stored in r and s?",
     "r is 3 and s is 1. Integer division discards the fractional part; the remainder operator % gives what is left. For a fractional quotient, use a floating-point operand."},
    {"Predict: short-circuit", "Follow the condition from left to right.",
     "int x = 0; if (x != 0 && 10 / x > 1) {}", "Will the division run? Why does the order matter?",
     "No. && stops after the false left side, so the division is never evaluated. Reversing the two operands would try to divide by zero before checking x."},
    {"Predict: loop boundaries", "Write down the changing value of i on each iteration.",
     "for (int i = 0; i < 3; ++i) std::cout << i;", "What is printed? Would <= 3 change the result?",
     "It prints 012. The loop runs for i = 0, 1 and 2; with i <= 3 it would also print 3. Off-by-one conditions are a common loop bug."},
    {"Predict: value parameters", "This function receives a copy of its argument.",
     "void add(int n) { n += 2; }", "If x is 5, what is x after add(x)?",
     "x is still 5. A by-value parameter is a local copy. To modify the caller's int, use a reference parameter such as void add(int& n)."},
    {"Predict: reference alias", "Two names refer to one integer.",
     "int a = 5; int& b = a; b = 9;", "What is a after the assignment to b?",
     "a is 9. b is a reference alias for a, not a separate copy. Changing b changes the same object seen through a."},
    {"Predict: index and length", "Remember that indices start at zero.",
     "std::string s = \"code\"; char c = s.at(2);", "Which character is stored in c?",
     "'d'. Index 0 is 'c', index 1 is 'o', and index 2 is 'd'. at() checks bounds and throws if the index is invalid."},
    {"Predict: vector mutation", "Watch the size after pushing and removing a value.",
     "std::vector<int> v{2, 4}; v.push_back(6);", "What are v.size() and v.back()?",
     "size() is 3 and back() is 6. push_back appends one element; back() returns the final element (the vector must not be empty)."},
    {"Predict: sort direction", "Consider the comparator's return value.",
     "std::sort(v.begin(), v.end(), std::greater<int>{});", "For v = {2, 9, 4}, what order results?",
     "9, 4, 2. std::greater<int> orders larger values before smaller ones. Include <algorithm> and <functional> for these facilities."},
    {"Predict: encapsulation", "A private data member cannot be accessed from unrelated code.",
     "class Counter { int value = 0; public: int get() const { return value; } };",
     "Can main access c.value? How should it read the value?",
     "No: class members are private by default. main should call c.get(), a public const member function that reads without modifying the object."},
    {"Predict: resource lifetime", "A local object has automatic storage duration.",
     "{ std::vector<int> v{1, 2, 3}; }", "What happens to the vector's storage at the closing brace?",
     "The local vector is destroyed and releases the memory it owns. This is RAII: resources are tied to object lifetime, so you do not manually delete the vector's elements."},
    {"Predict: move ownership", "A unique_ptr may be moved but not copied.",
     "auto a = std::make_unique<int>(7); auto b = std::move(a);",
     "Who owns the integer after the move, and what is a?",
     "b owns it. a becomes empty (nullptr) after moving a unique_ptr. std::move permits moving; the unique_ptr move operation transfers ownership."},
    {"Predict: virtual dispatch", "A base pointer refers to a derived object.",
     "struct Base{virtual int id(){return 1;}}; struct Child:Base{int id()override{return 2;}};",
     "What does b->id() return when Base* b points to Child?",
     "2. virtual enables runtime dispatch to Child::id through the base pointer. Without virtual, the base version would be selected."},
    {"Predict: templates", "A template can work with more than one type.",
     "template<class T> T twice(T x) { return x + x; }",
     "What are twice(3) and twice(std::string(\"A\"))?",
     "6 and \"AA\". T is deduced separately for each call. This template works for types where the + operation is valid."},
    {"Predict: checked access", "A vector contains two elements.",
     "std::vector<int> v{1, 2}; v.at(3);",
     "What happens here, and how can the caller handle it?",
     "at(3) throws std::out_of_range because the index is beyond the vector. The caller can catch const std::out_of_range& or check the index first."},
    {"Predict: optional", "An optional can either contain a value or be empty.",
     "std::optional<int> x; x = 4;",
     "What are x.has_value() and *x now?",
     "has_value() is true and *x is 4. Before assigning, x was empty and dereferencing it would be invalid; use has_value() or value() to check."}
}};

inline void AddPredictBeforeReveal(std::vector<Topic>& topics) {
    if (topics.size() != predictions.size()) throw std::logic_error("C++ topic count changed; update prediction prompts");
    for (std::size_t i = 0; i < predictions.size(); ++i) {
        if (topics[i].modules.empty()) throw std::logic_error("Missing C++ module");
        const auto& p = predictions[i];
        LessonCard card = L(p.title, p.setup, p.code);
        card.recallPrompt = p.question;
        card.recallAnswer = p.explanation;
        // After the topic's first worked explanation, invite a low-stakes prediction.
        topics[i].modules.front().lesson.push_back(std::move(card));
    }
}

// Two fresh, application-oriented review questions on each later topic test:
// recent and older concepts are mixed with current-topic content.
inline void AddSpiralRetrieval(std::vector<Topic>& topics) {
    if (topics.size() != 18) throw std::logic_error("C++ topic count changed; update spiral reviews");
    using Pair = std::array<Question, 2>;
    const std::array<Pair, 17> review{{
      Pair{Q("Review: after int x{3}; x += 4;, what is x?", {"7", "3", "4", "12"}, "x += 4 adds 4 to the existing 3. Assignment changes the stored value."),
           Q("Review: what does a successful main normally return to the operating system?", {"0", "1", "The printed text", "A semicolon"}, "Returning 0 signals success; printing is a separate operation.")},
      Pair{Q("Review: int x = 5; std::cout << x; prints what?", {"5", "x", "0", "A compile error"}, "The stream prints the variable's value rather than its name."),
           Q("Review: which declaration avoids reading an uninitialised local integer?", {"int attempts{0};", "int attempts;", "int attempts() = 0;", "attempts int = 0;"}, "Give local integers an initial value; brace initialization also checks narrowing.")},
      Pair{Q("Review: what does 9 / 2 evaluate to when both operands are int?", {"4", "4.5", "5", "0"}, "Integer division truncates the fractional part, so 9 / 2 is 4."),
           Q("Review: what appears from std::cout << 1 << 2;?", {"12", "3", "1 2", "Nothing"}, "Output insertions do not insert a space unless asked.")},
      Pair{Q("Review: for int n = 6, what is n % 2 == 0?", {"true", "false", "6", "A compiler error"}, "% gives the remainder. Six is divisible by two with remainder zero."),
           Q("Review: what is the value of int total = 2 + 3 * 4;?", {"14", "20", "24", "9"}, "Multiplication takes precedence over addition: 2 + 12 = 14.")},
      Pair{Q("Review: for (int i = 0; i < 3; ++i), how many iterations run?", {"3", "2", "4", "0"}, "i takes values 0, 1, 2. The boundary i < 3 excludes 3."),
           Q("Review: what happens with if (false && risky())?", {"risky() is skipped", "risky() always runs", "Both branches run", "A compile error"}, "&& short-circuits when its left side is false.")},
      Pair{Q("Review: which function parameter can update the caller's int?", {"void update(int& value)", "void update(int value)", "void update(const int value)", "void update()"}, "An int& parameter aliases the original int; by-value parameters are copies."),
           Q("Review: int a = 2; int b = a; b = 9; what is a?", {"2", "9", "11", "Undefined"}, "b is an independent copy, not a reference.")},
      Pair{Q("Review: int a = 4; int& r = a; r++; what is a?", {"5", "4", "0", "An invalid reference"}, "A reference aliases the same object, so incrementing r also changes a."),
           Q("Review: what is the last index of an array of 5 elements?", {"4", "5", "1", "6"}, "C++ arrays are zero-indexed: valid positions are 0 through 4.")},
      Pair{Q("Review: std::string s = \"cat\"; what is s.at(1)?", {"'a'", "'c'", "'t'", "A null character"}, "Index 1 is the second character; at() checks for out-of-range indices."),
           Q("Review: what must be true before dereferencing an int* p?", {"p points to a live int", "p is always nullptr", "p is declared const", "p holds an even number"}, "A pointer must point to a valid, still-alive object to dereference it safely.")},
      Pair{Q("Review: after std::vector<int> v{1, 2}; v.push_back(3); what is v.size()?", {"3", "2", "4", "1"}, "push_back adds one element to the end of a vector."),
           Q("Review: for an empty vector, which call is invalid?", {"v.back()", "v.empty()", "v.size()", "v.begin() == v.end()"}, "back() requires a nonempty vector. Check empty() first.")},
      Pair{Q("Review: sorting {3,1,2} with std::sort(begin,end) produces?", {"1,2,3", "3,2,1", "3,1,2", "2,1,3"}, "The default std::sort orders ints ascending."),
           Q("Review: which statement actually modifies a std::vector<int> v?", {"v.push_back(9);", "v.size();", "v.empty();", "v.at(0);"}, "push_back changes the container by appending an element.")},
      Pair{Q("Review: which member is accessible from outside a class?", {"A public member", "Every private member", "Only static private members", "Private members if main is nearby"}, "public is the interface for callers; private is restricted to class members and friends."),
           Q("Review: what is a valid way to invoke a function stored in a lambda variable f?", {"f()", "call f", "lambda(f)", "invoke f() without syntax"}, "A lambda object can be called with parentheses like another callable.")},
      Pair{Q("Review: a local std::vector goes out of scope. What releases its owned storage?", {"Its destructor", "Manual delete[] on v", "The compiler warning", "Nothing; it leaks"}, "RAII ties storage cleanup to the vector object's lifetime."),
           Q("Review: why might a C++ class need a constructor?", {"To establish a valid initial object state", "To erase every object", "To force all members public", "To avoid calling functions"}, "Constructors initialise a newly created object and its members.")},
      Pair{Q("Review: after auto b = std::move(a) for a unique_ptr, which owns the object?", {"b", "Both a and b", "Neither", "A global variable"}, "Moving a unique_ptr transfers ownership and empties the source pointer."),
           Q("Review: why is copy-owning a raw pointer into two owning objects dangerous?", {"Both may delete the same memory", "It makes the code constexpr", "It always performs a deep copy", "The memory becomes read-only"}, "Shallow copies of owning raw pointers can cause double deletion; prefer RAII types.")},
      Pair{Q("Review: through Base* p to Derived, what enables Derived::run() to be called?", {"A virtual run() in Base", "A private run() in Base", "An unrelated function", "A global using declaration"}, "Virtual dispatch chooses the derived override at runtime."),
           Q("Review: which pointer represents sole ownership by default?", {"std::unique_ptr", "std::shared_ptr", "A raw pointer", "void*"}, "unique_ptr expresses single ownership and deletes automatically.")},
      Pair{Q("Review: why are function-template definitions often kept in headers?", {"The compiler needs the definition at instantiation", "They cannot return values", "Templates are runtime-only", "Headers make them virtual"}, "The compiler generates concrete specializations when it sees template uses."),
           Q("Review: if Base has a virtual destructor, what happens deleting Derived through Base*?", {"Both derived and base destructors run", "Only the base destructor runs", "Neither runs", "It is always a compile error"}, "A virtual base destructor makes polymorphic deletion safe.")},
      Pair{Q("Review: what is thrown by std::vector<int>{1}.at(5)?", {"std::out_of_range", "std::bad_alloc always", "std::logic_error always", "Nothing"}, "at() performs checked access and reports an invalid index using std::out_of_range."),
           Q("Review: when a function template is called with int, what happens?", {"An int specialization can be instantiated", "It changes the source file", "All types become int", "Templates cannot accept int"}, "Template parameters are substituted with concrete types at compile time.")},
      Pair{Q("Review: what is true of std::optional<int> x; before assigning a value?", {"x.has_value() is false", "*x is always zero", "x contains an int at index 0", "x throws on construction"}, "A default-constructed optional is empty. Check before dereferencing."),
           Q("Review: what should you catch to avoid slicing std::exception-derived objects?", {"const std::exception&", "std::exception by value", "An int", "void*"}, "Catching exceptions by const reference avoids copying and slicing.")}
    }};
    for (std::size_t t = 1; t < topics.size(); ++t) {
        // The original tests and code tasks remain intact, as do course save IDs.
        for (const auto& q : review[t - 1]) topics[t].test.questions.push_back(q);
    }
}

// Recap questions rather than simply replaying all model answers immediately.
inline void AddRetrievalCheckpoints(std::vector<Topic>& topics) {
    for (std::size_t t = 0; t < topics.size(); ++t) {
        if (topics[t].modules.empty()) continue;
        LessonCard card = L("Recall before the test",
            "Try to answer without looking at the previous lessons. Then reveal the model reasoning. "
            "This is ungraded practice, so a mistake is useful information.");
        const auto& p = predictions[t];
        card.recallPrompt = std::string("First, explain the key idea from ") + topics[t].name + ". " + p.question;
        card.recallAnswer = std::string(p.explanation) + " Now attempt the mixed test; some questions come from earlier topics.";
        topics[t].test.lesson.clear();
        topics[t].test.lesson.push_back(std::move(card));
    }
}

inline void Apply(std::vector<Topic>& topics) {
    AddPredictBeforeReveal(topics);
    AddSpiralRetrieval(topics);
    AddRetrievalCheckpoints(topics);
}

} // namespace cpp_learning
