#pragma once
#include "CppCourseSupport.h"

inline std::vector<Topic> CppTopicsA() {
    std::vector<Topic> T;

    // ============================================================ 1  Getting Started
    T.push_back(CppTopic(0, "Getting Started", "Start", "What C++ is, how a program is built, and your first lines of code.", {
        WithLesson(M("What is C++?", {
            Q("What does a C++ compiler do?", {"Translates source code into an executable program", "Runs your code line by line as you type", "Draws the window of your program", "Downloads libraries from the internet"},
              "The compiler translates the whole source file into machine code before the program ever runs."),
            Q("Which file extension is normally used for C++ source files?", {".cpp", ".cp++", ".java", ".exe"},
              ".cpp is the usual extension (.cc and .cxx also exist). .exe is the compiled result."),
            Q("C++ is a compiled language. What is one consequence?", {"Many mistakes are reported at build time, before the program runs", "Programs only run inside a browser", "You cannot use variables", "The code is re-translated every time a line runs"},
              "Because translation happens up front, the compiler can catch typos and type errors early."),
        }), {
            L("C++ is compiled", "You write source code in a .cpp file. A compiler translates it into object code and catches many type and syntax mistakes. A linker combines objects and libraries into an executable. Compilation allows optimisation, but program speed also depends on your algorithm."),
            L("Source to program", "A typical build has two steps: compile .cpp files into object files, then link those files and libraries into a runnable program. Fix the first reported error, build again, then run the program."),
        }),

        WithLesson(WithCode(M("Your First Program", {
            Q("Which function is where every C++ program starts running?", {"main", "start", "begin", "run"}, "Execution always begins at main."),
            Q("What does #include <iostream> do?", {"Makes input/output tools such as std::cout available", "Starts the program", "Creates a variable named iostream", "Saves the file"},
              "It pastes in the declarations for the standard input/output streams."),
            Q("Which line prints Hello followed by a new line?", {R"c(std::cout << "Hello\n";)c", R"c(cout("Hello");)c", R"c(System.out.println("Hello");)c", R"c(print Hello;)c"},
              "std::cout << sends text into the output stream. \\n is the new line character."),
        }), "Write the statement that prints Hello followed by a new line, with \\n inside the string.", "std::cout << ... ;",
            {R"c(std::cout << "Hello\n";)c", R"c(std::cout << "Hello" << '\n';)c", R"c(std::cout << "Hello" << std::endl;)c"},
            "std::cout << \"Hello\\n\"; sends the text, including the new line, to the screen."), {
            L("Printing text", "std::cout is the standard output stream. The << operator sends whatever follows it into the stream. Text goes in double quotes, and every statement ends with a semicolon.",
              R"c(std::cout << "Hello, World!\n";)c",
              {{"std::cout", "the output stream (your screen)"}, {"<<", "sends the next value into the stream"}, {R"c("Hello, World!\n")c", "the text. \\n means start a new line"}, {";", "every statement ends with a semicolon"}}),
        }),

        WithLesson(WithCode(M("Anatomy of main", {
            Q("What does return 0; at the end of main usually signal?", {"The program finished successfully", "The program crashed", "No errors were printed", "The program should start again"},
              "By convention 0 means success; non-zero values report a problem to the operating system."),
            Q("Which is the correct minimal program?", {"int main() { }", "void main() { }", "main() { }", "int main { }"},
              "Standard C++ requires main to return int. The other forms are not valid standard C++."),
            Q("What happens if you leave out return 0; at the end of main?", {"The program still returns 0 automatically", "A compile error", "The program never ends", "It returns 1"},
              "main is special: reaching its closing brace behaves like return 0;."),
        }), "Write a complete main function that returns 0, on one line.", "int main() { return 0; }",
            {"int main() { return 0; }", "int main() {}", "int main() { }"}, "main returns an int; 0 means the program succeeded."), {
            L("Every program has a main", "main is the entry point. Its return type is int, the empty parentheses mean it takes no inputs, and the braces hold the body. The value it returns is handed back to the operating system.",
              "int main() { return 0; }",
              {{"int", "main returns an integer"}, {"main", "the name the program starts from"}, {"()", "no parameters in this version"}, {"{ return 0; }", "the body. return 0 means success"}}),
        }),

        WithLesson(M("Comments & Errors", {
            Q("Which is a valid single-line comment in C++?", {"// note", "# note", "-- note", "<!-- note -->"}, "// starts a comment that runs to the end of the line."),
            Q("What does the compiler do with comments?", {"Ignores them", "Prints them", "Runs them", "Reports an error"}, "Comments are for humans. They are removed before compilation."),
            Q("A statement is missing its semicolon. What most likely happens?", {"The compiler reports an error", "The program prints nothing but compiles", "It crashes only at runtime", "Nothing, semicolons are optional"},
              "A declaration or expression statement such as int x = 2; needs its semicolon; control-flow blocks such as if (...) { ... } do not need one after the closing brace."),
        }), {
            L("Comments", "Use // for a comment that ends at the end of the line, and /* ... */ for a block. The compiler ignores them completely.", "// this line is ignored",
              {{"//", "starts a comment"}, {"this line is ignored", "everything to the end of the line is skipped"}}),
            L("Reading error messages", "Read the first error first: later ones are often side effects of it. The message names the file and line. The most common beginner errors are a missing semicolon, a misspelled name, and a missing #include."),
        }),
    },
        WithCode(M("Topic Test", {
            Q("Which header is needed for std::cout?", {"<iostream>", "<cout>", "<print>", "<console>"}, "std::cout lives in <iostream>."),
            Q("What ends a statement in C++?", {"A semicolon", "A period", "A colon", "A newline"}, "Statements end with ;."),
            Q("What is std::cout?", {"The standard output stream", "A variable you must declare", "A keyword", "A function that returns text"}, "It is an object provided by the standard library."),
            Q("Which error stops you from getting an executable at all?", {"A compile error", "A run that prints nothing", "A slow program", "A long variable name"}, "Compile errors stop the build."),
        }), "Write the include line for iostream.", "#include <...>", {"#include <iostream>"}, "#include <iostream> makes std::cout available.")));

    // ============================================================ 2  Variables & Types
    T.push_back(CppTopic(1, "Variables & Types", "Types", "Store numbers, letters, truth values and text, and let C++ check your types.", {
        WithLesson(WithCode(M("Declaring Variables", {
            Q("Which line declares an integer named score with the value 10?", {"int score = 10;", "score int = 10;", "int = score 10;", "let score = 10;"}, "C++ declarations are: type, name, initial value."),
            Q("What does an uninitialised local int contain?", {"An indeterminate value (garbage)", "Always 0", "Always -1", "Whatever the last variable held, guaranteed"}, "Reading it is a bug. Always give variables a value."),
            Q("Which line fails to compile because it would lose information?", {"int x{3.5};", "int x = 3.5;", "int x(3.5);", "int x; x = 3.5;"},
              "Brace initialisation forbids narrowing. The other forms silently truncate to 3."),
        }), "Declare an int named count with the value 10 using braces.", "int count{...};",
            {"int count{10};"}, "Brace initialisation is safe: it refuses conversions that lose data."), {
            L("Variables", "A variable is a named place in memory that holds a value of one type. Write the type first, then the name, then an optional starting value.", "int age = 18;",
              {{"int", "the type: a whole number"}, {"age", "the name you choose"}, {"=", "store the value on the right"}, {"18", "the initial value"}}),
            L("Always initialise", "A local variable you never give a value holds garbage. Prefer brace initialisation: it also refuses conversions that would lose information.", "int age{18};",
              {{"int", "the type"}, {"age", "the name"}, {"{18}", "the initial value in braces"}}),
        }),

        WithLesson(WithCode(M("Numbers: int & double", {
            Q("Which type is best for storing 3.14?", {"double", "int", "char", "bool"}, "int drops the decimals. double stores fractional values."),
            Q("Which statement about the size of int is portable C++?", {"It depends on the implementation; use sizeof(int)", "It is always 4 bytes", "It always takes 8 bytes", "It changes each time the program runs"}, "The standard specifies minimum ranges, not one size for every machine. sizeof(int) reports its size in bytes on your platform."),
            Q("Which literal is a float rather than a double?", {"2.5f", "2.5", "2.5d", "2.5F64"}, "A decimal literal is double unless it ends in f."),
        }), "Declare a double named price with the value 9.99.", "double price = ...;",
            {"double price = 9.99;", "double price{9.99};"}, "double holds decimal numbers."), {
            L("int and double", "int holds whole numbers. double holds decimals with about 15 digits of precision, and is the default choice for fractions. A literal with a decimal point is a double.", "double price = 9.99;",
              {{"double", "decimal number type"}, {"price", "variable name"}, {"9.99", "a double literal"}}),
        }),

        WithLesson(WithCode(M("char, bool & string", {
            Q("Which is a char literal?", {"'A'", "\"A\"", "A", "`A`"}, "Single quotes make a char. Double quotes make a string."),
            Q("What can a bool store?", {"true or false", "Any integer", "One letter", "Text"}, "bool holds exactly true or false."),
            Q("Which header provides std::string?", {"<string>", "<text>", "<chars>", "<words>"}, "Include <string> to use std::string."),
        }), "Declare a char named grade holding the letter A.", "char grade = ...;",
            {"char grade = 'A';", "char grade{'A'};"}, "A char literal uses single quotes."), {
            L("Three more types", "char holds one character in single quotes. bool holds true or false. std::string holds text in double quotes and needs #include <string>.", "char grade = 'A';",
              {{"char", "one character"}, {"grade", "the name"}, {"'A'", "single quotes for a char"}}),
        }),

        WithLesson(WithCode(M("const & auto", {
            Q("What happens if you assign to a const variable?", {"A compile error", "It changes anyway", "A runtime exception", "It is silently ignored"}, "const values cannot change after initialisation."),
            Q("What type does auto x = 3.0; deduce?", {"double", "int", "float", "auto"}, "3.0 is a double literal, so x is a double."),
            Q("Which is a valid const declaration?", {"const int max = 100;", "const max = 100 int;", "constant int max = 100;", "int const; max = 100;"}, "const goes with the type and needs a value."),
        }), "Declare a constant int named maxScore with the value 100.", "const int maxScore = ...;",
            {"const int maxScore = 100;", "const int maxScore{100};", "int const maxScore = 100;"}, "const makes the value read-only."), {
            L("const", "Mark values that must never change with const. The compiler then refuses any later assignment, which catches bugs for you.", "const double pi = 3.14159;",
              {{"const", "cannot change after this line"}, {"double", "the type"}, {"pi", "the name"}, {"3.14159", "the fixed value"}}),
            L("auto", "auto asks the compiler to work out the type from the initial value. It saves typing, but the value must be obvious from context.", "auto n = 42;",
              {{"auto", "deduce the type"}, {"n", "the name"}, {"42", "an int, so n is an int"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("Which declaration is invalid?", {"int 2x = 5;", "int x2 = 5;", "int _x = 5;", "int x_2 = 5;"}, "A name cannot start with a digit."),
            Q("What is the type of 'a' + 1?", {"int", "char", "std::string", "double"}, "Arithmetic promotes char to int, so the result is an int."),
            Q("Which prints a bool as true or false instead of 1 or 0?", {"std::cout << std::boolalpha << flag;", "std::cout << flag;", "std::cout << (string)flag;", "std::cout << true_false(flag);"}, "std::boolalpha switches bool output to words."),
        }), "Declare an auto variable named total with the value 2.5.", "auto total = ...;", {"auto total = 2.5;", "auto total{2.5};"}, "auto deduces double from 2.5.")));

    // ============================================================ 3  Input & Output
    T.push_back(CppTopic(2, "Input & Output", "I/O", "Talk to the user: print, read numbers and whole lines, and format results.", {
        WithLesson(WithCode(M("cout in Depth", {
            Q("What does std::endl do besides starting a new line?", {"Flushes the output buffer", "Closes the program", "Clears the screen", "Nothing else"}, "endl writes a newline and flushes, which can be slower than '\\n'."),
            Q("Which prints Hi and then a new line?", {R"c(std::cout << "Hi" << '\n';)c", R"c(std::cout < "Hi" < '\n';)c", R"c(std::cout >> "Hi";)c", R"c(std::cout("Hi\n");)c"}, "Use << to send values into cout."),
            Q("What does std::cout << 5 << 3; print?", {"53", "8", "5 3", "An error"}, "Each value is printed in turn with nothing between them."),
        }), "Print the variable score followed by a new line using '\\n' (chain with <<).", "std::cout << score << ... ;",
            {R"c(std::cout << score << '\n';)c", R"c(std::cout << score << "\n";)c", R"c(std::cout << score << std::endl;)c"}, "You can chain several << in one statement."), {
            L("Chaining output", "Each << returns the stream, so you can chain values. Mix text and variables freely. Use '\\n' for a new line; std::endl also flushes the buffer.", R"c(std::cout << "Age: " << age << '\n';)c",
              {{"std::cout", "the stream"}, {R"c("Age: ")c", "text first"}, {"age", "then a variable's value"}, {R"c('\n')c", "then a new line"}}),
        }),

        WithLesson(WithCode(M("Reading with cin", {
            Q("Which reads an int into age?", {"std::cin >> age;", "std::cin << age;", "std::input(age);", "age = cin();"}, ">> extracts from the stream into the variable."),
            Q("The user types 12 7 and you run std::cin >> a >> b;. What are a and b?", {"12 and 7", "127 and 0", "12 and 12", "Both 12"}, "cin skips whitespace and reads one value per >>."),
            Q("std::cin >> name for a std::string reads...", {"One word, up to whitespace", "The whole line", "One character", "Nothing"}, "Extraction stops at whitespace."),
        }), "Read a value from the keyboard into the variable age.", "std::cin >> ... ;",
            {"std::cin >> age;"}, "std::cin >> variable; reads one value."), {
            L("Input with cin", "std::cin is the input stream. The >> operator points from the stream into your variable, the opposite direction of cout. It reads one whitespace-separated value per >>.", "std::cin >> age;",
              {{"std::cin", "the keyboard input stream"}, {">>", "extract a value into..."}, {"age", "...this variable"}}),
        }),

        WithLesson(WithCode(M("Lines & Formatting", {
            Q("Which reads a whole line, spaces included, into std::string line?", {"std::getline(std::cin, line);", "std::cin >> line;", "std::cin.line(line);", "std::readline(line);"}, "getline reads up to the end of the line."),
            Q("After std::cin >> n;, a std::getline may return an empty line. Why?", {"The newline left in the buffer is read as an empty line", "getline cannot follow cin", "n was too large", "cin deletes the buffer"}, "Clear it with std::cin.ignore() before getline."),
            Q("Which prints 3.14159 as 3.14?", {"std::cout << std::fixed << std::setprecision(2) << x;", "std::cout << round(x, 2);", "std::cout << x.2;", "std::cout << std::precision(2) << x;"}, "fixed + setprecision(2) gives two digits after the point. Include <iomanip>."),
        }), "Read a whole line into the string name.", "std::getline(...);",
            {"std::getline(std::cin, name);"}, "getline(stream, string) reads a full line."), {
            L("Whole lines", "cin >> stops at whitespace. To read a sentence use std::getline(std::cin, text). If you used cin >> just before, a leftover newline can make getline return an empty line.", "std::getline(std::cin, name);",
              {{"std::getline", "read a full line"}, {"std::cin", "from the keyboard"}, {"name", "into this string"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("Which header is needed for std::setprecision?", {"<iomanip>", "<iostream>", "<format>", "<cmath>"}, "The manipulators live in <iomanip>."),
            Q("What does std::cout << 'A' + 1 print?", {"66", "A1", "B", "An error"}, "'A' is 65, and char + int is an int."),
            Q("Which statement about '\\n' and std::endl is correct?", {"'\\n' only ends the line; std::endl also flushes", "They are identical", "'\\n' flushes; std::endl does not", "std::endl is not allowed"}, "Flushing is the difference."),
        }), "Print the text Done followed by a new line with std::cout.", "std::cout << ... ;",
        {R"c(std::cout << "Done\n";)c", R"c(std::cout << "Done" << '\n';)c", R"c(std::cout << "Done" << std::endl;)c"}, "Text in double quotes, then a new line.")));

    // ============================================================ 4  Operators
    T.push_back(CppTopic(3, "Operators", "Operators", "Arithmetic, integer division, increments, comparisons and casts.", {
        WithLesson(WithCode(M("Arithmetic", {
            Q("What is 2 + 3 * 4?", {"14", "20", "24", "10"}, "Multiplication binds tighter than addition."),
            Q("What is 10 - 4 - 3?", {"3", "9", "-3", "7"}, "Subtraction is evaluated left to right: (10 - 4) - 3."),
            Q("Which operator gives the remainder of a division?", {"%", "/", "//", "mod"}, "% is the remainder operator."),
        }), "Declare int avg as the sum of a and b (in parentheses) divided by 2.", "int avg = (...) / 2;",
            {"int avg = (a + b) / 2;"}, "Parentheses force the addition first."), {
            L("Arithmetic operators", "+ - * / and % work as you expect. * / % bind tighter than + -. Use parentheses to make the order explicit.", "int area = (w + 2) * h;",
              {{"(w + 2)", "calculated first because of the parentheses"}, {"*", "then multiplied by h"}}),
        }),

        WithLesson(WithCode(M("Division & Modulo", {
            Q("What is 7 / 2 when both are ints?", {"3", "3.5", "4", "2"}, "Integer division drops the fractional part."),
            Q("What is -7 / 2?", {"-3", "-4", "-3.5", "3"}, "Integer division truncates toward zero."),
            Q("What is 17 % 5?", {"2", "3", "3.4", "12"}, "17 = 3 * 5 + 2."),
        }), "Declare double result as 5 divided by 2.0.", "double result = 5 / ...;",
            {"double result = 5 / 2.0;", "double result = 5.0 / 2;", "double result = 5.0 / 2.0;"}, "One operand must be a double to get 2.5."), {
            L("Integer division", "When both operands are integers the result is an integer: 7 / 2 is 3 and 7 % 2 is 1. Make at least one operand a double to get a fractional answer.", "double r = 7 / 2.0;",
              {{"7", "an int"}, {"/", "divide"}, {"2.0", "a double, so the whole division is a double: 3.5"}}),
        }),

        WithLesson(WithCode(M("Compound & Increment", {
            Q("int x = 5; int y = x++;  What are x and y?", {"x = 6, y = 5", "x = 6, y = 6", "x = 5, y = 6", "x = 5, y = 5"}, "Postfix returns the old value, then increments."),
            Q("int x = 5; int y = ++x;  What are x and y?", {"x = 6, y = 6", "x = 6, y = 5", "x = 5, y = 6", "x = 5, y = 5"}, "Prefix increments first, then returns the new value."),
            Q("What does x *= 3 do?", {"Multiplies x by 3 and stores the result in x", "Prints 3 times x", "Compares x with 3", "Declares x"}, "x *= 3 is x = x * 3."),
        }), "Increase total by 10 using a compound assignment.", "total ... 10;",
            {"total += 10;"}, "+= adds and stores."), {
            L("Shortcuts", "x += 5 means x = x + 5. ++x and x++ both add 1, but ++x gives the new value while x++ gives the old one.", "int y = x++;",
              {{"y", "receives the old value of x"}, {"x++", "gives the old value of x, then adds 1"}}),
        }),

        WithLesson(WithCode(M("Comparison & Logic", {
            Q("Which operator tests equality?", {"==", "=", "===", "equals"}, "= assigns, == compares."),
            Q("What is true || false && false?", {"true", "false", "An error", "It depends on spacing"}, "&& binds tighter than ||, so it is true || (false && false)."),
            Q("What does if (x = 5) do for an int x?", {"Assigns 5 to x, and the condition is true", "Compares x with 5", "Always a compile error", "The condition is false"}, "A classic bug: = assigns, and 5 is non-zero so it is true."),
        }), "Write the condition: n is between 1 and 10 inclusive (just the if line, no body).", "if (n >= 1 && ...)",
            {"if (n >= 1 && n <= 10)"}, "Both comparisons must be true."), {
            L("Comparing and combining", "Comparisons (== != < > <= >=) give bool. Combine with && (and), || (or) and ! (not). && and || stop as soon as the answer is known.", "if (age >= 18 && hasId)",
              {{"age >= 18", "comparison"}, {"&&", "and: both sides must be true"}, {"hasId", "a bool variable"}}),
        }),

        WithLesson(WithCode(M("Casting", {
            Q("Which is the preferred way to convert between numeric types?", {"static_cast<double>(x)", "(double)x everywhere", "double::cast(x)", "to_double(x)"}, "static_cast is explicit and searchable."),
            Q("int a = 7, b = 2;  What is static_cast<double>(a) / b?", {"3.5", "3", "4", "2"}, "One operand is a double, so the division is real division."),
            Q("What does int n = static_cast<int>(3.99); give?", {"3", "4", "3.99", "An error"}, "Converting to int truncates the fraction."),
        }), "Convert the int total to a double using static_cast (just the expression).", "static_cast<...>(total)",
            {"static_cast<double>(total)", "static_cast<double>(total);"}, "static_cast<type>(value) converts explicitly."), {
            L("Explicit conversion", "static_cast<T>(value) converts a value to type T and shows your intent. Use it to avoid accidental integer division or silent truncation.", "static_cast<double>(sum) / count",
              {{"static_cast<double>", "convert to double"}, {"(sum)", "the value to convert"}, {"/ count", "now a real division"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("int a = 17; a %= 5;  What is a?", {"2", "3", "12", "0"}, "17 % 5 is 2."),
            Q("Which expression is true?", {"5 / 2 == 2", "5 / 2 == 2.5", "5 % 2 == 2", "5 / 2 == 3"}, "Integer division gives 2."),
            Q("What does !true && false evaluate to?", {"false", "true", "An error", "1 as text"}, "!true is false, and false && false is false."),
            Q("x -= 2 is the same as...", {"x = x - 2", "x = 2 - x", "x == x - 2", "x = -2"}, "Compound assignment subtracts and stores."),
        }), "Declare int result as the remainder of 29 divided by 6.", "int result = ...;", {"int result = 29 % 6;"}, "% gives the remainder.")));

    // ============================================================ 5  Conditionals
    T.push_back(CppTopic(4, "Conditionals", "Decisions", "Make your program choose: if, else if, switch and the ternary operator.", {
        WithLesson(WithCode(M("if / else", {
            Q("Which if statement is valid C++?", {"if (x > 5) { }", "if x > 5 { }", "if (x > 5) then { }", "if [x > 5] { }"}, "The condition must be in parentheses."),
            Q("What prints?  int x = 3; if (x > 5) std::cout << 'A'; std::cout << 'B';", {"B", "AB", "A", "Nothing"}, "Without braces, if only controls the next statement."),
            Q("Why are braces recommended even for a one-line body?", {"They prevent bugs when you add a second line later", "They make the program faster", "The compiler requires them", "They declare a variable"}, "A second line would silently fall outside the if."),
        }), "Write the start of an if that checks temp is above 30, ending with an opening brace.", "if (...) {",
            {"if (temp > 30) {"}, "if (condition) {"), {
            L("Choosing with if", "if runs its body only when the condition in parentheses is true. Add else for the other case. Always use braces so the extent of the body is clear.", "if (score >= 50) {",
              {{"if", "start a decision"}, {"(score >= 50)", "the condition"}, {"{", "the body begins"}}),
        }),

        WithLesson(WithCode(M("else if Chains", {
            Q("In an if / else if / else chain, how many branches run?", {"At most one", "Every true one", "Exactly two", "All of them"}, "The first true condition wins; the rest are skipped."),
            Q("int n = 10; if (n > 5) A; else if (n > 8) B; else C;  What runs?", {"A only", "A and B", "B only", "C"}, "n > 5 is true first, so only A runs."),
            Q("When does the final else run?", {"When no earlier condition was true", "Always", "When the first condition is true", "Never"}, "else is the fallback."),
        }), "Write the line that starts a second branch: else if score is at least 60, ending with an opening brace.", "else if (...) {",
            {"else if (score >= 60) {", "} else if (score >= 60) {"}, "else if (condition) {"), {
            L("Chains", "Test several cases in order with else if. Put the most specific or most likely condition first, because the first true branch ends the chain.", "} else if (score >= 60) {",
              {{"}", "closes the previous branch"}, {"else if", "try another condition"}, {"(score >= 60)", "only checked if all earlier ones were false"}}),
        }),

        WithLesson(WithCode(M("switch", {
            Q("What happens if you forget break in a case?", {"Execution falls through into the next case", "The program stops", "A compile error", "Only default runs"}, "Without break, execution continues into the next case."),
            Q("Which types can a switch test?", {"Integers, chars and enums", "Any std::string", "doubles", "Any object"}, "switch needs an integral or enum value."),
            Q("Which label handles values that no case matches?", {"default", "else", "otherwise", "other"}, "default is the catch-all."),
        }), "Write the label that handles every unmatched value (including the colon).", "...:",
            {"default:"}, "default: is the catch-all case."), {
            L("switch", "switch compares one integer or enum against several case labels. End each case with break unless you mean to fall through. default handles everything else.", "case 1: break;",
              {{"case 1:", "runs when the value is 1"}, {"break;", "stops execution from running into the next case"}}),
        }),

        WithLesson(WithCode(M("Ternary & Short-Circuit", {
            Q("int r = (5 > 3) ? 10 : 20;  What is r?", {"10", "20", "5", "3"}, "The condition is true, so the first value is chosen."),
            Q("Why is if (p != nullptr && p->ok) safe?", {"&& skips the right side when the left side is false", "The compiler checks p", "p is always valid", "-> never crashes"}, "Short-circuit evaluation avoids dereferencing a null pointer."),
            Q("Which is the ternary operator?", {"?:", "??", "::", "->"}, "condition ? a : b."),
        }), "Assign to int bigger the larger of a and b using a ternary.", "int bigger = ... ? a : b;",
            {"int bigger = (a > b) ? a : b;", "int bigger = a > b ? a : b;"}, "condition ? valueIfTrue : valueIfFalse."), {
            L("The ternary operator", "condition ? a : b is an expression that gives a when the condition is true and b otherwise. It is handy for small choices. Use if/else when the logic grows.", "int r = (x > 0) ? 1 : -1;",
              {{"(x > 0)", "the condition"}, {"1", "result if true"}, {"-1", "result if false"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("Which condition checks that x is NOT equal to 5?", {"x != 5", "x =! 5", "x <> 5", "!x == 5"}, "!x == 5 would negate x first."),
            Q("What does if (0) do?", {"The body never runs", "The body runs", "A compile error", "The body runs once"}, "0 converts to false."),
            Q("What prints?  int x = 7; if (x % 2 == 0) std::cout << \"even\"; else std::cout << \"odd\";", {"odd", "even", "evenodd", "Nothing"}, "7 % 2 is 1, so the else branch runs."),
        }), "Assign to auto label the text \"even\" when n % 2 == 0, otherwise \"odd\" (ternary).", "auto label = ... ? ... : ...;",
        {R"c(auto label = n % 2 == 0 ? "even" : "odd";)c", R"c(auto label = (n % 2 == 0) ? "even" : "odd";)c"}, "A ternary picks one of two values.")));

    // ============================================================ 6  Loops
    T.push_back(CppTopic(5, "Loops", "Loops", "Repeat work with while, for, do-while and range-for.", {
        WithLesson(WithCode(M("while", {
            Q("How many times does the body run?  int i = 0; while (i < 3) { ++i; }", {"3", "2", "4", "Forever"}, "i goes 0, 1, 2 and then the condition fails."),
            Q("What causes an infinite loop?", {"The condition never becomes false", "Using ++i", "Using braces", "An empty line"}, "Something inside must eventually make the condition false."),
            Q("When is a while condition checked?", {"Before each iteration", "After each iteration", "Only once", "Never"}, "A while loop may run zero times."),
        }), "Write the while header that loops while n is greater than 0, with an opening brace.", "while (...) {",
            {"while (n > 0) {"}, "while (condition) {"), {
            L("while", "A while loop repeats its body as long as the condition is true. The condition is checked before every pass, so the body might never run.", "while (n > 0) {",
              {{"while", "repeat while..."}, {"(n > 0)", "...this condition is true"}, {"{", "loop body starts"}}),
        }),

        WithLesson(WithCode(M("for", {
            Q("What are the three parts of a for header, in order?", {"initialise; condition; update", "condition; initialise; update", "update; condition; initialise", "initialise, condition, update"}, "They are separated by semicolons."),
            Q("How many times does for (int i = 1; i <= 10; i += 3) run?", {"4", "3", "10", "5"}, "i takes the values 1, 4, 7 and 10."),
            Q("What is the scope of i declared in for (int i = 0; ...)?", {"Only the loop", "The whole function", "The whole file", "Nowhere"}, "A variable declared in the header exists only for the loop."),
        }), "Write a for header that counts i from 0 to 9, ending with an opening brace.", "for (int i = 0; ...; ...) {",
            {"for (int i = 0; i < 10; ++i) {", "for (int i = 0; i < 10; i++) {"}, "for (init; condition; update) {"), {
            L("for", "Use for when you know how many times to repeat. The header has three parts: where to start, when to keep going, and how to step.", "for (int i = 0; i < 5; ++i)",
              {{"int i = 0", "start: runs once"}, {"i < 5", "keep going while true"}, {"++i", "step: runs after each pass"}}),
        }),

        WithLesson(WithCode(M("do-while & range-for", {
            Q("Which loop always runs its body at least once?", {"do-while", "while", "for", "range-for"}, "do-while checks its condition after the body."),
            Q("Which header lets you modify every element of a std::vector<int> v?", {"for (int& x : v)", "for (int x : v)", "for (const int& x : v)", "for (x in v)"}, "A non-const reference refers to the real element."),
            Q("Why write const auto& in a range-for over strings?", {"It avoids copying each string and prevents modification", "It skips elements", "It converts them to int", "It is required syntax"}, "A const reference gives read-only access without a copy."),
        }), "Write a range-for header over names, reading each element as const auto& name (with an opening brace).", "for (const auto& ... : ...) {",
            {"for (const auto& name : names) {"}, "for (declaration : container)"), {
            L("range-for", "A range-for visits every element of a container without indexes. Use const auto& to read without copying, and plain auto& to modify elements.", "for (const auto& item : items)",
              {{"const auto&", "read-only, no copy"}, {"item", "the name for the current element"}, {"items", "the container being walked"}}),
        }),

        WithLesson(WithCode(M("break & continue", {
            Q("What does break do inside a loop?", {"Exits the loop immediately", "Skips to the next iteration", "Ends the program", "Restarts the loop"}, "break leaves the innermost loop."),
            Q("What does continue do?", {"Skips the rest of this iteration", "Exits the loop", "Ends the function", "Pauses the program"}, "It jumps to the next iteration."),
            Q("A break inside the inner of two nested loops exits...", {"Only the inner loop", "Both loops", "The function", "Nothing"}, "break affects only the innermost loop."),
        }), "Write the statement that skips to the next loop iteration.", "...;",
            {"continue;"}, "continue; jumps to the next iteration."), {
            L("Steering loops", "break leaves the loop. continue skips the rest of the current pass and moves to the next. Both only affect the innermost loop.", "if (i == 5) break;",
              {{"if (i == 5)", "when i reaches 5"}, {"break;", "leave the loop"}}),
        }),
    },
        WithCode(M("Topic Test", {
            Q("What does this print?  for (int i = 0; i < 3; ++i) std::cout << i;", {"012", "123", "0123", "321"}, "i takes the values 0, 1, 2."),
            Q("Which loop fits best when the number of repetitions is known in advance?", {"for", "do-while", "while (true)", "goto"}, "for keeps start, test and step together."),
            Q("What is sum?  int sum = 0; for (int i = 1; i <= 4; ++i) sum += i;", {"10", "4", "6", "24"}, "1 + 2 + 3 + 4 = 10."),
        }), "Write a statement that exits the loop when i equals 5.", "if (...) break;",
        {"if (i == 5) break;", "if (i == 5) { break; }"}, "break leaves the loop.")));

    return T;
}
