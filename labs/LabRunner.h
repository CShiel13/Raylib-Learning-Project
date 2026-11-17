#pragma once
// Compiles learner code with a WASI-targeted compiler, executes in a restricted
// Wasmtime sandbox and checks observable behaviour against multiple test cases.
// This never compiles or runs learner code as a native host executable.
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "LabBank.h"
#include "../practice/ProcessRunner.h"

namespace questlabs {
struct Result {
    bool available = true;
    bool accepted = false;
    bool compileError = false;
    int passed = 0, total = 0, failedTest = 0;
    std::string message;
};
inline bool RuntimeReady() {
#if defined(_WIN32) || defined(__APPLE__) || defined(__linux__)
    const auto sdk = practice_runner_detail::WasiSdkPath();
    if (sdk.empty() || practice_runner_detail::FindWasmtime().empty()) return false;
#if defined(_WIN32)
    const auto cc = sdk / "bin" / "clang++.exe";
#else
    const auto cc = sdk / "bin" / "clang++";
#endif
    std::error_code ec;
    return std::filesystem::is_regular_file(cc,ec) &&
           std::filesystem::is_directory(sdk / "share" / "wasi-sysroot",ec);
#else
    return false;
#endif
}
inline std::string SetupHelp() {
    return "Labs need WASI SDK + Wasmtime. Set WASI_SDK_PATH and WASMTIME_PATH, then restart the game. See LABS_SETUP.md.";
}
inline std::string Harness(const Lab& lab, const std::string& code) {
    std::ostringstream s;
    s << "#include <cstdio>\n#include <cstring>\n#include <climits>\n#include <cctype>\n"
         "#include <string>\n#include <iostream>\n#include <algorithm>\n#include <cmath>\n#include <sstream>\n#include <vector>\n"
         "#line 1 \"Your lab solution\"\n" << code << "\n"
         "#line 1 \"C++ Quest lab checks\"\nint main(){\n"
         "int passed=0, total=0, firstFail=0;\n";
    if (!lab.expectedOutput.empty()) {
        s << "std::printf(\"JQLAB_OUTPUT_BEGIN\\n\"); std::fflush(stdout);\n";
        if (lab.id == 5) s << "drawRightAngledTriangle();\n";
        else if (lab.id == 6) s << "drawIsocelesTriangle();\n";
        else if (lab.id == 7) s << "drawIsocelesTriangle2();\n";
        else if (lab.id == 19) s << "int grid[4][6]={{0,0,3,1,3,4},{0,0,2,3,4,3},{0,0,1,3,3,2},{0,0,1,1,1,1}};PrintArray(grid);\n";
        s << "std::cout.flush();std::fflush(stdout);std::printf(\"\\nJQLAB_OUTPUT_END\\n\");\n";
    } else {
        for (size_t i=0;i<lab.tests.size();++i) {
            s << "++total; if((" << lab.tests[i] << ")) ++passed; else if(!firstFail) firstFail=" << (i+1) << ";\n";
        }
    }
    s << "std::printf(\"JQLAB_RESULT:%d:%d:%d\\n\",passed,total,firstFail);return 0;\n}\n";
    return s.str();
}
inline std::string TrimLine(std::string s) {
    while (!s.empty() && std::isspace((unsigned char)s.back())) s.pop_back();
    size_t begin=0;
    while (begin<s.size() && (s[begin]==' '||s[begin]=='\t')) ++begin;
    return s.substr(begin);
}
inline std::string NormalizePrinted(const std::string& text) {
    std::istringstream in(text);std::string line,out;
    while (std::getline(in,line)) {
        if (!line.empty() && line.back()=='\r') line.pop_back();
        line=TrimLine(line);
        // Normalize whitespace between columns but preserve row boundaries.
        std::istringstream words(line); std::string word, row;
        while (words>>word) { if(!row.empty())row+=' '; row+=word; }
        if (!row.empty()) out+=row+'\n';
    }
    return out;
}
inline bool ParseOutput(const Lab& lab,const std::string& output,Result& result){
    const std::string marker="JQLAB_RESULT:";
    auto pos=output.rfind(marker);
    if(pos==std::string::npos){ result.message="No valid lab results returned. Check for a runtime error.";return false; }
    int pass=-1,total=-1,first=-1;
    if (std::sscanf(output.c_str()+pos,"JQLAB_RESULT:%d:%d:%d",&pass,&total,&first)!=3 ||
        pass<0 || total<0 || pass>total || total>100 || first<0 || first>total) {
        result.message="The test result could not be interpreted.";return false;
    }
    if (!lab.expectedOutput.empty()) {
        const std::string start="JQLAB_OUTPUT_BEGIN\n",end="JQLAB_OUTPUT_END";
        auto a=output.find(start), b=a==std::string::npos?std::string::npos:output.find(end,a+start.size());
        if (a==std::string::npos || b==std::string::npos) {
            result.message="Expected printed output was not found.";return false;
        }
        const auto actual=NormalizePrinted(output.substr(a+start.size(),b-a-start.size()));
        const auto expected=NormalizePrinted(lab.expectedOutput);
        result.total=1;result.passed=actual==expected?1:0;result.failedTest=actual==expected?0:1;
        if (result.passed==0) result.message="Printed pattern does not match the example. Check each row and newline.";
    } else {
        if(total!=(int)lab.tests.size()){result.message="Unexpected test count returned.";return false;}
        result.passed=pass;result.total=total;result.failedTest=first;
        if(pass!=total) result.message="Check "+std::to_string(first)+" failed. Revisit the edge cases and try again.";
    }
    result.accepted=result.passed==result.total && result.total>0;
    if(result.accepted)result.message="All "+std::to_string(result.total)+" checks passed. Great work!";
    return true;
}
inline Result Run(const Lab& lab,const std::string& code) {
    Result result;
    if (code.size()>24000){ result.message="The submission is too long (24 KB limit).";return result; }
    if (lab.group==4 && code.find('[')!=std::string::npos) {
        result.message="Pointer lab: use pointer arithmetic, not bracket indexing. Remove any [ ] indexing.";return result;
    }
    if (!RuntimeReady()){ result.available=false;result.message=SetupHelp();return result; }
#if defined(_WIN32) || defined(__APPLE__) || defined(__linux__)
    std::string error;
    const auto dir=practice_runner_detail::CreatePrivateTempDirectory(error);
    if (dir.empty()){result.message=error;return result;}
    struct Cleanup{std::filesystem::path path;~Cleanup(){std::error_code ec;std::filesystem::remove_all(path,ec);}} cleanup{dir};
    const auto src=dir/"lab.cpp",wasm=dir/"lab.wasm";
    {std::ofstream out(src,std::ios::binary);if(!out){result.message="Could not prepare lab source.";return result;}out<<Harness(lab,code);}
    const auto sdk=practice_runner_detail::WasiSdkPath();
#if defined(_WIN32)
    const auto compiler=sdk/"bin"/"clang++.exe";
#else
    const auto compiler=sdk/"bin"/"clang++";
#endif
    const auto cc=practice_runner_detail::RunProcess(compiler,
        {practice_runner_detail::Arg("-std=c++17"),practice_runner_detail::Arg("-O0"),
         practice_runner_detail::Arg(src),practice_runner_detail::Arg("-o"),practice_runner_detail::Arg(wasm)},
         12000,dir,(size_t)1024*1024*1024);
    if(cc.timedOut){result.message="Compilation timed out.";return result;}
    if(cc.launchFailed){result.message=cc.output;return result;}
    if(cc.code!=0){result.compileError=true;result.message=cc.output.empty()?"Compilation failed.":cc.output.substr(0,1600);return result;}
    const auto run=practice_runner_detail::RunProcess(practice_runner_detail::FindWasmtime(),
        {practice_runner_detail::Arg("run"),practice_runner_detail::Arg("-C"),practice_runner_detail::Arg("cache=n"),
         practice_runner_detail::Arg("-W"),practice_runner_detail::Arg("fuel=50000000"),
         practice_runner_detail::Arg("-W"),practice_runner_detail::Arg("max-memory-size=16777216"),
         practice_runner_detail::Arg("-W"),practice_runner_detail::Arg("max-wasm-stack=1048576"),
         practice_runner_detail::Arg("-S"),practice_runner_detail::Arg("inherit-env=n"),
         practice_runner_detail::Arg("-S"),practice_runner_detail::Arg("inherit-network=n"),
         practice_runner_detail::Arg("-S"),practice_runner_detail::Arg("tcp=n"),
         practice_runner_detail::Arg("-S"),practice_runner_detail::Arg("udp=n"),
         practice_runner_detail::Arg("-S"),practice_runner_detail::Arg("http=n"),
         practice_runner_detail::Arg("-S"),practice_runner_detail::Arg("inherit-stdin=n"),
         practice_runner_detail::Arg("-S"),practice_runner_detail::Arg("inherit-stdout=y"),
         practice_runner_detail::Arg("-S"),practice_runner_detail::Arg("inherit-stderr=y"),
         practice_runner_detail::Arg(wasm)},
        5000,dir,(size_t)512*1024*1024);
    if(run.launchFailed){result.message=run.output;return result;}
    if(run.timedOut){result.message="Execution stopped: time limit exceeded.";return result;}
    if(run.code!=0){result.message="Execution stopped: runtime error or instruction/memory limit reached.";return result;}
    ParseOutput(lab,run.output,result);
#endif
    return result;
}
} // namespace questlabs
