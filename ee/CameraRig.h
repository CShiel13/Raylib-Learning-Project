#pragma once
// Silent Localhost: collision-safe camera placement for the long indoor halls.
// Pure C++ so this can be tested without opening a window or loading raylib.
#include <algorithm>
#include <cmath>

namespace sh2 {

struct CameraPoint {
    float x = 0.0f, y = 0.0f, z = 0.0f;
};

class IndoorCameraRig {
public:
    // Hall ceiling starts at y=3.25, walls at x=+/-6.85, and solid shelving
    // begins around x=+/-4.35. Keep the lens inside the unobstructed lane.
    static CameraPoint Desired(float playerX, float playerZ, int shot) {
        const float shoulder = (shot % 2 == 0) ? 2.5f : -2.5f;
        return {
            std::clamp(playerX + shoulder, -3.7f, 3.7f),
            2.05f,
            std::clamp(playerZ + 5.25f, -28.8f, 7.5f)
        };
    }

    // Scripted dialogue uses authored camera positions. Bring those inside the
    // hall as well; otherwise a cutscene can start with a view of the roof.
    static CameraPoint SafeCut(CameraPoint shot) {
        return {
            std::clamp(shot.x, -3.7f, 3.7f),
            std::clamp(shot.y, 1.65f, 2.20f),
            std::clamp(shot.z, -28.8f, 7.5f)
        };
    }

    void Reset(float playerX, float playerZ, int shot = 0) {
        position_ = Desired(playerX, playerZ, shot);
    }

    void Track(float playerX, float playerZ, int shot, float dt) {
        const CameraPoint goal = Desired(playerX, playerZ, shot);
        const float step = 1.0f - std::exp(-8.0f * std::clamp(dt, 0.0f, 0.05f));
        position_.x += (goal.x - position_.x) * step;
        position_.y += (goal.y - position_.y) * step;
        position_.z += (goal.z - position_.z) * step;
    }

    CameraPoint Position() const { return position_; }

private:
    CameraPoint position_{};
};

} // namespace sh2
