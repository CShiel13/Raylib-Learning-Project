#pragma once
#include <string>
#include <vector>

struct PracticeRunResult {
    bool success = false;
    bool matched = false;
    bool compileError = false;
    std::vector<unsigned char> cells;
    std::string learnerOutput;
    std::string message;
};

enum class PracticeRunMode : unsigned char { GridDrawing = 1, PrintOutput = 2 };
