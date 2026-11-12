#pragma once
// Avatar.h - Pou-style avatar drawing. Include after UI.h.
//   DrawAvatar(cx, cy, scale, look, happy, time)   the character (time drives blinking / flapping / flames)
//   DrawPet(x, y, scale, pet, time)                companion standing at ground point (x, y)
//   DrawAvatarBackdrop(rect, scene, time, accent)  scene behind the avatar (clip it with ScissorOn first)
// Avatar space: head centre is (0,0), head radius 40, body spans y 25..95. Everything scales with `s`.
#include <cmath>
#include <cstdlib>
#include "../ui/Theme.h"
#include "../ui/Math.h"
#include "../ui/Shapes.h"
#include "AvatarData.h"

inline Color AvPal(int field, int i) {
    static const Color skin[6] = { {255,218,180,255}, {255,230,205,255}, {241,194,150,255}, {214,160,110,255}, {160,108,72,255}, {108,72,50,255} };
    static const Color hair[8] = { {92,60,40,255}, {30,28,34,255}, {240,200,100,255}, {205,95,45,255}, {225,225,235,255}, kSky, {255,120,190,255}, kMint };
    static const Color outf[8] = { kViolet, kSky, kMint, kCoral, kAmber, {240,240,245,255}, {44,48,66,255}, {255,130,200,255} };
    int n = AvPaletteSize(field); i = ((i % n) + n) % n;
    return field == AV_SKIN ? skin[i] : (field == AV_HAIRC ? hair[i] : outf[i]);
}

inline void AvHeart(Vector2 c, float r, Color col) {
    DrawCircleV({ c.x - r * 0.5f, c.y - r * 0.2f }, r * 0.55f, col);
    DrawCircleV({ c.x + r * 0.5f, c.y - r * 0.2f }, r * 0.55f, col);
    Tri({ c.x - r * 1.02f, c.y + r * 0.05f }, { c.x + r * 1.02f, c.y + r * 0.05f }, { c.x, c.y + r * 1.05f }, col);
}
inline void AvStar(Vector2 c, float r, Color col) {          // six-point star
    Tri({ c.x - r, c.y + r * 0.5f }, { c.x + r, c.y + r * 0.5f }, { c.x, c.y - r }, col);
    Tri({ c.x - r, c.y - r * 0.5f }, { c.x + r, c.y - r * 0.5f }, { c.x, c.y + r }, col);
}

// Redesigned avatar: outlined "sticker" style, top-left light, warm shade on the lower right.
// Every part is drawn with a dark outline (shape drawn once grown by `o`, then again at size) so the
// silhouette reads at small sizes. Hair is built as a back layer + a front layer, and each style has its
// own silhouette (see the case comments). Look data and DrawAvatar's signature are unchanged.
inline void DrawAvatar(float cx, float cy, float s, const AvatarLook& L, bool happy = true, float t = 0) {
    const Color ol{ 24, 28, 44, 255 }, white{ 255, 255, 255, 255 }, black{ 0, 0, 0, 255 }, gold{ 255, 200, 61, 255 };
    const float o = 2.4f;                                                       // outline thickness (avatar units)
    const int hairS = L.v[AV_HAIR], eyes = L.v[AV_EYES], mouth = L.v[AV_MOUTH], face = L.v[AV_FACE], outfit = L.v[AV_OUTFIT], hat = L.v[AV_HAT];
    const Color skin = AvPal(AV_SKIN, L.v[AV_SKIN]);
    const Color skinD = Mix(skin, Color{ 150, 70, 60, 255 }, 0.24f);
    const Color hair = AvPal(AV_HAIRC, L.v[AV_HAIRC]);
    const Color hairD = Mix(hair, black, 0.38f), hairL = Mix(hair, white, 0.38f);
    const Color body = AvPal(AV_OUTC, L.v[AV_OUTC]);
    const Color trim = L.v[AV_OUTC] == 4 ? kCoral : kAmber;
    const Color hatC = L.v[AV_OUTC] == 1 ? kViolet : kSky;
    const bool hatOn = hat != 0 && hat != 9;                                    // a real hat covers the crown: tall hair shrinks under it
    const bool blink = std::fmod(t, 3.6f) > 3.48f;
    const float fl = std::sin(t * 4) * 8;                                       // wing flap

    auto P = [&](float x, float y) { return Vector2{ cx + x * s, cy + y * s }; };
    auto R = [&](float x, float y, float w, float h) { return Rectangle{ cx + x * s, cy + y * s, w * s, h * s }; };
    auto C = [&](float x, float y, float r, Color c, float g = 0) { DrawCircleV(P(x, y), (r + g) * s, c); };
    auto Bx = [&](float x, float y, float w, float h, float rad, Color c, float g = 0) { Box(R(x - g, y - g, w + 2 * g, h + 2 * g), (rad + g) * s, c); };
    auto Tr = [&](float x1, float y1, float x2, float y2, float x3, float y3, Color c, float g = 0) {   // triangle, grown about its centre
        float mx = (x1 + x2 + x3) / 3, my = (y1 + y2 + y3) / 3;
        auto grow = [&](float& x, float& y) { float dx = x - mx, dy = y - my, l = std::sqrt(dx * dx + dy * dy) + 1e-4f; x += dx / l * g * 1.7f; y += dy / l * g * 1.7f; };
        grow(x1, y1); grow(x2, y2); grow(x3, y3);
        Tri(P(x1, y1), P(x2, y2), P(x3, y3), c);
    };
    auto Cap = [&](float x1, float y1, float x2, float y2, float w, Color c, float g = 0) {   // thick line with round ends
        DrawLineEx(P(x1, y1), P(x2, y2), (w + 2 * g) * s, c); C(x1, y1, w / 2, c, g); C(x2, y2, w / 2, c, g);
    };
    auto Dome = [&](float rin, float rout, float a0, float a1, Color c, float g = 0) {        // band of hair hugging the skull top
        DrawRing(P(0, 0), (rin - g) * s, (rout + g) * s, a0 - (g > 0 ? 3 : 0), a1 + (g > 0 ? 3 : 0), 48, c);
    };
    auto HDome = [&](float rin, float rout, Color c, float g) {                                // dome whose ends step down to the skull radius
        Dome(rin, rout, 200, 340, c, g); Dome(rin, std::min(rout, 41.5f), 182, 200, c, g); Dome(rin, std::min(rout, 41.5f), 340, 358, c, g);
    };
    auto Sec = [&](float x, float y, float r, float a0, float a1, Color c, float g = 0) { DrawCircleSector(P(x, y), (r + g) * s, a0, a1, 32, c); };
    auto OC = [&](float x, float y, float r, Color c) { C(x, y, r, ol, o); C(x, y, r, c); };
    auto OB = [&](float x, float y, float w, float h, float rad, Color c) { Bx(x, y, w, h, rad, ol, o); Bx(x, y, w, h, rad, c); };
    auto OT = [&](float x1, float y1, float x2, float y2, float x3, float y3, Color c) { Tr(x1, y1, x2, y2, x3, y3, ol, o); Tr(x1, y1, x2, y2, x3, y3, c); };
    auto OCap = [&](float x1, float y1, float x2, float y2, float w, Color c) { Cap(x1, y1, x2, y2, w, ol, o); Cap(x1, y1, x2, y2, w, c); };
    auto OSec = [&](float x, float y, float r, float a0, float a1, Color c) { Sec(x, y, r, a0, a1, ol, o); Sec(x, y, r, a0, a1, c); };

    // ================= back item (behind everything)
    switch (L.v[AV_BACK]) {
    case 1: OB(-40, 34, 80, 54, 14, Color{ 130, 90, 62, 255 }); Bx(-30, 60, 60, 6, 3, Color{ 100, 68, 48, 255 }); break;            // backpack
    case 2: OT(-50, -2, 50, -2, 0, 102, Color{ 196, 52, 94, 255 }); Tr(-34, 6, 34, 6, 0, 84, Color{ 150, 36, 76, 255 }); break;     // cape
    case 3: case 4:                                                                                                                   // wings
        for (int k = -1; k <= 1; k += 2) {
            Color wc = L.v[AV_BACK] == 3 ? Color{ 245, 247, 255, 255 } : Color{ 140, 56, 108, 255 };
            OT(k * 24.0f, 30, k * 98.0f, -24 + fl, k * 72.0f, 66, wc);
            Tr(k * 24.0f, 34, k * 80.0f, 6 + fl * 0.6f, k * 58.0f, 70, Mix(wc, black, 0.14f));
        }
        break;
    case 5:                                                                                                                           // jetpack
        for (int k = -1; k <= 1; k += 2) {
            OB(k * 30.0f - 11, 34, 22, 52, 8, Color{ 150, 158, 175, 255 });
            float fh = 12 + std::sin(t * 30 + k) * 5;
            Tr(k * 30.0f - 7, 86, k * 30.0f + 7, 86, k * 30.0f, 86 + fh, kAmber);
        }
        break;
    case 6: {                                                                                                                         // rainbow cape
        static const Color rb[5] = { kCoral, kAmber, kMint, kSky, kViolet };
        OT(-50, -2, 50, -2, 0, 106, rb[0]);
        for (int i = 1; i < 5; i++) { float a = 1 - i * 0.17f; Tr(-50 * a, -2, 50 * a, -2, 0, -2 + 108 * a, rb[i]); }
    } break;
    }

    // ================= hair behind the head
    const Color hOl = hairS == 9 ? Color{ 120, 30, 40, 255 } : ol;              // flame hair has its own fire palette
    const Color hFill = hairS == 9 ? kCoral : hair;
    auto hairBack = [&](Color c, float g) {
        switch (hairS) {
        case 3: Bx(-47, -34, 94, 104, 38, c, g); break;                                                       // long: a curtain behind head and shoulders
        case 6:                                                                                               // afro: big scalloped cloud
            for (int i = 0; i < 14; i++) { float a = i * 0.4488f; C(std::cos(a) * 47, -12 + std::sin(a) * 43, 15, c, g); }
            C(0, -12, 48, c, g); break;
        case 7: for (int k = -1; k <= 1; k += 2) { Cap(k * 43.0f, -2, k * 52.0f, 18, 15, c, g); Cap(k * 52.0f, 18, k * 56.0f, 38, 11, c, g); } break;   // pigtails hang at the sides
        case 8: { float sw = std::sin(t * 3) * 3;                                                             // ponytail: long S-shaped tail off the back
            Cap(32, -34, 50, -30, 17, c, g); Cap(50, -30, 61 + sw, -4, 19, c, g); Cap(61 + sw, -4, 58 + sw, 24, 15, c, g); Cap(58 + sw, 24, 48 + sw * 1.6f, 46, 9, c, g);
        } break;
        case 9: if (!hatOn)                                                                                   // flame: animated tongues
            for (int i = 0; i < 7; i++) {
                float a = (212 + i * 19.3f) * 0.017453f, w = std::sin(t * 7 + i * 1.9f), len = 74 - std::abs(i - 3) * 5.0f + w * 7, bend = w * 0.16f;
                Tr(36 * std::cos(a - 0.33f), 36 * std::sin(a - 0.33f), 36 * std::cos(a + 0.33f), 36 * std::sin(a + 0.33f), len * std::cos(a + bend), len * std::sin(a + bend), c, g);
            }
            break;
        }
    };
    if (hairS >= 3 && hairS != 4 && hairS != 5) { hairBack(hOl, o); hairBack(hFill, 0); }

    // ================= arms, torso, outfit
    Color torso = body;
    if (outfit == 4) torso = Color{ 150, 160, 180, 255 };
    else if (outfit == 5) torso = Color{ 92, 60, 160, 255 };
    else if (outfit == 7) torso = Color{ 240, 190, 60, 255 };
    const Color sleeve = Mix(torso, black, 0.14f), torsoD = Mix(torso, black, 0.2f);
    for (int k = -1; k <= 1; k += 2) {                                                                         // arms and hands
        OCap(k * 34.0f, 46, k * 41.0f, 76, 15, sleeve);
        OC(k * 41.0f, 82, 6.5f, skin);
    }
    OB(-33, 31, 66, 65, 24, torso);
    Bx(-30, 76, 60, 20, 17, torsoD);                                                                           // shade across the lower torso
    Bx(-26, 40, 5, 24, 2.5f, Al(white, 0.22f));                                                                // rim light
    switch (outfit) {
    case 1: OB(-32, 30, 64, 17, 8, Mix(body, black, 0.3f)); OB(-17, 68, 34, 17, 7, Mix(body, black, 0.22f)); break;                        // hoodie
    case 2: for (int i = 0; i < 3; i++) Bx(-29, 50 + i * 13.0f, 58, 6, 3, Mix(body, white, 0.55f)); break;                                // stripes
    case 3: Tr(-12, 31, 12, 31, 0, 66, white); Tr(-33, 33, -10, 31, -17, 66, Mix(body, black, 0.35f)); Tr(10, 31, 33, 33, 17, 66, Mix(body, black, 0.35f));   // suit
            Tr(-4.5f, 40, 4.5f, 40, 0, 78, kCoral); break;
    case 4: Bx(-27, 40, 54, 14, 6, Color{ 205, 212, 226, 255 }); Bx(-27, 61, 54, 14, 6, Color{ 205, 212, 226, 255 });                       // armor
            OC(-34, 40, 11, Color{ 120, 130, 150, 255 }); OC(34, 40, 11, Color{ 120, 130, 150, 255 }); break;
    case 5: Bx(-31, 66, 62, 8, 0, gold); for (int i = 0; i < 4; i++) C(-18 + i * 12.0f, 46 + (i % 2) * 12.0f, 2.5f, gold); break;          // robe
    case 6: C(0, 60, 15, kAmber); C(0, 60, 10, kCoral); AvStar(P(0, 60), 5 * s, white); break;                                              // hero
    case 7: Cap(-18, 40, -8, 86, 3, Al(white, 0.5f)); Cap(-6, 40, 2, 86, 2, Al(white, 0.35f)); break;                                      // golden
    }
    Bx(-9, 30, 18, 16, 6, skinD);                                                                              // neck
    Bx(-16, 34, 32, 9, 4.5f, Mix(torso, black, 0.32f));                                                        // collar

    // ================= head
    for (int k = -1; k <= 1; k += 2) { OC(k * 39.5f, 3, 6.5f, skin); C(k * 39.5f, 3, 3, skinD); }              // ears
    C(0, 0, 40, ol, o); C(0, 0, 40, skinD); C(-1.8f, -1.8f, 38.4f, skin);                                      // head: outline, shade, lit face
    if (face == 4) { Sec(0, 16, 38, 15, 165, ol, o); Sec(0, 16, 38, 15, 165, hair); }                          // beard (under the neck items)

    switch (L.v[AV_NECK]) {
    case 1: OT(-17, 38, -17, 54, 0, 46, kCoral); OT(17, 38, 17, 54, 0, 46, kCoral); OC(0, 46, 4.5f, Color{ 170, 40, 60, 255 }); break;      // bow tie
    case 2: OB(-35, 32, 70, 13, 6, trim); OB(10, 40, 13, 28, 5, trim); break;                                                               // scarf
    case 3: OT(-31, 34, 31, 34, 0, 62, kCoral); for (int i = -1; i <= 1; i++) C(i * 11.0f, 41 + (i == 0 ? 8.0f : 0.0f), 2, white); break;   // bandana
    case 4: Cap(-10, 36, 0, 60, 3, kSky); Cap(10, 36, 0, 60, 3, kSky); OC(0, 66, 8, gold); C(0, 66, 4, Mix(gold, white, 0.5f)); break;      // medal
    case 5: DrawRing(P(0, 32), 21 * s, 24 * s, 10, 170, 24, gold); OC(0, 57, 5, gold); break;                                               // chain
    case 6: for (int i = 0; i < 7; i++) OC(-27 + i * 9.0f, 39 + 10 * std::sin(i * 0.5236f), 5, i % 2 ? kCoral : kAmber); break;            // lei
    }

    // ================= hair on top of the head (front layer)
    auto hairFront = [&](Color c, float g) {
        switch (hairS) {
        case 1: HDome(23, 44, c, g); Tr(-32, -13, 9, -43, 20, -23, c, g); Tr(-14, -28, 12, -39, 4, -17, c, g); break;                      // short: tidy cap, one swept fringe
        case 2:                                                                                                // spiky: star-burst of long spikes + jagged bangs
            if (!hatOn) for (int i = 0; i < 7; i++) {
                float a = (205 + i * 21.7f) * 0.017453f, len = 76 - std::abs(i - 3) * 6.0f;
                Tr(34 * std::cos(a - 0.30f), 34 * std::sin(a - 0.30f), 34 * std::cos(a + 0.30f), 34 * std::sin(a + 0.30f),
                   len * std::cos(a + 0.10f), len * std::sin(a + 0.10f), c, g);
            }
            if (!hatOn) for (int k = -1; k <= 1; k += 2) Tr(k * 35.0f, -16, k * 39.0f, 6, k * 66.0f, 8, c, g);   // side spikes at ear level
            HDome(22, 45, c, g);
            Tr(-33, -30, -13, -36, -26, -12, c, g); Tr(-12, -37, 12, -37, 0, -14, c, g); Tr(13, -36, 33, -30, 26, -12, c, g);
            break;
        case 3:                                                                                                // long: centre part, curtains down both sides
            HDome(23, 44, c, g);
            Tr(-37, -26, -1, -45, -18, -12, c, g); Tr(1, -45, 37, -26, 18, -12, c, g);
            Cap(-39, -4, -43, 62, 17, c, g); Cap(39, -4, 43, 62, 17, c, g);
            break;
        case 4: HDome(25, 43, c, g); if (!hatOn) C(0, -61, 15, c, g);                                  // bun: sleek, high knot
                Cap(-37, -14, -37, 4, 6, c, g); Cap(37, -14, 37, 4, 6, c, g); break;
        case 5: if (!hatOn) {                                                                                   // mohawk: tall narrow crest, shaved sides
                    Bx(-13, -78, 26, 54, 11, c, g);
                    Tr(-13, -72, -7, -90, 0, -72, c, g); Tr(-5, -76, 2, -98, 9, -76, c, g); Tr(4, -72, 11, -88, 13, -70, c, g);
                } else Bx(-9, -52, 18, 28, 6, c, g);
                break;
        case 6: if (g > 0) Dome(23.6f, 26, 190, 350, c); else {                                                  // afro: only the hairline is outlined, so the cloud is one mass
                    Dome(26, 44, 190, 350, c);
                    for (int i = 0; i < 7; i++) { float a = (200 + i * 23.3f) * 0.017453f; C(33 * std::cos(a), 33 * std::sin(a), 10.5f, c); }
                }
                break;
        case 7: HDome(23, 44, c, g); Tr(-35, -26, -3, -44, -20, -14, c, g); Tr(3, -44, 35, -26, 20, -14, c, g); break;   // pigtails: centre-part bangs
        case 8: HDome(23, 44, c, g); Tr(33, -16, -6, -50, -17, -24, c, g); break;                      // ponytail: fringe swept the other way
        case 9: HDome(22, 45, c, g); break;
        }
    };
    if (hairS == 5) {                                                                                           // shaved sides show stubble under the crest
        DrawRing(P(0, 0), 24 * s, 40 * s, 190, 350, 40, Mix(skin, hair, 0.30f));
    }
    if (hairS >= 1) { hairFront(hOl, o); hairFront(hFill, 0); }
    if (hairS == 4) { Bx(-9, -50, 18, 5, 2.5f, kCoral); }
    if ((hairS == 7)) for (int k = -1; k <= 1; k += 2) OC(k * 43.0f, -3, 5.5f, kCoral);                         // hair ties
    if (hairS == 8) OC(33, -33, 5.5f, kCoral);

    // hair detail: highlight + a few strands in each style's own direction
    if (hairS >= 1 && hairS != 5) {
        DrawRing(P(0, 0), 35 * s, 38 * s, 212, 262, 24, Al(hairS == 9 ? kAmber : hairL, 0.75f));
        switch (hairS) {
        case 1: Cap(-2, -46, -17, -30, 1.8f, hairD); Cap(6, -44, -4, -30, 1.8f, hairD); break;
        case 2: for (int i = 0; i < 7 && !hatOn; i++) { float a = (205 + i * 21.7f) * 0.017453f, len = 76 - std::abs(i - 3) * 6.0f;
                    Cap(38 * std::cos(a - 0.1f), 38 * std::sin(a - 0.1f), (len - 9) * std::cos(a + 0.08f), (len - 9) * std::sin(a + 0.08f), 1.8f, hairL); } break;
        case 3: Cap(-40, 0, -42, 52, 1.8f, hairD); Cap(40, 0, 42, 52, 1.8f, hairD); break;
        case 6: for (int i = 0; i < 5; i++) C(-30 + i * 15.0f, -52 + (i % 2) * 6.0f, 2.2f, hairL); break;
        case 9: if (!hatOn) for (int i = 0; i < 5; i++) {
                    float a = (222 + i * 19.0f) * 0.017453f, w = std::sin(t * 9 + i * 2.3f), len = 46 + w * 6 - std::abs(i - 2) * 3.0f;
                    Tr(34 * std::cos(a - 0.2f), 34 * std::sin(a - 0.2f), 34 * std::cos(a + 0.2f), 34 * std::sin(a + 0.2f), len * std::cos(a), len * std::sin(a), kAmber);
                } break;
        }
    }
    if (hairS == 0) DrawRing(P(0, 0), 31 * s, 35 * s, 214, 258, 20, Al(white, 0.35f));                         // bald: scalp shine

    // ================= face
    const Color brow = hairS == 0 ? Mix(skin, black, 0.55f) : hairD;
    if (hairS != 2 && hairS != 3 && hairS != 7)                                                                 // bangs already cover the brows
        for (int k = -1; k <= 1; k += 2) Cap(k * 20.0f, -16.5f, k * 8.0f, happy ? -19.0f : -14.5f, 2.8f, brow);
    for (int k = -1; k <= 1; k += 2) C(k * 24.0f, 9, 5.5f, Al(kCoral, face == 1 ? 0.5f : 0.2f));              // cheeks
    C(1.2f, 6, 1.6f, skinD);                                                                                    // nose

    for (int k = -1; k <= 1; k += 2) {                                                                          // eyes
        const float ex = k * 14.0f; const Vector2 e = P(ex, -5);
        if (blink) { Cap(ex - 6, -5, ex + 6, -5, 2.8f, ol); continue; }
        switch (eyes) {
        case 0: Bx(ex - 5, -11.5f, 10, 13, 5, ol); C(ex - 1.5f, -8, 2.2f, white); C(ex + 1.8f, -1.5f, 1.1f, Al(white, 0.75f)); break;                       // glossy dots
        case 1: Bx(ex - 9, -15, 18, 21, 9, white, 1); Bx(ex - 6.5f, -11, 13, 16, 6.5f, Color{ 60, 110, 190, 255 }); Bx(ex - 3.4f, -8, 6.8f, 10, 3.4f, ol);
                C(ex - 2, -8, 2.2f, white); C(ex + 2.5f, -2, 1.2f, Al(white, 0.8f)); break;                                                                    // big anime eyes
        case 2: DrawRing(P(ex, -3), 5.5f * s, 8.5f * s, 190, 350, 12, ol); break;
        case 3: Cap(ex - 7, -5, ex + 7, -5, 3, ol); Cap(ex - 5, -1.5f, ex + 5, -1.5f, 1.4f, Al(ol, 0.5f)); break;
        case 4: AvStar(e, 7.5f * s, kAmber); C(ex, -5, 2.4f, ol); break;
        case 5: AvHeart({ e.x, e.y - 3 * s }, 7 * s, kCoral); break;
        case 6: C(ex, -5, 8, white); C(ex, -5, 8, ol, 1); C(ex, -5, 8, white); DrawRing(e, 1.5f * s, 3 * s, 0, 360, 14, ol); DrawRing(e, 4.5f * s, 6 * s, 40, 360, 18, ol); break;
        case 7: Bx(ex - 9, -11, 18, 12, 3, Color{ 20, 24, 36, 255 }); C(ex, -5, 3.5f, Mix(kCoral, white, 0.2f + 0.2f * std::sin(t * 6))); break;
        }
    }

    const float mY = 15;                                                                                        // mouth
    auto smile = [&]() { Cap(-12, mY - 3, 0, mY + 4, 2.8f, ol); Cap(0, mY + 4, 12, mY - 3, 2.8f, ol); };
    if (!happy) Cap(-10, mY + 4, 10, mY + 4, 2.8f, ol);
    else switch (mouth) {
    case 0: smile(); break;
    case 1: Sec(0, mY - 4, 14, 0, 180, ol); Bx(-10, mY - 4, 20, 4.5f, 1, white); Bx(-6, mY + 3.5f, 12, 5, 2.5f, kCoral); break;
    case 2: Cap(-10, mY + 2, 5, mY + 2, 2.8f, ol); Cap(5, mY + 2, 12, mY - 4, 2.8f, ol); break;
    case 3: Bx(-4.5f, mY + 2, 10, 12, 5, ol, 1); Bx(-4.5f, mY + 2, 10, 12, 5, kCoral); smile(); break;
    case 4: DrawRing(P(-5, mY - 2), 3.5f * s, 6 * s, 5, 175, 10, ol); DrawRing(P(5, mY - 2), 3.5f * s, 6 * s, 5, 175, 10, ol); break;
    case 5: smile(); Tr(-9, mY - 1, -3, mY + 1, -6, mY + 8, white); Tr(3, mY + 1, 9, mY - 1, 6, mY + 8, white); break;
    case 6: Bx(-14, mY - 4, 28, 12, 2, ol, 1); Bx(-14, mY - 4, 28, 12, 2, Color{ 70, 78, 96, 255 });
            for (int i = 1; i < 4; i++) Cap(-14 + i * 7.0f, mY - 3, -14 + i * 7.0f, mY + 6, 1.5f, Color{ 190, 198, 215, 255 }); break;
    }

    switch (face) {                                                                                             // face extras
    case 2: { static const float fx[6] = { -24, -18, -22, 24, 18, 22 }, fy[6] = { 5, 9, 12, 5, 9, 12 };
              for (int i = 0; i < 6; i++) C(fx[i], fy[i], 1.6f, Color{ 160, 100, 70, 220 }); } break;
    case 3: OB(-17, 5, 34, 8, 4, Mix(hair, black, 0.2f)); break;
    case 5: Cap(-14, -5, -40, -19, 2, ol); Cap(-14, -5, 36, -26, 2, ol); OC(-14, -5, 11, ol); break;
    case 6: for (int k = -1; k <= 1; k += 2) { Cap(k * 27.0f - 6, 1, k * 27.0f + 6, 5, 3, kSky); Cap(k * 27.0f - 6, 8, k * 27.0f + 6, 12, 3, kSky); } break;
    case 7: OC(0, 5, 7, kCoral); C(-2, 3, 2, Al(white, 0.6f)); break;
    }

    switch (L.v[AV_GLASSES]) {                                                                                  // glasses
    case 1: DrawRing(P(-14, -5), 8 * s, 10.5f * s, 0, 360, 28, ol); DrawRing(P(14, -5), 8 * s, 10.5f * s, 0, 360, 28, ol); Cap(-4, -5, 4, -5, 2, ol);
            C(-14, -5, 8, Al(white, 0.12f)); C(14, -5, 8, Al(white, 0.12f)); break;
    case 2: OB(-26, -14, 23, 15, 5, Color{ 20, 22, 30, 255 }); OB(3, -14, 23, 15, 5, Color{ 20, 22, 30, 255 });
            Cap(-3, -9, 3, -9, 2, ol); Cap(-21, -11, -14, -11, 2, Al(white, 0.45f)); Cap(8, -11, 15, -11, 2, Al(white, 0.45f)); break;
    case 3: AvHeart(P(-14, -5), 10 * s, Al(kCoral, 0.9f)); AvHeart(P(14, -5), 10 * s, Al(kCoral, 0.9f)); break;
    case 4: DrawRing(P(14, -5), 9 * s, 11.5f * s, 0, 360, 28, gold); Cap(23, -1, 30, 44, 1.5f, gold); break;
    case 5: OB(-26, -13, 22, 15, 3, Color{ 255, 60, 60, 190 }); OB(4, -13, 22, 15, 3, Color{ 60, 220, 255, 190 }); Cap(-4, -8, 4, -8, 2, ol); break;
    case 6: Cap(-41, -5, 41, -5, 5, Color{ 80, 60, 48, 255 });
            for (int k = -1; k <= 1; k += 2) { OC(k * 14.0f, -5, 11, Color{ 120, 90, 60, 255 }); C(k * 14.0f, -5, 8.5f, Color{ 255, 190, 60, 200 }); C(k * 14.0f - 3, -8, 2.5f, Al(white, 0.6f)); } break;
    case 7: OB(-32, -15, 64, 18, 8, Color{ 80, 220, 255, 200 }); Cap(-24, -9, -10, -9, 2, Al(white, 0.7f)); break;
    }

    // ================= hat (sits 7 units lower than the skull top so it looks worn, not hovering)
    cy += 7 * s;
    switch (hat) {
    case 1: OB(-27, -73, 54, 36, 17, hatC); OB(-45, -44, 90, 10, 5, Mix(hatC, black, 0.2f)); C(0, -73, 3.5f, Mix(hatC, black, 0.3f)); Bx(-19, -66, 6, 16, 3, Al(white, 0.28f)); break;   // cap
    case 2: OSec(0, -30, 41, 180, 360, kCoral); OB(-43, -41, 86, 14, 6, Mix(kCoral, black, 0.25f)); OC(0, -73, 8, white); break;                                                    // beanie
    case 3: OB(-60, -48, 120, 11, 5, Color{ 140, 96, 60, 255 }); OB(-24, -74, 48, 32, 12, Color{ 160, 110, 70, 255 }); Bx(-24, -52, 48, 6, 0, kAmber); break;                          // cowboy
    case 4: OT(-25, -40, 25, -40, 0, -96, kViolet); C(-8, -57, 3, kAmber); C(7, -69, 3, kMint); OC(0, -96, 5, kAmber); break;                                                       // party
    case 5: OB(-29, -55, 58, 16, 3, kAmber); for (int i = -1; i <= 1; i++) OT(i * 22.0f - 8, -52, i * 22.0f + 8, -52, i * 22.0f, -74, kAmber); C(0, -47, 3, kCoral); break;        // crown
    case 6: OB(-45, -47, 90, 10, 4, Color{ 40, 44, 62, 255 }); OB(-25, -92, 50, 48, 4, Color{ 40, 44, 62, 255 }); Bx(-25, -56, 50, 8, 0, kCoral); break;                            // top hat
    case 7: OB(-54, -47, 108, 11, 5, Color{ 70, 40, 130, 255 }); OT(-30, -44, 30, -44, 12, -108, Color{ 92, 60, 160, 255 }); AvStar(P(-2, -66), 6 * s, kAmber); break;                // wizard
    case 8: OB(-47, -57, 94, 18, 9, Color{ 40, 44, 62, 255 }); OT(-47, -42, 47, -42, 0, -82, Color{ 40, 44, 62, 255 });
            C(0, -58, 6, white); C(-2, -59, 1.3f, ol); C(2, -59, 1.3f, ol); break;                                                                                                  // pirate
    case 9: { float hy = -68 + std::sin(t * 3) * 2; Vector2 hc = P(0, hy);                                                                                                         // halo
              DrawEllipse((int)hc.x, (int)hc.y, 30 * s, 8 * s, Al(kAmber, 0.25f));
              for (int i = 0; i < 3; i++) DrawEllipseLines((int)hc.x, (int)hc.y, (26 + i * 0.8f) * s, (6 + i * 0.5f) * s, kAmber); } break;
    case 10: OB(-33, -59, 66, 19, 4, gold); for (int i = -2; i <= 2; i++) OT(i * 14.0f - 7, -56, i * 14.0f + 7, -56, i * 14.0f, (i % 2) ? -82.0f : -90.0f, gold);
             C(0, -49, 4, kCoral); C(-18, -49, 3, kSky); C(18, -49, 3, kMint); break;                                                                                                // gold crown
    }
}

// Companion standing at ground point (x, y). Hops gently.
inline void DrawPet(float x, float y, float s, int pet, float t) {
    if (pet <= 0) return;
    const Color ink{ 30, 34, 48, 255 }, white{ 255, 255, 255, 255 };
    auto P = [&](float dx, float dy) { return Vector2{ x + dx * s, y + dy * s }; };
    auto R = [&](float dx, float dy, float w, float h) { return Rectangle{ x + dx * s, y + dy * s, w * s, h * s }; };
    const float b = std::fabs(std::sin(t * 4)) * 5;
    DrawEllipse((int)x, (int)y, 22 * s, 5 * s, Color{ 0, 0, 0, 60 });
    switch (pet) {
    case 1: {                                                                                                        // slime
        Box(R(-20, -28 - b, 40, 28), 14 * s, kMint);
        DrawCircleV(P(-7, -16 - b), 3 * s, ink); DrawCircleV(P(7, -16 - b), 3 * s, ink); DrawCircleV(P(-11, -23 - b), 3 * s, Al(white, 0.5f));
    } break;
    case 2: {                                                                                                        // cat
        const Color c{ 255, 160, 80, 255 };
        DrawEllipse((int)P(0, -14 - b).x, (int)P(0, -14 - b).y, 16 * s, 12 * s, c);
        DrawCircleV(P(14, -28 - b), 11 * s, c);
        Tri(P(6, -36 - b), P(12, -36 - b), P(8, -48 - b), c); Tri(P(16, -36 - b), P(22, -36 - b), P(20, -48 - b), c);
        DrawLineEx(P(-14, -14 - b), P(-26, -30 - b + std::sin(t * 5) * 4), 4 * s, c);
        DrawCircleV(P(11, -30 - b), 1.6f * s, ink); DrawCircleV(P(18, -30 - b), 1.6f * s, ink);
    } break;
    case 3: {                                                                                                        // dog
        const Color c{ 200, 150, 100, 255 };
        Box(R(-16, -24 - b, 32, 20), 8 * s, c); DrawCircleV(P(16, -30 - b), 11 * s, c);
        Box(R(21, -38 - b, 8, 14), 4 * s, Mix(c, ink, 0.35f)); DrawCircleV(P(24, -27 - b), 4 * s, Mix(c, white, 0.3f));
        DrawCircleV(P(17, -32 - b), 1.8f * s, ink);
        DrawLineEx(P(-16, -22 - b), P(-26, -34 - b + std::sin(t * 12) * 4), 4 * s, c);
    } break;
    case 4: {                                                                                                        // ghost
        const Color g = Al(white, 0.9f);
        DrawCircleV(P(0, -30 - b), 14 * s, g); DrawRectangleRec(R(-14, -30 - b, 28, 24), g);
        for (int i = 0; i < 3; i++) DrawCircleV(P(-9 + i * 9.0f, -6 - b), 4.5f * s, g);
        DrawCircleV(P(-5, -32 - b), 2 * s, ink); DrawCircleV(P(5, -32 - b), 2 * s, ink); DrawCircleV(P(0, -25 - b), 2 * s, ink);
    } break;
    case 5: {                                                                                                        // robot
        Box(R(-14, -24 - b, 28, 22), 5 * s, Color{ 170, 178, 195, 255 }); Box(R(-12, -44 - b, 24, 18), 4 * s, Color{ 200, 206, 220, 255 });
        DrawCircleV(P(-5, -36 - b), 3 * s, kSky); DrawCircleV(P(5, -36 - b), 3 * s, kSky);
        DrawLineEx(P(0, -44 - b), P(0, -54 - b), 2 * s, Color{ 170, 178, 195, 255 });
        DrawCircleV(P(0, -55 - b), 3 * s, Mix(kCoral, white, 0.5f + 0.5f * std::sin(t * 6)));
    } break;
    case 6: {                                                                                                        // baby dragon
        const Color c{ 90, 200, 120, 255 };
        DrawEllipse((int)P(0, -16 - b).x, (int)P(0, -16 - b).y, 18 * s, 13 * s, c);
        DrawCircleV(P(18, -30 - b), 10 * s, c);
        Tri(P(14, -38 - b), P(18, -38 - b), P(16, -48 - b), kAmber);
        Tri(P(-6, -22 - b), P(-22, -44 - b + std::sin(t * 6) * 4), P(4, -30 - b), Mix(c, kViolet, 0.4f));
        DrawLineEx(P(-16, -12 - b), P(-30, -22 - b), 5 * s, c);
        DrawCircleV(P(20, -32 - b), 1.8f * s, ink);
        DrawCircleV(P(32, -30 - b), (3 + 2 * std::sin(t * 10)) * s, Al(kAmber, 0.8f));
    } break;
    }
}

// Scene behind the avatar. Call ScissorOn(rect) first and EndScissorMode() after, so shapes stay inside.
inline void DrawAvatarBackdrop(Rectangle r, int bg, float t, Color accent) {
    const Color white{ 255, 255, 255, 255 };
    const float gy = r.y + r.height * 0.8f;                                        // ground line
    auto sky = [&](Color a, Color b) { DrawRectangleGradientV((int)r.x, (int)r.y, (int)r.width, (int)r.height, a, b); };
    auto ground = [&](Color c) { DrawRectangle((int)r.x, (int)gy, (int)r.width, (int)(r.y + r.height - gy), c); };
    auto stars = [&](int n, float h) {
        for (int i = 0; i < n; i++)
            DrawCircleV({ r.x + std::fmod(i * 97.3f + 13, r.width), r.y + std::fmod(i * 53.7f + 7, r.height * h) },
                        1.2f + (i % 3) * 0.5f, Al(white, 0.45f + 0.45f * std::sin(t * 2 + i)));
    };
    switch (bg) {
    case 0: DrawCircleV({ r.x + r.width / 2, r.y + r.height * 0.4f }, 205, Al(accent, 0.06f));
            DrawCircleV({ r.x + r.width / 2, r.y + r.height * 0.4f }, 145, Al(accent, 0.08f)); break;
    case 1: sky({ 255, 120, 100, 255 }, { 255, 200, 120, 255 });                                                    // sunset
            DrawCircleV({ r.x + r.width / 2, gy - 10 }, 80, Color{ 255, 236, 160, 255 }); ground({ 80, 50, 90, 255 }); break;
    case 2: sky({ 10, 14, 36, 255 }, { 38, 48, 96, 255 }); stars(26, 0.7f);                                         // night
            DrawCircleV({ r.x + r.width - 80, r.y + 80 }, 28, Color{ 240, 240, 215, 255 });
            DrawCircleV({ r.x + r.width - 68, r.y + 74 }, 24, Color{ 18, 24, 52, 255 }); ground({ 20, 26, 50, 255 }); break;
    case 3: sky({ 120, 200, 255, 255 }, { 215, 242, 255, 255 });                                                    // meadow
            DrawCircleV({ r.x + 90, gy + 140 }, 220, Color{ 90, 170, 90, 255 });
            DrawCircleV({ r.x + r.width - 60, gy + 170 }, 240, Color{ 70, 150, 80, 255 });
            for (int i = 0; i < 10; i++) DrawCircleV({ r.x + 30 + i * 41.0f, gy + 18 + (i % 3) * 14.0f }, 3.5f, i % 2 ? kCoral : kAmber);
            break;
    case 4: sky({ 100, 190, 240, 255 }, { 190, 235, 250, 255 });                                                    // ocean
            for (float x = r.x; x < r.x + r.width; x += 8) {
                float wy = gy - 10 + std::sin(x * 0.04f + t * 2) * 5;
                DrawRectangle((int)x, (int)wy, 9, (int)(r.y + r.height - wy), Color{ 30, 110, 190, 255 });
            } break;
    case 5: {                                                                                                        // city
        sky({ 28, 32, 66, 255 }, { 120, 84, 130, 255 });
        static const float bh[7] = { 150, 220, 120, 260, 170, 210, 140 };
        for (int i = 0; i < 7; i++) {
            float bx = r.x + i * 64.0f - 4, bt = gy - bh[i];
            DrawRectangle((int)bx, (int)bt, 60, (int)(r.y + r.height - bt), Color{ 26, 28, 52, 255 });
            for (int w = 0; w < (int)(bh[i] / 26); w++)
                for (int c = 0; c < 2; c++)
                    if ((i * 7 + w * 3 + c * 5) % 4) DrawRectangle((int)(bx + 12 + c * 26), (int)(bt + 12 + w * 26), 10, 12, Al(kAmber, 0.8f));
        }
    } break;
    case 6: {                                                                                                        // space
        sky({ 6, 8, 22, 255 }, { 26, 18, 56, 255 }); stars(44, 1.0f);
        DrawCircleV({ r.x + 110, r.y + 150 }, 46, Color{ 150, 110, 230, 255 });
        DrawRing({ r.x + 110, r.y + 150 }, 58, 62, 200, 340, 30, Al(white, 0.5f));
        float k = std::fmod(t * 0.4f, 1.0f);
        DrawLineEx({ r.x + r.width * k, r.y + 40 + 120 * k }, { r.x + r.width * k - 40, r.y + 16 + 120 * k }, 2, Al(white, 0.8f * (1 - k)));
    } break;
    case 7: sky({ 6, 14, 30, 255 }, { 14, 30, 50, 255 }); stars(20, 0.6f);                                          // aurora
            for (float x = r.x; x < r.x + r.width; x += 6) {
                float a = 0.5f + 0.5f * std::sin(x * 0.03f + t * 0.8f), h = 90 + 70 * std::sin(x * 0.02f + t * 0.5f);
                Color c = Mix(kMint, kViolet, a);
                DrawRectangleGradientV((int)x, (int)(r.y + 90), 7, (int)h, Al(c, 0.5f), Al(c, 0.0f));
            } break;
    }
}
