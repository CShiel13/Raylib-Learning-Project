#pragma once
// Full source-based exercise wording and clear grader conventions.
// Kept outside LabBank so existing lab IDs, starter code, tests and saves stay stable.
#include <array>
#include <string>
namespace questlabs {
struct LabFullQuestion {
    const char* source;
    const char* question;
    const char* example;
    const char* checkNotes;
};
// The first 24 entries reproduce the exercise comments provided with the lab code.
// The remaining tasks were supplied as prose instructions; their descriptions are
// presented in comment form for consistency inside the editor.
inline const char* TaskComment(int id) {
    static const std::array<const char*,37> comments = {{
        R"JQLABCMT(// Q1: Check if an inputted year is a Leap Year.
// A leap year is divisible by 4, but not by 100 unless also divisible by 400.)JQLABCMT", // lab 1
        R"JQLABCMT(// Q2 (helper): Reverse the digits of a number and return the reversed integer.
// E.g., 123 -> 321.)JQLABCMT", // lab 2
        R"JQLABCMT(// Q2: Check Whether a Number is Palindrome or Not.
// A palindrome reads the same forwards and backwards (e.g., 121).)JQLABCMT", // lab 3
        R"JQLABCMT(// Q3: Check Whether a Number is Prime or Not.
// A prime number is only divisible by itself and 1.)JQLABCMT", // lab 4
        R"JQLABCMT(// Q4: Convert Binary Number to Decimal without using the shift operator.
// E.g., 110 -> 6.)JQLABCMT", // lab 5
        R"JQLABCMT(// Q6: Output a right-angled triangle of 'A's using 2 nested for loops:
// A
// AA
// AAA
// AAAA)JQLABCMT", // lab 6
        R"JQLABCMT(// Q7: Output an isosceles (diamond-like) triangle of 'A's using 2 for loops
// and at most 1 if/else:
// A
// AA
// AAA
// AAAA
// AAA
// AA
// A)JQLABCMT", // lab 7
        R"JQLABCMT(// Q19 (EXTRA): Output the same diamond pattern as Q7 using only 2 for loops
// and no conditional statements.)JQLABCMT", // lab 8
        R"JQLABCMT(// Q8: Find an element in an array and print the index where it was found,
// or -1 if not found.)JQLABCMT", // lab 9
        R"JQLABCMT(// Q9: Find and return the second largest element in an array of positive
// integers; returns -1 if it can't find one.)JQLABCMT", // lab 10
        R"JQLABCMT(// Q10: Copy all elements from arr1 to arr2. Both arrays are the same size.)JQLABCMT", // lab 11
        R"JQLABCMT(// Q11: Insert an element at a specified index in the array, shifting elements
// to the right. 'count' tracks the number of elements and cannot exceed 'size'.
// Returns true if an element was inserted, otherwise false.)JQLABCMT", // lab 12
        R"JQLABCMT(// Q12: Delete an element at a specified position, shifting higher elements
// to the left by one. 'count' tracks the number of elements and cannot exceed
// 'size'. Returns true if an element was deleted, otherwise false.)JQLABCMT", // lab 13
        R"JQLABCMT(// Q13: Count the frequency of a given element in an array.)JQLABCMT", // lab 14
        R"JQLABCMT(// Q14: Count the total number of duplicate elements in an array.)JQLABCMT", // lab 15
        R"JQLABCMT(// Q15: Reverse an array in place using only 1 array.)JQLABCMT", // lab 16
        R"JQLABCMT(// Q16: Left rotate an array by one position, moving the first element to the end.)JQLABCMT", // lab 17
        R"JQLABCMT(// Q17: Given a flight length and an array of movie lengths, return true if
// there are exactly two movies whose runtimes sum to the flight length.
// Users watch exactly two movies and cannot watch the same movie twice.)JQLABCMT", // lab 18
        R"JQLABCMT(// Q18: Count the number of separate words in an array of chars, where a word
// is one or more characters in ranges A..Z or a..z separated by spaces.)JQLABCMT", // lab 19
        R"JQLABCMT(// Q19: PrintArray
// Prints the 4x6 array in rows and columns.
// This is the ONLY function allowed to use printf.)JQLABCMT", // lab 20
        R"JQLABCMT(// Q20: ReplaceWithMinusOne
// Scans the array from position (0,0), moving left-to-right across each row.
// At each position, checks for a HORIZONTAL line of 3+ identical items first,
// and if none found, checks for a VERTICAL line of 3+ identical items.
// The first cluster found is scored by summing its values (1-4), then the
// cluster is replaced with -1's. Returns the score, or 0 if no cluster found.
// 0's and -2's are never part of a cluster.)JQLABCMT", // lab 21
        R"JQLABCMT(// Q21:FallDownAndReplace
// Removes the -1's by making all items above them "fall down" into the empty
// space. New items added at the top of the array are given the value -2, and
// these will not be part of the next cluster.
// Works column by column.)JQLABCMT", // lab 22
        R"JQLABCMT(// Q21: FindAllVariables
// Scans the C code string and fills 'variables' with a comma-separated list
// of every variable name declared as type "int" or "char".
// Only one variable is declared per line, and strings are 0-terminated.
// E.g., "int var1=10;" adds "var1" to the list.)JQLABCMT", // lab 23
        R"JQLABCMT(// Q22: FindAllVariablesInScope
// Fills 'variables' with a comma-separated list of variables that are in
// scope AFTER the given lineNumber. The code has no "for" loops, and the
// only function declaration is "int main()". Only one variable is declared
// per line, and only "int" / "char" variables are considered.
// Scope is tracked with a brace depth counter. A variable declared at depth
// D is in scope after its declaration line as long as the current depth is
// >= D. A declaration at the same depth as the target line counts; a
// declaration at a deeper depth (inside a block that closes before/at the
// target line) does not.)JQLABCMT", // lab 24
        R"JQLABCMT(// 1. swap_ints
// Description: Swap the values of two integers using pointers.)JQLABCMT", // lab 25
        R"JQLABCMT(// 2. swap_chars
// Description: Swap the values of two characters using pointers.)JQLABCMT", // lab 26
        R"JQLABCMT(// 3. find_max
// Description: Find the maximum value in an integer array using pointers.)JQLABCMT", // lab 27
        R"JQLABCMT(// 4. reverse_array
// Description: Reverse the contents of an integer array in-place using pointers.)JQLABCMT", // lab 28
        R"JQLABCMT(// 5. strlen_ptr
// Description: Implement a function that calculates the length of a string
// (null-terminated character array) using pointers.)JQLABCMT", // lab 29
        R"JQLABCMT(// 6. count_vowels
// Description: Count the number of vowels in a string using pointers.)JQLABCMT", // lab 30
        R"JQLABCMT(// 7. copy_string
// Description: Copy the contents of one string to another using pointers
// (you may assume the destination has enough space).)JQLABCMT", // lab 31
        R"JQLABCMT(// 8. concat_strings
// Description: Concatenate two strings using pointers
// (you may assume the destination has enough space).)JQLABCMT", // lab 32
        R"JQLABCMT(// 9. sum_array
// Description: Compute the sum of all elements in an integer array using pointers.)JQLABCMT", // lab 33
        R"JQLABCMT(// 10. compare_strings
// Description: Compare two strings lexicographically using pointers.
// Return 0 if they are equal, a negative value if the first string is
// lexicographically smaller, and a positive value if the first is larger.)JQLABCMT", // lab 34
        R"JQLABCMT(// Coding Exam 2, Part 2
// DoublyLinkedNode has both a prevPointer and nextPointer.
// Implement InsertAtBeginning.)JQLABCMT", // lab 35
        R"JQLABCMT(// Coding Exam 2, Part 2
// DoublyLinkedNode has both a prevPointer and nextPointer.
// Implement InsertAtEnd.)JQLABCMT", // lab 36
        R"JQLABCMT(// Coding Exam 2, Part 2
// DoublyLinkedNode has both a prevPointer and nextPointer.
// Implement InsertAtPosition.)JQLABCMT", // lab 37
    }};
    return comments.at(static_cast<size_t>(id));
}
inline const char* TaskCommentLabel(int id) {
    return id < 24 ? "ORIGINAL ASSIGNMENT COMMENTS" : "EXERCISE DESCRIPTION";
}
inline const LabFullQuestion& FullQuestion(int id) {
    static const std::array<LabFullQuestion,37> items = {{
        { R"JQLAB(Original exercises - Q1)JQLAB", R"JQLAB(Q1: Check if an inputted year is a Leap Year.
A leap year is divisible by 4, but not by 100 unless also divisible by 400.

Implement the function so it returns true for leap years and false otherwise.)JQLAB", R"JQLAB(isLeapYear(1700) -> false
isLeapYear(1600) -> true
isLeapYear(2016) -> true)JQLAB", R"JQLAB(The output is a boolean, not printed text. Consider century years and ordinary years.)JQLAB" }, // 0
        { R"JQLAB(Original exercises - Q2 helper)JQLAB", R"JQLAB(Q2 (helper): Reverse the digits of a number and return the reversed integer.
E.g., 123 -> 321.

Implement Reversed(int testNumber) by taking digits from right to left and forming a new integer.)JQLAB", R"JQLAB(Reversed(123) -> 321
Reversed(1) -> 1
Reversed(1234) -> 4321)JQLAB", R"JQLAB(Lab inputs are nonnegative, fit in int, and have reversals that fit in int. Any leading zeros of the reversed result disappear.)JQLAB" }, // 1
        { R"JQLAB(Original exercises - Q2)JQLAB", R"JQLAB(Q2: Check Whether a Number is Palindrome or Not.
A palindrome reads the same forwards and backwards (e.g., 121).

Implement isAPalindrome(int testNumber), returning true for palindromes and false otherwise. You can reuse the Reversed helper from the preceding question.)JQLAB", R"JQLAB(isAPalindrome(1) -> true
isAPalindrome(121) -> true
isAPalindrome(1213) -> false)JQLAB", R"JQLAB(Lab inputs are nonnegative. Your submission needs the Reversed helper implementation as well if you call it.)JQLAB" }, // 2
        { R"JQLAB(Original exercises - Q3)JQLAB", R"JQLAB(Q3: Check Whether a Number is Prime or Not.
A prime number is only divisible by itself and 1.

Implement isAPrimeNumber(int numbertoTest) and return a boolean instead of printing the answer.)JQLAB", R"JQLAB(isAPrimeNumber(3) -> true
isAPrimeNumber(2147483647) -> true
isAPrimeNumber(4) -> false)JQLAB", R"JQLAB(Values less than 2 are not prime. The input may be a large positive int, so check divisors efficiently.)JQLAB" }, // 3
        { R"JQLAB(Original exercises - Q4)JQLAB", R"JQLAB(Q4: Convert Binary Number to Decimal without using the shift operator.
E.g., 110 -> 6.

The input is an integer whose decimal digits represent binary bits (0 or 1), not a text string. Convert those bits to their base-10 value without << or >>.)JQLAB", R"JQLAB(convertBinarytoDecimal(110) -> 6
convertBinarytoDecimal(111) -> 7
convertBinarytoDecimal(10001) -> 17)JQLAB", R"JQLAB(Inputs contain only binary digits 0 and 1 and are nonnegative; 0 represents zero.)JQLAB" }, // 4
        { R"JQLAB(Original exercises - Q6)JQLAB", R"JQLAB(Q6: Output a right-angled triangle of 'A's using 2 nested for loops.

Print the exact pattern below, one row per line. The outer loop controls the number of rows and the inner loop controls the number of A characters printed.)JQLAB", R"JQLAB(A
AA
AAA
AAAA)JQLAB", R"JQLAB(Printed output is checked, including row lengths and the order of rows. The assignment requires two nested for loops.)JQLAB" }, // 5
        { R"JQLAB(Original exercises - Q7)JQLAB", R"JQLAB(Q7: Output an isosceles (diamond-like) triangle of 'A's using 2 for loops and at most 1 if/else.

Grow from one A to four A's, then shrink back to one A. Print the entire seven-line pattern in order.)JQLAB", R"JQLAB(A
AA
AAA
AAAA
AAA
AA
A)JQLAB", R"JQLAB(The required output is seven lines. The assignment restricts your solution to two for loops and at most one if/else.)JQLAB" }, // 6
        { R"JQLAB(Original exercises - Q19 EXTRA)JQLAB", R"JQLAB(Q19 (EXTRA): Output the same diamond pattern as Q7 using only 2 for loops and no conditional statements.

It must still contain the same seven rows and must not use an if, else if, or else statement.)JQLAB", R"JQLAB(A
AA
AAA
AAAA
AAA
AA
A)JQLAB", R"JQLAB(The output checker verifies the pattern; you are also expected to meet the original structural restriction (two for loops, no conditionals).)JQLAB" }, // 7
        { R"JQLAB(Original exercises - Q8)JQLAB", R"JQLAB(Q8: Find an element in an array and print the index where it was found, or -1 if not found.

Function prototype: int find(int size, int arr[], int toFind). The function returns the index so the caller can print it. Search the array from index 0.)JQLAB", R"JQLAB(arr = {4, 9, 4}, toFind = 9 -> 1
arr = {4, 9, 4}, toFind = 7 -> -1)JQLAB", R"JQLAB(The checker expects the first matching zero-based index if a value occurs more than once.)JQLAB" }, // 8
        { R"JQLAB(Original exercises - Q9)JQLAB", R"JQLAB(Q9: Find and return the second largest element in an array of positive integers; returns -1 if it can't find one.

Use the given size to inspect the array and return a single integer.)JQLAB", R"JQLAB(arr = {8, 3, 8, 5} -> 5
arr = {5, 5} -> -1
arr = {1, 2, 3} -> 2)JQLAB", R"JQLAB(The original question does not define repeated maximum values. This lab's tests use the second DISTINCT largest value; fewer than two distinct values -> -1.)JQLAB" }, // 9
        { R"JQLAB(Original exercises - Q10)JQLAB", R"JQLAB(Q10: Copy all elements from arr1 to arr2. Both arrays are the same size.

Implement copyArraytoArray(int size, int arr1[], int arr2[]). Write each source item to the corresponding destination position.)JQLAB", R"JQLAB(arr1 = {1, 2, 3}; arr2 = {0, 0, 0}
After copying, arr2 = {1, 2, 3})JQLAB", R"JQLAB(Copy the first size elements. Leave arr1 unchanged. There is no return value.)JQLAB" }, // 10
        { R"JQLAB(Original exercises - Q11)JQLAB", R"JQLAB(Q11: Insert an element at a specified index in the array, shifting elements to the right.
'count' tracks the number of elements and cannot exceed 'size'.
Returns true if an element was inserted, otherwise false.

Use the provided function signature with size and count passed by reference.)JQLAB", R"JQLAB(size = 5; count = 3; arr = {1,2,3,0,0}
Insert 9 at index 1 -> {1,9,2,3,0}; count = 4
A full array cannot accept another element.)JQLAB", R"JQLAB(Lab interpretation: size is array capacity and is unchanged. Valid insertion indices are 0 through count inclusive. On success increment count; on failure leave inputs unchanged.)JQLAB" }, // 11
        { R"JQLAB(Original exercises - Q12)JQLAB", R"JQLAB(Q12: Delete an element at a specified position, shifting higher elements to the left by one.
'count' tracks the number of elements and cannot exceed 'size'.
Returns true if an element was deleted, otherwise false.

Implement deleteElement using the given index.)JQLAB", R"JQLAB(size = 5; count = 3; arr = {4,7,9,0,0}
Delete index 1 -> {4,9,...}; count = 2)JQLAB", R"JQLAB(Lab interpretation: size is array capacity and does not change. Valid deletion indices are 0 to count - 1. Invalid requests return false without changing count.)JQLAB" }, // 12
        { R"JQLAB(Original exercises - Q13)JQLAB", R"JQLAB(Q13: Count the frequency of a given element in an array.

Implement frequencyCount(int size, int arr[], int value). Count every matching entry among the first size elements, including repeated appearances.)JQLAB", R"JQLAB(arr = {2, 3, 2, 2}; value = 2 -> 3
arr = {1, 3}; value = 8 -> 0)JQLAB", R"JQLAB(Return the count rather than printing it. An empty array has frequency zero.)JQLAB" }, // 13
        { R"JQLAB(Original exercises - Q14)JQLAB", R"JQLAB(Q14: Count the total number of duplicate elements in an array.

Implement countDuplicates(int size, int arr[]) and return a single number. The input should not be modified.)JQLAB", R"JQLAB(arr = {3, 3, 3, 4, 4} -> 3
arr = {1, 2, 3} -> 0)JQLAB", R"JQLAB(The question does not specify the counting convention. The lab counts duplicate OCCURRENCES after the first instance of each value: {3,3,3} has 2 duplicates.)JQLAB" }, // 14
        { R"JQLAB(Original exercises - Q15)JQLAB", R"JQLAB(Q15: Reverse an array in place using only 1 array.

Implement reverse(int size, int arr[]) so that all elements end up in reverse order without creating a second array.)JQLAB", R"JQLAB(arr = {1, 2, 3, 4} -> {4, 3, 2, 1}
arr = {7} -> {7})JQLAB", R"JQLAB(The original array must be modified in place. No return value is needed.)JQLAB" }, // 15
        { R"JQLAB(Original exercises - Q16)JQLAB", R"JQLAB(Q16: Left rotate an array by one position, moving the first element to the end.

Implement rotateLeft(int size, int arr[]). Preserve the relative order of the remaining elements while modifying the original array.)JQLAB", R"JQLAB(arr = {1, 2, 3} -> {2, 3, 1}
arr = {8} -> {8})JQLAB", R"JQLAB(The original function signature returns int but does not describe its meaning. For this lab, return the original first element, or -1 for an empty array.)JQLAB" }, // 16
        { R"JQLAB(Original exercises - Q17)JQLAB", R"JQLAB(Q17: Given a flight length and an array of movie lengths, return true if there are exactly two movies whose runtimes sum to the flight length.
Users watch exactly two movies and cannot watch the same movie twice.

Implement twoMovies(flightLength, movieLengths, size) and return true if a suitable pair exists.)JQLAB", R"JQLAB(flightLength = 150
movieLengths = {60, 90, 120}
Result -> true (60 + 90)
Only one movie of length 75 cannot be reused twice.)JQLAB", R"JQLAB(Use two different ARRAY POSITIONS, even if their lengths happen to be equal. Return false if no valid pair exists.)JQLAB" }, // 17
        { R"JQLAB(Original exercises - Q18)JQLAB", R"JQLAB(Q18: Count the number of separate words in an array of chars, where a word is one or more characters in ranges A..Z or a..z separated by spaces.

Implement wordCounter(int size, char characters[]). Return the number of complete runs of letters.)JQLAB", R"JQLAB(characters = "Hello C++" -> 2
characters = "one two three" -> 3)JQLAB", R"JQLAB(Lab tests treat every character outside A..Z and a..z as a separator (including punctuation and digits). Only the first size characters count.)JQLAB" }, // 18
        { R"JQLAB(Original exercises - Q19 PrintArray)JQLAB", R"JQLAB(Q19: PrintArray.
Prints the 4x6 array in rows and columns.
This is the ONLY function allowed to use printf in this grid exercise.

Implement PrintArray(int array[4][6]) so all 24 values are printed in their original row/column positions.)JQLAB", R"JQLAB(Input:
1 2 3 4 1 2
0 0 1 1 2 2
3 3 3 4 4 4
2 1 0 0 2 3

Output: same four rows, six values per row.)JQLAB", R"JQLAB(Output should have a space between adjacent values and a newline after each row. No return value.)JQLAB" }, // 19
        { R"JQLAB(Original exercises - Q20)JQLAB", R"JQLAB(Q20: ReplaceWithMinusOne.
Scans the array from position (0,0), moving left-to-right across each row.
At each position, checks for a HORIZONTAL line of 3+ identical items first, and if none found, checks for a VERTICAL line of 3+ identical items.
The first cluster found is scored by summing its values (1-4), then the cluster is replaced with -1's.
Returns the score, or 0 if no cluster found.
0's and -2's are never part of a cluster.)JQLAB", R"JQLAB(Three adjacent 2s -> score 6; replace those 2s with -1.
A horizontal match at a scanned position wins over a vertical match there.)JQLAB", R"JQLAB(Lab checks cover the FIRST run in row-major scan order, horizontal before vertical. Only values 1..4 form clusters. Existing -1 cells cannot form clusters.)JQLAB" }, // 20
        { R"JQLAB(Original exercises - Q21 gravity)JQLAB", R"JQLAB(Q21: FallDownAndReplace.
Removes the -1's by making all items above them "fall down" into the empty space.
New items added at the top of the array are given the value -2, and these will not be part of the next cluster.
Works column by column.)JQLAB", R"JQLAB(Before, top to bottom: {1, -1, 3, -1}
After: {-2, -2, 1, 3})JQLAB", R"JQLAB(Each column is processed independently. Preserve the relative order of non--1 values and fill every vacated top cell with -2.)JQLAB" }, // 21
        { R"JQLAB(Original exercises - Q21 variables)JQLAB", R"JQLAB(Q21: FindAllVariables.
Scans the C code string and fills 'variables' with a comma-separated list of every variable name declared as type "int" or "char".
Only one variable is declared per line, and strings are 0-terminated.
E.g., "int var1=10;" adds "var1" to the list.)JQLAB", R"JQLAB(code = "int var1=10;\nchar initial;\n"
variables -> "var1,initial")JQLAB", R"JQLAB(Lab test input uses simple int/char declarations, one per line, without comments or complex declaration syntax. Terminate the output with a null byte.)JQLAB" }, // 22
        { R"JQLAB(Original exercises - Q22)JQLAB", R"JQLAB(Q22: FindAllVariablesInScope.
Fills 'variables' with a comma-separated list of variables that are in scope AFTER the given lineNumber.
The code has no "for" loops, and the only function declaration is "int main()". Only one variable is declared per line, and only "int" / "char" variables are considered.

Scope is tracked with a brace depth counter. A variable declared at depth D is in scope after its declaration line as long as the current depth is >= D. A declaration at the same depth as the target line counts; a declaration at a deeper depth (inside a block that closes before/at the target line) does not.)JQLAB", R"JQLAB(1  int main() {
2    int a = 1;
3    {
4      int b = 2;
5    }
6    int z = 3;

After line 4 -> "a,b"
After line 6 -> "a,z")JQLAB", R"JQLAB(This lab uses one-based line numbers, declaration order and comma-separated names. A closing brace removes names declared in that block.)JQLAB" }, // 23
        { R"JQLAB(Coding Exam 2 - Pointers 1)JQLAB", R"JQLAB(1. swap_ints
Description: Swap the values of two integers using pointers.

Prototype: void swap_ints(int *a, int *b);

The function must change the caller's values through the pointers passed in.)JQLAB", R"JQLAB(int x = 5, y = 10;
swap_ints(&x, &y);
// Now x = 10, y = 5)JQLAB", R"JQLAB(Pointer exam rule: no array indexing (arr[i]) in any of these ten functions; use pointer arithmetic or dereferencing.)JQLAB" }, // 24
        { R"JQLAB(Coding Exam 2 - Pointers 2)JQLAB", R"JQLAB(2. swap_chars
Description: Swap the values of two characters using pointers.

Prototype: void swap_chars(char *a, char *b);)JQLAB", R"JQLAB(char a = 'A', b = 'B';
swap_chars(&a, &b);
// Now a = 'B', b = 'A' )JQLAB", R"JQLAB(Pointer exam rule: use pointer dereferencing, not array indexing.)JQLAB" }, // 25
        { R"JQLAB(Coding Exam 2 - Pointers 3)JQLAB", R"JQLAB(3. find_max
Description: Find the maximum value in an integer array using pointers.

Prototype: int find_max(int *arr, int size);)JQLAB", R"JQLAB(int arr[5] = {3, 9, 2, 8, 5};
int max = find_max(arr, 5);
// max = 9)JQLAB", R"JQLAB(Pointer exam rule: no arr[i] inside the function. Lab tests assume the array is nonempty.)JQLAB" }, // 26
        { R"JQLAB(Coding Exam 2 - Pointers 4)JQLAB", R"JQLAB(4. reverse_array
Description: Reverse the contents of an integer array in-place using pointers.

Prototype: void reverse_array(int *arr, int size);)JQLAB", R"JQLAB(int arr[5] = {1, 2, 3, 4, 5};
reverse_array(arr, 5);
// arr = {5, 4, 3, 2, 1})JQLAB", R"JQLAB(Pointer exam rule: no array indexing in the function; reverse the existing array, not a separate copy.)JQLAB" }, // 27
        { R"JQLAB(Coding Exam 2 - Pointers 5)JQLAB", R"JQLAB(5. strlen_ptr
Description: Implement a function that calculates the length of a string (null-terminated character array) using pointers.

Prototype: int strlen_ptr(char *str);)JQLAB", R"JQLAB(char str[] = "Hello";
int length = strlen_ptr(str);
// length = 5)JQLAB", R"JQLAB(Pointer exam rule: no array indexing. Do not count the null terminator itself.)JQLAB" }, // 28
        { R"JQLAB(Coding Exam 2 - Pointers 6)JQLAB", R"JQLAB(6. count_vowels
Description: Count the number of vowels in a string using pointers.

Prototype: int count_vowels(char *str);)JQLAB", R"JQLAB(char str[] = "OpenAI";
int vowels = count_vowels(str);
// Correct count = 4: O, e, A, I)JQLAB", R"JQLAB(The supplied example says 3 vowels, but "OpenAI" contains four (O, e, A, I). The lab checks 4. Count A/E/I/O/U in either case; use pointers without indexing.)JQLAB" }, // 29
        { R"JQLAB(Coding Exam 2 - Pointers 7)JQLAB", R"JQLAB(7. copy_string
Description: Copy the contents of one string to another using pointers (you may assume the destination has enough space).

Prototype: void copy_string(char *dest, char *src);)JQLAB", R"JQLAB(char src[] = "Hello";
char dest[10];
copy_string(dest, src);
// dest = "Hello")JQLAB", R"JQLAB(Pointer exam rule: no array indexing; copy the terminating null character too.)JQLAB" }, // 30
        { R"JQLAB(Coding Exam 2 - Pointers 8)JQLAB", R"JQLAB(8. concat_strings
Description: Concatenate two strings using pointers (you may assume the destination has enough space).

Prototype: void concat_strings(char *dest, char *src);)JQLAB", R"JQLAB(char dest[20] = "Hello, ";
char src[] = "World!";
concat_strings(dest, src);
// dest = "Hello, World!")JQLAB", R"JQLAB(Pointer exam rule: find the null terminator in the destination before appending; no array indexing.)JQLAB" }, // 31
        { R"JQLAB(Coding Exam 2 - Pointers 9)JQLAB", R"JQLAB(9. sum_array
Description: Compute the sum of all elements in an integer array using pointers.

Prototype: int sum_array(int *arr, int size);)JQLAB", R"JQLAB(int arr[4] = {1, 2, 3, 4};
int sum = sum_array(arr, 4);
// sum = 10)JQLAB", R"JQLAB(Pointer exam rule: no array indexing; size may be zero, in which case the lab expects sum 0.)JQLAB" }, // 32
        { R"JQLAB(Coding Exam 2 - Pointers 10)JQLAB", R"JQLAB(10. compare_strings
Description: Compare two strings lexicographically using pointers. Return 0 if they are equal, a negative value if the first string is lexicographically smaller, and a positive value if the first string is larger.

Prototype: int compare_strings(char *str1, char *str2);)JQLAB", R"JQLAB(char str1[] = "apple";
char str2[] = "banana";
int cmp = compare_strings(str1, str2);
// cmp < 0 because "apple" < "banana")JQLAB", R"JQLAB(Pointer exam rule: use pointer arithmetic without array indexing. Only the sign of the result matters, not its exact magnitude.)JQLAB" }, // 33
        { R"JQLAB(Coding Exam 2 - Part 2: first)JQLAB", R"JQLAB(Part 2: Build a two-direction linked sequence using nodes with prevPointer and nextPointer.

InsertAtBeginning: add a new node at the beginning. New head should have prevPointer == nullptr; if an old head exists its prevPointer must point back to the new node.)JQLAB", R"JQLAB(Initially: empty
Insert 3 -> 3
Insert 2 -> 2 <-> 3)JQLAB", R"JQLAB(These three tasks use the supplied DoublyLinkedNode structure. Each allocated node must eventually be deleted by its owner.)JQLAB" }, // 34
        { R"JQLAB(Coding Exam 2 - Part 2: end)JQLAB", R"JQLAB(Part 2: Build a two-direction linked sequence using nodes with prevPointer and nextPointer.

InsertAtEnd: allocate a new node and append it at the end. Connect nextPointer from the previous tail and prevPointer from the new tail.)JQLAB", R"JQLAB(Initially: 2
Insert 8 at end -> 2 <-> 8)JQLAB", R"JQLAB(Empty sequences also need to work; an empty head becomes the new first and last node.)JQLAB" }, // 35
        { R"JQLAB(Coding Exam 2 - Part 2: position)JQLAB", R"JQLAB(Part 2: Build a two-direction linked sequence using nodes with prevPointer and nextPointer.

InsertAtPosition: insert a node at the requested position, preserving links in both directions. Support insertion at the beginning, in the middle and at the end.)JQLAB", R"JQLAB(Initially: 4 <-> 7
Insert 9 at position 1 -> 4 <-> 9 <-> 7)JQLAB", R"JQLAB(For this lab, positions are zero-based from 0 through current length inclusive. Return false for invalid positions and do not modify the sequence.)JQLAB" }, // 36
    }};
    return items.at(static_cast<size_t>(id));
}
} // namespace questlabs
