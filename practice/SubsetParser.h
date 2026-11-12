#pragma once
#include <cctype>
#include <set>
#include <string>
#include <vector>
#include "PracticeTypes.h"

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
} // namespace practice_runner_detail
