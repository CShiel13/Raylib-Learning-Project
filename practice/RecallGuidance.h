#pragma once

#include <cctype>
#include <string>
#include "RecallDeck.h"

// Text used by the Recall Reactor UI. Kept independent of raylib and the
// review scheduler so wording can be tested without starting the game.
class RecallGuidance {
public:
    static const char* Name(RecallAnswerKind kind) {
        switch (kind) {
        case RecallAnswerKind::Statement: return "statement";
        case RecallAnswerKind::Condition: return "condition";
        case RecallAnswerKind::Expression: return "expression";
        case RecallAnswerKind::Value: return "value";
        case RecallAnswerKind::LoopClause: return "loop header";
        case RecallAnswerKind::Parameters: return "parameter";
        case RecallAnswerKind::Lambda: return "lambda";
        }
        return "code";
    }

    static const char* Instruction(RecallAnswerKind kind) {
        switch (kind) {
        case RecallAnswerKind::Statement: return "a complete statement, including the final ;";
        case RecallAnswerKind::Condition: return "only the test (no if, parentheses or semicolon)";
        case RecallAnswerKind::Expression: return "only an expression (no trailing semicolon)";
        case RecallAnswerKind::Value: return "only the missing value (no trailing semicolon)";
        case RecallAnswerKind::LoopClause: return "the loop header contents (no for or parentheses)";
        case RecallAnswerKind::Parameters: return "the parameter declaration (no parentheses)";
        case RecallAnswerKind::Lambda: return "the lambda expression (no trailing semicolon)";
        }
        return "the missing C++ code";
    }

    static std::string GapLabel(const RecallCard& card) {
        return "[ " + std::string(Name(card.answerKind)) + " goes here ]";
    }

    static std::string EditorPrompt(const RecallCard& card) {
        return "Type the missing " + std::string(Name(card.answerKind)) + " here...";
    }

    // Useful in docs and tests, while the UI draws the gap with a highlighted
    // rectangle at its exact position to keep existing code alignment.
    static std::string Preview(const RecallCard& card, const std::string& replacement = "") {
        const std::string source = card.context;
        const auto at = source.find("___");
        if (at == std::string::npos) return source;
        return source.substr(0, at) + (replacement.empty() ? GapLabel(card) : replacement) + source.substr(at + 3);
    }

    static bool HasOneGap(const RecallCard& card) {
        const std::string source = card.context;
        const auto at = source.find("___");
        return at != std::string::npos && source.find("___", at + 3) == std::string::npos;
    }

    static std::string CorrectionTip(const RecallCard& card, const std::string& input) {
        const auto first = input.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "Enter some code in the answer box.";
        if (input.find("___") != std::string::npos) return "Type the code that replaces the gap, not the ___ marker.";
        const auto last = input.find_last_not_of(" \t\r\n");
        if (card.answerKind == RecallAnswerKind::Statement && input[last] != ';')
            return "This gap needs a complete statement ending with a semicolon (;).";
        if (card.answerKind != RecallAnswerKind::Statement && input[last] == ';')
            return "The surrounding code already handles the punctuation. Omit the final ;";
        if (card.answerKind == RecallAnswerKind::Condition && input.compare(first, 2, "if") == 0 &&
            (first + 2 == input.size() || !std::isalnum(static_cast<unsigned char>(input[first + 2]))))
            return "Type only what goes between the if parentheses, not the entire if statement.";
        return "Compare the operators and variable names with the working answer below.";
    }
};
