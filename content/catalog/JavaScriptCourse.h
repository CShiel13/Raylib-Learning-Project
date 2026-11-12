#pragma once
#include "../CourseModel.h"

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
