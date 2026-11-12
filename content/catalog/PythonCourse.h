#pragma once
#include "../CourseModel.h"

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
