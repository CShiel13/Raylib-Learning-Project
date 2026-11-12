#pragma once
#include <algorithm>
#include <cmath>
#include "Math.h"

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
