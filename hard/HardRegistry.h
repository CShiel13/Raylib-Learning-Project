#pragma once
#include <map>
#include <string>
#include <utility>
#include "HardTypes.h"

inline std::map<std::string, HardProblem>& HardRegistry() { static std::map<std::string, HardProblem> m; return m; }
inline std::string HardKey(const std::string& course, int topic, int module) { return course + ":" + std::to_string(topic) + ":" + std::to_string(module); }
inline void AddHard(const std::string& course, int topic, int module, HardProblem p) { HardRegistry()[HardKey(course, topic, module)] = std::move(p); }
inline const HardProblem* FindHard(const std::string& course, int topic, int module) {
    auto& m = HardRegistry();
    auto it = m.find(HardKey(course, topic, module));
    return it == m.end() ? nullptr : &it->second;
}

// Sample problems. Add yours the same way: AddHard("Java", topicIndex, moduleIndex, {...});
inline void BuildHard() {
    static bool done = false; if (done) return; done = true;
    {
        HardProblem p;
        p.title = "Sum of Two"; p.difficulty = "Easy";
        p.statement = "Complete the method sum so that it returns the sum of its two int parameters.";
        p.examples = { {"sum(2, 3)", "5", ""}, {"sum(-4, 10)", "6", "Negative numbers work too."} };
        p.constraints = { "-1000 <= a, b <= 1000" };
        p.starter = "public class Solution {\n    public static int sum(int a, int b) {\n        // your code here\n    }\n}\n";
        p.solutions = { "public class Solution { public static int sum(int a, int b) { return a + b; } }",
                       "public class Solution { public static int sum(int a, int b) { return b + a; } }" };
        p.mustContain = { "return" };
        p.hints = { "Methods hand back a value with the return keyword.", "Combine a and b with the + operator." };
        AddHard("Java", 0, 0, p);
    }
    {
        HardProblem p;
        p.title = "Even or Odd"; p.difficulty = "Easy";
        p.statement = "Write parity so it returns the String \"even\" when n is even and \"odd\" when it is odd.";
        p.examples = { {"parity(4)", "\"even\"", ""}, {"parity(7)", "\"odd\"", ""}, {"parity(0)", "\"even\"", "Zero is even."} };
        p.constraints = { "0 <= n <= 1,000,000" };
        p.starter = "public class Solution {\n    public static String parity(int n) {\n        // your code here\n    }\n}\n";
        p.solutions = { "public class Solution { public static String parity(int n) { return n % 2 == 0 ? \"even\" : \"odd\"; } }",
                       "public class Solution { public static String parity(int n) { if (n % 2 == 0) { return \"even\"; } return \"odd\"; } }",
                       "public class Solution { public static String parity(int n) { if (n % 2 == 0) { return \"even\"; } else { return \"odd\"; } } }" };
        p.mustContain = { "%", "return" };
        p.hints = { "The % operator gives a remainder. What is the remainder of an even number divided by 2?" };
        AddHard("Java", 0, 1, p);
    }
    {
        HardProblem p;
        p.title = "Fizz or Buzz"; p.difficulty = "Medium";
        p.statement = "Write fizz so it returns \"FizzBuzz\" if n is divisible by both 3 and 5, \"Fizz\" if divisible by 3 only, "
            "\"Buzz\" if divisible by 5 only, otherwise the number as a String.";
        p.examples = { {"fizz(15)", "\"FizzBuzz\"", "Divisible by 3 and 5."}, {"fizz(9)", "\"Fizz\"", ""}, {"fizz(10)", "\"Buzz\"", ""}, {"fizz(7)", "\"7\"", ""} };
        p.constraints = { "1 <= n <= 100,000" };
        p.starter = "public class Solution {\n    public static String fizz(int n) {\n        // your code here\n    }\n}\n";
        p.solutions = { "public class Solution { public static String fizz(int n) { if (n % 15 == 0) { return \"FizzBuzz\"; } if (n % 3 == 0) { return \"Fizz\"; } if (n % 5 == 0) { return \"Buzz\"; } return String.valueOf(n); } }",
                       "public class Solution { public static String fizz(int n) { if (n % 3 == 0 && n % 5 == 0) { return \"FizzBuzz\"; } if (n % 3 == 0) { return \"Fizz\"; } if (n % 5 == 0) { return \"Buzz\"; } return String.valueOf(n); } }" };
        p.mustContain = { "FizzBuzz", "Fizz", "Buzz", "%" };
        p.hints = { "Check the combined case (3 and 5) first, otherwise the single cases would catch it.", "String.valueOf(n) turns an int into a String." };
        p.xp = 60; p.coins = 25;
        AddHard("Java", 1, 0, p);
    }
}
