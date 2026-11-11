#pragma once
// UIContext.h - single entry point to the app's UI toolkit (the `UI` class).
//
// READ THIS FIRST (humans and AI agents):
//   * Everything is drawn on a fixed 1100x700 "canvas" that UI.h scales to fit the window.
//     Always lay out in canvas units; never use raw window pixels.
//   * Use the global instance `gUI` for all drawing: gUI.Button(...), gUI.Card3D(...), gUI.Text(...).
//   * UI is a thin facade over UI.h. It forwards to the existing inline functions and globals,
//     so old code (Game.h, Practice.h, Content.h) keeps compiling untouched.
//   * Screens follow one pattern:  SomeAction Draw(float dt)  that returns an enum (None = stay).
//
// Frame order:
//   gUI.Init()           once, after InitWindow()   (loads fonts)
//   gUI.Update(dt)       once per frame, before drawing (rescales canvas, advances particles)
//   ...screens draw using gUI...
//   gUI.DrawEffects()    once per frame, last (sparks, rings, floating text)
//
// Scrolling:
//   float off = gUI.scroller.Begin(view, contentHeight, dt);  draw content;  gUI.scroller.End();
//   Inside Begin..End, gUI.Mouse() is remapped so hit-testing matches what is drawn.
#include "UI.h"

class UI {
public:
    // ---------------------------------------------------------------- palette
    struct Palette {
        const Color& bg = kBg;           // window background
        const Color& surface = kSurface;     // cards, header
        const Color& surface2 = kSurface2;   // secondary surfaces
        const Color& border = kBorder;
        const Color& text = kText;
        const Color& muted = kMuted;
        const Color& mint = kMint;         // success / go
        const Color& violet = kViolet;
        const Color& amber = kAmber;       // warning / medium
        const Color& coral = kCoral;       // error / hard
        const Color& sky = kSky;           // info / primary accent
        const Color& lock = kLock;         // disabled
    };
    const Palette colors;

    // ---------------------------------------------------------------- lifecycle
    void Init() { LoadFonts(); }                              // call after InitWindow
    void Update(float dt) { UpdateViewScale(); FxUpdate(dt); }
    void DrawEffects() { FxDraw(); }

    // ---------------------------------------------------------------- canvas / view state
    float CanvasW() const { return kCanvasW; }                // 1100
    float CanvasH() const { return kCanvasH; }                // 700
    float Scale() const { return gScale; }                    // canvas -> window scale
    Vector2 Offset() const { return { gOffX, gOffY }; }       // letterbox offset
    float TextScale() const { return gTextScale; }
    void SetTextScale(float s) { gTextScale = s; }            // global text size multiplier
    bool InScroll() const { return gInScroll; }
    float ScrollY() const { return gScrollY; }
    Vector2 Mouse() const { return ::Mouse(); }               // content coords (scroll-aware)
    Vector2 MouseCanvas() const { return ::MouseCanvas(); }   // canvas coords (ignores scroll)
    void ClipTo(float x, float y, float w, float h) { ScissorOn(x, y, w, h); }  // pair with EndScissorMode()

    Scroller scroller;                                        // see "Scrolling" above

    // ---------------------------------------------------------------- text (canvas units, fs = font size)
    float TextWidth(const std::string& s, float fs, bool mono = false) const { return TxtW(s, fs, mono); }
    void Text(const std::string& s, float x, float y, float fs, Color c, bool mono = false) { Txt(s, x, y, fs, c, mono); }
    void TextCentered(const std::string& s, float cx, float y, float fs, Color c, bool mono = false) { TxtC(s, cx, y, fs, c, mono); }
    void TextRight(const std::string& s, float rx, float y, float fs, Color c, bool mono = false) { TxtR(s, rx, y, fs, c, mono); }
    std::string FitText(const std::string& s, float maxW, float fs, bool mono = false) const { return Fit(s, maxW, fs, mono); }
    // Word wrap. draw=false only measures. Returns the height used.
    float WrapText(const std::string& s, float x, float y, float maxW, float fs, Color c,
        bool draw = true, bool mono = false, int* linesOut = nullptr, float lineHeight = 1.35f) {
        return Wrap(s, x, y, maxW, fs, c, draw, mono, linesOut, lineHeight);
    }

    // ---------------------------------------------------------------- shapes
    void Box(Rectangle r, float radius, Color fill) { ::Box(r, radius, fill); }
    void BoxB(Rectangle r, float radius, Color fill, Color border, float bw = 2) { ::BoxB(r, radius, fill, border, bw); }
    void Shadow(Rectangle r, float radius, float off = 6, float alpha = 0.45f) { ::Shadow(r, radius, off, alpha); }
    void Card3D(Rectangle r, float radius, Color fill, Color edge, float depth = 6) { ::Card3D(r, radius, fill, edge, depth); }
    template <class IconId> void Icon(IconId id, float x, float y, float size, Color c) { ::Icon(id, x, y, size, c); }

    // ---------------------------------------------------------------- widgets
    // Returns true on click. Pass ghost=true for the outlined/secondary style.
    bool Button(Rectangle r, const std::string& label, Color col, bool enabled = true, float fs = 22, bool ghost = false) {
        return ::Button(r, label, col, enabled, fs, ghost);
    }
    // Standard 72px title bar used by every screen.
    void HeaderBackground() { DrawHeaderBackground(); }
    bool HeaderBackButton(bool enabled = true) { return DrawHeaderBackButton(enabled); }   // true on click
    void HeaderTitle(const std::string& title, bool hasBackButton) { DrawHeaderTitle(title, hasBackButton); }

    // ---------------------------------------------------------------- effects (drawn later by DrawEffects)
    void Ring(float x, float y, Color c, float radius = 60) { FxRing(x, y, c, radius); }
    void Sparks(float x, float y, Color c, int count = 8) { FxSparks(x, y, c, count); }
    void FloatText(float x, float y, const std::string& s, Color c, float fs = 26) { FxFloat(x, y, s, c, fs); }

    // ---------------------------------------------------------------- math / colour helpers
    static float Clamp(float v) { return Clamp01(v); }
    static float Lerp(float a, float b, float t) { return Lerpf(a, b, t); }
    static float Ease(float t) { return EaseOut(t); }
    static float EaseBack(float t) { return EaseOutBack(t); }
    static float SmoothTo(float cur, float target, float rate, float dt) { return Smooth(cur, target, rate, dt); } // frame-rate independent
    static Color Blend(Color a, Color b, float t) { return Mix(a, b, t); }
    static Color Alpha(Color c, float a) { return Al(c, a); }
    static Color TextOn(Color bgColor) { return OnColor(bgColor); }   // readable text colour for a background
    static float Random01() { return Rnd(); }
};

inline UI gUI;   // the one instance; use this everywhere
