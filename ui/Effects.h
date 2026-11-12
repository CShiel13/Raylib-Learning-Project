#pragma once
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include "Math.h"
#include "Text.h"
#include "Shapes.h"
#include "Viewport.h"

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
