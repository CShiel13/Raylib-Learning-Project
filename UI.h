#pragma once
// UI.h - dark theme, fonts, animated widgets and particle effects.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include "raylib.h"
#include "rlgl.h"

// ------------------------------------------------------------------ palette
inline const Color kBg{ 14,  19,  31, 255 };
inline const Color kSurface{ 23,  30,  47, 255 };
inline const Color kSurface2{ 33,  42,  65, 255 };
inline const Color kBorder{ 50,  62,  92, 255 };
inline const Color kText{ 232, 236, 246, 255 };
inline const Color kMuted{ 139, 151, 181, 255 };
inline const Color kMint{ 62, 224, 176, 255 };
inline const Color kViolet{ 139, 108, 255, 255 };
inline const Color kAmber{ 255, 196,  61, 255 };
inline const Color kCoral{ 255,  92, 110, 255 };
inline const Color kSky{ 77, 171, 255, 255 };
inline const Color kLock{ 72,  82, 112, 255 };

// ------------------------------------------------------------------ view scaling / scrolling state
// The whole UI is laid out on a 1100x700 canvas and scaled to whatever the window is.
inline const float kCanvasW = 1100, kCanvasH = 700;
inline float gScale = 1, gOffX = 0, gOffY = 0;          // canvas -> window transform
inline float gTextScale = 1.10f;                         // global text size boost (readability)
inline bool gInScroll = false;                           // inside a Scroller block?
inline float gScrollY = 0;
inline Rectangle gScrollView{ 0, 0, 0, 0 };

inline void UpdateViewScale() {
    float sw = (float)GetScreenWidth(), sh = (float)GetScreenHeight();
    gScale = std::min(sw / kCanvasW, sh / kCanvasH);
    gOffX = (sw - kCanvasW * gScale) / 2; gOffY = (sh - kCanvasH * gScale) / 2;
}
inline Vector2 MouseCanvas() {                           // mouse in canvas units, ignoring scroll
    Vector2 m = GetMousePosition();
    return { (m.x - gOffX) / gScale, (m.y - gOffY) / gScale };
}
inline Vector2 Mouse() {                                 // mouse in the coordinates content is drawn in
    Vector2 m = MouseCanvas();
    if (gInScroll) {
        if (!CheckCollisionPointRec(m, gScrollView)) return { -9999, -9999 };
        m.y += gScrollY;
    }
    return m;
}
inline void ScissorOn(float x, float y, float w, float h) {
    BeginScissorMode((int)(x * gScale + gOffX), (int)(y * gScale + gOffY), (int)(w * gScale) + 1, (int)(h * gScale) + 1);
}

// ------------------------------------------------------------------ math
inline float Clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }
inline float Lerpf(float a, float b, float t) { return a + (b - a) * t; }
inline float EaseOut(float t) { t = Clamp01(t); return 1 - (1 - t) * (1 - t) * (1 - t); }
inline float EaseOutBack(float t) {
    t = Clamp01(t); const float c1 = 1.70158f, c3 = c1 + 1;
    return 1 + c3 * std::pow(t - 1, 3.0f) + c1 * std::pow(t - 1, 2.0f);
}
inline float Smooth(float cur, float target, float rate, float dt) {   // frame-rate independent easing
    return cur + (target - cur) * (1 - std::exp(-rate * dt));
}
inline Color Mix(Color a, Color b, float t) {
    t = Clamp01(t);
    return Color{ (unsigned char)Lerpf(a.r, b.r, t), (unsigned char)Lerpf(a.g, b.g, t),
                 (unsigned char)Lerpf(a.b, b.b, t), (unsigned char)Lerpf(a.a, b.a, t) };
}
inline Color Al(Color c, float a) { c.a = (unsigned char)(c.a * Clamp01(a)); return c; }
inline bool IsBright(Color c) { return 0.299f * c.r + 0.587f * c.g + 0.114f * c.b > 150; }
inline Color OnColor(Color c) { return IsBright(c) ? kBg : Color{ 255, 255, 255, 255 }; }
inline float Rnd() { return GetRandomValue(0, 10000) / 10000.0f; }

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

// ------------------------------------------------------------------ shapes
inline float RR(Rectangle r, float rad) { return std::min(1.0f, 2 * rad / std::max(1.0f, std::min(r.width, r.height))); }
inline void Box(Rectangle r, float rad, Color fill) { DrawRectangleRounded(r, RR(r, rad), 14, fill); }
inline void BoxB(Rectangle r, float rad, Color fill, Color border, float bw = 2) {
    Box(r, rad, border);
    Rectangle i{ r.x + bw, r.y + bw, r.width - 2 * bw, r.height - 2 * bw };
    Box(i, std::max(1.0f, rad - bw), fill);
}
inline void Shadow(Rectangle r, float rad, float off = 6, float a = 0.45f) {
    Box({ r.x, r.y + off, r.width, r.height }, rad, Al(Color{ 0, 0, 0, 255 }, a));
}
// Raised panel: soft drop shadow, extruded lip underneath, lit top rim.
inline void Card3D(Rectangle r, float rad, Color fill, Color edge, float depth = 6) {
    for (int i = 3; i >= 1; i--) Box({ r.x - i * 2.0f, r.y + depth + i * 3.0f, r.width + i * 4.0f, r.height }, rad + i * 2, Al(Color{ 0, 0, 0, 255 }, 0.10f));
    Box({ r.x, r.y + depth, r.width, r.height }, rad, Mix(edge, kBg, 0.55f));
    Box(r, rad, edge);
    Rectangle i{ r.x + 2, r.y + 2, r.width - 4, r.height - 4 };
    Box(i, std::max(1.0f, rad - 2), Mix(fill, Color{ 255, 255, 255, 255 }, 0.09f));
    Box({ i.x, i.y + 2, i.width, i.height - 2 }, std::max(1.0f, rad - 2), fill);
}
// Sunken panel (code boxes, editors): dark well with an inner top shadow.
inline void Inset(Rectangle r, float rad, Color fill, Color edge, float = 2) {
    Box(r, rad, Mix(edge, Color{ 255, 255, 255, 255 }, 0.06f));
    Rectangle i{ r.x + 2, r.y + 2, r.width - 4, r.height - 4 };
    Box(i, std::max(1.0f, rad - 2), fill);
    Box({ i.x, i.y, i.width, std::min(10.0f, i.height) }, std::max(1.0f, rad - 2), Al(Color{ 0, 0, 0, 255 }, 0.22f));
    Box({ i.x, i.y + 4, i.width, i.height - 4 }, std::max(1.0f, rad - 2), fill);
}
inline void DrawBackground() {
    DrawRectangleGradientV(-3000, -3000, 7100, 6700, Color{ 12, 16, 27, 255 }, Color{ 22, 28, 46, 255 });
    DrawCircleV({ 120, 90 }, 420, Al(kViolet, 0.045f));
    DrawCircleV({ 1000, 640 }, 460, Al(kMint, 0.035f));
}
inline Rectangle ScaleRect(Rectangle r, float s) {
    float w = r.width * s, h = r.height * s;
    return { r.x + (r.width - w) / 2, r.y + (r.height - h) / 2, w, h };
}
inline void Tri(Vector2 a, Vector2 b, Vector2 c, Color col) { DrawTriangle(a, b, c, col); DrawTriangle(a, c, b, col); }

inline void Bar(Rectangle r, float frac, Color c) {
    Box(r, r.height / 2, kSurface2);
    frac = Clamp01(frac);
    if (frac <= 0.001f) return;
    Rectangle f{ r.x, r.y, std::max(r.height, r.width * frac), r.height };
    Box(f, r.height / 2, c);
    Box({ f.x + 4, f.y + 2, std::max(2.0f, f.width - 8), r.height * 0.28f }, r.height * 0.14f, Al(Color{ 255, 255, 255, 255 }, 0.28f));
}

enum IconType { IC_COIN, IC_BULB, IC_BOLT, IC_CHECK, IC_CROSS, IC_LOCK, IC_FLAME, IC_STAR };
inline void Icon(int type, float cx, float cy, float s, Color c) {
    switch (type) {
    case IC_COIN:
        DrawCircleV({ cx, cy }, s, c); DrawCircleV({ cx, cy }, s * 0.68f, Mix(c, kBg, 0.3f)); DrawCircleV({ cx, cy }, s * 0.48f, c); break;
    case IC_BULB:
        DrawCircleV({ cx, cy - s * 0.15f }, s * 0.75f, c);
        Box({ cx - s * 0.38f, cy + s * 0.45f, s * 0.76f, s * 0.5f }, s * 0.15f, Mix(c, kBg, 0.35f)); break;
    case IC_BOLT:
        Tri({ cx + s * 0.3f, cy - s }, { cx - s * 0.55f, cy + s * 0.2f }, { cx + s * 0.1f, cy + s * 0.2f }, c);
        Tri({ cx - s * 0.3f, cy + s }, { cx + s * 0.55f, cy - s * 0.2f }, { cx - s * 0.1f, cy - s * 0.2f }, c); break;
    case IC_CHECK:
        DrawLineEx({ cx - s * 0.7f, cy }, { cx - s * 0.2f, cy + s * 0.55f }, s * 0.35f, c);
        DrawLineEx({ cx - s * 0.2f, cy + s * 0.55f }, { cx + s * 0.75f, cy - s * 0.55f }, s * 0.35f, c); break;
    case IC_CROSS:
        DrawLineEx({ cx - s * 0.6f, cy - s * 0.6f }, { cx + s * 0.6f, cy + s * 0.6f }, s * 0.35f, c);
        DrawLineEx({ cx + s * 0.6f, cy - s * 0.6f }, { cx - s * 0.6f, cy + s * 0.6f }, s * 0.35f, c); break;
    case IC_LOCK:
        Box({ cx - s * 0.7f, cy - s * 0.05f, s * 1.4f, s * 1.0f }, s * 0.2f, c);
        DrawRing({ cx, cy - s * 0.15f }, s * 0.38f, s * 0.6f, 180, 360, 16, c); break;
    case IC_FLAME:
        DrawCircleV({ cx, cy + s * 0.25f }, s * 0.7f, c);
        Tri({ cx - s * 0.6f, cy + s * 0.1f }, { cx + s * 0.6f, cy + s * 0.1f }, { cx + s * 0.1f, cy - s }, c); break;
    case IC_STAR:
        for (int i = 0; i < 5; i++) {
            float a1 = -1.5708f + i * 1.2566f, a2 = a1 + 1.2566f;
            Tri({ cx, cy }, { cx + std::cos(a1) * s, cy + std::sin(a1) * s }, { cx + std::cos(a2) * s, cy + std::sin(a2) * s }, c);
        } break;
    }
}

inline void DrawHeart(float x, float y, float s, Color c) {
    DrawCircleV({ x - s * 0.5f, y }, s * 0.55f, c);
    DrawCircleV({ x + s * 0.5f, y }, s * 0.55f, c);
    Tri({ x - s * 1.02f, y + s * 0.25f }, { x + s * 1.02f, y + s * 0.25f }, { x, y + s * 1.25f }, c);
}

// ------------------------------------------------------------------ particles / effects
struct Particle { Vector2 p, v; float rot, vr, life, max, size; Color c; int kind; };
struct RingFx { Vector2 p; float t, max, r1; Color c; };
struct FloatFx { Vector2 p; float t, max, fs; std::string s; Color c; };
struct FxState { std::vector<Particle> ps; std::vector<RingFx> rs; std::vector<FloatFx> fs; };
inline FxState& Fx() { static FxState s; return s; }

inline void FxBurst(float x, float y, int n) {          // confetti
    y -= gScrollY;
    const Color pal[5] = { kMint, kAmber, kViolet, kSky, kCoral };
    for (int i = 0; i < n; i++) {
        float a = Rnd() * 6.2832f, sp = 140 + Rnd() * 420;
        Particle p{ {x, y}, {std::cos(a) * sp, std::sin(a) * sp - 180}, Rnd() * 360, (Rnd() - 0.5f) * 700,
                   0, 1.1f + Rnd() * 0.9f, 7 + Rnd() * 8, pal[GetRandomValue(0, 4)], 0 };
        Fx().ps.push_back(p);
    }
}
inline void FxSparks(float x, float y, Color c, int n = 10) {
    y -= gScrollY;
    for (int i = 0; i < n; i++) {
        float a = Rnd() * 6.2832f, sp = 60 + Rnd() * 180;
        Fx().ps.push_back({ {x, y}, {std::cos(a) * sp, std::sin(a) * sp}, 0, 0, 0, 0.35f + Rnd() * 0.25f, 3 + Rnd() * 3, c, 1 });
    }
}
inline void FxRing(float x, float y, Color c, float r1 = 60) { Fx().rs.push_back({ {x, y - gScrollY}, 0, 0.45f, r1, c }); }
inline void FxFloat(float x, float y, const std::string& s, Color c, float fs = 26) { Fx().fs.push_back({ {x, y - gScrollY}, 0, 1.0f, fs, s, c }); }

inline void FxUpdate(float dt) {
    FxState& f = Fx();
    for (auto& p : f.ps) {
        p.life += dt; p.rot += p.vr * dt;
        if (p.kind == 0) p.v.y += 900 * dt;
        p.v.x *= (1 - 1.2f * dt); p.p.x += p.v.x * dt; p.p.y += p.v.y * dt;
    }
    f.ps.erase(std::remove_if(f.ps.begin(), f.ps.end(), [](const Particle& p) { return p.life >= p.max; }), f.ps.end());
    for (auto& r : f.rs) r.t += dt;
    f.rs.erase(std::remove_if(f.rs.begin(), f.rs.end(), [](const RingFx& r) { return r.t >= r.max; }), f.rs.end());
    for (auto& t : f.fs) { t.t += dt; t.p.y -= 46 * dt; }
    f.fs.erase(std::remove_if(f.fs.begin(), f.fs.end(), [](const FloatFx& t) { return t.t >= t.max; }), f.fs.end());
}
inline void FxDraw() {
    FxState& f = Fx();
    for (auto& r : f.rs) {
        float k = EaseOut(r.t / r.max);
        DrawRing(r.p, r.r1 * k * 0.9f, r.r1 * k, 0, 360, 40, Al(r.c, 1 - k));
    }
    for (auto& p : f.ps) {
        float a = 1 - Clamp01(p.life / p.max);
        if (p.kind == 0) DrawRectanglePro({ p.p.x, p.p.y, p.size, p.size * 0.5f }, { p.size / 2, p.size / 4 }, p.rot, Al(p.c, a * 1.6f));
        else DrawCircleV(p.p, p.size * a + 1, Al(p.c, a * 1.5f));
    }
    for (auto& t : f.fs) {
        float k = t.t / t.max, a = k < 0.7f ? 1.0f : 1 - (k - 0.7f) / 0.3f;
        float s = t.fs * (0.8f + 0.3f * EaseOutBack(k * 3));
        TxtC(t.s, t.p.x, t.p.y, s, Al(t.c, a));
    }
}

// ------------------------------------------------------------------ animated button
struct BtnAnim { float hover = 0, press = 0; };
inline BtnAnim& BtnState(const std::string& label, Rectangle r) {
    static std::unordered_map<uint64_t, BtnAnim> m;
    uint64_t k = std::hash<std::string>()(label) * 31u + (uint64_t)(int)r.x * 7919u + (uint64_t)(int)r.y * 104729u;
    return m[k];
}
// Returns true on click. 3D key-cap look: hover lifts, press sinks into its base, click emits a ripple.
inline bool Button(Rectangle r, const std::string& label, Color col, bool enabled = true, float fs = 22, bool ghost = false) {
    float dt = GetFrameTime();
    BtnAnim& a = BtnState(label, r);
    Vector2 m = Mouse();
    bool hover = enabled && CheckCollisionPointRec(m, r);
    bool down = hover && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    a.hover = Smooth(a.hover, hover ? 1.0f : 0.0f, 18, dt);
    a.press = Smooth(a.press, down ? 1.0f : 0.0f, 32, dt);
    const float depth = 7;
    float rad = std::min(16.0f, r.height / 2.5f);
    float lift = -2 * a.hover * (1 - a.press) + 5 * a.press;      // face travel: up on hover, down on press
    Rectangle face{ r.x, r.y + lift, r.width, r.height };
    Color base, top, tc;
    if (!enabled) { base = Mix(kSurface2, kBg, 0.4f); top = kSurface2; tc = kLock; }
    else if (ghost) { base = Mix(kBorder, kBg, 0.4f); top = Mix(kSurface, col, 0.10f + 0.12f * a.hover); tc = Mix(kText, col, 0.55f); }
    else { base = Mix(col, Color{ 0, 0, 0, 255 }, 0.42f); top = Mix(col, Color{ 255, 255, 255, 255 }, 0.10f * a.hover); tc = OnColor(col); }
    if (enabled) {
        Shadow({ r.x, r.y + 2, r.width, r.height }, rad, depth, 0.30f);
        if (a.hover > 0.02f) Box({ r.x - 5 * a.hover, r.y - 5 * a.hover, r.width + 10 * a.hover, r.height + 10 * a.hover }, rad + 4, Al(col, 0.20f * a.hover));
    }
    Box({ r.x, r.y + depth, r.width, r.height }, rad, base);                  // extruded side
    if (ghost && enabled) {
        BoxB(face, rad, top, Mix(kBorder, col, 0.45f + 0.55f * a.hover), 2);
    }
    else {
        Box(face, rad, enabled ? Mix(col, Color{ 255, 255, 255, 255 }, 0.20f) : kBorder);   // lit rim
        Box({ face.x, face.y + 2, face.width, face.height - 2 }, rad, enabled ? top : kSurface2);
    }
    if (!label.empty()) {
        std::string l = Fit(label, face.width - 14, fs);
        TxtC(l, face.x + face.width / 2, face.y + (face.height - fs * gTextScale * 1.2f) / 2, fs, tc);
    }
    bool clicked = hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    if (clicked) { FxRing(m.x, m.y, col, 46); FxSparks(m.x, m.y, col, 8); }
    return clicked;
}

// Shared chrome for screens with a title in the 72px top bar.
inline void DrawHeaderBackground() {
    DrawRectangle(-1500, 0, 4100, 72, kSurface);
    DrawRectangle(-1500, 72, 4100, 2, kBorder);
    DrawRectangleGradientV(-1500, 74, 4100, 12, Al(Color{ 0, 0, 0, 255 }, 0.35f), Al(Color{ 0, 0, 0, 255 }, 0.0f));
}
inline bool DrawHeaderBackButton(bool enabled = true) {
    bool clicked = Button({ 24, 14, 44, 44 }, "", kSurface2, enabled, 24, true);
    DrawLineEx({ 50, 26 }, { 40, 36 }, 3, kText);
    DrawLineEx({ 40, 36 }, { 50, 46 }, 3, kText);
    return clicked;
}
inline void DrawHeaderTitle(const std::string& title, bool hasBackButton) {
    Txt(Fit(title, 300, 28), hasBackButton ? 84.0f : 76.0f, 18, 28, kText);
}

// ------------------------------------------------------------------ scrolling
// Usage:  float off = sc.Begin(view, contentHeight, dt);  ...draw content at normal coordinates...  sc.End();
// Wheel (vertical or horizontal), touchpad, PageUp/PageDown/Home/End, and a draggable scrollbar all work.
// Inside Begin..End, Mouse() is remapped so hit-testing matches what is drawn.
struct Scroller {
    float cur = 0, target = 0, grab = 0, maxS = 0, contentH = 0;
    bool drag = false, active = false;
    Rectangle view{ 0, 0, 0, 0 };
    void Reset() { cur = target = 0; drag = false; }
    float Begin(Rectangle v, float contentHeight, float dt) {
        view = v; contentH = contentHeight; maxS = std::max(0.0f, contentH - v.height);
        Vector2 m = MouseCanvas();
        bool hover = CheckCollisionPointRec(m, v);
        float wheel = GetMouseWheelMoveV().y;
        if (wheel == 0) wheel = GetMouseWheelMove();
        if (hover && wheel != 0) target -= wheel * 70;
        if (hover) {
            if (IsKeyPressed(KEY_PAGE_DOWN)) target += v.height * 0.8f;
            if (IsKeyPressed(KEY_PAGE_UP)) target -= v.height * 0.8f;
            if (IsKeyPressed(KEY_HOME)) target = 0;
            if (IsKeyPressed(KEY_END)) target = maxS;
        }
        if (maxS > 0) {                                                    // draggable scrollbar
            float th = std::max(44.0f, v.height * v.height / contentH);
            float thumbY = v.y + (v.height - th) * (cur / maxS);
            Rectangle track{ v.x + v.width - 22, v.y, 22, v.height };
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(m, track)) {
                drag = true;
                grab = (m.y >= thumbY && m.y <= thumbY + th) ? m.y - thumbY : th / 2;
            }
            if (drag) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) target = cur = std::max(0.0f, std::min(maxS, (m.y - grab - v.y) / std::max(1.0f, v.height - th) * maxS));
                else drag = false;
            }
        }
        target = std::max(0.0f, std::min(maxS, target));
        if (!drag) cur = Smooth(cur, target, 16, dt);
        if (std::fabs(cur - target) < 0.4f) cur = target;
        gInScroll = true; gScrollY = cur; gScrollView = v; active = true;
        ScissorOn(v.x, v.y, v.width, v.height);
        rlPushMatrix(); rlTranslatef(0, -cur, 0);
        return cur;
    }
    void End() {
        if (!active) return;
        rlPopMatrix(); EndScissorMode();
        gInScroll = false; gScrollY = 0; active = false;
        if (maxS > 0) {                                                    // scrollbar (drawn unscrolled)
            float th = std::max(44.0f, view.height * view.height / contentH);
            float ty = view.y + (view.height - th) * (cur / maxS);
            Vector2 m = MouseCanvas();
            bool near = drag || CheckCollisionPointRec(m, { view.x + view.width - 22, view.y, 22, view.height });
            Box({ view.x + view.width - 12, view.y + 2, 8, view.height - 4 }, 4, Al(Color{ 0, 0, 0, 255 }, near ? 0.35f : 0.18f));
            Box({ view.x + view.width - 12, ty, 8, th }, 4, near ? kSky : kBorder);
        }
    }
};

// ------------------------------------------------------------------ avatar
// Gear unlocks with level: hat L2, glasses L3, scarf L5, cape L7, crown L10.
// hideMask bits (0 hat, 1 glasses, 2 scarf, 3 cape, 4 crown) let the player switch unlocked gear off.
inline Color OutfitColor(int i) {
    static const Color pal[5] = { kViolet, kSky, kMint, kCoral, kAmber };
    return pal[((i % 5) + 5) % 5];
}
inline void DrawAvatar(float cx, float cy, float s, int level, bool happy = true, unsigned hideMask = 0, int outfit = 0) {
    const Color ink{ 30, 34, 48, 255 };
    const Color body = OutfitColor(outfit);
    const Color trim = outfit == 4 ? kCoral : kAmber;          // scarf contrasts with the outfit
    const Color hatC = outfit == 1 ? kViolet : kSky;
    if (level >= 7 && !(hideMask & 8)) Tri({ cx - 48 * s, cy - 5 * s }, { cx + 48 * s, cy - 5 * s }, { cx, cy + 100 * s }, Color{ 190, 50, 90, 255 });
    Box({ cx - 30 * s, cy + 25 * s, 60 * s, 70 * s }, 22 * s, body);
    if (level >= 5 && !(hideMask & 4)) Box({ cx - 34 * s, cy + 22 * s, 68 * s, 12 * s }, 6 * s, trim);
    DrawCircleV({ cx, cy }, 40 * s, Color{ 255, 218, 180, 255 });
    DrawCircleV({ cx - 14 * s, cy - 6 * s }, 5 * s, ink);
    DrawCircleV({ cx + 14 * s, cy - 6 * s }, 5 * s, ink);
    float my = cy + 14 * s;
    if (happy) {
        DrawLineEx({ cx - 13 * s, my - 4 * s }, { cx, my + 4 * s }, 3 * s, ink);
        DrawLineEx({ cx, my + 4 * s }, { cx + 13 * s, my - 4 * s }, 3 * s, ink);
    }
    else DrawLineEx({ cx - 12 * s, my + 4 * s }, { cx + 12 * s, my + 4 * s }, 3 * s, ink);
    if (level >= 3 && !(hideMask & 2)) {
        DrawRing({ cx - 14 * s, cy - 6 * s }, 8 * s, 10 * s, 0, 360, 24, ink);
        DrawRing({ cx + 14 * s, cy - 6 * s }, 8 * s, 10 * s, 0, 360, 24, ink);
        DrawLineEx({ cx - 4 * s, cy - 6 * s }, { cx + 4 * s, cy - 6 * s }, 2 * s, ink);
    }
    bool crown = level >= 10 && !(hideMask & 16);
    if (crown) {
        DrawRectangleRec({ cx - 28 * s, cy - 52 * s, 56 * s, 14 * s }, kAmber);
        for (int i = -1; i <= 1; i++)
            Tri({ cx + i * 22 * s - 8 * s, cy - 52 * s }, { cx + i * 22 * s + 8 * s, cy - 52 * s }, { cx + i * 22 * s, cy - 72 * s }, kAmber);
    }
    else if (level >= 2 && !(hideMask & 1)) {
        DrawRectangleRec({ cx - 44 * s, cy - 40 * s, 88 * s, 9 * s }, hatC);
        DrawRectangleRec({ cx - 26 * s, cy - 72 * s, 52 * s, 34 * s }, hatC);
    }
}
