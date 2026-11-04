// JavaQuest - learn programming with bite-sized quizzes. C++17 + raylib 5.x/6.x
#include <ctime>
#include "raylib.h"
#include "Game.h"

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1100, 700, "JavaQuest");
    SetExitKey(KEY_NULL);          // ESC goes back instead of quitting
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)time(nullptr));
    LoadFonts();

    {
        Game game;
        while (!WindowShouldClose()) {
            BeginDrawing();
            game.Frame();
            EndDrawing();
        }
        game.SaveNow();
    }
    CloseWindow();
    return 0;
}