#pragma once
// Tiny in-world C++ logic locks. Kept independent from raylib and course content.
#include <array>
#include <string>

namespace sh2 {
struct TerminalQuestion {
    const char* name;
    const char* prompt;
    std::array<const char*, 4> answers;
    int correct;
    const char* why;
};
inline const TerminalQuestion& GateQuestion(int chapter) {
    static const TerminalQuestion brookhaven{
        "BROOKHAVEN TERMINAL", "The security log says: int pass = 4; ++pass;  What is pass now?",
        {{"4", "5", "3", "Compiler error"}}, 1,
        "Prefix increment adds one, so pass becomes 5."
    };
    static const TerminalQuestion hotel{
        "LAKEVIEW ARCHIVE", "Which C++ type automatically releases the object it uniquely owns?",
        {{"int*", "std::unique_ptr", "void*", "std::string&"}}, 1,
        "std::unique_ptr provides single ownership and automatic cleanup."
    };
    return chapter == 8 ? hotel : brookhaven;
}
class TerminalLock {
public:
    void Begin(int chapter) { chapter_ = chapter; selected_ = 0; attempts_ = 0; unlocked_ = false; }
    int Selected() const { return selected_; }
    int Attempts() const { return attempts_; }
    bool Unlocked() const { return unlocked_; }
    const TerminalQuestion& Question() const { return GateQuestion(chapter_); }
    void Select(int index) { if (index >= 0 && index < 4) selected_ = index; }
    bool Confirm() { ++attempts_; unlocked_ = selected_ == Question().correct; return unlocked_; }
private:
    int chapter_ = 6, selected_ = 0, attempts_ = 0;
    bool unlocked_ = false;
};
} // namespace sh2
