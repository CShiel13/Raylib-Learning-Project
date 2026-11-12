#pragma once
// Optional keyboard easter egg. Enabled only on the Bonus Languages screen.
#include <vector>
#include "KonamiCode.h"
#include "game.h"

namespace sh2 {
class Konami {
public:
    Konami() : code_({KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN,
                      KEY_LEFT, KEY_RIGHT, KEY_LEFT, KEY_RIGHT, KEY_B, KEY_A}) {}
    bool Feed() {
        bool found = false;
        for (int key = GetKeyPressed(); key != 0; key = GetKeyPressed())
            found = code_.Feed(key) || found;
        return found;
    }
    void Reset() { code_.Reset(); }
private:
    KonamiCode code_;
};
class Mode {
public:
    explicit Mode(const std::vector<Topic>* cppTopics = nullptr) : game_(cppTopics) {}
    bool Frame(float dt) { return game_.Frame(dt); }
private:
    Game3D game_;
};
} // namespace sh2
