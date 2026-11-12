#pragma once
#include "CppCourseSupport.h"

inline void ApplyCppLessonExtras(std::vector<Topic>& T) {
    auto mod = [&](int t, int m) -> Module& { return T[t].modules[m]; };
    auto AddCard = [&](int t, int m, LessonCard c) { mod(t, m).lesson.push_back(std::move(c)); };
    auto Tx = [&](int t, int m, int card, const std::string& s) { mod(t, m).lesson[card].text += s; };

    // ---- 1 Getting Started
    AddCard(0, 1, L("The program skeleton", "Every program has two parts you will see again and again. #include <iostream> at the top pastes in the standard input/output tools, which is what makes std::cout available. Then comes main, the function where the program starts running. Everything inside its braces runs in order, top to bottom.",
                    "#include <iostream>", {{"#include", "copy another file's declarations into this one"}, {"<iostream>", "the standard input/output library, which provides std::cout"}}));
    Tx(0, 2, 0, " If you leave return 0; out, the compiler adds it for you at the end of main, so the program still returns 0 automatically.");

    // ---- 3 Input & Output
    AddCard(2, 2, L("Formatting numbers", "Include <iomanip> to control how numbers print. std::fixed makes a double print with a fixed number of decimals, and std::setprecision(n) chooses how many. Together they give tidy output such as 3.14 instead of 3.14159.",
                    "std::cout << std::fixed << std::setprecision(2) << x;", {{"std::fixed", "use a fixed number of decimals"}, {"std::setprecision(2)", "two digits after the point"}, {"x", "the value to print"}}));

    // ---- 4 Operators
    Tx(3, 2, 0, " The same pattern works for the other operators: x -= 2 subtracts, x *= 3 multiplies x by 3 and stores the result back in x, and x /= 2 divides.");
    AddCard(3, 3, L("= is not ==", "A single = assigns a value; a double == compares two values. Writing if (x = 5) by mistake assigns 5 to x, and because 5 is non-zero the condition is true every time. It compiles without error, so it is a classic bug.",
                    "if (x == 5)", {{"==", "compares: is x equal to 5?"}}));

    // ---- 5 Conditionals
    Tx(4, 2, 0, " A switch can test an integer, a char or an enum.");
    AddCard(4, 3, L("Short-circuit evaluation", "&& and || stop as soon as the answer is known. In a && b, if a is false then b is never evaluated; in a || b, if a is true then b is skipped. That is why if (p != nullptr && p->ok) is safe: the pointer is only used when it is not null.",
                    "if (p != nullptr && p->ok)", {{"p != nullptr", "checked first"}, {"&&", "if the left side is false, the right side is skipped"}, {"p->ok", "only reached when p is valid"}}));

    // ---- 6 Loops
    Tx(5, 0, 0, " If nothing inside the loop ever makes the condition false, the loop never ends: that is an infinite loop.");
    Tx(5, 1, 0, " A variable declared in the for header, such as i, exists only inside the loop.");

    // ---- 7 Functions
    Tx(6, 0, 0, " A function that returns nothing has the return type void. The compiler reads a file from top to bottom, so a function must be declared (or defined) before the line that calls it.");
    Tx(6, 1, 0, " Default arguments must go on the rightmost parameters: once one parameter has a default, every parameter after it needs one too.");

    // ---- 8 References & Pointers
    Tx(7, 1, 0, " A const reference can also bind to a temporary value, such as const std::string& s = \"hi\";.");
    Tx(7, 3, 0, " Dereferencing a null pointer is undefined behaviour and often crashes the program.");

    // ---- 9 Arrays & Strings
    Tx(8, 1, 0, " at() throws std::out_of_range when the index is invalid. Unlike a raw array, a std::array does not decay to a plain pointer, so it keeps its size when passed around.");

    // ---- 10 Vectors & Containers
    AddCard(9, 0, L("[] versus at()", "v[i] does no bounds checking: a bad index is undefined behaviour. v.at(i) checks the index and throws std::out_of_range if it is invalid. Use at() while you are learning, or whenever an index might be wrong.",
                    "int x = v.at(2);", {{"v.at(2)", "checked access: throws if the index is out of range"}}));
    Tx(9, 1, 0, " size() returns an unsigned type (std::size_t), and v.back() gives the last element (v.front() the first).");
    Tx(9, 2, 0, " Reading m[key] for a missing key inserts it with a default value (0 for ints), which is a common source of surprise.");

    // ---- 11 Algorithms & Lambdas
    Tx(10, 0, 0, " A classic loop walks from begin() while it != v.end(), moving on with ++it.");

    // ---- 12 Classes & Objects
    Tx(11, 1, 0, " If you declare no constructor at all, the compiler generates a default constructor for you.");

    // ---- 13 Memory & RAII
    Tx(12, 0, 0, " Memory from new int[5] is released with delete[], not delete. Match every new with delete and every new[] with delete[].");
    Tx(12, 1, 0, " RAII stands for Resource Acquisition Is Initialization. When an exception is thrown, C++ unwinds the stack and runs the destructor of every local object on the way out, so resources are released even then.");
    Tx(12, 2, 0, " The default copy just copies the pointer value (a shallow copy), so two objects end up owning the same memory and it is deleted twice. The Rule of Zero avoids this by using members such as std::vector and smart pointers that manage their own resources.");

    // ---- 14 Smart Pointers & Move
    Tx(13, 0, 0, " The smart pointers live in the <memory> header.");
    Tx(13, 1, 0, " The reference count tracks how many owners exist; when the last owner is gone and the count reaches 0, the object is freed.");
    Tx(13, 2, 0, " Moves are cheap because they transfer ownership of internal resources (a string's buffer, a vector's storage) instead of copying them.");

    // ---- 15 Inheritance & Polymorphism
    Tx(14, 0, 0, " Private members of the base class stay inaccessible to the derived class; only protected and public members can be used directly.");
    Tx(14, 1, 0, " Without virtual, a call through an Animal* uses Animal's version (static binding), whatever the real object is.");
    Tx(14, 3, 0, " A good container for polymorphic objects is std::vector<std::unique_ptr<Base>>: the pointers keep the real type and the unique_ptrs own the objects.");

    // ---- 16 Templates
    Tx(15, 1, 0, " Because the compiler needs the full definition at every use, templates are normally defined in headers. std::pair<int, std::string> is an instantiation of the class template std::pair.");

    // ---- 17 Exceptions
    Tx(16, 0, 0, " If an exception is never caught, std::terminate is called and the program ends.");
    Tx(16, 1, 0, " Common standard exceptions live in <stdexcept>: std::runtime_error, std::invalid_argument and std::out_of_range, which v.at(99) throws for a bad index.");

    // ---- 18 Modern C++
    Tx(17, 0, 0, " Structured bindings arrived in C++17.");
    Tx(17, 1, 0, " It is declared in the <optional> header.");

    // ---- second pass: gaps the re-audit still found
    Tx(10, 2, 0, " For example, std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; }); sorts in descending order: the lambda is the comparison that sort calls.");
    Tx(13, 2, 0, " Technically std::move only casts its argument to an rvalue; the actual move is done by the move constructor or move assignment.");
    Tx(16, 0, 0, " Catch exceptions by const reference, as in catch (const std::exception& e), so nothing is copied or sliced.");
    Tx(11, 3, 0, " A getter is a small const member function that returns a member without changing the object, for example int getAge() const { return age_; }.");
    Tx(13, 1, 0, " Two shared_ptrs that point at each other form a reference cycle: neither count ever reaches zero, so the objects leak.");
}

