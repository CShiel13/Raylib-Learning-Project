#pragma once

#include <algorithm>
#include <string>
#include "RecallInputBuffer.h"
#include "../ui/Theme.h"
#include "../ui/Viewport.h"
#include "../ui/Text.h"
#include "../ui/Shapes.h"

// One editable answer displayed inside the missing C++ code gap.
// Rendering and text entry are separate from question/scheduler dependencies.
class RecallInlineField {
    RecallInputBuffer buffer_;

    struct Visible {
        std::string text;
        std::size_t start = 0;
        float caretOffset = 0;
    };

    Visible Portion(float available, float fontSize) const {
        const auto& value = buffer_.Text();
        const std::size_t caret = buffer_.Caret();
        std::size_t first = 0;
        // Keep the caret visible even if the answer is longer than the inline gap.
        while (first < caret && TxtW(value.substr(first, caret - first), fontSize, true) > available)
            ++first;
        std::size_t last = first;
        while (last < value.size() &&
               TxtW(value.substr(first, last - first + 1), fontSize, true) <= available)
            ++last;
        return { value.substr(first, last - first), first,
                 TxtW(value.substr(first, caret - first), fontSize, true) };
    }

    void PositionCaretOnClick(Rectangle bounds, float textX, float fs, float available, bool enabled) {
        if (!enabled || !IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
            !CheckCollisionPointRec(Mouse(), bounds)) return;
        const Visible visible = Portion(available, fs);
        if (buffer_.Text().empty()) { buffer_.Home(); return; }
        const float clicked = Mouse().x - textX;
        std::size_t offset = 0;
        while (offset < visible.text.size() &&
               TxtW(visible.text.substr(0, offset + 1), fs, true) < clicked)
            ++offset;
        buffer_.SetCaret(visible.start + offset);
    }

    void DrawText(Rectangle clip, float textX, float textY, float fs,
                  const std::string& placeholder, bool enabled) const {
        const float available = std::max(8.0f, clip.x + clip.width - textX - 6.0f);
        const Visible visible = Portion(available, fs);
        const bool active = enabled;
        ScissorOn(clip.x + 3, clip.y + 2, clip.width - 6, clip.height - 4);
        if (buffer_.Selected() && active) {
            DrawRectangle(static_cast<int>(textX - 1), static_cast<int>(textY - 1),
                          static_cast<int>(std::min(TxtW(visible.text, fs, true) + 4, available)),
                          static_cast<int>(fs * gTextScale + 4), Al(kSky, 0.25f));
        }
        if (buffer_.Text().empty()) {
            Txt(Fit(placeholder, available, fs, true), textX, textY, fs, kMuted, true);
        } else {
            Txt(visible.text, textX, textY, fs, kText, true);
        }
        if (active && static_cast<int>(GetTime() * 2) % 2 == 0) {
            DrawRectangle(static_cast<int>(textX + visible.caretOffset + 1),
                          static_cast<int>(textY - 1), 2, static_cast<int>(fs * gTextScale + 4), kSky);
        }
        EndScissorMode();
    }

public:
    void Clear() { buffer_.Clear(); }
    const std::string& Text() const { return buffer_.Text(); }
    bool HasContent() const { return buffer_.HasContent(); }

    // Width grows as code is typed, but never pushes the suffix out of the code panel.
    float Width(float maxWidth, float fontSize, const std::string& kind) const {
        const float promptWidth = TxtW("type " + kind, fontSize, true) + 21.0f;
        const float wanted = std::max({ 126.0f, promptWidth, TxtW(buffer_.Text(), fontSize, true) + 27.0f });
        return std::min(std::max(54.0f, maxWidth), wanted);
    }

    // Draw in place of "___"; click to move the caret, type without moving away.
    void DrawInline(Rectangle bounds, const std::string& kind, bool enabled) {
        const bool active = enabled;
        BoxB(bounds, 6, Color{ 9, 12, 20, 255 }, active ? kSky : kBorder, 2);
        const float fs = 17.0f;
        const float tx = bounds.x + 8.0f;
        const float available = bounds.width - 17.0f;
        DrawText(bounds, tx, bounds.y + 3.0f, fs, "type " + kind, enabled);
        PositionCaretOnClick(bounds, tx, fs, available, enabled);
    }

    // Called only while a question is unanswered. Enter submits; ordinary text
    // entry, navigation and paste edit the inline answer buffer.
    bool UpdateKeyboard() {
        const bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        auto repeat = [](int key) { return IsKeyPressed(key) || IsKeyPressedRepeat(key); };
        if (ctrl && IsKeyPressed(KEY_A)) buffer_.SelectAll();
        if (ctrl && IsKeyPressed(KEY_V)) {
            if (const char* text = GetClipboardText()) buffer_.Insert(text);
        }
        int ch = GetCharPressed();
        while (ch != 0) {
            if (!ctrl && ch >= 32 && ch <= 126)
                buffer_.Insert(std::string(1, static_cast<char>(ch)));
            ch = GetCharPressed();
        }
        if (repeat(KEY_BACKSPACE)) buffer_.Backspace();
        if (repeat(KEY_DELETE)) buffer_.Delete();
        if (repeat(KEY_LEFT)) buffer_.Left();
        if (repeat(KEY_RIGHT)) buffer_.Right();
        if (IsKeyPressed(KEY_HOME)) buffer_.Home();
        if (IsKeyPressed(KEY_END)) buffer_.End();
        return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
    }
};
