#pragma once

#include <algorithm>
#include <cstddef>
#include <string>

// Single-line C++ answer and caret. Independent of raylib, rendering and review data.
// Only the inline code gap uses this buffer.
class RecallInputBuffer {
    static constexpr std::size_t kMaximumLength = 100;
    std::string value_;
    std::size_t caret_ = 0;
    bool selected_ = false;

public:
    void Clear() { value_.clear(); caret_ = 0; selected_ = false; }
    const std::string& Text() const { return value_; }
    std::size_t Caret() const { return caret_; }
    bool Selected() const { return selected_; }
    void SelectAll() { selected_ = !value_.empty(); caret_ = value_.size(); }
    bool HasContent() const { return value_.find_first_not_of(" \t\r\n") != std::string::npos; }

    void SetCaret(std::size_t index) { caret_ = std::min(index, value_.size()); selected_ = false; }
    void Home() { caret_ = 0; selected_ = false; }
    void End() { caret_ = value_.size(); selected_ = false; }
    void Left() { if (selected_) { Home(); return; } if (caret_ > 0) --caret_; }
    void Right() { if (selected_) { End(); return; } if (caret_ < value_.size()) ++caret_; }

    void Insert(const std::string& text) {
        if (selected_) Clear();
        for (unsigned char ch : text) {
            if (value_.size() >= kMaximumLength) break;
            // One-line ASCII C++ expressions only; skip newlines/control characters.
            if (ch < 32 || ch > 126) continue;
            value_.insert(caret_, 1, static_cast<char>(ch));
            ++caret_;
        }
    }
    void Backspace() {
        if (selected_) { Clear(); return; }
        if (caret_ == 0) return;
        value_.erase(--caret_, 1);
    }
    void Delete() {
        if (selected_) { Clear(); return; }
        if (caret_ < value_.size()) value_.erase(caret_, 1);
    }
};
