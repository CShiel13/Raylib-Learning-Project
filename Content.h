#pragma once
// ---------------------------------------------------------------------------
// Content.h  -  ALL quiz content lives here. Add topics/modules/questions by
// adding more Q(...) / M(...) lines; nothing else in the code has to change.
//
//  Q(prompt, {options}, explanation)  -> the FIRST option is the correct one
//                                        (options are shuffled at runtime).
//  M(title, {questions})              -> a module (a 5 minute mini quiz)
//  WithCode(module, ...)              -> adds a typed "small code question"
//  MakeTopic(...)                     -> groups modules + a topic test
// ---------------------------------------------------------------------------
#include <string>
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
};

struct Module {
    std::string title;
    std::vector<Question> questions;
    bool hasCode = false;
    CodeTask code;
};

struct Topic {
    std::string name, shortName, blurb;
    Color color{};
    int cost = 0;                 // coins to unlock early
    std::vector<Module> modules;
    Module test;                  // end-of-topic test
};

inline Question Q(const std::string& p, std::vector<std::string> o, const std::string& e) {
    Question q; q.prompt = p; q.options = std::move(o); q.correct = 0; q.explain = e; return q;
}
inline Module M(const std::string& title, std::vector<Question> qs) {
    Module m; m.title = title; m.questions = std::move(qs); return m;
}
inline Module WithCode(Module m, const std::string& prompt, const std::string& hint,
                       std::vector<std::string> accepted, const std::string& explain) {
    m.hasCode = true; m.code = {prompt, hint, std::move(accepted), explain}; return m;
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
        Color{88,204,2,255}, 0,
    {
        WithCode(M("Hello, Java", {
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
        {"System.out.println(\"Hello\");"}, "Strings go in double quotes and statements end with ;"),

        WithCode(M("Variables", {
            Q("Which line declares an integer variable?", {"int x = 5;", "integer x = 5;", "x = int 5;", "Int x := 5;"},
              "Java uses the lowercase primitive type int."),
            Q("Which type stores true / false?", {"boolean", "bool", "bit", "tf"}, "boolean is the Java type."),
            Q("What is the default type of the literal 3.14?", {"double", "float", "decimal", "real"},
              "Decimal literals are double unless you add f."),
        }), "Declare an int named age with the value 18.", "int name = value;",
        {"int age = 18;"}, "type name = value;"),
    },
        WithCode(M("Topic Test", {
            Q("Which of these is NOT a primitive type?", {"String", "int", "char", "boolean"}, "String is a class."),
            Q("What does int x = 7 / 2; store in x?", {"3", "3.5", "4", "2"}, "Integer division drops the decimals."),
            Q("Which line declares a String correctly?", {"String s = \"hi\";", "string s = 'hi';", "String s = 'hi';", "Str s = \"hi\";"},
              "String is capitalised and uses double quotes."),
        }), "Write a statement that prints the result of 2+3.", "Put 2+3 inside println.",
        {"System.out.println(2+3);"}, "println accepts an expression.")
    ));

    // ---------------------------------------------------------------- 2
    T.push_back(MakeTopic("Operators & Strings", "Strings", "Do maths, compare values and work with text.",
        Color{28,176,246,255}, 40,
    {
        M("Operators", {
            Q("What is 10 % 3 ?", {"1", "3", "0", "3.33"}, "% gives the remainder."),
            Q("What does x++ do?", {"Adds 1 to x", "Subtracts 1", "Doubles x", "Nothing"}, "++ is increment."),
            Q("Which operator tests equality of primitives?", {"==", "=", "===", ":="}, "= assigns, == compares."),
        }),
        WithCode(M("Strings", {
            Q("What does \"Java\".length() return?", {"4", "5", "3", "Error"}, "Four characters."),
            Q("Which compares String contents?", {"a.equals(b)", "a == b", "a = b", "a.same(b)"}, "== compares references."),
            Q("What does \"Hello\".charAt(1) return?", {"'e'", "'H'", "'l'", "'o'"}, "Indexes start at 0."),
        }), "Call the length method on a String variable named s.", "s.something()",
        {"s.length()", "s.length();"}, "length() is a method, so it has parentheses."),
    },
        WithCode(M("Topic Test", {
            Q("What is \"a\" + 1 + 2 ?", {"\"a12\"", "\"a3\"", "\"3a\"", "Error"}, "Left to right: String + int concatenates."),
            Q("Which is logical AND?", {"&&", "&&&", "and", "||"}, "&& is short-circuit AND."),
            Q("What does \"hello\".toUpperCase() return?", {"\"HELLO\"", "\"Hello\"", "\"hello\"", "\"H\""}, "All letters become capitals."),
        }), "Declare int r holding the remainder of 9 divided by 4.", "int r = ... % ...;",
        {"int r = 9 % 4;"}, "% is the remainder operator.")
    ));

    // ---------------------------------------------------------------- 3
    T.push_back(MakeTopic("Control Flow", "Flow", "Make decisions with if, else and switch.",
        Color{255,150,0,255}, 70,
    {
        M("If / Else", {
            Q("Which is valid Java?", {"if (x > 5) { }", "if x > 5 then", "if [x > 5]", "if x > 5:"}, "Conditions go in parentheses."),
            Q("What type must an if condition be?", {"boolean", "int", "String", "Anything"}, "Java needs a true boolean."),
            Q("When does the else branch run?", {"When the condition is false", "Always", "When it is true", "Never"}, "else is the fallback."),
        }),
        WithCode(M("Switch & Ternary", {
            Q("Which keyword stops fall-through in a switch case?", {"break", "stop", "exit", "end"}, "Without break execution continues."),
            Q("Which label handles values with no matching case?", {"default", "else", "otherwise", "none"}, "default is the catch-all."),
            Q("int y = (x > 0) ? 1 : 2;  If x is 5, y is...", {"1", "2", "5", "Error"}, "Condition true -> first value."),
        }), "Assign \"yes\" to String s if flag is true, else \"no\" (use a ternary).", "String s = flag ? ... : ...;",
        {"String s = flag ? \"yes\" : \"no\";"}, "condition ? a : b"),
    },
        WithCode(M("Topic Test", {
            Q("What happens with  if (x = 5)  in Java?", {"Compile error", "It is true", "It is false", "It runs"}, "= is assignment, giving an int, not boolean."),
            Q("Which is NOT a Java control keyword?", {"unless", "switch", "if", "else"}, "Java has no unless."),
            Q("What is 5 > 3 && 2 > 4 ?", {"false", "true", "0", "Error"}, "Both sides must be true."),
        }), "Print \"big\" if n is greater than 100 (one if statement).", "if (...) System.out.println(...);",
        {"if(n>100)System.out.println(\"big\");", "if(n>100){System.out.println(\"big\");}"}, "A simple if around println.")
    ));

    // ---------------------------------------------------------------- 4
    T.push_back(MakeTopic("Loops & Arrays", "Loops", "Repeat work and store many values.",
        Color{206,130,255,255}, 100,
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
        {"while(i<5){"}, "while (condition) { ... }")
    ));

    // ---------------------------------------------------------------- 5
    T.push_back(MakeTopic("Methods", "Methods", "Reusable blocks of code: parameters, returns and scope.",
        Color{0,205,156,255}, 140,
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
        {"greet();"}, "name(args);")
    ));

    // ---------------------------------------------------------------- 6
    T.push_back(MakeTopic("Classes & Objects", "Classes", "The heart of OOP: classes, objects and encapsulation.",
        Color{255,75,75,255}, 180,
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
        {"Cat c = new Cat();"}, "ClassName var = new ClassName();")
    ));

    // ---------------------------------------------------------------- 7
    T.push_back(MakeTopic("Inheritance & Polymorphism", "Inherit", "Reuse code and let objects take many forms.",
        Color{255,134,208,255}, 230,
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
        {"super.speak();"}, "super.method() reaches the parent.")
    ));

    // ---------------------------------------------------------------- 8
    T.push_back(MakeTopic("Interfaces & Abstraction", "Interfaces", "Contracts, abstract classes and design.",
        Color{255,200,0,255}, 280,
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
        {"interface Pet{", "public interface Pet{"}, "interface Name { ... }")
    ));

    // ---------------------------------------------------------------- 9
    T.push_back(MakeTopic("Exception Handling", "Errors", "Handle the unexpected without crashing.",
        Color{200,60,90,255}, 330,
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
        {"try{"}, "try { risky code } catch ...")
    ));

    return T;
}
