#pragma once
// Robot Routines rules: pure C++17; procedural, solvable mazes and command interpreter.
// No raylib or game-global state. UI lives in RobotRoutinesScreen.h.
#include <algorithm>
#include <array>
#include <cstdint>
#include <queue>
#include <random>
#include <vector>

namespace robot_routines {

struct Cell { int x = 0, y = 0; };
inline bool operator==(Cell a, Cell b) { return a.x == b.x && a.y == b.y; }

enum class Command { Forward, TurnLeft, TurnRight };
enum class Phase { Editing, Running, Stopped, Crashed, Solved };

class Session {
public:
    static constexpr int Width = 7;
    static constexpr int Height = 7;
    static constexpr int MaxCommands = 36;
    explicit Session(uint32_t seed = std::random_device{}()) : rng_(seed) { NewMaze(); }

    int Level() const { return level_; }
    int SolvedCount() const { return solved_; }
    Phase State() const { return phase_; }
    Cell Robot() const { return robot_; }
    Cell Goal() const { return {Width - 1, Height - 1}; }
    int Direction() const { return direction_; } // 0=right, 1=down, 2=left, 3=up
    int Step() const { return step_; }
    int ShortestPath() const { return shortest_; }
    const std::vector<Command>& Program() const { return commands_; }
    bool IsWall(int x, int y) const {
        return x < 0 || x >= Width || y < 0 || y >= Height || walls_[static_cast<size_t>(y * Width + x)];
    }

    bool Add(Command c) {
        if (phase_ == Phase::Running || phase_ == Phase::Solved || commands_.size() >= MaxCommands) return false;
        commands_.push_back(c); phase_ = Phase::Editing; return true;
    }
    void Undo() {
        if (phase_ == Phase::Running || phase_ == Phase::Solved || commands_.empty()) return;
        commands_.pop_back(); phase_ = Phase::Editing;
    }
    void ClearProgram() {
        if (phase_ == Phase::Running || phase_ == Phase::Solved) return;
        commands_.clear(); ResetRobot();
    }
    bool Run() {
        if (phase_ == Phase::Running || phase_ == Phase::Solved || commands_.empty()) return false;
        ResetRobot(); phase_ = Phase::Running; return true;
    }
    // One instruction per step. UI controls the rate; logic remains deterministic.
    void Tick() {
        if (phase_ != Phase::Running) return;
        if (step_ >= static_cast<int>(commands_.size())) { phase_ = Phase::Stopped; return; }
        Command c = commands_[static_cast<size_t>(step_++)];
        if (c == Command::TurnLeft) direction_ = (direction_ + 3) % 4;
        else if (c == Command::TurnRight) direction_ = (direction_ + 1) % 4;
        else {
            static constexpr int dx[4] = {1, 0, -1, 0};
            static constexpr int dy[4] = {0, 1, 0, -1};
            Cell next{robot_.x + dx[direction_], robot_.y + dy[direction_]};
            if (IsWall(next.x, next.y)) { phase_ = Phase::Crashed; return; }
            robot_ = next;
        }
        if (robot_ == Goal()) { phase_ = Phase::Solved; ++solved_; return; }
        if (step_ >= static_cast<int>(commands_.size())) phase_ = Phase::Stopped;
    }
    void NewMaze() {
        if (phase_ == Phase::Solved) ++level_;
        commands_.clear();
        // Random barriers; regenerate until a path exists. Harder levels have more barriers.
        std::bernoulli_distribution block(std::min(0.29, 0.12 + 0.018 * level_));
        for (int attempt = 0; attempt < 200; ++attempt) {
            walls_.fill(false);
            for (int y = 0; y < Height; ++y) for (int x = 0; x < Width; ++x)
                if (!((x == 0 && y == 0) || (x == Width - 1 && y == Height - 1)))
                    walls_[static_cast<size_t>(y * Width + x)] = block(rng_);
            shortest_ = PathLength();
            if (shortest_ >= 12 && shortest_ <= 22) { ResetRobot(); return; }
        }
        // Bounded fallback always has a solution, even if the random attempts fail.
        walls_.fill(false); shortest_ = Width + Height - 2; ResetRobot();
    }

private:
    std::mt19937 rng_;
    std::array<bool, Width * Height> walls_{};
    std::vector<Command> commands_;
    Cell robot_{};
    int direction_ = 0, step_ = 0, shortest_ = 12, level_ = 1, solved_ = 0;
    Phase phase_ = Phase::Editing;

    void ResetRobot() { robot_ = {0, 0}; direction_ = 0; step_ = 0; phase_ = Phase::Editing; }
    int PathLength() const {
        std::array<int, Width * Height> distance;
        distance.fill(-1);
        std::queue<Cell> pending;
        pending.push({0, 0}); distance[0] = 0;
        static constexpr int dx[4] = {1, 0, -1, 0};
        static constexpr int dy[4] = {0, 1, 0, -1};
        while (!pending.empty()) {
            Cell c = pending.front(); pending.pop();
            int d = distance[static_cast<size_t>(c.y * Width + c.x)];
            if (c == Goal()) return d;
            for (int k = 0; k < 4; ++k) {
                int x = c.x + dx[k], y = c.y + dy[k];
                if (IsWall(x, y)) continue;
                size_t id = static_cast<size_t>(y * Width + x);
                if (distance[id] != -1) continue;
                distance[id] = d + 1; pending.push({x, y});
            }
        }
        return -1;
    }
};

inline const char* Label(Command c) {
    switch (c) {
    case Command::Forward: return "moveForward();";
    case Command::TurnLeft: return "turnLeft();";
    case Command::TurnRight: return "turnRight();";
    }
    return "";
}

} // namespace robot_routines
