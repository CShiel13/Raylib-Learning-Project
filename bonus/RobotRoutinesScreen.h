#pragma once
// Interactive instruction-sequencing puzzle. Uses the existing C++ Quest visual toolkit.
#include <string>
#include "RobotRoutines.h"
#include "ui/Widgets.h"

class RobotRoutinesScreen {
public:
    void Reset() { session_ = robot_routines::Session{}; timer_ = 0.0f; }
    void Draw(float dt) {
        using namespace robot_routines;
        if (session_.State() == Phase::Running) {
            timer_ += dt;
            if (timer_ >= 0.24f) { timer_ = 0.0f; session_.Tick(); }
        } else timer_ = 0.0f;
        if (session_.State() == Phase::Solved && IsKeyPressed(KEY_ENTER)) session_.NewMaze();
        if (session_.State() != Phase::Running && session_.State() != Phase::Solved) {
            if (IsKeyPressed(KEY_ONE)) session_.Add(Command::Forward);
            if (IsKeyPressed(KEY_TWO)) session_.Add(Command::TurnLeft);
            if (IsKeyPressed(KEY_THREE)) session_.Add(Command::TurnRight);
            if (IsKeyPressed(KEY_BACKSPACE)) session_.Undo();
            if (IsKeyPressed(KEY_ENTER)) session_.Run();
        }
        Txt("ROBOT ROUTINES", 35, 89, 30, kText);
        Txt("Build a tiny program to deliver a packet across the maze.", 35, 132, 16, kMuted);
        Card3D({30, 165, 515, 419}, 22, kSurface, kBorder, 5);
        Card3D({566, 165, 504, 419}, 22, kSurface, kBorder, 5);
        DrawMaze();
        DrawProgram();
        DrawControls();
    }

private:
    robot_routines::Session session_;
    float timer_ = 0;

    void DrawMaze() {
        using namespace robot_routines;
        Txt("THE NETWORK", 51, 180, 16, kSky);
        const float gap = 48, x0 = 67, y0 = 216;
        for (int y = 0; y < Session::Height; ++y) for (int x = 0; x < Session::Width; ++x) {
            Rectangle tile{x0 + x * gap, y0 + y * gap, 43, 43};
            bool wall = session_.IsWall(x, y);
            Color fill = wall ? kBorder : ((x + y) % 2 ? kSurface2 : Mix(kSurface2, kBg, 0.18f));
            Box(tile, 7, fill);
            if (wall) {
                Box({tile.x + 9, tile.y + 9, 25, 25}, 5, Mix(kBorder, kBg, 0.33f));
                TxtC("X", tile.x + 21.5f, tile.y + 12, 15, kMuted, true);
            }
            if (Cell{x,y} == session_.Goal()) {
                Box({tile.x + 4, tile.y + 4, 35, 35}, 9, Mix(kMint, kSurface, 0.57f));
                TxtC("GO", tile.x + 21.5f, tile.y + 11, 14, kText, true);
            }
        }
        auto p = session_.Robot();
        const char* facing[4] = {">", "v", "<", "^"};
        Rectangle robot{x0 + p.x * gap + 4, y0 + p.y * gap + 4, 35, 35};
        Box(robot, 10, session_.State() == Phase::Crashed ? kCoral : kSky);
        TxtC(facing[session_.Direction()], robot.x + 17.5f, robot.y + 5, 23, kBg, true);
        const char* state = "Blue arrow starts right. Avoid X tiles; reach GO.";
        switch (session_.State()) {
            case Phase::Running: state = "Executing your program..."; break;
            case Phase::Stopped: state = "Not at the exit yet. Edit and try again."; break;
            case Phase::Crashed: state = "Obstacle hit! Check the route and retry."; break;
            case Phase::Solved: state = "Packet delivered! Next puzzle is ready."; break;
            default: break;
        }
        Txt(state, 51, 555, 14, session_.State() == Phase::Solved ? kMint :
            session_.State() == Phase::Crashed ? kCoral : kMuted);
    }
    void DrawProgram() {
        const auto& commands = session_.Program();
        Txt("YOUR PROGRAM", 586, 184, 17, kMint);
        TxtR(std::to_string(commands.size()) + "/36 commands", 1043, 184, 13, kMuted);
        Txt("Commands run from top to bottom, then continue in column two.", 587, 213, 13, kMuted);
        if (commands.empty()) {
            Txt("// Add commands with buttons below", 595, 257, 18, kMuted, true);
            Txt("// Reach the green GO tile to win", 595, 290, 17, kMuted, true);
        }
        for (size_t i = 0; i < commands.size(); ++i) {
            int col = static_cast<int>(i) / 18;
            int row = static_cast<int>(i) % 18;
            float x = 586.0f + col * 247.0f;
            float y = 245.0f + row * 18.0f;
            bool active = session_.State() == robot_routines::Phase::Running && session_.Step() == static_cast<int>(i);
            if (active) Box({x, y - 1, 236, 20}, 5, Mix(kSky, kSurface, 0.72f));
            Txt(std::to_string(i + 1) + ".", x + 2, y, 12, active ? kAmber : kMuted, true);
            Txt(robot_routines::Label(commands[i]), x + 25, y, 12, active ? kText : kSky, true);
        }
    }
    void DrawControls() {
        using namespace robot_routines;
        bool editable = session_.State() != Phase::Running && session_.State() != Phase::Solved;
        if (Button({30, 605, 170, 53}, "1: Forward", kSky, editable, 18)) session_.Add(Command::Forward);
        if (Button({209, 605, 170, 53}, "2: Turn left", kViolet, editable, 18)) session_.Add(Command::TurnLeft);
        if (Button({388, 605, 170, 53}, "3: Turn right", kViolet, editable, 18)) session_.Add(Command::TurnRight);
        if (session_.State() == Phase::Solved) {
            if (Button({567, 605, 244, 53}, "Next maze [Enter]", kMint, true, 18)) session_.NewMaze();
            TxtR("Solved: " + std::to_string(session_.SolvedCount()), 1050, 623, 18, kMint);
        } else {
            if (Button({567, 605, 157, 53}, "Run [Enter]", kMint, editable && !session_.Program().empty(), 17)) session_.Run();
            if (Button({733, 605, 151, 53}, "Undo", kAmber, editable && !session_.Program().empty(), 17)) session_.Undo();
            if (Button({893, 605, 176, 53}, "Clear program", kCoral, editable, 17, true)) session_.ClearProgram();
        }
    }
};
