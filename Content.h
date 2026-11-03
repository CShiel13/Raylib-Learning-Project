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
