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
            { "System.out.println(2+3);" }, "println accepts an expression.")
            ));

    // ---------------------------------------------------------------- 2
    T.push_back(MakeTopic("Operators & Strings", "Strings", "Do maths, compare values and work with text.",
        Color{ 28,176,246,255 }, 40,
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
            { "int r = 9 % 4;" }, "% is the remainder operator.")
            ));

    // ---------------------------------------------------------------- 3
    T.push_back(MakeTopic("Control Flow", "Flow", "Make decisions with if, else and switch.",
        Color{ 255,150,0,255 }, 70,
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
// C++ course. Mirrors the Java structure: each topic has 2 modules + 1 test,
// each module has 3 MC questions + 1 small code question, the test has 3 MC
// questions + 1 small code question.
// ---------------------------------------------------------------------------
inline std::vector<Topic> BuildCppTopics() {
    std::vector<Topic> T;

    // ---------------------------------------------------------------- 1: M1
    T.push_back(MakeTopic("Basic Syntax, Compilation & I/O", "Basics",
        "The compilation pipeline, fundamental types and getting text onto the screen.",
        Color{ 60,120,220,255 }, 0,
        {
            WithCode(M("Compilation & Types", {
                Q("What is the correct order of the compilation pipeline?",
                  {"Preprocess, compile, assemble, link", "Compile, preprocess, link, assemble", "Link, compile, preprocess, assemble", "Assemble, compile, preprocess, link"},
                  "Preprocessor, compiler, assembler, linker."),
                Q("Which is a fundamental type?",
                  {"int", "string", "vector", "array"},
                  "int, double, char, bool are fundamental."),
                Q("What does sizeof(int) return on typical systems?",
                  {"4", "8", "2", "1"},
                  "Usually 4 bytes; sizeof returns size_t."),
            }), "Declare an int x and set it to 5.", "type name = value;",
            {"int x = 5;"}, "int x = 5;"),

            WithCode(M("I/O with iostream", {
                Q("Which header declares std::cout?",
                  {"<iostream>", "<stdio.h>", "<cout>", "<print>"},
                  "iostream declares the standard streams."),
                Q("Which operator sends data to std::cout?",
                  {"<<", ">>", "+", "->"},
                  "<< inserts into the stream."),
                Q("What does std::endl do?",
                  {"Writes a newline and flushes the stream", "Only writes a newline", "Only flushes the stream", "Closes the stream"},
                  "endl is newline plus flush."),
            }), "Print Hello and a newline using std::cout.", "std::cout << ...;",
            {"std::cout << \"Hello\" << std::endl;", "std::cout << \"Hello\\n\";", "cout << \"Hello\" << endl;"},
            "std::cout << \"Hello\" << std::endl;"),
        },
        WithCode(M("Topic Test", {
            Q("Which stage expands #include directives?", {"The preprocessor", "The compiler", "The assembler", "The linker"},
              "The preprocessor expands includes and macros."),
            Q("What must main() return?", {"int", "void", "auto", "size_t"}, "int main() is the portable signature."),
            Q("Which is NOT a C++ type modifier?", {"static", "signed", "short", "long"}, "static is a storage class specifier."),
            }), "Print the result of 2 + 3 with std::cout.", "std::cout << ...;",
            { "std::cout << 2 + 3;", "std::cout << 2+3;" }, "std::cout << 2 + 3;")
            ));

    // ---------------------------------------------------------------- 2: M2
    T.push_back(MakeTopic("Control Flow & Functions", "Flow",
        "if/switch/loops, function declarations vs definitions, and pass-by-value vs reference.",
        Color{ 80,140,230,255 }, 40,
        {
            WithCode(M("Control Flow", {
                Q("Which loop always runs at least once?", {"do-while", "while", "for", "range-for"}, "do-while checks after the body."),
                Q("Which keyword exits a switch case?", {"break", "exit", "return", "stop"}, "Without break, execution falls through."),
                Q("Which is a range-based for over v?", {"for (auto x : v)", "for (x in v)", "foreach (x : v)", "for (x of v)"}, "Colon syntax for ranges."),
            }), "Write a for loop header counting i = 0..9 (include opening brace).", "for (...;...;...) {",
            {"for (int i = 0; i < 10; ++i) {", "for (int i = 0; i < 10; i++) {", "for(int i=0;i<10;++i){"}, "for (init; cond; step) {"),

            WithCode(M("Functions & Parameters", {
                Q("What is the difference between a declaration and a definition?", {"A declaration names the function; a definition provides the body", "They are the same", "A definition comes first", "A declaration must be in a .cpp file"}, "Declarations go in headers; definitions in .cpp."),
                Q("Which is the modern keyword to encourage inlining?", {"inline", "register", "auto", "static"}, "inline hints to the compiler."),
                Q("Why pass by const reference for non-trivial types?", {"To avoid copies while preventing modification", "It is required", "To share memory", "To enable overloading"}, "const& avoids copies and prohibits mutation."),
            }), "Declare a function inc that takes int& x and returns void (single line, no body).", "void name(int& p);",
            {"void inc(int& x);"}, "void inc(int& x);"),
        },
        WithCode(M("Topic Test", {
            Q("Where must the declaration of a function used across files appear?", {"In a header (.h / .hpp)", "In the .cpp of the caller", "Only in main", "In a comment"},
              "Headers advertise the interface."),
            Q("What does passing by value do?", {"Copies the argument", "Shares the argument", "Returns a reference", "Nothing"}, "The callee gets a copy."),
            Q("Which is a valid main signature taking arguments?", {"int main(int argc, char* argv[])", "void main(char* argv[])", "int main(char* argv)", "main(int argc)"}, "Standard argc/argv form."),
            }), "Declare (no body) a function greet that takes no arguments and returns void.", "void name();",
            { "void greet();" }, "void greet();")
            ));

    // ---------------------------------------------------------------- 3: M3
    T.push_back(MakeTopic("Pointers & References", "Pointers",
        "Addresses, dereferencing, nullptr, and references vs. pointers.",
        Color{ 100,160,240,255 }, 70,
        {
            WithCode(M("Memory & Pointers", {
                Q("Which operator takes an address?", {"&", "*", "->", "."}, "The unary & operator."),
                Q("Which operator dereferences a pointer?", {"*", "&", "->", "[]"}, "Unary *."),
                Q("What is the modern null pointer literal?", {"nullptr", "NULL", "0", "void*"}, "nullptr is type-safe."),
            }), "Declare an int pointer p initialized to nullptr.", "int* p = ...;",
            {"int* p = nullptr;", "int *p = nullptr;"}, "int* p = nullptr;"),

            WithCode(M("References & const", {
                Q("What does int& mean?", {"A reference to an int", "A pointer to an int", "A copy", "An array"}, "References alias an existing object."),
                Q("What does const int& promise?", {"The function will not modify the argument", "The value cannot be read", "It is a pointer", "It is a copy"}, "const prevents modification."),
                Q("Can a reference be reseated to another variable?", {"No", "Yes", "Only after delete", "Only if const"}, "Once bound, always bound."),
            }), "Return a const reference to a member 'name': const std::string& getName() (single line, no body).", "const T& name() const;",
            {"const std::string& getName() const;", "const std::string& getName();"}, "Return by const ref to avoid copies."),
        },
        WithCode(M("Topic Test", {
            Q("Which is the key difference between a pointer and a reference?", {"A reference cannot be null; a pointer can", "A pointer is faster", "They are identical", "References are dynamic"}, "References must bind to an object."),
            Q("What does int* const p mean?", {"The pointer cannot be reseated", "The int cannot be modified", "Both are const", "Neither"}, "The pointer itself is const."),
            Q("What does pointer arithmetic p + 1 do?", {"Advances by one element of the pointed-to type", "Adds one to the pointed value", "Advances one byte", "Nothing"}, "Pointer arithmetic is scaled by sizeof(T)."),
            }), "Declare a const reference r to an int named x.", "const int& r = ...;",
            { "const int& r = x;", "const int &r = x;" }, "const int& r = x;")
            ));

    // ---------------------------------------------------------------- 4: M4
    T.push_back(MakeTopic("Dynamic Memory & RAII", "Memory",
        "Stack vs. heap, new/delete, and the RAII principle that ties resource lifetime to object lifetime.",
        Color{ 120,180,250,255 }, 100,
        {
            WithCode(M("Stack vs. Heap", {
                Q("Where do local variables live?", {"On the stack", "On the heap", "In registers only", "In static storage"}, "Automatic locals go on the stack."),
                Q("Which operator allocates on the heap?", {"new", "malloc", "create", "alloc"}, "new (or malloc from C)."),
                Q("Which frees an array allocated with new T[n]?", {"delete[] p;", "delete p;", "free(p);", "remove p;"}, "Arrays require delete[]."),
            }), "Allocate an int with value 5 on the heap and assign it to p.", "int* p = new ...;",
            {"int* p = new int(5);", "int* p = new int{5};"}, "new int(5)"),

            WithCode(M("RAII", {
                Q("RAII stands for...", {"Resource Acquisition Is Initialization", "Runtime Allocation Is Invalid", "Reference And Iteration", "Readily Available I/O"}, "Resources are tied to object lifetime."),
                Q("When is a destructor called?", {"When the object goes out of scope", "Only when delete is called", "At program start", "Never"}, "Scope end triggers destruction."),
                Q("RAII mainly prevents...", {"Resource leaks", "Stack overflows", "Compile errors", "Slow sorting"}, "Automatic release avoids leaks."),
            }), "Write the destructor for class MyClass (single line).", "~Name() { ... }",
            {"~MyClass() { }", "~MyClass(){}"}, "~ClassName() { }"),
        },
        WithCode(M("Topic Test", {
            Q("A memory leak happens when...", {"You call new without matching delete", "You forget a semicolon", "You use nullptr", "You call delete twice"}, "Every new needs a delete."),
            Q("What does the stack unwind do on an exception?", {"Calls destructors of locals as the stack unwinds", "Skips destructors", "Terminates", "Rewrites the stack"}, "Locals are destructed during unwinding."),
            Q("Where does dynamic memory live?", {"On the heap", "On the stack", "In registers", "In static storage"}, "Heap storage until delete."),
            }), "Allocate an array of 10 ints and assign it to p.", "int* p = new int[...];",
            { "int* p = new int[10];" }, "new int[10]")
            ));

    // ---------------------------------------------------------------- 5: M5
    T.push_back(MakeTopic("Smart Pointers", "Smart",
        "unique_ptr, shared_ptr and weak_ptr: RAII applied to memory ownership.",
        Color{ 60,200,220,255 }, 140,
        {
            WithCode(M("unique_ptr", {
                Q("What ownership model does std::unique_ptr represent?", {"Exclusive ownership", "Shared ownership", "A raw pointer", "A weak reference"}, "One owner at a time."),
                Q("Which factory creates a unique_ptr?", {"std::make_unique<T>(args)", "std::new_unique<T>()", "unique_ptr<T>(new T)", "std::allocate_unique<T>()"}, "Prefer make_unique."),
                Q("Can a unique_ptr be copied?", {"No, only moved", "Yes", "Only for trivially copyable T", "Only when const"}, "Unique_ptr is move-only."),
            }), "Create a unique_ptr<int> p holding 5 using make_unique.", "auto p = std::make_unique<...>(...);",
            {"auto p = std::make_unique<int>(5);", "std::unique_ptr<int> p = std::make_unique<int>(5);"}, "std::make_unique<int>(5)"),

            WithCode(M("shared_ptr & weak_ptr", {
                Q("How does shared_ptr know when to delete?", {"Reference counting", "A garbage collector", "Manual delete only", "Reference cycles"}, "The last owner deletes."),
                Q("Which factory creates a shared_ptr?", {"std::make_shared<T>(args)", "std::shared<T>()", "std::new_shared<T>()", "std::shared_ptr::make<T>()"}, "Prefer make_shared."),
                Q("What does weak_ptr break?", {"Reference cycles", "Reference counting", "Movability", "Const-correctness"}, "weak_ptr observes without owning."),
            }), "Create a shared_ptr<int> p holding 42 using make_shared.", "auto p = std::make_shared<...>(...);",
            {"auto p = std::make_shared<int>(42);", "std::shared_ptr<int> p = std::make_shared<int>(42);"}, "std::make_shared<int>(42)"),
        },
        WithCode(M("Topic Test", {
            Q("After std::move(p), a unique_ptr is...", {"Empty and safely destructible", "Use-after-move", "Undefined", "Copied"}, "Moved-from unique_ptrs are empty."),
            Q("What method upgrades a weak_ptr to a shared_ptr?", {"lock()", "upgrade()", "get()", "share()"}, "weak_ptr.lock() returns shared_ptr or empty."),
            Q("What is the main advantage of shared_ptr over raw pointer?", {"Automatic deletion when the last owner releases", "Faster access", "Smaller size", "Supports null"}, "Ownership is managed by reference count."),
            }), "Declare an empty std::weak_ptr<int> named w.", "std::weak_ptr<...> w;",
            { "std::weak_ptr<int> w;" }, "std::weak_ptr<int> w;")
            ));

    // ---------------------------------------------------------------- 6: M6
    T.push_back(MakeTopic("Classes & Object Lifecycles", "Classes",
        "Encapsulation, constructors/destructors, and the Rule of 3/5/0 with move semantics.",
        Color{ 100,220,180,255 }, 180,
        {
            WithCode(M("Encapsulation & Constructors", {
                Q("What is the default member access in a class?", {"private", "public", "protected", "internal"}, "class defaults to private (struct defaults to public)."),
                Q("What is a constructor's name?", {"Same as the class", "init", "new", "construct"}, "The class name, no return type."),
                Q("Which initializes a member efficiently?", {"A member initializer list in the constructor", "Assignment in the body", "A setter", "A macro"}, "The initializer list constructs in place."),
            }), "Declare an empty class named Dog (include ;).", "class Name { ... };",
            {"class Dog { };", "class Dog{};"}, "class Dog { };"),

            WithCode(M("Rule of 3/5/0 & Move", {
                Q("What does the Rule of Three say?", {"If you define a destructor, copy ctor, or copy assignment, define all three", "Only define destructors", "Never define copy ctors", "Only use move"}, "Consistent resource handling."),
                Q("What is T&&?", {"An rvalue reference", "A pointer", "A const reference", "An array"}, "Used for move semantics."),
                Q("What does std::move(x) do?", {"Casts to an rvalue reference", "Deletes x", "Copies x", "Nulls x"}, "It enables move construction or assignment."),
            }), "Declare a move constructor for class Vec taking Vec&& other (single line, no body).", "Vec(Vec&& o) ...;",
            {"Vec(Vec&& other) noexcept;", "Vec(Vec&& other);"}, "Vec(Vec&& other) noexcept;"),
        },
        WithCode(M("Topic Test", {
            Q("What does noexcept on a move constructor allow?", {"std::vector to move rather than copy on reallocation", "Faster compilation", "Implicit delete", "Nothing"}, "Containers prefer noexcept move for strong guarantee."),
            Q("The Rule of Zero says...", {"Prefer types that need no custom copy/move/dtor", "Always write all five", "Only write the destructor", "Never use smart pointers"}, "Let members handle resources."),
            Q("Copy constructor takes...", {"const T&", "T", "T&&", "T*"}, "Copy from a const ref."),
            }), "Delete the copy constructor of class Vec (modern C++11 form).", "Vec(const Vec&) = ...;",
            { "Vec(const Vec&) = delete;", "Vec(const Vec&) = delete" }, "Vec(const Vec&) = delete;")
            ));

    // ---------------------------------------------------------------- 7: M7
    T.push_back(MakeTopic("Operator Overloading & Polymorphism", "Operators",
        "Making custom types respond to operators, and runtime polymorphism with virtual functions.",
        Color{ 160,140,220,255 }, 230,
        {
            WithCode(M("Operator Overloading", {
                Q("Which operator makes objects printable via std::cout?", {"operator<<", "operator>>", "operator+", "operator()"}, "operator<< for ostream."),
                Q("Which operator makes obj(x) callable?", {"operator()", "operator[]", "operator*", "operator->"}, "operator() makes a functor."),
                Q("Which operator makes obj[i] valid?", {"operator[]", "operator()", "operator.", "operator->"}, "operator[] for subscripting."),
            }), "Declare a member operator+ for Vec taking const Vec& other.", "Vec operator+(...) const;",
            {"Vec operator+(const Vec& other) const;", "Vec operator+(const Vec& other);"}, "Vec operator+(const Vec& other) const;"),

            WithCode(M("Inheritance & virtual", {
                Q("How do you inherit publicly?", {"class Dog : public Animal { }", "class Dog extends Animal", "class Dog inherits Animal", "class Dog(public Animal)"}, "Colon + access specifier."),
                Q("Which keyword enables runtime dispatch?", {"virtual", "abstract", "override", "dynamic"}, "virtual dispatch is resolved at runtime."),
                Q("Why does a polymorphic base need a virtual destructor?", {"So deleting via base pointer destroys the derived part", "For faster destruction", "Required by constructors", "It is a compiler requirement"}, "Otherwise derived destructor is not called."),
            }), "Declare a pure virtual void speak() (end with ;).", "virtual void ... = 0;",
            {"virtual void speak() = 0;", "virtual void speak()=0;"}, "virtual void speak() = 0;"),
        },
        WithCode(M("Topic Test", {
            Q("What does override on a method do?", {"Checks it overrides a base virtual", "Makes it virtual", "Deletes the parent's", "Nothing"}, "override documents and validates intent."),
            Q("What makes a class abstract?", {"A pure virtual method", "A private constructor", "No methods", "A virtual destructor"}, "Pure virtual = 0."),
            Q("Which operator is often a non-member?", {"operator<<", "operator=", "operator[]", "operator()"}, "operator<< takes the stream first."),
            }), "Call the base class's speak() from an overriding method.", "Base::...",
            { "Animal::speak();", "Base::speak();" }, "Base::speak();")
            ));

    // ---------------------------------------------------------------- 8: M8
    T.push_back(MakeTopic("Templates", "Templates",
        "Function templates, class templates, specialization, and the basics of metaprogramming.",
        Color{ 220,140,180,255 }, 280,
        {
            WithCode(M("Function Templates", {
                Q("Which declares a template with type T?", {"template <typename T>", "template <T>", "template T", "generic <T>"}, "The typename keyword names the type parameter."),
                Q("When is a template instantiated?", {"At compile time", "At runtime", "At link time", "Never"}, "The compiler generates concrete code."),
                Q("Templates avoid...", {"Writing the same code for every type", "Runtime overhead", "Both is fine", "Nothing"}, "Zero-cost genericity."),
            }), "Declare a template of type T and function T id(T x) (single line, no body).", "template <...> T ...(...)",
            {"template <typename T> T id(T x);", "template <class T> T id(T x);"}, "template <typename T> T id(T x)"),

            WithCode(M("Class Templates & Specialization", {
                Q("What does a class template look like?", {"template <typename T> class Box { }", "template class<T> Box", "class Box<T>", "generic class Box"}, "template precedes the class."),
                Q("What is template specialization?", {"A different implementation for a specific type", "A faster compiler path", "An inline function", "A macro"}, "Customize behavior for T = X."),
                Q("What is a non-type template parameter?", {"A compile-time value parameter (e.g. int N)", "A reference parameter", "A lambda", "A pointer"}, "template <typename T, int N> allows N."),
            }), "Declare a class template Box with a type parameter T.", "template <...> class ...",
            {"template <typename T> class Box", "template <class T> class Box"}, "template <typename T> class Box"),
        },
        WithCode(M("Topic Test", {
            Q("Which is preferred in modern C++?", {"typename", "class", "type", "T"}, "typename is more general; class is legacy."),
            Q("When are templates compiled?", {"At compile time, once per instantiation", "At runtime, each call", "At link time", "Never"}, "Instantiation happens in the compiler."),
            Q("What is partial specialization?", {"Customizing a template for a subset of types (e.g. T*)", "A deprecated feature", "Customizing every type", "A macro"}, "Partial specialization narrows the pattern."),
            }), "Call the template function id with an int argument 5.", "id<...>(...);",
            { "id<int>(5);", "id(5);" }, "id<int>(5) or id(5)")
            ));

    // ---------------------------------------------------------------- 9: M9
    T.push_back(MakeTopic("STL Containers", "Containers",
        "Sequence, associative, unordered and adapter containers from the standard library.",
        Color{ 60,180,240,255 }, 330,
        {
            WithCode(M("Sequence & Associative", {
                Q("Which is a dynamic array?", {"std::vector", "std::list", "std::array", "std::deque"}, "vector resizes itself."),
                Q("Which container stores unique values?", {"std::set", "std::vector", "std::list", "std::deque"}, "set keeps elements unique."),
                Q("Which stores key-value pairs in sorted order?", {"std::map", "std::vector", "std::unordered_map", "std::set"}, "map keeps keys sorted."),
            }), "Declare a std::vector<int> named v with {1, 2, 3}.", "std::vector<int> v = { ... };",
            {"std::vector<int> v = {1, 2, 3};", "std::vector<int> v{1, 2, 3};"}, "std::vector<int> v = {1, 2, 3};"),

            WithCode(M("Unordered & Adapters", {
                Q("Which is a hash-based map?", {"std::unordered_map", "std::map", "std::vector", "std::set"}, "unordered_map hashes keys."),
                Q("Which container is FIFO?", {"std::queue", "std::stack", "std::vector", "std::set"}, "queue is first-in, first-out."),
                Q("Which container is LIFO?", {"std::stack", "std::queue", "std::vector", "std::map"}, "stack is last-in, first-out."),
            }), "Declare a std::unordered_map<std::string, int> named scores.", "std::unordered_map<...> ...;",
            {"std::unordered_map<std::string, int> scores;", "unordered_map<string, int> scores;"}, "std::unordered_map<std::string, int> scores;"),
        },
        WithCode(M("Topic Test", {
            Q("Which is O(1) average for lookup?", {"std::unordered_map", "std::map", "std::vector", "std::set"}, "Hash tables give average O(1)."),
            Q("Which container maintains sorted order?", {"std::map", "std::unordered_map", "std::vector", "std::queue"}, "map sorts by key."),
            Q("What does priority_queue pop first?", {"The largest by default", "The smallest by default", "The oldest", "The newest"}, "Default uses std::less<T>."),
            }), "Declare a std::vector<std::string> named names.", "std::vector<...> ...;",
            { "std::vector<std::string> names;", "vector<string> names;" }, "std::vector<std::string> names;")
            ));

    // ---------------------------------------------------------------- 10: M10
    T.push_back(MakeTopic("Iterators & Algorithms", "Algorithms",
        "Iterators, STL algorithms and lambdas as algorithm callbacks.",
        Color{ 140,200,220,255 }, 380,
        {
            WithCode(M("Iterators", {
                Q("What does v.begin() return?", {"An iterator to the first element", "The first element", "The vector", "An index"}, "begin() is an iterator."),
                Q("What does v.end() point to?", {"One past the last element", "The last element", "The first element", "nullptr"}, "end() is past-the-end."),
                Q("What category is a vector iterator?", {"Random access", "Forward only", "Input only", "Output only"}, "vector iterators are random-access."),
            }), "Iterate over vector<int> v using an iterator named it (just the for header).", "for (auto it = ...; it != ...; ++it)",
            {"for (auto it = v.begin(); it != v.end(); ++it)", "for(auto it=v.begin();it!=v.end();++it)"}, "for (auto it = v.begin(); it != v.end(); ++it)"),

            WithCode(M("Algorithms & Lambdas", {
                Q("Which algorithm sorts?", {"std::sort", "std::order", "std::arrange", "std::heap"}, "std::sort is in <algorithm>."),
                Q("Which algorithm finds the first matching element?", {"std::find", "std::search_first", "std::get", "std::locate"}, "std::find returns an iterator."),
                Q("Which begins a lambda?", {"[]", "()", "{}", "<>"}, "[] is the capture list."),
            }), "Declare a lambda that adds 1 to a number (just the lambda).", "[...](...) { ... }",
            {"[](int x) { return x + 1; }", "[](int x){return x+1;}"}, "[](int x) { return x + 1; }"),
        },
        WithCode(M("Topic Test", {
            Q("What does std::for_each do?", {"Applies a function to each element", "Sorts the range", "Copies the range", "Finds an element"}, "for_each runs a function per element."),
            Q("Why pass a lambda by value to std::sort?", {"It's the standard signature and cheap for captures", "It's required", "It copies the container", "It's faster"}, "sort takes a comparator by value."),
            Q("What does std::transform do?", {"Writes transformed values to another range", "Sorts the range", "Sums the range", "Nothing"}, "transform = map for iterators."),
            }), "Sort vector<int> v with std::sort.", "std::sort(...);",
            { "std::sort(v.begin(), v.end());", "sort(v.begin(), v.end());" }, "std::sort(v.begin(), v.end());")
            ));

    // ---------------------------------------------------------------- 11: M11
    T.push_back(MakeTopic("Modern Language Features", "Modern",
        "auto, decltype, structured bindings, constexpr and initializer improvements.",
        Color{ 180,220,140,255 }, 430,
        {
            WithCode(M("Type Deduction", {
                Q("What does auto do?", {"Deduces the type from the initializer", "Makes it dynamic", "Requires Java", "Makes it global"}, "Compile-time type inference."),
                Q("What does decltype(expr) yield?", {"The type of expr", "The value of expr", "A pointer", "Nothing"}, "Declares a variable of expr's type."),
                Q("auto x = 3.14; makes x what?", {"double", "float", "decimal", "auto"}, "Decimal literals default to double."),
            }), "Declare auto x initialized to 5.", "auto x = ...;",
            {"auto x = 5;"}, "auto x = 5;"),

            WithCode(M("constexpr & Structured Bindings", {
                Q("What does constexpr promise?", {"Compile-time evaluation when possible", "Runtime evaluation only", "No side effects", "Immutability"}, "The compiler may evaluate at compile time."),
                Q("What does auto [a, b] = pair; do?", {"Structured binding of pair.first and pair.second", "Creates a tuple", "Assigns pair to a and b separately", "Nothing"}, "Structured bindings unpack tuples/pairs/structs."),
                Q("What does {} initialization prevent?", {"Narrowing conversions", "Copies", "Overload resolution", "Lambdas"}, "Brace init rejects narrowing."),
            }), "Use structured bindings to unpack auto [a, b] from pair p.", "auto [a, b] = ...;",
            {"auto [a, b] = p;", "auto[a,b]=p;"}, "auto [a, b] = p;"),
        },
        WithCode(M("Topic Test", {
            Q("Which is preferred for variables you never modify?", {"const auto", "auto", "decltype", "register"}, "const makes intent clear."),
            Q("What does if constexpr do?", {"Compile-time branch selection in templates", "Runtime if", "Throws", "Skips"}, "Compile-time conditional compilation."),
            Q("What does the range-for require of the range?", {"begin() and end() returning iterators", "A size()", "An operator+", "A copy constructor"}, "Range-for uses begin/end."),
            }), "Declare a constexpr int named N with value 10.", "constexpr ... = ...;",
            { "constexpr int N = 10;", "constexpr int N = 10" }, "constexpr int N = 10;")
            ));

    // ---------------------------------------------------------------- 12: M12
    T.push_back(MakeTopic("Concurrency & Error Handling", "Concurrent",
        "Exceptions and exception safety, plus threads, mutexes and atomics.",
        Color{ 220,120,100,255 }, 480,
        {
            WithCode(M("Exceptions", {
                Q("Which keyword throws an exception?", {"throw", "raise", "emit", "signal"}, "throw."),
                Q("Which catch syntax catches anything?", {"catch (...)", "catch (all)", "catch (*)", "catch (Exception&)"}, "Ellipsis catches all."),
                Q("What should custom exceptions inherit from?", {"std::exception or a subclass", "std::error_code", "std::string", "Nothing"}, "Inherit from std::exception."),
            }), "Throw a std::runtime_error with the message \"bad\".", "throw ...(\"...\");",
            {"throw std::runtime_error(\"bad\");"}, "throw std::runtime_error(\"bad\");"),

            WithCode(M("Threads & Atomics", {
                Q("Which header provides std::thread?", {"<thread>", "<concurrency>", "<mutex>", "<atomic>"}, "std::thread is in <thread>."),
                Q("Which class provides a scoped mutex?", {"std::lock_guard", "std::mutex", "std::atomic", "std::thread"}, "lock_guard locks on construction and unlocks on destruction."),
                Q("Which type provides lock-free atomic operations on integers?", {"std::atomic<int>", "std::mutex", "std::thread", "volatile int"}, "std::atomic<int> provides atomic reads/writes."),
            }), "Create a std::thread t that runs the function work().", "std::thread t(...);",
            {"std::thread t(work);", "std::thread t(work); "}, "std::thread t(work);"),
        },
        WithCode(M("Topic Test", {
            Q("What must you call before a std::thread is destroyed?", {"join() or detach()", "close()", "stop()", "delete"}, "Otherwise std::terminate is called."),
            Q("What does std::atomic guarantee?", {"Indivisible reads and writes", "Locking", "Ordering between all threads", "A single thread"}, "Atomic operations are indivisible."),
            Q("What does std::lock_guard do?", {"Locks a mutex in its constructor, unlocks in its destructor", "Locks manually", "Creates a mutex", "Nothing"}, "Scope-based locking."),
            }), "Wait for thread t to finish before continuing.", "t....()",
            { "t.join();" }, "t.join();")
            ));

    return T;
}// ---------------------------------------------------------------------------
// Python course. Same shape as Java/C++: 8 topics, each with 2 modules + 1
// test; each module = 3 MC + 1 small code; each test = 3 MC + 1 small code.
// Content follows the 6-phase Python roadmap:
//   P1 Foundations  -> Basics, Flow
//   P2 Collections  -> Data Structures
//   P3 Functions    -> Functions (def/args, lambdas/generators)
//   P4 OOP          -> Classes & OOP
//   P5 Advanced     -> Exceptions & Modules (decorators, with)
//   P6 Stdlib/IO    -> Strings & Files, Pythonic Code
// ---------------------------------------------------------------------------
inline std::vector<Topic> BuildPythonTopics() {
    std::vector<Topic> T;

    // ---------------------------------------------------------------- 1: M1
    T.push_back(MakeTopic("Syntax, Variables & Basic Types", "Basics",
        "The interpreter, dynamic typing, and Python's core scalar types.",
        Color{ 53,114,165,255 }, 0,
        {
            WithCode(M("The Interpreter & Print", {
                Q("Which function prints to the console?", {"print()", "echo()", "console.log()", "printf()"}, "print() is a built-in."),
                Q("Which file extension do Python scripts use?", {".py", ".pt", ".pyt", ".python"}, ".py."),
                Q("Which line is a comment in Python?", {"# comment", "// comment", "/* comment */", "-- comment"}, "# starts a comment."),
            }), "Print Hello using the print function.", "print(...)",
            {"print(\"Hello\")", "print('Hello')"}, "print(\"Hello\")"),

            WithCode(M("Variables & Types", {
                Q("How do you assign the string 'hi' to x?", {"x = 'hi'", "string x = 'hi'", "let x = 'hi'", "var x := 'hi'"}, "No type keyword."),
                Q("What does type(3.0) return?", {"<class 'float'>", "<class 'int'>", "<class 'double'>", "float"}, "3.0 is a float."),
                Q("Which built-in converts '42' to an int?", {"int('42')", "str('42')", "cast('42')", "toInt('42')"}, "int() converts."),
            }), "Assign the integer 18 to a variable named age.", "name = value",
            {"age = 18"}, "Python assignment is just name = value."),
        },
        WithCode(M("Topic Test", {
            Q("Which is a boolean literal?", {"True", "true", "TRUE", "1b"}, "Capital T."),
            Q("What does 7 // 2 evaluate to?", {"3", "3.5", "4", "2"}, "// is floor division."),
            Q("Which built-in returns the type of a value?", {"type(x)", "typeof(x)", "class(x)", "kind(x)"}, "type() is the built-in."),
            }), "Print the result of 2 + 3.", "print(...)",
            { "print(2 + 3)", "print(2+3)" }, "print(2+3) prints the expression's value.")
            ));

    // ---------------------------------------------------------------- 2: M2
    T.push_back(MakeTopic("Control Flow & Operators", "Flow",
        "if/elif/else, for/while loops, and match-case pattern matching.",
        Color{ 80,150,200,255 }, 40,
        {
            WithCode(M("Conditionals", {
                Q("Which keyword adds another condition?", {"elif", "elseif", "else if", "otherwise"}, "elif."),
                Q("Which is a conditional expression?", {"x if cond else y", "cond ? x : y", "if cond then x else y", "x when cond else y"}, "a if c else b."),
                Q("Blocks are defined by...", {"indentation", "curly braces", "begin/end", "semicolons"}, "Whitespace defines blocks."),
            }), "Assign 'yes' to s if flag is True, else 'no' (use a conditional expression).", "s = ... if ... else ...",
            {"s = 'yes' if flag else 'no'", "s = \"yes\" if flag else \"no\""}, "s = a if cond else b"),

            WithCode(M("Loops & match-case", {
                Q("What does range(3) generate?", {"0, 1, 2", "1, 2, 3", "0, 1, 2, 3", "1, 2"}, "range is exclusive on stop."),
                Q("Which keyword skips to the next iteration?", {"continue", "break", "skip", "next"}, "continue."),
                Q("When does a loop's else block run?", {"When the loop finishes without break", "Always", "Only if the body is empty", "Only after break"}, "Loop else runs on normal exit."),
            }), "Write a while header that runs while i is less than 5 (include :).", "while ...:",
            {"while i < 5:", "while i<5:"}, "while condition:"),
        },
        WithCode(M("Topic Test", {
            Q("What does range(1, 5) generate?", {"1, 2, 3, 4", "1, 2, 3, 4, 5", "0, 1, 2, 3", "2, 3, 4, 5"}, "Stop is exclusive."),
            Q("Which introduces pattern matching (3.10+)?", {"match", "switch", "case", "when"}, "match/case."),
            Q("What is bool([])?", {"False", "True", "0", "Error"}, "Empty containers are falsy."),
            }), "Write a for header iterating over range(5) (include :).", "for ... in ...:",
            { "for i in range(5):" }, "for i in range(5):")
            ));

    // ---------------------------------------------------------------- 3: M3
    T.push_back(MakeTopic("Core Collections", "Data",
        "Lists, tuples, dicts and sets - Python's four workhorse types.",
        Color{ 100,140,200,255 }, 70,
        {
            WithCode(M("Lists & Tuples", {
                Q("Which of these is mutable?", {"list", "tuple", "str", "frozenset"}, "Lists can change."),
                Q("Which creates a tuple?", {"(1, 2, 3)", "[1, 2, 3]", "{1, 2, 3}", "<1, 2, 3>"}, "Parentheses make a tuple."),
                Q("Which method adds to the END of a list?", {"append", "push", "insert", "extend"}, "lst.append(x)."),
            }), "Create a list named xs holding 1, 2, 3.", "name = [ ... ]",
            {"xs = [1, 2, 3]", "xs = [1,2,3]"}, "Square brackets make a list."),

            WithCode(M("Dicts & Sets", {
                Q("Which creates a dict literal?", {"{'a': 1}", "['a', 1]", "('a', 1)", "{'a', 1}"}, "Dicts use key: value."),
                Q("Which collection stores unique values only?", {"set", "list", "tuple", "deque"}, "Sets reject duplicates."),
                Q("Which method returns all keys?", {".keys()", ".items()", ".values()", ".all()"}, "dict.keys()."),
            }), "Create a dict mapping 'a' to 1 and 'b' to 2.", "{ ...: ..., ...: ... }",
            {"{'a': 1, 'b': 2}", "d = {'a': 1, 'b': 2}"}, "{'a': 1, 'b': 2}"),
        },
        WithCode(M("Topic Test", {
            Q("What does [1, 2, 3][-1] return?", {"3", "1", "Error", "0"}, "Negative indexes count from the end."),
            Q("Which set operation gives intersection?", {"a & b", "a | b", "a - b", "a ^ b"}, "& is intersection."),
            Q("Are tuples hashable?", {"Yes, if their elements are hashable", "No", "Only with frozenset", "Only of length 1"}, "Tuples can be dict keys if hashable."),
            }), "Create a set named s with values 1, 2, 3.", "name = { ... }",
            { "s = {1, 2, 3}", "s = {1,2,3}" }, "Braces make a set (with no colons).")
            ));

    // ---------------------------------------------------------------- 4: M4
    T.push_back(MakeTopic("Comprehensions & Advanced Slicing", "Compreh",
        "Concise loops that build collections, plus the full slice syntax.",
        Color{ 120,150,210,255 }, 100,
        {
            WithCode(M("Comprehensions", {
                Q("What does [x*x for x in range(3)] produce?", {"[0, 1, 4]", "[1, 4, 9]", "[0, 1, 2]", "[1, 2, 3]"}, "range(3) is 0, 1, 2."),
                Q("What does {k: v for k, v in pairs} produce?", {"A dict", "A list", "A set", "A tuple"}, "Dict comprehension."),
                Q("What does {x for x in seq} produce?", {"A set", "A dict", "A list", "A tuple"}, "Set comprehension."),
            }), "Write a list comprehension of x for x in range(5) when x is even.", "[x for x in ... if ...]",
            {"[x for x in range(5) if x % 2 == 0]", "[x for x in range(5) if x%2==0]"}, "[x for x in range(5) if x % 2 == 0]"),

            WithCode(M("Slicing", {
                Q("What does s[1:4] return?", {"Elements at indexes 1, 2, 3", "Elements 1 through 4", "First four elements", "One element"}, "Stop is exclusive."),
                Q("What does s[::-1] do?", {"Reverses the sequence", "Returns s unchanged", "Returns empty", "Doubles s"}, "Step -1 reverses."),
                Q("What does s[::2] do?", {"Every second element", "First two elements", "Last two elements", "Skips two"}, "Step 2 selects every other."),
            }), "Get the last three elements of a list lst.", "lst[...]",
            {"lst[-3:]", "lst[-3::]"}, "lst[-3:]"),
        },
        WithCode(M("Topic Test", {
            Q("Which comprehension is fastest to write?", {"List comprehension", "map + list", "for loop + append", "filter + list"}, "List comprehensions are idiomatic."),
            Q("Does a generator expression create a list?", {"No, it yields lazily", "Yes", "Only in Python 3", "Only with list()"}, "(expr for x in it) is a generator."),
            Q("What does {**a, **b} do?", {"Merges two dicts into a new dict", "Creates a set", "Errors", "Raises"}, "Dict unpacking merge."),
            }), "Extract every other element of lst starting at 0.", "lst[...]",
            { "lst[::2]" }, "lst[::2]")
            ));

    // ---------------------------------------------------------------- 5: M5
    T.push_back(MakeTopic("Functions & Scope", "Functions",
        "def, *args/**kwargs, and the LEGB rule.",
        Color{ 140,170,200,255 }, 140,
        {
            WithCode(M("Defining Functions", {
                Q("Which keyword defines a function?", {"def", "func", "fn", "function"}, "def."),
                Q("What does a bare return produce?", {"None", "0", "False", "Error"}, "None."),
                Q("Positional arguments are matched by...", {"Position", "Name", "Type", "Value"}, "Order matters for positional."),
            }), "Define a function square(x) that returns x*x (one line).", "def name(params): return ...",
            {"def square(x): return x * x", "def square(x): return x*x"}, "def square(x): return x * x"),

            WithCode(M("*args, **kwargs & Scope", {
                Q("What does *args collect?", {"Extra positional arguments into a tuple", "Extra keywords into a dict", "A list", "A set"}, "Star collects positional."),
                Q("What does **kwargs collect?", {"Extra keyword arguments into a dict", "Positional into a tuple", "A list", "A set"}, "Double-star collects keywords."),
                Q("Which order does LEGB follow?", {"Local, Enclosing, Global, Built-in", "Local, External, Global, Base", "List, Env, Global, Built-in", "Local, Error, Getter, Bytecode"}, "LEGB."),
            }), "Define a function add that accepts *args and returns their sum.", "def add(*args): return ...",
            {"def add(*args): return sum(args)"}, "def add(*args): return sum(args)"),
        },
        WithCode(M("Topic Test", {
            Q("Which keyword rebinds a module-level name inside a function?", {"global", "nonlocal", "outer", "module"}, "global."),
            Q("Which rebinds a name in an enclosing function?", {"nonlocal", "global", "outer", "enclosing"}, "nonlocal."),
            Q("Can a function have a variable named 'len' inside it?", {"Yes, it shadows the built-in locally", "No", "Only with -X", "Only at module scope"}, "Local scope shadows built-ins."),
            }), "Call greet with argument 'Ada'.", "greet(...);",
            { "greet('Ada')", "greet(\"Ada\")" }, "greet('Ada')")
            ));

    // ---------------------------------------------------------------- 6: M6
    T.push_back(MakeTopic("Functional Tools & Generators", "Functional",
        "Lambdas, map/filter/zip/enumerate and generator expressions.",
        Color{ 160,180,220,255 }, 180,
        {
            WithCode(M("Lambdas & Built-ins", {
                Q("Which is a valid lambda?", {"lambda x: x + 1", "lambda(x) { return x+1; }", "lambda x -> x+1", "(x) => x+1"}, "lambda args: expr."),
                Q("What does map(f, seq) produce?", {"An iterator of f(x) for each x", "A list", "A tuple", "A set"}, "map returns an iterator in Python 3."),
                Q("What does enumerate(seq) yield?", {"(index, value) pairs", "values only", "indices only", "(value, index) pairs"}, "Indexes come first."),
            }), "Create a lambda named double that doubles its argument.", "double = lambda x: ...",
            {"double = lambda x: x * 2", "double = lambda x: 2 * x", "double=lambda x:x*2"}, "lambda x: x * 2"),

            WithCode(M("Generators", {
                Q("What does yield do?", {"Pauses the function and produces a value", "Exits the function", "Returns None", "Prints the value"}, "yield makes a generator."),
                Q("Are generators lazy?", {"Yes, they produce values on demand", "No, they build a list", "Only in Python 2", "Only with map()"}, "Generators are lazy."),
                Q("Which built-in filters a sequence with a predicate?", {"filter", "select", "where", "keep"}, "filter(func, seq)."),
            }), "Write a generator function counter() that yields 0, 1, 2.", "def name(): yield ...",
            {"def counter():\n    yield 0\n    yield 1\n    yield 2", "def counter(): yield 0; yield 1; yield 2"}, "Generator functions use yield."),
        },
        WithCode(M("Topic Test", {
            Q("Which function zips two sequences?", {"zip", "pair", "combine", "join"}, "zip(a, b)."),
            Q("What does zip() return in Python 3?", {"An iterator of tuples", "A list of tuples", "A dict", "A single tuple"}, "zip is lazy."),
            Q("Can a generator be iterated twice?", {"No, only once", "Yes, freely", "Only with list()", "Only in async"}, "Generators exhaust."),
            }), "Iterate over names with enumerate in a for header (include :).", "for ... in enumerate(...):",
            { "for i, name in enumerate(names):" }, "for i, name in enumerate(names):")
            ));

    // ---------------------------------------------------------------- 7: M7
    T.push_back(MakeTopic("Classes & Objects", "Classes",
        "class, __init__, self, instance vs class variables.",
        Color{ 180,200,220,255 }, 230,
        {
            WithCode(M("Classes & __init__", {
                Q("Which method is the constructor?", {"__init__", "__new__", "__ctor__", "init"}, "__init__."),
                Q("What is the first parameter of an instance method?", {"self", "this", "cls", "me"}, "self."),
                Q("How do you create a Dog object?", {"Dog()", "new Dog()", "Dog", "create Dog"}, "Calling the class instantiates it."),
            }), "Write the __init__ header for class Dog taking self and name (include :).", "def __init__(...):",
            {"def __init__(self, name):", "def __init__(self,name):"}, "def __init__(self, name):"),

            WithCode(M("Instance vs Class Variables", {
                Q("Where does a class variable live?", {"On the class, shared across instances", "On each instance", "In __init__ only", "In the module"}, "Class variables are shared."),
                Q("Where does an instance variable live?", {"On the object, set via self", "On the class", "In globals", "In __dict__ of the module"}, "Instance attributes are per-object."),
                Q("What does @classmethod change about the first param?", {"It receives the class (cls) instead of the instance", "It receives nothing", "It receives the module", "It makes it async"}, "Classmethods take cls."),
            }), "Declare a class-level constant name = 'dog' inside class Dog (single line).", "name = ...",
            {"name = 'dog'", "name = \"dog\""}, "name = 'dog'"),
        },
        WithCode(M("Topic Test", {
            Q("How do you call a method on obj?", {"obj.method()", "method(obj)", "obj.method", "obj->method()"}, "obj.method()."),
            Q("What does @staticmethod mean?", {"The method receives neither self nor cls", "It receives the class", "It receives the instance", "It is deprecated"}, "Static methods take no implicit first arg."),
            Q("What does __dict__ hold?", {"Instance attributes", "Class attributes only", "Methods only", "The module"}, "Instances store attributes in __dict__."),
            }), "Create a Dog object named d.", "d = ...()",
            { "d = Dog()" }, "d = Dog()")
            ));

    // ---------------------------------------------------------------- 8: M8
    T.push_back(MakeTopic("Advanced OOP & Dunder Methods", "OOP",
        "Dunder methods, inheritance/super(), properties and name mangling.",
        Color{ 200,180,200,255 }, 280,
        {
            WithCode(M("Dunder Methods", {
                Q("Which method customizes str(obj)?", {"__str__", "__repr__", "__print__", "str"}, "__str__."),
                Q("Which method customizes len(obj)?", {"__len__", "__size__", "__length__", "len"}, "__len__."),
                Q("Which method customizes obj == other?", {"__eq__", "__cmp__", "__same__", "__match__"}, "__eq__."),
            }), "Write the header of a __str__ method (include :).", "def __str__(...):",
            {"def __str__(self):"}, "def __str__(self):"),

            WithCode(M("Inheritance & Properties", {
                Q("How do you inherit from Animal?", {"class Dog(Animal):", "class Dog extends Animal:", "class Dog : Animal {", "class Dog inherits Animal:"}, "Parentheses after the class name."),
                Q("What does super().__init__() do?", {"Calls the parent's constructor", "Makes it final", "Creates a subclass", "Deletes the parent"}, "super() reaches the parent."),
                Q("What decorator makes a method a getter?", {"@property", "@getter", "@staticmethod", "@field"}, "@property."),
            }), "Call the parent class's speak() from a child method.", "super()...",
            {"super().speak()"}, "super().speak()"),
        },
        WithCode(M("Topic Test", {
            Q("What does __repr__ aim to do?", {"Provide an unambiguous, ideally eval-able string", "Print pretty", "Compare objects", "Hash the object"}, "repr is for debugging."),
            Q("What does name mangling do to __x inside class C?", {"Renames it to _C__x", "Makes it public", "Deletes it", "Nothing"}, "Avoids accidental inheritance clashes."),
            Q("What is MRO?", {"Method Resolution Order for multiple inheritance", "A decorator", "A built-in", "An exception"}, "MRO defines lookup order."),
            }), "Define a property getter for a name attribute (decorator only).", "@property",
            { "@property" }, "@property")
            ));

    // ---------------------------------------------------------------- 9: M9
    T.push_back(MakeTopic("Exception Handling", "Errors",
        "try/except/else/finally and custom exception classes.",
        Color{ 220,140,140,255 }, 330,
        {
            WithCode(M("try / except / else / finally", {
                Q("Which block always runs, error or not?", {"finally", "except", "try", "else"}, "finally."),
                Q("Which keyword raises an exception?", {"raise ValueError('bad')", "throw ValueError", "raise(ValueError)", "error ValueError"}, "raise."),
                Q("When does try's else block run?", {"No exception was raised", "Exception was raised", "Always", "Never"}, "else runs on success."),
            }), "Raise a ValueError with the message 'bad'.", "raise ...('...')",
            {"raise ValueError('bad')", "raise ValueError(\"bad\")"}, "raise ValueError('bad')"),

            WithCode(M("Custom Exceptions", {
                Q("What should custom exceptions subclass?", {"Exception", "Error", "BaseException", "object"}, "Subclass Exception."),
                Q("Which exception should you normally NOT catch?", {"BaseException", "Exception", "ValueError", "TypeError"}, "BaseException includes KeyboardInterrupt/SystemExit."),
                Q("What does the with statement use to signal errors?", {"Exceptions raised inside the with block", "A return code", "A sentinel", "Nothing"}, "Exceptions propagate normally."),
            }), "Define a custom exception class named MyError.", "class Name(Exception): ...",
            {"class MyError(Exception): pass", "class MyError(Exception):\n    pass"}, "class MyError(Exception): pass"),
        },
        WithCode(M("Topic Test", {
            Q("What does re-raising do?", {"Propagates an exception to the caller", "Swallows it", "Logs it", "Deletes it"}, "raise in an except block."),
            Q("Which clause can add a condition to an except?", {"None - use subclassing", "if", "when", "where"}, "Use a more specific exception or inspect in the block."),
            Q("What does traceback show?", {"The chain of calls leading to the exception", "The message only", "The type only", "Nothing"}, "traceback shows the call chain."),
            }), "Catch a ValueError as e.", "except ... as ...:",
            { "except ValueError as e:", "except ValueError as e" }, "except ValueError as e:")
            ));

    // ---------------------------------------------------------------- 10: M10
    T.push_back(MakeTopic("Decorators & Context Managers", "Decorators",
        "First-class functions, closures, @decorator, and the with protocol.",
        Color{ 200,120,120,255 }, 380,
        {
            WithCode(M("Decorators", {
                Q("What does @decorator above def f do?", {"Passes f through decorator and rebinds the name", "Renames f", "Deletes f", "Prints f"}, "Decorators wrap functions."),
                Q("Which module provides functools.wraps?", {"functools", "builtins", "decorators", "itertools"}, "from functools import wraps."),
                Q("What do closures capture?", {"Their enclosing scope's variables", "Nothing", "Only globals", "Only their own arguments"}, "Closures remember their lexical environment."),
            }), "Decorate a function f with @wraps.", "@...",
            {"@wraps(f)", "@wraps(f)"}, "@wraps(f)"),

            WithCode(M("Context Managers", {
                Q("Which method runs when entering a with-block?", {"__enter__", "__init__", "__with__", "__open__"}, "__enter__."),
                Q("Which method runs on exit?", {"__exit__", "__del__", "__close__", "__finally__"}, "__exit__."),
                Q("What does __exit__ receive?", {"Exception type, value and traceback", "Nothing", "The block's return", "Only the exception message"}, "__exit__(exc_type, exc, tb)."),
            }), "Open 'f.txt' for reading using a with statement (header only).", "with open(...) as ...:",
            {"with open('f.txt') as f:", "with open(\"f.txt\") as f:"}, "with open('f.txt') as f:"),
        },
        WithCode(M("Topic Test", {
            Q("What does @contextmanager from contextlib do?", {"Turns a generator into a context manager", "Adds __enter__/__exit__", "Both are correct", "Nothing"}, "It wraps a generator yielding once."),
            Q("Why use functools.wraps on a decorator's inner function?", {"Preserves the wrapped function's name and docstring", "For speed", "For type-checking", "Required"}, "Preserves metadata."),
            Q("Is a decorator a callable?", {"Yes", "No", "Only classes", "Only functions"}, "Decorators are callables that return callables."),
            }), "Import wraps from functools.", "from ??? import ???",
            { "from functools import wraps" }, "from functools import wraps")
            ));

    // ---------------------------------------------------------------- 11: M11
    T.push_back(MakeTopic("File I/O & Serialization", "Files",
        "with open(), json and csv from the standard library.",
        Color{ 180,100,100,255 }, 430,
        {
            WithCode(M("File Reading & Writing", {
                Q("What is the safe way to open a file?", {"with open(...) as f:", "f = open(...)", "open(...)", "file = fopen(...)"}, "The with block closes it."),
                Q("Which mode truncates the file?", {"'w'", "'r'", "'a'", "'x'"}, "'w' truncates."),
                Q("Which method reads the entire file into a string?", {"f.read()", "f.readlines()", "f.readline()", "f.read(1)"}, "read() with no args reads all."),
            }), "Read the entire text file 'data.txt' into a string s.", "s = open(...).read()",
            {"with open('data.txt') as f: s = f.read()", "s = open('data.txt').read()"}, "with open('data.txt') as f: s = f.read()"),

            WithCode(M("JSON & CSV", {
                Q("Which module serializes JSON?", {"json", "csv", "pickle", "os"}, "import json."),
                Q("Which function converts a dict to a JSON string?", {"json.dumps(d)", "json.loads(d)", "json.to_string(d)", "json.encode(d)"}, "dumps = dump string."),
                Q("Which function reads a JSON file?", {"json.load(f)", "json.loads(f)", "json.read(f)", "json.parse(f)"}, "load reads from a file."),
            }), "Parse the JSON string txt into a Python object.", "json....(txt)",
            {"json.loads(txt)"}, "json.loads(txt)"),
        },
        WithCode(M("Topic Test", {
            Q("Which iterates over a file lazily?", {"for line in f:", "while f.read():", "for line in f.readlines():", "f.read().split()"}, "Iterating the file object."),
            Q("What does csv.reader return?", {"An iterator of rows (lists of strings)", "A list of dicts", "A dict", "A string"}, "Rows are lists of strings."),
            Q("Which mode opens a file for binary reading?", {"'rb'", "'r'", "'b'", "'rb+'"}, "'rb' is read-binary."),
            }), "Convert the dict d to a JSON string.", "json....(d)",
            { "json.dumps(d)" }, "json.dumps(d)")
            ));

    // ---------------------------------------------------------------- 12: M12
    T.push_back(MakeTopic("Essential Standard Library", "Stdlib",
        "pathlib, os, datetime, math, collections and itertools.",
        Color{ 160,80,80,255 }, 480,
        {
            WithCode(M("Path & OS", {
                Q("Which module provides Path?", {"pathlib", "os", "sys", "fileio"}, "from pathlib import Path."),
                Q("Which module reads command-line args?", {"sys", "os", "argparse", "cli"}, "sys.argv."),
                Q("Which pathlib method joins paths?", {"the / operator", "+", "join()", "concat()"}, "Path('a') / 'b'."),
            }), "Import Path from pathlib.", "from ??? import ???",
            {"from pathlib import Path"}, "from pathlib import Path"),

            WithCode(M("Time, Math & Data Utilities", {
                Q("Which module handles dates and times?", {"datetime", "time", "calendar", "date"}, "import datetime."),
                Q("Which function computes square roots?", {"math.sqrt", "math.root", "math.pow", "math.square"}, "math.sqrt."),
                Q("Which collections class counts occurrences?", {"collections.Counter", "collections.Count", "collections.Tally", "collections.Histogram"}, "Counter."),
            }), "Create a Counter from the list words.", "Counter(...)",
            {"Counter(words)", "from collections import Counter; Counter(words)"}, "Counter(words)"),
        },
        WithCode(M("Topic Test", {
            Q("Which collections class supplies a default for missing keys?", {"collections.defaultdict", "collections.Counter", "dict.default", "collections.Dict"}, "defaultdict factory."),
            Q("Which itertools function chains iterables?", {"itertools.chain", "itertools.join", "itertools.concat", "itertools.combine"}, "chain(a, b) chains."),
            Q("Which datetime class represents a timezone-aware timestamp?", {"datetime.datetime", "datetime.date", "datetime.time", "datetime.timestamp"}, "datetime is the standard."),
            }), "Import chain from itertools.", "from ??? import ???",
            { "from itertools import chain" }, "from itertools import chain")
            ));

    return T;
}// ---------------------------------------------------------------------------
// C# course. Same shape as Java/C++/Python: 8 topics, each with 2 modules + 1
// test; each module = 3 MC + 1 small code; each test = 3 MC + 1 small code.
// Content follows the 5-phase .NET roadmap:
//   P1 Foundations  -> Basics, Flow, Methods
//   P2 OOP          -> Classes & Objects, Inheritance & Interfaces
//   P3 Advanced     -> Collections & Generics, LINQ & Lambdas
//   P4 Async        -> Async & Exceptions
//   P5 Memory/Misc  -> folded into Async & Exceptions (IDisposable, using)
// ---------------------------------------------------------------------------
inline std::vector<Topic> BuildCSharpTopics() {
    std::vector<Topic> T;

    // ---------------------------------------------------------------- 1: M1
    T.push_back(MakeTopic("Basic Syntax & Types", "Basics",
        "CLR, value vs. reference types, var, and nullable references.",
        Color{ 155,79,150,255 }, 0,
        {
            WithCode(M("The Runtime & Types", {
                Q("What does CLR stand for?", {"Common Language Runtime", "C# Language Runtime", "Central Language Runtime", "Core Level Runtime"}, "CLR is .NET's virtual machine."),
                Q("Which is a value type?", {"int", "string", "List<int>", "object"}, "Primitives are value types."),
                Q("Which is a reference type?", {"object", "int", "bool", "char"}, "object is the root reference type."),
            }), "Declare an int x and set it to 5.", "int name = value;",
            {"int x = 5;"}, "int x = 5;"),

            WithCode(M("var & Nullability", {
                Q("What does var require?", {"An initializer", "A type annotation", "A public modifier", "A static modifier"}, "var infers from the initializer."),
                Q("What does string? signal?", {"A nullable reference type", "An optional field", "A static string", "A read-only string"}, "NRT: compiler warns on possible null."),
                Q("Which declares a nullable int?", {"int? x = null;", "int x = null;", "Nullable<int>? x;", "int x?;"}, "int? is shorthand for Nullable<int>."),
            }), "Print Hello using Console.WriteLine.", "Console.WriteLine(...);",
            {"Console.WriteLine(\"Hello\");"}, "Console.WriteLine(\"Hello\");"),
        },
        WithCode(M("Topic Test", {
            Q("What is CIL?", {"Common Intermediate Language", "C# Intermediate Language", "Compiled Intermediate Layer", "Core Interop Library"}, "Assemblies contain CIL; JIT compiles it."),
            Q("How is var different from dynamic?", {"var is compile-time; dynamic is runtime", "They are the same", "dynamic is faster", "var is deprecated"}, "var is inferred; dynamic is late-bound."),
            Q("Where do value-type locals live?", {"On the stack", "On the heap", "In static storage", "In registers only"}, "Value types are stack-allocated when local."),
            }), "Print the result of 2 + 3.", "Console.WriteLine(...)",
            { "Console.WriteLine(2 + 3);", "Console.WriteLine(2+3);" }, "Console.WriteLine(2 + 3);")
            ));

    // ---------------------------------------------------------------- 2: M2
    T.push_back(MakeTopic("Control Flow & Pattern Matching", "Flow",
        "if/else, switch expressions, loops, and C#'s pattern matching.",
        Color{ 175,89,180,255 }, 40,
        {
            WithCode(M("Conditionals & switch", {
                Q("Which is the modern switch style?", {"value switch { ... }", "switch(value) return", "case (value)", "match-case"}, "C# 8+ switch expressions."),
                Q("Which is valid C#?", {"if (x > 5) { }", "if x > 5 then { }", "if (x > 5) then { }", "if x > 5 { }"}, "Conditions go in parentheses."),
                Q("Which is the default label in a switch statement?", {"default:", "else:", "otherwise:", "case none:"}, "default: is the fallback."),
            }), "Print \"big\" if x > 10 (one-line if).", "if (...) Console.WriteLine(...);",
            {"if (x > 10) Console.WriteLine(\"big\");", "if(x>10)Console.WriteLine(\"big\");"}, "A single if around Console.WriteLine."),

            WithCode(M("Loops & Pattern Matching", {
                Q("What does foreach iterate over?", {"Any IEnumerable", "Only arrays", "Only lists", "Only strings"}, "foreach works over IEnumerable."),
                Q("Which loop runs its body at least once?", {"do-while", "while", "for", "foreach"}, "do-while checks after the body."),
                Q("Which keyword pattern-matches and binds a variable?", {"is", "as", "in", "let"}, "is (pattern) binds within a scope."),
            }), "Write the header of a while loop that runs while i < 5 (include opening brace).", "while (...) {",
            {"while (i < 5) {", "while(i<5){"}, "while (condition) { ... }"),
        },
        WithCode(M("Topic Test", {
            Q("What does break do?", {"Exits the loop immediately", "Skips to the next iteration", "Restarts the loop", "Exits the program"}, "break leaves the loop."),
            Q("Which pattern tests a property value?", {"obj is { Age: > 18 }", "obj.Age > 18", "when obj.Age", "match Age > 18"}, "Property patterns bind and check."),
            Q("Which operator tests a type safely?", {"is", "as", "typeof", "cast"}, "is returns bool; as returns null on failure."),
            }), "Write a for loop header counting i = 0..9 (include opening brace).", "for (...; ...; ...) {",
            { "for (int i = 0; i < 10; i++) {", "for(int i=0;i<10;i++){" }, "for (init; cond; step) {")
            ));

    // ---------------------------------------------------------------- 3: M3
    T.push_back(MakeTopic("Strings & Text Handling", "Strings",
        "Immutability, StringBuilder, interpolation and raw string literals.",
        Color{ 195,99,190,255 }, 70,
        {
            WithCode(M("String Manipulation", {
                Q("Are C# strings mutable?", {"No", "Yes", "Only with StringBuilder", "Only ASCII ones"}, "Strings are immutable."),
                Q("Which prefix enables interpolation?", {"$", "@", "#", "%"}, "The $ prefix."),
                Q("Which class builds strings efficiently?", {"StringBuilder", "String", "TextBuilder", "StringBuffer"}, "StringBuilder from System.Text."),
            }), "Create a string s that interpolates 'Hello, ' + the variable name.", "$...{...}...",
            {"string s = $\"Hello, {name}\";", "string s = $\"Hello, {name}\""}, "$\"Hello, {name}\""),

            WithCode(M("Raw & Verbatim Strings", {
                Q("Which prefix disables escape sequences?", {"@", "$", "#", "~"}, "@\"...\""),
                Q("What is a raw string literal (C# 11+)?", {"\"\"\"...\"\"\"", "@\"...\"", "$...$", "'...'"}, "Triple quotes make a raw literal."),
                Q("What does .Length return?", {"Number of characters", "Number of words", "Number of bytes", "The string itself"}, "Length is the character count."),
            }), "Get the length of the string s.", "s....",
            {"s.Length"}, "s.Length (a property, no parentheses)"),
        },
        WithCode(M("Topic Test", {
            Q("What does ToUpper() return?", {"A new uppercase string", "Mutates in place", "A bool", "The original string"}, "Strings are immutable - a new string is returned."),
            Q("What does string.Join do?", {"Concatenates strings with a separator", "Splits the string", "Replaces substrings", "Removes whitespace"}, "Join(sep, items)."),
            Q("What does string.IsNullOrEmpty(s) check?", {"Null or zero-length", "Null only", "Empty only", "Whitespace"}, "Covers both null and empty."),
            }), "Split the string s on commas.", "s.Split(...)",
            { "s.Split(',')", "s.Split(\",\")" }, "s.Split(',')")
            ));

    // ---------------------------------------------------------------- 4: M4
    T.push_back(MakeTopic("Classes, Structs & Records", "Classes",
        "Classes with auto-properties, performance structs, and value-based records.",
        Color{ 215,109,190,255 }, 100,
        {
            WithCode(M("Classes & Properties", {
                Q("Which is an auto-property?", {"public int Age { get; set; }", "public int Age;", "public int get_Age() { }", "public Age { }"}, "The compiler generates a backing field."),
                Q("What is the default member access inside a class?", {"private", "public", "protected", "internal"}, "Class members default to private."),
                Q("What does internal mean?", {"Accessible within the same assembly", "Accessible everywhere", "Accessible in subclasses", "Private to the class"}, "internal = same assembly only."),
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
            Q("What does readonly struct enforce?", {"All fields are readonly", "The struct is sealed", "Only static methods", "Nothing"}, "Readonly struct cannot mutate."),
            Q("Why prefer a record for DTOs?", {"Built-in value equality and ToString", "Faster than classes", "Smaller memory", "Supports inheritance only"}, "Records simplify data models."),
            Q("Which are reference types by default?", {"classes", "structs", "ints", "enums"}, "Classes are reference types."),
            }), "Create a Cat object named c.", "Cat c = new ...;",
            { "Cat c = new Cat();" }, "Cat c = new Cat();")
            ));

    // ---------------------------------------------------------------- 5: M5
    T.push_back(MakeTopic("Core OOP & Interfaces", "OOP",
        "Inheritance, virtual/override, abstract classes, interfaces and DIM.",
        Color{ 235,119,190,255 }, 140,
        {
            WithCode(M("Inheritance & Polymorphism", {
                Q("How do you inherit from Animal?", {"class Dog : Animal { }", "class Dog extends Animal", "class Dog inherits Animal", "class Dog(Animal)"}, "Colon + base type."),
                Q("Which keyword lets a base method be overridden?", {"virtual", "override", "abstract", "sealed"}, "Mark the base method virtual."),
                Q("Which keyword replaces a base method?", {"override", "virtual", "new", "implements"}, "override replaces the base implementation."),
            }), "Write the class header: Dog inherits from Animal (include opening brace).", "class ... : ... {",
            {"class Dog : Animal {", "class Dog:Animal{"}, "class Dog : Animal {"),

            WithCode(M("Interfaces & DIM", {
                Q("How do you implement IPet?", {"class Cat : IPet { }", "class Cat implements IPet", "class Cat inherits IPet", "class Cat(IPet)"}, "Interfaces go after the colon too."),
                Q("Can a class implement multiple interfaces?", {"Yes, any number", "No, only one", "Only two", "Only with generics"}, "A class implements any number."),
                Q("What is a default interface method?", {"Has a body and can be overridden", "Abstract only", "Cannot be called", "A constructor"}, "DIMs allow implementation in the interface."),
            }), "Declare an interface named IPet (include opening brace).", "interface ... {",
            {"interface IPet {", "interface IPet{"}, "interface IPet {"),
        },
        WithCode(M("Topic Test", {
            Q("What does base refer to?", {"The parent class", "The current class", "The assembly", "The namespace"}, "base reaches the parent."),
            Q("What does abstract mean for a method?", {"No body; subclasses must implement", "Final", "Static", "Private"}, "Abstract methods have no body."),
            Q("Can a class inherit multiple base classes?", {"No", "Yes, any number", "Only two", "Only via interfaces"}, "Single class inheritance only."),
            }), "Call the parent class's Speak() from an overriding method.", "base....()",
            { "base.Speak();" }, "base.Speak();")
            ));

    // ---------------------------------------------------------------- 6: M6
    T.push_back(MakeTopic("Generics & Collections", "Generics",
        "Generic classes/methods with constraints, plus the collection types.",
        Color{ 190,60,170,255 }, 180,
        {
            WithCode(M("Generics", {
                Q("Which declares a type parameter?", {"<T>", "[T]", "(T)", "{T}"}, "Angle brackets."),
                Q("Which is a generic constraint?", {"where T : class", "where T is class", "constrain T", "T : class"}, "The where clause."),
                Q("What do generics provide?", {"Compile-time type safety", "Runtime-only checks", "Faster parsing", "Dynamic typing"}, "Type safety without casts."),
            }), "Declare a generic method int GetId<T>(T value) with no body.", "int GetId<T>(T value);",
            {"int GetId<T>(T value);"}, "int GetId<T>(T value);"),

            WithCode(M("Collections", {
                Q("Which is a resizable array?", {"List<T>", "Array", "ArrayList", "Span<T>"}, "List<T> grows automatically."),
                Q("Which stores key-value pairs?", {"Dictionary<TKey, TValue>", "List<T>", "HashSet<T>", "Queue<T>"}, "Dictionary is a hash map."),
                Q("Which stores unique elements?", {"HashSet<T>", "List<T>", "Queue<T>", "Dictionary<TKey,TValue>"}, "HashSet rejects duplicates."),
            }), "Create an empty List<int> named nums.", "var nums = new ...",
            {"var nums = new List<int>();", "List<int> nums = new List<int>();"}, "new List<int>()"),
        },
        WithCode(M("Topic Test", {
            Q("Queue<T> is...", {"FIFO", "LIFO", "Sorted", "Random"}, "First-in, first-out."),
            Q("Stack<T> is...", {"LIFO", "FIFO", "Sorted", "Random"}, "Last-in, first-out."),
            Q("Which collection is thread-safe?", {"ConcurrentDictionary<TKey, TValue>", "Dictionary<TKey, TValue>", "List<T>", "Queue<T>"}, "The Concurrent* collections are thread-safe."),
            }), "Create an empty Dictionary<string, int> named ages.", "var ages = new ...",
            { "var ages = new Dictionary<string, int>();", "Dictionary<string, int> ages = new Dictionary<string, int>();" }, "new Dictionary<string, int>()")
            ));

    // ---------------------------------------------------------------- 7: M7
    T.push_back(MakeTopic("Delegates, Events & Lambdas", "Delegates",
        "Func/Action/Predicate, lambdas, custom delegates and events.",
        Color{ 220,80,140,255 }, 230,
        {
            WithCode(M("Delegates & Lambdas", {
                Q("Which is a valid lambda?", {"x => x * 2", "lambda x: x * 2", "(x) -> x * 2", "func(x) => x * 2"}, "The => operator."),
                Q("Action<T> returns...", {"void", "T", "bool", "int"}, "Action returns void."),
                Q("Func<T, R> returns...", {"R", "T", "void", "bool"}, "Func returns the last type parameter."),
            }), "Write a lambda that doubles x.", "x => ...",
            {"x => x * 2", "x => 2 * x"}, "x => x * 2"),

            WithCode(M("Events", {
                Q("Which keyword declares a publisher-side event?", {"event", "delegate", "subscribe", "publish"}, "The event keyword."),
                Q("Which delegate is the standard event handler type?", {"EventHandler", "Action", "Func", "Callback"}, "EventHandler / EventHandler<T>."),
                Q("How does a subscriber attach a handler?", {"+= handler", "= handler", "-> handler", "attach(handler)"}, "obj.Event += handler;"),
            }), "Attach the handler OnClick to the event e.Click.", "e.Click ... handler;",
            {"e.Click += OnClick;", "e.Click += OnClick"}, "e.Click += OnClick;"),
        },
        WithCode(M("Topic Test", {
            Q("What does -= do on an event?", {"Detaches a handler", "Attaches a handler", "Fires the event", "Deletes the event"}, "Removes a subscriber."),
            Q("Why use events instead of public delegate fields?", {"Encapsulation - subscribers cannot fire the event", "They are faster", "They save memory", "They compile faster"}, "Only the declaring class can raise the event."),
            Q("Which delegate takes no args and returns void?", {"Action", "Func<void>", "Predicate", "EventHandler"}, "Action."),
            }), "Fire the event Click with sender this and EventArgs.Empty.", "Click?....(...)",
            { "Click?.Invoke(this, EventArgs.Empty);", "Click?.Invoke(this, EventArgs.Empty)" }, "Click?.Invoke(this, EventArgs.Empty)")
            ));

    // ---------------------------------------------------------------- 8: M8
    T.push_back(MakeTopic("LINQ", "LINQ",
        "Query syntax, method syntax, and deferred vs. immediate execution.",
        Color{ 240,80,80,255 }, 280,
        {
            WithCode(M("Query & Method Syntax", {
                Q("Which LINQ method filters?", {"Where", "Select", "Filter", "Find"}, "Where filters."),
                Q("Which LINQ method transforms?", {"Select", "Where", "Map", "Apply"}, "Select projects."),
                Q("Which begins query syntax?", {"from x in source", "select from x", "query x in source", "foreach x in source"}, "Query expressions start with from."),
            }), "Filter sequence xs to positive numbers using method syntax.", "xs.Where(...)",
            {"xs.Where(x => x > 0)", "xs.Where(x => x > 0);"}, "xs.Where(x => x > 0)"),

            WithCode(M("Deferred Execution", {
                Q("What does deferred execution mean?", {"The query runs when enumerated", "It runs immediately", "It never runs", "It runs on a background thread"}, "LINQ is lazy."),
                Q("Which operator forces immediate execution?", {"ToList()", "Where()", "Select()", "AsQueryable()"}, "ToList materializes the results."),
                Q("What does .Count() do?", {"Iterates and counts (immediate)", "Returns 0 always", "Defers", "Throws"}, "Count() is a terminal operator."),
            }), "Force xs.Where(...) into a list.", "xs.Where(...).???",
            {"xs.Where(x => x > 0).ToList()", "xs.Where(x=>x>0).ToList()"}, "xs.Where(...).ToList()"),
        },
        WithCode(M("Topic Test", {
            Q("Which is an immediate execution operator?", {"ToList", "Select", "Where", "OrderBy"}, "ToList materializes."),
            Q("What does GroupBy do?", {"Groups elements by a key selector", "Sorts by key", "Filters by key", "Joins two sequences"}, "GroupBy returns groups of matching elements."),
            Q("What does First() throw when the sequence is empty?", {"InvalidOperationException", "NullReferenceException", "ArgumentException", "Nothing"}, "First() throws; FirstOrDefault returns default."),
            }), "Take the first 3 elements of xs using LINQ.", "xs.Take(...)",
            { "xs.Take(3)", "xs.Take(3);" }, "xs.Take(3)")
            ));

    // ---------------------------------------------------------------- 9: M9
    T.push_back(MakeTopic("Asynchronous Programming", "Async",
        "Task/Task<T>, async/await, ValueTask, CancellationToken and async streams.",
        Color{ 200,120,80,255 }, 330,
        {
            WithCode(M("Tasks & async/await", {
                Q("What does an async method return?", {"Task or Task<T>", "void always", "int", "Thread"}, "async returns Task / Task<T>."),
                Q("Which keyword awaits a Task?", {"await", "wait", "yield", "async"}, "await."),
                Q("Which suffix do async methods conventionally get?", {"Async", "_async", "Task", "_t"}, "The Async suffix."),
            }), "Await a Task-returning method called SomeWorkAsync().", "await ...;",
            {"await SomeWorkAsync();"}, "await SomeWorkAsync();"),

            WithCode(M("Advanced Async", {
                Q("What does CancellationToken allow?", {"Cooperative cancellation", "Forceful thread kill", "Memory release", "Interop with COM"}, "The token polls for cancellation."),
                Q("When should you prefer ValueTask over Task?", {"When the operation often completes synchronously", "Always", "Never", "Only for void"}, "ValueTask avoids allocation on the sync path."),
                Q("What does IAsyncEnumerable<T> stream?", {"Asynchronous sequences via await foreach", "Synchronous collections", "Threads", "Tasks"}, "await foreach consumes it."),
            }), "Await a Task<T> method GetDataAsync and store the result in data.", "var data = await ...;",
            {"var data = await GetDataAsync();", "var data = await GetDataAsync();"}, "var data = await GetDataAsync();"),
        },
        WithCode(M("Topic Test", {
            Q("What does async always return?", {"A Task-like type", "void", "undefined", "A callback"}, "async wraps in a Task."),
            Q("Why avoid async void outside event handlers?", {"Exceptions are unobservable", "It is slower", "It does not compile", "It blocks the thread"}, "async void exceptions cannot be awaited or caught by the caller."),
            Q("How do you handle errors with async/await?", {"try/catch around the awaits", "Only .ContinueWith", "Task.Fault()", "You cannot"}, "try/catch works the same as sync code."),
            }), "Await Task.Delay(1000) and store nothing (expression).", "await Task....",
            { "await Task.Delay(1000)", "await Task.Delay(1000);" }, "await Task.Delay(1000);")
            ));

    // ---------------------------------------------------------------- 10: M10
    T.push_back(MakeTopic("Concurrency & Threading", "Threads",
        "ThreadPool, lock/Monitor, and thread-safe collections.",
        Color{ 200,80,80,255 }, 380,
        {
            WithCode(M("Threading Basics", {
                Q("Where do short-lived CPU tasks run?", {"On the ThreadPool", "On the main thread", "In async methods only", "In finalizers"}, "ThreadPool reuses worker threads."),
                Q("Which class represents a thread?", {"Thread", "Task", "Runnable", "Worker"}, "System.Threading.Thread."),
                Q("What does Task.Run do?", {"Queues work on the ThreadPool", "Blocks the thread", "Creates a new process", "Nothing"}, "Task.Run(work) queues it."),
            }), "Start a Task that runs the method work() on the pool.", "Task.Run(...)",
            {"Task.Run(work)", "Task.Run(() => work())"}, "Task.Run(work)"),

            WithCode(M("Synchronization", {
                Q("Which keyword takes a lock?", {"lock", "synchronized", "mutex", "Monitor"}, "lock (sync) { ... }."),
                Q("Which type provides a fine-grained locking primitive?", {"Monitor", "Lock", "Semaphore", "Event"}, "Monitor.Enter / Exit."),
                Q("Which collection is thread-safe?", {"ConcurrentDictionary<TKey, TValue>", "Dictionary<TKey, TValue>", "List<T>", "Queue<T>"}, "Concurrent* collections are thread-safe."),
            }), "Take a lock on object _sync around the code block (opening).", "lock (...) {",
            {"lock (_sync) {", "lock(_sync){"}, "lock (_sync) { ... }"),
        },
        WithCode(M("Topic Test", {
            Q("What does Interlocked.Increment(ref x) do?", {"Atomically increments x", "Locks x", "Copies x", "Resets x"}, "Interlocked offers atomic ops."),
            Q("What is a race condition?", {"Threads interfere over shared mutable state", "Fast loops", "Any exception", "A design pattern"}, "Unsynchronized access to shared state."),
            Q("Which type signals between threads with an event?", {"ManualResetEventSlim", "Thread", "Task", "Interlocked"}, "ManualResetEventSlim is a synchronization primitive."),
            }), "Declare a private readonly object field named _sync.", "private readonly object _sync = ...;",
            { "private readonly object _sync = new object();", "private readonly object _sync = new();" }, "private readonly object _sync = new object();")
            ));

    // ---------------------------------------------------------------- 11: M11
    T.push_back(MakeTopic("Exception Handling & Disposal", "Errors",
        "try/catch/finally, exception filters, IDisposable and using.",
        Color{ 180,60,60,255 }, 430,
        {
            WithCode(M("Exceptions & Filters", {
                Q("Which block always runs?", {"finally", "catch", "try", "when"}, "finally."),
                Q("Which keyword adds a filter to catch?", {"when", "filter", "if", "where"}, "catch (Exception ex) when (...)."),
                Q("Are C# exceptions checked at compile time?", {"No", "Yes", "Only in async", "Only in methods"}, "C# exceptions are unchecked."),
            }), "Write the opening of a try block (include opening brace).", "try ...",
            {"try {", "try{"}, "try { ... } catch ..."),

            WithCode(M("IDisposable & using", {
                Q("Which interface signals disposal?", {"IDisposable", "ICloseable", "using", "IResource"}, "IDisposable."),
                Q("What does using do for an IDisposable?", {"Calls Dispose automatically at scope end", "Calls Close", "Does nothing", "Makes it read-only"}, "using guarantees Dispose."),
                Q("What is a using declaration (C# 8+)?", {"using var x = new ...; that disposes at method end", "Same as using statement", "A namespace import", "A static import"}, "The variable disposes at end of scope."),
            }), "Wrap a block that uses IDisposable r with a using statement (opening).", "using (...) {",
            {"using (r) {", "using (var x = r) {"}, "using (r) { ... }"),
        },
        WithCode(M("Topic Test", {
            Q("What does a custom exception usually subclass?", {"Exception", "Error", "BaseException", "object"}, "Subclass Exception."),
            Q("What does a finally block do?", {"Runs after try/catch regardless of outcome", "Only on success", "Only on failure", "Skips cleanup"}, "finally is guaranteed."),
            Q("Why prefer using declarations over try/finally?", {"Conciseness and correctness", "Performance", "Async support", "Interop"}, "C# 8+ using var simplifies disposal."),
            }), "Write a using declaration that owns a new StreamReader for 'f.txt'.", "using var r = new ...;",
            { "using var r = new StreamReader(\"f.txt\");", "using var r = new StreamReader(\"f.txt\")" }, "using var r = new StreamReader(\"f.txt\");")
            ));

    // ---------------------------------------------------------------- 12: M12
    T.push_back(MakeTopic("Advanced Memory & Low-Level C#", "Advanced",
        "GC generations, Span<T>, stackalloc and unsafe code.",
        Color{ 160,40,40,255 }, 480,
        {
            WithCode(M("Garbage Collection & Span", {
                Q("Which GC generation holds short-lived objects?", {"Gen 0", "Gen 1", "Gen 2", "LOH"}, "Gen 0 is the youngest."),
                Q("What is Span<T>?", {"A stack-only, allocation-free view over memory", "A list", "An array", "A class"}, "Span<T> avoids allocations."),
                Q("What is the Large Object Heap threshold?", {"~85 KB", "1 MB", "10 KB", "1 GB"}, "Objects over ~85 KB go to LOH."),
            }), "Create a Span<int> over an array named arr.", "Span<int> s = ...",
            {"Span<int> s = arr;", "Span<int> s = new Span<int>(arr);"}, "Span<int> s = arr;"),

            WithCode(M("stackalloc & unsafe", {
                Q("What does stackalloc do?", {"Allocates on the stack", "Allocates on the heap", "Clones an array", "Frees memory"}, "Stack-only allocation."),
                Q("What does the fixed keyword do?", {"Pins a managed object during a pointer scope", "Freezes a variable", "Locks a thread", "Finalizes"}, "fixed pins objects."),
                Q("Which permission does pointer code require?", {"unsafe context", "admin", "root", "elevated"}, "unsafe { ... }."),
            }), "Declare an unsafe method header for void Run().", "unsafe void Run()",
            {"unsafe void Run()", "public unsafe void Run()"}, "public unsafe void Run()"),
        },
        WithCode(M("Topic Test", {
            Q("Does Span<T> box?", {"No, it is stack-only", "Yes", "Only for reference types", "Only in methods"}, "Span is a ref struct."),
            Q("What is the advantage of stackalloc over new?", {"No heap allocation or GC pressure", "Larger allocation", "Safer", "Persistent"}, "Stack allocation avoids GC."),
            Q("Why is unsafe code faster?", {"Bypasses bounds checks and GC moves", "It compiles faster", "It uses less memory", "It runs on other threads"}, "Unsafe bypasses safety checks."),
            }), "Write the opening of an unsafe block.", "unsafe { ... }",
            { "unsafe {", "unsafe{" }, "unsafe { ... }")
            ));

    return T;
}// ---------------------------------------------------------------------------
// JavaScript course. Same shape as the rest: 8 topics, each with 2 modules +
// 1 test; each module = 3 MC + 1 small code; each test = 3 MC + 1 small code.
// Covers the modern language (ES2015+) end-to-end:
//   1 Basics      - let/const, types, console.log
//   2 Flow        - if/else, switch, for, while, for...of / for...in
//   3 Functions   - declarations, arrows, closures, default & rest params
//   4 Data        - arrays (map/filter/reduce), objects, destructuring
//   5 DOM         - querySelector, addEventListener, classList
//   6 Classes     - class, constructor, extends, super, prototype
//   7 Async       - callbacks -> promises -> async/await, fetch
//   8 Modern      - modules, template literals, spread/rest, optional chain
// ---------------------------------------------------------------------------
inline std::vector<Topic> BuildJavaScriptTopics() {
    std::vector<Topic> T;

    // ---------------------------------------------------------------- 1
    T.push_back(MakeTopic("JS Basics", "Basics", "let, const, primitive types and console output.",
        Color{ 230,180,0,255 }, 0,
        {
            WithCode(M("Hello, JavaScript", {
                Q("Which function logs to the console?", {"console.log()", "print()", "echo()", "System.out.println()"}, "console.log is the browser/Node log."),
                Q("Which file extension do JS modules use?", {".js", ".javascript", ".jsc", ".jsx"}, ".js (and .mjs / .cjs for modules)."),
                Q("How do you write a single-line comment?", {"// comment", "# comment", "<!-- comment -->", "/* comment"}, "Double slash."),
            }), "Print Hello using console.log.", "console.log(...)",
            {"console.log(\"Hello\")", "console.log('Hello')"}, "console.log(\"Hello\")"),

            WithCode(M("let, const & Types", {
                Q("Which declares a block-scoped variable?", {"let", "var", "def", "dim"}, "let is block-scoped."),
                Q("Which declares a value you cannot reassign?", {"const", "let", "var", "final"}, "const prevents reassignment."),
                Q("What does typeof 'hi' return?", {"'string'", "'text'", "'char'", "'literal'"}, "typeof returns 'string'."),
            }), "Declare a constant named age with the value 18.", "const name = value;",
            {"const age = 18;", "const age = 18"}, "const name = value;"),
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
// Courses (main menu). Java is fully playable. The others are outlines only:
// topic names/blurbs show on the map as "Content coming soon". To add content,
// write a BuildCppTopics() etc. using MakeTopic/M/Q like BuildTopics() above and
// use it in BuildCourses() instead of OutlineTopics(...).
// ---------------------------------------------------------------------------
struct Course {
    std::string name, tagline;
    Color color{};
    std::vector<Topic> topics;

    bool Ready() const { for (auto& t : topics) if (!t.modules.empty()) return true; return false; }
    int  TotalNodes() const {                       // modules + one test per playable topic
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