#pragma once
// ---------------------------------------------------------------------------
// Content.h  -  ALL quiz content lives here. Add topics/modules/questions by
// adding more Q(...) / M(...) lines; nothing else in the code has to change.
//
//  Q(prompt, {options}, explanation)  -> the FIRST option is the correct one
//                                        (options are shuffled at runtime).
//  M(title, {questions})              -> a module (a 5 minute mini quiz)
//  WithCode(module, ...)              -> adds a typed "small code question"
//  WithLesson(module, {cards})        -> adds "learn first" cards before a module
//  MakeTopic(...)                     -> groups modules + a topic test
// ---------------------------------------------------------------------------
#include <string>
#include <utility>
#include <vector>
#include "raylib.h"

struct Question {
    std::string prompt;
    std::vector<std::string> options;
    int correct = 0;          // index into options (always 0 in the data)
    std::string explain;
};

struct CodeTask {
    std::string prompt;
    std::string hint;
    std::vector<std::string> accepted;  // whitespace-insensitive matches
    std::string explain;
    int mode = -1;   // -1 auto (by topic position), 0 build from pieces, 1 fill the blank, 2 type it
};

struct LessonCard {
    std::string title;
    std::string text;
    std::string code;
    std::vector<std::pair<std::string, std::string>> parts;
};

struct Module {
    std::string title;
    std::vector<Question> questions;
    bool hasCode = false;
    CodeTask code;
    std::vector<LessonCard> lesson;
};

struct Topic {
    std::string name, shortName, blurb;
    Color color{};
    int cost = 0;
    std::vector<Module> modules;
    Module test;
};

inline Question Q(const std::string& p, std::vector<std::string> o, const std::string& e) {
    Question q; q.prompt = p; q.options = std::move(o); q.correct = 0; q.explain = e; return q;
}
inline Module M(const std::string& title, std::vector<Question> qs) {
    Module m; m.title = title; m.questions = std::move(qs); return m;
}
inline Module WithLesson(Module m, std::vector<LessonCard> cards) { m.lesson = std::move(cards); return m; }
inline Module WithCodeMode(Module m, int mode) { m.code.mode = mode; return m; }
inline Module WithCode(Module m, const std::string& prompt, const std::string& hint,
    std::vector<std::string> accepted, const std::string& explain) {
    m.hasCode = true; m.code = { prompt, hint, std::move(accepted), explain }; return m;
}
inline Topic MakeTopic(const std::string& name, const std::string& shortName, const std::string& blurb,
    Color c, int cost, std::vector<Module> mods, Module test) {
    Topic t; t.name = name; t.shortName = shortName; t.blurb = blurb; t.color = c;
    t.cost = cost; t.modules = std::move(mods); t.test = std::move(test); t.test.title = "Topic Test";
    return t;
}

inline std::vector<Topic> BuildTopics() {
    std::vector<Topic> T;

    // ---------------------------------------------------------------- 1
    T.push_back(MakeTopic("Java Basics", "Basics", "Your first lines of Java: programs, printing and variables.",
        Color{ 88,204,2,255 }, 0,
        {
            WithLesson(WithCode(M("Hello, Java", {
                Q("Which method is the entry point of a Java program?",
                  {"public static void main(String[] args)", "void start()", "static int run()", "public void main()"},
                  "The JVM looks for exactly this signature."),
                Q("Which line prints text followed by a new line?",
                  {"System.out.println(\"Hi\");", "print(\"Hi\");", "console.log(\"Hi\");", "echo \"Hi\";"},
                  "System.out.println prints and adds a line break."),
                Q("Java source files use which extension?",
                  {".java", ".jv", ".class", ".jar"},
                  ".java is source; .class is compiled bytecode."),
            }), "Write a statement that prints Hello (use double quotes).", "System.out.println(...);",
            {"System.out.println(\"Hello\");"}, "Strings go in double quotes and statements end with ;"), {
                {"The main method",
                 "Every Java program needs one entry point. The JVM looks for a method with this exact signature - if it isn't there, the program won't run.",
                 "public static void main(String[] args)",
                 {{"public static void", "the modifiers and return type - the JVM requires exactly this"},
                  {"main", "the method name - the JVM starts executing here"},
                  {"String[] args", "command-line arguments, passed as an array of strings"}}},
                {"Printing with System.out.println",
                 "System.out is the standard output stream. println is a method on it that prints text and adds a newline.",
                 "System.out.println(\"Hello\");",
                 {{"System.out", "the standard output stream"},
                  {"println", "prints text and moves to a new line"},
                  {"\"Hello\"", "the text - in double quotes"},
                  {";", "ends the statement"}}},
                {".java source files",
                 "Java source code is saved in .java files. When you compile, the compiler produces .class files (bytecode) that the JVM runs.",
                 "Hello.java",
                 {{".java", "the source file extension"}}},
                {"Your turn",
                 "The next question asks you to build a statement that prints Hello. Structure: System.out, then .println, then the text in quotes, then a semicolon.",
                 "", {}},
            }),

            WithLesson(WithCode(M("Variables", {
                Q("Which line declares an integer variable?", {"int x = 5;", "integer x = 5;", "x = int 5;", "Int x := 5;"},
                  "Java uses the lowercase primitive type int."),
                Q("Which type stores true / false?", {"boolean", "bool", "bit", "tf"}, "boolean is the Java type."),
                Q("What is the default type of the literal 3.14?", {"double", "float", "decimal", "real"},
                  "Decimal literals are double unless you add f."),
            }), "Declare an int named age with the value 18.", "int name = value;",
            {"int age = 18;"}, "type name = value;"), {
                {"What is a variable?",
                 "A variable is a labelled box. It has a type, a name, and a value. The type decides what kind of box to make, so it goes first.",
                 "int age = 18;",
                 {{"int", "the type - a whole number"},
                  {"age", "the name - you choose it"},
                  {"=", "assigns the value"},
                  {"18", "the value stored in the box"},
                  {";", "ends the statement"}}},
                {"boolean: true or false",
                 "boolean is the only type that can hold true or false. Java if statements require a boolean - not an int, not a string.",
                 "boolean isReady = true;",
                 {{"boolean", "the type - true or false only"},
                  {"isReady", "the name"},
                  {"true", "the value"}}},
                {"double: decimals",
                 "Decimal literals like 3.14 are double by default. If you want a float instead, you must add f (3.14f), otherwise the literal is a double.",
                 "double price = 3.14;",
                 {{"double", "the type - decimal numbers"},
                  {"3.14", "a decimal literal - double is the default"}}},
                {"Your turn",
                 "The next question asks you to declare an int named age with the value 18. Pattern: type, name, equals, value, semicolon.",
                 "", {}},
            }),
        },
        WithCode(M("Topic Test", {
            Q("Which of these is NOT a primitive type?", {"String", "int", "char", "boolean"}, "String is a class."),
            Q("What does int x = 7 / 2; store in x?", {"3", "3.5", "4", "2"}, "Integer division drops the decimals."),
            Q("Which line declares a String correctly?", {"String s = \"hi\";", "string s = 'hi';", "String s = 'hi';", "Str s = \"hi\";"},
              "String is capitalised and uses double quotes."),
            }), "Write a statement that prints the result of 2+3.", "Put 2+3 inside println.",
            { "System.out.println(2+3);" }, "println accepts an expression.")
            ));

    // ---------------------------------------------------------------- 2
    T.push_back(MakeTopic("Operators & Strings", "Strings", "Do maths, compare values and work with text.",
        Color{ 28,176,246,255 }, 40,
        {
            WithLesson(M("Operators", {
                Q("What is 10 % 3 ?", {"1", "3", "0", "3.33"}, "% gives the remainder."),
                Q("What does x++ do?", {"Adds 1 to x", "Subtracts 1", "Doubles x", "Nothing"}, "++ is increment."),
                Q("Which operator tests equality of primitives?", {"==", "=", "===", ":="}, "= assigns, == compares."),
            }), {
                {"Arithmetic operators",
                 "Java has +, -, *, /, and % (remainder). The % operator gives what is left over after division.",
                 "int r = 10 % 3;",
                 {{"10", "the number being divided"},
                  {"%", "remainder operator"},
                  {"3", "the divisor"},
                  {"r", "the leftover - 1 in this case"}}},
                {"Increment shortcuts",
                 "x++ adds one to x. x-- subtracts one. They are shortcuts for x = x + 1 and x = x - 1.",
                 "x++;",
                 {{"x", "the variable to change"},
                  {"++", "adds one"}}},
                {"One equals vs two",
                 "A single = assigns a value. Two == compares two values. Mixing these up is the most common Java mistake for beginners.",
                 "if (x == 5)",
                 {{"==", "compares - are they equal?"}}},
            }),
            WithLesson(WithCode(M("Strings", {
                Q("What does \"Java\".length() return?", {"4", "5", "3", "Error"}, "Four characters."),
                Q("Which compares String contents?", {"a.equals(b)", "a == b", "a = b", "a.same(b)"}, "== compares references."),
                Q("What does \"Hello\".charAt(1) return?", {"'e'", "'H'", "'l'", "'o'"}, "Indexes start at 0."),
            }), "Call the length method on a String variable named s.", "s.something()",
            {"s.length()", "s.length();"}, "length() is a method, so it has parentheses."), {
                {"Strings are text",
                 "A String holds text. The type is capital S, and the text goes in double quotes.",
                 "String s = \"Hello\";",
                 {{"String", "the type - capital S"},
                  {"s", "the variable name"},
                  {"\"Hello\"", "the text, in double quotes"}}},
                {"s.length()",
                 "The length method returns how many characters a string has. Parentheses are required because it's a method call.",
                 "s.length()",
                 {{"s", "the string variable"},
                  {"length()", "returns the character count"}}},
                {"Comparing with equals()",
                 "Never use == to compare string contents. Use .equals() instead - == checks whether two variables point to the same object, not whether the text matches.",
                 "a.equals(b)",
                 {{"a", "the first string"},
                  {"equals(b)", "compares contents - returns true or false"}}},
                {"s.charAt(i)",
                 "charAt returns the character at a given index. Indexes start at 0, so charAt(0) is the first character.",
                 "s.charAt(1)",
                 {{"s", "the string"},
                  {"charAt(1)", "the character at index 1 - indexes start at 0"}}},
            }),
        },
        WithCode(M("Topic Test", {
            Q("What is \"a\" + 1 + 2 ?", {"\"a12\"", "\"a3\"", "\"3a\"", "Error"}, "Left to right: String + int concatenates."),
            Q("Which is logical AND?", {"&&", "&&&", "and", "||"}, "&& is short-circuit AND."),
            Q("What does \"hello\".toUpperCase() return?", {"\"HELLO\"", "\"Hello\"", "\"hello\"", "\"H\""}, "All letters become capitals."),
            }), "Declare int r holding the remainder of 9 divided by 4.", "int r = ... % ...;",
            { "int r = 9 % 4;" }, "% is the remainder operator.")
            ));

    // ---------------------------------------------------------------- 3
    T.push_back(MakeTopic("Control Flow", "Flow", "Make decisions with if, else and switch.",
        Color{ 255,150,0,255 }, 70,
        {
            WithLesson(M("If / Else", {
                Q("Which is valid Java?", {"if (x > 5) { }", "if x > 5 then", "if [x > 5]", "if x > 5:"}, "Conditions go in parentheses."),
                Q("What type must an if condition be?", {"boolean", "int", "String", "Anything"}, "Java needs a true boolean."),
                Q("When does the else branch run?", {"When the condition is false", "Always", "When it is true", "Never"}, "else is the fallback."),
            }), {
                {"The shape of an if",
                 "An if statement runs code only when a condition is true. The condition goes in parentheses, and the body goes in braces.",
                 "if (x > 5) { ... }",
                 {{"if", "the keyword - makes a decision"},
                  {"(x > 5)", "the condition"},
                  {"{ ... }", "the body - runs when true"}}},
                {"The condition must be a boolean",
                 "Java is strict: the condition in an if must evaluate to true or false. An int or a String will not compile. Use a boolean expression like a comparison.",
                 "if (isReady) { ... }",
                 {{"isReady", "a boolean - the condition must be true or false"}}},
                {"Adding an else",
                 "else runs when the if condition is false. Together, if and else guarantee exactly one branch runs.",
                 "if (x > 5) { ... } else { ... }",
                 {{"if", "runs when the condition is true"},
                  {"else", "runs when the condition is false"}}},
                {"Your turn",
                 "The next question asks you to print \"big\" if n > 100. Structure: if, then the condition in parentheses, then the println call.",
                 "", {}},
            }),
            WithLesson(WithCode(M("Switch & Ternary", {
                Q("Which keyword stops fall-through in a switch case?", {"break", "stop", "exit", "end"}, "Without break execution continues."),
                Q("Which label handles values with no matching case?", {"default", "else", "otherwise", "none"}, "default is the catch-all."),
                Q("int y = (x > 0) ? 1 : 2;  If x is 5, y is...", {"1", "2", "5", "Error"}, "Condition true -> first value."),
            }), "Assign \"yes\" to String s if flag is true, else \"no\" (use a ternary).", "String s = flag ? ... : ...;",
            {"String s = flag ? \"yes\" : \"no\";"}, "condition ? a : b"), {
                {"switch, case and break",
                 "switch compares a value against several case labels. Without a break, execution falls through to the next case - usually a bug, so most cases end in break.",
                 "switch (x) { case 1: ... break; }",
                 {{"switch", "the keyword - compares x to each case"},
                  {"case 1:", "runs when x equals 1"},
                  {"break;", "exits the switch - without it, execution falls through"}}},
                {"default: the fallback case",
                 "default is the case that runs when no other case matches. Like any case, it usually ends with break.",
                 "switch (x) { default: ... }",
                 {{"default:", "runs when no case matches"}}},
                {"The ternary: condition ? a : b",
                 "The ternary operator picks one of two values based on a condition. It's the shorthand for a very short if/else.",
                 "int y = (x > 0) ? 1 : 2;",
                 {{"(x > 0)", "the condition"},
                  {"?", "if true, use the value after the ?"},
                  {"1", "the true-value"},
                  {":", "otherwise, use the value after the :"},
                  {"2", "the false-value"}}},
            }),
        },
        WithCode(M("Topic Test", {
            Q("What happens with  if (x = 5)  in Java?", {"Compile error", "It is true", "It is false", "It runs"}, "= is assignment, giving an int, not boolean."),
            Q("Which is NOT a Java control keyword?", {"unless", "switch", "if", "else"}, "Java has no unless."),
            Q("What is 5 > 3 && 2 > 4 ?", {"false", "true", "0", "Error"}, "Both sides must be true."),
            }), "Print \"big\" if n is greater than 100 (one if statement).", "if (...) System.out.println(...);",
            { "if(n>100)System.out.println(\"big\");", "if(n>100){System.out.println(\"big\");}" }, "A simple if around println.")
            ));

    // ---------------------------------------------------------------- 4
    T.push_back(MakeTopic("Loops & Arrays", "Loops", "Repeat work and store many values.",
        Color{ 206,130,255,255 }, 100,
        {
            M("Loops", {
                Q("Which loop always runs at least once?", {"do-while", "while", "for", "for-each"}, "do-while checks after the body."),
                Q("for (int i=0; i<3; i++) runs how many times?", {"3", "2", "4", "Forever"}, "i = 0, 1, 2."),
                Q("Which keyword exits a loop early?", {"break", "exit", "stop", "leave"}, "break leaves the loop."),
            }),
            WithCode(M("Arrays", {
                Q("Which creates an int array of 5?", {"int[] a = new int[5];", "int a[5];", "array<int> a(5);", "int a = [5];"}, "new int[5]"),
                Q("What is the index of the first element?", {"0", "1", "-1", "Depends"}, "Zero-based."),
                Q("How do you get an array's length?", {"a.length", "a.length()", "a.size()", "len(a)"}, "length is a field, no parentheses."),
            }), "Declare int[] nums holding 1, 2, 3 using an initializer.", "int[] nums = {...};",
            {"int[] nums = {1, 2, 3};", "int[] nums = new int[]{1,2,3};"}, "Braces create an array literal."),
        },
        WithCode(M("Topic Test", {
            Q("int[] a = new int[5]; a[5] = 1;  causes...", {"ArrayIndexOutOfBoundsException", "Nothing", "NullPointerException", "A compile error"}, "Valid indexes are 0-4."),
            Q("Which is a for-each over int[] nums?", {"for (int n : nums)", "foreach (n in nums)", "for n in nums", "for (n of nums)"}, "The colon syntax."),
            Q("What does continue do?", {"Skips to the next iteration", "Exits the loop", "Ends the program", "Restarts the loop"}, "Jumps to the next pass."),
            }), "Write a while loop header that runs while i is less than 5 (include the opening brace).", "while (...) {",
            { "while(i<5){" }, "while (condition) { ... }")
            ));

    // ---------------------------------------------------------------- 5
    T.push_back(MakeTopic("Methods", "Methods", "Reusable blocks of code: parameters, returns and scope.",
        Color{ 0,205,156,255 }, 140,
        {
            M("Defining Methods", {
                Q("Which return type means 'returns nothing'?", {"void", "null", "none", "empty"}, "void."),
                Q("Which is a valid method signature?", {"public int add(int a, int b)", "function add(a, b)", "int add(a, b)", "def add(a, b)"}, "Types are required on parameters."),
                Q("Method overloading means...", {"Same name, different parameters", "Same name and parameters", "Different names", "Calling itself"}, "The parameter list differs."),
            }),
            WithCode(M("Scope & Return", {
                Q("A variable declared inside a method is visible...", {"Only inside that method", "Everywhere", "In all classes", "After the method ends"}, "Local scope."),
                Q("Can a void method use return;?", {"Yes, to exit early", "No, compile error", "Only with a value", "Only in main"}, "Bare return exits."),
                Q("Java passes arguments by...", {"Value", "Reference", "Pointer", "Name"}, "Always by value (object references are copied)."),
            }), "Write the header of a public static method square that takes int x and returns int (include {).", "public static int ...(int x) {",
            {"public static int square(int x){"}, "modifiers returnType name(params) {"),
        },
        WithCode(M("Topic Test", {
            Q("What does return do?", {"Ends the method and sends back a value", "Prints a value", "Repeats the method", "Declares a variable"}, "It exits the method."),
            Q("Which call is valid for  static int twice(int n)?", {"twice(4)", "twice()", "twice(\"4\")", "int twice(4)"}, "One int argument."),
            Q("What is the entry method's return type?", {"void", "int", "String", "boolean"}, "main is void."),
            }), "Call method greet with no arguments (end with ;).", "greet...;",
            { "greet();" }, "name(args);")
            ));

    // ---------------------------------------------------------------- 6
    T.push_back(MakeTopic("Classes & Objects", "Classes", "The heart of OOP: classes, objects and encapsulation.",
        Color{ 255,75,75,255 }, 180,
        {
            M("Classes", {
                Q("Which creates a Dog object?", {"Dog d = new Dog();", "Dog d = Dog();", "new Dog d;", "Dog d = alloc Dog;"}, "Use new."),
                Q("A constructor's name is...", {"The class name", "init", "new", "Anything"}, "Same as the class, no return type."),
                Q("What does 'this' refer to?", {"The current object", "The class", "The parent", "null"}, "The instance running the method."),
            }),
            WithCode(M("Encapsulation", {
                Q("Best way to expose a private field?", {"Getter / setter methods", "Make it public", "Make it static", "Delete it"}, "Control access through methods."),
                Q("A static field belongs to...", {"The class", "Each object", "The JVM only", "Nothing"}, "One shared copy."),
                Q("Default value of an int field?", {"0", "null", "-1", "Undefined"}, "Numeric fields default to 0."),
            }), "Write a getter header for int field age (include {).", "public int get...() {",
            {"public int getAge(){"}, "Getters are named getX."),
        },
        WithCode(M("Topic Test", {
            Q("Which keyword makes a member inaccessible outside the class?", {"private", "static", "final", "void"}, "private."),
            Q("What is an object?", {"An instance of a class", "A method", "A package", "A file"}, "A class is the blueprint."),
            Q("If you write no constructor, Java gives you...", {"A default no-arg constructor", "Nothing", "An error", "A static one"}, "Implicit default constructor."),
            }), "Create a Cat object named c (use new).", "Cat c = new ...;",
            { "Cat c = new Cat();" }, "ClassName var = new ClassName();")
            ));

    // ---------------------------------------------------------------- 7
    T.push_back(MakeTopic("Inheritance & Polymorphism", "Inherit", "Reuse code and let objects take many forms.",
        Color{ 255,134,208,255 }, 230,
        {
            M("Inheritance", {
                Q("Which keyword makes a class inherit another?", {"extends", "inherits", "implements", "super"}, "class B extends A"),
                Q("What does super() do in a constructor?", {"Calls the parent constructor", "Makes it final", "Calls itself", "Creates an interface"}, "Runs the parent's constructor."),
                Q("Can a Java class extend two classes?", {"No, only one", "Yes, any number", "Only abstract ones", "Only in main"}, "Single class inheritance."),
            }),
            WithCode(M("Polymorphism", {
                Q("Animal a = new Dog(); is legal because...", {"Dog is-a Animal", "Dog has-a Animal", "Java guesses", "Both are static"}, "A child can be used as its parent type."),
                Q("What does @Override signal?", {"A method replaces the parent's", "A new field", "A constructor", "A loop"}, "The compiler checks it."),
                Q("Which is runtime polymorphism?", {"Method overriding", "Method overloading", "Operators", "Comments"}, "Resolved by the actual object type."),
            }), "Write the class header: Dog extends Animal (include {).", "class Dog ... Animal {",
            {"class Dog extends Animal{", "public class Dog extends Animal{"}, "class Child extends Parent"),
        },
        WithCode(M("Topic Test", {
            Q("Animal a = new Dog(); a.speak();  runs...", {"Dog's speak (if overridden)", "Animal's speak always", "Nothing", "A compile error"}, "Dynamic dispatch."),
            Q("Which class sits at the top of all Java classes?", {"Object", "Class", "Main", "Base"}, "java.lang.Object."),
            Q("Overriding requires...", {"Same name and parameters", "Different parameters", "Static methods", "Private methods"}, "Same signature."),
            }), "Call the parent class's speak() method from a child method.", "super.speak...",
            { "super.speak();" }, "super.method() reaches the parent.")
            ));

    // ---------------------------------------------------------------- 8
    T.push_back(MakeTopic("Interfaces & Abstraction", "Interfaces", "Contracts, abstract classes and design.",
        Color{ 255,200,0,255 }, 280,
        {
            M("Interfaces", {
                Q("Which keyword makes a class follow an interface?", {"implements", "extends", "uses", "inherits"}, "class A implements B"),
                Q("Interface methods are by default...", {"public and abstract", "private", "static", "final"}, "Implicitly public abstract."),
                Q("A class may implement...", {"Many interfaces", "Only one interface", "None", "Only two"}, "Multiple interfaces are fine."),
            }),
            WithCode(M("Abstract Classes", {
                Q("Can you create an object of an abstract class?", {"No", "Yes", "Only in main", "Only if empty"}, "It must be subclassed."),
                Q("An abstract method has...", {"No body", "A body", "Only a return", "A constructor"}, "Subclasses supply the body."),
                Q("Abstract class vs interface: only the abstract class can...", {"Hold instance fields", "Have methods", "Be extended", "Be public"}, "Interfaces only have constants."),
            }), "Write the header: class Cat implements Pet (include {).", "class Cat ... Pet {",
            {"class Cat implements Pet{", "public class Cat implements Pet{"}, "implements Interface"),
        },
        WithCode(M("Topic Test", {
            Q("Which best describes an interface?", {"A contract of methods", "A kind of loop", "A variable", "A package"}, "It defines what, not how."),
            Q("Which keyword declares an abstract class?", {"abstract", "virtual", "interface", "base"}, "abstract class Shape"),
            Q("Why use interfaces?", {"Program to behaviour, swap implementations", "Faster loops", "Smaller files", "Avoid classes"}, "Loose coupling."),
            }), "Declare an interface named Pet (include {).", "interface ... {",
            { "interface Pet{", "public interface Pet{" }, "interface Name { ... }")
            ));

    // ---------------------------------------------------------------- 9
    T.push_back(MakeTopic("Exception Handling", "Errors", "Handle the unexpected without crashing.",
        Color{ 200,60,90,255 }, 330,
        {
            M("try / catch", {
                Q("Which block ALWAYS runs, error or not?", {"finally", "catch", "try", "else"}, "finally."),
                Q("Where do you put risky code?", {"try", "catch", "finally", "throws"}, "try wraps it."),
                Q("What does catch (Exception e) do?", {"Handles thrown exceptions", "Throws one", "Declares one", "Ignores syntax"}, "Runs when an exception occurs."),
            }),
            WithCode(M("throw & throws", {
                Q("Which keyword actually raises an exception?", {"throw", "throws", "raise", "catch"}, "throw new ..."),
                Q("throws in a method header means...", {"The method may pass an exception up", "It always crashes", "It catches", "It ignores"}, "Declares possible exceptions."),
                Q("A custom checked exception extends...", {"Exception", "Object only", "Error only", "String"}, "extends Exception."),
            }), "Throw a new IllegalArgumentException with the message \"bad\".", "throw new ...(\"bad\");",
            {"throw new IllegalArgumentException(\"bad\");"}, "throw new Type(message);"),
        },
        WithCode(M("Topic Test", {
            Q("Which is an unchecked exception?", {"NullPointerException", "IOException", "SQLException", "FileNotFoundException"}, "RuntimeExceptions are unchecked."),
            Q("Order of multiple catches should be...", {"Specific before general", "General before specific", "Any order", "Alphabetical"}, "General first causes a compile error."),
            Q("int x = 5 / 0; throws...", {"ArithmeticException", "NullPointerException", "Nothing", "IOException"}, "Integer division by zero."),
            }), "Write the line that starts a try block (include {).", "try ...",
            { "try{" }, "try { risky code } catch ...")
            ));

    // ---------------------------------------------------------------- 10
    T.push_back(MakeTopic("Generics & Enums", "Generics", "Type-safe code with type parameters, enums and nested classes.",
        Color{ 0,180,180,255 }, 380,
        {
            WithCode(M("Generics", {
                Q("What does List<String> mean?", {"A list that only holds Strings", "A list of String lists", "A String called List", "Invalid syntax"}, "Type parameters keep collections type-safe."),
                Q("Which declares a generic method?", {"<T> void print(T item)", "void print(T item)", "generic void print(T item)", "T void print(T item)"}, "The <T> before the return type declares the type parameter."),
                Q("What does the wildcard ? mean?", {"An unknown type", "A null type", "Any primitive", "A boolean"}, "Unbounded wildcard."),
            }), "Declare a generic method that returns its parameter: static <T> T id(T x) (include {).", "static <T> T ...(T x) {",
            {"static <T> T id(T x){"}, "The type parameter goes before the return type."),

            WithCode(M("Enums & Nested Classes", {
                Q("Which is a valid enum declaration?", {"enum Color { RED, GREEN }", "enum Color = { RED };", "Enum Color { RED }", "enum Color(RED, GREEN)"}, "Enums list constants in braces."),
                Q("What does Enum.values() return?", {"An array of all constants", "A list", "A single value", "null"}, "Useful for iteration."),
                Q("A static nested class belongs to...", {"The outer class", "Each instance", "The package", "The JVM"}, "It is shared, not per-instance."),
            }), "Declare an enum Size with values SMALL and LARGE (include ;).", "enum Name { A, B };",
            {"enum Size { SMALL, LARGE };", "enum Size{SMALL,LARGE};"}, "List constants separated by commas."),
        },
        WithCode(M("Topic Test", {
            Q("Which is NOT a benefit of generics?", {"Faster runtime loops", "Compile-time type checks", "No manual casts", "Reusable code"}, "Generics are a compile-time feature."),
            Q("Can you create an array of a generic type like new T[5]?", {"No", "Yes", "Only in main", "Only for enums"}, "Type erasure makes this illegal."),
            Q("enum Planet { EARTH }  What is Planet.EARTH?", {"A constant", "An instance of Planet", "A method", "An int"}, "Enum constants are singleton instances of the enum."),
            }), "Access the MERCURY constant of enum Planet.", "Planet.MERCURY",
            { "Planet.MERCURY", "Planet.MERCURY;" }, "EnumType.CONSTANT")
            ));

    // ---------------------------------------------------------------- 11
    T.push_back(MakeTopic("Collections Framework", "Collections", "Lists, sets, maps and queues: the workhorses of Java.",
        Color{ 150,100,255,255 }, 430,
        {
            M("Lists & Sets", {
                Q("Which List allows fast random access?", {"ArrayList", "LinkedList", "Vector", "Stack"}, "ArrayList is backed by an array."),
                Q("Which Set keeps insertion order?", {"LinkedHashSet", "HashSet", "TreeSet", "BitSet"}, "LinkedHashSet preserves insertion order."),
                Q("Which Set is sorted?", {"TreeSet", "HashSet", "LinkedHashSet", "EnumSet"}, "TreeSet keeps elements sorted."),
            }),
            WithCode(M("Maps & Queues", {
                Q("Which Map has no order and O(1) lookup?", {"HashMap", "TreeMap", "LinkedHashMap", "Hashtable"}, "HashMap uses hashing."),
                Q("Which Map keeps keys sorted?", {"TreeMap", "HashMap", "LinkedHashMap", "IdentityHashMap"}, "TreeMap is a red-black tree."),
                Q("Which Queue is a double-ended queue?", {"ArrayDeque", "PriorityQueue", "LinkedList", "Vector"}, "ArrayDeque is a deque."),
            }), "Construct an ArrayList of String (just the new ...(...) expression).", "new ArrayList<...>()",
            {"new ArrayList<String>()", "new ArrayList<String>();", "new ArrayList<>()"}, "The diamond <> infers the type."),
        },
        WithCode(M("Topic Test", {
            Q("Which collection stores unique elements?", {"Set", "List", "Queue", "Deque"}, "Sets reject duplicates."),
            Q("Which Map implementation keeps insertion order?", {"LinkedHashMap", "HashMap", "TreeMap", "Hashtable"}, "LinkedHashMap preserves order."),
            Q("What does PriorityQueue return first?", {"The smallest element", "The oldest element", "The newest element", "Any element"}, "Natural ordering by default."),
            }), "Construct a HashSet of Integer (just the new ...(...) expression).", "new HashSet<...>()",
            { "new HashSet<Integer>()", "new HashSet<Integer>();", "new HashSet<>()" }, "The diamond <> infers the type.")
            ));

    // ---------------------------------------------------------------- 12
    T.push_back(MakeTopic("Modern Java", "Modern", "Lambdas, streams and Optional: the functional side of Java 8+.",
        Color{ 180,220,0,255 }, 480,
        {
            M("Lambdas & Functional Interfaces", {
                Q("Which is a valid lambda?", {"(a, b) -> a + b", "lambda(a, b) { return a+b; }", "function(a, b) -> a+b", "def(a, b) = a+b"}, "Arrow syntax."),
                Q("Which interface takes one argument and returns boolean?", {"Predicate<T>", "Function<T,R>", "Consumer<T>", "Supplier<T>"}, "Predicate<T>.test(x)."),
                Q("Which interface takes one argument and returns nothing?", {"Consumer<T>", "Function<T,R>", "Predicate<T>", "Supplier<T>"}, "Consumer<T>.accept(x)."),
            }),
            WithCode(M("Streams & Optional", {
                Q("Which Stream method filters elements?", {"filter", "select", "where", "keep"}, "filter(Predicate)."),
                Q("Which Stream method transforms elements?", {"map", "convert", "apply", "transform"}, "map(Function)."),
                Q("What is Optional used for?", {"Avoiding null checks", "Sorting lists", "Looping faster", "Hashing"}, "It models a possibly-absent value."),
            }), "Create an Optional containing the String \"hi\" (just the of(...) call).", "Optional.of(...)",
            {"Optional.of(\"hi\")", "Optional.of(\"hi\");"}, "Use Optional.of(value) for non-null values."),
        },
        WithCode(M("Topic Test", {
            Q("What does list.stream().map(String::length).collect(Collectors.toList()) do?", {"Returns the lengths of the strings", "Sorts the list", "Filters empty strings", "Nothing"}, "map(String::length) transforms each string."),
            Q("Which method turns a Stream into a List?", {"collect(Collectors.toList())", "toList()", "asList()", "list()"}, "The terminal collect() gathers results."),
            Q("What does Optional.ofNullable(x).orElse(\"d\") return if x is null?", {"\"d\"", "null", "An exception", "x"}, "orElse provides the fallback."),
            }), "Filter a stream of Strings named s to those containing \"a\".", "s.filter(...)",
            { "s.filter(x -> x.contains(\"a\"))", "s.filter(x->x.contains(\"a\"))" }, "Pass a Predicate lambda to filter.")
            ));

    // ---------------------------------------------------------------- 13
    T.push_back(MakeTopic("Concurrency", "Threads", "Run code in parallel: threads, synchronization and executors.",
        Color{ 255,120,100,255 }, 530,
        {
            M("Threads & Synchronization", {
                Q("Which interface has a single run() method?", {"Runnable", "Callable", "Thread", "Executor"}, "Runnable is the task."),
                Q("Which keyword prevents concurrent access to a method?", {"synchronized", "volatile", "final", "static"}, "synchronized serializes access."),
                Q("Which method waits for a thread to finish?", {"join()", "wait()", "sleep()", "stop()"}, "join() blocks until done."),
            }),
            WithCode(M("Executors & Virtual Threads", {
                Q("Which class manages a pool of threads?", {"Executors", "Thread", "Runnable", "Lock"}, "Executors.newFixedThreadPool."),
                Q("Which method submits a Runnable to an ExecutorService?", {"submit", "start", "run", "execute"}, "submit returns a Future."),
                Q("Virtual threads are...", {"Lightweight threads (Project Loom)", "OS threads", "Locks", "Processes"}, "They scale to millions."),
            }), "Create a fixed thread pool with 4 threads (just the expression).", "Executors.newFixed...(...)",
            {"Executors.newFixedThreadPool(4)", "Executors.newFixedThreadPool(4);"}, "Executors.newFixedThreadPool(n)"),
        },
        WithCode(M("Topic Test", {
            Q("What is a race condition?", {"When threads interfere with shared state", "A loop that runs fast", "A type of exception", "A design pattern"}, "Unsynchronized access causes it."),
            Q("Which block always runs even if an exception is thrown in try?", {"finally", "catch", "synchronized", "volatile"}, "finally is cleanup."),
            Q("Which is a modern concurrency feature?", {"Virtual threads", "synchronized", "Thread", "Runnable"}, "Project Loom introduced virtual threads."),
            }), "Declare a Runnable r that prints \"hi\".", "Runnable r = () -> ...;",
            { "Runnable r = () -> System.out.println(\"hi\");", "Runnable r=()->System.out.println(\"hi\");" }, "Lambdas are an easy way to write Runnables.")
            ));

    // ---------------------------------------------------------------- 14
    T.push_back(MakeTopic("Files & I/O", "Files", "Read and write files with classic I/O and modern NIO.",
        Color{ 100,180,255,255 }, 580,
        {
            M("Classic I/O", {
                Q("Which class reads text line by line efficiently?", {"BufferedReader", "FileReader", "Scanner", "InputStream"}, "BufferedReader adds buffering."),
                Q("Which class writes text efficiently?", {"BufferedWriter", "FileWriter", "PrintStream", "OutputStream"}, "BufferedWriter adds buffering."),
                Q("Which abstract class represents byte input?", {"InputStream", "Reader", "Writer", "OutputStream"}, "InputStreams read bytes."),
            }),
            WithCode(M("NIO", {
                Q("Which package holds Paths and Files?", {"java.nio.file", "java.io", "java.util", "java.lang"}, "NIO lives in java.nio."),
                Q("Which method reads all lines of a file into a List?", {"Files.readAllLines(path)", "FileReader.readLines()", "Path.read()", "IO.read(path)"}, "Files.readAllLines(path)."),
                Q("Path is to NIO as File is to...", {"java.io", "java.lang", "java.util", "java.net"}, "Path is the NIO replacement for File."),
            }), "Get a Path for \"data.txt\" in the current directory.", "Paths.get(...)",
            {"Paths.get(\"data.txt\")", "Paths.get(\"data.txt\");", "Path.of(\"data.txt\");"}, "Paths.get(\"data.txt\") or Path.of(...)"),
        },
        WithCode(M("Topic Test", {
            Q("Which statement reads a whole file into a String using NIO?", {"Files.readString(path)", "new String(path)", "File.read(path)", "IO.readAll(path)"}, "Java 11 added Files.readString."),
            Q("try-with-resources requires the resource to implement...", {"AutoCloseable", "Runnable", "Serializable", "Comparable"}, "It calls close() automatically."),
            Q("Which stream deals with raw bytes?", {"InputStream", "Reader", "Writer", "Scanner"}, "Readers/Writers handle text."),
            }), "Read all bytes of a file at Path p into a byte array.", "Files.readAll...(...)",
            { "Files.readAllBytes(p)", "Files.readAllBytes(p);" }, "Files.readAllBytes(Path)")
            ));

    // ---------------------------------------------------------------- 15
    T.push_back(MakeTopic("Design & Memory", "Design", "Garbage collection, SOLID and classic design patterns.",
        Color{ 180,140,80,255 }, 630,
        {
            M("Memory & Garbage Collection", {
                Q("Who frees memory for unreachable objects?", {"The garbage collector", "The programmer", "The OS", "The compiler"}, "The JVM GC handles it."),
                Q("Which pool holds local variables and method calls?", {"Stack", "Heap", "Metaspace", "Young gen"}, "The stack."),
                Q("When can an object be garbage collected?", {"When no live references point to it", "After 10 seconds", "When it is null", "Never"}, "Reachability analysis."),
            }),
            WithCode(M("SOLID & Patterns", {
                Q("Which SOLID principle says a class should have one reason to change?", {"Single Responsibility", "Open/Closed", "Liskov Substitution", "Interface Segregation"}, "SRP."),
                Q("Which pattern ensures one instance per JVM?", {"Singleton", "Factory", "Builder", "Observer"}, "Singleton."),
                Q("Which pattern builds complex objects step by step?", {"Builder", "Singleton", "Factory", "Decorator"}, "Builder."),
            }), "Write the private static field that holds a Singleton's instance (include ;).", "private static ... instance;",
            {"private static Singleton instance;", "private static Singleton instance = null;"}, "Singleton pattern uses a static instance."),
        },
        WithCode(M("Topic Test", {
            Q("Which pattern creates objects without specifying the concrete class?", {"Factory", "Builder", "Singleton", "Strategy"}, "The Factory pattern."),
            Q("What does the O in SOLID stand for?", {"Open/Closed", "Object", "Override", "Observer"}, "Open for extension, closed for modification."),
            Q("A memory leak in Java usually means...", {"Unreferenced objects are still reachable", "The heap is full", "NullPointerException", "A stack overflow"}, "Stale references prevent GC."),
            }), "Declare a StringBuilder named sb initialized with \"Java\".", "new StringBuilder(...)",
            { "StringBuilder sb = new StringBuilder(\"Java\");", "StringBuilder sb=new StringBuilder(\"Java\");" }, "StringBuilder is mutable; new StringBuilder(...) creates it.")
            ));

    // ---------------------------------------------------------------- 16
    T.push_back(MakeTopic("JDBC & SQL", "JDBC", "Talk to relational databases from Java.",
        Color{ 220,50,220,255 }, 680,
        {
            M("SQL Basics", {
                Q("Which SQL statement retrieves rows?", {"SELECT", "GET", "FETCH", "READ"}, "SELECT reads rows."),
                Q("Which clause filters rows?", {"WHERE", "FILTER", "HAVING", "LIMIT"}, "WHERE applies to rows."),
                Q("Which key uniquely identifies a row?", {"Primary key", "Foreign key", "Index", "Trigger"}, "The primary key."),
            }),
            WithCode(M("JDBC", {
                Q("Which interface represents a database connection?", {"Connection", "Session", "Database", "Link"}, "java.sql.Connection."),
                Q("Which class executes a precompiled SQL statement?", {"PreparedStatement", "Statement", "Connection", "ResultSet"}, "PreparedStatements prevent SQL injection."),
                Q("Which method advances a ResultSet cursor?", {"next()", "advance()", "forward()", "step()"}, "ResultSet.next() moves to the next row."),
            }), "Call ps to run a SELECT and assign the result to rs.", "rs = ps.execute...()",
            {"rs = ps.executeQuery();", "ResultSet rs = ps.executeQuery();"}, "executeQuery() runs a SELECT."),
        },
        WithCode(M("Topic Test", {
            Q("Which JDBC URL is valid for MySQL?", {"jdbc:mysql://localhost:3306/db", "mysql://localhost/db", "jdbc:mysql/db", "http://localhost/db"}, "JDBC URLs start with jdbc:"),
            Q("Which method commits a transaction?", {"Connection.commit()", "Statement.commit()", "ResultSet.commit()", "PreparedStatement.commit()"}, "conn.setAutoCommit(false) then commit()."),
            Q("Which is the safest against SQL injection?", {"PreparedStatement with ?", "String concatenation", "Statement with literals", "Executing user input"}, "Parameterized queries."),
            }), "Read column 1 of the current row as an int id.", "int id = rs.getInt(...)",
            { "int id = rs.getInt(1);", "int id=rs.getInt(1);", "rs.getInt(1)" }, "ResultSet.getInt(index) reads by column number.")
            ));

    // ---------------------------------------------------------------- 17
    T.push_back(MakeTopic("JPA & Hibernate", "JPA", "Map Java objects to database tables.",
        Color{ 40,140,80,255 }, 730,
        {
            M("JPA Basics", {
                Q("Which annotation marks a class as a JPA entity?", {"@Entity", "@Table", "@Column", "@Bean"}, "@Entity."),
                Q("Which class loads and saves entities?", {"EntityManager", "Session", "Persistence", "JpaContext"}, "EntityManager."),
                Q("Which file lists persistence units?", {"persistence.xml", "jpa.xml", "hibernate.cfg.xml", "beans.xml"}, "META-INF/persistence.xml."),
            }),
            WithCode(M("Relationships", {
                Q("Which annotation maps a one-to-many relationship?", {"@OneToMany", "@ManyToOne", "@OneToOne", "@ManyToMany"}, "@OneToMany."),
                Q("Which column stores the foreign key in a @ManyToOne?", {"The join column", "The primary key", "A side table", "It isn't stored"}, "The owning side's join column."),
                Q("Which query language does JPA use?", {"JPQL", "SQL", "HQL only", "JQL"}, "JPQL operates on entities."),
            }), "Annotate a field String name to map to column \"full_name\".", "@Column(...)",
            {"@Column(name=\"full_name\")"}, "@Column(name=...)"),
        },
        WithCode(M("Topic Test", {
            Q("What is the difference between @Entity and @Table?", {"@Entity marks the class; @Table names the table", "Both do the same", "@Table marks the class; @Entity names the table", "Neither is JPA"}, "@Entity is required; @Table customizes the table name."),
            Q("What does EntityManager.persist(obj) do?", {"Saves a new entity", "Loads it", "Deletes it", "Updates it"}, "Persist inserts."),
            Q("Which is lazy loading?", {"Data is fetched on first access", "Data is fetched up front", "Data is never fetched", "Data is cached"}, "Proxies delay the load."),
            }), "Load the entity User with primary key id 7.", "em.find(...)",
            { "em.find(User.class, 7)", "User u = em.find(User.class, 7);" }, "EntityManager.find(Class, id)")
            ));

    // ---------------------------------------------------------------- 18
    T.push_back(MakeTopic("Web & Spring Boot", "Spring", "Build REST APIs with Spring Boot.",
        Color{ 60,80,200,255 }, 780,
        {
            M("HTTP & REST", {
                Q("Which HTTP method retrieves a resource?", {"GET", "POST", "PUT", "DELETE"}, "GET."),
                Q("Which status code means Created?", {"201", "200", "204", "404"}, "201 Created."),
                Q("Which method usually updates a resource?", {"PUT", "GET", "DELETE", "HEAD"}, "PUT replaces the resource."),
            }),
            WithCode(M("Spring Boot", {
                Q("Which annotation marks a REST controller?", {"@RestController", "@Controller", "@Service", "@Component"}, "@RestController."),
                Q("Which annotation maps a GET request to a method?", {"@GetMapping", "@Get", "@GET", "@Method"}, "@GetMapping."),
                Q("Which annotation maps a POST request?", {"@PostMapping", "@Post", "@POST", "@Create"}, "@PostMapping."),
            }), "Annotate a class as a REST controller.", "@RestController",
            {"@RestController"}, "Put @RestController on the class."),
        },
        WithCode(M("Topic Test", {
            Q("Which annotation extracts {id} from the path?", {"@PathVariable", "@RequestParam", "@RequestBody", "@Header"}, "@PathVariable."),
            Q("Which HTTP status means Not Found?", {"404", "500", "200", "301"}, "404."),
            Q("Which format is commonly used for REST responses?", {"JSON", "XML", "CSV", "HTML"}, "JSON is the default for Spring Boot."),
            }), "Map a method to handle GET /hello.", "@GetMapping(...)",
            { "@GetMapping(\"/hello\")" }, "@GetMapping(path)")
            ));

    return T;
}

// ---------------------------------------------------------------------------
// C++ course.
// ---------------------------------------------------------------------------
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
inline std::vector<Topic> BuildPythonTopics() {
    std::vector<Topic> T;

    // ---------------------------------------------------------------- 1
    T.push_back(MakeTopic("Python Basics", "Basics", "Your first lines of Python: printing, variables and types.",
        Color{ 53,114,165,255 }, 0,
        {
            WithLesson(WithCode(M("Hello, Python", {
                Q("Which function prints to the console?", {"print()", "echo()", "console.log()", "printf()"}, "print() is a built-in."),
                Q("Which file extension do Python scripts use?", {".py", ".pt", ".pyt", ".python"}, ".py."),
                Q("Which line is a Python comment?", {"# this is a comment", "// this is a comment", "/* this */", "-- this"}, "Python uses #."),
            }), "Print Hello (use the print function with a quoted string).", "print(...)",
            {"print(\"Hello\")", "print('Hello')"}, "print(\"Hello\") - text in quotes inside print()."), {
                {"The print function",
                 "print is a built-in function. You call it with parentheses, and put what you want to print inside them.",
                 "print(\"Hello\")",
                 {{"print", "the built-in function that writes to the screen"},
                  {"(\"Hello\")", "the argument - the text to print, in quotes"}}},
                {".py source files",
                 "Python source files end in .py. You run them with the python command.",
                 "main.py",
                 {{".py", "the Python source file extension"}}},
                {"Comments with #",
                 "A comment starts with # and runs to the end of the line. Python ignores it when running the code.",
                 "# this is a comment",
                 {{"#", "line comments start with #"}}},
                {"Your turn",
                 "The next question asks you to call print with the word Hello. Write print, then the text in quotes, inside parentheses.",
                 "", {}},
            }),

            WithLesson(WithCode(M("Variables & Types", {
                Q("How do you assign the string 'hi' to x?", {"x = 'hi'", "string x = 'hi'", "let x = 'hi'", "var x := 'hi'"}, "Python has no type keyword."),
                Q("What does type(3.0) return?", {"<class 'float'>", "<class 'int'>", "<class 'double'>", "float"}, "3.0 is a float."),
                Q("How do you convert the string '42' to an int?", {"int('42')", "str('42')", "(int)'42'", "cast(42)"}, "int() converts."),
            }), "Assign the integer 18 to a variable named age.", "name = value",
            {"age = 18"}, "Python assignment is just name = value."), {
                {"Assignment with =",
                 "In Python you assign with = and no type keyword. The variable takes the type of whatever value you give it.",
                 "age = 18",
                 {{"age", "the name - you choose it"},
                  {"=", "assigns - no type needed"},
                  {"18", "the value"}}},
                {"The type() function",
                 "type() tells you what type a value is. type(3.0) returns <class 'float'> because 3.0 has a decimal point.",
                 "type(3.0)",
                 {{"type", "returns the type of a value"},
                  {"3.0", "a decimal - type(3.0) is <class 'float'>"}}},
                {"Converting with int()",
                 "int() converts a value to an int. int('42') takes the string '42' and produces the number 42.",
                 "int('42')",
                 {{"int", "the conversion function - to int"},
                  {"'42'", "the string to convert"}}},
                {"Your turn",
                 "The next question asks you to assign 18 to a variable named age. Just age, equals, eighteen.",
                 "", {}},
            }),
        },
        WithCode(M("Topic Test", {
            Q("Which is a valid boolean literal in Python?", {"True", "true", "TRUE", "1b"}, "Capital T."),
            Q("What does 7 // 2 evaluate to?", {"3", "3.5", "4", "2"}, "// is floor division."),
            Q("Which built-in returns the type of a value?", {"type(x)", "typeof(x)", "class(x)", "kind(x)"}, "type() is the built-in."),
            }), "Print the result of 2 + 3.", "print(...)",
            { "print(2 + 3)", "print(2+3)" }, "print(2+3) works - print accepts any expression.")
            ));

    // ---------------------------------------------------------------- 2
    T.push_back(MakeTopic("Control Flow", "Flow", "Decisions and loops, the Python way.",
        Color{ 80,150,200,255 }, 40,
        {
            WithCode(M("if / elif / else", {
                Q("Which keyword adds another condition?", {"elif", "elseif", "else if", "elseif else"}, "elif."),
                Q("Which is a valid conditional expression?", {"x if cond else y", "cond ? x : y", "if cond then x else y", "x when cond else y"}, "a if c else b form."),
                Q("Python blocks are defined by...", {"indentation", "curly braces", "begin/end", "semicolons"}, "Whitespace defines blocks."),
            }), "Assign 'yes' to s if flag is True, else 'no' (use a conditional expression).", "s = ... if ... else ...",
            {"s = 'yes' if flag else 'no'", "s = \"yes\" if flag else \"no\""}, "Ternary: a if cond else b"),

            WithCode(M("Loops & match-case", {
                Q("for i in range(3): runs how many times?", {"3", "2", "4", "Forever"}, "i = 0, 1, 2."),
                Q("Which keyword skips to the next iteration?", {"continue", "break", "skip", "next"}, "continue."),
                Q("When does a loop's else block run?", {"When the loop finishes without break", "Always", "Only if the body is empty", "Only after break"}, "Loop else runs on normal exit."),
            }), "Write a while header that runs while i is less than 5 (include :).", "while ...:",
            {"while i < 5:", "while i<5:"}, "while condition:"),
        },
        WithCode(M("Topic Test", {
            Q("What does range(1, 5) generate?", {"1, 2, 3, 4", "1, 2, 3, 4, 5", "0, 1, 2, 3", "2, 3, 4, 5"}, "Stop is exclusive."),
            Q("Which keyword introduces pattern matching (3.10+)?", {"match", "switch", "case", "when"}, "match/case."),
            Q("What is bool([])?", {"False", "True", "0", "Error"}, "Empty containers are falsy."),
            }), "Write a for header that iterates over range(5) (include :).", "for ... in ...:",
            { "for i in range(5):" }, "for i in range(5):")
            ));

    // ---------------------------------------------------------------- 3
    T.push_back(MakeTopic("Functions", "Functions", "def, flexible args, lambdas and generators.",
        Color{ 110,180,200,255 }, 70,
        {
            WithCode(M("Defining Functions", {
                Q("Which keyword defines a function?", {"def", "func", "fn", "function"}, "def."),
                Q("Which syntax collects extra keyword arguments?", {"**kwargs", "*kwargs", "&kwargs", "@kwargs"}, "** collects into a dict."),
                Q("What does a bare return give back?", {"None", "0", "False", "Error"}, "None."),
            }), "Define a function square(x) that returns x*x (one line).", "def name(params): return ...",
            {"def square(x): return x * x", "def square(x): return x*x"}, "def name(params): return ..."),

            WithCode(M("Lambdas, map/filter & Generators", {
                Q("Which is a valid lambda?", {"lambda x: x + 1", "lambda(x) { return x+1 }", "lambda x -> x+1", "(x) => x+1"}, "lambda args: expr."),
                Q("What does yield turn a function into?", {"A generator", "A coroutine", "A normal function", "A lambda"}, "yield makes a generator."),
                Q("What does enumerate(seq) produce?", {"(index, value) pairs", "values only", "indices only", "(value, index) pairs"}, "Indexes come first."),
            }), "Define a lambda named double that doubles its argument.", "double = lambda x: ...",
            {"double = lambda x: x * 2", "double = lambda x: 2 * x", "double=lambda x:x*2"}, "lambda x: x*2"),
        },
        WithCode(M("Topic Test", {
            Q("Which order does Python's LEGB scope rule use?", {"Local, Enclosing, Global, Built-in", "Local, External, Global, Base", "List, Env, Global, Built-in", "Local, Error, Getter, Bytecode"}, "LEGB."),
            Q("Which keyword rebinds a module-level name inside a function?", {"global", "nonlocal", "outer", "module"}, "global."),
            Q("Which built-in filters a sequence with a predicate?", {"filter", "select", "where", "keep"}, "filter(func, seq)."),
            }), "Iterate over names with enumerate in a for header (include :).", "for i, x in enumerate(...):",
            { "for i, name in enumerate(names):" }, "for i, name in enumerate(names):")
            ));

    // ---------------------------------------------------------------- 4
    T.push_back(MakeTopic("Data Structures", "Data", "Lists, tuples, dicts, sets and comprehensions.",
        Color{ 120,190,140,255 }, 100,
        {
            WithCode(M("Lists, Tuples, Sets, Dicts", {
                Q("Which of these is mutable?", {"list", "tuple", "str", "frozenset"}, "Lists can change."),
                Q("Which creates a dict literal?", {"{'a': 1}", "['a', 1]", "('a', 1)", "{'a', 1}"}, "Dicts use key: value."),
                Q("Which collection stores unique values only?", {"set", "list", "tuple", "deque"}, "Sets reject duplicates."),
            }), "Create a list named xs holding 1, 2, 3.", "name = [ ... ]",
            {"xs = [1, 2, 3]", "xs = [1,2,3]"}, "Square brackets make a list."),

            WithCode(M("Comprehensions & Slicing", {
                Q("What is [x*x for x in range(3)]?", {"[0, 1, 4]", "[1, 4, 9]", "[0, 1, 2]", "[1, 2, 3]"}, "range(3) is 0, 1, 2."),
                Q("What does s[1:4] return?", {"Elements at indexes 1, 2, 3", "Elements 1..4", "1 to 4", "First 4 elements"}, "Stop is exclusive."),
                Q("What does 'abcdef'[::-1] return?", {"'fedcba'", "'abcdef'", "'bcdefa'", "''"}, "Step -1 reverses."),
            }), "Write a list comprehension of x for x in range(5) when x is even.", "[x for x in ... if ...]",
            {"[x for x in range(5) if x % 2 == 0]", "[x for x in range(5) if x%2==0]"}, "[x for x in range(5) if x % 2 == 0]"),
        },
        WithCode(M("Topic Test", {
            Q("What does [1, 2, 3][-1] return?", {"3", "1", "Error", "0"}, "Negative indexes count from the end."),
            Q("Which method adds an item to the end of a list?", {"append", "push", "add", "insert"}, "lst.append(x)."),
            Q("Which dict method returns all keys?", {".keys()", ".items()", ".values()", ".all()"}, "dict.keys()."),
            }), "Create a dict mapping 'a' to 1 and 'b' to 2.", "{ ...: ..., ...: ... }",
            { "{'a': 1, 'b': 2}", "d = {'a': 1, 'b': 2}" }, "{'a': 1, 'b': 2}")
            ));

    // ---------------------------------------------------------------- 5
    T.push_back(MakeTopic("Strings & Files", "Files", "Working with text and reading/writing files safely.",
        Color{ 200,180,60,255 }, 140,
        {
            WithCode(M("Strings", {
                Q("Which is the modern string interpolation?", {"f'{x}'", "'{}'.format(x)", "'%s' % x", "f'$x'"}, "f-strings."),
                Q("Are Python strings mutable?", {"No", "Yes", "Only single-char ones", "Only with .upper()"}, "Strings are immutable."),
                Q("What does 'Hi'.upper() return?", {"'HI'", "'hi'", "'Hi'", "Error"}, "All caps."),
            }), "Get the length of the string s using the built-in.", "len(...)",
            {"len(s)"}, "len(s) returns the length."),

            WithCode(M("File I/O & JSON", {
                Q("Which is the safe way to open a file?", {"with open('f.txt') as f:", "f = open('f.txt')", "open('f.txt')", "file = fopen('f.txt')"}, "The with block closes it."),
                Q("Which standard module serializes JSON?", {"json", "csv", "pickle", "os"}, "import json."),
                Q("Which method reads the entire file into a string?", {"f.read()", "f.readlines()", "f.readline()", "f.read(1)"}, "read() with no args reads all."),
            }), "Convert the dict d to a JSON string.", "json.???",
            {"json.dumps(d)", "json.dumps(d);"}, "json.dumps(obj) -> string."),
        },
        WithCode(M("Topic Test", {
            Q("Which mode opens a file for writing (truncating)?", {"open('f.txt', 'w')", "open('f.txt', 'r')", "open('f.txt', 'a')", "open('f.txt')"}, "'w' truncates."),
            Q("Which parses a JSON string into Python objects?", {"json.loads(s)", "json.dumps(s)", "json.parse(s)", "json.read(s)"}, "loads = load from string."),
            Q("Which iterates over a file lazily?", {"for line in f:", "while f.read():", "for line in f.readlines():", "f.read().split()"}, "Iterating the file object."),
            }), "Split the string s on commas.", "s.???",
            { "s.split(',')", "s.split(\",\")" }, "s.split(',') returns a list of pieces.")
            ));

    // ---------------------------------------------------------------- 6
    T.push_back(MakeTopic("Classes & OOP", "Classes", "Classes, self, dunder methods and inheritance.",
        Color{ 220,140,60,255 }, 180,
        {
            WithCode(M("Classes & __init__", {
                Q("Which method is the constructor?", {"__init__", "__new__", "__ctor__", "init"}, "__init__."),
                Q("What is the first parameter of instance methods?", {"self", "this", "cls", "me"}, "self."),
                Q("How do you create a Dog object?", {"Dog()", "new Dog()", "Dog", "create Dog"}, "Call the class."),
            }), "Write the __init__ header for class Dog taking self and name (include :).", "def __init__(...):",
            {"def __init__(self, name):", "def __init__(self,name):"}, "def __init__(self, ...):"),

            WithCode(M("Inheritance & Dunder", {
                Q("How do you inherit from Animal?", {"class Dog(Animal):", "class Dog extends Animal:", "class Dog : Animal {", "class Dog inherits Animal:"}, "Parentheses after the class name."),
                Q("What does super().__init__() do?", {"Calls the parent's constructor", "Makes it final", "Creates a subclass", "Deletes the parent"}, "super() reaches the parent."),
                Q("Which method customizes str(obj)?", {"__str__", "__repr__", "__print__", "str"}, "__str__."),
            }), "Call the parent class's speak() method from a child method.", "super()...",
            {"super().speak()"}, "super().speak()"),
        },
        WithCode(M("Topic Test", {
            Q("Which decorator makes a method a getter?", {"@property", "@getter", "@staticmethod", "@field"}, "@property."),
            Q("What does name mangling do to __x inside class C?", {"Renames it to _C__x", "Makes it public", "Deletes it", "Nothing"}, "Used for collision avoidance."),
            Q("Which method customizes len(obj)?", {"__len__", "__size__", "__length__", "len"}, "__len__."),
            }), "Write the header of a __str__ method (include :).", "def __str__(...):",
            { "def __str__(self):" }, "def __str__(self):")
            ));

    // ---------------------------------------------------------------- 7
    T.push_back(MakeTopic("Exceptions & Modules", "Errors", "try/except, decorators, context managers and the stdlib.",
        Color{ 220,90,90,255 }, 230,
        {
            WithCode(M("Exceptions", {
                Q("Which block always runs, error or not?", {"finally", "except", "try", "else"}, "finally."),
                Q("Which keyword raises an exception?", {"raise ValueError('bad')", "throw ValueError", "raise(ValueError)", "error ValueError"}, "raise."),
                Q("A custom exception usually subclasses...", {"Exception", "Error", "BaseException", "object"}, "Subclass Exception."),
            }), "Raise a ValueError with the message 'bad'.", "raise ...('...')",
            {"raise ValueError('bad')", "raise ValueError(\"bad\")"}, "raise ValueError('bad')"),

            WithCode(M("Decorators & Context Managers", {
                Q("What does @decorator above def f do?", {"Passes f through decorator and rebinds it", "Renames f", "Removes f", "Prints f"}, "Decorators wrap functions."),
                Q("Which module provides functools.wraps?", {"functools", "builtins", "decorators", "itertools"}, "from functools import wraps."),
                Q("Which method runs when a with-block is entered?", {"__enter__", "__init__", "__with__", "__open__"}, "__enter__."),
            }), "Open 'f.txt' for reading using a with statement (header, include :).", "with open(...) as ...:",
            {"with open('f.txt') as f:", "with open(\"f.txt\") as f:"}, "with open('f.txt') as f:"),
        },
        WithCode(M("Topic Test", {
            Q("Which except clause catches every exception cleanly?", {"except Exception as e:", "except:", "catch(Exception):", "catch all:"}, "Bare except swallows everything; prefer Exception."),
            Q("When does try/except's else block run?", {"No exception was raised", "Exception was raised", "Always", "Never"}, "else runs when try succeeded."),
            Q("A context manager needs which two methods?", {"__enter__ and __exit__", "__init__ and __del__", "__with__ and __done__", "__open__ and __close__"}, "Enter/exit protocol."),
            }), "Import wraps from functools.", "from ??? import ???",
            { "from functools import wraps" }, "from functools import wraps")
            ));

    // ---------------------------------------------------------------- 8
    T.push_back(MakeTopic("Pythonic Code", "Pythonic", "Stdlib gems, idioms and readable code.",
        Color{ 160,90,200,255 }, 280,
        {
            WithCode(M("Standard Library Power", {
                Q("Which collections class counts items?", {"collections.Counter", "collections.Count", "collections.Tally", "collections.Histogram"}, "Counter."),
                Q("Which pathlib class represents a filesystem path?", {"pathlib.Path", "pathlib.File", "pathlib.Dir", "pathlib.FsPath"}, "Path."),
                Q("Which itertools helper cycles forever?", {"itertools.cycle", "itertools.loop", "itertools.repeat", "itertools.wrap"}, "cycle."),
            }), "Import Path from pathlib.", "from ??? import ???",
            {"from pathlib import Path"}, "from pathlib import Path"),

            WithCode(M("Idioms", {
                Q("What does a, b = (1, 2) do?", {"Sets a=1, b=2", "Sets a=2, b=1", "Sets a=(1,2)", "Raises an error"}, "Tuple unpacking."),
                Q("What does the walrus := do?", {"Assigns inside an expression", "Compares two values", "Type-checks a value", "Creates a set"}, "Assignment expression."),
                Q("What is the Pythonic way to swap a and b?", {"a, b = b, a", "temp = a; a = b; b = temp", "swap(a, b)", "a = b; b = a"}, "Tuple unpack."),
            }), "Swap a and b using tuple unpacking.", "... , ... = ..., ...",
            {"a, b = b, a", "a,b=b,a"}, "a, b = b, a"),
        },
        WithCode(M("Topic Test", {
            Q("Which returns the sum of the list xs?", {"sum(xs)", "xs.sum()", "total(xs)", "reduce(xs)"}, "sum()."),
            Q("Which returns a NEW sorted list (leaving xs intact)?", {"sorted(xs)", "xs.sort()", "order(xs)", "sort(xs)"}, "sorted() returns a new list; .sort() sorts in place."),
            Q("What is `1 if True else 2`?", {"1", "2", "True", "Error"}, "True chooses the first branch."),
            }), "Iterate over names and ages together with zip (for header, include :).", "for x, y in zip(...):",
            { "for n, a in zip(names, ages):" }, "for n, a in zip(names, ages):")
            ));

    return T;
}

// ---------------------------------------------------------------------------
// C# course.
// ---------------------------------------------------------------------------
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
inline std::vector<Topic> BuildJavaScriptTopics() {
    std::vector<Topic> T;

    // ---------------------------------------------------------------- 1
    T.push_back(MakeTopic("JS Basics", "Basics", "let, const, primitive types and console output.",
        Color{ 230,180,0,255 }, 0,
        {
            WithLesson(WithCode(M("Hello, JavaScript", {
                Q("Which function logs to the console?", {"console.log()", "print()", "echo()", "System.out.println()"}, "console.log is the browser/Node log."),
                Q("Which file extension do JS modules use?", {".js", ".javascript", ".jsc", ".jsx"}, ".js (and .mjs / .cjs for modules)."),
                Q("How do you write a single-line comment?", {"// comment", "# comment", "<!-- comment -->", "/* comment"}, "Double slash."),
            }), "Print Hello using console.log.", "console.log(...)",
            {"console.log(\"Hello\")", "console.log('Hello')"}, "console.log(\"Hello\")"), {
                {"console.log",
                 "JavaScript writes output with console.log. console is the console object, log is a method on it.",
                 "console.log(\"Hello\")",
                 {{"console", "the console object"},
                  {"log", "a method that prints to the console"},
                  {"\"Hello\"", "the text - in quotes"}}},
                {".js source files",
                 "JavaScript source files end in .js. Modern modules may use .mjs, and Node may use .cjs, but .js is the default.",
                 "app.js",
                 {{".js", "the JavaScript source file extension"}}},
                {"Comments with //",
                 "A single-line comment starts with // and runs to the end of the line. JavaScript ignores it.",
                 "// a comment",
                 {{"//", "single-line comments start with //"}}},
                {"Your turn",
                 "The next question asks you to call console.log with Hello. Structure: console.log, then the text in quotes inside parentheses.",
                 "", {}},
            }),

            WithLesson(WithCode(M("let, const & Types", {
                Q("Which declares a block-scoped variable?", {"let", "var", "def", "dim"}, "let is block-scoped."),
                Q("Which declares a value you cannot reassign?", {"const", "let", "var", "final"}, "const prevents reassignment."),
                Q("What does typeof 'hi' return?", {"'string'", "'text'", "'char'", "'literal'"}, "typeof returns 'string'."),
            }), "Declare a constant named age with the value 18.", "const name = value;",
            {"const age = 18;", "const age = 18"}, "const name = value;"), {
                {"let: block-scoped",
                 "let declares a variable you can reassign later. Its scope is the enclosing block, not the whole function.",
                 "let count = 0;",
                 {{"let", "declares a block-scoped variable you can reassign"},
                  {"count", "the name"},
                  {"0", "the starting value"}}},
                {"const: cannot reassign",
                 "const declares a value you cannot reassign. Use it by default - switch to let only when you need to change the variable.",
                 "const age = 18;",
                 {{"const", "declares a value you cannot reassign"},
                  {"age", "the name"},
                  {"18", "the value"}}},
                {"typeof",
                 "typeof returns a string naming the type of a value. typeof 'hi' returns 'string'.",
                 "typeof 'hi'",
                 {{"typeof", "returns the type as a string"},
                  {"'hi'", "a string - typeof gives 'string'"}}},
                {"Your turn",
                 "The next question asks you to declare const age = 18. const, name, equals, value, semicolon.",
                 "", {}},
            }),
        },
        WithCode(M("Topic Test", {
            Q("Which is NOT a JS primitive?", {"object", "number", "boolean", "undefined"}, "object is a reference type."),
            Q("What does typeof null return?", {"'object'", "'null'", "'undefined'", "'void'"}, "A historical quirk - it returns 'object'."),
            Q("Which primitive is returned by uninitialized variables?", {"undefined", "null", "void", "NaN"}, "let x; gives undefined."),
            }), "Print the value of 2 + 3.", "console.log(...)",
            { "console.log(2 + 3)", "console.log(2+3)" }, "console.log accepts any expression.")
            ));

    // ---------------------------------------------------------------- 2
    T.push_back(MakeTopic("Control Flow & Loops", "Flow", "if/else, switch, for, while, for...of and for...in.",
        Color{ 235,175,50,255 }, 40,
        {
            WithCode(M("Conditionals", {
                Q("Which is valid?", {"if (x > 5) { }", "if x > 5 then { }", "if x > 5:", "if x > 5 do { }"}, "Conditions go in parentheses."),
                Q("Which is a JS strict-equality operator?", {"===", "==", "!==", "="}, "=== compares value AND type."),
                Q("What does the ternary expression look like?", {"cond ? a : b", "a if cond else b", "cond -> a : b", "(cond) a | b"}, "cond ? a : b"),
            }), "Assign 'yes' to s if flag is truthy, else 'no' (use a ternary).", "const s = ... ? ... : ...;",
            {"const s = flag ? 'yes' : 'no';", "const s = flag ? \"yes\" : \"no\";", "let s = flag ? 'yes' : 'no';"}, "cond ? a : b"),

            WithCode(M("Loops & Iteration", {
                Q("What does for (let i = 0; i < 3; i++) run for?", {"i = 0, 1, 2", "i = 1, 2, 3", "i = 0, 1, 2, 3", "Forever"}, "Stops when i === 3."),
                Q("Which iterates VALUES of an array?", {"for...of", "for...in", "forEach.in", "loop.of"}, "for...of gives values."),
                Q("Which iterates KEYS of an object?", {"for...in", "for...of", "keys()", "entries()"}, "for...in enumerates keys."),
            }), "Write a while loop header that runs while i < 5 (include opening brace).", "while (...) {",
            {"while (i < 5) {", "while(i<5){"}, "while (condition) { ... }"),
        },
        WithCode(M("Topic Test", {
            Q("What does break do?", {"Exits the loop immediately", "Skips to the next iteration", "Restarts the loop", "Exits the program"}, "break leaves the loop."),
            Q("What does continue do?", {"Skips to the next iteration", "Exits the loop", "Throws", "Restarts"}, "continue jumps to the next pass."),
            Q("What does switch compare with by default?", {"Strict equality (===)", "Loose equality (==)", "Reference identity", "Nothing"}, "switch uses ===."),
            }), "Write a for header counting i from 0 to 9 (include opening brace).", "for (...; ...; ...) {",
            { "for (let i = 0; i < 10; i++) {", "for(let i=0;i<10;i++){" }, "for (init; cond; step) {")
            ));

    // ---------------------------------------------------------------- 3
    T.push_back(MakeTopic("Functions", "Functions", "Declarations, arrows, closures, default and rest params.",
        Color{ 240,170,100,255 }, 70,
        {
            WithCode(M("Defining Functions", {
                Q("Which is a function declaration?", {"function f(x) { }", "const f = (x) =>", "f = function(x)", "def f(x)"}, "function name(args) { }."),
                Q("Which is a valid arrow function?", {"(x) => x * 2", "arrow(x) => x*2", "func(x) { return x*2; }", "fn x -> x*2"}, "The => operator."),
                Q("What does a function return by default?", {"undefined", "null", "0", "NaN"}, "No return statement yields undefined."),
            }), "Declare an arrow function double that takes x and returns x*2.", "const double = x => ...;",
            {"const double = x => x * 2;", "const double = (x) => x * 2;", "const double=x=>x*2;"}, "const double = x => x * 2;"),

            WithCode(M("Closures & Flexible Params", {
                Q("What is a closure?", {"A function that captures its outer scope", "A class method", "A loop", "A promise"}, "Closures remember their lexical environment."),
                Q("What does the ... operator do in function parameters?", {"Collects remaining args into an array", "Spreads a value", "Multiplies two values", "Deletes an arg"}, "Rest parameters gather extra args."),
                Q("How do you give a parameter a default?", {"function f(x = 5) { }", "function f(x: 5) { }", "function f(x ||= 5) { }", "function f(default 5 x) { }"}, "Default values via = in the params."),
            }), "Declare a function greet(name) with default value 'World' that returns 'Hello, ' + name.", "function greet(name = 'World') { ... }",
            {"function greet(name = 'World') { return 'Hello, ' + name; }", "function greet(name='World'){return 'Hello, '+name;}"}, "Use a default value in the parameter list."),
        },
        WithCode(M("Topic Test", {
            Q("What does this refer to inside an arrow function?", {"The enclosing scope's this", "The caller", "The global object always", "undefined always"}, "Arrows inherit this lexically."),
            Q("How do you call a function immediately?", {"(function(){})()", "function()();", "call function()", "new function()"}, "An IIFE."),
            Q("Which is a named vs anonymous distinction?", {"function f(){} vs function(){}", "func vs fn", "const vs let", "arrow vs lambda"}, "Named functions have a name token."),
            }), "Call greet with one argument 'Ada'.", "greet(...);",
            { "greet('Ada');", "greet(\"Ada\");" }, "greet('Ada');")
            ));

    // ---------------------------------------------------------------- 4
    T.push_back(MakeTopic("Arrays & Objects", "Data", "map / filter / reduce, objects and destructuring.",
        Color{ 200,200,60,255 }, 100,
        {
            WithCode(M("Arrays", {
                Q("Which returns a new array of transformed values?", {"map", "filter", "reduce", "forEach"}, "map transforms each element."),
                Q("Which returns elements matching a predicate?", {"filter", "map", "reduce", "some"}, "filter keeps matching items."),
                Q("Which method adds to the END of an array?", {"push", "pop", "unshift", "shift"}, "push appends."),
            }), "Double every number in arr using map.", "arr.map(...)",
            {"arr.map(x => x * 2)", "arr.map(x => x * 2);"}, "arr.map(x => x * 2)"),

            WithCode(M("Objects & Destructuring", {
                Q("Which destructures name from obj?", {"const { name } = obj;", "const name = obj[name];", "const [name] = obj;", "const name => obj;"}, "Curly braces destructure keys."),
                Q("Which destructures a and b from an array arr?", {"const [a, b] = arr;", "const { a, b } = arr;", "const (a, b) = arr;", "const a, b = arr;"}, "Square brackets destructure arrays."),
                Q("How do you add a key to obj safely without mutating?", {"{ ...obj, key: 1 }", "obj.push({ key: 1 })", "obj[].key = 1", "obj.key := 1"}, "Spread copies and extends."),
            }), "Destructure the properties a and b from obj.", "const { ... } = ...;",
            {"const { a, b } = obj;", "const {a,b}=obj;"}, "const { a, b } = obj;"),
        },
        WithCode(M("Topic Test", {
            Q("What does reduce take as its first argument?", {"A reducer function", "An array", "A filter", "A promise"}, "reduce(callback, initial)."),
            Q("What does arr.length return?", {"Number of elements", "The last index", "The array itself", "A new array"}, "Length = count."),
            Q("What does Object.keys(o) return?", {"An array of keys", "An object of keys", "A string", "A number"}, "Returns keys as an array."),
            }), "Filter arr to keep positive numbers using filter.", "arr.filter(...)",
            { "arr.filter(x => x > 0)", "arr.filter(x => x > 0);" }, "arr.filter(x => x > 0)")
            ));

    // ---------------------------------------------------------------- 5
    T.push_back(MakeTopic("The DOM", "DOM", "Select elements, handle events and toggle classes.",
        Color{ 180,200,80,255 }, 140,
        {
            WithCode(M("Selecting & Events", {
                Q("Which selects the first matching element?", {"document.querySelector(sel)", "document.getElement(sel)", "document.find(sel)", "document.select(sel)"}, "querySelector uses CSS selectors."),
                Q("Which adds a click handler?", {"el.addEventListener('click', fn)", "el.on('click', fn)", "el.click = fn()", "el.bind('click', fn)"}, "addEventListener registers a listener."),
                Q("What does event.preventDefault() do?", {"Stops the default browser behaviour", "Deletes the event", "Stops bubbling", "Cancels the fetch"}, "Stops the default action (e.g. form submit)."),
            }), "Select the element with id 'app' and store it in el.", "document.querySelector(...)",
            {"const el = document.querySelector('#app');", "const el = document.getElementById('app');", "document.getElementById('app')"}, "document.querySelector('#app') or getElementById('app')"),

            WithCode(M("Classes & Bubbling", {
                Q("Which toggles a CSS class?", {"el.classList.toggle('x')", "el.toggleClass('x')", "el.classToggle('x')", "toggle(el, 'x')"}, "classList.toggle."),
                Q("Which stops an event from bubbling up?", {"event.stopPropagation()", "event.preventDefault()", "event.halt()", "event.stop()"}, "stopPropagation stops bubbling."),
                Q("Which property reads an element's text content?", {"el.textContent", "el.text", "el.value", "el.inner"}, "textContent is the safe read."),
            }), "Set the text content of el to 'Hello'.", "el.textContent = ...;",
            {"el.textContent = 'Hello';", "el.textContent = \"Hello\";", "el.innerText = 'Hello';"}, "el.textContent = 'Hello';"),
        },
        WithCode(M("Topic Test", {
            Q("Which selects ALL matching elements?", {"document.querySelectorAll(sel)", "document.querySelector(sel)", "document.getAll(sel)", "document.find(sel)"}, "querySelectorAll returns a NodeList."),
            Q("Where does the event object come from?", {"The listener's parameter", "A global variable", "window.event", "document.event"}, "addEventListener passes it in."),
            Q("Which binds a handler that runs at most once?", {"addEventListener('x', fn, { once: true })", "addEventListener('x', fn)", "fn.once = true", "addOnce('x', fn)"}, "The { once: true } option."),
            }), "Listen for a click on el and run fn.", "el.addEventListener(...)",
            { "el.addEventListener('click', fn)", "el.addEventListener(\"click\", fn)" }, "el.addEventListener('click', fn)")
            ));

    // ---------------------------------------------------------------- 6
    T.push_back(MakeTopic("Classes & Prototypes", "Classes", "class syntax, inheritance and the prototype model.",
        Color{ 160,180,120,255 }, 180,
        {
            WithCode(M("Classes", {
                Q("Which is a valid class?", {"class Dog { }", "class Dog() { }", "class Dog: { }", "class Dog() => { }"}, "No parentheses on the class name."),
                Q("Which is a constructor?", {"constructor(...) { }", "__init__(...) { }", "init(...) { }", "new(...) { }"}, "constructor."),
                Q("What does new Dog() do?", {"Creates an instance and calls the constructor", "Calls a static method", "Defines the class", "Nothing"}, "new allocates and runs the constructor."),
            }), "Declare an empty class named Dog.", "class Name { }",
            {"class Dog { }", "class Dog{}"}, "class Dog { }"),

            WithCode(M("Inheritance & Prototype", {
                Q("Which makes Dog extend Animal?", {"class Dog extends Animal { }", "class Dog : Animal { }", "class Dog inherits Animal", "class Dog(Animal)"}, "The extends keyword."),
                Q("What does super() do?", {"Calls the parent constructor", "Deletes the parent", "Makes it abstract", "Calls this"}, "super() invokes the parent's constructor."),
                Q("What is the prototype chain used for?", {"Method lookup when a property is not on the instance", "Memory allocation", "Type checking", "Error handling"}, "Inherited lookups walk the prototype chain."),
            }), "Call the parent class's speak() from a child method.", "super....()",
            {"super.speak()", "super.speak();"}, "super.method()"),
        },
        WithCode(M("Topic Test", {
            Q("Which keyword refers to the current instance?", {"this", "self", "me", "super"}, "this."),
            Q("Static methods are called on...", {"The class itself", "Instances", "The prototype", "Nothing"}, "Class.method()."),
            Q("What does instanceof do?", {"Checks the prototype chain", "Compares values", "Compares types", "Copies objects"}, "obj instanceof Type walks the chain."),
            }), "Declare a class Point with a constructor(x, y) that stores both on this.", "class Point { constructor(...) { ... } }",
            { "class Point { constructor(x, y) { this.x = x; this.y = y; } }", "class Point{constructor(x,y){this.x=x;this.y=y;}}" }, "class Name { constructor(...) { ... } }")
            ));

    // ---------------------------------------------------------------- 7
    T.push_back(MakeTopic("Async JavaScript", "Async", "Callbacks, promises, async/await and fetch.",
        Color{ 140,140,200,255 }, 230,
        {
            WithCode(M("Promises & async/await", {
                Q("What are the three promise states?", {"pending, fulfilled, rejected", "open, closed, error", "wait, run, done", "created, started, stopped"}, "Pending, fulfilled, rejected."),
                Q("Which keyword waits for a promise?", {"await", "wait", "yield", "async"}, "await."),
                Q("Where can await be used?", {"Inside an async function (or top-level module)", "Only in the main function", "Only inside callbacks", "Anywhere"}, "async functions (or top-level modules)."),
            }), "Await a promise-returning call foo() and store the result in x.", "const x = await ...;",
            {"const x = await foo();", "const x = await foo()"}, "const x = await foo();"),

            WithCode(M("fetch & Error Handling", {
                Q("Which function performs HTTP in browsers?", {"fetch", "request", "http", "xhr"}, "fetch(url)."),
                Q("What does .then() register?", {"A handler for a resolved promise", "An error handler", "A timer", "A callback for rejection only"}, ".then runs on fulfillment."),
                Q("What does .catch() do?", {"Handles a rejected promise", "Cancels the promise", "Creates a new promise", "Runs on fulfillment"}, ".catch handles rejections."),
            }), "Fetch '/api' and await its response object.", "await fetch(...)",
            {"const res = await fetch('/api');", "const res = await fetch(\"/api\");"}, "const res = await fetch('/api');"),
        },
        WithCode(M("Topic Test", {
            Q("What does Promise.all([a, b]) return?", {"A promise resolving to an array of results", "The first resolved value", "An error if one fails after another", "Void"}, "Resolves when all resolve; rejects on first rejection."),
            Q("What does async always return?", {"A promise", "void", "undefined", "A callback"}, "async wraps the return value in a Promise."),
            Q("How do you handle errors in async/await?", {"try/catch around the awaits", "Only .then", "Promise.fail()", "You cannot"}, "try/catch works the same as sync code."),
            }), "Wait 1 second and resolve with 'done' using a Promise + setTimeout.", "new Promise(resolve => setTimeout(...))",
            { "new Promise(resolve => setTimeout(() => resolve('done'), 1000))", "new Promise(resolve => setTimeout(() => resolve('done'), 1000));" }, "new Promise(resolve => setTimeout(() => resolve('done'), 1000))")
            ));

    // ---------------------------------------------------------------- 8
    T.push_back(MakeTopic("Modern JS", "Modern", "Modules, template literals, spread/rest and optional chaining.",
        Color{ 100,120,220,255 }, 280,
        {
            WithCode(M("Modules & Templates", {
                Q("Which exports a value from a module?", {"export const x = 1;", "module.x = 1;", "public const x = 1;", "exports.x = 1;"}, "ES modules use export."),
                Q("Which imports default from 'm'?", {"import def from 'm';", "require('m');", "using def from 'm';", "from 'm' import def;"}, "import x from 'm'."),
                Q("Which template literal syntax is correct?", {"`Hello ${name}`", "'Hello ${name}'", "\"Hello ${name}\"", "`Hello {name}`"}, "Backticks + ${expr}."),
            }), "Export a const named PI with value 3.14.", "export const ...",
            {"export const PI = 3.14;", "export const PI = 3.14"}, "export const PI = 3.14;"),

            WithCode(M("Spread & Optional Chaining", {
                Q("What does [...a, ...b] do?", {"Concatenates two arrays", "Multiplies two arrays", "Compares arrays", "Deletes both"}, "Spread copies values into a new array."),
                Q("What does obj?.name do?", {"Returns undefined if obj is null/undefined", "Throws if obj is null", "Deletes name", "Copies name"}, "Optional chaining short-circuits nullish access."),
                Q("What does a ?? b return when a is null/undefined?", {"b", "a", "Error", "Null"}, "Nullish coalescing falls back."),
            }), "Merge two objects a and b into c (use spread).", "const c = { ...a, ...b };",
            {"const c = { ...a, ...b };", "const c={...a,...b};"}, "const c = { ...a, ...b };"),
        },
        WithCode(M("Topic Test", {
            Q("What does ?.chain short-circuit on?", {"null or undefined", "false", "0", "empty string"}, "Only nullish values trigger the short-circuit."),
            Q("What is the difference between ?? and ||?", {"?? only falls back on null/undefined", "?? works only with numbers", "|| is new", "They are identical"}, "|| also falls back on falsy values like 0."),
            Q("Spread vs rest - which packs arguments?", {"rest", "spread", "both", "neither"}, "Rest packs; spread unpacks."),
            }), "Access obj.user?.name safely (just the expression).", "obj.?....",
            { "obj.user?.name", "obj?.user?.name" }, "obj.user?.name")
            ));

    return T;
}

// ---------------------------------------------------------------------------
// Course container and outline helper (unchanged).
// ---------------------------------------------------------------------------
struct Course {
    std::string name, tagline;
    Color color{};
    std::vector<Topic> topics;

    bool Ready() const { for (auto& t : topics) if (!t.modules.empty()) return true; return false; }
    int  TotalNodes() const {
        int n = 0;
        for (auto& t : topics) if (!t.modules.empty()) n += (int)t.modules.size() + 1;
        return n;
    }
};

struct OutlineRow { const char* name; const char* shortName; const char* blurb; };

inline std::vector<Topic> OutlineTopics(const std::vector<OutlineRow>& rows, Color c) {
    std::vector<Topic> v;
    for (size_t i = 0; i < rows.size(); i++)
        v.push_back(MakeTopic(rows[i].name, rows[i].shortName, rows[i].blurb, c,
            i == 0 ? 0 : 30 + (int)i * 40, {}, Module{}));
    return v;
}

inline std::vector<Course> BuildCourses() {
    std::vector<Course> C;
    C.push_back({ "Java", "Classes, OOP, collections, streams, Spring Boot and more.",
                 Color{255,150,0,255}, BuildTopics() });

    C.push_back({ "C++", "Systems programming: memory, pointers, templates and the STL.", Color{60,120,220,255},
        BuildCppTopics() });

    C.push_back({ "Python", "Readable, powerful scripting for beginners and pros.", Color{53,114,165,255},
        BuildPythonTopics() });

    C.push_back({ "C#", "Modern .NET: OOP, LINQ and async programming.", Color{155,79,150,255},
        BuildCSharpTopics() });

    C.push_back({ "JavaScript", "The language of the web: DOM, async and modern ES6+.", Color{230,180,0,255},
        BuildJavaScriptTopics() });
    return C;
}