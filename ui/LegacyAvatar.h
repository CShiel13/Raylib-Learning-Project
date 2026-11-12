#pragma once
#include "Math.h"
#include "Shapes.h"

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
