#pragma once
// ---------------------------------------------------------------------------
// ee/game.h - OPTIONAL easter egg: a 3D foggy-town parody. Delete the ee/ folder to remove it.
// ---------------------------------------------------------------------------
#include <fstream>
#include <functional>
#include <set>
#include "engine.h"
#include "CameraRig.h"
#include "ui/Text.h"
#include "ui/Shapes.h"
#include "rlgl.h"
#include "content/CourseModel.h"
#include "AdventurePlan.h"
#include "Story.h"
#include "TerminalLock.h"

namespace sh2 {

inline const char* kTitle = "SILENT LOCALHOST";

struct Blk { Vector3 c, s; Color col; bool solid; };
struct Tree { float x, z, h; };
struct Cam { Vector3 pos, target; float fov; Rectangle trig; };
struct Pickup { Vector2 p; int id; };
struct Enemy { int kind = 0; Vector2 p{0, 0}, home{0, 0}; float yaw = 0, cool = 0, dissolve = 0; bool alive = true, active = false, retreating = false; };
struct Area {
    Color fog{100, 100, 100, 255}; float density = 0.08f, ambient = 0.8f, vignette = 0.6f, desat = 0.3f; bool dark = false;
    std::vector<Blk> boxes; std::vector<Tree> trees; std::vector<Cam> cams; std::vector<Pickup> picks;
    Vector2 spawn{0, 0}; float yaw = 3.14159f; Rectangle exit{0, 0, 0, 0};
};
enum ItemId { I_DUCK, I_COMMIT, I_PLANK, I_DRINK, I_RADIO, I_BADGE, I_BACKUP, I_ADMIN, I_COUNT };
enum Flag { F_RESTROOM = 1, F_OVERLOOK, F_ANGELA, F_STREAKS, F_SHADOW, F_HEAP, F_PYRAMID, F_RADIO_TALK,
    F_APARTMENTS = 20, F_APARTMENT_ANGELA, F_PARK, F_DATACENTER, F_MAZE, F_HOTEL, F_HOTEL_TRUTH,
    F_FINAL_INTRO, F_FINAL_BOSS, F_LOG, F_ENDING_SEEN, F_DATA_MONSTER, F_DATA_LOCK, F_HOTEL_LOCK };
enum St { S_TITLE, S_PLAY, S_CUT, S_INV, S_FIGHT, S_PAUSE, S_DEAD, S_END, S_CHOOSE, S_GATE };

inline float Dist(Vector2 a, Vector2 b) { return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)); }
inline float Flicker(float t) {
    float f = 0.86f + 0.14f * std::sin(t * 37.0f) * std::sin(t * 13.7f);
    if (std::fmod(t, 7.3f) < 0.16f) f *= 0.35f;
    return std::max(0.25f, std::min(1.0f, f));
}
inline Color C(int r, int g, int b, int a = 255) { return Color{(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a}; }

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
    float lh = fs * 1.35f; int budget = shown;
    for (size_t i = 0; i < lines.size() && budget > 0; i++) {
        std::string part = lines[i].substr(0, std::min((size_t)budget, lines[i].size()));
        float x = cx - TxtW(lines[i], fs) / 2;
        Txt(part, x + 2, y + i * lh + 2, fs, Al(C(0, 0, 0), 0.85f * c.a / 255.0f));
        Txt(part, x, y + i * lh, fs, c);
        budget -= (int)lines[i].size() + 1;
    }
}

inline std::vector<Line> ScriptRestroom() {
    return {{"JAMES", "Mary... could you really be on this server?"},
            {"JAMES", "A pull request showed up three days ago. The author was her. My wife's username."},
            {"JAMES", "Dead accounts don't push code. That is what I keep telling myself."}};
}
inline std::vector<Line> ScriptOverlook() {
    return {{"", "Fog sits on the lake like a loading screen."},
            {"MARY", "I can still see that repo whenever I sleep, James. Port 3000. You said you would redeploy me there someday."},
            {"MARY", "I'm on my own in it now. Our special repository. Waiting for you."},
            {"JAMES", "Her handle was on the commit. Three years since she crashed, and I'm following a hash."},
            {"JAMES", "If she left a branch anywhere, it's the /park endpoint. We spent a whole sprint there, just the two of us, staring at the console."},
            {"JAMES", "Is she really running somewhere... waiting for me?"}};
}
inline std::vector<Line> ScriptAngela() {
    return {{"ANGELA", "A-ah! Sorry! I thought you were a merge conflict."},
            {"JAMES", "No, I'm sorry I startled you. I think I'm lost."},
            {"ANGELA", "Lost? You don't have GPS?"},
            {"JAMES", "I'm looking for localhost:3000. Is this the right port?"},
            {"ANGELA", "It's hard to tell in this fog, but there's only one localhost. You can't miss it. Just... don't open it in Internet Explorer."},
            {"JAMES", "Thanks."},
            {"ANGELA", "But I'd stay away, if I were you. Something is wrong with that server. The console keeps screaming."},
            {"JAMES", "Is it dangerous?"},
            {"ANGELA", "Maybe. And not just the memory leaks. I've seen stack traces that go on for days."},
            {"JAMES", "I'll be careful. I brought a rubber duck."},
            {"ANGELA", "I'm serious!"},
            {"JAMES", "I believe you. But I'm going either way."},
            {"ANGELA", "Why?"},
            {"JAMES", "I'm looking for someone very important to me."},
            {"ANGELA", "Me too. My mother. She wrote the original docs. I haven't read a README that explained anything since."},
            {"ANGELA", "Sorry. Not your problem. I'll just... npm install my feelings."},
            {"JAMES", "I hope you find her."},
            {"ANGELA", "You too. May your builds be green."}};
}

class Game3D {
public:
    explicit Game3D(const std::vector<Topic>* cpp) {
        BuildPools(cpp);
        audio.Init(); rend.Init();
        audio.Play("stinger");
        std::ifstream f("ee_save.dat"); hasSave = f.good();
        names[I_DUCK] = "Rubber Duck";   descs[I_DUCK] = "A rubber duck for debugging. It has heard everything.";
        names[I_COMMIT] = "Mary's Commit"; descs[I_COMMIT] = "A screenshot of a pull request. Author: Mary. Her last real commit was three years ago.";
        names[I_PLANK] = "Plank with Nails"; descs[I_PLANK] = "A plank with nails through one end. Basically a mechanical keyboard missing its keycaps.";
        names[I_DRINK] = "Health Drink"; descs[I_DRINK] = "Restores health. Tastes like cold coffee.";
        names[I_RADIO] = "Radio"; descs[I_RADIO] = "It hisses when something is near. Or it is just a podcast.";
        names[I_BADGE] = "Maintenance Badge"; descs[I_BADGE] = "A faded badge. It opens the Brookhaven server room.";
        names[I_BACKUP] = "Backup Tape"; descs[I_BACKUP] = "The original restoration image. Whether you should use it is another question.";
        names[I_ADMIN] = "Admin Log"; descs[I_ADMIN] = "The author signed off every midnight commit with a tiny paw print.";
        TraceLog(LOG_INFO, "SH2: easter egg ready (rt=%d)", (int)rend.Ready());
    }
    ~Game3D() { audio.Shutdown(); rend.Shutdown(); }

    bool Frame(float dt) {
        if (dt > 0.05f) dt = 0.05f;
        time += dt;
        audio.Update();
        hurt = std::max(0.0f, hurt - dt * 1.6f);

        if (!rend.Ready()) {
            ClearBackground(C(10, 10, 14));
            BeginMode2D(CanvasCam());
            TxtC("3D rendering unavailable", kCanvasW / 2.0f, kCanvasH / 2.0f - 40, 32, C(220, 220, 220));
            TxtC("The easter egg needs OpenGL 3.3 or newer.", kCanvasW / 2.0f, kCanvasH / 2.0f + 10, 18, C(160, 160, 160));
            TxtC("Press Enter or Esc to return to C++ Quest.", kCanvasW / 2.0f, kCanvasH / 2.0f + 46, 16, C(120, 120, 120));
            EndMode2D();
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) wantExit = true;
            return !wantExit;
        }

        // Detach from any outer canvas transform the host may have pushed. Harmless when the
        // outer transform is identity (window == canvas), and correct when it is not.
        rlDrawRenderBatchActive();
        EndScissorMode();
        rlPushMatrix();
        rlLoadIdentity();

        switch (st) {
            case S_TITLE: UpdateTitle(); break;
            case S_PLAY:  UpdatePlay(dt); break;
            case S_CUT:   UpdateCut(dt); break;
            case S_INV:   UpdateInv(); break;
            case S_FIGHT: UpdateFight(dt); break;
            case S_PAUSE:
                if (IsKeyPressed(KEY_ESCAPE)) st = S_PLAY;
                if (IsKeyPressed(KEY_Q)) wantExit = true;
                break;
            case S_DEAD:
                if (IsKeyPressed(KEY_Y)) { hp = 60; LoadArea(areaIdx, true); }
                if (IsKeyPressed(KEY_N)) wantExit = true;
                break;
            case S_END:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) wantExit = true;
                break;
            case S_CHOOSE: UpdateEndingChoice(); break;
            case S_GATE: UpdateGate(); break;
        }

        if (st != S_TITLE) { UpdateAudio(dt); FadeStep(dt); }
        Render();

        rlPopMatrix();
        return !wantExit;
    }

private:
    Audio audio; Renderer rend; Rng rng;
    int st = S_TITLE, menuSel = 0;
    bool wantExit = false, hasSave = false;
    float time = 0, hurt = 0, fadeA = 0;
    int changeTo = -1;

    Area area; int areaIdx = 0, curCam = 0;
    Vector3 camTarget{0, 1, 0};
    IndoorCameraRig indoorCam;

    // The restroom has a bespoke small-room camera. Later indoor chapters
    // share the long corridor geometry and use the safe tracking rig.
    bool HasCorridorCamera() const { return areaIdx >= 4 && areaIdx != 5; }
    struct Actor { Vector2 p{0, 0}; float yaw = 0, phase = 0, moving = 0; } J;
    int hp = 100; bool flash = true;
    float stepT = 0, heartT = 0;
    std::vector<Enemy> enemies;
    std::set<int> flags;
    int inv[I_COUNT] = {1, 1, 0, 0, 0};
    const char* names[I_COUNT]; const char* descs[I_COUNT];

    std::vector<Line> cut; int cutIdx = 0; float cutT = 0; Vector3 cutPos{0, 0, 0}, cutTgt{0, 0, 0}; float cutFov = 45;
    std::function<void()> cutAfter;
    std::string notice, subWho, subText; float noticeT = 0, subT = 0, hintT = 0, streakT = -1;
    float shadowT = -1; bool shadowSpoke = false;
    int invSel = 0;
    const Question* q = nullptr; std::vector<int> order; int fightE = -1, sel = 0; bool answered = false, correct = false; float resT = 0;
    std::vector<std::vector<const Question*>> pool;
    int endingSel = 0; Ending ending = Ending::Leave; bool objectiveHud = true;
    float objectiveT = 0, lockNoticeT = 0;
    TerminalLock terminal;

    static Camera2D CanvasCam() {
        int sw = GetScreenWidth(), sh = GetScreenHeight();
        float cw = (float)kCanvasW, ch = (float)kCanvasH;
        float s = std::min((float)sw / cw, (float)sh / ch);
        if (!(s > 0.01f) || s > 100.0f) s = 1.0f;
        Camera2D cam = {0};
        cam.offset = {((float)sw - cw * s) * 0.5f, ((float)sh - ch * s) * 0.5f};
        cam.target = {0, 0};
        cam.zoom = s;
        cam.rotation = 0;
        return cam;
    }

    void BuildPools(const std::vector<Topic>* cpp) {
        if (cpp) for (const Topic& t : *cpp) {
            std::vector<const Question*> v;
            for (const Module& m : t.modules) for (const Question& x : m.questions) v.push_back(&x);
            for (const Question& x : t.test.questions) v.push_back(&x);
            pool.push_back(v);
        }
    }
    const Question* PickQ(int maxTopic) {
        static std::vector<Question> fb = {
            Q("Which function does every C++ program start in?", {"main", "start", "run", "begin"}, "Execution begins at main."),
            Q("Which operator gives the remainder of a division?", {"%", "/", "//", "mod"}, "% is the remainder operator."),
            Q("What does std::cout << 5 << 3; print?", {"53", "8", "5 3", "An error"}, "Each value is printed in turn."),
            Q("Which keyword makes a value impossible to change?", {"const", "static", "final", "fixed"}, "const makes it read-only.")};
        std::vector<const Question*> cand;
        for (int t = 0; t <= maxTopic && t < (int)pool.size(); t++) for (auto* x : pool[t]) cand.push_back(x);
        if (cand.empty()) for (auto& x : fb) cand.push_back(&x);
        const Question* pick = cand[(size_t)(rng.F() * cand.size()) % cand.size()];
        if (pick == q && cand.size() > 1) pick = cand[(size_t)(rng.F() * cand.size()) % cand.size()];
        return pick;
    }

    static void B(Area& a, float x, float y, float z, float w, float h, float d, Color c, bool solid = false) { a.boxes.push_back({{x, y, z}, {w, h, d}, c, solid}); }
    static float H(float i) { float s = std::sin(i * 12.9898f) * 43758.5453f; return s - std::floor(s); }

    Area MakeRestroom() const {
        Area a; a.fog = C(14, 17, 16); a.density = 0.075f; a.ambient = 0.78f; a.vignette = 0.75f; a.desat = 0.35f; a.dark = true;
        Color wall = C(92, 104, 98);
        B(a, 0, -0.05f, 0, 8, 0.1f, 6.4f, C(86, 98, 92)); B(a, 0, 3.05f, 0, 8.2f, 0.1f, 6.6f, C(40, 46, 44));
        B(a, 0, 1.5f, -3.25f, 8.4f, 3, 0.2f, wall, true); B(a, -4.1f, 1.5f, 0, 0.2f, 3, 6.6f, wall, true); B(a, 4.1f, 1.5f, 0, 0.2f, 3, 6.6f, wall, true);
        B(a, -2.4f, 1.5f, 3.25f, 3.6f, 3, 0.2f, wall, true); B(a, 2.4f, 1.5f, 3.25f, 3.6f, 3, 0.2f, wall, true); B(a, 0, 1.1f, 3.3f, 1.2f, 2.2f, 0.1f, C(30, 30, 28));
        B(a, 0, 0.45f, -2.75f, 2.6f, 0.9f, 0.7f, C(168, 170, 162), true); B(a, 0, 0.92f, -2.75f, 1.2f, 0.04f, 0.45f, C(70, 74, 72)); B(a, 0, 1.05f, -3.0f, 0.08f, 0.2f, 0.08f, C(150, 154, 160));
        B(a, 0, 1.85f, -3.12f, 1.9f, 1.15f, 0.12f, C(26, 28, 30)); B(a, 0, 1.85f, -3.05f, 1.65f, 0.9f, 0.02f, C(120, 175, 195, 250));
        for (float z : {-2.0f, -0.2f, 1.6f}) B(a, 3.1f, 1.0f, z, 1.8f, 2.0f, 0.08f, C(120, 130, 124), true);
        B(a, 0, 2.95f, 0, 2.2f, 0.08f, 0.2f, C(230, 235, 225, 250)); B(a, -1.5f, 0.01f, 0.5f, 1.2f, 0.01f, 1.0f, C(50, 54, 48));
        a.cams.push_back({{0, 2.4f, 3.0f}, {0, 1.1f, -2.5f}, 55, {-4, -3.3f, 8, 6.6f}});
        a.spawn = {0, -2.0f}; a.yaw = 0; a.exit = {-0.6f, 2.7f, 1.2f, 0.8f};
        return a;
    }
    Area MakeOverlook() const {
        Area a; a.fog = C(146, 154, 150); a.density = 0.035f; a.ambient = 1.05f; a.vignette = 0.5f; a.desat = 0.35f;
        B(a, 0, -0.1f, 0, 70, 0.2f, 60, C(74, 90, 64)); B(a, 0, 0.0f, 0, 4.4f, 0.03f, 44, C(120, 112, 96)); B(a, -30, -0.18f, 0, 44, 0.1f, 60, C(70, 88, 98));
        for (float z = -14; z <= 14; z += 2) B(a, -3.2f, 0.55f, z, 0.1f, 1.1f, 0.1f, C(90, 76, 60));
        B(a, -3.2f, 1.05f, 0, 0.08f, 0.08f, 30, C(100, 84, 66));
        for (int i = 0; i < 15; i++) a.trees.push_back({5.0f + H(i) * 6, -24.0f + i * 3.2f + H(i + 20) * 2, 4.0f + H(i + 40) * 3});
        B(a, -3.6f, 1, 0, 0.4f, 2, 48, C(0, 0, 0, 0), true); B(a, 5.0f, 1, 0, 0.4f, 2, 48, C(0, 0, 0, 0), true); B(a, 0, 1, 24, 12, 2, 0.4f, C(0, 0, 0, 0), true);
        B(a, 0, 1.6f, 27, 8, 3.2f, 4, C(98, 100, 94));
        a.cams.push_back({{7, 3.4f, 19}, {-2, 1.4f, 6}, 58, {-8, 8, 16, 16}});
        a.cams.push_back({{5, 2.8f, 6}, {0, 1.4f, -6}, 55, {-8, -6, 16, 14}});
        a.cams.push_back({{3, 2.6f, -8}, {0, 1.4f, -20}, 55, {-8, -24, 16, 18}});
        a.spawn = {0, 16}; a.exit = {-2.2f, -22, 4.4f, 2};
        return a;
    }
    Area MakeCemetery() const {
        Area a; a.fog = C(118, 124, 120); a.density = 0.07f; a.ambient = 0.85f; a.vignette = 0.6f; a.desat = 0.4f;
        B(a, 0, -0.1f, 0, 60, 0.2f, 60, C(66, 74, 60)); B(a, 0, 0.0f, 0, 3, 0.03f, 40, C(96, 92, 84));
        for (float x : {-6.0f, -3.5f, 3.5f, 6.0f}) for (float z = -2; z >= -14; z -= 3) {
            B(a, x, 0.45f, z, 0.9f, 0.9f, 0.15f, C(128, 130, 126), true); B(a, x, 0.08f, z + 0.9f, 0.9f, 0.16f, 1.6f, C(58, 50, 40));
        }
        B(a, -9, 0.8f, -8, 0.1f, 1.6f, 40, C(40, 40, 44), true); B(a, 9, 0.8f, -8, 0.1f, 1.6f, 40, C(40, 40, 44), true);
        B(a, -5.5f, 0.8f, -20, 7, 1.6f, 0.1f, C(40, 40, 44), true); B(a, 5.5f, 0.8f, -20, 7, 1.6f, 0.1f, C(40, 40, 44), true);
        B(a, 0, 1, 22, 20, 2, 0.4f, C(0, 0, 0, 0), true);
        for (int i = 0; i < 12; i++) a.trees.push_back({(i % 2 ? 11.0f : -11.0f) + H(i) * 3, -18.0f + i * 3.5f, 5.0f + H(i + 7) * 3});
        a.cams.push_back({{-8, 3.2f, 12}, {0, 1.2f, -4}, 55, {-10, -2, 20, 24}});
        a.cams.push_back({{-6, 2.8f, -4}, {2, 1.0f, -12}, 55, {-10, -22, 20, 20}});
        a.spawn = {0, 16}; a.exit = {-2, -21, 4, 1.6f};
        return a;
    }
    Area MakeRoad() const {
        Area a; a.fog = C(92, 96, 94); a.density = 0.09f; a.ambient = 0.62f; a.vignette = 0.75f; a.desat = 0.4f; a.dark = true;
        B(a, 0, -0.05f, -26, 8, 0.1f, 100, C(44, 46, 48)); B(a, -5.2f, 0.04f, -26, 2.4f, 0.14f, 100, C(82, 84, 82)); B(a, 5.2f, 0.04f, -26, 2.4f, 0.14f, 100, C(82, 84, 82));
        for (int i = 0; i < 10; i++) for (int side = -1; side <= 1; side += 2) {
            float h = 7 + H(i * 2 + side) * 5, z = -4.0f - i * 9;
            B(a, side * 12.0f, h / 2, z, 8, h, 7.5f, C(58 + (int)(H(i + side) * 24), 60 + (int)(H(i * 3) * 20), 62), false);
            B(a, side * 7.95f, 2.5f, z, 0.06f, 1.0f, 1.2f, C(18, 20, 22)); B(a, side * 7.95f, 2.5f, z - 2.4f, 0.06f, 1.0f, 1.2f, C(18, 20, 22));
        }
        for (int k = 0; k < 15; k++) B(a, -2.8f + H(k) * 5.6f, 0.01f, -3.0f - k * 3.1f, 1 + H(k + 5) * 2.2f, 0.01f, 0.1f, C(150, 28, 28));
        B(a, -5.8f, 0.7f, -22, 0.1f, 1.4f, 6, C(70, 60, 48), true); B(a, 3.4f, 0.6f, -12, 2, 1.2f, 4.2f, C(70, 72, 76), true);
        B(a, -6.6f, 1.5f, -26, 0.4f, 3, 100, C(0, 0, 0, 0), true); B(a, 6.6f, 1.5f, -26, 0.4f, 3, 100, C(0, 0, 0, 0), true);
        B(a, 0, 1.5f, 8, 14, 3, 0.4f, C(0, 0, 0, 0), true); B(a, 0, 1.5f, -58, 14, 3, 0.4f, C(0, 0, 0, 0), true);
        a.picks = {{{-3.8f, -6.0f}, 1}, {{4.2f, -9.0f}, 2}, {{-5.0f, -22.0f}, 3}, {{0, 1.5f}, 4}};
        a.cams.push_back({{0, 3.4f, 8}, {0, 1.2f, -14}, 58, {-8, -9, 16, 17}});
        a.cams.push_back({{0, 3.2f, -8}, {0, 1.2f, -30}, 58, {-8, -27, 16, 18}});
        a.cams.push_back({{0, 3.2f, -26}, {0, 1.2f, -50}, 58, {-8, -58, 16, 31}});
        a.spawn = {0, 4}; a.exit = {-6, -57, 12, 1.5f};
        return a;
    }
    // Later acts reuse a bounded corridor layout but each location gets a distinct
    // environment, lighting, landmarks, pickups and a local cinematic camera.
    Area MakeInterior(int chapter) const {
        Area a; a.dark = true; a.fog = chapter == 6 ? C(38, 52, 56) : chapter == 7 ? C(52, 40, 39) : C(70, 65, 59);
        a.density = chapter == 7 ? 0.075f : 0.055f;
        a.ambient = chapter == 6 ? 0.48f : chapter == 7 ? 0.45f : 0.65f;
        a.vignette = 0.76f; a.desat = 0.43f;
        const Color wall = chapter == 6 ? C(60, 72, 73) : chapter == 7 ? C(76, 55, 49) : C(107, 94, 83);
        const Color floor = chapter == 6 ? C(35, 45, 49) : chapter == 7 ? C(58, 47, 43) : C(76, 72, 66);
        B(a, 0, -0.09f, -11, 14, 0.15f, 40, floor);
        B(a, -7, 1.6f, -11, 0.3f, 3.4f, 40, wall, true);
        B(a, 7, 1.6f, -11, 0.3f, 3.4f, 40, wall, true);
        B(a, 0, 3.3f, -11, 14.3f, 0.1f, 40.4f, Mix(wall, C(0,0,0), 0.45f));
        B(a, 0, 1.4f, 9, 14, 2.8f, 0.2f, wall, true);
        for (int i = 0; i < 7; ++i) {
            float z = 4.0f - i * 4.8f;
            float side = (i % 2) ? 1.0f : -1.0f;
            Color object = chapter == 6 ? C(35, 59, 66) : chapter == 7 ? C(75, 58, 50) : C(104, 95, 85);
            B(a, side * 5.25f, 0.88f, z, 1.8f, 1.75f, 1.15f, object, true);
            // A second surface above the shelves gives the hall a recognisable silhouette.
            B(a, side * 5.25f, 1.8f, z, 1.95f, 0.09f, 1.25f, C(25, 28, 29));
            if (chapter == 6) {
                B(a, side * 5.25f, 1.05f, z - 0.58f, 0.6f, 0.6f, 0.03f, C(20, 98, 98));
            }
            if (chapter == 7) B(a, side * 5.2f, 0.02f, z - 1, 1.7f, 0.03f, 0.15f, C(126, 54, 43));
        }
        B(a, 0, 1.6f, -31, 14, 3.2f, 0.3f, wall, true);
        B(a, -3, 1.1f, -30.8f, 2.0f, 2.1f, 0.1f, C(22, 25, 25));
        B(a, 3, 1.1f, -30.8f, 2.0f, 2.1f, 0.1f, C(22, 25, 25));
        // These shots define camera zones and shoulder direction. The actual
        // position tracks James, below the roof and away from the shelves.
        a.cams.push_back({{2.5f, 2.05f, 7.5f}, {0, 1.1f, -7}, 62, {-7, -11, 14, 21}});
        a.cams.push_back({{-2.5f, 2.05f, -8}, {0, 1.2f, -24}, 62, {-7, -31, 14, 20}});
        a.spawn = {0, 5}; a.yaw = kPi; a.exit = {-2, -29.6f, 4, 1.8f};
        a.picks = {{{0, 1.5f}, 4}};
        if (chapter == 4) { a.picks.push_back({{-3.3f, -9.0f}, 5}); a.picks.push_back({{3, -17}, 2}); }
        if (chapter == 6) { a.picks.push_back({{3, -9}, 2}); }
        if (chapter == 7) { a.picks.push_back({{-3, -20}, 6}); }
        if (chapter == 8) { a.picks.push_back({{3, -12}, 7}); a.picks.push_back({{-3, -22}, 2}); }
        return a;
    }
    Area MakePark() const {
        Area a; a.fog = C(121, 135, 139); a.density = 0.063f; a.ambient = 0.86f;
        a.vignette = 0.65f; a.desat = 0.38f;
        B(a, 0, -0.13f, -10, 26, 0.2f, 48, C(75, 85, 68));
        B(a, 0, 0, -10, 4, 0.06f, 45, C(113, 111, 102));
        B(a, -18, -0.17f, -10, 12, 0.1f, 48, C(54, 86, 93));
        for (int i = 0; i < 12; ++i) {
            float z = 11 - i * 3.2f;
            B(a, -3.3f, 0.5f, z, 0.12f, 1.0f, 0.12f, C(86, 78, 65));
            a.trees.push_back({6.0f + H(i + 34) * 4.0f, z + H(i + 20), 4.0f + H(i + 22) * 3.0f});
        }
        B(a, -3.3f, 0.97f, -10, 0.08f, 0.07f, 43, C(91, 83, 70));
        B(a, 0, 1.2f, 17, 12, 2.4f, 0.1f, C(0,0,0,0), true);
        B(a, 0, 1.2f, -33, 12, 2.4f, 0.1f, C(0,0,0,0), true);
        B(a, 6.5f, 1.0f, -10, 0.2f, 2, 48, C(0,0,0,0), true);
        B(a, -5.0f, 1.0f, -10, 0.2f, 2, 48, C(0,0,0,0), true);
        a.cams.push_back({{6.5f, 3.0f, 10}, {0, 1.2f, -4}, 60, {-5, -9, 11, 26}});
        a.cams.push_back({{5.5f, 3.2f, -9}, {0, 1.2f, -24}, 60, {-5, -32, 11, 23}});
        a.spawn = {0, 12}; a.yaw = kPi; a.exit = {-2, -31.0f, 4, 2.0f};
        a.picks = {{{0, 5}, 4}};
        return a;
    }
    Area MakeFinalRoom() const {
        Area a = MakeInterior(7);
        a.fog = C(61, 31, 31); a.ambient = 0.45f; a.density = 0.09f;
        a.picks.clear(); a.picks.push_back({{0, 2}, 4});
        B(a, 0, 1.1f, -27, 2.5f, 2.2f, 1.0f, C(105, 70, 65));
        B(a, 0, 2.0f, -26.4f, 1.7f, 0.9f, 0.05f, C(108, 181, 156));
        return a;
    }
    Area MakeArea(int i) const {
        switch (i) {
        case 0: return MakeRestroom(); case 1: return MakeOverlook();
        case 2: return MakeCemetery(); case 3: return MakeRoad();
        case 5: return MakePark(); case 9: return MakeFinalRoom();
        default: return MakeInterior(i);
        }
    }


    void LoadArea(int idx, bool respawn = false) {
        area = MakeArea(idx); areaIdx = idx; curCam = 0;
        J.p = area.spawn; J.yaw = area.yaw; J.moving = 0;
        enemies.clear();
        if (idx == 3) {
            Enemy h; h.kind = 0; h.p = h.home = {0, -30}; if (!flags.count(F_HEAP)) enemies.push_back(h);
            Enemy p; p.kind = 1; p.p = p.home = {0, -52}; p.yaw = 0; enemies.push_back(p);
        }
        if (idx == 6 && !flags.count(F_DATA_MONSTER)) {
            Enemy e; e.kind = 0; e.p = e.home = {0, -16}; e.active = true; enemies.push_back(e);
        }
        if (idx == 9 && !flags.count(F_FINAL_BOSS)) {
            Enemy boss; boss.kind = 2; boss.p = boss.home = {0, -24}; boss.active = true; enemies.push_back(boss);
        }
        camTarget = {J.p.x, 1.15f, J.p.y};
        if (HasCorridorCamera()) indoorCam.Reset(J.p.x, J.p.y, curCam);
        shadowT = -1; streakT = idx == 3 && !flags.count(F_STREAKS) ? 2.5f : -1;
        hintT = 9; objectiveT = 10; lockNoticeT = 0;
        if (respawn) { st = S_PLAY; return; }
        st = S_PLAY;
        if (idx == 0 && !flags.count(F_RESTROOM)) {
            flags.insert(F_RESTROOM);
            StartCut(ScriptRestroom(), {0.45f, 1.75f, -0.4f}, {0, 1.8f, -3.1f}, 42, [this] { hintT = 9; });
        } else if (idx == 1 && !flags.count(F_OVERLOOK)) {
            flags.insert(F_OVERLOOK);
            StartCut(ScriptOverlook(), {3.2f, 1.7f, 18.5f}, {-4, 1.8f, 8}, 50, nullptr);
        } else if (idx == 4 && !flags.count(F_APARTMENTS)) {
            flags.insert(F_APARTMENTS);
            StartCut(ApartmentEntry(), {3, 2.4f, 3}, {0, 1.3f, -8}, 55, nullptr);
        } else if (idx == 5 && !flags.count(F_PARK)) {
            flags.insert(F_PARK); StartCut(ParkEntry(), {5.5f, 2.8f, 8}, {0, 1.2f, -12}, 58, nullptr);
        } else if (idx == 6 && !flags.count(F_DATACENTER)) {
            flags.insert(F_DATACENTER);
            StartCut(DataCenterEntry(), {-6, 2.7f, 6}, {0, 1.2f, -13}, 52, nullptr);
        } else if (idx == 7 && !flags.count(F_MAZE)) {
            flags.insert(F_MAZE); StartCut(MazeEntry(), {6, 3, 5}, {0, 1.2f, -20}, 54, nullptr);
        } else if (idx == 8 && !flags.count(F_HOTEL)) {
            flags.insert(F_HOTEL); StartCut(HotelEntry(), {-6, 3, 5}, {0, 1.2f, -22}, 51, nullptr);
        } else if (idx == 9 && !flags.count(F_FINAL_INTRO)) {
            flags.insert(F_FINAL_INTRO); StartCut(FinalEntry(), {6, 2.7f, 5}, {0, 1.3f, -23}, 54, nullptr);
        }
        if (idx >= 4 && !respawn) Save();
    }
    void StartCut(std::vector<Line> lines, Vector3 pos, Vector3 tgt, float fov, std::function<void()> after) {
        if (HasCorridorCamera()) {
            const CameraPoint safe = IndoorCameraRig::SafeCut({pos.x, pos.y, pos.z});
            pos = {safe.x, safe.y, safe.z};
        }
        cut = std::move(lines); cutIdx = 0; cutT = 0; cutPos = pos; cutTgt = tgt; cutFov = fov; cutAfter = std::move(after); st = S_CUT;
    }
    void GoTo(int idx) { if (changeTo < 0) changeTo = idx; }
    void FadeStep(float dt) {
        if (changeTo >= 0) { fadeA += dt / 0.8f; if (fadeA >= 1) { fadeA = 1; int n = changeTo; changeTo = -1; if (n >= (int)Chapter::Count) { st = S_CHOOSE; } else LoadArea(n); } }
        else if (fadeA > 0) fadeA = std::max(0.0f, fadeA - dt / 0.8f);
    }

    void Save() {
        std::ofstream f("ee_save.dat", std::ios::trunc); if (!f) return;
        f << "EE2 " << areaIdx << ' ' << hp;
        for (int i = 0; i < I_COUNT; i++) f << ' ' << inv[i];
        f << ' ' << flags.size(); for (int x : flags) f << ' ' << x;
        f << '\n'; hasSave = true;
    }
    bool LoadSave() {
        std::ifstream f("ee_save.dat"); if (!f) return false;
        std::string version; if (!(f >> version)) return false;
        int a = 0, loadedHp = 100, n = 0;
        const bool modern = version == "EE2";
        if (modern) { if (!(f >> a >> loadedHp)) return false; }
        else { // Old EE saves were an unversioned chapter number and five inventory fields.
            try { a = std::stoi(version); } catch (...) { return false; }
            if (!(f >> loadedHp)) return false;
        }
        int items[I_COUNT] = {1, 1, 0, 0, 0, 0, 0, 0};
        const int count = modern ? I_COUNT : 5;
        for (int i = 0; i < count; ++i) if (!(f >> items[i]) || items[i] < 0 || items[i] > 50) return false;
        if (!(f >> n) || n < 0 || n > 256 || a < 0 || a >= (int)Chapter::Count) return false;
        std::set<int> loadedFlags;
        for (int i = 0; i < n; ++i) { int id; if (!(f >> id)) return false; loadedFlags.insert(id); }
        for (int i = 0; i < I_COUNT; ++i) inv[i] = items[i];
        flags = std::move(loadedFlags); hp = std::max(1, std::min(100, loadedHp));
        LoadArea(a, true); hintT = 10; return true;
    }

    void UpdateTitle() {
        audio.PlayMusic("title"); audio.Loop("drone", 0.25f);
        if (IsKeyPressed(KEY_ESCAPE)) { wantExit = true; return; }
        int n = hasSave ? 3 : 2;
        if (IsKeyPressed(KEY_UP) && menuSel > 0) { menuSel--; audio.Play("tick", 0.6f); }
        if (IsKeyPressed(KEY_DOWN) && menuSel < n - 1) { menuSel++; audio.Play("tick", 0.6f); }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            audio.Play("select", 0.8f);
            static const int withSave[3] = {0, 1, 2}, noSave[2] = {0, 2};
            int choice = hasSave ? withSave[menuSel] : noSave[menuSel];
            if (choice == 0) {
                hp = 100;
                flags.clear();
                for (int i = 0; i < I_COUNT; i++) inv[i] = (i <= I_COMMIT);
                ending = Ending::Leave; endingSel = 0;
                fadeA = 0;
                changeTo = -1;
                LoadArea(0);
            }
            else if (choice == 1) { if (!LoadSave()) { hp = 100; LoadArea(0); } }
            else wantExit = true;
        }
    }

    void Collide(Vector2& p, float r) const {
        for (int it = 0; it < 2; it++) for (const Blk& b : area.boxes) {
            if (!b.solid) continue;
            float minx = b.c.x - b.s.x / 2, maxx = b.c.x + b.s.x / 2, minz = b.c.z - b.s.z / 2, maxz = b.c.z + b.s.z / 2;
            float cx = std::max(minx, std::min(p.x, maxx)), cz = std::max(minz, std::min(p.y, maxz));
            float dx = p.x - cx, dz = p.y - cz, d2 = dx * dx + dz * dz;
            if (d2 >= r * r) continue;
            if (d2 > 1e-6f) { float d = std::sqrt(d2); p.x = cx + dx / d * r; p.y = cz + dz / d * r; }
            else {
                float l = p.x - minx, rr = maxx - p.x, t = p.y - minz, bt = maxz - p.y, m = std::min(std::min(l, rr), std::min(t, bt));
                if (m == l) p.x = minx - r; else if (m == rr) p.x = maxx + r; else if (m == t) p.y = minz - r; else p.y = maxz + r;
            }
        }
    }
    Vector2 Fwd() const { return {std::sin(J.yaw), std::cos(J.yaw)}; }
    static bool In(Rectangle r, Vector2 p) { return p.x >= r.x && p.x <= r.x + r.width && p.y >= r.y && p.y <= r.y + r.height; }

    void Sub(const std::string& who, const std::string& text, float secs = 4.5f) { subWho = who; subText = text; subT = secs; }
    void Notice(const std::string& s) { notice = s; noticeT = 3.0f; }

    void UpdatePlay(float dt) {
        bool Lk = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A), Rk = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
        bool Uk = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W), Dk = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
        bool run = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        J.yaw += ((Lk ? 1.0f : 0.0f) - (Rk ? 1.0f : 0.0f)) * 2.5f * dt;
        float weak = hp < 30 ? 0.7f : 1.0f;
        float spd = Uk ? (run ? 4.2f : 1.9f) * weak : (Dk ? -1.1f : 0.0f);
        Vector2 f = Fwd();
        J.p.x += f.x * spd * dt; J.p.y += f.y * spd * dt;
        Collide(J.p, 0.35f);
        J.moving = Smooth(J.moving, spd != 0 ? 1.0f : 0.0f, 12, dt);
        J.phase += dt * (run && Uk ? 10.0f : 6.0f) * (spd != 0 ? 1.0f : 0.0f);
        stepT -= dt;
        if (spd != 0 && stepT <= 0) { audio.Play((int)(J.phase) % 2 ? "step1" : "step0", 0.45f); stepT = (run && Uk) ? 0.30f : 0.52f; }

        for (size_t i = 0; i < area.cams.size(); i++) if (In(area.cams[i].trig, J.p)) { curCam = (int)i; break; }
        if (HasCorridorCamera()) indoorCam.Track(J.p.x, J.p.y, curCam, dt);
        camTarget.x = Lerpf(camTarget.x, J.p.x, 0.10f); camTarget.z = Lerpf(camTarget.z, J.p.y, 0.10f); camTarget.y = 1.15f;

        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) TryAction();
        if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_I)) { st = S_INV; audio.Play("open", 0.8f); invSel = 0; }
        if (IsKeyPressed(KEY_F)) { flash = !flash; audio.Play("tick", 0.5f); }
        if (IsKeyPressed(KEY_ESCAPE)) st = S_PAUSE;
        if (subT > 0) subT -= dt;
        if (noticeT > 0) noticeT -= dt;
        if (hintT > 0) hintT -= dt;
        if (objectiveT > 0) objectiveT -= dt;
        if (lockNoticeT > 0) lockNoticeT -= dt;
        if (IsKeyPressed(KEY_H)) objectiveHud = !objectiveHud;

        if (changeTo < 0) {
            if (areaIdx < 3 && In(area.exit, J.p)) GoTo(areaIdx + 1);
            if (areaIdx == 3 && J.p.y < -54.0f) GoTo(4);
            if (areaIdx >= 4 && In(area.exit, J.p)) TryExit();
            if (areaIdx == 4 && !flags.count(F_APARTMENT_ANGELA) && J.p.y < -15) {
                flags.insert(F_APARTMENT_ANGELA);
                StartCut(ApartmentAngela(), {6, 2.5f, -12}, {0, 1.3f, -18}, 54, nullptr);
            }
            if (areaIdx == 8 && !flags.count(F_HOTEL_TRUTH) && J.p.y < -15) {
                flags.insert(F_HOTEL_TRUTH);
                StartCut(HotelTruth(), {5, 2.8f, -11}, {0, 1.4f, -25}, 48, nullptr);
            }
            if (areaIdx == 2 && !flags.count(F_ANGELA) && Dist(J.p, {2.4f, -7.0f}) < 7.0f) {
                flags.insert(F_ANGELA);
                StartCut(ScriptAngela(), {-1.2f, 1.5f, -3.2f}, {2.4f, 1.0f, -7.0f}, 48, nullptr);
            }
        }
        if (areaIdx == 3) UpdateRoadEvents(dt);
        UpdateEnemies(dt);
    }

    void TryExit() {
        if (areaIdx == 4 && !inv[I_BADGE]) { BlockExit("The exit keypad needs a maintenance badge. Search the apartments."); return; }
        if (areaIdx == 6 && !flags.count(F_DATA_MONSTER)) { BlockExit("The cable creature still guards the maintenance route."); return; }
        if (areaIdx == 6 && !flags.count(F_DATA_LOCK)) { OpenTerminal(6); return; }
        if (areaIdx == 7 && !inv[I_BACKUP]) { BlockExit("The locked door needs the backup tape. Search the shelves."); return; }
        if (areaIdx == 8 && !flags.count(F_HOTEL_LOCK)) { OpenTerminal(8); return; }
        if (areaIdx == 9) {
            if (flags.count(F_FINAL_BOSS)) { st = S_CHOOSE; endingSel = 0; audio.Play("open"); }
            else BlockExit("The final merge conflict still blocks the terminal.");
            return;
        }
        GoTo(areaIdx + 1);
    }
    void BlockExit(const char* text) {
        if (lockNoticeT > 0) return;
        lockNoticeT = 3.8f; Notice(text); audio.Play("tick", 0.5f, 0.7f);
    }

    void OpenTerminal(int chapter) {
        terminal.Begin(chapter); st = S_GATE; audio.Play("open", 0.6f);
    }
    void UpdateGate() {
        if (IsKeyPressed(KEY_ESCAPE)) { st = S_PLAY; return; }
        if (IsKeyPressed(KEY_UP)) terminal.Select(terminal.Selected() - 1);
        if (IsKeyPressed(KEY_DOWN)) terminal.Select(terminal.Selected() + 1);
        for (int i=0;i<4;++i) if (IsKeyPressed(KEY_ONE+i)) { terminal.Select(i); break; }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (terminal.Confirm()) {
                flags.insert(areaIdx == 8 ? F_HOTEL_LOCK : F_DATA_LOCK);
                st = S_PLAY; audio.Play("select"); Notice("Access granted. Continue through the doorway."); Save();
            } else { audio.Play("tick", 0.6f); }
        }
    }

    void UpdateRoadEvents(float dt) {
        if (streakT > 0) { streakT -= dt; if (streakT <= 0) { flags.insert(F_STREAKS); Sub("JAMES", "Those streaks on the road... are those stack traces?"); } }
        if (!flags.count(F_SHADOW) && J.p.y < -12.0f) { flags.insert(F_SHADOW); shadowT = 0; shadowSpoke = false; }
        if (shadowT >= 0) {
            shadowT += dt;
            if (!shadowSpoke && shadowT > 5.0f) { shadowSpoke = true; Sub("JAMES", "That shadow... was that a senior developer?"); }
            if (shadowT > 9.0f) shadowT = -1;
        }
        if (!inv[I_RADIO] && Dist(J.p, {-5.0f, -22.0f}) < 14.0f && !flags.count(F_RADIO_TALK)) { flags.insert(F_RADIO_TALK); audio.Play("modem", 0.5f); }
    }

    void TryAction() {
        Vector2 f = Fwd();
        for (const Pickup& pk : area.picks) {
            if (pk.id != 4 && flags.count(100 + areaIdx * 10 + pk.id)) continue;
            float dist = Dist(J.p, pk.p);
            if (dist > 2.2f) continue;
            if (pk.id == 4) { Save(); Notice("Game saved."); audio.Play("select", 0.8f); return; }
            flags.insert(100 + areaIdx * 10 + pk.id); audio.Play("select", 0.8f);
            if (pk.id == 1) { inv[I_PLANK] = 1; Notice("Obtained: Plank with Nails"); Sub("JAMES", "Basically a mechanical keyboard missing its keycaps."); }
            if (pk.id == 2) { inv[I_DRINK]++; Notice("Obtained: Health Drink"); }
            if (pk.id == 3) {
                inv[I_RADIO] = 1; Notice("Obtained: Radio"); audio.Play("modem", 0.4f);
                Sub("JAMES", flags.count(F_HEAP) ? "I'll take it anyway. I might need it for emotional support." : "Is this thing broken? Is this a podcast?", 5.5f);
            }
            if (pk.id == 5) { inv[I_BADGE] = 1; Notice("Obtained: Maintenance Badge"); Sub("JAMES", "At least this credential has a readable name."); }
            if (pk.id == 6) { inv[I_BACKUP] = 1; Notice("Obtained: Backup Tape"); Sub("JAMES", "This might let me restore the old repository."); }
            if (pk.id == 7) { inv[I_ADMIN] = 1; flags.insert(F_LOG); Notice("Found: Admin Log"); Sub("JAMES", "All the night commits are signed... with a paw print?"); }
            return;
        }
    }

    void UpdateEnemies(float dt) {
        for (size_t i = 0; i < enemies.size(); i++) {
            Enemy& e = enemies[i];
            if (!e.alive) continue;
            if (e.dissolve > 0) { e.dissolve += dt; if (e.dissolve > 1.3f) e.alive = false; continue; }
            if (e.cool > 0) e.cool -= dt;
            if (e.kind == 0 && !e.active && J.p.y < -14.0f) e.active = true;
            if (e.kind == 1 && !e.active && flags.count(F_HEAP) && J.p.y < -36.0f) e.active = true;
            if (!e.active) continue;
            float d = Dist(e.p, J.p);
            Vector2 to{J.p.x - e.p.x, J.p.y - e.p.y};
            if (e.retreating) {
                Vector2 away{e.home.x - e.p.x, e.home.y - e.p.y}; float ad = Dist(e.home, e.p);
                if (ad > 0.3f) { e.p.x += away.x / ad * 2.0f * dt; e.p.y += away.y / ad * 2.0f * dt; }
                if (e.cool <= 0) e.retreating = false;
                continue;
            }
            if (d > 0.01f) e.yaw = std::atan2(to.x, to.y);
            float spd = e.kind == 0 ? 0.9f : 1.05f;
            if (d < 30.0f && d > 1.9f && e.cool <= 0) { e.p.x += to.x / d * spd * dt; e.p.y += to.y / d * spd * dt; }
            if (d < (e.kind == 0 ? 2.0f : 2.4f) && e.cool <= 0 && st == S_PLAY) StartFight((int)i);
        }
    }
    void StartFight(int ei) {
        fightE = ei; q = PickQ(enemies[ei].kind == 0 ? 4 : (enemies[ei].kind == 1 ? 9 : 17));
        order.assign(q->options.size(), 0);
        for (size_t i = 0; i < order.size(); i++) order[i] = (int)i;
        for (int k = (int)order.size() - 1; k > 0; k--) std::swap(order[k], order[(int)(rng.F() * (k + 1)) % (k + 1)]);
        sel = 0; answered = false; resT = 0; st = S_FIGHT; audio.Play("open", 0.6f, 0.7f);
    }
    void UpdateFight(float dt) {
        Enemy& e = enemies[fightE];
        if (!answered) {
            int n = (int)order.size();
            if (IsKeyPressed(KEY_UP) && sel > 0) { sel--; audio.Play("tick", 0.6f); }
            if (IsKeyPressed(KEY_DOWN) && sel < n - 1) { sel++; audio.Play("tick", 0.6f); }
            for (int i = 0; i < n && i < 4; i++) if (IsKeyPressed(KEY_ONE + i)) { sel = i; Answer(); return; }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) Answer();
            return;
        }
        resT += dt;
        if (resT > 0.6f && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || resT > 5.0f)) {
            if (hp <= 0) { st = S_DEAD; return; }
            if (correct && e.kind == 0) {
                if (areaIdx == 3) {
                    flags.insert(F_HEAP); Sub("JAMES", "Is it dead? What the hell was that? It's not human... it looks like jQuery.", 6.0f);
                } else if (areaIdx == 6) { flags.insert(F_DATA_MONSTER); Notice("Server pathway cleared."); }
            }
            if (correct && e.kind == 1) flags.insert(F_PYRAMID);
            if (correct && e.kind == 2) {
                flags.insert(F_FINAL_BOSS); st = S_CHOOSE; endingSel = 0; Save(); return;
            }
            st = S_PLAY;
        }
    }
    void Answer() {
        Enemy& e = enemies[fightE];
        answered = true; resT = 0; correct = order[sel] == q->correct;
        if (correct) {
            audio.Play("select", 0.9f, 0.8f);
            if (e.kind == 0 || e.kind == 2) e.dissolve = 0.01f;
            else { e.retreating = true; e.cool = 40.0f; }
        } else {
            int dmg = e.kind == 0 ? 16 : 34;
            hp -= dmg; hurt = 1.0f; audio.Play("hit", 1.0f);
            Vector2 away{e.p.x - J.p.x, e.p.y - J.p.y}; float d = std::max(0.1f, Dist(e.p, J.p));
            e.p.x += away.x / d * 2.2f; e.p.y += away.y / d * 2.2f; e.cool = 2.5f;
        }
    }

    bool EndingUnlocked(int i) const {
        return i < 3 || (i == 3 && inv[I_BACKUP] > 0) || (i == 4 && inv[I_ADMIN] > 0);
    }
    void UpdateEndingChoice() {
        if (IsKeyPressed(KEY_UP) && endingSel > 0) { --endingSel; audio.Play("tick", 0.65f); }
        if (IsKeyPressed(KEY_DOWN) && endingSel < (int)kEndingNames.size() - 1) { ++endingSel; audio.Play("tick", 0.65f); }
        if (IsKeyPressed(KEY_ESCAPE)) { st = S_PLAY; return; }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (!EndingUnlocked(endingSel)) { audio.Play("tick", 0.5f); return; }
            ending = static_cast<Ending>(endingSel);
            flags.insert(F_ENDING_SEEN);
            audio.Play("stinger", 0.8f);
            StartCut(EndingDialogue(ending), {2.8f, 2, -21}, {0, 1.5f, -27}, 45,
                [this]() { st = S_END; Save(); });
        }
    }

    std::vector<int> InvList() const { std::vector<int> v; for (int i = 0; i < I_COUNT; i++) if (inv[i] > 0) v.push_back(i); return v; }
    void UpdateInv() {
        std::vector<int> l = InvList();
        if (invSel >= (int)l.size()) invSel = std::max(0, (int)l.size() - 1);
        if (IsKeyPressed(KEY_UP) && invSel > 0) { invSel--; audio.Play("tick", 0.7f); }
        if (IsKeyPressed(KEY_DOWN) && invSel < (int)l.size() - 1) { invSel++; audio.Play("tick", 0.7f); }
        if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) && !l.empty() && l[invSel] == I_DRINK && hp < 100) {
            inv[I_DRINK]--; hp = std::min(100, hp + 40); audio.Play("select", 0.9f);
        }
        if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_I) || IsKeyPressed(KEY_ESCAPE)) { st = S_PLAY; audio.Play("open", 0.6f, 1.2f); }
    }

    static constexpr float kCps = 38.0f;
    void UpdateCut(float dt) {
        cutT += dt;
        if (IsKeyPressed(KEY_ESCAPE)) cutIdx = (int)cut.size();
        if (cutIdx >= (int)cut.size()) { st = S_PLAY; if (cutAfter) cutAfter(); return; }
        float typeT = cut[cutIdx].text.size() / kCps; bool full = cutT >= typeT;
        bool adv = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER);
        if (adv && !full) cutT = typeT + 0.001f;
        else if ((adv && full) || (full && cutT - typeT > 1.8f + cut[cutIdx].text.size() * 0.02f)) { cutIdx++; cutT = 0; }
    }

    void UpdateAudio(float dt) {
        bool indoor = ChapterAt(areaIdx).indoors, mute = st == S_TITLE;
        audio.Loop("drone", mute ? 0 : (indoor ? 0.30f : 0.16f + (areaIdx == 3 ? 0.10f : 0.0f)));
        audio.Loop("wind", mute || indoor ? 0.0f : 0.45f);
        float sv = 0;
        if (inv[I_RADIO]) { for (auto& e : enemies) if (e.alive && e.active && e.dissolve == 0) sv = std::max(sv, Clamp01(1 - Dist(e.p, J.p) / 16.0f) * 0.85f); }
        else if (areaIdx == 3) sv = Clamp01(1 - Dist(J.p, {-5.0f, -22.0f}) / 20.0f) * 0.4f;
        audio.Loop("static", st == S_PAUSE ? 0.0f : sv);
        audio.PlayMusic(indoor ? "indoors" : (areaIdx == 3 ? "town" : "outdoors"));
        heartT -= dt;
        if (hp < 35 && hp > 0 && heartT <= 0 && st == S_PLAY) { audio.Play("heart", 0.55f); heartT = 1.1f; }
    }

    static void Figure(Vector2 p, float yaw, float phase, float moving, Color jacket, Color pants, Color hair, float crouch = 0) {
        rlPushMatrix(); rlTranslatef(p.x, 0, p.y); rlRotatef(yaw * 57.29578f, 0, 1, 0);
        float sw = std::sin(phase) * moving, cy = crouch * 0.35f;
        DrawCube({-0.12f, 0.45f - crouch * 0.15f, sw * 0.22f}, 0.16f, 0.9f - crouch * 0.3f, 0.18f, pants);
        DrawCube({0.12f, 0.45f - crouch * 0.15f, -sw * 0.22f}, 0.16f, 0.9f - crouch * 0.3f, 0.18f, pants);
        DrawCube({0, 1.25f - cy, 0}, 0.5f, 0.62f, 0.28f, jacket);
        DrawCube({-0.33f, 1.22f - cy, -sw * 0.2f}, 0.12f, 0.58f, 0.14f, jacket);
        DrawCube({0.33f, 1.22f - cy, sw * 0.2f}, 0.12f, 0.58f, 0.14f, jacket);
        DrawSphereEx({0, 1.72f - cy * 1.1f, 0}, 0.17f, 8, 8, C(204, 168, 138));
        DrawSphereEx({0, 1.78f - cy * 1.1f, -0.04f}, 0.165f, 8, 8, hair);
        rlPopMatrix();
    }
    void DrawHeap(const Enemy& e) const {
        float s = 1.0f - e.dissolve / 1.3f; if (s <= 0.01f) return;
        rlPushMatrix(); rlTranslatef(e.p.x, 0, e.p.y); rlRotatef(e.yaw * 57.29578f, 0, 1, 0); rlScalef(s, s, s);
        DrawSphereEx({0, 0.95f + 0.05f * std::sin(time * 3), 0}, 0.65f, 8, 8, C(40, 42, 46));
        for (int k = 0; k < 11; k++) {
            float a = k * 0.58f + std::sin(time * 2.0f + k) * 0.15f;
            Color col = k % 3 == 0 ? C(70, 110, 150) : (k % 3 == 1 ? C(190, 130, 60) : C(60, 64, 70));
            DrawCylinderEx({0, 0.9f, 0}, {std::cos(a) * 1.1f, 0.15f + (k % 4) * 0.35f, std::sin(a) * 1.1f}, 0.07f, 0.04f, 5, col);
            if (k % 3 == 0) DrawCube({std::cos(a) * 0.9f, 0.5f + (k % 4) * 0.25f, std::sin(a) * 0.9f}, 0.14f, 0.1f, 0.18f, C(200, 160, 60));
        }
        rlPopMatrix();
    }
    void DrawPyramid(const Enemy& e) const {
        rlPushMatrix(); rlTranslatef(e.p.x, 0, e.p.y); rlRotatef(e.yaw * 57.29578f, 0, 1, 0);
        float sw = std::sin(time * 1.6f) * 0.25f;
        DrawCube({-0.2f, 0.55f, sw * 0.3f}, 0.22f, 1.1f, 0.24f, C(70, 66, 64)); DrawCube({0.2f, 0.55f, -sw * 0.3f}, 0.22f, 1.1f, 0.24f, C(70, 66, 64));
        DrawCube({0, 1.55f, 0}, 0.8f, 1.0f, 0.42f, C(100, 58, 50));
        DrawCube({-0.5f, 1.5f, 0}, 0.16f, 0.9f, 0.18f, C(86, 52, 46)); DrawCube({0.5f, 1.5f, 0}, 0.16f, 0.9f, 0.18f, C(86, 52, 46));
        rlPushMatrix(); rlRotatef(45, 0, 1, 0);
        DrawCylinderEx({0, 2.05f, 0}, {0, 2.9f, 0}, 0.5f, 0.0f, 4, C(118, 70, 54));
        rlPopMatrix();
        DrawCylinderEx({0.62f, 0.15f, 0.1f}, {0.62f, 1.7f, 1.1f}, 0.05f, 0.05f, 6, C(90, 90, 94));
        rlPopMatrix();
    }
    void DrawWorld() const {
        for (const Blk& b : area.boxes) if (b.col.a != 0) DrawCube(b.c, b.s.x, b.s.y, b.s.z, b.col);
        for (const Tree& t : area.trees) {
            DrawCylinder({t.x, 0, t.z}, 0.14f, 0.2f, t.h * 0.5f, 6, C(58, 44, 34));
            DrawCylinder({t.x, t.h * 0.3f, t.z}, 0.0f, 1.5f, t.h * 0.8f, 7, C(40, 62, 44));
        }
        for (const Pickup& pk : area.picks) {
            if (pk.id != 4 && flags.count(100 + areaIdx * 10 + pk.id)) continue;
            if (pk.id == 1) DrawCube({pk.p.x, 0.12f, pk.p.y}, 1.0f, 0.06f, 0.18f, C(120, 90, 60));
            if (pk.id == 2) DrawCylinder({pk.p.x, 0, pk.p.y}, 0.06f, 0.06f, 0.2f, 8, C(70, 170, 90));
            if (pk.id == 3) { DrawCube({pk.p.x, 0.85f, pk.p.y}, 0.28f, 0.18f, 0.1f, C(70, 72, 70)); DrawCylinderEx({pk.p.x + 0.1f, 0.94f, pk.p.y}, {pk.p.x + 0.16f, 1.3f, pk.p.y}, 0.012f, 0.008f, 4, C(150, 150, 150)); }
            if (pk.id == 4) { float pu = 0.85f + 0.15f * std::sin(time * 3); DrawCube({pk.p.x, 0.02f, pk.p.y}, 0.9f * pu, 0.02f, 0.9f * pu, C(190, 20, 20, 250)); }
            if (pk.id == 5) { DrawCube({pk.p.x, 0.15f, pk.p.y}, 0.46f, 0.05f, 0.31f, C(198, 176, 96)); }
            if (pk.id == 6) { DrawCube({pk.p.x, 0.15f, pk.p.y}, 0.8f, 0.26f, 0.44f, C(60, 66, 69)); }
            if (pk.id == 7) { DrawCube({pk.p.x, 0.1f, pk.p.y}, 0.65f, 0.02f, 0.48f, C(183, 190, 171)); }
        }
        if (areaIdx == 2) Figure({2.4f, -7.0f}, 1.5708f, 0, 0, C(150, 70, 90), C(50, 52, 60), C(60, 40, 30), 0.7f);
        if (areaIdx == 4 && !flags.count(F_APARTMENT_ANGELA)) Figure({3.5f, -13}, 1.5708f, 0, 0, C(160, 69, 76), C(51, 45, 45), C(62, 50, 39), 0.55f);
        if (areaIdx == 5) Figure({-2.2f, -6}, 0, 0, 0, C(172, 67, 75), C(50, 43, 45), C(82, 51, 34));
        if (areaIdx == 3 && shadowT >= 0 && shadowT < 9.0f) {
            float k = Clamp01(shadowT / 9.0f);
            Figure({-4.0f + k * 8.0f, -36.0f}, 1.5708f, shadowT * 4, 1, C(8, 8, 10), C(8, 8, 10), C(8, 8, 10));
        }
        for (const Enemy& e : enemies) if (e.alive && (e.active || e.kind == 1)) { if (e.kind == 0) DrawHeap(e); else DrawPyramid(e); }
        if (st != S_TITLE) Figure(J.p, J.yaw, J.phase, J.moving, C(64, 88, 58), C(70, 70, 78), C(112, 88, 60), hp < 30 ? 0.12f : 0);
    }

    // raylib 6.0's Camera3D has no near/far fields - BeginMode3D uses hardcoded values.
    Camera3D MakeCam() const {
        Camera3D c{}; c.up = {0, 1, 0}; c.projection = CAMERA_PERSPECTIVE;
        if (st == S_CUT) { c.position = cutPos; c.target = cutTgt; c.fovy = cutFov; }
        else if (HasCorridorCamera()) {
            const CameraPoint p = indoorCam.Position();
            c.position = {p.x, p.y, p.z};
            c.target = camTarget;
            c.fovy = 62.0f;
        }
        else { const Cam& k = area.cams[std::min(curCam, (int)area.cams.size() - 1)]; c.position = k.pos; c.target = camTarget; c.fovy = k.fov; }
        return c;
    }

    void Render() {
        if (st == S_TITLE) {
            ClearBackground(C(4, 5, 5));
            BeginMode2D(CanvasCam());
            DrawTitle();
            EndMode2D();
            return;
        }
        if (!rend.Ready()) return;

        ClearBackground(area.fog);

        Vector2 f = Fwd();
        Vector3 lp{J.p.x + f.x * 0.2f, 1.3f, J.p.y + f.y * 0.2f}, ld{f.x, -0.12f, f.y};
        float ln = std::sqrt(ld.x * ld.x + ld.y * ld.y + ld.z * ld.z); ld = {ld.x / ln, ld.y / ln, ld.z / ln};
        float amb = area.ambient * (areaIdx == 0 ? Flicker(time) : 1.0f);
        rend.Begin(MakeCam(), area.fog, area.density, amb, lp, ld, flash ? (area.dark ? 1.0f : 0.5f) : 0.0f);
        DrawWorld();
        rend.End();

        BeginMode2D(CanvasCam());
        bool weak = hp < 35;
        rend.Present(time, area.vignette + (weak ? 0.25f : 0.0f), hurt, area.desat + (weak ? 0.2f : 0.0f));
        DrawOverlay();
        EndMode2D();
    }

    void DrawOverlay() {
        const float W = kCanvasW, Hh = kCanvasH;
        if (st == S_CUT) {
            DrawRectangle(-3000, -3000, 7100, 3064, C(0, 0, 0)); DrawRectangle(-3000, (int)Hh - 64, 7100, 3064, C(0, 0, 0));
            if (cutIdx < (int)cut.size()) {
                const Line& l = cut[cutIdx];
                if (!l.who.empty()) TxtC(l.who, W / 2, Hh - 190, 14, Al(C(160, 164, 156), 0.9f));
                CenterWrap(l.text, W / 2, Hh - 168, 820, 24, C(236, 236, 228), (int)(cutT * kCps));
            }
        }
        if (st == S_PLAY || st == S_FIGHT || st == S_INV) {
            if (st == S_PLAY) {
                const ChapterInfo& chapter = ChapterAt(areaIdx);
                Txt(chapter.title, 24, 20, 17, C(209, 210, 199));
                if (objectiveHud || objectiveT > 0) {
                    Txt(Fit(chapter.goal, 790, 15), 24, 49, 15, C(170, 181, 171));
                }
                // A contextual prompt makes interaction discoverable without a permanent HUD.
                for (const Pickup& pk : area.picks) {
                    if (pk.id != 4 && flags.count(100 + areaIdx * 10 + pk.id)) continue;
                    if (Dist(J.p, pk.p) > 2.2f) continue;
                    const char* t = pk.id == 4 ? "SAVE GAME" : (pk.id == 5 ? "MAINTENANCE BADGE" :
                        (pk.id == 6 ? "BACKUP TAPE" : (pk.id == 7 ? "ADMIN LOG" : "PICK UP")));
                    TxtC(std::string("Space / Enter: ") + t, W / 2, Hh - 85, 18, C(225, 220, 198));
                    break;
                }
            }
            if (subT > 0) {
                float a = Clamp01(subT / 0.5f);
                TxtC(subWho, W / 2, Hh - 150, 14, Al(C(160, 164, 156), a));
                CenterWrap(subText, W / 2, Hh - 130, 820, 22, Al(C(236, 236, 228), a), 9999);
            }
            if (noticeT > 0) TxtR(notice, W - 40, Hh - 60, 18, Al(C(210, 214, 204), Clamp01(noticeT / 0.6f)));
            if (hintT > 0 && st == S_PLAY) TxtC("Up / Down: walk    Left / Right: turn    Shift: run    Space: action    Tab: items    F: light    Esc: pause", W / 2, Hh - 36, 14, Al(C(190, 194, 186), Clamp01(hintT / 1.0f) * 0.8f));
        }
        if (st == S_FIGHT) DrawFight();
        if (st == S_INV) DrawInventory();
        if (st == S_CHOOSE) DrawEndingChoice();
        if (st == S_GATE) DrawGate();
        if (st == S_PAUSE) {
            DrawRectangle(-3000, -3000, 7100, 6700, Al(C(0, 0, 0), 0.7f));
            TxtC("PAUSED", W / 2, 290, 40, C(220, 222, 214));
            TxtC("Esc: resume    Q: return to C++ Quest    H: show / hide objective", W / 2, 350, 18, C(150, 154, 148));
        }
        if (st == S_DEAD) {
            DrawRectangle(-3000, -3000, 7100, 6700, Al(C(0, 0, 0), 0.92f));
            TxtC("GAME OVER", W / 2, 270, 52, C(150, 30, 30));
            TxtC("Would you like to continue? (Y/N)", W / 2, 350, 22, C(200, 202, 196));
        }
        if (st == S_END) {
            DrawRectangle(-3000, -3000, 7100, 6700, C(0, 0, 0));
            TxtC("SILENT LOCALHOST", W / 2, 215, 46, C(210, 212, 204));
            TxtC(kEndingNames[static_cast<int>(ending)], W / 2, 305, 24, C(180, 185, 173));
            TxtC("THE END", W / 2, 366, 38, C(210, 212, 204));
            TxtC("Press Enter to return to C++ Quest", W / 2, 440, 18, C(130, 134, 128));
        }
        if (fadeA > 0.001f) DrawRectangle(-3000, -3000, 7100, 6700, Al(C(0, 0, 0), fadeA));
    }

    void DrawGate() {
        DrawRectangle(-3000, -3000, 7100, 6700, Al(C(4, 7, 8), 0.95f));
        const TerminalQuestion& q = terminal.Question();
        TxtC(q.name, kCanvasW / 2, 100, 34, C(177, 218, 197));
        Wrap(q.prompt, 180, 176, 760, 22, C(220, 228, 215));
        for (int i = 0; i < 4; ++i) {
            const float y = 266.0f + i * 57;
            if (i == terminal.Selected()) Box({182, y - 8, 736, 49}, 5, C(43, 68, 63));
            Txt(std::to_string(i+1) + ". " + q.answers[i], 213, y, 23,
                i == terminal.Selected() ? C(240, 245, 222) : C(166, 184, 174));
        }
        if (terminal.Attempts() > 0)
            Wrap(q.why, 195, 537, 710, 18, C(221, 187, 146));
        TxtC("Up / Down + Enter or 1-4 then Enter    Esc: back", kCanvasW / 2, 646, 15, C(142, 156, 151));
    }

    void DrawEndingChoice() {
        DrawRectangle(-3000, -3000, 7100, 6700, Al(C(2, 4, 4), 0.94f));
        TxtC("THE LAST COMMIT", kCanvasW / 2, 90, 38, C(219, 219, 211));
        TxtC("You cannot recover the past. Which branch do you follow?", kCanvasW / 2, 154, 19, C(177, 180, 172));
        for (int i = 0; i < (int)kEndingNames.size(); ++i) {
            bool unlocked = EndingUnlocked(i);
            const float y = 236.0f + i * 62.0f;
            Color col = !unlocked ? C(91, 93, 90) : endingSel == i ? C(222, 227, 210) : C(145, 155, 146);
            if (endingSel == i) Box({200, y - 11, 700, 48}, 6, C(45, 50, 48));
            TxtC(std::string(kEndingNames[i]) + (unlocked ? "" : " (locked)"), kCanvasW / 2, y, 22, col);
        }
        TxtC("Up / Down: choose    Enter: commit    Esc: reconsider", kCanvasW / 2, 610, 16, C(128, 136, 128));
    }

    void DrawFight() {
        const float W = kCanvasW;
        const Enemy& e = enemies[fightE];
        DrawRectangle(-3000, -3000, 7100, 6700, Al(C(0, 0, 0), 0.6f));
        Rectangle r{110, 90, 880, 520};
        Box(r, 6, C(10, 12, 12, 240)); Box({r.x + 2, r.y + 2, r.width - 4, r.height - 4}, 5, C(16, 18, 18, 250));
        TxtC(e.kind == 0 ? "CABLE HEAP" : (e.kind == 1 ? "THE RED PYRAMID" : "THE FINAL MERGE"), W / 2, r.y + 22, 30, e.kind == 0 ? C(190, 170, 130) : C(170, 80, 64));
        TxtC(e.kind == 0 ? "It blocks the way. Answer correctly to refactor it." : (e.kind == 1 ? "It cannot be refactored. Answer correctly to make it withdraw." : "Resolve the final conflict to reach the truth."), W / 2, r.y + 64, 15, C(130, 134, 128));
        float y = r.y + 100;
        y += Wrap(q->prompt, r.x + 40, y, r.width - 80, 24, C(226, 228, 220), true, false, nullptr, 1.3f) + 22;
        for (size_t i = 0; i < order.size(); i++) {
            const std::string& txt = q->options[order[i]];
            float h = std::max(46.0f, Wrap(txt, 0, 0, r.width - 150, 20, C(0, 0, 0), false) + 22);
            Rectangle o{r.x + 40, y, r.width - 80, h};
            bool is = (int)i == sel, right = answered && order[i] == q->correct, wrong = answered && (int)i == sel && !correct;
            Color fill = right ? C(30, 60, 36) : (wrong ? C(70, 24, 24) : (is ? C(40, 44, 42) : C(22, 24, 24)));
            Box(o, 4, fill);
            if (is && !answered) Box({o.x, o.y, 4, o.height}, 2, C(200, 204, 196));
            Txt(std::to_string(i + 1), o.x + 18, o.y + (h - 22) / 2, 20, C(130, 134, 128));
            Wrap(txt, o.x + 56, o.y + 11, o.width - 76, 20, C(220, 222, 214));
            y += h + 10;
        }
        if (answered) {
            std::string head = correct ? (e.kind == 0 ? "Process terminated." : (e.kind == 1 ? "It turns away into the fog." : "Merge conflict resolved.")) : "Segmentation fault!";
            Txt(head, r.x + 40, y + 8, 22, correct ? C(150, 200, 150) : C(210, 90, 80));
            if (!correct) Txt("-" + std::to_string(e.kind == 0 ? 16 : 34) + " health", r.x + r.width - 160, y + 10, 18, C(210, 90, 80));
            Wrap(q->explain, r.x + 40, y + 42, r.width - 80, 17, C(170, 174, 166));
            if (resT > 0.6f) TxtC("Enter: continue", W / 2, r.y + r.height - 34, 14, C(120, 124, 118));
        } else TxtC("Up / Down + Enter, or press 1 - 4", W / 2, r.y + r.height - 34, 14, C(120, 124, 118));
    }

    void DrawInventory() {
        const float W = kCanvasW, Hh = kCanvasH;
        DrawRectangle(-3000, -3000, 7100, 6700, Al(C(6, 7, 7), 0.94f));
        for (int y = 0; y < (int)Hh; y += 4) DrawRectangle(0, y, (int)W, 1, Al(C(255, 255, 255), 0.025f));
        TxtC("ITEMS", 230, 56, 34, C(210, 212, 204));
        const char* word = hp > 66 ? "Fine" : (hp > 33 ? "Caution" : "Danger");
        Color sc = hp > 66 ? C(120, 190, 120) : (hp > 33 ? C(210, 190, 90) : C(200, 70, 60));
        Txt("James Sunderland", 60, 112, 20, C(180, 184, 176)); Txt(word, 60, 142, 28, sc);
        float beat = hp > 0 ? 0.6f + (1.0f - Clamp01(hp / 100.0f)) * 1.6f : 0;
        for (int i = 0; i < 200; i++) {
            float x = 60 + i * 1.2f, ph = std::fmod(i * 0.03f - time * beat, 1.0f);
            float yy = 205 - (ph > 0.6f && ph < 0.66f ? 30 : (ph > 0.66f && ph < 0.72f ? -12 : 0));
            DrawRectangle((int)x, (int)yy, 2, 2, Al(sc, 0.8f));
        }
        std::vector<int> l = InvList();
        for (size_t i = 0; i < l.size(); i++) {
            float y = 250 + i * 46; bool is = (int)i == invSel;
            if (is) { DrawRectangle(50, (int)y - 6, 400, 40, Al(C(255, 255, 255), 0.09f)); DrawRectangle(50, (int)y - 6, 4, 40, C(200, 204, 196)); }
            std::string nm = names[l[i]]; if (inv[l[i]] > 1) nm += "  x" + std::to_string(inv[l[i]]);
            Txt(nm, 70, y, 24, is ? C(236, 236, 228) : C(140, 144, 138));
        }
        if (!l.empty()) {
            int it = l[std::min(invSel, (int)l.size() - 1)];
            float sq = std::fabs(std::cos(time * 0.9f));
            Color col = it == I_DRINK ? C(80, 170, 90) : (it == I_PLANK ? C(130, 98, 66) : (it == I_RADIO ? C(100, 104, 100) : (it == I_DUCK ? C(220, 190, 60) : C(150, 160, 170))));
            Rectangle card{760 - 110 * std::max(0.08f, sq), 150, 220 * std::max(0.08f, sq), 240};
            Box(card, 10, col); Box({card.x + 6, card.y + 6, card.width - 12, card.height - 12}, 8, Mix(col, C(0, 0, 0), 0.35f));
            Wrap(descs[it], 590, 430, 340, 19, C(200, 202, 194));
            if (it == I_DRINK) Txt(hp < 100 ? "Enter: drink  (+40 health)" : "Health is full.", 590, 560, 16, C(140, 144, 138));
        }
        TxtC("Up / Down: select        Tab or Esc: close", W / 2, Hh - 40, 14, C(110, 114, 108));
    }

    void DrawTitle() {
        const float W = kCanvasW;
        DrawRectangle(-3000, -3000, 7100, 6700, C(4, 5, 5));
        for (int k = 0; k < 5; k++) {
            float x = std::fmod(time * (12 + k * 5) + k * 300, W + 500) - 250;
            DrawCircleV({x, 300 + k * 50.0f}, 260, Al(C(150, 160, 152), 0.055f));
        }
        TxtC(kTitle, W / 2, 190, 64, C(205, 208, 196));
        TxtC("a C++ Quest easter egg", W / 2, 268, 16, C(120, 126, 118));
        const char* opts3[3] = {"New Game", "Continue", "Quit"}; const char* opts2[2] = {"New Game", "Quit"};
        int n = hasSave ? 3 : 2;
        for (int i = 0; i < n; i++) {
            const char* t = hasSave ? opts3[i] : opts2[i];
            TxtC(t, W / 2, 340 + i * 44.0f, 26, i == menuSel ? C(230, 232, 224) : C(110, 114, 108));
        }
        TxtC("Up / Down + Enter", W / 2, 520, 14, C(90, 94, 88));
        for (int k = 0; k < 300; k++) DrawRectangle(GetRandomValue(0, (int)W), GetRandomValue(0, (int)kCanvasH), 2, 2, Al(C(255, 255, 255), 0.04f));
    }
};

}  // namespace sh2