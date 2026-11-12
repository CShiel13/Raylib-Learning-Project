// C++ Quest - learn programming with bite-sized challenges. C++17 + raylib 5.x/6.x
#include <algorithm>
#include <ctime>
#include "raylib.h"
#include "ui/Viewport.h"
#include "rlgl.h"
#include "app/Game.h"

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1100, 700, "C++ Quest");

    // Open at a comfortable size for this monitor (the UI scales to any window size).
    int mon = GetCurrentMonitor();
    float s = std::min(GetMonitorWidth(mon) * 0.92f / kCanvasW, GetMonitorHeight(mon) * 0.86f / kCanvasH);
    s = std::max(1.0f, std::min(2.4f, s));
    int w = (int)(kCanvasW * s), h = (int)(kCanvasH * s);
    SetWindowSize(w, h);
    SetWindowPosition(std::max(0, (GetMonitorWidth(mon) - w) / 2), std::max(0, (GetMonitorHeight(mon) - h) / 2 - 20));
    SetWindowMinSize(880, 560);

    SetExitKey(KEY_NULL);          // ESC goes back instead of quitting
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)time(nullptr));
    LoadFonts();

    {
        Game game;
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(kBg);
            UpdateViewScale();
            rlPushMatrix();
            rlTranslatef(gOffX, gOffY, 0);
            rlScalef(gScale, gScale, 1);
            game.Frame();
            rlPopMatrix();
            EndDrawing();
        }
        game.SaveNow();
    }
    CloseWindow();
    return 0;
}
