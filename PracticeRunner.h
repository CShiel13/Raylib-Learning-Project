#pragma once

// Runs only a deliberately small, parsed C++ subset. User code is compiled by
// the installed WASI SDK and executed by a separate Wasmtime process.
#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdlib>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#elif defined(__APPLE__) || defined(__linux__)
#include <cerrno>
#include <fcntl.h>
#include <poll.h>
#include <spawn.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
extern char **environ;
#endif

struct PracticeRunResult {
    bool success = false;
    bool matched = false;
    bool compileError = false;
    std::vector<unsigned char> cells;
    std::string learnerOutput;
    std::string message;
};

enum class PracticeRunMode : unsigned char { GridDrawing = 1, PrintOutput = 2 };

namespace practice_runner_detail {

class SubsetParser {
    struct Token { std::string text; bool identifier = false; };
    std::vector<Token> tokens;
    size_t pos = 0;
    int depth = 0, loopDepth = 0, statementCount = 0, expressionDepth = 0;
    std::vector<std::set<std::string>> scopes{ 1 };
    std::string error;
    bool allowCout = false;

    bool Fail(const std::string& why) { if (error.empty()) error = why; return false; }
    bool At(const char* t) const { return pos < tokens.size() && tokens[pos].text == t; }
    bool Take(const char* t) { if (!At(t)) return false; ++pos; return true; }
    bool Identifier(std::string& out) {
        if (pos >= tokens.size() || !tokens[pos].identifier) return false;
        out = tokens[pos++].text; return true;
    }
    bool Declared(const std::string& n) const {
        for (auto i = scopes.rbegin(); i != scopes.rend(); ++i) if (i->count(n)) return true;
        return false;
    }
    bool IsColour(const std::string& n) const {
        return n == "Red" || n == "Blue" || n == "Yellow" || n == "Green" || n == "Purple";
    }
    bool Reserved(const std::string& n) const { return n == "drawCell" || n == "print" || n == "printLine" || n == "std" || n == "cout" || n == "printf" || IsColour(n); }
    bool AddVariable(const std::string& n) {
        if (Reserved(n)) return Fail("That name is reserved by the drawing API.");
        if (scopes.back().count(n)) return Fail("A variable with that name already exists in this block.");
        scopes.back().insert(n); return true;
    }
    bool Enter() { if (++depth > 32) return Fail("Nesting is limited to 32 levels."); scopes.emplace_back(); return true; }
    void Leave() { scopes.pop_back(); --depth; }

    bool Lex(const std::string& src) {
        if (src.size() > 8192) return Fail("Keep the solution under 8 KB.");
        size_t i = 0;
        while (i < src.size()) {
            unsigned char c = (unsigned char)src[i];
            if (std::isspace(c)) { ++i; continue; }
            if (src[i] == '#') return Fail("Preprocessor lines are outside this C++ subset.");
            if (src[i] == '"' || src[i] == '\'') {
                char quote = src[i++]; size_t begin = i - 1; bool closed = false;
                while (i < src.size()) {
                    unsigned char ch = (unsigned char)src[i++];
                    if (ch == (unsigned char)quote) { closed = true; break; }
                    if (ch == '\\') {
                        if (i >= src.size()) return Fail("Unfinished escape in print literal.");
                        char escaped = src[i++];
                        if (escaped != 'n' && escaped != 'r' && escaped != 't' && escaped != '\\' && escaped != '\'' && escaped != '"')
                            return Fail("Only newline, tab, slash, and quote escapes are supported in print literals.");
                    } else if (ch < 32 || ch > 126) return Fail("Print literals must use printable ASCII characters.");
                }
                if (!closed) return Fail("Unclosed print literal.");
                if (quote == '\'' && i - begin != 3 && !(i - begin == 4 && src[begin + 1] == '\\'))
                    return Fail("Character literals must contain one supported character or escape.");
                tokens.push_back({ src.substr(begin, i - begin), false });
                if (tokens.size() > 3000) return Fail("The solution has too many tokens.");
                continue;
            }
            if (src[i] == '/' && i + 1 < src.size() && src[i + 1] == '/') { i += 2; while (i < src.size() && src[i] != '\n') ++i; continue; }
            if (src[i] == '/' && i + 1 < src.size() && src[i + 1] == '*') {
                i += 2; while (i + 1 < src.size() && !(src[i] == '*' && src[i + 1] == '/')) ++i;
                if (i + 1 >= src.size()) return Fail("Unclosed block comment.");
                i += 2; continue;
            }
            if (std::isalpha(c) || src[i] == '_') {
                size_t j = i + 1; while (j < src.size() && (std::isalnum((unsigned char)src[j]) || src[j] == '_')) ++j;
                tokens.push_back({ src.substr(i, j - i), true }); i = j;
            } else if (std::isdigit(c)) {
                size_t j = i + 1; while (j < src.size() && std::isdigit((unsigned char)src[j])) ++j;
                tokens.push_back({ src.substr(i, j - i), false }); i = j;
            } else {
                static const char* two[] = { "++", "--", "+=", "-=", "*=", "/=", "%=", "==", "!=", "<=", ">=", "&&", "||", "<<" };
                bool matched = false;
                if (i + 1 < src.size()) for (const char* op : two) if (src.compare(i, 2, op) == 0) {
                    tokens.push_back({ op, false }); i += 2; matched = true; break;
                }
                if (!matched) {
                    if (std::string("{}();,:+-*/%<>=!&|^").find(src[i]) == std::string::npos)
                        return Fail("This character is outside the supported C++ subset.");
                    tokens.push_back({ src.substr(i, 1), false }); ++i;
                }
            }
            if (tokens.size() > 3000) return Fail("The solution has too many tokens.");
        }
        return true;
    }

    bool Primary() {
        int prefixes = 0;
        while (At("!") || At("+") || At("-") || At("~")) {
            ++pos;
            if (++prefixes > 32) return Fail("Unary operator nesting is limited to 32 levels.");
        }
        if (Take("(")) { if (!Expression()) return false; if (!Take(")")) return Fail("Expected ')'."); return true; }
        if (pos >= tokens.size()) return Fail("Expected an expression.");
        if (!tokens[pos].identifier) { ++pos; return true; }
        std::string name = tokens[pos++].text;
        if (name == "true" || name == "false" || IsColour(name)) return true;
        if (name == "drawCell") {
            if (!Take("(")) return Fail("drawCell must be called with three arguments.");
            if (!Expression() || !Take(",") || !Expression() || !Take(",") || !Expression() || !Take(")"))
                return Fail("Use drawCell(x, y, colour) with three expressions.");
            return true;
        }
        if (!Declared(name)) return Fail("Unknown name '" + name + "'. Only local variables, colours, and drawCell are available.");
        return true;
    }
    bool Unary() {
        if (Take("++") || Take("--")) {
            std::string n; if (!Identifier(n) || !Declared(n)) return Fail("Increment and decrement need a declared variable.");
            return true;
        }
        if (!Primary()) return false;
        if (Take("++") || Take("--")) return true;
        return true;
    }
    bool BinaryLevel(int level) {
        static const char* groups[][5] = {
            { "||", nullptr }, { "&&", nullptr }, { "|", "^", "&", nullptr },
            { "==", "!=", nullptr }, { "<", ">", "<=", ">=", nullptr },
            { "+", "-", nullptr }, { "*", "/", "%", nullptr }
        };
        if (level == 7) return Unary();
        if (!BinaryLevel(level + 1)) return false;
        while (pos < tokens.size()) {
            bool found = false;
            for (int j = 0; groups[level][j]; ++j) if (At(groups[level][j])) { ++pos; found = true; break; }
            if (!found) break;
            if (!BinaryLevel(level + 1)) return false;
        }
        return true;
    }
    bool Expression() {
        if (++expressionDepth > 64) { --expressionDepth; return Fail("Expression nesting is limited to 64 levels."); }
        bool ok = BinaryLevel(0);
        --expressionDepth;
        return ok;
    }
    bool UpdateExpression() {
        std::string n;
        if (Take("++") || Take("--")) return Identifier(n) && Declared(n) ? true : Fail("Invalid update expression.");
        if (!Identifier(n) || !Declared(n)) return Fail("A loop update must change a declared variable.");
        if (Take("++") || Take("--")) return true;
        if (Take("=") || Take("+=") || Take("-=") || Take("*=") || Take("/=") || Take("%=")) return Expression();
        return Fail("Expected an assignment or increment in the loop update.");
    }
    bool Declaration(bool semicolon) {
        if (!(Take("int") || Take("bool") || Take("char"))) return false;
        std::string n;
        if (!Identifier(n) || !AddVariable(n)) return Fail("Expected a new variable name.");
        if (Take("=") && !Expression()) return false;
        if (semicolon && !Take(";")) return Fail("Expected ';' after variable declaration.");
        return true;
    }
    bool SimpleStatement(bool semicolon) {
        if (pos >= tokens.size() || !tokens[pos].identifier) return Fail("Expected a variable assignment or drawCell call.");
        std::string n = tokens[pos].text;
        if (n == "std") {
            if (!allowCout) return Fail("std::cout is only available in Print Output mode.");
            ++pos;
            if (!Take(":") || !Take(":") || !Take("cout") || !Take("<<")) return Fail("Use std::cout << value; in Print Output mode.");
            int itemCount = 0;
            do {
                if (pos >= tokens.size()) return Fail("std::cout needs a printable value.");
                const Token& t = tokens[pos];
                if (!t.identifier && t.text.size() >= 2 && (t.text.front() == '"' || t.text.front() == '\'')) ++pos;
                else if (!t.identifier && !t.text.empty() && std::isdigit((unsigned char)t.text[0])) ++pos;
                else if (t.identifier && (Declared(t.text) || t.text == "true" || t.text == "false")) ++pos;
                else return Fail("std::cout supports only string/character literals, integer literals, and local variables.");
                if (++itemCount > 64) return Fail("A std::cout statement may contain at most 64 values.");
            } while (Take("<<"));
        } else if (n == "drawCell") {
            ++pos;
            if (!Take("(") || !Expression() || !Take(",") || !Expression() || !Take(",") || !Expression() || !Take(")"))
                return Fail("Use drawCell(x, y, colour) with three expressions.");
        } else if (n == "print" || n == "printLine") {
            ++pos;
            if (!Take("(")) return Fail("Use print(value) or printLine(value).");
            if (At(")")) return Fail("Print helpers need one value argument.");
            if (pos < tokens.size() && !tokens[pos].identifier && tokens[pos].text.size() >= 2 &&
                (tokens[pos].text.front() == '"' || tokens[pos].text.front() == '\'')) ++pos;
            else if (!Expression()) return false;
            if (!Take(")")) return Fail("Print helpers accept one value argument.");
        } else {
            ++pos;
            if (!Declared(n)) return Fail("Unknown variable '" + n + "'.");
            if (Take("++") || Take("--")) { }
            else if (Take("=") || Take("+=") || Take("-=") || Take("*=") || Take("/=") || Take("%=")) {
                if (!Expression()) return false;
            } else return Fail("Expected an assignment or increment.");
        }
        return !semicolon || Take(";") || Fail("Expected ';'.");
    }
    bool Statement() {
        if (++statementCount > 600) return Fail("The solution has too many statements.");
        if (Take(";")) return true;
        if (Take("{")) {
            if (!Enter()) return false;
            while (pos < tokens.size() && !At("}")) if (!Statement()) return false;
            if (!Take("}")) return Fail("Expected '}'.");
            Leave(); return true;
        }
        if (At("int") || At("bool") || At("char")) return Declaration(true);
        if (Take("if")) {
            if (!Take("(" ) || !Expression() || !Take(")")) return Fail("Expected if (condition).");
            if (!Statement()) return false;
            if (Take("else") && !Statement()) return false;
            return true;
        }
        if (Take("while")) {
            if (!Take("(") || !Expression() || !Take(")")) return Fail("Expected while (condition).");
            ++loopDepth; bool ok = Statement(); --loopDepth; return ok;
        }
        if (Take("for")) {
            if (!Take("(") || !Enter()) return Fail("Expected for (initialization; condition; update).");
            if (At("int") || At("bool")) { if (!Declaration(true)) return false; }
            else if (!Take(";")) { if (!SimpleStatement(true)) return false; }
            if (!At(";")) { if (!Expression()) return false; }
            if (!Take(";")) return Fail("Expected ';' in for loop.");
            if (!At(")") && !UpdateExpression()) return false;
            if (!Take(")")) return Fail("Expected ')' after for loop.");
            ++loopDepth; bool ok = Statement(); --loopDepth; Leave(); return ok;
        }
        if (Take("break") || Take("continue")) {
            if (!loopDepth) return Fail("break and continue are only allowed inside a loop.");
            return Take(";") || Fail("Expected ';'.");
        }
        return SimpleStatement(true);
    }

public:
    bool Validate(const std::string& src, std::string& why, PracticeRunMode mode = PracticeRunMode::GridDrawing) {
        allowCout = mode == PracticeRunMode::PrintOutput;
        if (!Lex(src)) { why = error; return false; }
        while (pos < tokens.size()) if (!Statement()) { why = error; return false; }
        return true;
    }
};

struct ProcessResult { int code = -1; bool timedOut = false; bool launchFailed = false; std::string output; };

#if defined(_WIN32)
using ProcessArg = std::wstring;
inline ProcessArg Arg(const char* value) { return std::wstring(value, value + std::char_traits<char>::length(value)); }
inline ProcessArg Arg(const std::filesystem::path& value) { return value.native(); }
inline std::filesystem::path WasiSdkPath() {
    const wchar_t* value = _wgetenv(L"WASI_SDK_PATH");
    return value && *value ? std::filesystem::path(value) : std::filesystem::path();
}
#elif defined(__APPLE__) || defined(__linux__)
using ProcessArg = std::string;
inline ProcessArg Arg(const char* value) { return value; }
inline ProcessArg Arg(const std::filesystem::path& value) { return value.native(); }
inline std::filesystem::path WasiSdkPath() {
    const char* value = std::getenv("WASI_SDK_PATH");
    return value && *value ? std::filesystem::path(value) : std::filesystem::path();
}
#endif

#if defined(__APPLE__) || defined(__linux__)
inline ProcessResult RunProcess(const std::filesystem::path& executable, const std::vector<ProcessArg>& args,
    int timeoutMs, const std::filesystem::path& workingDirectory, size_t) {
    ProcessResult result;
    int pipefd[2];
    if (pipe(pipefd) != 0) { result.launchFailed = true; result.output = "Could not create a process output pipe."; return result; }
    posix_spawn_file_actions_t actions;
    posix_spawnattr_t attr;
    posix_spawn_file_actions_init(&actions);
    posix_spawnattr_init(&attr);
    posix_spawn_file_actions_adddup2(&actions, pipefd[1], STDOUT_FILENO);
    posix_spawn_file_actions_adddup2(&actions, pipefd[1], STDERR_FILENO);
    posix_spawn_file_actions_addclose(&actions, pipefd[0]);
    posix_spawn_file_actions_addclose(&actions, pipefd[1]);
    short flags = POSIX_SPAWN_SETPGROUP;
    posix_spawnattr_setflags(&attr, flags);
    posix_spawnattr_setpgroup(&attr, 0);
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>(executable.c_str()));
    for (const ProcessArg& a : args) argv.push_back(const_cast<char*>(a.c_str()));
    argv.push_back(nullptr);
    pid_t pid = -1;
    int spawnError = posix_spawn(&pid, executable.c_str(), &actions, &attr, argv.data(), environ);
    posix_spawn_file_actions_destroy(&actions);
    posix_spawnattr_destroy(&attr);
    close(pipefd[1]);
    if (spawnError != 0) { close(pipefd[0]); result.launchFailed = true; result.output = "Could not start the configured tool."; return result; }
    int oldFlags = fcntl(pipefd[0], F_GETFL, 0);
    fcntl(pipefd[0], F_SETFL, oldFlags | O_NONBLOCK);
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
    bool childDone = false, pipeDone = false, killed = false;
    int status = 0;
    char buf[2048];
    while (!childDone || !pipeDone) {
        struct pollfd pfd{ pipefd[0], POLLIN | POLLHUP, 0 };
        poll(&pfd, 1, 20);
        for (;;) {
            ssize_t n = read(pipefd[0], buf, sizeof(buf));
            if (n > 0) {
                if (result.output.size() + (size_t)n > 65536) {
                    result.output = "Tool output exceeded the 64 KB limit.";
                    kill(-pid, SIGKILL); killed = true;
                } else if (!killed) result.output.append(buf, (size_t)n);
                continue;
            }
            if (n == 0) pipeDone = true;
            break;
        }
        if (!childDone) {
            pid_t w = waitpid(pid, &status, WNOHANG);
            if (w == pid) childDone = true;
        }
        if (!childDone && std::chrono::steady_clock::now() >= deadline) {
            result.timedOut = true; kill(-pid, SIGKILL); killed = true;
            while (waitpid(pid, &status, 0) < 0 && errno == EINTR) { }
            childDone = true;
        }
    }
    close(pipefd[0]);
    (void)workingDirectory;
    if (!killed && WIFEXITED(status)) result.code = WEXITSTATUS(status);
    else if (!killed && WIFSIGNALED(status)) result.code = 128 + WTERMSIG(status);
    return result;
}

inline std::filesystem::path FindOnPath(const char* name) {
    const char* path = std::getenv("PATH"); if (!path) return {};
    std::string dirs(path); size_t start = 0;
    while (start <= dirs.size()) {
        size_t end = dirs.find(':', start);
        std::filesystem::path candidate(dirs.substr(start, end == std::string::npos ? end : end - start));
        candidate /= name;
        std::error_code ec;
        if (std::filesystem::is_regular_file(candidate, ec) && access(candidate.c_str(), X_OK) == 0) return candidate;
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return {};
}
#elif defined(_WIN32)
inline std::wstring QuoteWindowsArgument(const std::wstring& value) {
    std::wstring quoted(1, L'"');
    size_t slashes = 0;
    for (wchar_t ch : value) {
        if (ch == L'\\') { ++slashes; continue; }
        if (ch == L'"') {
            quoted.append(slashes * 2 + 1, L'\\');
            quoted.push_back(L'"');
        } else {
            quoted.append(slashes, L'\\');
            quoted.push_back(ch);
        }
        slashes = 0;
    }
    quoted.append(slashes * 2, L'\\');
    quoted.push_back(L'"');
    return quoted;
}

inline ProcessResult RunProcess(const std::filesystem::path& executable, const std::vector<ProcessArg>& args,
    int timeoutMs, const std::filesystem::path& workingDirectory, size_t memoryLimitBytes) {
    ProcessResult result;
    SECURITY_ATTRIBUTES security{ sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    HANDLE readPipe = nullptr, writePipe = nullptr, nullInput = nullptr;
    HANDLE job = nullptr;
    PROCESS_INFORMATION process{};
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
    SIZE_T attributeBytes = 0;
    std::vector<ULONG_PTR> attributeStorage;
    PPROC_THREAD_ATTRIBUTE_LIST attributes = nullptr;
    HANDLE inherited[2]{};
    STARTUPINFOEXW startup{};
    std::wstring command;
    std::vector<wchar_t> mutableCommand;
    BOOL created = FALSE;
    DWORD flags = 0;
    auto fail = [&](const wchar_t* message, DWORD code = GetLastError()) {
        result.launchFailed = true;
        result.output = std::string("Windows runner: ") + std::string(message, message + wcslen(message)) +
            " (Windows error " + std::to_string(code) + ").";
    };
    if (!CreatePipe(&readPipe, &writePipe, &security, 0)) { fail(L"could not create output pipe"); return result; }
    SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);
    nullInput = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &security,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (nullInput == INVALID_HANDLE_VALUE) { nullInput = nullptr; fail(L"could not open null input"); goto cleanup; }

    job = CreateJobObjectW(nullptr, nullptr);
    if (!job) { fail(L"could not create a process job"); goto cleanup; }
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE |
        JOB_OBJECT_LIMIT_JOB_TIME | JOB_OBJECT_LIMIT_PROCESS_MEMORY | JOB_OBJECT_LIMIT_JOB_MEMORY |
        JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
    limits.BasicLimitInformation.ActiveProcessLimit = 8;
    limits.BasicLimitInformation.PerJobUserTimeLimit.QuadPart = (LONGLONG)timeoutMs * 10000;
    limits.ProcessMemoryLimit = (SIZE_T)memoryLimitBytes;
    limits.JobMemoryLimit = (SIZE_T)memoryLimitBytes;
    if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits))) {
        fail(L"could not configure CPU, memory, and child-process limits"); goto cleanup;
    }

    InitializeProcThreadAttributeList(nullptr, 1, 0, &attributeBytes);
    attributeStorage.resize((attributeBytes + sizeof(ULONG_PTR) - 1) / sizeof(ULONG_PTR));
    attributes = reinterpret_cast<PPROC_THREAD_ATTRIBUTE_LIST>(attributeStorage.data());
    if (!InitializeProcThreadAttributeList(attributes, 1, 0, &attributeBytes)) {
        fail(L"could not initialize process handle restrictions"); goto cleanup;
    }
    inherited[0] = writePipe; inherited[1] = nullInput;
    if (!UpdateProcThreadAttribute(attributes, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
        inherited, sizeof(inherited), nullptr, nullptr)) {
        DeleteProcThreadAttributeList(attributes); fail(L"could not restrict inherited process handles"); goto cleanup;
    }

    startup.StartupInfo.cb = sizeof(startup);
    startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput = nullInput;
    startup.StartupInfo.hStdOutput = writePipe;
    startup.StartupInfo.hStdError = writePipe;
    startup.lpAttributeList = attributes;
    command = QuoteWindowsArgument(executable.native());
    for (const ProcessArg& arg : args) { command.push_back(L' '); command += QuoteWindowsArgument(arg); }
    mutableCommand.assign(command.begin(), command.end());
    mutableCommand.push_back(L'\0');
    flags = CREATE_SUSPENDED | CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT;
    created = CreateProcessW(executable.c_str(), mutableCommand.data(), nullptr, nullptr, TRUE,
        flags, nullptr, workingDirectory.c_str(), &startup.StartupInfo, &process);
    DeleteProcThreadAttributeList(attributes);
    if (!created) { fail(L"could not start the configured tool"); goto cleanup; }
    CloseHandle(writePipe); writePipe = nullptr;
    CloseHandle(nullInput); nullInput = nullptr;
    if (!AssignProcessToJobObject(job, process.hProcess)) {
        fail(L"could not apply Windows job limits; the program was not started");
        TerminateProcess(process.hProcess, ERROR_ACCESS_DENIED);
        WaitForSingleObject(process.hProcess, INFINITE);
        goto cleanup;
    }
    if (ResumeThread(process.hThread) == (DWORD)-1) {
        fail(L"could not resume the limited process");
        TerminateJobObject(job, ERROR_PROCESS_ABORTED);
        WaitForSingleObject(process.hProcess, INFINITE);
        goto cleanup;
    }
    CloseHandle(process.hThread); process.hThread = nullptr;

    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
        bool processDone = false, pipeDone = false, terminated = false;
        char buffer[2048];
        DWORD exitCode = STILL_ACTIVE;
        while (!processDone || !pipeDone) {
            if (!pipeDone) {
                DWORD available = 0;
                if (!PeekNamedPipe(readPipe, nullptr, 0, nullptr, &available, nullptr)) {
                    if (GetLastError() == ERROR_BROKEN_PIPE) pipeDone = true;
                    else { fail(L"could not read tool output"); TerminateJobObject(job, ERROR_READ_FAULT); terminated = true; }
                }
                while (!pipeDone && available > 0) {
                    DWORD got = 0, request = std::min<DWORD>((DWORD)sizeof(buffer), available);
                    BOOL readOk = ReadFile(readPipe, buffer, request, &got, nullptr);
                    if (!readOk || got == 0) {
                        if (!readOk && GetLastError() == ERROR_BROKEN_PIPE) pipeDone = true;
                        break;
                    }
                    if (result.output.size() + got > 65536) {
                        result.launchFailed = true; result.output = "Tool output exceeded the 64 KB limit.";
                        TerminateJobObject(job, ERROR_BUFFER_OVERFLOW); terminated = true;
                    } else if (!terminated) result.output.append(buffer, got);
                    available -= got;
                }
            }
            if (!processDone && WaitForSingleObject(process.hProcess, 0) == WAIT_OBJECT_0) processDone = true;
            if (std::chrono::steady_clock::now() >= deadline && (!processDone || !pipeDone)) {
                result.timedOut = true; TerminateJobObject(job, ERROR_TIMEOUT); terminated = true;
                TerminateProcess(process.hProcess, ERROR_TIMEOUT);
                WaitForSingleObject(process.hProcess, INFINITE); processDone = true; pipeDone = true;
            }
            if (!processDone || !pipeDone) Sleep(10);
        }
        if (!terminated && GetExitCodeProcess(process.hProcess, &exitCode)) {
            result.code = (int)exitCode;
            // Windows reports ERROR_NOT_ENOUGH_QUOTA when a job CPU-time
            // limit terminates the job. Treat it as an execution limit too.
            if (exitCode == ERROR_NOT_ENOUGH_QUOTA) result.timedOut = true;
        }
    }

cleanup:
    if (process.hThread) CloseHandle(process.hThread);
    if (process.hProcess) CloseHandle(process.hProcess);
    if (writePipe) CloseHandle(writePipe);
    if (readPipe) CloseHandle(readPipe);
    if (nullInput) CloseHandle(nullInput);
    if (job) CloseHandle(job); // KILL_ON_JOB_CLOSE also stops any unexpected descendants.
    return result;
}

inline bool CreatePrivateDirectory(const std::filesystem::path& directory, DWORD& error) {
    HANDLE token = nullptr;
    PACL acl = nullptr;
    PSID systemSid = nullptr;
    bool ok = false;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) { error = GetLastError(); return false; }
    DWORD tokenBytes = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &tokenBytes);
    std::vector<ULONG_PTR> tokenBuffer((tokenBytes + sizeof(ULONG_PTR) - 1) / sizeof(ULONG_PTR));
    if (!tokenBytes || !GetTokenInformation(token, TokenUser, tokenBuffer.data(), tokenBytes, &tokenBytes)) {
        error = GetLastError(); goto cleanup;
    }
    {
        auto user = reinterpret_cast<TOKEN_USER*>(tokenBuffer.data());
        acl = (PACL)LocalAlloc(LPTR, 1024);
        SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;
        if (!acl || !InitializeAcl(acl, 1024, ACL_REVISION) ||
            !AllocateAndInitializeSid(&authority, 1, SECURITY_LOCAL_SYSTEM_RID, 0, 0, 0, 0, 0, 0, 0, &systemSid) ||
            !AddAccessAllowedAceEx(acl, ACL_REVISION, OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE, GENERIC_ALL, user->User.Sid) ||
            !AddAccessAllowedAceEx(acl, ACL_REVISION, OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE, GENERIC_ALL, systemSid)) {
            error = GetLastError(); goto cleanup;
        }
        SECURITY_DESCRIPTOR descriptor{};
        if (!InitializeSecurityDescriptor(&descriptor, SECURITY_DESCRIPTOR_REVISION) ||
            !SetSecurityDescriptorDacl(&descriptor, TRUE, acl, FALSE) ||
            !SetSecurityDescriptorControl(&descriptor, SE_DACL_PROTECTED, SE_DACL_PROTECTED)) {
            error = GetLastError(); goto cleanup;
        }
        SECURITY_ATTRIBUTES attributes{ sizeof(SECURITY_ATTRIBUTES), &descriptor, FALSE };
        ok = CreateDirectoryW(directory.c_str(), &attributes) != 0;
        if (!ok) error = GetLastError();
    }
cleanup:
    if (systemSid) FreeSid(systemSid);
    if (acl) LocalFree(acl);
    if (token) CloseHandle(token);
    return ok;
}

inline std::filesystem::path FindOnPath(const wchar_t* name) {
    if (const wchar_t* overridePath = _wgetenv(L"WASMTIME_PATH")) {
        std::filesystem::path candidate(overridePath);
        std::error_code ec;
        if (std::filesystem::is_regular_file(candidate, ec)) return candidate;
    }
    const wchar_t* path = _wgetenv(L"PATH"); if (!path) return {};
    std::wstring dirs(path); size_t start = 0;
    while (start <= dirs.size()) {
        size_t end = dirs.find(L';', start);
        std::filesystem::path candidate(dirs.substr(start, end == std::wstring::npos ? end : end - start));
        candidate /= name;
        std::error_code ec;
        if (std::filesystem::is_regular_file(candidate, ec)) return candidate;
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    return {};
}
#endif

inline std::filesystem::path FindWasmtime() {
#if defined(_WIN32)
    return FindOnPath(L"wasmtime.exe");
#elif defined(__APPLE__) || defined(__linux__)
    return FindOnPath("wasmtime");
#else
    return {};
#endif
}

inline std::filesystem::path CreatePrivateTempDirectory(std::string& error) {
#if defined(_WIN32)
    std::vector<wchar_t> tempPath(32768);
    DWORD n = GetTempPathW((DWORD)tempPath.size(), tempPath.data());
    if (!n || n >= tempPath.size()) { error = "Windows could not find a usable per-user temporary folder."; return {}; }
    std::random_device random;
    for (int attempt = 0; attempt < 32; ++attempt) {
        std::wstringstream name;
        name << L"JavaQuestPractice-" << GetCurrentProcessId() << L'-' << std::hex
             << random() << random() << random() << random();
        std::filesystem::path candidate = std::filesystem::path(tempPath.data()) / name.str();
        DWORD createError = ERROR_SUCCESS;
        if (CreatePrivateDirectory(candidate, createError)) return candidate;
        if (createError == ERROR_ALREADY_EXISTS) continue;
        error = "Windows could not create a private practice temporary folder (error " +
            std::to_string(createError) + ").";
        return {};
    }
    error = "Windows could not allocate a unique practice temporary folder.";
    return {};
#elif defined(__APPLE__) || defined(__linux__)
    std::string pattern = (std::filesystem::temp_directory_path() / "javaquest-practice-XXXXXX").string();
    std::vector<char> temp(pattern.begin(), pattern.end()); temp.push_back('\0');
    char* directory = mkdtemp(temp.data());
    if (!directory) { error = "Could not create a private temporary build folder."; return {}; }
    return std::filesystem::path(directory);
#else
    error = "The isolated process adapter is not implemented on this platform.";
    return {};
#endif
}

} // namespace practice_runner_detail

// Language adapters own the submission-specific details. The Practice screen
// consumes one active adapter; only implemented adapters are registered below.
class PracticeLanguageRunner {
public:
    virtual ~PracticeLanguageRunner() = default;
    virtual const std::string& LanguageName() const = 0;
    virtual const std::string& StarterCode() const = 0;
    virtual const std::string& Instructions() const = 0;
    virtual bool PlatformSupported() const = 0;
    virtual bool Available() const = 0;
    virtual std::string SetupMessage() const = 0;
    virtual PracticeRunResult Run(const std::string& source, int side, PracticeRunMode mode = PracticeRunMode::GridDrawing) const = 0;
};

inline bool PracticeGridMatches(const std::vector<unsigned char>& output, const std::vector<unsigned char>& target) {
    return output == target;
}

inline bool ParsePracticeResultRecord(const std::string& record, PracticeRunMode mode, int side, PracticeRunResult& out) {
    std::vector<std::string> fields; size_t fieldStart = 0;
    while (true) { size_t sep = record.find(':', fieldStart); fields.push_back(record.substr(fieldStart, sep == std::string::npos ? sep : sep - fieldStart)); if (sep == std::string::npos) break; fieldStart = sep + 1; }
    if (fields.size() != 8 || fields[0] != "JQRP1") { out.message = "Malformed runner result (header or version)."; return false; }
    auto number = [](const std::string& s, unsigned& n) {
        if (s.empty()) return false; n=0;
        for (unsigned char c : s) { if(c<'0'||c>'9'||n>(0xffffffffu-(c-'0'))/10) return false; n=n*10+(c-'0'); }
        return true;
    };
    unsigned parsedMode=0,bad=0,cellCount=0,textLength=0,overflow=0;
    if(!number(fields[1],parsedMode)||!number(fields[2],bad)||!number(fields[3],cellCount)||!number(fields[4],textLength)||!number(fields[7],overflow)) {
        out.message = "Malformed runner result (invalid numeric field)."; return false;
    }
    size_t expectedCells = mode == PracticeRunMode::GridDrawing ? (size_t)side * side : 0;
    if(parsedMode!=(unsigned)mode||bad>1||overflow>1||cellCount!=expectedCells||textLength>4096||fields[5].size()!=(size_t)cellCount*2||fields[6].size()!=(size_t)textLength*2) {
        out.message = "Malformed runner result (length, mode, or bounds check failed)."; return false;
    }
    auto hexByte=[](const std::string&s,size_t at,unsigned char& value) {
        auto digit=[](char c)->int { if(c>='0'&&c<='9')return c-'0'; if(c>='a'&&c<='f')return c-'a'+10; if(c>='A'&&c<='F')return c-'A'+10; return -1; };
        if(at+1>=s.size())return false; int a=digit(s[at]),b=digit(s[at+1]); if(a<0||b<0)return false; value=(unsigned char)(a*16+b); return true;
    };
    out.cells.resize(cellCount);
    for(size_t i=0;i<cellCount;++i) if(!hexByte(fields[5],i*2,out.cells[i])||out.cells[i]>5) { out.message="Malformed runner result (invalid grid data)."; return false; }
    out.learnerOutput.resize(textLength);
    for(size_t i=0;i<textLength;++i) { unsigned char c=0; if(!hexByte(fields[6],i*2,c)) { out.message="Malformed runner result (invalid output data)."; return false; } out.learnerOutput[i]=(char)c; }
    if(overflow) { out.cells.clear(); out.learnerOutput.clear(); out.message="Learner output exceeded the 4096-byte limit."; return false; }
    if(bad) { out.cells.clear(); out.learnerOutput.clear(); out.message="drawCell used an invalid coordinate or colour."; return false; }
    return true;
}

class CppPracticeRunner final : public PracticeLanguageRunner {
public:
    const std::string& LanguageName() const override {
        static const std::string name = "C++";
        return name;
    }
    const std::string& StarterCode() const override {
        static const std::string starter = "// Write statements inside solution().\n// Available: drawCell(x, y, Colour)\ndrawCell(0, 0, Red);\n";
        return starter;
    }
    const std::string& Instructions() const override {
        static const std::string instructions = "Only the documented practice subset is intended.";
        return instructions;
    }
    bool PlatformSupported() const override {
#if defined(_WIN32) || defined(__APPLE__) || defined(__linux__)
        return true;
#else
        return false;
#endif
    }
    bool Available() const override {
#if defined(_WIN32) || defined(__APPLE__) || defined(__linux__)
        std::filesystem::path sdk = practice_runner_detail::WasiSdkPath();
        if (sdk.empty() || practice_runner_detail::FindWasmtime().empty()) return false;
#if defined(_WIN32)
        std::filesystem::path compiler = sdk / "bin" / "clang++.exe";
#else
        std::filesystem::path compiler = sdk / "bin" / "clang++";
#endif
        std::filesystem::path sysroot = sdk / "share" / "wasi-sysroot";
        std::error_code ec;
#if defined(_WIN32)
        return std::filesystem::is_regular_file(compiler, ec) && std::filesystem::is_directory(sysroot, ec);
#else
        return std::filesystem::is_regular_file(compiler, ec) && access(compiler.c_str(), X_OK) == 0 &&
            std::filesystem::is_directory(sysroot, ec);
#endif
#else
        return false;
#endif
    }

    std::string SetupMessage() const override {
        if (!PlatformSupported()) return "The isolated runner is not available on this platform.";
        std::filesystem::path sdk = practice_runner_detail::WasiSdkPath();
        if (sdk.empty()) return "WASI SDK missing: set WASI_SDK_PATH to its extracted folder.";
#if defined(_WIN32)
        std::filesystem::path compiler = sdk / "bin" / "clang++.exe";
#else
        std::filesystem::path compiler = sdk / "bin" / "clang++";
#endif
        std::error_code ec;
        if (!std::filesystem::is_regular_file(compiler, ec) ||
            !std::filesystem::is_directory(sdk / "share" / "wasi-sysroot", ec))
            return "WASI SDK incomplete: set WASI_SDK_PATH to a complete SDK folder.";
        if (practice_runner_detail::FindWasmtime().empty()) {
#if defined(_WIN32)
            return "Wasmtime missing: add wasmtime.exe to PATH or set WASMTIME_PATH to its full path.";
#else
            return "Wasmtime missing: add it to PATH before starting JavaQuest.";
#endif
        }
        return "WASI SDK and Wasmtime are ready.";
    }

    PracticeRunResult Run(const std::string& source, int side, PracticeRunMode mode = PracticeRunMode::GridDrawing) const override {
        PracticeRunResult out;
#if defined(_WIN32) || defined(__APPLE__) || defined(__linux__)
        if (!Available()) { out.message = SetupMessage(); return out; }
        std::string reason;
        practice_runner_detail::SubsetParser parser;
        if (!parser.Validate(source, reason, mode)) { out.message = "Subset error: " + reason; return out; }

        std::filesystem::path sdk = practice_runner_detail::WasiSdkPath();
#if defined(_WIN32)
        std::filesystem::path compiler = sdk / "bin" / "clang++.exe";
#else
        std::filesystem::path compiler = sdk / "bin" / "clang++";
#endif
        std::filesystem::path wasmtime = practice_runner_detail::FindWasmtime();
        std::filesystem::path dir = practice_runner_detail::CreatePrivateTempDirectory(out.message);
        if (dir.empty()) return out;
        struct Cleanup { std::filesystem::path p; ~Cleanup() { std::error_code ec; std::filesystem::remove_all(p, ec); } } cleanup{ dir };
        std::filesystem::path cpp = dir / "solution.cpp", wasm = dir / "solution.wasm";
        std::ofstream file(cpp, std::ios::binary);
        if (!file) { out.message = "Could not write the temporary solution file."; return out; }
        if (mode == PracticeRunMode::PrintOutput) file << "#include <iostream>\n#include <streambuf>\n";
        file << "extern \"C\" int printf(const char*, ...);\n"
             << "enum { Red=1, Blue=2, Yellow=3, Green=4, Purple=5 };\n"
             << "static int grid[" << (side * side > 0 ? side * side : 1) << "] = {}; static int bad=0; static unsigned calls=0;\n"
             << "static char jout[4096]; static unsigned joutn=0, joutbad=0;\n"
             << "void emitc(char c){ if(joutn<sizeof(jout)) jout[joutn++]=c; else joutbad=1; }\n"
             << "void print(const char*s){ for(unsigned i=0;s[i];++i) emitc(s[i]); }\n"
             << "void print(char c){ emitc(c); }\n"
             << "void print(int v){ char b[16]; unsigned n=0, u=(unsigned)v; if(v<0){emitc('-');u=0u-u;} do{b[n++]=(char)('0'+u%10);u/=10;}while(u&&n<15); while(n)emitc(b[--n]); }\n"
             << "void printLine(const char*s){print(s);emitc('\\n');} void printLine(char c){emitc(c);emitc('\\n');} void printLine(int v){print(v);emitc('\\n');}\n"
             << "void drawCell(int x,int y,int c){ if(++calls>4096){bad=1;return;} if(x<0||y<0||x>=" << side << "||y>=" << side
             << "||c<1||c>5){bad=1;return;} grid[y*" << side << "+x]=c; }\n"
             << "void solution(){\n#line 1 \"Practice solution\"\n" << source << "\n}\n"
             ;
        if (mode == PracticeRunMode::PrintOutput)
            file << "class PracticeBuf: public std::streambuf { protected: int_type overflow(int_type c) override { if(!traits_type::eq_int_type(c,traits_type::eof())) emitc(traits_type::to_char_type(c)); return traits_type::not_eof(c); } std::streamsize xsputn(const char*s,std::streamsize n) override { for(std::streamsize i=0;i<n;++i) emitc(s[i]); return n; } };\n";
        file << "int main(){ ";
        if (mode == PracticeRunMode::PrintOutput) file << "PracticeBuf practiceBuf; std::cout.rdbuf(&practiceBuf); ";
        file << "solution(); ";
        if (mode == PracticeRunMode::PrintOutput) file << "std::cout.flush(); ";
        file << "printf(\"JQRP1:" << (unsigned)mode << ":%d:" << side * side << ":%u:\",bad,joutn);"
             << "for(int i=0;i<" << side * side << ";++i) printf(\"%02x\",grid[i]); printf(\":\");"
             << "for(unsigned i=0;i<joutn;++i) printf(\"%02x\",(unsigned char)jout[i]);"
             << "printf(\":%u\\n\",joutbad); return 0; }\n";
        file.close();
        if (!file) { out.message = "Could not finish writing the temporary solution file."; return out; }

        auto compile = practice_runner_detail::RunProcess(compiler,
            { practice_runner_detail::Arg("-std=c++17"), practice_runner_detail::Arg("-O0"),
              practice_runner_detail::Arg("-fno-exceptions"), practice_runner_detail::Arg("-fno-rtti"),
              practice_runner_detail::Arg("-Werror"), practice_runner_detail::Arg(cpp),
              practice_runner_detail::Arg("-o"), practice_runner_detail::Arg(wasm) },
            12000, dir, (size_t)1024 * 1024 * 1024);
        if (compile.timedOut) { out.message = "Compilation took too long and was stopped."; return out; }
        if (compile.launchFailed) { out.message = compile.output; return out; }
        if (compile.code != 0) {
            out.compileError = true;
            out.message = compile.output.empty() ? "The C++ subset did not compile; the compiler returned no diagnostic." : compile.output;
            return out;
        }
        auto run = practice_runner_detail::RunProcess(wasmtime,
            { practice_runner_detail::Arg("run"), practice_runner_detail::Arg("-C"), practice_runner_detail::Arg("cache=n"),
              practice_runner_detail::Arg("-W"), practice_runner_detail::Arg("fuel=10000000"),
              practice_runner_detail::Arg("-W"), practice_runner_detail::Arg("max-memory-size=16777216"),
              practice_runner_detail::Arg("-W"), practice_runner_detail::Arg("max-wasm-stack=1048576"),
              practice_runner_detail::Arg("-S"), practice_runner_detail::Arg("inherit-env=n"),
              practice_runner_detail::Arg("-S"), practice_runner_detail::Arg("inherit-network=n"),
              practice_runner_detail::Arg("-S"), practice_runner_detail::Arg("tcp=n"),
              practice_runner_detail::Arg("-S"), practice_runner_detail::Arg("udp=n"),
              practice_runner_detail::Arg("-S"), practice_runner_detail::Arg("http=n"),
              practice_runner_detail::Arg("-S"), practice_runner_detail::Arg("inherit-stdin=n"),
              practice_runner_detail::Arg("-S"), practice_runner_detail::Arg("inherit-stdout=y"),
              practice_runner_detail::Arg("-S"), practice_runner_detail::Arg("inherit-stderr=y"),
              practice_runner_detail::Arg(wasm) },
            5000, dir, (size_t)512 * 1024 * 1024);
        if (run.launchFailed) { out.message = run.output; return out; }
        if (run.timedOut) { out.message = "The runner exceeded its time or process resource limit."; return out; }
        if (run.code != 0) {
            if (run.output.find("fuel consumed") != std::string::npos) out.message = "Program stopped: instruction limit reached.";
            else if (run.output.find("memory") != std::string::npos) out.message = "Program stopped: WebAssembly memory limit reached.";
            else out.message = "Program stopped with a WebAssembly runtime error.";
            return out;
        }

        size_t recordAt = run.output.find("JQRP1:");
        if (recordAt == std::string::npos) { out.message = "The runner returned no valid result record."; return out; }
        out.message = run.output.substr(0, recordAt); // Runtime diagnostics are kept separate from learner output.
        std::string record = run.output.substr(recordAt);
        if (!record.empty() && record.back() == '\n') record.pop_back();
        if (!ParsePracticeResultRecord(record, mode, side, out)) return out;
        out.success = true; if (out.message.empty()) out.message = "Code ran successfully.";
#else
        (void)source; (void)side;
        out.message = "The isolated runner is not available on this platform yet.";
#endif
        return out;
    }
};

inline const CppPracticeRunner& CppPracticeLanguage() {
    static const CppPracticeRunner runner;
    return runner;
}

inline const std::vector<const PracticeLanguageRunner*>& RegisteredPracticeLanguages() {
    static const std::vector<const PracticeLanguageRunner*> runners{ &CppPracticeLanguage() };
    return runners;
}

// There is deliberately no language selector yet. Only the implemented C++
// adapter is active; future adapters can be registered once they actually run.
inline const PracticeLanguageRunner& ActivePracticeLanguage() {
    return *RegisteredPracticeLanguages().front();
}
