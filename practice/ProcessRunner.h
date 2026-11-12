#pragma once
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cwchar>
#include <filesystem>
#include <random>
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
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// Avoid clashes with raylib symbols when this header is used by UI code.
#define CloseWindow WinApiCloseWindow
#define ShowCursor WinApiShowCursor
#define Rectangle WinApiRectangle
#include <windows.h>
#undef CloseWindow
#undef ShowCursor
#undef Rectangle
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

namespace practice_runner_detail {
struct ProcessResult { int code = -1; bool timedOut = false; bool launchFailed = false; std::string output; };

#if defined(_WIN32)
using ProcessArg = std::wstring;
inline ProcessArg Arg(const char* value) { return std::wstring(value, value + std::char_traits<char>::length(value)); }
inline ProcessArg Arg(const std::filesystem::path& value) { return value.native(); }
inline std::wstring ReadEnvironment(const wchar_t* name) {
    wchar_t* value = nullptr;
    size_t length = 0;
    if (_wdupenv_s(&value, &length, name) != 0 || !value) return {};
    std::wstring result(value);
    std::free(value);
    return result;
}
inline std::filesystem::path WasiSdkPath() {
    return std::filesystem::path(ReadEnvironment(L"WASI_SDK_PATH"));
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
    const std::wstring overridePath = ReadEnvironment(L"WASMTIME_PATH");
    if (!overridePath.empty()) {
        std::filesystem::path candidate(overridePath);
        std::error_code ec;
        if (std::filesystem::is_regular_file(candidate, ec)) return candidate;
    }
    std::wstring dirs = ReadEnvironment(L"PATH");
    if (dirs.empty()) return {};
    size_t start = 0;
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
        name << L"CppQuestPractice-" << GetCurrentProcessId() << L'-' << std::hex
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
    std::string pattern = (std::filesystem::temp_directory_path() / "cppquest-practice-XXXXXX").string();
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
