#pragma once
// Standalone C++ Lab curriculum: prompts, lecture-based summaries, scaffold and tests.
// Tests are compiled/executed only by the optional WASI sandbox, never as host code.
#include <string>
#include <vector>
namespace questlabs {
struct Lab {
    int id, group;
    std::string title, summary, prompt, signature, example, expectedOutput, starter;
    std::vector<std::string> hints;
    std::vector<std::string> tests;
};
struct Group { const char* title; const char* intro; };
inline const std::vector<Group>& Groups() {
    static const std::vector<Group> v = {
        {"Number logic", "From the introduction slides: conditions, remainders, integer division and loops. Predict the edge cases first."},
        {"Loop patterns", "From the loops slides: the outer loop chooses a row; the inner loop repeats characters. Trace row counts before coding."},
        {"Arrays and searches", "From the arrays and functions slides: indices begin at zero; bounds and order of updates matter."},
        {"Grid and scope", "From the memory slides: two-dimensional traversal, null-terminated strings and brace-scoped variables."},
        {"Pointers and strings", "From the pointer and memory slides: & obtains an address; * accesses a value. A pointer must refer to valid memory."},
        {"Linked nodes", "From pointer-linked examples: each node has data, next and previous pointers. Update both links when inserting; delete allocations when done."}
    }; return v;
}
inline const std::vector<Lab>& All() {
    static const std::vector<Lab> v = {
        {0, 0, R"LAB(Leap Year)LAB", R"LAB(A condition can combine divisibility tests with &&, || and !. The % operator returns a remainder.)LAB", R"LAB(Return true for a leap year: divisible by 4, except centuries unless divisible by 400.)LAB", R"LAB(bool isLeapYear(int year))LAB", R"LAB(2000 -> true; 1900 -> false)LAB", R"LAB()LAB", R"LAB(bool isLeapYear(int year) {
    // your code here
    return false;
}
)LAB", {R"LAB(Test divisibility using year % divisor == 0.)LAB",R"LAB(Years divisible by 100 usually fail, even when divisible by 4.)LAB",R"LAB(The complete rule is: by 400 OR (by 4 AND not by 100).)LAB"}, {R"LAB(isLeapYear(2000) && !isLeapYear(1900))LAB",R"LAB(isLeapYear(2024) && !isLeapYear(2023))LAB",R"LAB(isLeapYear(1600) && !isLeapYear(1700))LAB"}},
        {1, 0, R"LAB(Reverse Digits)LAB", R"LAB(Integer division by 10 removes the last digit. Modulo 10 extracts the last digit.)LAB", R"LAB(Reverse the digits of a nonnegative integer, dropping leading zeros in the result. Inputs fit an int.)LAB", R"LAB(int Reversed(int testNumber))LAB", R"LAB(123 -> 321; 1200 -> 21)LAB", R"LAB()LAB", R"LAB(int Reversed(int testNumber) {
    // your code here
    return 0;
}
)LAB", {R"LAB(Take the last digit with number % 10.)LAB",R"LAB(Shift your result left one decimal place: result = result * 10 + digit.)LAB",R"LAB(Repeat while the input is positive, using input /= 10.)LAB"}, {R"LAB(Reversed(123)==321 && Reversed(1)==1)LAB",R"LAB(Reversed(1200)==21 && Reversed(0)==0)LAB",R"LAB(Reversed(50103)==30105)LAB"}},
        {2, 0, R"LAB(Palindrome Number)LAB", R"LAB(Reuse a helper instead of repeating the same work. Comparing values tests whether digits read the same both ways.)LAB", R"LAB(Return true if a nonnegative decimal integer reads the same in reverse.)LAB", R"LAB(bool isAPalindrome(int testNumber))LAB", R"LAB(121 -> true; 123 -> false)LAB", R"LAB()LAB", R"LAB(bool isAPalindrome(int testNumber) {
    // your code here
    return false;
}
)LAB", {R"LAB(First test what Reversed(121) returns.)LAB",R"LAB(This lab also needs Reversed. You may copy your earlier implementation above this function.)LAB",R"LAB(Compare testNumber against Reversed(testNumber); negative inputs are outside this lab.)LAB"}, {R"LAB(isAPalindrome(121) && isAPalindrome(7))LAB",R"LAB(!isAPalindrome(1213) && !isAPalindrome(123))LAB",R"LAB(isAPalindrome(0) && isAPalindrome(1221))LAB"}},
        {3, 0, R"LAB(Prime Test)LAB", R"LAB(A prime has exactly two positive factors: 1 and itself. You only need to search for divisors up to the square root.)LAB", R"LAB(Return true for prime integers, including large primes. Values below 2 are not prime.)LAB", R"LAB(bool isAPrimeNumber(int numbertoTest))LAB", R"LAB(17 -> true; 21 -> false)LAB", R"LAB()LAB", R"LAB(bool isAPrimeNumber(int numbertoTest) {
    // your code here
    return false;
}
)LAB", {R"LAB(Reject numbers less than 2.)LAB",R"LAB(Try candidate divisors from 2 upward; a remainder of zero means composite.)LAB",R"LAB(Stop when divisor > number / divisor, avoiding overflow from divisor * divisor.)LAB"}, {R"LAB(isAPrimeNumber(2) && isAPrimeNumber(3) && !isAPrimeNumber(1))LAB",R"LAB(!isAPrimeNumber(4) && !isAPrimeNumber(100))LAB",R"LAB(isAPrimeNumber(2147483647) && !isAPrimeNumber(2147483646))LAB"}},
        {4, 0, R"LAB(Binary Digits to Decimal)LAB", R"LAB(Repeated division and modulo unpack decimal digits. A binary digit has a weight of 1, 2, 4, 8 and so on.)LAB", R"LAB(Interpret an integer such as 1101 as a sequence of binary digits, without shift operators. Inputs contain only 0 and 1.)LAB", R"LAB(int convertBinarytoDecimal(int binaryNumber))LAB", R"LAB(1101 -> 13)LAB", R"LAB()LAB", R"LAB(int convertBinarytoDecimal(int binaryNumber) {
    // your code here
    return 0;
}
)LAB", {R"LAB(Remove the rightmost decimal digit with % 10.)LAB",R"LAB(Keep a place-value variable that starts at 1 and doubles after each digit.)LAB",R"LAB(Add digit * placeValue, then binaryNumber /= 10.)LAB"}, {R"LAB(convertBinarytoDecimal(110)==6 && convertBinarytoDecimal(111)==7)LAB",R"LAB(convertBinarytoDecimal(0)==0 && convertBinarytoDecimal(1)==1)LAB",R"LAB(convertBinarytoDecimal(10001)==17 && convertBinarytoDecimal(101010)==42)LAB"}},
        {5, 1, R"LAB(Right-Angled A Triangle)LAB", R"LAB(Nested loops are useful when output has rows and columns. The outer loop chooses the row; the inner loop prints its characters.)LAB", R"LAB(Print four lines: A, AA, AAA, AAAA. Use two nested for loops. Use printf for output.)LAB", R"LAB(void drawRightAngledTriangle())LAB", R"LAB(A / AA / AAA / AAAA)LAB", R"LAB(A
AA
AAA
AAAA
)LAB", R"LAB(void drawRightAngledTriangle() {
    // your code here
}
)LAB", {R"LAB(Use an outer loop from row 1 through row 4.)LAB",R"LAB(On row r, the inner loop should output r copies of A.)LAB",R"LAB(Output a newline after the inner loop finishes each row.)LAB"}, {R"LAB(true)LAB",R"LAB(2+2==4)LAB"}},
        {6, 1, R"LAB(Growing and Shrinking Triangle)LAB", R"LAB(A pattern can grow to a peak and then shrink. Trace the row lengths before writing the printing logic.)LAB", R"LAB(Print seven lines containing 1,2,3,4,3,2,1 letters A. Use two for loops and at most one if/else.)LAB", R"LAB(void drawIsocelesTriangle())LAB", R"LAB(A / AA / AAA / AAAA / AAA / AA / A)LAB", R"LAB(A
AA
AAA
AAAA
AAA
AA
A
)LAB", R"LAB(void drawIsocelesTriangle() {
    // your code here
}
)LAB", {R"LAB(Think of seven rows, indexed 0 through 6.)LAB",R"LAB(The row width can be calculated from the distance to the middle row.)LAB",R"LAB(One loop chooses rows; the second prints the needed A characters, then a newline.)LAB"}, {R"LAB(true)LAB",R"LAB(3*3==9)LAB"}},
        {7, 1, R"LAB(Triangle Without Branches)LAB", R"LAB(Sometimes a mathematical expression avoids a branch. Two separate phases can also build a symmetrical pattern.)LAB", R"LAB(Print the same 1,2,3,4,3,2,1 pattern using exactly two for loops and no if/else.)LAB", R"LAB(void drawIsocelesTriangle2())LAB", R"LAB(Seven lines, widest line AAAA)LAB", R"LAB(A
AA
AAA
AAAA
AAA
AA
A
)LAB", R"LAB(void drawIsocelesTriangle2() {
    // your code here
}
)LAB", {R"LAB(Split the pattern into growing and shrinking phases.)LAB",R"LAB(Consider one loop for the top and one for the bottom, with character repetitions built via std::string(row, 'A') or repeated writes.)LAB",R"LAB(Avoid if/else and keep four A characters at the peak.)LAB"}, {R"LAB(true)LAB",R"LAB(5>3)LAB"}},
        {8, 2, R"LAB(Find an Array Element)LAB", R"LAB(Arrays contain contiguous elements indexed from zero. A linear search visits each index until a match.)LAB", R"LAB(Return the first index of toFind in arr, or -1 if absent.)LAB", R"LAB(int find(int size, int arr[], int toFind))LAB", R"LAB([4,9,4], find 9 -> 1)LAB", R"LAB()LAB", R"LAB(int find(int size, int arr[], int toFind) {
    // your code here
    return -1;
}
)LAB", {R"LAB(Start at index zero and stop before size.)LAB",R"LAB(Compare arr[i] to toFind at each position.)LAB",R"LAB(Return immediately on the first match; -1 if the scan finishes.)LAB"}, {R"LAB(([](){int a[]={2,7,2};return find(3,a,2)==0;})())LAB",R"LAB(([](){int a[]={2,7,2};return find(3,a,9)==-1;})())LAB",R"LAB(([](){int a[]={8};return find(1,a,8)==0;})())LAB"}},
        {9, 2, R"LAB(Second Distinct Largest)LAB", R"LAB(When looking for two extremes, update the best two candidates during a single scan.)LAB", R"LAB(Return the second DISTINCT largest positive integer, or -1 if fewer than two distinct values.)LAB", R"LAB(int find2ndLargest(int size, int arr[]))LAB", R"LAB([8,3,8,5] -> 5)LAB", R"LAB()LAB", R"LAB(int find2ndLargest(int size, int arr[]) {
    // your code here
    return -1;
}
)LAB", {R"LAB(Keep the largest and second-largest values seen so far.)LAB",R"LAB(If a new value beats largest, the previous largest becomes second.)LAB",R"LAB(Ignore values equal to largest, so [5,5] has no distinct second largest.)LAB"}, {R"LAB(([](){int a[]={4,9,2,9};return find2ndLargest(4,a)==4;})())LAB",R"LAB(([](){int a[]={5,5};return find2ndLargest(2,a)==-1;})())LAB",R"LAB(([](){int a[]={1,2,3,4};return find2ndLargest(4,a)==3;})())LAB"}},
        {10, 2, R"LAB(Copy an Array)LAB", R"LAB(Pass an array into a function to work on its original storage. Copy only valid positions.)LAB", R"LAB(Copy size integers from arr1 to arr2 without changing arr1.)LAB", R"LAB(void copyArraytoArray(int size, int arr1[], int arr2[]))LAB", R"LAB([1,2] -> [1,2])LAB", R"LAB()LAB", R"LAB(void copyArraytoArray(int size, int arr1[], int arr2[]) {
    // your code here
}
)LAB", {R"LAB(Start at index zero.)LAB",R"LAB(Assign arr2[i] = arr1[i] at every index.)LAB",R"LAB(Stop before i == size; zero-length arrays require no work.)LAB"}, {R"LAB(([](){int a[]={1,2,3},b[]={0,0,0};copyArraytoArray(3,a,b);return b[0]==1&&b[1]==2&&b[2]==3;})())LAB",R"LAB(([](){int a[]={7},b[]={0};copyArraytoArray(1,a,b);return b[0]==7;})())LAB",R"LAB(([](){int a[]={-1,5},b[]={8,8};copyArraytoArray(2,a,b);return b[0]==-1&&b[1]==5;})())LAB"}},
        {11, 2, R"LAB(Insert Into an Array)LAB", R"LAB(Inserting into a fixed-capacity array requires room and a right-to-left shift to avoid overwriting.)LAB", R"LAB(Insert at a zero-based index 0..count. size is capacity and stays unchanged; count grows on success. Return false for full/invalid requests.)LAB", R"LAB(bool insertElement(int& size, int& count, int arr[], int elementToInsert, int insertIndex))LAB", R"LAB(Insert 9 at 1 into [1,2,3] -> [1,9,2,3])LAB", R"LAB()LAB", R"LAB(bool insertElement(
    int& size,
    int& count,
    int arr[],
    int elementToInsert,
    int insertIndex
) {
    // your code here
    return false;
}
)LAB", {R"LAB(Check 0 <= insertIndex <= count and count < size.)LAB",R"LAB(Move elements from count-1 down to insertIndex, one position right.)LAB",R"LAB(Write the new value at insertIndex, increment count, return true.)LAB"}, {R"LAB(([](){int cap=5,n=3,a[]={1,2,3,0,0};bool b=insertElement(cap,n,a,9,1);return b&&cap==5&&n==4&&a[0]==1&&a[1]==9&&a[2]==2&&a[3]==3;})())LAB",R"LAB(([](){int cap=2,n=2,a[]={1,2};return !insertElement(cap,n,a,9,0)&&n==2;})())LAB",R"LAB(([](){int cap=4,n=2,a[]={5,6,0,0};return insertElement(cap,n,a,7,2)&&n==3&&a[2]==7;})())LAB"}},
        {12, 2, R"LAB(Delete From an Array)LAB", R"LAB(Deletion slides later elements one position left so the remaining values stay contiguous.)LAB", R"LAB(Delete at a zero-based index 0..count-1. size is capacity and stays unchanged; count decreases on success.)LAB", R"LAB(bool deleteElement(int& size, int& count, int arr[], int deleteIndex))LAB", R"LAB([4,7,9], delete 1 -> [4,9])LAB", R"LAB()LAB", R"LAB(bool deleteElement(
    int& size,
    int& count,
    int arr[],
    int deleteIndex
) {
    // your code here
    return false;
}
)LAB", {R"LAB(Check that count > 0 and deleteIndex is within [0,count).)LAB",R"LAB(For each index from deleteIndex to count-2, assign arr[i] = arr[i+1].)LAB",R"LAB(Decrement count only after a successful shift.)LAB"}, {R"LAB(([](){int cap=5,n=3,a[]={4,7,9,0,0};bool b=deleteElement(cap,n,a,1);return b&&cap==5&&n==2&&a[0]==4&&a[1]==9;})())LAB",R"LAB(([](){int cap=2,n=0,a[]={1,2};return !deleteElement(cap,n,a,0)&&n==0;})())LAB",R"LAB(([](){int cap=3,n=2,a[]={6,8,0};return deleteElement(cap,n,a,0)&&n==1&&a[0]==8;})())LAB"}},
        {13, 2, R"LAB(Frequency Count)LAB", R"LAB(A frequency is the number of matching values in a collection.)LAB", R"LAB(Return how many times value appears in the first size entries.)LAB", R"LAB(int frequencyCount(int size, int arr[], int value))LAB", R"LAB([2,3,2], value 2 -> 2)LAB", R"LAB()LAB", R"LAB(int frequencyCount(int size, int arr[], int value) {
    // your code here
    return 0;
}
)LAB", {R"LAB(Begin with a counter of zero.)LAB",R"LAB(Visit the first size positions and compare each element to value.)LAB",R"LAB(Increment the counter on every match, including repeats.)LAB"}, {R"LAB(([](){int a[]={2,3,2,2};return frequencyCount(4,a,2)==3;})())LAB",R"LAB(([](){int a[]={1,3};return frequencyCount(2,a,8)==0;})())LAB",R"LAB(([](){int a[]={9};return frequencyCount(1,a,9)==1;})())LAB"}},
        {14, 2, R"LAB(Count Duplicate Occurrences)LAB", R"LAB(Duplicate counting requires a precise definition. Here each occurrence AFTER the first occurrence of the same value counts once.)LAB", R"LAB(Return total duplicate occurrences, e.g. [3,3,3,4,4] -> 3. Do not modify the input.)LAB", R"LAB(int countDuplicates(int size, int arr[]))LAB", R"LAB([1,1,1] -> 2)LAB", R"LAB()LAB", R"LAB(int countDuplicates(int size, int arr[]) {
    // your code here
    return 0;
}
)LAB", {R"LAB(For each index i, ask whether arr[i] has appeared earlier.)LAB",R"LAB(Search indices 0..i-1 and stop once a match is found.)LAB",R"LAB(Count one duplicate for each index with an earlier match.)LAB"}, {R"LAB(([](){int a[]={3,3,3,4,4};return countDuplicates(5,a)==3;})())LAB",R"LAB(([](){int a[]={1,2,3};return countDuplicates(3,a)==0;})())LAB",R"LAB(([](){int a[]={8,8};return countDuplicates(2,a)==1;})())LAB"}},
        {15, 2, R"LAB(Reverse an Array in Place)LAB", R"LAB(Swapping mirrored positions avoids allocating a second array.)LAB", R"LAB(Reverse arr in place using only the original array.)LAB", R"LAB(void reverse(int size, int arr[]))LAB", R"LAB([1,2,3] -> [3,2,1])LAB", R"LAB()LAB", R"LAB(void reverse(int size, int arr[]) {
    // your code here
}
)LAB", {R"LAB(Use two indices: left=0 and right=size-1.)LAB",R"LAB(Swap arr[left] and arr[right], then move inward.)LAB",R"LAB(Stop when left >= right; the middle element stays put for odd sizes.)LAB"}, {R"LAB(([](){int a[]={1,2,3,4};reverse(4,a);return a[0]==4&&a[1]==3&&a[2]==2&&a[3]==1;})())LAB",R"LAB(([](){int a[]={7};reverse(1,a);return a[0]==7;})())LAB",R"LAB(([](){int a[]={2,9,5};reverse(3,a);return a[0]==5&&a[1]==9&&a[2]==2;})())LAB"}},
        {16, 2, R"LAB(Rotate Left Once)LAB", R"LAB(Rotating keeps every element but moves its position. Save the first element before shifting.)LAB", R"LAB(Move the first entry to the end in place. Return the original first value; return -1 for size 0.)LAB", R"LAB(int rotateLeft(int size, int arr[]))LAB", R"LAB([1,2,3] -> [2,3,1])LAB", R"LAB()LAB", R"LAB(int rotateLeft(int size, int arr[]) {
    // your code here
    return -1;
}
)LAB", {R"LAB(If size is zero, return -1 without reading arr[0].)LAB",R"LAB(Save arr[0], then move each arr[i+1] into arr[i].)LAB",R"LAB(Put the saved first value at arr[size-1], then return it.)LAB"}, {R"LAB(([](){int a[]={1,2,3};return rotateLeft(3,a)==1&&a[0]==2&&a[1]==3&&a[2]==1;})())LAB",R"LAB(([](){int a[]={8};return rotateLeft(1,a)==8&&a[0]==8;})())LAB",R"LAB(([](){int a[]={4};return rotateLeft(0,a)==-1;})())LAB"}},
        {17, 2, R"LAB(Two Movies for a Flight)LAB", R"LAB(A pair-sum task illustrates nested search, early return and the rule that one array position cannot be used twice.)LAB", R"LAB(Return true when two DISTINCT indices have movie lengths summing exactly to flightLength.)LAB", R"LAB(bool twoMovies(int flightLength, int movieLengths[], int size))LAB", R"LAB(Flight 150, movies [60,90,120] -> true)LAB", R"LAB()LAB", R"LAB(bool twoMovies(int flightLength, int movieLengths[], int size) {
    // your code here
    return false;
}
)LAB", {R"LAB(Try the first movie at index i.)LAB",R"LAB(Compare it with every movie at index j > i.)LAB",R"LAB(Return true if movieLengths[i] + movieLengths[j] equals flightLength.)LAB"}, {R"LAB(([](){int a[]={60,90,120};return twoMovies(150,a,3);})())LAB",R"LAB(([](){int a[]={90};return !twoMovies(180,a,1);})())LAB",R"LAB(([](){int a[]={60,60};return twoMovies(120,a,2);})())LAB"}},
        {18, 2, R"LAB(Count Words in Characters)LAB", R"LAB(A state flag can remember whether you are inside a run of letters. A new word begins when a letter follows a non-letter.)LAB", R"LAB(Count runs of A..Z or a..z in a char array of length size. Any other character separates words.)LAB", R"LAB(int wordCounter(int size, char characters[]))LAB", R"LAB("Hello C++" -> 2 words)LAB", R"LAB()LAB", R"LAB(int wordCounter(int size, char characters[]) {
    // your code here
    return 0;
}
)LAB", {R"LAB(Keep a bool inWord and an integer count.)LAB",R"LAB(When a letter arrives and inWord is false, start a new word and increment count.)LAB",R"LAB(When a separator arrives, set inWord back to false.)LAB"}, {R"LAB(([](){char a[]={'H','i',' ','C','+','+'};return wordCounter(6,a)==2;})())LAB",R"LAB(([](){char a[]={' ','a',' ',' ','B',' '};return wordCounter(6,a)==2;})())LAB",R"LAB(([](){char a[]={'1','2','3'};return wordCounter(3,a)==0;})())LAB"}},
        {19, 3, R"LAB(Print a 4 by 6 Grid)LAB", R"LAB(A two-dimensional array has rows and columns. Nested loops visit every cell using row and column indices.)LAB", R"LAB(Print all 4 rows and 6 columns, separated by spaces, one row per line. Only this grid task uses printf for output.)LAB", R"LAB(void PrintArray(int array[4][6]))LAB", R"LAB(4 rows, 6 values each)LAB", R"LAB(0 0 3 1 3 4
0 0 2 3 4 3
0 0 1 3 3 2
0 0 1 1 1 1
)LAB", R"LAB(void PrintArray(int array[4][6]) {
    // your code here
}
)LAB", {R"LAB(Use an outer loop with row values 0..3.)LAB",R"LAB(Inside, iterate columns 0..5 and print each integer.)LAB",R"LAB(Print a newline at the end of each row; do not print extra text.)LAB"}, {R"LAB(true)LAB",R"LAB(4+6==10)LAB"}},
        {20, 3, R"LAB(Remove First Matching Cluster)LAB", R"LAB(In a 4 by 6 grid, scanning order matters. At each position check horizontal first, then vertical.)LAB", R"LAB(Find the first horizontal or vertical run of 3+ equal positive values (1..4), score the whole run, replace it with -1, return score. Ignore 0, -1 and -2.)LAB", R"LAB(int ReplaceWithMinusOne(int array[4][6]))LAB", R"LAB(Three 2s in a row -> score 6)LAB", R"LAB()LAB", R"LAB(int ReplaceWithMinusOne(int array[4][6]) {
    // your code here
    return 0;
}
)LAB", {R"LAB(Scan row 0 to 3 and column 0 to 5. Skip values outside 1..4.)LAB",R"LAB(At each cell, count the run to the right first; if shorter than 3, count downward.)LAB",R"LAB(Mark every member of the chosen run -1 and return runLength * value.)LAB"}, {R"LAB(([](){int a[4][6]={{1,1,1,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}};return ReplaceWithMinusOne(a)==3&&a[0][0]==-1&&a[0][2]==-1;})())LAB",R"LAB(([](){int a[4][6]={{2,0,0,0,0,0},{2,0,0,0,0,0},{2,0,0,0,0,0},{0,0,0,0,0,0}};return ReplaceWithMinusOne(a)==6&&a[2][0]==-1;})())LAB",R"LAB(([](){int a[4][6]={{-2,-2,-2,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}};return ReplaceWithMinusOne(a)==0;})())LAB"}},
        {21, 3, R"LAB(Gravity in a 4 by 6 Grid)LAB", R"LAB(Stable compaction moves surviving values downward while preserving their relative order. Fill remaining spaces at the top.)LAB", R"LAB(For every column, remove -1 cells, move all remaining values down in order, and fill the top with -2.)LAB", R"LAB(void FallDownAndReplace(int array[4][6]))LAB", R"LAB(Column [1,-1,3,-1] -> [-2,-2,1,3])LAB", R"LAB()LAB", R"LAB(void FallDownAndReplace(int array[4][6]) {
    // your code here
}
)LAB", {R"LAB(Process one column at a time, starting at the bottom row.)LAB",R"LAB(Keep writeRow=3. Copy non--1 cells downward while scanning rows 3 to 0.)LAB",R"LAB(Fill remaining rows 0..writeRow with -2.)LAB"}, {R"LAB(([](){int a[4][6]={{1,0,0,0,0,0},{-1,0,0,0,0,0},{3,0,0,0,0,0},{-1,0,0,0,0,0}};FallDownAndReplace(a);return a[0][0]==-2&&a[1][0]==-2&&a[2][0]==1&&a[3][0]==3;})())LAB",R"LAB(([](){int a[4][6]={{-1,0,0,0,0,0},{-1,0,0,0,0,0},{-1,0,0,0,0,0},{5,0,0,0,0,0}};FallDownAndReplace(a);return a[0][0]==-2&&a[2][0]==-2&&a[3][0]==5;})())LAB",R"LAB(([](){int a[4][6]={{1,0,0,0,0,0},{2,0,0,0,0,0},{3,0,0,0,0,0},{4,0,0,0,0,0}};FallDownAndReplace(a);return a[0][0]==1&&a[3][0]==4;})())LAB"}},
        {22, 3, R"LAB(Find Declared Variable Names)LAB", R"LAB(A C string ends with a null terminator. Reading code text can be done line by line, recognising simple declarations.)LAB", R"LAB(Write comma-separated names of int/char variables from code into variables; only one declaration per line. Inputs contain simple declarations, no comments or complex types. Output must be null-terminated.)LAB", R"LAB(void FindAllVariables(char variables[], char code[]))LAB", R"LAB(int age=10; char c; -> age,c)LAB", R"LAB()LAB", R"LAB(void FindAllVariables(char variables[], char code[]) {
    // your code here
}
)LAB", {R"LAB(Start with variables[0] = '\0'.)LAB",R"LAB(At each new line, skip whitespace and check for int or char followed by whitespace.)LAB",R"LAB(Read the identifier until a non-letter/digit/underscore, and append it with commas.)LAB"}, {R"LAB(([](){char code[]="int count=10;\nchar grade;\n";char out[200]="";FindAllVariables(out,code);return std::strcmp(out,"count,grade")==0;})())LAB",R"LAB(([](){char code[]="  char x;\n  int total = 0;\n";char out[200]="";FindAllVariables(out,code);return std::strcmp(out,"x,total")==0;})())LAB",R"LAB(([](){char code[]="float ignored;\nint x;\n";char out[200]="";FindAllVariables(out,code);return std::strcmp(out,"x")==0;})())LAB"}},
        {23, 3, R"LAB(Variables Still in Scope)LAB", R"LAB(Braces define nested scopes. After a closing brace, declarations made inside that block stop being accessible.)LAB", R"LAB(Report the int/char names in scope AFTER the given 1-based line number. Only one declaration per line; braces may be on separate lines. Output comma-separated in declaration order.)LAB", R"LAB(void FindAllVariablesInScope(char variables[], char code[], int lineNumber))LAB", R"LAB(Inside block: a,b; after closing: a)LAB", R"LAB()LAB", R"LAB(void FindAllVariablesInScope(
    char variables[],
    char code[],
    int lineNumber
) {
    // your code here
}
)LAB", {R"LAB(Track brace depth as lines are read.)LAB",R"LAB(Remember every declaration and the depth when it appears.)LAB",R"LAB(When a } closes a block, discard declarations made deeper than the new depth; stop after lineNumber.)LAB"}, {R"LAB(([](){char c[]="int main() {\nint a=1;\n{\nint b=2;\n}\nint z=3;\n}\n";char out[200]="";FindAllVariablesInScope(out,c,4);return std::strcmp(out,"a,b")==0;})())LAB",R"LAB(([](){char c[]="int main() {\nint a=1;\n{\nint b=2;\n}\nint z=3;\n}\n";char out[200]="";FindAllVariablesInScope(out,c,6);return std::strcmp(out,"a,z")==0;})())LAB",R"LAB(([](){char c[]="int main() {\nchar flag;\n}\n";char out[200]="";FindAllVariablesInScope(out,c,2);return std::strcmp(out,"flag")==0;})())LAB"}},
        {24, 4, R"LAB(Swap Integers With Pointers)LAB", R"LAB(The & operator obtains an address; * reads or changes the value at that address. Pointer parameters can update caller variables.)LAB", R"LAB(Swap two integers using pointer dereferencing, without array indexing.)LAB", R"LAB(void swap_ints(int *a, int *b))LAB", R"LAB(5, 10 -> 10, 5)LAB", R"LAB()LAB", R"LAB(void swap_ints(int *a, int *b) {
    // your code here
}
)LAB", {R"LAB(Read *a and *b to get the values.)LAB",R"LAB(Store one value in a temporary integer.)LAB",R"LAB(Write into *a and *b; swapping the pointer addresses alone will not affect callers.)LAB"}, {R"LAB(([](){int a=5,b=10;swap_ints(&a,&b);return a==10&&b==5;})())LAB",R"LAB(([](){int a=-1,b=0;swap_ints(&a,&b);return a==0&&b==-1;})())LAB",R"LAB(([](){int a=7;swap_ints(&a,&a);return a==7;})())LAB"}},
        {25, 4, R"LAB(Swap Characters With Pointers)LAB", R"LAB(A char pointer works like an int pointer but refers to a character; dereferencing accesses the value.)LAB", R"LAB(Swap two characters using only pointer operations.)LAB", R"LAB(void swap_chars(char *a, char *b))LAB", R"LAB(A, B -> B, A)LAB", R"LAB()LAB", R"LAB(void swap_chars(char *a, char *b) {
    // your code here
}
)LAB", {R"LAB(Dereference each pointer with * to read its character.)LAB",R"LAB(Save *a in a temporary char.)LAB",R"LAB(Write *b into *a, then the saved character into *b.)LAB"}, {R"LAB(([](){char a='A',b='B';swap_chars(&a,&b);return a=='B'&&b=='A';})())LAB",R"LAB(([](){char a='x',b='x';swap_chars(&a,&b);return a=='x'&&b=='x';})())LAB",R"LAB(([](){char a='1',b='2';swap_chars(&a,&b);return a=='2'&&b=='1';})())LAB"}},
        {26, 4, R"LAB(Largest Value Via Pointers)LAB", R"LAB(Pointer arithmetic moves by elements, not raw bytes: *(arr + i) accesses element i.)LAB", R"LAB(Return the maximum integer in a nonempty array using only pointer arithmetic, not arr[i].)LAB", R"LAB(int find_max(int *arr, int size))LAB", R"LAB([3,9,2] -> 9)LAB", R"LAB()LAB", R"LAB(int find_max(int *arr, int size) {
    // your code here
    return 0;
}
)LAB", {R"LAB(Initialise max from *arr.)LAB",R"LAB(Advance through the remaining values with *(arr + i) or by incrementing a pointer.)LAB",R"LAB(Update max whenever the pointed-to value is larger.)LAB"}, {R"LAB(([](){int a[]={3,9,2,8,5};return find_max(a,5)==9;})())LAB",R"LAB(([](){int a[]={-5,-2,-9};return find_max(a,3)==-2;})())LAB",R"LAB(([](){int a[]={7};return find_max(a,1)==7;})())LAB"}},
        {27, 4, R"LAB(Reverse Array Via Pointers)LAB", R"LAB(Pointers can identify both ends of an array. Move inward to reverse without extra storage.)LAB", R"LAB(Reverse an integer array in place using pointer arithmetic only; no bracket indexing.)LAB", R"LAB(void reverse_array(int *arr, int size))LAB", R"LAB([1,2,3] -> [3,2,1])LAB", R"LAB()LAB", R"LAB(void reverse_array(int *arr, int size) {
    // your code here
}
)LAB", {R"LAB(Set left to arr and right to arr + size - 1 when size > 0.)LAB",R"LAB(Swap *left and *right.)LAB",R"LAB(Advance left and retreat right until they cross.)LAB"}, {R"LAB(([](){int a[]={1,2,3,4,5};reverse_array(a,5);return a[0]==5&&a[1]==4&&a[2]==3&&a[3]==2&&a[4]==1;})())LAB",R"LAB(([](){int a[]={8};reverse_array(a,1);return a[0]==8;})())LAB",R"LAB(([](){int a[]={1,2};reverse_array(a,2);return a[0]==2&&a[1]==1;})())LAB"}},
        {28, 4, R"LAB(C-String Length Via Pointers)LAB", R"LAB(A C string ends with \0. Its length is the number of characters before that terminator.)LAB", R"LAB(Implement string length using pointer movement only; do not use strlen or indexing.)LAB", R"LAB(int strlen_ptr(char *str))LAB", R"LAB("Hello" -> 5)LAB", R"LAB()LAB", R"LAB(int strlen_ptr(char *str) {
    // your code here
    return 0;
}
)LAB", {R"LAB(Start a counter at zero.)LAB",R"LAB(While *str is not '\0', increment the counter and the pointer.)LAB",R"LAB(Return the count, not including the terminator.)LAB"}, {R"LAB(([](){char a[]="Hello";return strlen_ptr(a)==5;})())LAB",R"LAB(([](){char a[]="";return strlen_ptr(a)==0;})())LAB",R"LAB(([](){char a[]="a b";return strlen_ptr(a)==3;})())LAB"}},
        {29, 4, R"LAB(Count Vowels Via Pointers)LAB", R"LAB(Scanning each character is a retrieval exercise: compare against a small set of values and advance safely to the terminator.)LAB", R"LAB(Count vowels a,e,i,o,u in either case with pointers only.)LAB", R"LAB(int count_vowels(char *str))LAB", R"LAB(OpenAI -> 4 vowels)LAB", R"LAB()LAB", R"LAB(int count_vowels(char *str) {
    // your code here
    return 0;
}
)LAB", {R"LAB(Loop until *str == '\0'.)LAB",R"LAB(Check a,e,i,o,u and their uppercase versions.)LAB",R"LAB(Advance the pointer once per character, even for consonants.)LAB"}, {R"LAB(([](){char a[]="OpenAI";return count_vowels(a)==4;})())LAB",R"LAB(([](){char a[]="rhythm";return count_vowels(a)==0;})())LAB",R"LAB(([](){char a[]="aeiouAEIOU";return count_vowels(a)==10;})())LAB"}},
        {30, 4, R"LAB(Copy C-String Via Pointers)LAB", R"LAB(Copying C strings means copying the null terminator too, so the destination is a valid string.)LAB", R"LAB(Copy src into dest with pointer arithmetic only. Assume dest has sufficient capacity.)LAB", R"LAB(void copy_string(char *dest, char *src))LAB", R"LAB("Hi" -> "Hi\0")LAB", R"LAB()LAB", R"LAB(void copy_string(char *dest, char *src) {
    // your code here
}
)LAB", {R"LAB(Copy *src into *dest.)LAB",R"LAB(Move both pointers one character forward after each copy.)LAB",R"LAB(Do not forget the terminating '\0'; stop after copying it.)LAB"}, {R"LAB(([](){char a[]="Hello",b[12]="";copy_string(b,a);return std::strcmp(b,"Hello")==0;})())LAB",R"LAB(([](){char a[]="",b[5]="hi";copy_string(b,a);return b[0]==0;})())LAB",R"LAB(([](){char a[]="C++",b[8]="";copy_string(b,a);return std::strcmp(b,"C++")==0;})())LAB"}},
        {31, 4, R"LAB(Concatenate C-Strings Via Pointers)LAB", R"LAB(To append a C string, first locate the end of the destination and then copy the source including its terminator.)LAB", R"LAB(Append src to dest using pointer arithmetic only. Assume enough space.)LAB", R"LAB(void concat_strings(char *dest, char *src))LAB", R"LAB("Hello, " + "World!" -> "Hello, World!")LAB", R"LAB()LAB", R"LAB(void concat_strings(char *dest, char *src) {
    // your code here
}
)LAB", {R"LAB(Advance dest until *dest == '\0'.)LAB",R"LAB(Copy each *src into *dest and advance both pointers.)LAB",R"LAB(Copy the final null terminator as well.)LAB"}, {R"LAB(([](){char d[30]="Hello, ",s[]="World!";concat_strings(d,s);return std::strcmp(d,"Hello, World!")==0;})())LAB",R"LAB(([](){char d[10]="",s[]="abc";concat_strings(d,s);return std::strcmp(d,"abc")==0;})())LAB",R"LAB(([](){char d[10]="abc",s[]="";concat_strings(d,s);return std::strcmp(d,"abc")==0;})())LAB"}},
        {32, 4, R"LAB(Sum an Array Via Pointers)LAB", R"LAB(Pointer addition advances by whole elements. Accumulate a running total as you traverse.)LAB", R"LAB(Return the sum of size integers using pointer arithmetic, not arr[i].)LAB", R"LAB(int sum_array(int *arr, int size))LAB", R"LAB([1,2,3,4] -> 10)LAB", R"LAB()LAB", R"LAB(int sum_array(int *arr, int size) {
    // your code here
    return 0;
}
)LAB", {R"LAB(Start sum at zero.)LAB",R"LAB(For i from 0 to size-1, add *(arr + i).)LAB",R"LAB(For size zero, return zero without dereferencing arr.)LAB"}, {R"LAB(([](){int a[]={1,2,3,4};return sum_array(a,4)==10;})())LAB",R"LAB(([](){int a[]={-1,5,-4};return sum_array(a,3)==0;})())LAB",R"LAB(([](){int a[]={4};return sum_array(a,0)==0;})())LAB"}},
        {33, 4, R"LAB(Compare C-Strings Via Pointers)LAB", R"LAB(Lexicographic comparison checks corresponding characters until the first difference or a terminating null.)LAB", R"LAB(Compare strings using pointers only; return negative, zero or positive like strcmp.)LAB", R"LAB(int compare_strings(char *str1, char *str2))LAB", R"LAB("apple" < "banana")LAB", R"LAB()LAB", R"LAB(int compare_strings(char *str1, char *str2) {
    // your code here
    return 0;
}
)LAB", {R"LAB(Advance both pointers while their characters are equal and nonzero.)LAB",R"LAB(At the first difference, compare as unsigned char values.)LAB",R"LAB(Return 0 only when the strings end together.)LAB"}, {R"LAB(([](){char a[]="apple",b[]="banana";return compare_strings(a,b)<0;})())LAB",R"LAB(([](){char a[]="same",b[]="same";return compare_strings(a,b)==0;})())LAB",R"LAB(([](){char a[]="z",b[]="aa";return compare_strings(a,b)>0;})())LAB"}},
        {34, 5, R"LAB(Insert a First Node)LAB", R"LAB(A linked sequence stores a value and pointers to neighbouring nodes. The first node has a null previous pointer.)LAB", R"LAB(Using the supplied DoublyLinkedNode structure, allocate a new node at the beginning. Update the old first node's prevPointer when needed.)LAB", R"LAB(void InsertAtBeginning(DoublyLinkedNode*& head, int value))LAB", R"LAB(Empty -> [3]; then insert 2 -> [2,3])LAB", R"LAB()LAB", R"LAB(struct DoublyLinkedNode {
    int data = 0;
    DoublyLinkedNode* prevPointer = nullptr;
    DoublyLinkedNode* nextPointer = nullptr;
};

void InsertAtBeginning(DoublyLinkedNode*& head, int value) {
    // your code here
}
)LAB", {R"LAB(Handle an initially empty list: head may be nullptr.)LAB",R"LAB(Allocate a node with new; its nextPointer should point to the old head and prevPointer be nullptr.)LAB",R"LAB(If there was an old head, change oldHead->prevPointer, then update head.)LAB"}, {R"LAB(([](){DoublyLinkedNode* h=nullptr;InsertAtBeginning(h,3);bool ok=h&&h->data==3&&h->prevPointer==nullptr&&h->nextPointer==nullptr;delete h;return ok;})())LAB",R"LAB(([](){DoublyLinkedNode* h=nullptr;InsertAtBeginning(h,9);InsertAtBeginning(h,2);bool ok=h&&h->data==2&&h->nextPointer&&h->nextPointer->data==9&&h->nextPointer->prevPointer==h;delete h->nextPointer;delete h;return ok;})())LAB",R"LAB(([](){DoublyLinkedNode* h=nullptr;InsertAtBeginning(h,5);InsertAtBeginning(h,4);InsertAtBeginning(h,3);bool ok=h->nextPointer->nextPointer->data==5&&h->prevPointer==nullptr;delete h->nextPointer->nextPointer;delete h->nextPointer;delete h;return ok;})())LAB"}},
        {35, 5, R"LAB(Append a Node)LAB", R"LAB(To append, traverse nextPointer until the last node. Connect the last node and the new node in both directions.)LAB", R"LAB(Allocate a new node with the supplied DoublyLinkedNode structure and append it; update prevPointer and nextPointer consistently.)LAB", R"LAB(void InsertAtEnd(DoublyLinkedNode*& head, int value))LAB", R"LAB(Append 8 to [2] -> [2,8])LAB", R"LAB()LAB", R"LAB(struct DoublyLinkedNode {
    int data = 0;
    DoublyLinkedNode* prevPointer = nullptr;
    DoublyLinkedNode* nextPointer = nullptr;
};

void InsertAtEnd(DoublyLinkedNode*& head, int value) {
    // your code here
}
)LAB", {R"LAB(If head is nullptr, the new node becomes head.)LAB",R"LAB(Otherwise, walk nextPointer until it equals nullptr.)LAB",R"LAB(Set last->nextPointer to the new node and newNode->prevPointer to last.)LAB"}, {R"LAB(([](){DoublyLinkedNode* h=nullptr;InsertAtEnd(h,3);bool ok=h&&h->data==3&&!h->prevPointer&&!h->nextPointer;delete h;return ok;})())LAB",R"LAB(([](){DoublyLinkedNode* h=nullptr;InsertAtEnd(h,2);InsertAtEnd(h,8);bool ok=h->nextPointer&&h->nextPointer->data==8&&h->nextPointer->prevPointer==h&&!h->nextPointer->nextPointer;delete h->nextPointer;delete h;return ok;})())LAB",R"LAB(([](){DoublyLinkedNode* h=nullptr;InsertAtEnd(h,1);InsertAtEnd(h,2);InsertAtEnd(h,3);bool ok=h->nextPointer->nextPointer->data==3&&h->nextPointer->nextPointer->prevPointer==h->nextPointer;delete h->nextPointer->nextPointer;delete h->nextPointer;delete h;return ok;})())LAB"}},
        {36, 5, R"LAB(Insert at a Node Position)LAB", R"LAB(Insertion between nodes updates four connections: the new node's two pointers and its neighbours' matching pointers.)LAB", R"LAB(Insert at a zero-based position from 0 to length inclusive. Return false for invalid positions. Use only the supplied node structure.)LAB", R"LAB(bool InsertAtPosition(DoublyLinkedNode*& head, int position, int value))LAB", R"LAB(Insert 9 at index 1 into [4,7] -> [4,9,7])LAB", R"LAB()LAB", R"LAB(struct DoublyLinkedNode {
    int data = 0;
    DoublyLinkedNode* prevPointer = nullptr;
    DoublyLinkedNode* nextPointer = nullptr;
};

bool InsertAtPosition(
    DoublyLinkedNode*& head,
    int position,
    int value
) {
    // your code here
    return false;
}
)LAB", {R"LAB(Reject negative positions and handle position zero by updating head.)LAB",R"LAB(For position > 0, find the node at position-1; reject if it does not exist.)LAB",R"LAB(Link newNode between previous and next. If next exists, update its prevPointer too.)LAB"}, {R"LAB(([](){DoublyLinkedNode* h=nullptr;bool b=InsertAtPosition(h,0,7);bool ok=b&&h&&h->data==7&&!h->prevPointer&&!h->nextPointer;delete h;return ok;})())LAB",R"LAB(([](){DoublyLinkedNode* h=nullptr;InsertAtPosition(h,0,1);InsertAtPosition(h,1,3);bool b=InsertAtPosition(h,1,2);bool ok=b&&h->data==1&&h->nextPointer->data==2&&h->nextPointer->nextPointer->data==3&&h->nextPointer->prevPointer==h&&h->nextPointer->nextPointer->prevPointer==h->nextPointer;delete h->nextPointer->nextPointer;delete h->nextPointer;delete h;return ok;})())LAB",R"LAB(([](){DoublyLinkedNode* h=nullptr;bool b=InsertAtPosition(h,3,8);return !b&&!h;})())LAB"}},
    }; return v;
}
inline std::string SaveKey(int id) { return "lab"+std::to_string(id); }
} // namespace questlabs
