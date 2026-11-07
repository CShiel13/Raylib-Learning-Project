# Raylib-Learning-Project

## Endless Practice

Endless Practice is a standalone C++ drawing activity. Its session state is kept
inside the Practice screen and is not written to the course save file. Puzzle
generation currently creates a non-empty target grid from simple beginner
patterns (diagonals, cross, frame, and sparse colour patterns); every coloured
cell has a direct `drawCell(x, y, colour)` solution. Grid size and palette are
intended to grow with the number solved during the current session.

The submission language is a deliberately small, standard-library-free C++
subset. A solution is the body of a provided `solution()` wrapper. It supports
`int`/`bool` local variables, integer and boolean expressions, assignment,
increment/decrement, `for`/`while`, `if`/`else`, `break`/`continue`, blocks, and
`drawCell(x, y, colour)`. Coordinates are zero-based with `(0, 0)` at the top
left. Colours are `Red`, `Blue`, `Yellow`, `Green`, and `Purple`. Headers,
standard-library access, strings, arrays, pointers, arbitrary functions,
preprocessor directives, file or network access, and unrestricted C++ are not
supported. The editor submission is checked against this grammar before it is
passed to the compiler. The generated wrapper uses a C++ `#line` directive so
compiler diagnostics point to the learner's editor line and column. The grid
shows zero-based x/y labels and a colour key. If editor input reaches the
existing 220-character per-line cap, it displays a warning rather than silently
dropping text.

The Practice screen uses a language-runner adapter for language-specific
metadata (language name, starter code, instructions, setup availability) and
structured run results. Puzzle generation, grid comparison, hints, and
difficulty progression remain language-neutral. C++ is currently the only
registered and offered adapter; there is no language selector until another
runner is implemented and verified.

On macOS and Linux, the game starts the installed WASI compiler and Wasmtime as
separate processes using OS process APIs (no shell). Source is limited to 8 KB,
3000 tokens, 600 statements, and 32 nested blocks. Compilation has a 12-second
limit and bounded captured output. WASM runs with a 10-million instruction fuel
budget, 16 MiB linear-memory limit, 1 MiB stack limit, 5-second wall-clock cap,
no inherited environment, no preopened host directories, and no network
capabilities. Only the wrapper can write the small grid result to standard
output; output is captured and bounded. WASI file APIs do not receive a host
directory. Run is disabled on platforms without the process adapter. These
limits contain submitted code execution in Wasmtime; compiler processes are
also constrained by a strict source grammar, source and output limits, and a
time limit. The compiler is a trusted installed tool.

Successful output is compared cell-for-cell with the target. A correct puzzle
advances the session-only difficulty for the next puzzle. Practice progress and
hints remain in memory and are not stored in the course save.

### Runner setup on Apple Silicon macOS

The current Apple Silicon setup has been verified with wasi-sdk 34 and
Wasmtime 50. To set up another Mac, install the runtime and SDK outside the
game:

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
these dependencies enables Run on macOS/Linux. The app itself and course code
are never used to execute a submission. The Windows process adapter and setup
steps are described below.

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
