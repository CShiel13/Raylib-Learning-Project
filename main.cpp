// JavaQuest - learn Java with bite-sized quizzes. C++17 + raylib 5.x
#include <ctime>
#include "raylib.h"
#include "Game.h"

int main() {
    InitWindow(960, 640, "JavaQuest - Learn Java in 5 minutes");
    SetExitKey(KEY_NULL);          // ESC goes back instead of quitting
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)time(nullptr));

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
