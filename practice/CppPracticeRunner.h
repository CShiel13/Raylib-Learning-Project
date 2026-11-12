#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "PracticeLanguageRunner.h"
#include "PracticeResultProtocol.h"
#include "ProcessRunner.h"
#include "SubsetParser.h"

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
            return "Wasmtime missing: add it to PATH before starting C++ Quest.";
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
