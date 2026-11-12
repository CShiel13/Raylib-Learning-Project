#pragma once
#include <cmath>
#include "Theme.h"

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
