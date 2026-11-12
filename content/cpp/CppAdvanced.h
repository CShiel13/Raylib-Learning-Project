#pragma once
#include "CppCourseSupport.h"

inline std::vector<Topic> CppTopicsC() {
    std::vector<Topic> T;

    // ============================================================ 13  Memory & RAII
    T.push_back(CppTopic(12, "Memory & RAII", "Memory", "Stack and heap, new and delete, and the idiom that keeps C++ safe: RAII.", {
        WithLesson(WithCode(M("Stack, Heap & new", {
            Q("Which memory is released automatically at the end of its scope?", {"Stack (automatic) variables", "Heap objects created with new", "Both", "Neither"}, "Heap objects live until you delete them."),
            Q("What must you pair with new int[5]?", {"delete[]", "delete", "free", "release"}, "Array new needs array delete."),
            Q("What is a memory leak?", {"Heap memory that is never freed", "A stack overflow", "Using nullptr", "Deleting twice"}, "The program loses track of memory it still owns."),
        }), "Free the single int that p points to.", "delete ...;",
            {"delete p;"}, "Every new needs a matching delete."), {
            L("Two kinds of memory", "Local variables live on the stack and vanish at the end of their scope. Objects made with new live on the heap until you call delete. Forgetting delete leaks memory; deleting twice or using a pointer after delete is undefined behaviour.", "int* p = new int(5);",
              {{"int*", "pointer to an int"}, {"p", "the pointer, which lives on the stack"}, {"new int(5)", "allocates an int on the heap holding 5"}}),
        }),

        WithLesson(WithCode(M("Destructors & RAII", {
            Q("When does a stack object's destructor run?", {"When it goes out of scope", "When the program starts", "Only when you call it", "Never"}, "Automatically, in reverse order of construction."),
            Q("What does RAII stand for?", {"Resource Acquisition Is Initialization", "Run And Initialize Immediately", "Reference And Instance Interface", "Random Access Is Implicit"}, "Resources are tied to object lifetime."),
            Q("Why does RAII make code exception safe?", {"Destructors run while the stack unwinds", "Exceptions are disabled", "It uses goto", "It copies resources"}, "Cleanup happens automatically even when an exception flies."),
        }), "Write the declaration of the destructor of class Buffer, on one line.", "~...;",
            {"~Buffer();"}, "A destructor is ~ followed by the class name."), {
            L("RAII", "Acquire a resource in a constructor and release it in the destructor. Because destructors run automatically when an object leaves scope, even during an exception, resources can never be forgotten. This is the most important C++ idea.", "~Buffer() { delete[] data_; }",
              {{"~Buffer", "destructor: ~ plus the class name"}, {"()", "never takes parameters"}, {"delete[] data_;", "releases the resource"}}),
        }),

        WithLesson(WithCode(M("Rule of Three, Five & Zero", {
            Q("A class owns raw memory. Which three special functions does the Rule of Three say you usually need?", {"Destructor, copy constructor, copy assignment", "Constructor, setter, getter", "Destructor, swap, move", "Constructor, destructor, main"}, "If you need one of them you almost always need all three."),
            Q("The Rule of Zero advises...", {"Let members like vector and smart pointers manage resources", "Never write constructors", "Avoid classes", "Always use new"}, "Prefer designs where the compiler-generated versions are correct."),
            Q("What does the default copy do with a raw pointer member?", {"Copies the pointer (shallow copy), risking a double delete", "Copies the pointed-to data", "Deletes it", "Sets it to null"}, "Two objects then own the same memory."),
        }), "Declare a deleted copy constructor for class Buffer, on one line.", "Buffer(const Buffer&) ...;",
            {"Buffer(const Buffer&) = delete;"}, "= delete forbids the operation."), {
            L("Copy control", "If a class manages a resource directly you must define how it is copied. Rule of Three: destructor, copy constructor, copy assignment. Rule of Five adds move constructor and move assignment. Rule of Zero: use members that manage themselves and write none.", "Buffer(const Buffer&) = delete;",
              {{"Buffer(const Buffer&)", "the copy constructor"}, {"= delete;", "copying is not allowed"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("int* p = new int(3); delete p; What is *p afterwards?", {"Undefined behaviour (dangling pointer)", "3", "0", "nullptr"}, "The memory has been released."),
            Q("Which is safer than raw new and delete?", {"std::vector and smart pointers", "malloc", "goto", "global arrays"}, "They free memory for you."),
            Q("Calling delete twice on the same pointer is...", {"Undefined behaviour", "Fine", "A compile error", "Only a warning"}, "A double free."),
        }), "Allocate an int array of 10 elements with new and store it in int* arr.", "int* arr = new ...;",
        {"int* arr = new int[10];"}, "new type[count] returns a pointer.")));

    // ============================================================ 14  Smart Pointers & Move
    T.push_back(CppTopic(13, "Smart Pointers & Move", "Smart", "Ownership made automatic: unique_ptr, shared_ptr and move semantics.", {
        WithLesson(WithCode(M("unique_ptr", {
            Q("Which header declares the smart pointers?", {"<memory>", "<smart>", "<pointer>", "<ptr>"}, "#include <memory>"),
            Q("Can a std::unique_ptr be copied?", {"No, only moved", "Yes", "Only when const", "Only in main"}, "It has exclusive ownership."),
            Q("What is the preferred way to create one?", {"std::make_unique<T>(args)", "new unique_ptr<T>", "unique_ptr(T)", "std::new_unique<T>"}, "make_unique is safe and short."),
        }), "Create a unique_ptr to Dog named d with std::make_unique, using auto.", "auto d = ...;",
            {"auto d = std::make_unique<Dog>();"}, "make_unique<Type>(args)."), {
            L("unique_ptr", "std::unique_ptr owns an object exclusively and deletes it automatically when it goes out of scope. It cannot be copied, only moved, so ownership is always clear. It is the default smart pointer.", "auto p = std::make_unique<Dog>();",
              {{"auto p", "the smart pointer"}, {"std::make_unique", "creates the object and the owner together"}, {"<Dog>()", "what to create"}}),
        }),

        WithLesson(WithCode(M("shared_ptr & weak_ptr", {
            Q("A shared_ptr frees its object when...", {"The last owner is gone (count reaches 0)", "The first owner is gone", "You call delete", "The program exits only"}, "Reference counting."),
            Q("Two shared_ptrs that point to each other cause...", {"A reference cycle that leaks", "A crash", "Faster code", "Nothing"}, "Neither count ever reaches zero."),
            Q("What breaks that cycle?", {"std::weak_ptr", "std::unique_ptr", "raw delete", "const"}, "weak_ptr observes without owning."),
        }), "Create a shared_ptr<int> named p holding 5 with std::make_shared, using auto.", "auto p = ...;",
            {"auto p = std::make_shared<int>(5);"}, "make_shared<Type>(args)."), {
            L("Shared ownership", "std::shared_ptr lets several owners share one object, which is freed when the last owner disappears. Use it only when ownership is genuinely shared. std::weak_ptr observes an object without keeping it alive and breaks ownership cycles.", "auto p = std::make_shared<int>(5);",
              {{"std::make_shared", "creates a shared object"}, {"<int>", "the type"}, {"(5)", "constructor argument"}}),
        }),

        WithLesson(WithCode(M("Move Semantics", {
            Q("What does std::move actually do?", {"Casts to an rvalue so a move can happen", "Moves memory immediately", "Deletes the object", "Deep-copies it"}, "The move itself is done by the move constructor or assignment."),
            Q("After std::string b = std::move(a); what is a?", {"Valid but unspecified (typically empty)", "Destroyed", "Guaranteed unchanged", "Invalid memory"}, "Do not rely on its value; you may assign to it or destroy it."),
            Q("Why are moves cheap?", {"They transfer ownership of internal resources instead of copying them", "They skip destructors", "They use less code", "They use the stack"}, "A moved string just takes the other's buffer."),
        }), "Move string a into a new string b using std::move.", "std::string b = ...;",
            {"std::string b = std::move(a);"}, "std::move(a) allows b to steal from a."), {
            L("Moving instead of copying", "Copying duplicates resources. Moving steals them from an object that is about to be discarded. std::move marks an object as safe to steal from. After a move the old object is still valid but its contents are unspecified.", "std::string b = std::move(a);",
              {{"std::string b", "the new string"}, {"std::move(a)", "permit stealing a's contents"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("Which smart pointer is the default choice?", {"unique_ptr", "shared_ptr", "weak_ptr", "A raw pointer"}, "Prefer exclusive ownership unless sharing is needed."),
            Q("auto a = std::make_unique<int>(1); auto b = a;  Does it compile?", {"No, copying is deleted", "Yes", "Yes, but it leaks", "Only with std::move"}, "You must write std::move(a)."),
            Q("Returning a local std::vector by value is typically...", {"Cheap thanks to moves and return value optimisation", "Always a deep copy", "Illegal", "Only possible with pointers"}, "Modern compilers avoid the copy entirely."),
        }), "Transfer ownership from unique_ptr a into a new auto variable b.", "auto b = ...;",
        {"auto b = std::move(a);"}, "unique_ptrs are moved, not copied.")));

    // ============================================================ 15  Inheritance & Polymorphism
    T.push_back(CppTopic(14, "Inheritance & Polymorphism", "Inherit", "Build class hierarchies and call the right function at runtime.", {
        WithLesson(WithCode(M("Inheritance", {
            Q("Which syntax makes Dog inherit publicly from Animal?", {"class Dog : public Animal", "class Dog extends Animal", "class Dog inherits Animal", "class Dog -> Animal"}, "A colon, an access specifier and the base class."),
            Q("In what order are the parts of a Dog object constructed?", {"The Animal base first, then Dog", "Dog first, then Animal", "In random order", "Only Dog is constructed"}, "Base before derived; destruction is the reverse."),
            Q("Which base-class members can a derived class NOT access directly?", {"Private members", "Protected members", "Public members", "Static public members"}, "private is accessible only inside the base class."),
        }), "Write the class header for Dog publicly inheriting Animal, with an opening brace.", "class Dog : ... {",
            {"class Dog : public Animal {"}, "class Derived : public Base {"), {
            L("Inheritance", "A derived class gets the members of its base class and can add its own. Use public inheritance for an 'is-a' relationship: a Dog is an Animal. The base part is constructed first and destroyed last.", "class Dog : public Animal {",
              {{"class Dog", "the new class"}, {": public", "inherits publicly"}, {"Animal", "the base class"}}),
        }),

        WithLesson(WithCode(M("virtual & override", {
            Q("Without virtual, calling a function through an Animal* uses...", {"Animal's version (static binding)", "Dog's version", "A random one", "None, it fails to compile"}, "The pointer's declared type decides."),
            Q("What does override do?", {"Asks the compiler to check you really override a base virtual function", "Makes a function static", "Hides the function", "Creates a new virtual function"}, "It catches signature typos."),
            Q("Dynamic dispatch needs...", {"A virtual function called through a base pointer or reference", "A template", "A static function", "A friend function"}, "The actual object type is chosen at runtime."),
        }), "Write the declaration of void speak() marked as overriding, on one line.", "void speak() ...;",
            {"void speak() override;"}, "override goes after the parameter list."), {
            L("Runtime polymorphism", "Declare a function virtual in the base class and a call through a base pointer or reference runs the version of the real object. Mark overrides with override so the compiler checks the signature.", "void speak() override;",
              {{"void speak()", "same signature as the base's virtual function"}, {"override", "compiler verifies it really overrides"}}),
        }),

        WithLesson(WithCode(M("Abstract Classes", {
            Q("What makes a class abstract?", {"At least one pure virtual function", "A private constructor only", "The keyword abstract", "Being a template"}, "= 0 on a virtual function."),
            Q("Can you create an object of an abstract class?", {"No", "Yes", "Only on the heap", "Only const"}, "You must derive and implement the pure virtual functions."),
            Q("How is a pure virtual function declared?", {"virtual void draw() = 0;", "abstract void draw();", "void draw() = null;", "virtual draw() {}"}, "= 0 makes it pure."),
        }), "Declare the pure virtual function void draw() in a class, on one line.", "virtual void draw() ...;",
            {"virtual void draw() = 0;"}, "= 0 means derived classes must implement it."), {
            L("Interfaces", "A pure virtual function has no implementation in the base class: derived classes must provide it. A class with one is abstract and acts as an interface that many classes can implement.", "virtual void draw() = 0;",
              {{"virtual", "can be overridden"}, {"void draw()", "the signature"}, {"= 0", "pure: no body here"}}),
        }),

        WithLesson(WithCode(M("Virtual Destructors & Slicing", {
            Q("Why make a base class destructor virtual?", {"So deleting through a base pointer destroys the derived part too", "It makes it faster", "It makes the class abstract", "main requires it"}, "Otherwise only the base part is destroyed."),
            Q("Object slicing happens when...", {"A derived object is copied into a base object by value", "A pointer is null", "A vector shrinks", "A template is instantiated"}, "The derived part is cut off."),
            Q("Safest way to hold polymorphic objects in a container?", {"std::vector<std::unique_ptr<Base>>", "std::vector<Base>", "std::vector<Derived>", "std::array<Base, 3>"}, "Pointers keep the real type; unique_ptr owns it."),
        }), "Declare Animal's virtual destructor as defaulted, on one line.", "virtual ~Animal() ...;",
            {"virtual ~Animal() = default;"}, "= default asks for the compiler's version."), {
            L("Polymorphic base classes", "A class meant to be a base should have a virtual destructor, otherwise deleting a derived object through a base pointer is undefined behaviour. Store polymorphic objects by pointer, never by value, or slicing cuts off the derived part.", "virtual ~Animal() = default;",
              {{"virtual", "dynamic dispatch for deletion"}, {"~Animal()", "the destructor"}, {"= default;", "compiler-generated body"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("What is polymorphism?", {"One interface with many behaviours chosen at runtime", "Having many inheritance levels", "Using templates", "Copying objects"}, "The same call can behave differently."),
            Q("Which keyword prevents a virtual function from being overridden further?", {"final", "const", "static", "sealed"}, "final stops further overriding."),
            Q("Animal a = Dog(); What happens?", {"Slicing: only the Animal part is kept", "The full Dog is stored", "A compile error", "Dynamic dispatch happens"}, "Assigning by value slices."),
        }), "Inside an override call the base version Animal::speak() (end with ;).", "Animal::...;",
        {"Animal::speak();"}, "Base::function() calls the base version explicitly.")));

    // ============================================================ 16  Templates
    T.push_back(CppTopic(15, "Templates", "Templates", "Write code once that works for many types.", {
        WithLesson(WithCode(M("Function Templates", {
            Q("What does template <typename T> introduce?", {"A generic type parameter named T", "A new class", "A macro", "A namespace"}, "T stands for any type."),
            Q("When are templates instantiated?", {"At compile time for the types actually used", "At run time", "Only at link time", "Never"}, "The compiler generates a version per type."),
            Q("maxOf(3, 7) deduces T as...", {"int", "double", "T", "char"}, "Both arguments are ints."),
        }), "Write the template header line with a type parameter T using typename.", "template <...>",
            {"template <typename T>", "template<typename T>"}, "template <typename T> precedes the function."), {
            L("Generic functions", "A function template is a recipe: the compiler creates a real function for each type you use. T is replaced by the type deduced from the arguments.", "template <typename T> T maxOf(T a, T b)",
              {{"template <typename T>", "T is a placeholder for any type"}, {"T maxOf", "returns a T"}, {"(T a, T b)", "both parameters are T"}}),
        }),

        WithLesson(WithCode(M("Class Templates", {
            Q("Which is a use of a class template?", {"std::vector<int> v;", "vector(int) v;", "std::vector v<int>;", "vector::int v;"}, "The type goes in angle brackets after the name."),
            Q("Where must template definitions usually live?", {"In headers, visible wherever they are used", "Only in .cpp files", "In main only", "In comments"}, "The compiler needs the full definition to instantiate."),
            Q("What is std::pair<int, std::string>?", {"An instantiation of a class template", "A function", "A macro", "A keyword"}, "pair is a class template."),
        }), "Declare an empty class template named Box with type parameter T, on one line.", "template <typename T> class Box { ... };",
            {"template <typename T> class Box { };", "template<typename T> class Box {};"}, "template header, then the class."), {
            L("Generic classes", "Standard containers such as std::vector and std::map are class templates. You write your own the same way: put template <typename T> before the class and use T inside it.", "template <typename T> class Box { T value; };",
              {{"template <typename T>", "type parameter"}, {"class Box", "the class"}, {"T value;", "a member whose type is chosen by the user"}}),
        }),

        WithLesson(WithCode(M("Constraints & Concepts", {
            Q("What do concepts (C++20) do?", {"Constrain which types a template accepts", "Make templates run at run time", "Replace classes", "Allocate memory"}, "They state requirements on template parameters."),
            Q("Which header provides std::integral?", {"<concepts>", "<limits>", "<iostream>", "<utility>"}, "#include <concepts>"),
            Q("What is a main benefit of concepts?", {"Much clearer compile errors", "Smaller binaries always", "Faster code always", "Run-time checks"}, "The error says which requirement failed."),
        }), "Write a template header that constrains T with std::integral.", "template <std::integral ...>",
            {"template <std::integral T>", "template<std::integral T>"}, "A concept can replace typename."), {
            L("Concepts", "C++20 lets you say what a template parameter must support. std::integral accepts only integer types, so passing a double gives a short, readable error instead of a wall of text.", "template <std::integral T>",
              {{"template", "generic code follows"}, {"std::integral", "a concept: T must be an integer type"}, {"T", "the type parameter"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("template <typename T> void f(T x);  f(2.5) instantiates with T =", {"double", "float", "int", "T"}, "2.5 is a double."),
            Q("Templates give which kind of polymorphism?", {"Compile-time (static)", "Run-time (dynamic)", "None", "Virtual"}, "The compiler generates one function per type."),
            Q("typename versus class in a template parameter list...", {"They are interchangeable there", "typename works only for functions", "class is an error", "They differ in memory use"}, "Both introduce a type parameter."),
        }), "Call the template maxOf with the explicit type int on 3 and 7 (just the expression).", "maxOf<...>(3, 7)",
        {"maxOf<int>(3, 7)", "maxOf<int>(3, 7);"}, "Explicit type goes in angle brackets.")));

    // ============================================================ 17  Exceptions
    T.push_back(CppTopic(16, "Exceptions", "Errors", "Handle the unexpected: throw, catch and write exception-safe code.", {
        WithLesson(WithCode(M("try, catch & throw", {
            Q("Which keyword signals an error by throwing an exception?", {"throw", "raise", "error", "signal"}, "throw expression;"),
            Q("What is the best way to catch standard exceptions?", {"By const reference", "By value", "By pointer", "By rvalue reference"}, "Avoids copying and slicing."),
            Q("What happens if an exception is never caught?", {"std::terminate is called and the program ends", "The program continues", "It is ignored", "main returns 0"}, "An uncaught exception ends the program."),
        }), "Throw a std::runtime_error with the message bad.", "throw std::runtime_error(...);",
            {R"c(throw std::runtime_error("bad");)c"}, "throw an object; the message is its argument."), {
            L("Exceptions", "When something goes wrong, throw an exception object. Control jumps out of the current function, destroying local objects as it goes, until a matching catch block is found. try wraps the code that might fail.", R"c(throw std::runtime_error("bad");)c",
              {{"throw", "raise an exception"}, {"std::runtime_error", "the exception type"}, {R"c("bad")c", "a message describing the problem"}}),
        }),

        WithLesson(WithCode(M("Standard Exceptions", {
            Q("Which function returns the message of std::exception e?", {"e.what()", "e.message()", "e.text()", "e.msg"}, "what() returns a C string."),
            Q("v.at(99) on a small vector throws...", {"std::out_of_range", "std::bad_alloc", "Nothing", "std::runtime_error only"}, "at() checks the bounds."),
            Q("Which header declares std::runtime_error?", {"<stdexcept>", "<error>", "<exceptions>", "<except>"}, "#include <stdexcept>"),
        }), "Write the catch clause header that catches any std::exception by const reference named e, with an opening brace.", "catch (...) {",
            {"catch (const std::exception& e) {"}, "catch (const Type& name) {"), {
            L("Catching", "Catch by const reference. std::exception is the base of the standard exceptions, so one handler can catch them all, and what() describes the problem. List specific handlers before general ones.", "catch (const std::exception& e) {",
              {{"catch", "handler"}, {"const std::exception&", "by const reference"}, {"e", "the caught object"}}),
        }),

        WithLesson(WithCode(M("noexcept & Safety", {
            Q("What does noexcept promise?", {"The function will not throw", "The function is faster only", "It catches everything", "It is evaluated at compile time"}, "If it throws anyway, the program terminates."),
            Q("Destructors should generally be...", {"noexcept and never throw", "allowed to throw freely", "virtual only", "private"}, "Throwing from a destructor during unwinding ends the program."),
            Q("Which technique keeps resources safe when exceptions occur?", {"RAII", "goto cleanup", "Global flags", "Ignoring errors"}, "Destructors release resources during unwinding."),
        }), "Declare void swap() as noexcept, on one line.", "void swap() ...;",
            {"void swap() noexcept;"}, "noexcept follows the parameter list."), {
            L("Exception safety", "Mark functions that cannot fail noexcept; the compiler and standard library can then optimise. Combine RAII objects with exceptions so cleanup is automatic, and never let an exception escape a destructor.", "void swap() noexcept;",
              {{"void swap()", "the function"}, {"noexcept", "promises it never throws"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("In what order should several catch blocks be written?", {"Most specific first", "Most general first", "Alphabetically", "Order does not matter"}, "A general handler first would hide the others."),
            Q("What does catch (...) catch?", {"Any exception type", "Only ints", "Only std::exception", "Nothing"}, "The ellipsis is a catch-all."),
            Q("Which exception does new throw when memory runs out?", {"std::bad_alloc", "std::out_of_range", "std::runtime_error", "None, it returns nullptr"}, "Plain new throws instead of returning null."),
        }), "Write the catch-all handler header with an opening brace.", "catch (...) {",
        {"catch (...) {"}, "The ellipsis catches everything.")));

    // ============================================================ 18  Modern C++
    T.push_back(CppTopic(17, "Modern C++", "Modern", "Features from C++11 to C++20 that make code shorter and safer.", {
        WithLesson(WithCode(M("auto & Structured Bindings", {
            Q("Which C++ standard introduced structured bindings?", {"C++17", "C++11", "C++14", "C++20"}, "auto [a, b] = pair; arrived in C++17."),
            Q("Which unpacks pair p into a and b?", {"auto [a, b] = p;", "auto a, b = p;", "[a, b] = p;", "auto (a, b) = p;"}, "Square brackets after auto."),
            Q("Why write for (const auto& [k, v] : m)?", {"It avoids copying each key/value pair", "It makes keys mutable", "It is the only syntax", "It sorts the map"}, "const auto& is read-only without a copy."),
        }), "Unpack pair p into x and y with a structured binding using auto.", "auto [...] = p;",
            {"auto [x, y] = p;"}, "auto [names] = value;"), {
            L("Structured bindings", "A structured binding unpacks a pair, tuple or struct into separate named variables in one declaration. It makes loops over maps much more readable.", "auto [name, age] = person;",
              {{"auto", "deduce the types"}, {"[name, age]", "the new variable names"}, {"person", "the object being unpacked"}}),
        }),

        WithLesson(WithCode(M("optional & variant", {
            Q("std::optional<int> represents...", {"An int that may or may not be present", "Always an int", "A pointer to int", "A reference to int"}, "It holds a value or nothing."),
            Q("Which gives a fallback when an optional o is empty?", {"o.value_or(0)", "o.default(0)", "o.else(0)", "o || 0"}, "value_or returns the stored value or the fallback."),
            Q("Which header declares std::optional?", {"<optional>", "<maybe>", "<option>", "<variant>"}, "#include <optional>"),
        }), "Declare an empty std::optional<int> named result.", "std::optional<int> ...;",
            {"std::optional<int> result;"}, "No value is stored until assigned."), {
            L("Values that may be missing", "std::optional<T> either holds a T or is empty, which is clearer than magic values like -1 or null pointers. Check with has_value() or if (o), read with *o or value(), or use value_or for a fallback. std::variant holds exactly one of several types.", "std::optional<int> result;",
              {{"std::optional", "maybe a value"}, {"<int>", "the type it may hold"}, {"result", "starts empty"}}),
        }),

        WithLesson(WithCode(M("constexpr & enum class", {
            Q("constexpr means a value can be computed...", {"At compile time", "Only at run time", "Never", "By the linker"}, "The compiler can evaluate it while building."),
            Q("Why prefer enum class over a plain enum?", {"Scoped names and no implicit conversion to int", "It is faster", "It allows duplicates", "It is shorter"}, "Fewer accidental mix-ups."),
            Q("How do you name the value Red of enum class Color?", {"Color::Red", "Color.Red", "Red", "Color->Red"}, "Scoped enums use ::."),
        }), "Declare a scoped enum named Color with the values Red and Green, on one line.", "enum class Color { ... };",
            {"enum class Color { Red, Green };"}, "enum class Name { values };"), {
            L("Compile-time and scoped enums", "constexpr marks values and functions the compiler may evaluate while compiling, so they cost nothing at run time. enum class creates a scoped enumeration: its values need the Color:: prefix and never convert silently to int.", "enum class Color { Red, Green };",
              {{"enum class", "a scoped enumeration"}, {"Color", "its name"}, {"{ Red, Green }", "the allowed values"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("Which is NOT modern C++ style guidance?", {"Prefer raw new and delete over smart pointers", "Prefer RAII", "Prefer const where possible", "Prefer range-for loops"}, "Smart pointers and containers are preferred."),
            Q("What is std::string_view?", {"A non-owning view of characters", "An owning string", "A mutable buffer", "A pointer type only"}, "It refers to text owned elsewhere, so the owner must outlive it."),
            Q("What does [[nodiscard]] ask the compiler to do?", {"Warn if the return value is ignored", "Hide the function", "Make it constexpr", "Delete it"}, "Good for error codes and factory functions."),
        }), "Declare a constexpr int named size with the value 8.", "constexpr int size = ...;",
        {"constexpr int size = 8;", "constexpr int size{8};"}, "constexpr values are known at compile time.")));

    return T;
}
