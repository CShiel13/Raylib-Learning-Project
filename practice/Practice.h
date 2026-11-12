#pragma once

// Practice Arcade activities own isolated sessions. They deliberately do not
// reference Player, Player::hints, course progress, or the save file.
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <future>
#include <string>
#include <vector>
#include "../hard/CodeEditor.h"  // Shared editor only; practice has its own runner and grading.
#include "PracticeRunner.h"
#include "PracticeActivity.h"
#include "MiniGames.h"
#include "RecallReactor.h"
#include "../ui/Theme.h"
#include "../ui/Viewport.h"
#include "../ui/Math.h"
#include "../ui/Text.h"
#include "../ui/Shapes.h"
#include "../ui/Effects.h"
#include "../ui/Widgets.h"

inline std::string NormalizePracticeOutput(const std::string& text) {
    std::string normalized;
    normalized.reserve(text.size());
    for (size_t i=0;i<text.size();++i) {
        if (text[i]=='\r') { normalized+='\n'; if(i+1<text.size()&&text[i+1]=='\n') ++i; }
        else normalized+=text[i];
    }
    return normalized;
}
inline bool PracticeTextMatches(const std::string& actual, const std::string& expected) {
    return NormalizePracticeOutput(actual) == NormalizePracticeOutput(expected);
}

class PracticeModeSession : public IPracticeActivity {
    enum CellColour : unsigned char { Empty, Red, Blue, Yellow, Green, Purple };
    struct Puzzle {
        int size = 4;
        unsigned seed = 0;
        int style = 0;
        int colourCount = 1;
        std::vector<unsigned char> cells;
        unsigned char At(int x, int y) const { return cells[y * size + x]; }
    };

    Puzzle puzzle;
    CodeEditor editor;
    unsigned puzzlesSolved = 0;    // Session-only; never serialized.
    unsigned hintsShown = 0;
    unsigned rng = 1;
    unsigned puzzleRevision = 0, runRevision = 0;
    bool started = false;
    bool hasOutput = false;
    bool solvedCurrent = false;
    std::vector<unsigned char> lastOutput;
    std::string runMessage;
    bool runPassed = false;
    bool compileError = false;
    bool runFailed = false;
    bool runActive = false;
    bool confirmNewPuzzle = false;
    std::future<PracticeRunResult> runTask;
    const PracticeLanguageRunner* runner = &ActivePracticeLanguage();
    PracticeRunMode mode;
    std::vector<std::string> textTarget;
    std::string textOutput;
    int textStyle = 0;

    struct CompilerDiagnostic { std::string primary, source, caret; };
    static CompilerDiagnostic FindCompilerDiagnostic(const std::string& message) {
        std::vector<std::string> lines;
        size_t start = 0;
        while (start <= message.size()) {
            size_t end = message.find('\n', start);
            lines.push_back(message.substr(start, end == std::string::npos ? end : end - start));
            if (end == std::string::npos) break;
            start = end + 1;
        }
        CompilerDiagnostic d;
        size_t errorLine = lines.size();
        for (size_t i = 0; i < lines.size(); ++i) if (lines[i].find("error:") != std::string::npos) {
            d.primary = lines[i]; errorLine = i; break;
        }
        auto afterColumn = [](const std::string& line) {
            size_t pipe = line.find('|');
            if (pipe == std::string::npos) return std::string();
            std::string text = line.substr(pipe + 1);
            if (!text.empty() && text[0] == ' ') text.erase(0, 1);
            return text;
        };
        if (errorLine + 1 < lines.size()) d.source = afterColumn(lines[errorLine + 1]);
        if (errorLine + 2 < lines.size()) d.caret = afterColumn(lines[errorLine + 2]);
        if (d.caret.find('^') == std::string::npos) d.caret.clear();
        return d;
    }

    static Color Colour(int c) {
        switch (c) {
        case Red: return Color{ 242, 86, 96, 255 };
        case Blue: return Color{ 74, 154, 250, 255 };
        case Yellow: return Color{ 255, 195, 65, 255 };
        case Green: return Color{ 58, 207, 150, 255 };
        case Purple: return Color{ 166, 115, 255, 255 };
        default: return Color{ 35, 44, 66, 255 };
        }
    }
    static const char* ColourName(int c) {
        switch (c) {
        case Red: return "Red"; case Blue: return "Blue"; case Yellow: return "Yellow";
        case Green: return "Green"; case Purple: return "Purple"; default: return "Empty";
        }
    }
    unsigned NextRandom() {
        rng = rng * 1664525u + 1013904223u;
        return rng;
    }
    void MakePuzzle() {
        ++puzzleRevision;
        puzzle.seed = (unsigned)GetRandomValue(1, 0x7fffffff);
        rng = puzzle.seed;
        puzzle.size = std::min(12, 4 + (int)(puzzlesSolved / 3));
        if (mode == PracticeRunMode::PrintOutput) {
            int complexity = (int)puzzlesSolved;
            if (complexity > 2 && NextRandom() % 5 == 0) complexity = std::max(0, complexity - 2);
            int rows = std::min(9, 5 + complexity / 3);
            textStyle = complexity % 4;
            static const char symbols[] = { '*', '#', '+', '@', '%' };
            char symbol = symbols[NextRandom() % 5];
            textTarget.clear();
            for (int y = 0; y < rows; ++y) {
                int width = textStyle == 0 ? rows : (2 * rows - 1);
                int fill = 0;
                if (textStyle == 0) fill = y + 1;
                else if (textStyle == 1) fill = 2 * y + 1;
                else if (textStyle == 2) fill = 2 * (std::min(y, rows - 1 - y)) + 1;
                else fill = 2 * (rows - 1 - std::min(y, rows - 1 - y)) + 1;
                fill = std::min(width, fill);
                int indent = textStyle == 0 ? 0 : (width - fill) / 2;
                std::string line((size_t)indent, ' ');
                line.append((size_t)fill, symbol);
                textTarget.push_back(line);
            }
            textOutput.clear();
            std::string starter = "// Print with print(\"text\"), print(value), or printLine(value).\n";
            editor.SetText(starter);
        }
        puzzle.style = (int)(puzzlesSolved % 5);
        puzzle.colourCount = std::min(5, 1 + (int)(puzzlesSolved / 2));
        puzzle.cells.assign((size_t)puzzle.size * puzzle.size, Empty);
        int n = puzzle.size, mid = n / 2;
        bool any = false;
        for (int y = 0; y < n; y++) for (int x = 0; x < n; x++) {
            bool filled = false;
            switch (puzzle.style) {
            case 0: filled = x == y || x + y == n - 1; break;                         // diagonals
            case 1: filled = x == mid || y == mid; break;                             // cross
            case 2: filled = x == 0 || y == 0 || x == n - 1 || y == n - 1; break;     // frame
            case 3: filled = (x + y) % 2 == 0 && (x == mid || y == mid || x == y); break;
            default: filled = (NextRandom() % 100) < 38; break;                        // sparse pixels
            }
            if (!filled) continue;
            int c = 1 + (int)(NextRandom() % (unsigned)puzzle.colourCount);
            puzzle.cells[(size_t)y * n + x] = (unsigned char)c;
            any = true;
        }
        if (!any) puzzle.cells[(size_t)mid * n + mid] = Red;
        hintsShown = 0;
        hasOutput = false;
        lastOutput.clear(); solvedCurrent = false; runMessage.clear(); runPassed = false; compileError = false; runFailed = false;
        if (mode == PracticeRunMode::GridDrawing) editor.SetText(runner->StarterCode());
        confirmNewPuzzle = false;
    }

    bool DependenciesReady() const { return runner->Available(); }
    void RequestNewPuzzle() {
        if (runActive) return;
        const std::string starter = mode == PracticeRunMode::GridDrawing ? runner->StarterCode() : "// Print with print(\"text\"), print(value), or printLine(value).\n";
        if (editor.Text() != starter) confirmNewPuzzle = true;
        else NewPuzzle();
    }
    void StartRun() {
        if (!DependenciesReady() || runActive) return;
        std::string submitted = editor.Text();
        const int side = mode == PracticeRunMode::GridDrawing ? puzzle.size : 0;
        runRevision = puzzleRevision;
        runMessage = "Compiling " + runner->LanguageName() + " subset..."; runPassed = false; compileError = false; runFailed = false;
        hasOutput = false; lastOutput.clear(); runActive = true;
        try {
            const PracticeLanguageRunner* selectedRunner = runner;
            PracticeRunMode selectedMode = mode;
            runTask = std::async(std::launch::async, [selectedRunner, submitted = std::move(submitted), side, selectedMode]() {
                try { return selectedRunner->Run(submitted, side, selectedMode); }
                catch (const std::exception& e) { PracticeRunResult r; r.message = std::string("Runner error: ") + e.what(); return r; }
                catch (...) { PracticeRunResult r; r.message = "Runner failed unexpectedly."; return r; }
            });
        } catch (...) { runActive = false; runFailed = true; runMessage = "Could not start the isolated runner process."; }
    }
    void PollRun() {
        if (!runActive || runTask.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) return;
        PracticeRunResult result;
        try { result = runTask.get(); }
        catch (...) { result.message = "Runner failed unexpectedly."; }
        runActive = false;
        if (runRevision != puzzleRevision) return;
        runMessage = result.message;
        compileError = result.compileError;
        runFailed = !result.success;
        if (!result.success) return;
        hasOutput = true; lastOutput = std::move(result.cells); textOutput = std::move(result.learnerOutput);
        if (mode == PracticeRunMode::PrintOutput) {
            std::string expected;
            for (const std::string& row : textTarget) { expected+=row; expected+='\n'; }
            runPassed = PracticeTextMatches(textOutput, expected);
        } else runPassed = PracticeGridMatches(lastOutput, puzzle.cells);
        if (runPassed) {
            if (!solvedCurrent) {
                puzzlesSolved++;
                solvedCurrent = true;
                runMessage = "Puzzle solved! Session progress advanced.";
                FxRing(325, 247, kMint, 92);
                FxBurst(325, 247, 24);
            } else runMessage = "Already solved. Session progress is unchanged.";
        } else {
            runMessage = mode == PracticeRunMode::PrintOutput ? "Text does not match yet. Spaces are significant; visible dots mark spaces." : "Grid does not match yet. You can run it again.";
            FxRing(325, 247, kCoral, 76);
        }
    }

    void DrawGrid(float x, float y, float maxSide, bool target) const {
        float cell = std::min(26.0f, maxSide / puzzle.size);
        TxtC("x", x + cell * puzzle.size * 0.5f, y - 27, 10, kMuted, true);
        Txt("y", x - 14, y - 27, 10, kMuted, true);
        for (int gx = 0; gx < puzzle.size; gx++)
            TxtC(std::to_string(gx), x + (gx + 0.5f) * cell, y - 13, 10, kMuted, true);
        for (int gy = 0; gy < puzzle.size; gy++)
            TxtR(std::to_string(gy), x - 4, y + gy * cell + (cell - 12) * 0.5f, 10, kMuted, true);
        for (int gy = 0; gy < puzzle.size; gy++) for (int gx = 0; gx < puzzle.size; gx++) {
            size_t index = (size_t)gy * puzzle.size + gx;
            int c = target ? puzzle.At(gx, gy) : (hasOutput && index < lastOutput.size() ? lastOutput[index] : Empty);
            Rectangle r{ x + gx * cell, y + gy * cell, cell - 1, cell - 1 };
            DrawRectangleRec(r, Colour(c));
        }
        DrawRectangleLinesEx({ x - 1, y - 1, cell * puzzle.size + 1, cell * puzzle.size + 1 }, 1, kBorder);
    }
    void DrawLegendItem(float x, float y, int colour) const {
        DrawRectangleRec({ x, y + 1, 13, 13 }, Colour(colour));
        DrawRectangleLinesEx({ x, y + 1, 13, 13 }, 1, kBorder);
        Txt(ColourName(colour), x + 19, y - 1, 12, kMuted);
    }
    std::string HintText() const {
        if (mode == PracticeRunMode::PrintOutput) {
            static const char* shapes[] = { "a left-aligned triangle", "a centered pyramid", "a diamond", "an hourglass" };
            if (hintsShown <= 1) return std::string("Hint 1: Print each row with ") + std::to_string(textTarget.size()) + " characters. Spaces are part of the shape.";
            if (hintsShown == 2) return std::string("Hint 2: The pattern is ") + shapes[textStyle] + ".";
            return std::string("Hint ") + std::to_string(hintsShown) + ": Use printLine for each row; the symbol is " + (textTarget.empty()?"*":std::string(1, textTarget[0].find_first_not_of(' ') == std::string::npos ? '*' : textTarget[0][textTarget[0].find_first_not_of(' ')])) + ".";
        }
        if (hintsShown == 0) return "Hint 1: Use loops to visit the grid; draw only the coloured cells.";
        if (hintsShown == 1) {
            static const char* styles[] = { "two diagonals", "a plus-shaped cross", "a border frame", "a sparse pattern", "scattered coloured cells" };
            return std::string("Hint 2: Look for ") + styles[puzzle.style] + ".";
        }
        if (hintsShown == 2) return "Hint 3: This puzzle uses " + std::to_string(puzzle.colourCount) + " colour" + (puzzle.colourCount == 1 ? "." : "s.");
        for (int y = 0; y < puzzle.size; y++) for (int x = 0; x < puzzle.size; x++)
            if (puzzle.At(x, y) != Empty)
                return "Hint 4: One target cell is (" + std::to_string(x) + ", " + std::to_string(y) + ") = " + ColourName(puzzle.At(x, y)) + ".";
        return "The target is blank.";
    }

public:
    explicit PracticeModeSession(PracticeRunMode selectedMode) : mode(selectedMode) {}
    void Start() override {
        if (!started) { started = true; puzzlesSolved = 0; MakePuzzle(); }
        else RequestNewPuzzle();
    }
    void NewPuzzle() { confirmNewPuzzle = false; MakePuzzle(); }
    void ShowHint() { if (hintsShown < 4) hintsShown++; }

    PracticeAction Draw(float dt) override {
        DrawHeaderBackground();
        if (DrawHeaderBackButton(!confirmNewPuzzle)) return PracticeAction::Back;
        DrawHeaderTitle("Endless Practice", true);
        Txt(std::string(mode == PracticeRunMode::GridDrawing ? "Grid Drawing" : "Print Output") + " | Level " + std::to_string(puzzlesSolved / 3 + 1) + " | " + std::to_string(puzzlesSolved) + " solved this session", 84, 47, 14, kMuted);
        if (Button({ 1100 - 24 - 160, 14, 160, 46 }, "New puzzle", kViolet, !runActive && !confirmNewPuzzle, 20, true)) RequestNewPuzzle();

        Rectangle left{ 24, 90, 390, 526 };
        Card3D(left, 20, kSurface, kBorder, 5);
        Txt("Target", 43, 112, 18, kText);
        Txt(mode == PracticeRunMode::PrintOutput ? "Your output" : "Last output", 229, 112, 18, kText);
        float gridSide = 164.0f;
        if (mode == PracticeRunMode::GridDrawing) {
            DrawGrid(57, 165, gridSide, true);
            DrawGrid(243, 165, gridSide, false);
        } else {
            auto visible = [](const std::string& line) { std::string s=line; for(char& c:s) if(c==' ') c='.'; return s; };
            for (size_t i=0;i<textTarget.size()&&i<9;++i) Txt(visible(textTarget[i]), 43, 153+(float)i*19, 12, kText, true);
            std::string displayOutput = NormalizePracticeOutput(textOutput);
            std::vector<std::string> outLines; size_t start=0;
            while(start<=displayOutput.size()) { size_t end=displayOutput.find('\n',start); outLines.push_back(displayOutput.substr(start,end==std::string::npos?end:end-start)); if(end==std::string::npos)break; start=end+1; }
            for (size_t i=0;i<outLines.size()&&i<9;++i) {
                std::string line = outLines[i].size() > 19 ? outLines[i].substr(0,16) + "..." : outLines[i];
                Txt(visible(line), 229, 153+(float)i*19, 12, hasOutput?kText:kMuted, true);
            }
        }
        Txt("Guaranteed solvable", 43, 340, 14, kMint);
        Txt(hasOutput ? (runPassed ? "Correct!" : "Ran") : (runMessage.empty() ? "No run yet" : "No output"),
            229, 340, 14, hasOutput ? (runPassed ? kMint : kMuted) : kMuted);
        DrawLineEx({ 42, 367 }, { 396, 367 }, 1, kBorder);
        if (mode == PracticeRunMode::GridDrawing) {
            Txt(runner->LanguageName() + " subset | colour key", 43, 378, 18, kText);
            DrawLegendItem(43, 407, Red); DrawLegendItem(151, 407, Blue); DrawLegendItem(268, 407, Yellow);
            DrawLegendItem(43, 432, Green); DrawLegendItem(151, 432, Purple);
            Txt("Grid labels: x across, y down; both start at 0.", 43, 461, 12, kMuted);
        } else {
            Txt("Print subset | spaces shown as dots", 43, 378, 17, kText);
            Txt("Compare every character and line exactly.", 43, 407, 12, kMuted);
            Txt("Dots in previews stand for spaces.", 43, 432, 12, kMuted);
        }
        Txt("Unlimited attempts. Practice hints are free.", 43, 490, 13, kSky);
        if (mode == PracticeRunMode::GridDrawing) {
            Txt("No headers or standard library. Write statements", 43, 515, 12, kMuted);
            Txt("inside solution().", 43, 533, 12, kMuted);
        } else {
            Txt("Use print(text/value) or printLine(value).", 43, 515, 12, kMuted);
            Txt("Loops and integer values are supported.", 43, 533, 12, kMuted);
        }
        Txt("Puzzle seed: " + std::to_string(puzzle.seed), 43, 574, 13, kLock);

        Rectangle right{ 430, 90, 646, 526 };
        Card3D(right, 20, kSurface, kBorder, 5);
        Txt("Your " + runner->LanguageName() + " solution", 450, 107, 20, kText);
        Txt(runner->Instructions(), 450, 132, 14, kMuted);
        Rectangle editorView{ 444, 158, 618, 354 };
        if (!confirmNewPuzzle) editor.Update(editorView, !runActive);
        editor.Draw(editorView, !runActive && !confirmNewPuzzle, (float)GetTime());
        if (!confirmNewPuzzle && editor.wantRun) StartRun();
        PollRun();
        Rectangle status{ 444, 522, 618, 88 };
        Color statusColour = (runFailed || !DependenciesReady()) ? kCoral : (runPassed ? kMint : (hasOutput ? kCoral : kAmber));
        Box(status, 14, Al(statusColour, 0.11f));
        Icon(runFailed ? IC_CROSS : IC_BULB, status.x + 24, status.y + 27, 10, statusColour);
        if (!runner->PlatformSupported()) {
            Txt("Runner unavailable on this platform", status.x + 48, status.y + 12, 17, statusColour);
            Txt("The isolated process adapter is not implemented here.", status.x + 48, status.y + 42, 14, kText);
        } else if (!DependenciesReady()) {
            Txt("Runner setup required", status.x + 48, status.y + 12, 17, statusColour);
            Wrap(runner->SetupMessage(), status.x + 48, status.y + 39, status.width - 62, 12, kText, true, false, nullptr, 1.05f);
        } else if (runActive) {
            Txt(runMessage == "Compiling " + runner->LanguageName() + " subset..." ? "Compiling " + runner->LanguageName() + " subset" : "Running in isolated WebAssembly",
                status.x + 48, status.y + 12, 17, statusColour);
            Txt(runMessage, status.x + 48, status.y + 42, 14, kText);
        } else if (runFailed) {
            Txt(compileError ? runner->LanguageName() + " compile error" : "Code could not run", status.x + 48, status.y + 7, 16, statusColour);
            CompilerDiagnostic diagnostic = FindCompilerDiagnostic(runMessage);
            float detailX = status.x + 48, detailY = status.y + 29, detailW = status.width - 62;
            if (!diagnostic.primary.empty()) {
                float used = Wrap(diagnostic.primary, detailX, detailY, detailW, 12, kText, true, false, nullptr, 1.05f);
                if (used < 22 && !diagnostic.source.empty() && TxtW(diagnostic.source, 10, true) <= detailW) {
                    Txt(diagnostic.source, detailX, detailY + 18, 10, kMuted, true);
                    if (!diagnostic.caret.empty() && TxtW(diagnostic.caret, 10, true) <= detailW)
                        Txt(diagnostic.caret, detailX, detailY + 30, 10, kAmber, true);
                }
            } else {
                Wrap(runMessage, detailX, detailY, detailW, 12, kText, true, false, nullptr, 1.05f);
            }
        } else {
            Txt(runMessage.empty() ? runner->LanguageName() + " runner ready" : (runPassed ? "Practice puzzle solved" : "Runner result"), status.x + 48, status.y + 12, 17, statusColour);
            Wrap(runMessage.empty() ? "Run compiles and executes the supported subset." : runMessage,
                status.x + 48, status.y + 42, status.width - 62, 12, kText, true, false, nullptr, 1.05f);
        }

        if (hintsShown > 0) {
            Box({ 44, 623, 580, 42 }, 12, Al(kSky, 0.12f));
            Txt(HintText(), 58, 635, 14, kSky);
        }
        if (Button({ 650, 626, 150, 48 }, hintsShown >= 4 ? "Hints shown" : "Free hint", kSky, hintsShown < 4 && !confirmNewPuzzle, 17, true)) ShowHint();
        bool canRun = runner->PlatformSupported() && DependenciesReady() && !runActive && !confirmNewPuzzle;
        std::string runLabel = runActive ? "Running..." : (!runner->PlatformSupported() ? "Run unavailable" : (DependenciesReady() ? "Run code" : "Run (setup required)"));
        if (Button({ 820, 626, 256, 48 }, runLabel, canRun ? kMint : kLock, canRun, 18, true)) StartRun();

        if (confirmNewPuzzle) {
            DrawRectangle(0, 0, 1100, 700, Al(kBg, 0.72f));
            Rectangle dialog{ 320, 255, 460, 178 };
            Card3D(dialog, 22, kSurface, kSky, 6);
            Txt("Start a new puzzle?", dialog.x + 24, dialog.y + 20, 22, kText);
            Wrap("Your current code will be cleared. Keep editing or discard it to continue.",
                dialog.x + 24, dialog.y + 58, dialog.width - 48, 14, kMuted, true, false, nullptr, 1.2f);
            if (Button({ dialog.x + 24, dialog.y + 116, 190, 46 }, "Keep editing", kSky, true, 16, true)) confirmNewPuzzle = false;
            if (Button({ dialog.x + 238, dialog.y + 116, 198, 46 }, "Discard code", kCoral, true, 16, true)) NewPuzzle();
        }
        (void)dt;
        return PracticeAction::None;
    }
};

// The chooser is outside every activity. Each activity keeps its own session state.
class EndlessPractice {
    PracticeModeSession drawing{ PracticeRunMode::GridDrawing };
    PracticeModeSession printing{ PracticeRunMode::PrintOutput };
    PracticeQuizSession bugHunt{ PracticeMiniGameKind::BugHunt };
    PracticeQuizSession outputOracle{ PracticeMiniGameKind::OutputOracle };
    PracticeQuizSession logicLocks{ PracticeMiniGameKind::LogicLocks };
    RecallReactorSession recallReactor;
    bool choosing = false;
    IPracticeActivity* active = nullptr;

    void DrawActivityCard(Rectangle card, const char* title, const char* description,
                          const char* action, Color accent, IPracticeActivity& activity) {
        Vector2 mouse = Mouse();
        bool hover = CheckCollisionPointRec(mouse, card);
        Card3D(card, 18, Mix(kSurface, kSurface2, hover ? 0.35f : 0.0f),
               Mix(kBorder, accent, hover ? 0.8f : 0.35f), 4);
        Txt(title, card.x + 22, card.y + 18, 23, kText);
        Wrap(description, card.x + 22, card.y + 54, card.width - 44, 14, kMuted,
             true, false, nullptr, 1.18f);
        Rectangle play{ card.x + 22, card.y + card.height - 62, card.width - 44, 44 };
        if (Button(play, action, accent, true, 17, true)) {
            active = &activity;
            active->Start();
            choosing = false;
        }
    }

public:
    void Start() { choosing = true; active = nullptr; }

    PracticeAction Draw(float dt) {
        if (!choosing && active) {
            if (active->Draw(dt) == PracticeAction::Back) {
                active = nullptr;
                choosing = true; // Return to the arcade rather than leaving practice entirely.
            }
            return PracticeAction::None;
        }
        DrawHeaderBackground();
        if (DrawHeaderBackButton()) { choosing = false; return PracticeAction::Back; }
        DrawHeaderTitle("Endless Practice", true);
        Txt("Pick a challenge", 84, 47, 14, kMuted);

        // Six self-contained activities. No activity owns or mutates course progress.
        DrawActivityCard({ 28, 126, 324, 216 }, "Grid Drawing",
            "Recreate coloured patterns with drawCell(x, y, colour).",
            "Play Grid Drawing", kSky, drawing);
        DrawActivityCard({ 388, 126, 324, 216 }, "Print Output",
            "Use loops and print helpers to reproduce text patterns exactly.",
            "Play Print Output", kViolet, printing);
        DrawActivityCard({ 748, 126, 324, 216 }, "Bug Hunt",
            "Spot common C++ mistakes and choose a fix before the bug escapes.",
            "Hunt bugs", kCoral, bugHunt);
        DrawActivityCard({ 28, 372, 324, 216 }, "Output Oracle",
            "Trace variables, loops and conditions. Predict the exact output.",
            "Predict output", kMint, outputOracle);
        DrawActivityCard({ 388, 372, 324, 216 }, "Logic Locks",
            "Combine comparisons, && and || to unlock increasingly tricky doors.",
            "Crack the locks", kAmber, logicLocks);
        DrawActivityCard({ 748, 372, 324, 216 }, "Recall Reactor",
            "Type missing C++ from memory. Charge circuits and revisit skills on later days.",
            "Charge reactor", kMint, recallReactor);
        TxtC("Six activities  |  No course rewards  |  Recall reviews saved separately",
             550, 625, 14, kMuted, true);
        return PracticeAction::None;
    }
};
