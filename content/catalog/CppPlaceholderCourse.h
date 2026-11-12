#pragma once
#include "../CourseModel.h"

inline std::vector<Topic> BuildCppTopics() {
    std::vector<Topic> T;

    // ---------------------------------------------------------------- 1
    T.push_back(MakeTopic("C++ Basics", "Basics", "Compilation, syntax, and your first C++ program.",
        Color{ 60,120,220,255 }, 0,
        {
            WithLesson(WithCode(M("Hello, C++", {
                Q("Which header provides std::cout?", {"<iostream>", "<stdio.h>", "<cout>", "<print>"}, "iostream defines cin/cout."),
                Q("Which is the entry point of a C++ program?", {"int main()", "void start()", "void main()", "main()"}, "Standard is int main()."),
                Q("How does C++ source become an executable?", {"Compiled to machine code", "Interpreted at runtime", "Runs on the JVM", "Runs on .NET"}, "C++ is a compiled language."),
            }), "Print \"Hello\" using std::cout (end with ;).", "std::cout << ...;",
            {"std::cout << \"Hello\";", "cout << \"Hello\";"}, "std::cout << \"Hello\";"), {
                {"C++ is compiled",
                 "Unlike Java, C++ is compiled directly to machine code. The compiler reads your .cpp source and produces an executable the CPU runs natively - no virtual machine, no interpreter.",
                 "g++ main.cpp -o main",
                 {{"g++", "a common C++ compiler"},
                  {"main.cpp", "your source file"},
                  {"-o main", "the output executable"}}},
                {"The main function",
                 "Every C++ program has exactly one main function. It's the entry point - the operating system starts execution there. The standard signature returns int.",
                 "int main() { ... }",
                 {{"int main()", "the entry point - exactly one per program"}}},
                {"The iostream header",
                 "std::cout lives in the iostream header. To use it, you must include that header at the top of your file.",
                 "#include <iostream>",
                 {{"#include", "the preprocessor directive - pastes a header file in"},
                  {"<iostream>", "the header that declares std::cout"}}},
                {"Printing with std::cout",
                 "std::cout is the standard output stream. The insertion operator << sends data into it.",
                 "std::cout << \"Hello\";",
                 {{"std::cout", "the standard output stream"},
                  {"<<", "the insertion operator - sends data into the stream"},
                  {"\"Hello\"", "the text - in double quotes"},
                  {";", "ends the statement"}}},
            }),

            WithLesson(WithCode(M("Variables & Types", {
                Q("Which declares an int?", {"int x = 5;", "integer x = 5;", "Int x(5);", "var x = 5;"}, "int is a primitive type."),
                Q("What is auto x = 3.14; typed as?", {"double", "float", "decimal", "auto"}, "Decimal literals default to double."),
                Q("Which type stores 'a'?", {"char", "string", "int", "letter"}, "Single quotes make a char."),
            }), "Declare int age = 18;", "int name = value;",
            {"int age = 18;"}, "type name = value;"), {
                {"Declaring an int",
                 "A variable is a named slot in memory. C++ is statically typed, so the type comes first and cannot change.",
                 "int age = 18;",
                 {{"int", "the type - a whole number"},
                  {"age", "the name"},
                  {"=", "assigns the value"},
                  {"18", "the value"},
                  {";", "ends the statement"}}},
                {"char: single characters",
                 "char holds one character, written in single quotes. Double quotes make a string, which is a completely different type.",
                 "char c = 'a';",
                 {{"char", "the type - a single character"},
                  {"'a'", "the value - single quotes, one character"}}},
                {"auto and decimal literals",
                 "auto lets the compiler infer the type from the value. A decimal literal like 3.14 is a double by default - so auto deduces double, not float.",
                 "auto x = 3.14;",
                 {{"auto", "lets the compiler deduce the type"},
                  {"3.14", "a decimal literal - defaults to double"}}},
                {"Your turn",
                 "The next question asks you to declare int age = 18. Pattern: type, name, equals, value, semicolon.",
                 "", {}},
            }),
        },
        WithCode(M("Topic Test", {
            Q("What namespace holds cout?", {"std", "io", "cxx", "sys"}, "using namespace std; brings it in."),
            Q("sizeof(int) on most systems is...", {"4", "2", "8", "1"}, "Typically 4 bytes (32 bits)."),
            Q("Which line ends a C++ statement?", {"semicolon ;", "newline", "period .", "nothing"}, "C++ uses semicolons."),
            }), "Include the iostream header.", "#include <...>",
            { "#include <iostream>", "#include<iostream>" }, "The iostream header supplies cout.")
            ));

    // ---------------------------------------------------------------- 2
    T.push_back(MakeTopic("Control Flow & Loops", "Flow", "if, switch, for, while: controlling the flow of code.",
        Color{ 80,140,230,255 }, 40,
        {
            WithCode(M("Conditionals", {
                Q("Which is valid C++?", {"if (x > 5) { }", "if x > 5 then", "if x > 5 do { }", "if: x > 5"}, "Conditions go in parentheses."),
                Q("Which condition type does if accept?", {"Anything convertible to bool", "Only true/false", "Only int", "Only 0/1"}, "Implicit conversion to bool."),
                Q("Which keyword handles no-match in switch?", {"default", "else", "case default", "other"}, "default is the fallback."),
            }), "Print \"big\" if x > 10 (single line).", "if (...) std::cout << ...;",
            {"if (x > 10) std::cout << \"big\";", "if(x>10)cout<<\"big\";"}, "A single if around cout."),

            WithCode(M("Loops", {
                Q("Which loop runs its body at least once?", {"do-while", "while", "for", "range-for"}, "do-while checks after the body."),
                Q("for (int i=0; i<3; ++i) runs how many times?", {"3", "2", "4", "Forever"}, "i = 0, 1, 2."),
                Q("Which keyword exits a loop immediately?", {"break", "exit", "stop", "quit"}, "break jumps past the loop."),
            }), "Write a while header running while i < 5 (include {).", "while (...) {",
            {"while (i < 5) {", "while(i<5){"}, "while (condition) { ... }"),
        },
        WithCode(M("Topic Test", {
            Q("What does continue do?", {"Skips to the next iteration", "Exits the loop", "Ends the program", "Restarts the loop"}, "continue skips the rest of this pass."),
            Q("Which is a range-for over an int array/vector v?", {"for (int x : v)", "for (int x in v)", "foreach (int x : v)", "for x of v"}, "Colon syntax."),
            Q("while (true) creates...", {"An infinite loop", "A syntax error", "A do-while", "Nothing"}, "It loops forever (use break)."),
            }), "Write a for loop header counting i = 0..9 (include {).", "for (...;...;...) {",
            { "for (int i = 0; i < 10; ++i) {", "for (int i = 0; i < 10; i++) {", "for(int i=0;i<10;++i){" }, "for (init; cond; step) {")
            ));

    // ---------------------------------------------------------------- 3
    T.push_back(MakeTopic("Functions", "Functions", "Parameters, returns, references and overloading.",
        Color{ 100,160,240,255 }, 70,
        {
            WithCode(M("Defining Functions", {
                Q("Which return type means 'returns nothing'?", {"void", "null", "nil", "empty"}, "void."),
                Q("Which is a valid signature?", {"int add(int a, int b)", "function add(a, b)", "int add(a, b)", "def add(a, b)"}, "Types are required."),
                Q("When do you need a forward declaration?", {"When calling a function before its definition", "Never", "Only for int main", "Always"}, "The compiler must see the declaration first."),
            }), "Define int square(int x) returning x*x (single line).", "int name(int p) { return ...; }",
            {"int square(int x) { return x * x; }", "int square(int x){return x*x;}"}, "int name(params) { body }"),

            WithCode(M("References & Overloading", {
                Q("What does int& mean?", {"A reference to an int", "A pointer to an int", "A copy of an int", "An array of ints"}, "References alias another variable."),
                Q("Passing an int by reference lets the function...", {"Modify the caller's int", "Only read the int", "Copy the int", "Delete the int"}, "References allow modification."),
                Q("Function overloading requires...", {"Different parameter lists", "Different return types only", "Different names", "Different namespaces"}, "Different signatures."),
            }), "Define void inc(int& x) that increments x (single line).", "void name(int& p) { ... }",
            {"void inc(int& x) { ++x; }", "void inc(int& x) { x++; }", "void inc(int&x){++x;}"}, "Take int& to modify the caller's value."),
        },
        WithCode(M("Topic Test", {
            Q("Where do default arguments go?", {"In the declaration", "In the definition", "In the call", "Only in main"}, "They must appear in the declaration."),
            Q("What does const int& promise?", {"The function will not modify the argument", "A null reference", "A pointer", "A copy"}, "const prevents modification."),
            Q("The entry function's return type is...", {"int", "void", "bool", "auto"}, "int main() is standard."),
            }), "Declare (no body) a function greet that takes no args and returns nothing.", "void greet();",
            { "void greet();" }, "A declaration ends with ;")
            ));

    // ---------------------------------------------------------------- 4
    T.push_back(MakeTopic("Pointers & Memory", "Pointers", "Addresses, indirection, new/delete and ownership.",
        Color{ 120,180,250,255 }, 100,
        {
            WithCode(M("Pointers", {
                Q("How do you get the address of x?", {"&x", "*x", "x&", "addr(x)"}, "The unary & operator."),
                Q("How do you dereference pointer p?", {"*p", "&p", "p->", "p[0]"}, "Unary * reads through the pointer."),
                Q("What is the modern null pointer literal?", {"nullptr", "NULL", "0", "void*"}, "nullptr is type-safe."),
            }), "Declare an int pointer p = nullptr;", "int* p = ...;",
            {"int* p = nullptr;", "int *p = nullptr;"}, "The * marks the pointer type."),

            WithCode(M("Dynamic Memory", {
                Q("Which allocates an int on the heap?", {"new int", "malloc int", "alloc int", "int* = new"}, "new int or new int(5)."),
                Q("Which frees a single object allocated with new?", {"delete p;", "free(p);", "delete[] p;", "remove p;"}, "delete for new, delete[] for new[]."),
                Q("Which frees an array from new int[10]?", {"delete[] p;", "delete p;", "free(p);", "delete p[10];"}, "Arrays need delete[]."),
            }), "Allocate an int on the heap with value 5 and assign to p.", "int* p = new ...;",
            {"int* p = new int(5);", "int* p = new int{5};", "int*p=new int(5);"}, "new int(5) allocates and initializes."),
        },
        WithCode(M("Topic Test", {
            Q("A memory leak happens when...", {"You call new without matching delete", "You forget a semicolon", "You use nullptr", "You call delete twice"}, "Every new needs a delete."),
            Q("p->x is the same as...", {"(*p).x", "&p.x", "p.x", "*p.x"}, "-> is sugar for (*p).x."),
            Q("Compared to pointers, references...", {"Cannot be null and cannot be reseated", "Can be null", "Are always slower", "Are the same thing"}, "References are always bound."),
            }), "Free the array allocated as new int[10] stored in p.", "delete ...",
            { "delete[] p;", "delete [] p;" }, "Arrays require delete[].")
            ));

    // ---------------------------------------------------------------- 5
    T.push_back(MakeTopic("Classes & OOP", "Classes", "Classes, constructors, inheritance and virtual dispatch.",
        Color{ 140,200,255,255 }, 140,
        {
            WithCode(M("Classes & Constructors", {
                Q("What is the default access in a class?", {"private", "public", "protected", "package"}, "Unlike struct, class defaults to private."),
                Q("A constructor's name is...", {"The class name", "init", "new", "construct"}, "Same as the class, no return type."),
                Q("What does this point to?", {"The current object", "The class", "The parent", "nullptr"}, "this is a pointer to the instance."),
            }), "Declare an empty class named Dog (include ;).", "class Name { ... };",
            {"class Dog { };", "class Dog{};"}, "Classes end with a semicolon."),

            WithCode(M("Inheritance & virtual", {
                Q("How do you inherit publicly from Animal?", {"class Dog : public Animal { }", "class Dog extends Animal", "class Dog inherits Animal", "class Dog uses Animal"}, "Colon + access specifier."),
                Q("Which keyword enables runtime dispatch?", {"virtual", "abstract", "override", "dynamic"}, "virtual methods are dispatched at runtime."),
                Q("Why do polymorphic base classes need a virtual destructor?", {"So deleting via base pointer destroys the derived part", "For faster destruction", "Because of constructors", "It is required by the compiler"}, "Otherwise derived destructor isn't called."),
            }), "Write the class header: Dog inherits publicly from Animal (include {).", "class ... : public ... {",
            {"class Dog : public Animal {", "class Dog:public Animal{"}, "class Derived : public Base"),
        },
        WithCode(M("Topic Test", {
            Q("What makes a class abstract?", {"A pure virtual method (= 0)", "A private constructor", "No methods", "A virtual destructor"}, "Pure virtual = 0."),
            Q("What does = 0 after a method mean?", {"Pure virtual", "Returns zero", "Default argument", "Deletes it"}, "Declares a pure virtual function."),
            Q("What does the override keyword do?", {"Checks that the method really overrides a virtual", "Makes the method virtual", "Deletes the parent's version", "Nothing"}, "It documents and validates intent."),
            }), "Declare a pure virtual void speak() (end with ;).", "virtual void ... = 0;",
            { "virtual void speak() = 0;", "virtual void speak()=0;" }, "= 0 makes it pure virtual.")
            ));

    // ---------------------------------------------------------------- 6
    T.push_back(MakeTopic("Templates & STL", "STL", "Generic code and the standard containers.",
        Color{ 60,200,220,255 }, 180,
        {
            WithCode(M("Templates", {
                Q("Which declares a template with type T?", {"template <typename T>", "template <T>", "template T", "generic <T>"}, "The typename keyword names the parameter."),
                Q("When is a template instantiated?", {"At compile time", "At runtime", "At link time", "Never"}, "The compiler generates concrete code."),
                Q("Templates avoid...", {"Writing the same code for every type", "Runtime overhead", "Both is fine", "Nothing"}, "Templates give zero-cost genericity."),
            }), "Declare a template of type T and function T id(T x) (single line).", "template <...> ... name(...)",
            {"template <typename T> T id(T x)", "template <class T> T id(T x)"}, "template <typename T> T id(T x)"),

            WithCode(M("STL Containers", {
                Q("Which STL container is a dynamic array?", {"std::vector", "std::list", "std::array", "std::map"}, "vector resizes itself."),
                Q("Which STL container holds key-value pairs?", {"std::map", "std::set", "std::vector", "std::queue"}, "map stores keys with values."),
                Q("Which container stores unique elements?", {"std::set", "std::vector", "std::list", "std::array"}, "set rejects duplicates."),
            }), "Declare a std::vector<int> named v.", "std::vector<...> ...;",
            {"std::vector<int> v;", "vector<int> v;"}, "std::vector<int> name;"),
        },
        WithCode(M("Topic Test", {
            Q("Which method appends to a vector?", {"push_back", "append", "add", "insert_end"}, "v.push_back(x)."),
            Q("Which function sorts a vector<int> named v?", {"std::sort(v.begin(), v.end())", "v.sort()", "sort(v)", "std::order(v)"}, "std::sort takes iterators."),
            Q("Which header do STL algorithms like std::sort live in?", {"<algorithm>", "<vector>", "<utility>", "<numeric>"}, "std::sort is in <algorithm>."),
            }), "Declare std::vector<int> v with {1, 2, 3}.", "std::vector<int> v = { ... };",
            { "std::vector<int> v = {1, 2, 3};", "std::vector<int> v{1,2,3};", "vector<int> v = {1, 2, 3};" }, "Braces initialize the vector.")
            ));

    // ---------------------------------------------------------------- 7
    T.push_back(MakeTopic("Smart Pointers & RAII", "RAII", "Automatic resource management with unique_ptr and shared_ptr.",
        Color{ 100,220,180,255 }, 230,
        {
            WithCode(M("unique_ptr & shared_ptr", {
                Q("std::unique_ptr represents...", {"Exclusive ownership", "Shared ownership", "A raw pointer", "A weak reference"}, "Only one owner at a time."),
                Q("std::shared_ptr uses...", {"A reference count", "Unique ownership", "The GC", "Manual delete"}, "Shared owners share the count."),
                Q("How do you create a unique_ptr<int> holding 5?", {"std::make_unique<int>(5)", "new std::unique_ptr<int>(5)", "std::unique_ptr<int>(new int)", "make_shared<int>(5)"}, "std::make_unique is preferred."),
            }), "Create a unique_ptr<int> p holding 5 (use make_unique).", "auto p = std::make_unique<...>(...);",
            {"auto p = std::make_unique<int>(5);", "std::unique_ptr<int> p = std::make_unique<int>(5);"}, "std::make_unique<int>(5)"),

            WithCode(M("RAII", {
                Q("RAII stands for...", {"Resource Acquisition Is Initialization", "Runtime Allocation Is Invalid", "Reference And Iteration", "Readily Available I/O"}, "Resources are tied to object lifetime."),
                Q("When is a destructor called?", {"When the object goes out of scope", "When delete is called only", "At program start", "Never"}, "Scope end triggers destruction."),
                Q("RAII mainly prevents...", {"Resource leaks", "Stack overflows", "Compilation errors", "Slow sorting"}, "Automatic release avoids leaks."),
            }), "Write the destructor for class MyClass (empty body, single line).", "~Name() { ... }",
            {"~MyClass() { }", "~MyClass(){};"}, "~ClassName() { }"),
        },
        WithCode(M("Topic Test", {
            Q("When is a shared_ptr's object destroyed?", {"When the last shared_ptr goes away", "Immediately after creation", "At program start", "On nullptr"}, "Reference count reaching 0."),
            Q("A unique_ptr can be...", {"Moved but not copied", "Copied freely", "Neither moved nor copied", "Cast to int"}, "Exclusive ownership means move-only."),
            Q("What does std::move(p) do?", {"Transfers ownership", "Deletes p", "Copies p", "Nulls p"}, "Casts to an rvalue reference."),
            }), "Declare std::unique_ptr<int> p owning a new int with value 42.", "auto p = std::make_unique<...>(...);",
            { "auto p = std::make_unique<int>(42);", "std::unique_ptr<int> p = std::make_unique<int>(42);" }, "make_unique<int>(42)")
            ));

    // ---------------------------------------------------------------- 8
    T.push_back(MakeTopic("Modern C++", "Modern", "Lambdas, auto, move semantics and range-for.",
        Color{ 220,140,180,255 }, 280,
        {
            WithCode(M("Lambdas & auto", {
                Q("Which is a valid lambda?", {"[](int a, int b) { return a + b; }", "lambda(int a, int b) { return a+b; }", "func(a, b) -> a+b", "(a, b) => a+b"}, "Square brackets begin a lambda."),
                Q("What does auto do?", {"Deduces the type from the initializer", "Makes a variable dynamic", "Requires Java", "Makes a global"}, "The compiler infers the type."),
                Q("How do you capture everything by reference?", {"[&]", "[=]", "[ref]", "[this]"}, "[&] captures by reference."),
            }), "Declare auto add = lambda taking int a, int b returning a+b (single line).", "auto add = [](...) { return ...; };",
            {"auto add = [](int a, int b) { return a + b; };", "auto add=[](int a, int b){return a+b;};"}, "[capture](params) { body }"),

            WithCode(M("Move semantics & range-for", {
                Q("What does std::move do?", {"Casts to an rvalue reference", "Copies the object", "Deletes the object", "Swaps two objects"}, "It enables move construction."),
                Q("What is T&&?", {"An rvalue reference", "A pointer", "A const reference", "An array"}, "Used for move semantics."),
                Q("After std::move(x), x is...", {"Valid but unspecified", "Destroyed", "Null", "Always empty"}, "Do not rely on its value."),
            }), "Iterate over vector<int> v by reference (just the for header, include opening {).", "for (auto& x : ...) {",
            {"for (auto& x : v) {", "for (auto &x : v) {", "for(auto&x:v){"}, "for (auto& x : v) { ... }"),
        },
        WithCode(M("Topic Test", {
            Q("Range-for over a vector by const reference:", {"for (const auto& x : v)", "for (auto x : v&)", "for (x : v)", "for x in v"}, "const auto& avoids copies and mutation."),
            Q("nullptr vs NULL:", {"nullptr is type-safe", "NULL is type-safe", "They are identical", "NULL is newer"}, "nullptr has its own type."),
            Q("override marks...", {"A virtual method that overrides a parent's", "A function that hides", "A class", "A constructor"}, "It documents and validates overriding."),
            }), "Loop over vector<int> v with an iterator named it (header only).", "for (auto it = ...; ...; ...)",
            { "for (auto it = v.begin(); it != v.end(); ++it)", "for(auto it=v.begin();it!=v.end();++it)" }, "Iterators use begin() and end().")
            ));

    return T;
}

// ---------------------------------------------------------------------------
// Python course.
// ---------------------------------------------------------------------------
