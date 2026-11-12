#pragma once
#include <algorithm>
#include <cmath>
#include "rlgl.h"
#include "Math.h"
#include "Shapes.h"
#include "Viewport.h"

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
