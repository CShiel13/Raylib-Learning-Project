#pragma once
#include "../CourseModel.h"

inline std::vector<Topic> BuildCSharpTopics() {
    std::vector<Topic> T;

    // ---------------------------------------------------------------- 1
    T.push_back(MakeTopic("C# Basics", "Basics", "CLR, value vs reference types, var, and nullable references.",
        Color{ 155,79,150,255 }, 0,
        {
            WithLesson(WithCode(M("Hello, C#", {
                Q("Which method prints a line to the console?", {"Console.WriteLine(\"Hi\");", "print(\"Hi\");", "printf(\"Hi\");", "System.out.println(\"Hi\");"}, "Console.WriteLine prints a line."),
                Q("Which file extension do C# source files use?", {".cs", ".c#", ".csx", ".sharp"}, ".cs is standard."),
                Q("What does CLR stand for?", {"Common Language Runtime", "C# Language Runtime", "Central Language Runtime", "Core Level Runtime"}, "The CLR is .NET's virtual machine."),
            }), "Write a statement that prints Hello using Console.WriteLine.", "Console.WriteLine(...);",
            {"Console.WriteLine(\"Hello\");"}, "Console.WriteLine prints and adds a newline."), {
                {"Printing with Console.WriteLine",
                 "Console is the .NET console class. WriteLine is a method on it that writes text and adds a newline. Statements end with a semicolon.",
                 "Console.WriteLine(\"Hello\");",
                 {{"Console", "the .NET console class"},
                  {"WriteLine", "writes text plus a newline"},
                  {"\"Hello\"", "the text - in double quotes"},
                  {";", "ends the statement"}}},
                {".cs source files",
                 "C# source code is saved in .cs files. A project groups multiple .cs files together into one assembly.",
                 "Program.cs",
                 {{".cs", "the C# source file extension"}}},
                {"The CLR",
                 "C# compiles to CIL (Common Intermediate Language). The CLR - Common Language Runtime - runs that CIL, handling memory, loading assemblies, and JIT-compiling to machine code. It's .NET's version of Java's JVM.",
                 "// C# runs on the CLR",
                 {{"CLR", "the Common Language Runtime - .NET's virtual machine"}}},
                {"Your turn",
                 "The next question asks you to build Console.WriteLine(\"Hello\");. Structure: Console, then .WriteLine, then the text in quotes, then a semicolon.",
                 "", {}},
            }),

            WithLesson(WithCode(M("Value vs Reference Types", {
                Q("Which of these is a value type?", {"int", "string", "List<int>", "object"}, "Primitives like int are value types."),
                Q("Which of these is a reference type?", {"object", "int", "bool", "char"}, "object is the root reference type."),
                Q("What does the ? on string? mean?", {"Nullable reference type", "Optional field", "Static string", "Read-only string"}, "The compiler warns on possible null dereference."),
            }), "Declare an integer variable x with value 5.", "int name = value;",
            {"int x = 5;"}, "int x = 5;"), {
                {"Value types",
                 "A value type holds its data directly inside the variable. int, bool, char, double and structs are all value types.",
                 "int x = 5;",
                 {{"int", "a value type - data lives right here"},
                  {"x", "the name"},
                  {"5", "the value, stored in the variable"}}},
                {"Reference types",
                 "A reference type stores a pointer, not the data itself. The actual object lives on the heap. All class types - including object, string and List - are reference types.",
                 "object o = new object();",
                 {{"object", "a reference type - the root of all class types"},
                  {"new object()", "the actual object lives on the heap"}}},
                {"The nullable ?",
                 "Appending ? to a reference type means it can also be null. This lets the compiler warn you about possible null dereferences.",
                 "string? name = null;",
                 {{"string?", "a string that can be null"},
                  {"name", "the variable"},
                  {"null", "the null value"}}},
                {"Your turn",
                 "The next question asks you to declare int x = 5. Type, name, equals, value, semicolon.",
                 "", {}},
            }),
        },
        WithCode(M("Topic Test", {
            Q("What must `var` always have?", {"An initializer", "A type annotation", "A public modifier", "A static modifier"}, "var requires an initializer so the compiler can infer the type."),
            Q("Which declares a nullable int?", {"int? x = null;", "int x = null;", "int x? = null;", "nullable<int> x = null;"}, "int? is shorthand for Nullable<int>."),
            Q("What is CIL?", {"Common Intermediate Language", "C# Intermediate Language", "Compiled Intermediate Layer", "Core Interop Library"}, "Assemblies contain CIL, which the JIT compiles."),
            }), "Print the result of 2 + 3.", "Console.WriteLine(...)",
            { "Console.WriteLine(2 + 3);", "Console.WriteLine(2+3);" }, "Console.WriteLine accepts any expression.")
            ));

    // ---------------------------------------------------------------- 2
    T.push_back(MakeTopic("Control Flow & Loops", "Flow", "if, switch expressions, loops and pattern matching.",
        Color{ 175,89,180,255 }, 40,
        {
            WithCode(M("if / else & switch", {
                Q("Which is valid C#?", {"if (x > 5) { }", "if x > 5 then { }", "if (x > 5) then { }", "if x > 5 { }"}, "Conditions go in parentheses."),
                Q("Which is the modern switch style?", {"value switch { ... }", "switch(value) return", "case (value)", "match-case"}, "C# 8+ switch expressions."),
                Q("Which label handles no matching case?", {"default:", "else:", "otherwise:", "case none:"}, "default: is the fallback."),
            }), "Print \"big\" if x is greater than 10 (one-line if).", "if (...) Console.WriteLine(...);",
            {"if (x > 10) Console.WriteLine(\"big\");", "if(x>10)Console.WriteLine(\"big\");"}, "A single if around Console.WriteLine."),

            WithCode(M("Loops & Pattern Matching", {
                Q("What does foreach iterate over?", {"Any IEnumerable", "Only arrays", "Only lists", "Only strings"}, "foreach works over anything enumerable."),
                Q("Which loop runs its body at least once?", {"do-while", "while", "for", "foreach"}, "do-while checks after the body."),
                Q("Which keyword pattern-matches and binds a variable?", {"is", "as", "in", "let"}, "is (pattern) binds within a scope."),
            }), "Write the header of a while loop that runs while i is less than 5 (include the opening brace).", "while (...) {",
            {"while (i < 5) {", "while(i<5){"}, "while (condition) { ... }"),
        },
        WithCode(M("Topic Test", {
            Q("What does break do?", {"Exits the loop immediately", "Skips to the next iteration", "Restarts the loop", "Exits the program"}, "break leaves the loop."),
            Q("Which is a switch-expression syntax?", {"result = value switch { ... };", "result = switch(value) { ... };", "switch result = value { ... };", "match(value) { ... }"}, "value switch { patterns -> results }."),
            Q("What does continue do?", {"Skips to the next iteration", "Exits the loop", "Throws an exception", "Restarts the loop"}, "continue jumps to the next pass."),
            }), "Write the header of a for loop counting i = 0..9 (include the opening brace).", "for (...; ...; ...) {",
            { "for (int i = 0; i < 10; i++) {", "for(int i=0;i<10;i++){" }, "for (init; cond; step) {")
            ));

    // ---------------------------------------------------------------- 3
    T.push_back(MakeTopic("Methods", "Methods", "Strings, StringBuilder, interpolation, overloading and ref.",
        Color{ 195,99,190,255 }, 70,
        {
            WithCode(M("Strings & Interpolation", {
                Q("Which prefix enables string interpolation?", {"$", "@", "#", "%"}, "The $ prefix."),
                Q("Are C# strings mutable?", {"No", "Yes", "Only with StringBuilder", "Only ASCII ones"}, "Strings are immutable."),
                Q("Which class builds strings efficiently?", {"StringBuilder", "String", "StringBuffer", "TextBuilder"}, "System.Text.StringBuilder."),
            }), "Create a string s that interpolates 'Hello, ' followed by the variable name.", "$\"Hello, {name}\"",
            {"string s = $\"Hello, {name}\";", "string s = $\"Hello, {name}\""}, "$\"Hello, {name}\""),

            WithCode(M("Methods & Modifiers", {
                Q("Which return type means 'returns nothing'?", {"void", "null", "None", "empty"}, "void."),
                Q("Method overloading requires...", {"Different parameter lists", "Different return types only", "Different names", "Different access modifiers"}, "Overloads differ by signature."),
                Q("What does the ref keyword do?", {"Passes the argument by reference", "Makes it read-only", "Creates a copy", "Deletes the argument"}, "ref shares the caller's variable."),
            }), "Define a public static method Square that takes int x and returns x*x (one line).", "public static int ...(int x) { ... }",
            {"public static int Square(int x) { return x * x; }", "public static int Square(int x){return x*x;}"}, "public static int Square(int x) { return x * x; }"),
        },
        WithCode(M("Topic Test", {
            Q("What does s.Length return?", {"Number of characters", "Number of words", "Number of bytes", "The string itself"}, "Length is the character count."),
            Q("Which is a raw string literal (C# 11+)?", {"\"\"\"...\"\"\"", "@\"...\"", "$...$", "'...'"}, "Triple quotes form a raw literal."),
            Q("What does ToUpper() return?", {"A new uppercase string", "Mutates in place", "A bool", "The original string"}, "Strings are immutable - a new string is returned."),
            }), "Call the greet method with no arguments (end with ;).", "greet...;",
            { "greet();" }, "method(args);")
            ));

    // ---------------------------------------------------------------- 4
    T.push_back(MakeTopic("Classes & Objects", "Classes", "Auto-properties, structs and records.",
        Color{ 215,109,190,255 }, 100,
        {
            WithCode(M("Classes & Properties", {
                Q("Which is an auto-property?", {"public int Age { get; set; }", "public int Age;", "public int get_Age() { }", "public Age { }"}, "The compiler generates a backing field."),
                Q("What is the default member access inside a class?", {"private", "public", "protected", "internal"}, "Class members default to private."),
                Q("A constructor's name is...", {"The class name", "init", "new", "construct"}, "Same as the class, no return type."),
            }), "Declare an empty class named Dog.", "class Name { ... }",
            {"class Dog { }", "class Dog{}"}, "class Dog { }"),

            WithCode(M("Structs & Records", {
                Q("A struct is a...", {"Value type", "Reference type", "Interface", "Delegate"}, "Structs are value types."),
                Q("A record provides...", {"Value-based equality", "Reference-based equality", "Automatic locking", "Only static methods"}, "record generates equality on all fields."),
                Q("A record struct is a...", {"Value type record", "Reference type record", "Interface", "Delegate"}, "record struct combines both."),
            }), "Declare a positional record Person with a string Name.", "record Person(...);",
            {"record Person(string Name);", "public record Person(string Name);"}, "record Person(string Name);"),
        },
        WithCode(M("Topic Test", {
            Q("What does internal mean?", {"Accessible within the same assembly", "Accessible everywhere", "Accessible in subclasses", "Private to the class"}, "internal = same assembly only."),
            Q("What is the default value of a struct's fields?", {"Zero / false / null", "Uninitialized", "A compile error", "Undefined"}, "Value types default to zero-init."),
            Q("Which is a plain class, not a record?", {"class Person { }", "record Person;", "record struct Point;", "record Person(string Name);"}, "Plain classes don't add record semantics."),
            }), "Create a Cat object named c using new.", "Cat c = new ...;",
            { "Cat c = new Cat();" }, "ClassName var = new ClassName();")
            ));

    // ---------------------------------------------------------------- 5
    T.push_back(MakeTopic("Inheritance & Interfaces", "Inherit", "virtual/override, abstract and interfaces.",
        Color{ 235,119,190,255 }, 140,
        {
            WithCode(M("Inheritance", {
                Q("How do you inherit from Animal?", {"class Dog : Animal { }", "class Dog extends Animal", "class Dog inherits Animal", "class Dog(Animal)"}, "Colon + base type."),
                Q("Which keyword lets a base method be overridden?", {"virtual", "override", "abstract", "sealed"}, "Mark the base method virtual."),
                Q("Which keyword replaces a base method?", {"override", "virtual", "new", "implements"}, "override replaces the base implementation."),
            }), "Write the class header: Dog inherits from Animal (include opening brace).", "class ... : ... {",
            {"class Dog : Animal {", "class Dog:Animal{"}, "class Derived : Base {"),

            WithCode(M("Interfaces", {
                Q("How do you implement IPet?", {"class Cat : IPet { }", "class Cat implements IPet", "class Cat inherits IPet", "class Cat(IPet)"}, "Interfaces go after the colon too."),
                Q("Can a class implement multiple interfaces?", {"Yes, any number", "No, only one", "Only two", "Only with generics"}, "A class implements any number of interfaces."),
                Q("A default interface method...", {"Has a body and can be overridden", "Is abstract only", "Cannot be called", "Is a constructor"}, "DIMs allow implementation in the interface."),
            }), "Declare an interface named IPet (include opening brace).", "interface ... {",
            {"interface IPet {", "interface IPet{"}, "interface Name { ... }"),
        },
        WithCode(M("Topic Test", {
            Q("What does base refer to?", {"The parent class", "The current class", "The assembly", "The namespace"}, "base reaches the parent."),
            Q("What does abstract mean for a method?", {"No body; subclasses must implement", "Final", "Static", "Private"}, "Abstract methods have no body."),
            Q("Can a class inherit multiple base classes?", {"No", "Yes, any number", "Only two", "Only via interfaces"}, "Single class inheritance only."),
            }), "Call the parent class's Speak() method from an overriding method.", "base....()",
            { "base.Speak();" }, "base.Method();")
            ));

    // ---------------------------------------------------------------- 6
    T.push_back(MakeTopic("Collections & Generics", "Generics", "List<T>, Dictionary, HashSet and generic constraints.",
        Color{ 190,60,170,255 }, 180,
        {
            WithCode(M("Collections", {
                Q("Which is a resizable array?", {"List<T>", "Array", "ArrayList", "Span<T>"}, "List<T> grows automatically."),
                Q("Which stores key-value pairs?", {"Dictionary<TKey, TValue>", "List<T>", "HashSet<T>", "Queue<T>"}, "Dictionary is a hash map."),
                Q("Which stores unique elements?", {"HashSet<T>", "List<T>", "Queue<T>", "Dictionary<TKey,TValue>"}, "HashSet rejects duplicates."),
            }), "Create an empty List<int> named nums.", "var nums = new ...",
            {"var nums = new List<int>();", "List<int> nums = new List<int>();"}, "new List<int>()"),

            WithCode(M("Generics", {
                Q("Which declares a type parameter?", {"<T>", "[T]", "(T)", "{T}"}, "Angle brackets."),
                Q("Which is a generic constraint?", {"where T : class", "where T is class", "constrain T", "T : class"}, "The where clause."),
                Q("Generics provide...", {"Compile-time type safety", "Runtime-only type checks", "Faster parsing", "Dynamic typing"}, "Type safety without casts."),
            }), "Create an empty Dictionary<string, int> named ages.", "var ages = new ...",
            {"var ages = new Dictionary<string, int>();", "Dictionary<string, int> ages = new Dictionary<string, int>();"}, "new Dictionary<string, int>()"),
        },
        WithCode(M("Topic Test", {
            Q("Which is thread-safe?", {"ConcurrentDictionary<TKey, TValue>", "Dictionary<TKey, TValue>", "List<T>", "Queue<T>"}, "Concurrent collections are thread-safe."),
            Q("Queue<T> is...", {"FIFO", "LIFO", "Sorted", "Random"}, "First-in, first-out."),
            Q("Stack<T> is...", {"LIFO", "FIFO", "Sorted", "Random"}, "Last-in, first-out."),
            }), "Create an empty HashSet<int> named ids.", "var ids = new ...",
            { "var ids = new HashSet<int>();", "HashSet<int> ids = new HashSet<int>();" }, "new HashSet<int>()")
            ));

    // ---------------------------------------------------------------- 7
    T.push_back(MakeTopic("LINQ & Lambdas", "LINQ", "Delegates, Action/Func, and query/method LINQ syntax.",
        Color{ 220,80,140,255 }, 230,
        {
            WithCode(M("Delegates & Lambdas", {
                Q("Which is a valid lambda?", {"x => x * 2", "lambda x: x * 2", "(x) -> x * 2", "func(x) => x * 2"}, "The => operator."),
                Q("Action<T> returns...", {"void", "T", "bool", "int"}, "Action returns void."),
                Q("Func<T, R> returns...", {"R", "T", "void", "bool"}, "Func returns the last type parameter."),
            }), "Write a lambda that doubles x.", "x => ...",
            {"x => x * 2", "x => 2 * x"}, "x => x * 2"),

            WithCode(M("LINQ", {
                Q("Which LINQ method filters?", {"Where", "Select", "Filter", "Find"}, "LINQ uses Where for filtering."),
                Q("Which LINQ method transforms?", {"Select", "Where", "Map", "Apply"}, "LINQ uses Select for projection."),
                Q("Which forces immediate execution?", {"ToList()", "Where()", "Select()", "AsQueryable()"}, "ToList materializes the query."),
            }), "Filter sequence xs to positive numbers using LINQ.", "xs.Where(...)",
            {"xs.Where(x => x > 0)", "xs.Where(x => x > 0);"}, "xs.Where(x => x > 0)"),
        },
        WithCode(M("Topic Test", {
            Q("What does deferred execution mean?", {"The query runs when enumerated", "It runs immediately", "It never runs", "It runs on a background thread"}, "LINQ queries are lazy."),
            Q("Query syntax begins with...", {"from x in source", "select from x", "query x in source", "foreach x in source"}, "Query expressions start with from."),
            Q("Aggregate returns...", {"A single value", "A collection", "A sequence", "Void"}, "Aggregation collapses to one value."),
            }), "Project xs to their doubled values using LINQ.", "xs.Select(...)",
            { "xs.Select(x => x * 2)", "xs.Select(x => x * 2);" }, "xs.Select(x => x * 2)")
            ));

    // ---------------------------------------------------------------- 8
    T.push_back(MakeTopic("Async & Exceptions", "Async", "async/await, Task, try/catch/finally, IDisposable and using.",
        Color{ 250,100,100,255 }, 280,
        {
            WithCode(M("Async / Await", {
                Q("What does an async method return?", {"Task or Task<T>", "void always", "int", "Thread"}, "async methods return Task / Task<T>."),
                Q("Which keyword waits for a Task?", {"await", "wait", "yield", "async"}, "await."),
                Q("Which suffix do async methods conventionally get?", {"Async", "_async", "Task", "_t"}, "The Async suffix."),
            }), "Await a Task-returning method called SomeWorkAsync().", "await ...;",
            {"await SomeWorkAsync();"}, "await SomeWorkAsync();"),

            WithCode(M("Exceptions & Disposal", {
                Q("Which block always runs?", {"finally", "catch", "try", "when"}, "finally."),
                Q("Which interface signals disposal?", {"IDisposable", "ICloseable", "using", "IResource"}, "IDisposable."),
                Q("What does using do for an IDisposable?", {"Calls Dispose automatically at block end", "Calls Close", "Does nothing", "Makes it read-only"}, "using guarantees Dispose."),
            }), "Wrap a block that uses an IDisposable variable r with a using statement (include opening brace).", "using (...) {",
            {"using (r) {", "using (var x = r) {"}, "using (r) { ... }"),
        },
        WithCode(M("Topic Test", {
            Q("Are C# exceptions checked at compile time?", {"No, unchecked by default", "Yes, always checked", "Only for Task methods", "Only for async"}, "Unlike Java, C# exceptions are unchecked."),
            Q("What is catch (Exception ex) when (...) called?", {"An exception filter", "A pattern", "A guard", "A scope"}, "when adds a filter to catch."),
            Q("A custom exception usually subclasses...", {"Exception", "Error", "BaseException", "object"}, "Subclass Exception."),
            }), "Write the opening of a try block (include the opening brace).", "try ...",
            { "try {" }, "try { ... } catch ...")
            ));

    return T;
}

// ---------------------------------------------------------------------------
// JavaScript course.
// ---------------------------------------------------------------------------
