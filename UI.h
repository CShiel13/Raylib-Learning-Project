#pragma once
// UI.h - small immediate-mode widgets + the character avatar.
#include <string>
#include "raylib.h"

inline const Color cBG    {247, 247, 250, 255};
inline const Color cText  { 60,  60,  70, 255};
inline const Color cSoft  {130, 130, 140, 255};
inline const Color cLine  {222, 222, 228, 255};
inline const Color cGreen { 88, 204,   2, 255};
inline const Color cBlue  { 28, 176, 246, 255};
inline const Color cRed   {255,  75,  75, 255};
inline const Color cGold  {255, 200,   0, 255};
inline const Color cLock  {200, 200, 206, 255};

// Duolingo-style chunky button. Returns true on click.
inline bool Button(Rectangle r, const char* label, Color col, bool enabled = true, int fs = 22) {
    Vector2 m = GetMousePosition();
    bool hover = enabled && CheckCollisionPointRec(m, r);
    bool down  = hover && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    Color face = enabled ? col : cLock;
    float off = down ? 3.0f : 0.0f;
    DrawRectangleRounded({r.x, r.y + 4, r.width, r.height}, 0.3f, 8, ColorBrightness(face, -0.25f));
    DrawRectangleRounded({r.x, r.y + off, r.width, r.height}, 0.3f, 8,
                         (hover && !down) ? ColorBrightness(face, 0.08f) : face);
    int tw = MeasureText(label, fs);
    DrawText(label, (int)(r.x + (r.width - tw) / 2), (int)(r.y + off + (r.height - fs) / 2), fs,
             enabled ? WHITE : Color{150, 150, 156, 255});
    return hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

inline void TextCentered(const char* t, int cx, int y, int fs, Color c) {
    DrawText(t, cx - MeasureText(t, fs) / 2, y, fs, c);
}

// Word-wrapped text. Returns the pixel height used.
inline int DrawWrapped(const std::string& text, int x, int y, int maxW, int fs, Color c) {
    int lineH = fs + 6, cy = y;
    std::string line, word;
    auto pushWord = [&]() {
        if (word.empty()) return;
        std::string t = line.empty() ? word : line + " " + word;
        if (!line.empty() && MeasureText(t.c_str(), fs) > maxW) {
            DrawText(line.c_str(), x, cy, fs, c);
            cy += lineH;
            line = word;
        } else line = t;
        word.clear();
    };
    for (char ch : text) {
        if (ch == ' ') pushWord();
        else if (ch == '\n') { pushWord(); DrawText(line.c_str(), x, cy, fs, c); cy += lineH; line.clear(); }
        else word += ch;
    }
    pushWord();
    if (!line.empty()) { DrawText(line.c_str(), x, cy, fs, c); cy += lineH; }
    return cy - y;
}

// Triangle that doesn't care about winding order.
inline void Tri(Vector2 a, Vector2 b, Vector2 c, Color col) {
    DrawTriangle(a, b, c, col);
    DrawTriangle(a, c, b, col);
}

inline void DrawHeart(float x, float y, float s, Color c) {
    DrawCircleV({x - s * 0.5f, y}, s * 0.55f, c);
    DrawCircleV({x + s * 0.5f, y}, s * 0.55f, c);
    Tri({x - s * 1.02f, y + s * 0.25f}, {x + s * 1.02f, y + s * 0.25f}, {x, y + s * 1.25f}, c);
}

// The player's character. Items unlock as the player levels up:
//   L2 hat, L3 glasses, L5 scarf, L7 cape, L10 crown
inline void DrawAvatar(float cx, float cy, float s, int level, bool happy = true) {
    if (level >= 7)  // cape
        Tri({cx - 48 * s, cy - 5 * s}, {cx + 48 * s, cy - 5 * s}, {cx, cy + 100 * s}, Color{200, 50, 70, 255});
    DrawRectangleRounded({cx - 30 * s, cy + 25 * s, 60 * s, 70 * s}, 0.4f, 8, Color{88, 204, 2, 255});
    if (level >= 5)  // scarf
        DrawRectangleRounded({cx - 34 * s, cy + 22 * s, 68 * s, 12 * s}, 0.5f, 6, Color{255, 120, 40, 255});
    DrawCircleV({cx, cy}, 40 * s, Color{255, 224, 189, 255});               // head
    DrawCircleV({cx - 14 * s, cy - 6 * s}, 5 * s, Color{40, 40, 50, 255});  // eyes
    DrawCircleV({cx + 14 * s, cy - 6 * s}, 5 * s, Color{40, 40, 50, 255});
    float my = cy + 14 * s;                                                  // mouth
    if (happy) {
        DrawLineEx({cx - 13 * s, my - 4 * s}, {cx, my + 4 * s}, 3 * s, Color{40, 40, 50, 255});
        DrawLineEx({cx, my + 4 * s}, {cx + 13 * s, my - 4 * s}, 3 * s, Color{40, 40, 50, 255});
    } else {
        DrawLineEx({cx - 12 * s, my + 3 * s}, {cx + 12 * s, my + 3 * s}, 3 * s, Color{40, 40, 50, 255});
    }
    if (level >= 3) {  // glasses
        DrawRing({cx - 14 * s, cy - 6 * s}, 8 * s, 10 * s, 0, 360, 24, Color{40, 40, 50, 255});
        DrawRing({cx + 14 * s, cy - 6 * s}, 8 * s, 10 * s, 0, 360, 24, Color{40, 40, 50, 255});
        DrawLineEx({cx - 4 * s, cy - 6 * s}, {cx + 4 * s, cy - 6 * s}, 2 * s, Color{40, 40, 50, 255});
    }
    if (level >= 10) {  // crown
        DrawRectangleRec({cx - 28 * s, cy - 52 * s, 56 * s, 14 * s}, cGold);
        for (int i = -1; i <= 1; i++)
            Tri({cx + i * 22 * s - 8 * s, cy - 52 * s}, {cx + i * 22 * s + 8 * s, cy - 52 * s},
                {cx + i * 22 * s, cy - 72 * s}, cGold);
    } else if (level >= 2) {  // hat
        DrawRectangleRec({cx - 44 * s, cy - 40 * s, 88 * s, 9 * s}, Color{70, 90, 200, 255});
        DrawRectangleRec({cx - 26 * s, cy - 72 * s, 52 * s, 34 * s}, Color{70, 90, 200, 255});
    }
}
