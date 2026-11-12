#pragma once
#include <string>
#include "Theme.h"
#include "Viewport.h"

// ------------------------------------------------------------------ fonts / text
inline Font gFont{}, gMono{};
inline bool gFontsReady = false;

inline Font LoadFirstFont(const char* const* paths, int n, int size) {
    for (int i = 0; i < n; i++) {
        if (!FileExists(paths[i])) continue;
        Font f = LoadFontEx(paths[i], size, nullptr, 0);
        if (f.texture.id > 0) { GenTextureMipmaps(&f.texture); SetTextureFilter(f.texture, TEXTURE_FILTER_TRILINEAR); return f; }
    }
    return GetFontDefault();
}
inline void LoadFonts() {   // call after InitWindow. Falls back to raylib's built-in font.
    static const char* ui[] = { "C:/Windows/Fonts/segoeui.ttf", "C:/Windows/Fonts/arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" };
    static const char* mono[] = { "C:/Windows/Fonts/consola.ttf", "C:/Windows/Fonts/cour.ttf",
        "/System/Library/Fonts/Supplemental/Courier New.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf" };
    gFont = LoadFirstFont(ui, 4, 64);
    gMono = LoadFirstFont(mono, 4, 64);
    gFontsReady = true;
}
inline const Font& Fnt(bool mono) { return mono ? gMono : gFont; }

inline float TxtW(const std::string& s, float fs, bool mono = false) {
    fs *= gTextScale;
    return MeasureTextEx(Fnt(mono), s.c_str(), fs, fs * 0.02f).x;
}
inline void Txt(const std::string& s, float x, float y, float fs, Color c, bool mono = false) {
    fs *= gTextScale;
    DrawTextEx(Fnt(mono), s.c_str(), { x, y }, fs, fs * 0.02f, c);
}
inline void TxtC(const std::string& s, float cx, float y, float fs, Color c, bool mono = false) {
    Txt(s, cx - TxtW(s, fs, mono) / 2, y, fs, c, mono);
}
inline void TxtR(const std::string& s, float rx, float y, float fs, Color c, bool mono = false) {
    Txt(s, rx - TxtW(s, fs, mono), y, fs, c, mono);
}
inline std::string Fit(std::string s, float maxW, float fs, bool mono = false) {
    if (TxtW(s, fs, mono) <= maxW) return s;
    while (s.size() > 1 && TxtW(s + "...", fs, mono) > maxW) s.pop_back();
    return s + "...";
}
// Word-wrap. draw=false only measures. Returns the height used.
inline float Wrap(const std::string& text, float x, float y, float maxW, float fs, Color c,
    bool draw = true, bool mono = false, int* linesOut = nullptr, float lh = 1.35f) {
    float lineH = fs * gTextScale * lh, cy = y; int lines = 0;
    std::string line, word;
    auto flush = [&]() { if (draw) Txt(line, x, cy, fs, c, mono); cy += lineH; lines++; };
    auto pushWord = [&]() {
        if (word.empty()) return;
        std::string t = line.empty() ? word : line + " " + word;
        if (!line.empty() && TxtW(t, fs, mono) > maxW) { flush(); line = word; }
        else line = t;
        word.clear();
        };
    for (char ch : text) {
        if (ch == ' ') pushWord();
        else if (ch == '\n') { pushWord(); flush(); line.clear(); }
        else word += ch;
    }
    pushWord();
    if (!line.empty()) flush();
    if (linesOut) *linesOut = lines;
    return cy - y;
}
