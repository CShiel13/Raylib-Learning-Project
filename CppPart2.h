#pragma once
// CppPart2.h - C++ course, topics 7-12 (Functions ... Classes & Objects).
#include "CppPart1.h"

inline std::vector<Topic> CppTopicsB() {
    std::vector<Topic> T;

    // ============================================================ 7  Functions
    T.push_back(CppTopic(6, "Functions", "Functions", "Package code into reusable pieces: parameters, return values, overloads and scope.", {
        WithLesson(WithCode(M("Defining & Calling", {
            Q("Which is a valid function definition?", {"int add(int a, int b) { return a + b; }", "add(int a, int b) { return a + b; }", "function add(a, b) { return a + b; }", "int add(a, b) { return a + b; }"}, "C++ needs a return type and a type for every parameter."),
            Q("Which return type means 'returns nothing'?", {"void", "null", "none", "empty"}, "void functions do not return a value."),
            Q("Why must a function be declared before it is called?", {"The compiler reads top to bottom and must know its signature", "The linker requires it", "It makes the call faster", "Functions cannot be called otherwise"}, "A declaration (prototype) can come first; the definition can follow later."),
        }), "Write the header of a function named cube that takes an int x and returns an int, with an opening brace.", "int cube(...) {",
            {"int cube(int x) {"}, "return type, name, parameter list."), {
            L("Anatomy of a function", "A function has a return type, a name, a parameter list and a body. Call it by writing its name with arguments. The value after return is handed back to the caller.", "int square(int x) { return x * x; }",
              {{"int", "return type"}, {"square", "function name"}, {"(int x)", "parameters, each with a type"}, {"{ return x * x; }", "the body"}}),
        }),

        WithLesson(WithCode(M("Overloading & Defaults", {
            Q("Which pair is a valid overload?", {"int f(int);  and  int f(double);", "int f(int);  and  double f(int);", "int f(int);  and  int f(int);", "void f();  and  void f();"}, "Overloads must differ in their parameter lists, not just the return type."),
            Q("void log(int a, int b = 5);  Which call is valid?", {"log(1);", "log();", "log(, 2);", "log(b = 2);"}, "b has a default, a does not."),
            Q("Default arguments must be placed on...", {"The rightmost parameters", "The leftmost parameters", "Any parameters in any order", "The return value"}, "Once one parameter has a default, all after it need one too."),
        }), "Write a declaration of void print(int x) where x defaults to 0.", "void print(int x = ...);",
            {"void print(int x = 0);"}, "Put the default value in the declaration."), {
            L("Same name, different inputs", "Overloading lets several functions share a name when their parameter types differ. The compiler picks the best match from the call. A default argument fills in a missing value.", "void greet(std::string name = \"friend\");",
              {{"void", "returns nothing"}, {"greet", "name"}, {"std::string name", "a parameter"}, {"= \"friend\"", "used when the caller passes nothing"}}),
        }),

        WithLesson(WithCode(M("Pass by Value", {
            Q("void inc(int x) { ++x; }  int a = 1; inc(a);  What is a afterwards?", {"1", "2", "0", "Undefined"}, "The function changed its own copy."),
            Q("By default C++ passes arguments...", {"By value (a copy)", "By reference", "By pointer", "By name"}, "Use & to pass by reference."),
            Q("Passing a huge std::vector by value is...", {"Expensive: consider a const reference", "Free", "Impossible", "Always optimised away"}, "The whole vector gets copied."),
        }), "Write a function header named show taking a const reference to std::string called text and returning void (with an opening brace).", "void show(const ...& text) {",
            {"void show(const std::string& text) {"}, "const T& avoids a copy."), {
            L("Copies", "Parameters are copies of the arguments. Changing the parameter does not change the caller's variable. For large objects pass a const reference to avoid the copy.", "void show(const std::string& text);",
              {{"const", "the function may not change it"}, {"std::string&", "a reference: no copy is made"}, {"text", "parameter name"}}),
        }),

        WithLesson(WithCode(M("Scope & Lifetime", {
            Q("Where is a variable declared inside { } visible?", {"Only inside that block", "In the whole file", "In the whole program", "After the block, too"}, "That is block scope."),
            Q("What does static mean for a local variable?", {"It keeps its value between calls", "It is constant", "It is global", "It is private"}, "A static local is created once and lives for the whole program."),
            Q("int x = 1; { int x = 2; } std::cout << x;  prints?", {"1", "2", "12", "An error"}, "The inner x is a different variable that disappears at the closing brace."),
        }), "Declare a static int named calls initialised to 0.", "static int calls ...;",
            {"static int calls = 0;", "static int calls{0};"}, "static locals persist between calls."), {
            L("Scope", "A name is visible from its declaration to the end of its block. An inner block can reuse a name (shadowing), which is usually confusing. A static local variable lives for the whole program but is visible only in its function.", "static int calls = 0;",
              {{"static", "keep the value between calls"}, {"int calls", "the variable"}, {"= 0", "initialised only the first time"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("Which statement about recursion is true?", {"It needs a base case to stop", "It never uses the stack", "A function cannot call itself", "It only works with loops"}, "Every recursive function needs a way to stop."),
            Q("int f(int n) { return n <= 1 ? 1 : n * f(n - 1); }  What does f(4) return?", {"24", "10", "4", "1"}, "4 * 3 * 2 * 1."),
            Q("With f(int) and f(double), which is chosen for f(2.5)?", {"f(double)", "f(int)", "Ambiguous, so an error", "Neither"}, "2.5 is a double, an exact match."),
        }), "Write the return statement that returns n squared.", "return ... ;", {"return n * n;"}, "return sends the value back.")));

    // ============================================================ 8  References & Pointers
    T.push_back(CppTopic(7, "References & Pointers", "Pointers", "Aliases and addresses: how C++ refers to data without copying it.", {
        WithLesson(WithCode(M("References", {
            Q("int x = 5; int& r = x; r = 9;  What is x?", {"9", "5", "Undefined", "r"}, "A reference is another name for x."),
            Q("Which declares a reference to x?", {"int& r = x;", "int r& = x;", "ref int r = x;", "int* r = x;"}, "The & goes with the type."),
            Q("Can a reference be reseated to refer to a different variable?", {"No", "Yes with ==", "Yes with =>", "Only if const"}, "Assigning through a reference changes the original, it never rebinds."),
        }), "Declare an int reference named alias that refers to value.", "int& alias = ...;",
            {"int& alias = value;"}, "A reference must be initialised."), {
            L("References", "A reference is an alias: another name for an existing variable. It must be initialised when declared and can never be rebound. Changes through the reference change the original.", "int& r = x;",
              {{"int&", "reference to an int"}, {"r", "the alias name"}, {"x", "the variable it refers to"}}),
        }),

        WithLesson(WithCode(M("const References", {
            Q("Why pass big objects as const T&?", {"No copy and no modification", "It speeds up arithmetic", "It forces heap use", "It keeps them alive"}, "You get read-only access to the original."),
            Q("Which parameter lets a function modify the caller's int?", {"int& x", "int x", "const int& x", "const int x"}, "Only a non-const reference refers to the caller's variable."),
            Q("Does const std::string& s = \"hi\"; compile?", {"Yes, it binds to a temporary", "No", "Only with new", "Only inside main"}, "A const reference can extend a temporary's lifetime."),
        }), "Write a parameter: a const reference to std::vector<int> named data.", "const ...& data",
            {"const std::vector<int>& data"}, "const T& name"), {
            L("const references", "A const reference gives cheap, read-only access. It is the default way to accept strings, vectors and other big objects that a function only needs to read.", "void print(const std::vector<int>& data);",
              {{"const", "read-only"}, {"std::vector<int>&", "no copy: refers to the caller's vector"}, {"data", "parameter name"}}),
        }),

        WithLesson(WithCode(M("Pointers", {
            Q("int x = 4; int* p = &x; *p = 8;  What is x?", {"8", "4", "The address of x", "0"}, "*p is x itself."),
            Q("What does &x produce?", {"The address of x", "The value of x", "A copy of x", "A reference named p"}, "& before a variable gives its address."),
            Q("In int* a, b;  what is b?", {"An int", "An int*", "A reference", "A compile error"}, "The * binds to a only. Declare pointers on separate lines."),
        }), "Declare an int pointer named p holding the address of value.", "int* p = ...;",
            {"int* p = &value;", "int *p = &value;"}, "&value is the address."), {
            L("Pointers", "A pointer stores the address of another object. & gets an address; * (dereference) follows a pointer back to the value. A pointer can be reassigned and can be null, unlike a reference.", "int* p = &x;",
              {{"int*", "a pointer to an int"}, {"p", "the pointer's name"}, {"&x", "the address of x"}}),
        }),

        WithLesson(WithCode(M("nullptr & Safety", {
            Q("Which value means 'points to nothing'?", {"nullptr", "void", "nil", "0x0 as text"}, "nullptr is the null pointer constant."),
            Q("What happens when you dereference nullptr?", {"Undefined behaviour, often a crash", "It returns 0", "It allocates memory", "It always throws an exception"}, "Never dereference a null pointer."),
            Q("A dangling pointer is one that...", {"Points to memory that is no longer valid", "Is nullptr", "Points to a const", "Is never used"}, "The object it referred to is gone."),
        }), "Write an if that checks pointer p is not null, ending with an opening brace.", "if (...) {",
            {"if (p != nullptr) {", "if (p) {"}, "Compare against nullptr."), {
            L("Staying safe", "Check a pointer against nullptr before using it, and never use a pointer after the object it pointed to has been destroyed. Prefer references or smart pointers when you can.", "if (p != nullptr) {",
              {{"p", "the pointer"}, {"!= nullptr", "is it pointing at something?"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("Which signature lets swapValues change the caller's variables using references?", {"void swapValues(int& a, int& b)", "void swapValues(int a, int b)", "void swapValues(const int& a, const int& b)", "int swapValues(int a, int b)"}, "Non-const references alias the originals."),
            Q("Which is true about references and pointers?", {"A reference must be initialised and cannot be rebound", "A pointer can never be null", "A reference can be reassigned", "A pointer cannot be dereferenced"}, "Those are the key differences."),
            Q("int arr[3] = {1, 2, 3}; int* p = arr;  What is *(p + 1)?", {"2", "1", "3", "An address"}, "p + 1 points to the second element."),
        }), "Write the three statements that swap a and b using a temp variable.", "int temp = a; ...",
        {"int temp = a; a = b; b = temp;"}, "Save one value, overwrite, then restore.")));

    // ============================================================ 9  Arrays & Strings
    T.push_back(CppTopic(8, "Arrays & Strings", "Arrays", "Fixed-size arrays, std::array, and the std::string class.", {
        WithLesson(WithCode(M("Arrays", {
            Q("What is the index of the first element?", {"0", "1", "-1", "It depends"}, "Arrays are zero-based."),
            Q("int a[3] = {1, 2, 3}; a[3] = 4;  is...", {"Undefined behaviour", "Fine", "Always a compile error", "Adds a fourth element"}, "Valid indexes are 0 to 2. C++ does not check bounds."),
            Q("How do you compute the number of elements of a raw array a?", {"sizeof(a) / sizeof(a[0])", "sizeof(a)", "a.length", "len(a)"}, "Total bytes divided by bytes per element."),
        }), "Declare an int array named nums with 5 elements.", "int nums[...];",
            {"int nums[5];"}, "type name[size];"), {
            L("Arrays", "An array stores a fixed number of values of one type side by side. The size must be known at compile time. Indexes start at 0, and nothing stops you from reading past the end, which is undefined behaviour.", "int scores[5] = {90, 85, 70, 100, 60};",
              {{"int", "element type"}, {"scores[5]", "name and size"}, {"{90, 85, 70, 100, 60}", "initial values"}}),
        }),

        WithLesson(WithCode(M("std::array", {
            Q("Which header provides std::array?", {"<array>", "<vector>", "<list>", "<arrays>"}, "#include <array>"),
            Q("What does a.at(10) do on a 3-element std::array?", {"Throws std::out_of_range", "Returns garbage", "Returns 0", "Silently does nothing"}, "at() checks the bounds."),
            Q("An advantage of std::array over a raw array is...", {"It knows its size and does not decay to a pointer", "It can be resized", "It uses the heap", "It is always faster"}, "size(), at() and copying all work as expected."),
        }), "Declare a std::array of 4 ints named a.", "std::array<..., ...> a;",
            {"std::array<int, 4> a;"}, "std::array<type, size> name;"), {
            L("std::array", "std::array wraps a fixed-size array and gives it a size(), at() with bounds checking, and normal copy behaviour. Prefer it to raw arrays.", "std::array<int, 3> a{1, 2, 3};",
              {{"std::array", "fixed-size container"}, {"<int, 3>", "element type and size"}, {"a", "the name"}, {"{1, 2, 3}", "initial values"}}),
        }),

        WithLesson(WithCode(M("std::string Basics", {
            Q("std::string s = \"hi\"; What is s.size()?", {"2", "3", "1", "Undefined"}, "Two characters; there is no visible terminator."),
            Q("Which compares the contents of two strings a and b?", {"a == b", "a.equals(b)", "a === b", "strcmp is the only way"}, "std::string overloads ==."),
            Q("What does s + \"!\" give?", {"A new string with ! appended", "An error", "The number 33 added to s", "Nothing"}, "+ concatenates strings."),
        }), "Declare a std::string named greeting holding Hello.", "std::string greeting = ...;",
            {R"c(std::string greeting = "Hello";)c", R"c(std::string greeting{"Hello"};)c"}, "Text in double quotes."), {
            L("std::string", "std::string is a resizable piece of text. It knows its length (size), can be compared with ==, joined with +, and indexed with s[i]. Include <string>.", R"c(std::string s = "hi";)c",
              {{"std::string", "the text type"}, {"s", "name"}, {R"c("hi")c", "the contents"}}),
        }),

        WithLesson(WithCode(M("String Operations", {
            Q("std::string s = \"hello\"; What is s.substr(1, 3)?", {"\"ell\"", "\"hel\"", "\"ello\"", "\"el\""}, "substr(start, length): three characters from index 1."),
            Q("What does s.find(\"z\") return when z is absent?", {"std::string::npos", "0", "-1 as an int guaranteed", "nullptr"}, "npos is a special large value meaning 'not found'."),
            Q("Which appends to a string?", {"s += \"!\";", "s.add(\"!\");", "s << \"!\";", "s.push(\"!\");"}, "+= appends."),
        }), "Declare a char named first holding the first character of s.", "char first = ...;",
            {"char first = s[0];"}, "Indexes start at 0."), {
            L("Working with text", "Useful members: size(), empty(), substr(pos, len), find(text), and +=. find returns std::string::npos when nothing matches, so compare against it.", "std::string part = s.substr(1, 3);",
              {{"s.substr", "take a slice"}, {"1", "starting at index 1"}, {"3", "three characters long"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("int a[4]{};  What do the elements hold?", {"All 0", "Garbage", "All 1", "Unspecified"}, "Empty braces value-initialise to zero."),
            Q("What is s.empty() for an empty string?", {"true", "false", "An error", "0 as text"}, "empty() is true when size() is 0."),
            Q("std::string a = \"ab\"; a[1] = 'z';  What is a?", {"\"az\"", "\"zb\"", "\"abz\"", "A compile error"}, "Index 1 is the second character."),
        }), "Append the character '!' to string s using push_back.", "s.push_back(...);",
        {"s.push_back('!');"}, "push_back adds one char at the end.")));

    // ============================================================ 10  Vectors & Containers
    T.push_back(CppTopic(9, "Vectors & Containers", "Containers", "std::vector, std::map, std::set and friends: the everyday standard library.", {
        WithLesson(WithCode(M("vector Basics", {
            Q("Which adds an element at the end of vector v?", {"v.push_back(4);", "v.add(4);", "v.append(4);", "v << 4;"}, "push_back."),
            Q("std::vector<int> v(3, 7); contains...", {"{7, 7, 7}", "{3, 7}", "{3, 3, 3}", "{0, 0, 0}"}, "(count, value) builds count copies of value."),
            Q("What is the difference between v[10] and v.at(10)?", {"at() checks bounds and throws; [] does not check", "[] checks bounds; at() does not", "No difference", "at() returns 0"}, "Use at() when you want a safety net."),
        }), "Declare an empty std::vector of std::string named names.", "std::vector<...> names;",
            {"std::vector<std::string> names;"}, "std::vector<type> name;"), {
            L("vector", "std::vector is a resizable array and the default container to reach for. It grows with push_back, knows its size(), and stores its elements contiguously. Include <vector>.", "std::vector<int> v{1, 2, 3};",
              {{"std::vector", "dynamic array"}, {"<int>", "element type"}, {"v", "the name"}, {"{1, 2, 3}", "starting elements"}}),
        }),

        WithLesson(WithCode(M("Iterating", {
            Q("What kind of value does v.size() return?", {"An unsigned size type (std::size_t)", "int", "bool", "double"}, "Beware comparing it with a negative int."),
            Q("Which loop reads every element without copying it?", {"for (const auto& x : v)", "for (auto x : v)", "for (x : v)", "for (auto& x : v) only"}, "const auto& is read-only and copy-free."),
            Q("What does v.back() return?", {"The last element", "The first element", "An iterator", "The size"}, "back() is the final element."),
        }), "Write a range-for header over v reading each element as const auto& x, with an opening brace.", "for (const auto& x : ...) {",
            {"for (const auto& x : v) {"}, "for (declaration : container) {"), {
            L("Walking a vector", "A range-for is the cleanest way to visit each element. Use an index loop only when you need the position, and compare it with size() using a matching unsigned type.", "for (const auto& x : v) {",
              {{"const auto&", "read-only view of each element"}, {"x", "current element"}, {"v", "the vector"}}),
        }),

        WithLesson(WithCode(M("map & unordered_map", {
            Q("Which container keeps its keys sorted?", {"std::map", "std::unordered_map", "std::vector", "std::array"}, "std::map is ordered; unordered_map uses hashing."),
            Q("ages[\"Bob\"] when Bob is not a key does what?", {"Inserts Bob with a default value (0)", "Throws an exception", "Returns nullptr", "Does nothing"}, "operator[] creates a missing entry."),
            Q("Which checks for a key without inserting it?", {"m.find(key) != m.end()", "m[key] != 0", "m.has(key)", "m.exists(key)"}, "find returns end() when the key is absent."),
        }), "Set the entry \"Ann\" in map ages to 30 using brackets.", "ages[...] = ...;",
            {R"c(ages["Ann"] = 30;)c"}, "map[key] = value;"), {
            L("Key-value pairs", "std::map stores key/value pairs sorted by key. std::unordered_map is usually faster on average because it uses hashing but has no order. Use m[key] to read or insert, and find to test without inserting.", R"c(std::map<std::string, int> ages;)c",
              {{"std::map", "sorted key/value container"}, {"<std::string, int>", "key type, value type"}, {"ages", "the name"}}),
        }),

        WithLesson(WithCode(M("set & pair", {
            Q("std::set<int> s{3, 1, 3, 2}; What is s.size()?", {"3", "4", "2", "1"}, "A set stores each value once: {1, 2, 3}."),
            Q("Which gets the second member of pair p?", {"p.second", "p[1]", "p.2", "p->two"}, "first and second."),
            Q("In which order does a std::set iterate?", {"Sorted ascending by default", "Insertion order", "Random order", "Descending"}, "std::set is ordered."),
        }), "Insert the value 5 into set s.", "s.insert(...);",
            {"s.insert(5);"}, "insert adds a value."), {
            L("Unique values and pairs", "std::set stores unique values in sorted order. std::pair bundles two values, accessed as first and second. A map's elements are pairs.", "std::set<int> s{3, 1, 2};",
              {{"std::set", "unique sorted values"}, {"<int>", "element type"}, {"{3, 1, 2}", "iterates as 1, 2, 3"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("What does v.erase(v.begin()) remove?", {"The first element", "The last element", "All elements", "Nothing"}, "begin() is the first element."),
            Q("What is the average cost of a std::unordered_map lookup?", {"Constant time, O(1)", "O(n)", "O(log n)", "O(n log n)"}, "Hashing gives constant time on average."),
            Q("Which header declares std::map?", {"<map>", "<unordered_map>", "<dictionary>", "<tree>"}, "#include <map>"),
        }), "Declare a vector of ints named v with the values 1, 2, 3 using braces.", "std::vector<int> v{...};",
        {"std::vector<int> v{1, 2, 3};", "std::vector<int> v = {1, 2, 3};"}, "Braces list the initial elements.")));

    // ============================================================ 11  Algorithms & Lambdas
    T.push_back(CppTopic(10, "Algorithms & Lambdas", "Algorithms", "Let the standard library sort, search and count for you.", {
        WithLesson(WithCode(M("Iterators", {
            Q("What does v.end() refer to?", {"One position past the last element", "The last element", "The first element", "Nothing valid, it is null"}, "end() is a sentinel; never dereference it."),
            Q("How do you read the element an iterator it refers to?", {"*it", "it.value", "&it", "it()"}, "Dereference with *."),
            Q("Which loop walks a vector with iterators?", {"for (auto it = v.begin(); it != v.end(); ++it)", "for (auto it : v.begin())", "for (it in v)", "for (it = begin; it < end; it++)"}, "Start at begin(), stop at end()."),
        }), "Declare an iterator named it at the beginning of v using auto.", "auto it = ...;",
            {"auto it = v.begin();"}, "begin() gives the first position."), {
            L("Iterators", "An iterator points at an element of a container. begin() is the first position and end() is one past the last. * reads the element and ++ moves on. All standard algorithms are written in terms of begin/end ranges.", "auto it = v.begin();",
              {{"auto it", "an iterator"}, {"v.begin()", "points at the first element"}}),
        }),

        WithLesson(WithCode(M("sort & find", {
            Q("Which sorts vector v ascending?", {"std::sort(v.begin(), v.end());", "v.sort();", "std::sort(v);", "sort(v, asc);"}, "Pass the range to sort."),
            Q("What does std::find(v.begin(), v.end(), 9) return if 9 is absent?", {"v.end()", "nullptr", "-1", "false"}, "End means not found."),
            Q("Which header declares std::sort?", {"<algorithm>", "<sort>", "<vector>", "<utility>"}, "Most algorithms are in <algorithm>."),
        }), "Sort vector v into ascending order.", "std::sort(...);",
            {"std::sort(v.begin(), v.end());"}, "Pass begin and end."), {
            L("Algorithms", "<algorithm> provides std::sort, std::find, std::count, std::reverse and many more. They take a range given by two iterators, so they work on any container.", "std::sort(v.begin(), v.end());",
              {{"std::sort", "the algorithm"}, {"v.begin()", "start of the range"}, {"v.end()", "one past the end"}}),
        }),

        WithLesson(WithCode(M("Lambdas", {
            Q("What does [] at the start of a lambda do?", {"Introduces the capture list", "Declares an array", "Starts a comment", "Starts a template"}, "The capture list says which outer variables the lambda can use."),
            Q("Which sorts v in descending order?", {"std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });", "std::sort(v.begin(), v.end(), >);", "std::sort(v, desc);", "std::sort(v.begin(), v.end(), lambda a > b);"}, "The lambda is the comparison."),
            Q("Which capture copies every outer variable the lambda uses?", {"[=]", "[&]", "[]", "[this]"}, "[=] is capture by value, [&] by reference."),
        }), "Write a lambda stored in auto twice that takes int x and returns x * 2.", "auto twice = [](int x) { ... };",
            {"auto twice = [](int x) { return x * 2; };"}, "[](parameters) { body }"), {
            L("Lambdas", "A lambda is a small unnamed function written inline. The square brackets are the capture list, then come the parameters and the body. They are perfect for custom sorting and searching.", "auto isEven = [](int n) { return n % 2 == 0; };",
              {{"[]", "capture list: nothing captured"}, {"(int n)", "parameters"}, {"{ return n % 2 == 0; }", "the body"}}),
            L("Capturing", "[=] copies the outer variables the lambda uses. [&] refers to them directly. A lambda that outlives a captured reference will use a dead variable, so capture by value when in doubt.", "auto add = [offset](int x) { return x + offset; };",
              {{"[offset]", "copies offset into the lambda"}, {"(int x)", "parameter"}, {"x + offset", "uses the captured value"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("What does std::count(v.begin(), v.end(), 3) return?", {"How many elements equal 3", "The index of 3", "true or false", "The sum"}, "It counts matches."),
            Q("Why can capture by reference [&] be risky?", {"The lambda may outlive the captured variables", "It copies too much", "It is slower to compile", "It disables the lambda"}, "A dangling reference is undefined behaviour."),
            Q("Which header declares std::accumulate?", {"<numeric>", "<algorithm>", "<vector>", "<accumulate>"}, "Numeric algorithms live in <numeric>."),
        }), "Sum vector v starting from 0 into int total with std::accumulate.", "int total = std::accumulate(...);",
        {"int total = std::accumulate(v.begin(), v.end(), 0);"}, "accumulate(begin, end, start).")));

    // ============================================================ 12  Classes & Objects
    T.push_back(CppTopic(11, "Classes & Objects", "Classes", "Bundle data and behaviour together: classes, constructors and encapsulation.", {
        WithLesson(WithCode(M("struct vs class", {
            Q("What is the default access of a struct's members?", {"public", "private", "protected", "none"}, "struct members are public by default."),
            Q("What is the default access of a class's members?", {"private", "public", "protected", "inherited"}, "class members are private by default."),
            Q("Which creates a Point object named p?", {"Point p;", "new Point p;", "Point p = new;", "create Point p;"}, "Declare it like any variable."),
        }), "Declare a struct named Point with int members x and y, on one line, ending with };", "struct Point { ... };",
            {"struct Point { int x; int y; };"}, "struct Name { members };"), {
            L("Types of your own", "A struct or class defines a new type that groups data (members) and behaviour (functions). The only difference between them is the default access: public for struct, private for class.", "struct Point { int x; int y; };",
              {{"struct", "define a new type"}, {"Point", "its name"}, {"{ int x; int y; }", "the data members"}, {";", "a class definition ends with a semicolon"}}),
        }),

        WithLesson(WithCode(M("Constructors", {
            Q("A constructor's name is...", {"The class name", "init", "construct", "new"}, "Same as the class, with no return type."),
            Q("Why prefer a member initializer list?", {"It initialises members directly instead of assigning later", "It is mandatory", "It runs last", "It hides members"}, "Cleaner, and required for const and reference members."),
            Q("If you declare no constructor at all, the compiler...", {"Generates a default constructor", "Refuses to compile", "Generates none", "Makes the class abstract"}, "A default constructor is generated if you declare none."),
        }), "Write a constructor Point taking int x, storing it in member x_ through an initializer list, with an empty body.", "Point(int x) : ...",
            {"Point(int x) : x_(x) {}"}, "Name(params) : member(value) {}"), {
            L("Constructors", "A constructor runs when an object is created and sets it up. It has the class's name and no return type. A member initializer list, after the colon, initialises each member directly.", "Point(int x, int y) : x_(x), y_(y) {}",
              {{"Point", "same name as the class"}, {"(int x, int y)", "parameters"}, {": x_(x), y_(y)", "initializer list"}, {"{}", "empty body"}}),
        }),

        WithLesson(WithCode(M("Member Functions & const", {
            Q("What does const after a member function's parameter list mean?", {"It promises not to modify the object", "The return value is const", "The function is static", "The function is private"}, "A const member function cannot change members."),
            Q("Which can be called on a const Rectangle object?", {"A const member function", "Any member function", "Only constructors", "Only static functions"}, "Non-const functions might modify the object."),
            Q("What is this inside a member function?", {"A pointer to the current object", "A reference to the class", "The parent object", "A static keyword"}, "this points at the object the function was called on."),
        }), "Write the header of a const member function area returning int with no parameters, with an opening brace.", "int area() ... {",
            {"int area() const {"}, "const goes after the parameter list."), {
            L("const member functions", "Mark member functions that only read the object as const. They can then be called on const objects, and the compiler enforces your promise. Inside any member function, this points at the current object.", "int area() const { return w_ * h_; }",
              {{"int", "return type"}, {"area", "name"}, {"()", "no parameters"}, {"const", "promises not to modify the object"}}),
        }),

        WithLesson(WithCode(M("Encapsulation & static", {
            Q("Why make data members private?", {"To control how they change and keep the object valid", "It makes them faster", "The compiler requires it", "So they are copied"}, "Encapsulation protects invariants."),
            Q("A static data member is...", {"Shared by all objects of the class", "Stored once per object", "Always constant", "Local to a function"}, "There is a single copy for the whole class."),
            Q("Which is a getter?", {"int getAge() const { return age_; }", "void setAge(int a)", "Person(int a)", "~Person()"}, "A getter returns a value without changing the object."),
        }), "Write a const getter named getAge that returns int member age_, on one line.", "int getAge() const { ... }",
            {"int getAge() const { return age_; }"}, "Getters read, never modify."), {
            L("Encapsulation", "Keep data private and expose a small public interface. That lets you validate changes and alter the inside later without breaking users. A static member belongs to the class itself and is shared by every object.", "static int count;",
              {{"static", "one copy shared by all objects"}, {"int count", "the member"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("Which access specifier makes members visible to everyone?", {"public", "private", "protected", "friend"}, "public members are accessible from anywhere."),
            Q("What does Point p{1, 2}; require?", {"A matching constructor or public members to initialise", "A destructor", "A virtual function", "A template"}, "Braces initialise using a constructor or aggregate rules."),
            Q("A member function that does not change the object should be marked...", {"const", "static", "virtual", "explicit"}, "const documents and enforces it."),
        }), "Create a Point named p with x = 1 and y = 2 using braces.", "Point p{...};",
        {"Point p{1, 2};", "Point p = {1, 2};", "Point p(1, 2);"}, "Braces pass the arguments to the constructor.")));

    return T;
}
