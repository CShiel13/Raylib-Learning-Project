#pragma once
#include <algorithm>
#include "Theme.h"

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
