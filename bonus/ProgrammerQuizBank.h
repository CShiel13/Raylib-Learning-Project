#pragma once
// Self-contained question catalogue. No raylib dependency; answer 0 is correct
// in source data and the session shuffles the four displayed choices.
#include <array>
#include <string>
#include <vector>

namespace programmer_quiz {

enum class Difficulty { Easy, Middle, Hard, Jackpot };

struct Question {
    Difficulty difficulty;
    std::string prompt;
    std::array<std::string, 4> answers;
    std::string explanation;
    std::string duckHint; // Deliberately helpful, without revealing the solution.
};

inline Difficulty DifficultyForRound(int round) {
    if (round <= 3) return Difficulty::Easy;
    if (round <= 9) return Difficulty::Middle;
    if (round <= 14) return Difficulty::Hard;
    return Difficulty::Jackpot;
}

inline const std::vector<Question>& Questions() {
    static const std::vector<Question> questions = {
        // 1-3: easy, sometimes silly, to warm up.
        {Difficulty::Easy, "What does a C++ programmer usually do when the compiler complains?", {"Read the error message", "Turn the monitor around", "Ask the keyboard nicely", "Add more semicolons everywhere"}, "Compiler messages often identify the file and location of an error.", "The computer might actually be trying to help."},
        {Difficulty::Easy, "Which file extension is commonly used for C++ source code?", {".cpp", ".png", ".mp3", ".xlsx"}, "C++ implementation files commonly use .cpp.", "Think of the language's initials."},
        {Difficulty::Easy, "Which character normally ends a C++ statement?", {";", "?", "@", "#"}, "Most C++ statements end with a semicolon.", "It looks like a dot above a comma."},
        {Difficulty::Easy, "Which keyword declares an integer variable?", {"int", "integerish", "number", "digit"}, "int is the built-in integer type.", "Three characters, starting with i."},
        {Difficulty::Easy, "Which operator adds two numbers?", {"+", "&&", "==", "!="}, "+ performs arithmetic addition on numbers.", "The same symbol you use on a calculator."},
        {Difficulty::Easy, "Where does a normal hosted C++ program begin running?", {"main()", "launch()", "startGame()", "boot()"}, "Program execution begins in main().", "The entry function has four letters."},
        {Difficulty::Easy, "What is the value of 2 * 3 in C++?", {"6", "23", "5", "8"}, "The * operator multiplies two integers.", "This is multiplication, not string concatenation."},
        {Difficulty::Easy, "What does // mean outside a string literal?", {"Start a line comment", "Divide twice", "Close the program", "Declare an array"}, "// begins a comment extending to the end of the line.", "The compiler ignores the text after it on that line."},
        {Difficulty::Easy, "Which value can a bool hold?", {"true", "maybe", "sometimes", "unknown"}, "bool has the logical values true and false.", "It's a yes-or-no type."},
        {Difficulty::Easy, "Which symbol pair surrounds a function's argument list?", {"( )", "[ ]", "{ }", "< >"}, "Function calls use parentheses for their arguments.", "They are round, not square."},
        {Difficulty::Easy, "Which keyword can repeat a block while a condition holds?", {"while", "perhaps", "againPlease", "repeatForever"}, "while is a C++ loop statement.", "It is also an ordinary English word."},
        {Difficulty::Easy, "What should you do before blaming the compiler for a typo?", {"Check your code", "Reinstall Windows", "Buy a new GPU", "Unplug the mouse"}, "Most early compiler errors come from ordinary code mistakes.", "A misplaced symbol is more likely than a broken CPU."},
        {Difficulty::Easy, "Which two expressions can each increase an int i by one?", {"Both ++i and i++", "Only ++i", "Only i++", "Neither expression"}, "Both forms are valid C++. They differ in which value the expression supplies when that value is used.", "Putting ++ before or after the name both increments it."},

        // 4-9: broad, intermediate C++ knowledge.
        {Difficulty::Middle, "What does == do in a C++ expression?", {"Compares values for equality", "Assigns a new value", "Creates a reference", "Joins two strings"}, "== compares; = assigns.", "One equals sign assigns, two ask a question."},
        {Difficulty::Middle, "Which standard container can grow as you add elements?", {"std::vector", "std::array", "int[3]", "std::pair"}, "std::vector is a dynamically sized sequence container.", "It's named after a mathematical object."},
        {Difficulty::Middle, "What does a reference parameter such as int& x allow?", {"Modify the caller's int", "Always copy the argument", "Disable all exceptions", "Create an array"}, "A non-const lvalue reference can refer to and modify its original object.", "The & means the parameter refers to an existing object."},
        {Difficulty::Middle, "What is std::cout commonly used for?", {"Writing to standard output", "Reading a file", "Measuring time", "Creating a thread"}, "std::cout is the standard output stream.", "Its name is short for character output."},
        {Difficulty::Middle, "Which header declares std::vector?", {"<vector>", "<list>", "<iostream>", "<memory>"}, "Include <vector> when using std::vector.", "The header has the same name as the container."},
        {Difficulty::Middle, "What does const int score = 7; prevent?", {"Reassigning score", "Reading score", "Passing score to a function", "Printing score"}, "A const object cannot be reassigned after initialization.", "The keyword promises not to change the value."},
        {Difficulty::Middle, "What does a for loop's ++i usually do?", {"Increase i by one", "Decrease i by one", "Double i", "Set i to zero"}, "Prefix ++ increments by one.", "Two plus symbols means increment."},
        {Difficulty::Middle, "What is printed by int i = 7; std::cout << i++;?", {"7", "8", "78", "A compiler error"}, "Postfix i++ yields the old value 7 for printing; i becomes 8 afterward.", "The ++ is after the variable."},
        {Difficulty::Middle, "In a for loop with an int counter, which update is correct when its value is unused?", {"Both ++i and i++", "Only ++i", "Only i++", "Neither"}, "Both increment i by one; the loop's update expression result is discarded.", "The difference matters when a surrounding expression uses the result."},
        {Difficulty::Middle, "Which operator gives the remainder of integer division?", {"%", "/", "**", "::"}, "% returns the remainder of integer division.", "Useful for detecting odd and even numbers."},
        {Difficulty::Middle, "What does break do inside a loop?", {"Exits the nearest loop", "Skips to its next iteration", "Restarts the program", "Throws an exception"}, "break ends the innermost enclosing loop or switch.", "It ends something, rather than skipping one pass."},
        {Difficulty::Middle, "What is printed by int n=2; std::cout << ++n; ?", {"3", "2", "4", "21"}, "Prefix increment updates n before it is evaluated.", "Prefix ++ changes the variable first."},
        {Difficulty::Middle, "What does std::string store?", {"A sequence of characters", "Only one character", "Only integers", "A graphics texture"}, "std::string represents text as a sequence of char values.", "It's the standard C++ text type."},
        {Difficulty::Middle, "What is the first index of a C++ array?", {"0", "1", "-1", "It depends on the compiler"}, "Built-in arrays and standard sequence containers use zero-based indexing.", "A three-element array's indexes are 0, 1, and 2."},
        {Difficulty::Middle, "What does std::unique_ptr primarily represent?", {"Exclusive ownership", "Shared ownership by default", "A fixed array", "A network socket"}, "std::unique_ptr holds unique ownership and cleans up automatically.", "Only one owning pointer exists at a time."},
        {Difficulty::Middle, "Which header is needed for std::sort?", {"<algorithm>", "<cmath>", "<cstdio>", "<chrono>"}, "std::sort is declared in <algorithm>.", "Sorting is a common algorithm."},
        {Difficulty::Middle, "What does std::vector<int> v; v.push_back(9); do?", {"Appends 9 to v", "Removes 9 from v", "Resets v", "Sorts v"}, "push_back adds an element at the end of a vector.", "The verb describes pushing onto the back."},
        {Difficulty::Middle, "What does nullptr represent?", {"A null pointer value", "The integer -1", "An empty std::string", "The end of a loop"}, "nullptr is a dedicated null pointer literal.", "It's the modern replacement for a null pointer written as 0."},
        {Difficulty::Middle, "What does an else branch run after?", {"An if condition evaluates false", "An if condition evaluates true", "Every loop iteration", "A function definition"}, "An else branch handles the false case of its if statement.", "It's the other path."},
        {Difficulty::Middle, "What happens if you access v.at(100) out of range?", {"It throws std::out_of_range", "It must return zero", "It silently grows the vector", "It restarts the program"}, "std::vector::at checks bounds and throws on an invalid index.", "Unlike operator[], at performs bounds checking."},

        // 10-14: more specialized mechanics and pitfalls.
        {Difficulty::Hard, "Why should a polymorphic base class usually have a virtual destructor?", {"To destroy derived objects through base pointers", "To speed up virtual calls", "To disable inheritance", "To force stack allocation"}, "Deleting a derived object through an owning base pointer needs a virtual destructor (or another valid destruction mechanism).", "Think about deleting through Base* when the object is Derived."},
        {Difficulty::Hard, "What does std::move(x) do by itself?", {"Casts x to an rvalue expression", "Moves bytes immediately", "Deletes x", "Allocates new memory"}, "std::move is a cast enabling move operations; an actual move depends on subsequent operations.", "The name is misleading: no constructor is invoked by the cast alone."},
        {Difficulty::Hard, "What is RAII?", {"Binding resource lifetime to object lifetime", "A random access integer iterator", "An alternative to templates", "A process scheduler"}, "RAII acquires/releases resources through object lifetime management.", "Destructors are the key."},
        {Difficulty::Hard, "What does std::weak_ptr avoid?", {"Keeping a shared object alive", "All dynamic allocation", "Runtime polymorphism", "Copying strings"}, "weak_ptr observes shared ownership without increasing the strong reference count.", "It can break shared_ptr reference cycles."},
        {Difficulty::Hard, "What does noexcept communicate in a function declaration?", {"A non-throwing exception specification", "A compile-time-only function", "A thread-safe function", "An always-inline function"}, "noexcept declares a non-throwing exception specification (violations call std::terminate).", "It is about exception handling, not performance guarantees."},
        {Difficulty::Hard, "What does std::optional<T> represent?", {"A T that may be absent", "A pointer that must never be null", "A growable collection", "A compile-time constant"}, "std::optional represents either an engaged T or no value.", "Sometimes a function has no result to return."},
        {Difficulty::Hard, "What does std::forward<T>(x) preserve in forwarding code?", {"The original value category", "The original object address always", "The exact machine code", "The source-file encoding"}, "Perfect forwarding uses std::forward to preserve lvalue/rvalue qualification.", "It is paired with forwarding references in templates."},
        {Difficulty::Hard, "Which vector operation can invalidate every iterator when it reallocates?", {"push_back", "size", "empty", "capacity"}, "A reallocation caused by push_back invalidates all pointers, references and iterators to elements.", "Growing a dynamic array may relocate its elements."},
        {Difficulty::Hard, "What is the One Definition Rule (ODR) about?", {"Consistent definitions across a program", "One declaration per file", "One thread per object", "One class per namespace"}, "The ODR constrains where and how program entities are defined.", "Separate translation units must agree on shared definitions."},
        {Difficulty::Hard, "What happens when a constructor throws before construction completes?", {"Completed members/base subobjects are destroyed", "The object destructor runs normally", "Memory must always leak", "The constructor restarts"}, "Already-constructed members and bases are unwound; the incomplete object's destructor is not called.", "Think of which subobjects were fully constructed."},
        {Difficulty::Hard, "What is the purpose of a pure virtual function (= 0)?", {"Make the class abstract", "Erase the function at runtime", "Disable virtual dispatch", "Force static allocation"}, "A class with an unoverridden pure virtual function is abstract.", "You cannot instantiate an abstract class."},
        {Difficulty::Hard, "Which C++17 feature allows structured binding?", {"auto [a, b] = pair;", "let [a, b] = pair;", "var a, b = pair;", "tuple(a, b) := pair;"}, "C++17 introduced structured bindings with auto and square brackets.", "Think of destructuring a pair into named elements."},
        {Difficulty::Hard, "What is the correct way to catch a std::exception without copying it?", {"catch (const std::exception& e)", "catch (std::exception e)", "catch (std::exception* e)", "catch (std::exception&& e)"}, "Catch polymorphic exceptions by const reference to avoid slicing and copies.", "A reference preserves the dynamic type."},
        {Difficulty::Hard, "What does std::lock_guard<std::mutex> primarily provide?", {"Scoped mutex locking", "Dynamic thread creation", "Lock-free memory", "Recursive exception handling"}, "std::lock_guard locks on construction and unlocks on destruction.", "It uses RAII for synchronization."},
        {Difficulty::Hard, "Why is a virtual call from a base constructor surprising?", {"It dispatches within the currently constructing class", "It always calls the most-derived override", "It is a syntax error", "It changes the vtable permanently"}, "During base construction, virtual dispatch does not call overrides in derived classes not yet constructed.", "The derived part does not exist yet."},
        {Difficulty::Hard, "What does mutable permit for a data member?", {"Modification even through a const member function", "Changing a const object's type", "Calling deleted functions", "Disabling copy construction"}, "mutable exempts that member from certain constness restrictions.", "Useful for caches or synchronization members."},
        {Difficulty::Hard, "What does std::make_unique<T>() return?", {"std::unique_ptr<T>", "T* always", "std::shared_ptr<T>", "std::optional<T>"}, "make_unique creates an object owned by a unique_ptr.", "The factory name identifies its smart-pointer type."},
        {Difficulty::Hard, "Why might ++it be preferred over it++ for an iterator when the old value is unused?", {"Postfix may need a copy of the old iterator", "Prefix always skips an element", "Postfix is illegal for iterators", "Prefix always runs in constant time"}, "Postfix must provide the previous value as its expression result, so an iterator type may perform extra copying; compilers can often optimize it away.", "Think about what value each form needs to make available."},
        {Difficulty::Hard, "When is a lambda capture written as [this] useful?", {"To access the current object's members", "To copy the entire executable", "To prevent all side effects", "To run only at compile time"}, "[this] captures the current object pointer in the lambda.", "It is related to object member access."},

        // 15: a small bank of genuinely high-stakes questions, randomized per run.
        {Difficulty::Jackpot, "Which ordering guarantees do relaxed atomic operations provide on their own?", {"Atomicity without synchronization ordering", "Sequential consistency", "An implicit mutex", "Automatic thread joining"}, "memory_order_relaxed preserves atomic access but does not establish synchronization ordering across threads.", "Atomic doesn't automatically mean ordered."},
        {Difficulty::Jackpot, "What is a key condition for a valid std::launder use?", {"A suitable object already exists at that storage", "It allocates a new object", "It calls the destructor", "It converts any void* safely"}, "std::launder obtains a pointer to an existing object under specific storage/reachability conditions; it cannot create objects.", "The operation changes which pointer can be used, not whether an object exists."},
        {Difficulty::Jackpot, "In a class template, what does typename often disambiguate?", {"A dependent qualified name denoting a type", "A non-type template argument", "A preprocessor macro", "An overload set at runtime"}, "typename marks certain dependent names as types during template parsing.", "The parser does not always know a dependent name is a type."},
        {Difficulty::Jackpot, "What happens when std::terminate is called by a noexcept violation?", {"Normal stack unwinding is not guaranteed", "The exception is silently ignored", "The function returns a default value", "The compiler retries the call"}, "Violating a non-throwing exception specification calls std::terminate, with no guaranteed normal unwind.", "Think of a fatal runtime termination, not a regular catch."},
        {Difficulty::Jackpot, "Why can returning std::move(local) harm performance in some return statements?", {"It may inhibit named return value optimization", "It always copies twice", "It makes local static", "It disables all compiler optimizations"}, "An unnecessary std::move in a return can prevent NRVO of a local variable.", "Compilers can often construct a returned local directly."},
        {Difficulty::Jackpot, "Which condition is required for a data race in the C++ memory model?", {"Conflicting unsynchronized accesses, with at least one write", "Two reads of an immutable value", "Two threads running the same function", "Any use of std::atomic"}, "Conflicting, potentially concurrent non-atomic accesses without a happens-before relationship form a data race.", "Two reads alone cannot conflict; think about writes and synchronization."},
        {Difficulty::Jackpot, "What is a key benefit of std::scoped_lock locking multiple mutexes?", {"It uses deadlock-avoidance when locking them together", "It makes mutexes unnecessary", "It creates one OS thread per mutex", "It guarantees zero waiting"}, "std::scoped_lock manages multiple mutexes and uses a deadlock avoidance algorithm when given more than one.", "The tricky case is acquiring more than one lock."},
        {Difficulty::Jackpot, "Which guarantee is typical of copy-and-swap assignment when swap is non-throwing?", {"The strong exception guarantee", "No exceptions are possible anywhere", "All copies become moves", "Assignments are lock-free"}, "Prepare a copy before swapping to leave the original unchanged if copying fails.", "If preparation fails, the original target has not been modified."}
    };
    return questions;
}

inline const std::array<const char*, 15>& CareerLadder() {
    static const std::array<const char*, 15> titles = {{
        "Office coffee intern", "Bug triage volunteer", "Coding intern", "QA apprentice", "Paid C++ internship",
        "Junior tools coder", "Junior gameplay dev", "C++ software developer", "Embedded C++ developer", "Senior C++ developer",
        "Rendering specialist", "Engine programmer", "Principal C++ engineer", "Lead engine architect", "Chief game engine engineer"
    }};
    return titles;
}

inline const char* TierLabel(int round) {
    if (round <= 3) return "Warm-up interview";
    if (round <= 6) return "Junior interview";
    if (round <= 9) return "Mid-level interview";
    if (round <= 12) return "Senior interview";
    return "Dream-job interview";
}

} // namespace programmer_quiz
