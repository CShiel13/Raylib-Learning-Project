#pragma once
// ---------------------------------------------------------------------------
// Sh2Mode.h - OPTIONAL easter egg: a "foggy town" parody mode, unlocked with the Konami code
// (Up Up Down Down Left Right Left Right B A) while the Bonus menu is open.
//
//   * DELETE THIS FILE to remove the mode completely. Game.h only includes it when the file exists
//     (__has_include), so the project still builds without it. Nothing else in the game depends on it.
//   * Everything is drawn from shapes and the game's own fonts. No assets, no audio, nothing copied.
//   * The script is an original parody. It borrows the mood of a famous opening, not its text.
//
// Structure (so more scenes are easy to add later):
//   Konami      : detects the code.
//   Scene       : one cutscene or level. Update(dt, advance) returns true when finished; Draw(t) draws it.
//   Mode        : owns the list of scenes, fades between them, and applies the film look on top
//                 (letterbox, vignette, grain, fog). Esc leaves the mode at any time.
//   Scenes now  : TitleCard -> BathroomScene (opening cutscene) -> EndCard.
//   Next ideas  : stairwell / hallway, the road into town, ... (add a Scene subclass, push it in Mode()).
//   Audio hooks : search for TODO(audio) - drip, fluorescent hum, ambience.
// ---------------------------------------------------------------------------
#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include "UI.h"

namespace sh2 {

inline const char* kTitle = "SILENT LOCALHOST";      // parody title card: rename freely
constexpr float kW = 1100, kH = 700;                 // same canvas as the rest of the game

// ------------------------------------------------------------------ Konami code
struct Konami {
    std::vector<int> hist;                            // the last 10 keys pressed
    bool Feed() {                                     // call once per frame; true when the code was completed
        static const int seq[10] = {KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_LEFT, KEY_RIGHT, KEY_B, KEY_A};
        bool done = false;
        for (int k = GetKeyPressed(); k != 0; k = GetKeyPressed()) {
            hist.push_back(k);
            if (hist.size() > 10) hist.erase(hist.begin());
            if (hist.size() == 10 && std::equal(hist.begin(), hist.end(), seq)) { done = true; hist.clear(); }
        }
        return done;
    }
};

// ------------------------------------------------------------------ small helpers
inline float Hash(float x) { float s = std::sin(x * 127.1f) * 43758.5453f; return s - std::floor(s); }
inline Color Rgb(int r, int g, int b, int a = 255) { return Color{(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a}; }

// Wraps text, centres every line, and reveals only the first `shown` characters (typewriter).
// Layout is computed from the full text, so words never jump around while it types.
inline void CenterWrap(const std::string& text, float cx, float y, float maxW, float fs, Color c, int shown) {
    std::vector<std::string> lines; std::string line, word;
    auto push = [&]() {
        if (word.empty()) return;
        std::string t = line.empty() ? word : line + " " + word;
        if (!line.empty() && TxtW(t, fs) > maxW) { lines.push_back(line); line = word; } else line = t;
        word.clear();
    };
    for (char ch : text) { if (ch == ' ') push(); else word += ch; }
    push(); if (!line.empty()) lines.push_back(line);
    float lh = fs * gTextScale * 1.35f;
    int budget = shown;
    for (size_t i = 0; i < lines.size() && budget > 0; i++) {
        std::string part = lines[i].substr(0, std::min((size_t)budget, lines[i].size()));
        float x = cx - TxtW(lines[i], fs) / 2;
        Txt(part, x + 2, y + i * lh + 2, fs, Al(Rgb(0, 0, 0), 0.85f * c.a / 255.0f));      // drop shadow
        Txt(part, x, y + i * lh, fs, c);
        budget -= (int)lines[i].size() + 1;
    }
}

// ------------------------------------------------------------------ scenes
class Scene {
public:
    virtual ~Scene() {}
    virtual void Enter() {}
    virtual bool Update(float dt, bool advance) = 0;     // true = this scene has finished
    virtual void Draw(float t) = 0;                      // t = seconds since the scene started
};

class TitleCard : public Scene {
public:
    bool Update(float dt, bool advance) override { age += dt; return age > 4.6f || (advance && age > 1.5f); }
    void Draw(float t) override {
        DrawRectangle(-3000, -3000, 7100, 6700, Rgb(0, 0, 0));
        float a = Clamp01((t - 0.6f) / 1.6f) * (1.0f - Clamp01((t - 3.8f) / 0.8f));
        TxtC(kTitle, kW / 2, 292, 62, Al(Rgb(205, 208, 196), a));
        TxtC("a JavaQuest easter egg", kW / 2, 372, 18, Al(Rgb(130, 136, 128), a * 0.8f));
    }
private:
    float age = 0;
};

class EndCard : public Scene {
public:
    bool Update(float dt, bool advance) override { age += dt; return advance && age > 1.5f; }
    void Draw(float t) override {
        DrawRectangle(-3000, -3000, 7100, 6700, Rgb(0, 0, 0));
        float a = Clamp01((t - 0.4f) / 1.4f);
        TxtC("to be continued...", kW / 2, 310, 34, Al(Rgb(205, 208, 196), a));
        TxtC("Press Esc, Space or click to return to the game", kW / 2, 372, 16, Al(Rgb(130, 136, 128), a * (0.6f + 0.4f * std::sin(t * 3))));
    }
private:
    float age = 0;
};

// One line of the cutscene. zoom = camera push-in target, glitch = how badly the monitor tears.
struct Beat { std::string text; float hold = 2.0f; float zoom = 1.0f; float glitch = 0.0f; };

class BathroomScene : public Scene {
public:
    BathroomScene() {
        beats = {
            {"", 2.6f, 1.00f, 0.00f},                                                                    // silence, drip, flicker
            {"Localhost:3000... could you really be in this town?", 3.4f, 1.03f, 0.00f},
            {"ERR_CONNECTION_REFUSED. That is all it says.", 2.6f, 1.05f, 0.35f},
            {"No stack trace. No log. Not even an apology.", 2.6f, 1.06f, 0.15f},
            {"The README says it works on my machine. I believe it. That is the problem.", 3.4f, 1.07f, 0.00f},
            {"I deleted node_modules once. It grew back.", 2.6f, 1.08f, 0.10f},
            {"I keep refreshing the page. Nothing refreshes me.", 3.0f, 1.09f, 0.20f},
            {"Fine. I will go and find out what is listening on that port.", 3.4f, 1.12f, 0.00f},
            {"", 1.8f, 1.14f, 0.95f},                                                                    // the monitor loses it, then cut
        };
    }
    void Enter() override { i = 0; beatT = 0; zoom = 1.0f; glitch = 0; }

    bool Update(float dt, bool advance) override {
        if (i >= (int)beats.size()) return true;
        const Beat& b = beats[i];
        beatT += dt;
        zoom = Smooth(zoom, b.zoom, 0.7f, dt);
        glitch = Smooth(glitch, b.glitch, 5.0f, dt);
        float typeTime = b.text.size() / kCps;
        bool full = beatT >= typeTime;
        if (advance && !b.text.empty()) { if (!full) beatT = typeTime + 0.001f; else Next(); }
        else if (full && beatT - typeTime > b.hold) Next();
        return i >= (int)beats.size();
    }

    void Draw(float t) override {
        float flick = Flicker(t);
        rlPushMatrix();                                                  // slow camera push-in
        rlTranslatef(kW / 2, 330, 0); rlScalef(zoom, zoom, 1); rlTranslatef(-kW / 2, -330, 0);
        DrawWall();
        DrawSink(t);
        DrawMonitor(t, flick);
        DrawJames(t);
        rlPopMatrix();
        DrawRectangle(-3000, -3000, 7100, 6700, Al(Rgb(0, 0, 0), 0.26f + (1.0f - flick) * 0.9f));   // fluorescent light flicker
        // TODO(audio): water drip, fluorescent hum, low ambience

        if (i < (int)beats.size() && !beats[i].text.empty()) {
            int shown = (int)(beatT * kCps);
            CenterWrap(beats[i].text, kW / 2, 548, 800, 26, Rgb(236, 236, 228), shown);
        }
    }

private:
    static constexpr float kCps = 36.0f;                                 // typewriter speed
    std::vector<Beat> beats;
    int i = 0; float beatT = 0, zoom = 1.0f, glitch = 0;

    void Next() { i++; beatT = 0; }

    static float Flicker(float t) {
        float n = std::sin(t * 37.0f) * std::sin(t * 13.7f);
        float f = 0.86f + 0.14f * n;
        if (std::fmod(t, 7.3f) < 0.16f) f *= 0.35f;                      // occasional full dip
        return std::max(0.2f, std::min(1.0f, f));
    }

    void DrawWall() const {
        DrawRectangle(0, 0, (int)kW, (int)kH, Rgb(74, 84, 78));
        for (int ty = 0; ty < 10; ty++)
            for (int tx = 0; tx < 16; tx++) {                            // dingy tiles, each slightly different
                float h = Hash(tx * 13.1f + ty * 7.7f);
                DrawRectangle(tx * 70 + 1, ty * 70 + 1, 68, 68, Mix(Rgb(68, 78, 72), Rgb(94, 104, 96), h * 0.7f));
            }
        DrawRectangleGradientV(0, 0, (int)kW, 300, Al(Rgb(0, 0, 0), 0.4f), Al(Rgb(0, 0, 0), 0.0f));
        DrawRectangleGradientV(0, 300, (int)kW, 290, Al(Rgb(0, 0, 0), 0.0f), Al(Rgb(18, 28, 20), 0.6f));
        for (int k = 0; k < 11; k++) {                                   // water stains running down the wall
            float x = Hash(k * 3.3f) * kW, len = 90 + Hash(k + 9.0f) * 320;
            DrawRectangleGradientV((int)x, 0, 9, (int)len, Al(Rgb(36, 44, 32), 0.30f), Al(Rgb(36, 44, 32), 0.0f));
        }
        DrawRectangle(0, 586, (int)kW, 114, Rgb(26, 28, 26));            // floor
        DrawRectangleGradientV(0, 586, (int)kW, 34, Al(Rgb(0, 0, 0), 0.55f), Al(Rgb(0, 0, 0), 0.0f));
    }

    void DrawSink(float t) const {
        Box({420, 446, 260, 82}, 18, Rgb(150, 154, 146));                // porcelain basin
        Box({432, 456, 236, 52}, 14, Rgb(92, 96, 90));
        Box({412, 430, 276, 20}, 8, Rgb(176, 180, 172));                 // rim
        DrawRectangle(544, 398, 16, 40, Rgb(140, 144, 150));             // faucet
        DrawRectangle(544, 398, 52, 12, Rgb(150, 154, 160));
        DrawCircleV({596, 410}, 6, Rgb(150, 154, 160));
        float cyc = std::fmod(t, 1.7f);                                  // one drip every 1.7 s
        if (cyc < 0.55f) DrawCircleV({596, 418 + (cyc / 0.55f) * (cyc / 0.55f) * 58}, 3.5f, Rgb(170, 190, 200, 200));
        else if (cyc < 0.95f) {
            float r = (cyc - 0.55f) / 0.4f;
            DrawRing({596, 480}, 4 + r * 16, 6 + r * 16, 0, 360, 24, Al(Rgb(170, 190, 200), 0.5f * (1 - r)));
        }
    }

    void DrawMonitor(float t, float flick) const {
        const float sx = 376, sy = 136, sw = 348, sh = 230;
        Box({sx - 40, sy - 40, sw + 80, sh + 80}, 50, Al(Rgb(110, 200, 220), 0.07f * flick));          // glow on the wall
        Box({sx - 18, sy - 18, sw + 36, sh + 56}, 14, Rgb(30, 32, 34));                                   // bezel
        Box({sx - 4, sy - 4, sw + 8, sh + 8}, 8, Rgb(12, 14, 16));
        DrawRectangle((int)(sx + sw / 2 - 40), (int)(sy + sh + 24), 80, 4, Rgb(20, 22, 24));
        DrawCircleV({sx + sw - 8, sy + sh + 24}, 3.0f, (std::fmod(t, 2.0f) < 1.0f) ? Rgb(200, 40, 40) : Rgb(70, 16, 16));   // standby LED
        DrawRectangle((int)sx, (int)sy, (int)sw, (int)sh, Rgb(20, 24, 30));
        // browser bar and error page
        Box({sx + 12, sy + 12, sw - 24, 24}, 12, Rgb(40, 46, 56));
        Txt("localhost:3000", sx + 26, sy + 16, 13, Rgb(170, 182, 196), true);
        float cx = sx + sw / 2;
        DrawRing({cx, sy + 70}, 18, 21, 0, 360, 28, Rgb(150, 160, 172));
        DrawCircleV({cx - 7, sy + 64}, 2.5f, Rgb(150, 160, 172));
        DrawCircleV({cx + 7, sy + 64}, 2.5f, Rgb(150, 160, 172));
        DrawLineEx({cx - 8, sy + 80}, {cx + 8, sy + 80}, 2, Rgb(150, 160, 172));
        TxtC("This site can't be reached", cx, sy + 100, 20, Rgb(226, 230, 236));
        TxtC("localhost refused to connect.", cx, sy + 128, 14, Rgb(150, 160, 172));
        TxtC("ERR_CONNECTION_REFUSED", cx, sy + 150, 12, Rgb(110, 120, 132), true);
        // the faint reflection of whoever is standing at the sink
        float sway = std::sin(t * 0.6f) * 2.0f;
        DrawCircleV({cx + sway, sy + sh - 42}, 24, Al(Rgb(0, 0, 0), 0.50f));
        Box({cx - 56 + sway, sy + sh - 22, 112, 22}, 10, Al(Rgb(0, 0, 0), 0.50f));
        // scan lines and glitch tears
        for (int y = 0; y < (int)sh; y += 4) DrawRectangle((int)sx, (int)sy + y, (int)sw, 1, Al(Rgb(0, 0, 0), 0.18f));
        if (glitch > 0.02f) {
            int bars = 1 + (int)(glitch * 7);
            for (int k = 0; k < bars; k++) {
                float seed = std::floor(t * 18) + k * 5.1f;
                float ry = sy + Hash(seed) * (sh - 16), shift = (Hash(seed + 1.7f) - 0.5f) * 60 * glitch;
                DrawRectangle((int)(sx + shift), (int)ry, (int)sw, 3 + (int)(Hash(seed + 3.1f) * 10 * glitch), Al(Rgb(120, 220, 230), 0.30f));
                DrawRectangle((int)(sx - shift), (int)ry + 2, (int)sw, 2, Al(Rgb(230, 80, 90), 0.22f));
            }
            if (glitch > 0.8f) DrawRectangle((int)sx, (int)sy, (int)sw, (int)sh, Al(Rgb(220, 240, 245), 0.12f * Hash(std::floor(t * 30))));
        }
        DrawLineEx({sx - 6, sy + 30}, {sx + 120, sy + sh + 40}, 2, Al(Rgb(190, 200, 205), 0.20f));        // crack in the glass
    }

    void DrawJames(float t) const {                                      // seen from behind, facing the monitor
        float x = kW / 2 + std::sin(t * 0.7f) * 2.0f, br = std::sin(t * 1.3f) * 2.0f;
        Box({x - 130, 586 + br, 260, 220}, 80, Rgb(56, 76, 52));          // green jacket
        Box({x - 130, 586 + br, 260, 30}, 40, Rgb(70, 92, 64));
        DrawRectangle((int)x - 20, (int)(540 + br), 40, 50, Rgb(204, 168, 138));                           // neck
        DrawCircleV({x - 49, 514 + br}, 9, Rgb(204, 168, 138));                                            // ears
        DrawCircleV({x + 49, 514 + br}, 9, Rgb(204, 168, 138));
        DrawCircleV({x, 508 + br}, 50, Rgb(98, 76, 52));                                                   // head: back of the hair
        DrawCircleV({x, 500 + br}, 46, Rgb(112, 88, 60));
        Box({x - 38, 540 + br, 76, 18}, 8, Rgb(90, 68, 46));
    }
};

// ------------------------------------------------------------------ the mode itself
class Mode {
public:
    Mode() {
        scenes.push_back(std::make_unique<TitleCard>());
        scenes.push_back(std::make_unique<BathroomScene>());
        scenes.push_back(std::make_unique<EndCard>());
        scenes[0]->Enter();
    }

    // Runs one frame. Returns false when the player leaves (Esc) or the last scene ends.
    bool Frame(float dt) {
        if (IsKeyPressed(KEY_ESCAPE)) return false;
        bool advance = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        time += dt;
        ClearBackground(Rgb(0, 0, 0));
        Scene& s = *scenes[cur];

        bool finished = false;
        if (phase != 2) { sceneT += dt; finished = s.Update(dt, phase == 1 && advance); }
        if (phase == 0) { fade = std::max(0.0f, fade - dt / 0.9f); if (fade <= 0) phase = 1; }
        else if (phase == 1) { if (finished) phase = 2; }
        else {
            fade = std::min(1.0f, fade + dt / 0.9f);
            if (fade >= 1.0f) {
                if (cur + 1 >= (int)scenes.size()) return false;      // last scene done: leave the mode
                cur++; sceneT = 0; scenes[cur]->Enter(); phase = 0;
            }
        }
        scenes[cur]->Draw(sceneT);
        PostFx();
        if (fade > 0.001f) DrawRectangle(-3000, -3000, 7100, 6700, Al(Rgb(0, 0, 0), fade));
        return true;
    }

private:
    std::vector<std::unique_ptr<Scene>> scenes;
    int cur = 0, phase = 0;                              // 0 fading in, 1 playing, 2 fading out
    float fade = 1.0f, sceneT = 0, time = 0;

    void PostFx() const {                                // the film look, shared by every scene
        for (int k = 0; k < 4; k++) {                    // drifting haze
            float x = std::fmod(time * (14 + k * 6) + k * 330, kW + 600) - 300, y = 250 + k * 90 + std::sin(time * 0.3f + k) * 30;
            DrawCircleV({x, y}, 230, Al(Rgb(150, 160, 152), 0.028f));
        }
        DrawRectangleGradientV(0, 0, (int)kW, 180, Al(Rgb(0, 0, 0), 0.65f), Al(Rgb(0, 0, 0), 0.0f));   // vignette
        DrawRectangleGradientV(0, (int)kH - 180, (int)kW, 180, Al(Rgb(0, 0, 0), 0.0f), Al(Rgb(0, 0, 0), 0.65f));
        DrawRectangleGradientH(0, 0, 220, (int)kH, Al(Rgb(0, 0, 0), 0.55f), Al(Rgb(0, 0, 0), 0.0f));
        DrawRectangleGradientH((int)kW - 220, 0, 220, (int)kH, Al(Rgb(0, 0, 0), 0.0f), Al(Rgb(0, 0, 0), 0.55f));
        for (int k = 0; k < 420; k++) {                  // film grain
            float x = (float)GetRandomValue(0, (int)kW), y = (float)GetRandomValue(0, (int)kH);
            DrawRectangle((int)x, (int)y, 2, 2, Al(GetRandomValue(0, 1) ? Rgb(255, 255, 255) : Rgb(0, 0, 0), 0.05f + GetRandomValue(0, 8) / 100.0f));
        }
        DrawRectangle(-3000, -3000, 7100, 3064, Rgb(0, 0, 0));                  // letterbox bars
        DrawRectangle(-3000, (int)kH - 64, 7100, 3064, Rgb(0, 0, 0));
    }
};

}  // namespace sh2
