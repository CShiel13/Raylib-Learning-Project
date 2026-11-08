# Raylib-Learning-Project

## Endless Practice

Endless Practice is a standalone activity with Grid Drawing and Print Output
modes. The player chooses a mode before a puzzle starts. Each mode keeps its own
current puzzle, editor, hints, output, solved count, and difficulty in memory;
neither mode writes to the course save or awards course rewards. Grid Drawing
uses beginner colour-grid patterns and `drawCell(x, y, colour)`.

The submission language is a deliberately small C++ subset. A solution is the
body of a provided `solution()` wrapper. It supports
`int`/`bool` local variables, integer and boolean expressions, assignment,
increment/decrement, `for`/`while`, `if`/`else`, `break`/`continue`, blocks, and
`drawCell(x, y, colour)`, plus the bounded `print(value)` and `printLine(value)`
helpers for quoted printable ASCII strings/chars and integer expressions.
Print literals support `\\n`, `\\r`, `\\t`, `\\\\`, and quote escapes. In Print
Output mode, the restricted stream form `std::cout << item << item;` also
accepts printable string/character literals, integer literals, and local
`int`, `bool`, or `char` variables. Use `std::cout << '\\n';` for a newline.
Only this insertion syntax is accepted: headers, other standard-library
access, `std::endl`, `printf`, strings as variables, arrays, pointers, arbitrary
functions, preprocessor directives, file or network access, and unrestricted
C++ remain unsupported. Coordinates are zero-based with `(0, 0)` at the top
left. Colours are `Red`, `Blue`, `Yellow`, `Green`, and `Purple`. The editor
submission is checked against this grammar before it is
passed to the compiler. The generated wrapper uses a C++ `#line` directive so
compiler diagnostics point to the learner's editor line and column. The grid
shows zero-based x/y labels and a colour key. If editor input reaches the
existing 220-character per-line cap, it displays a warning rather than silently
dropping text.
Both `std::cout` and the helper functions write into the same 4096-byte bounded
learner-output buffer. The internal result record is emitted separately after
the solution finishes; output is hex-encoded and length-checked. Overflow is
reported as a failed run, never as truncated matching output.
Print Output uses generated triangles, pyramids, diamonds, and hourglass text
patterns with varied printable symbols. Pattern size grows with that mode's
solved count, with occasional easier puzzles mixed in. `print(value)` appends a
string/character literal or supported integer expression; `printLine(value)`
also appends a newline. Output is compared exactly after CRLF and lone CR are
normalized to LF. Spaces remain significant, and previews show spaces as dots.

The Practice screen uses a language-runner adapter for language-specific
metadata (language name, starter code, instructions, setup availability) and
structured run results. Each mode has an independent in-memory session and
uses the same level rule: puzzle complexity rises once per three puzzles solved
in that mode. C++ is currently the only registered language adapter; mode
selection does not add a language selector.

On macOS and Linux, the game starts the installed WASI compiler and Wasmtime as
separate processes using OS process APIs (no shell). Source is limited to 8 KB,
3000 tokens, 600 statements, and 32 nested blocks. Compilation has a 12-second
limit and bounded captured output. WASM runs with a 10-million instruction fuel
budget, 16 MiB linear-memory limit, 1 MiB stack limit, 5-second wall-clock cap,
no inherited environment, no preopened host directories, and no network
capabilities. Learner print output is buffered separately, limited to 4096
bytes, and never shares the result metadata channel. A versioned `JQRP1`
length-checked record carries grid bytes and hex-encoded learner output; malformed
or oversized records are rejected. Process output remains capped at 64 KB.
WASI file APIs do not receive a host
directory. Run is disabled on platforms without the process adapter. These
limits contain submitted code execution in Wasmtime; compiler processes are
also constrained by a strict source grammar, source and output limits, and a
time limit. The compiler is a trusted installed tool.

Successful output is compared cell-for-cell with the target. A correct puzzle
advances the session-only difficulty for the next puzzle. Practice progress and
hints remain in memory and are not stored in the course save.

### Runner setup on Apple Silicon macOS

The current Apple Silicon setup has been verified with wasi-sdk 34 and
Wasmtime 50, including compilation, linking, and execution of the C++ iostream
stream-buffer probe shown below. To set up another Mac, install the runtime and
SDK outside the game:

1. Install the Wasmtime runtime with Homebrew:

   ```sh
   brew install wasmtime
   ```

2. Download the official `wasi-sdk-34.0-arm64-macos.tar.gz` archive from the
   [WASI SDK releases](https://github.com/WebAssembly/wasi-sdk/releases), then
   unpack it into a directory you control, such as `~/Developer/Toolchains`:

   ```sh
   curl -L "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-34/wasi-sdk-34.0-arm64-macos.tar.gz" -o "$HOME/Downloads/wasi-sdk-34.0-arm64-macos.tar.gz"
   mkdir -p "$HOME/Developer/Toolchains"
   tar -xzf ~/Downloads/wasi-sdk-34.0-arm64-macos.tar.gz -C "$HOME/Developer/Toolchains"
   export WASI_SDK_PATH="$HOME/Developer/Toolchains/wasi-sdk-34.0-arm64-macos"
   export PATH="$WASI_SDK_PATH/bin:$PATH"
   ```

3. Confirm the tools are available:

   ```sh
   "$WASI_SDK_PATH/bin/clang++" --version
   wasmtime --version
   ```

Launch the game from the same Terminal session after setting `WASI_SDK_PATH`
and updating `PATH`, so the game can locate the SDK and Wasmtime. Installing
these dependencies enables Run on macOS. The app itself and course code are
never used to execute a submission.

### Check C++ stream-library support on Linux and Windows

The Linux and Windows WASI SDK/runtime combinations have not been verified in
this development environment. Before enabling Print Output on either platform,
run this check with that platform's installed SDK and Wasmtime. It tests both
linking `iostream` with the game's current flags and running a custom bounded
stream buffer under Wasmtime:

```cpp
#include <iostream>
#include <streambuf>
class ProbeBuffer : public std::streambuf {
protected:
  int_type overflow(int_type c) override { return traits_type::not_eof(c); }
  std::streamsize xsputn(const char*, std::streamsize n) override { return n; }
};
int main() {
  ProbeBuffer buffer;
  std::cout.rdbuf(&buffer);
  std::cout << "probe" << ' ' << 42 << '\n';
  std::cout.flush();
}
```

Save it as `cout_probe.cpp`. On Linux, complete the Linux setup below, set
`WASI_SDK_PATH` and `PATH`, then run:

```sh
"$WASI_SDK_PATH/bin/clang++" -std=c++17 -O0 -fno-exceptions -fno-rtti -Werror cout_probe.cpp -o cout_probe.wasm
wasmtime run -C cache=n -W fuel=10000000 -W max-memory-size=16777216 -W max-wasm-stack=1048576 -S inherit-env=n -S inherit-network=n -S tcp=n -S udp=n -S http=n -S inherit-stdin=n -S inherit-stdout=y -S inherit-stderr=y cout_probe.wasm
```

On Windows, save the same source as `cout_probe.cpp`, set `WASI_SDK_PATH` and
`WASMTIME_PATH` as in the Windows setup, and run these PowerShell commands:

```powershell
& "$env:WASI_SDK_PATH\bin\clang++.exe" -std=c++17 -O0 -fno-exceptions -fno-rtti -Werror cout_probe.cpp -o cout_probe.wasm
& $env:WASMTIME_PATH run -C cache=n -W fuel=10000000 -W max-memory-size=16777216 -W max-wasm-stack=1048576 -S inherit-env=n -S inherit-network=n -S tcp=n -S udp=n -S http=n -S inherit-stdin=n -S inherit-stdout=y -S inherit-stderr=y cout_probe.wasm
```

Both commands should exit successfully and print no output because the probe
redirects `std::cout` to its custom buffer. A compiler/linker failure or a
nonzero Wasmtime exit means that SDK/runtime package is not ready for the
Print Output stream feature; report the tool versions and full diagnostic.

### Runner setup on Linux

Install Wasmtime using the official installation instructions or a package
provided by your Linux distribution. Download the Linux WASI SDK archive for
your machine's architecture from the [WASI SDK releases](https://github.com/WebAssembly/wasi-sdk/releases)
page and extract it to a directory you control. For example, if extracted to
`$HOME/Developer/Toolchains/wasi-sdk`, configure the current shell as follows:

```sh
export WASI_SDK_PATH="$HOME/Developer/Toolchains/wasi-sdk"
export PATH="$WASI_SDK_PATH/bin:$PATH"
"$WASI_SDK_PATH/bin/clang++" --version
wasmtime --version
```

If Wasmtime is not on `PATH`, set `WASMTIME_PATH` to its full executable path
before starting the game. Launch JavaQuest from the same shell so it inherits
these settings. The Linux runner uses the same process isolation and WebAssembly
limits described above. The Windows process adapter and setup steps are below.

### Runner setup on Windows

Download a WASI SDK Windows archive and a Windows Wasmtime binary from their
official release pages: [WASI SDK releases](https://github.com/WebAssembly/wasi-sdk/releases)
and [Wasmtime releases](https://github.com/bytecodealliance/wasmtime/releases).
Extract them to folders you control. Use an SDK build that matches the Windows
machine's CPU architecture and contains `bin\clang++.exe` plus
`share\wasi-sysroot`.

In PowerShell, set the SDK folder and either add Wasmtime's folder to `PATH` or
set `WASMTIME_PATH` to the full `wasmtime.exe` path:

```powershell
$env:WASI_SDK_PATH = "C:\Tools\wasi-sdk"
$env:WASMTIME_PATH = "C:\Tools\wasmtime\wasmtime.exe"
$env:PATH = "$env:WASI_SDK_PATH\bin;$env:PATH"
& "$env:WASI_SDK_PATH\bin\clang++.exe" --version
& $env:WASMTIME_PATH --version
```

Start JavaQuest from that PowerShell session so it inherits these settings. The
Windows adapter launches both tools with `CreateProcessW`, quotes each argument
separately, and assigns each suspended process to a Job Object before resuming
it. The job limits CPU time, memory, and child-process count, and terminates the
whole process tree on timeout or cleanup. Each run gets a uniquely named folder
under the Windows temporary directory with an explicit user-and-System-only
ACL; files are removed after the run.
Wasmtime receives the same fuel, linear-memory, stack, environment, filesystem,
network, and output restrictions as on macOS/Linux. If Windows cannot apply the
Job Object controls, the process is not resumed.
