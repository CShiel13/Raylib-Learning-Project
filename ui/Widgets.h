#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include "Effects.h"
#include "Text.h"
#include "Shapes.h"
#include "Viewport.h"

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
