#pragma once
// ---------------------------------------------------------------------------
// ee/mode.h - entry point of the OPTIONAL easter egg.
// ---------------------------------------------------------------------------
#include <algorithm>
#include <vector>
#include "game.h"

namespace sh2 {

struct Konami {
    std::vector<int> hist;
    bool Feed() {
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

class Mode {
public:
    explicit Mode(const std::vector<Topic>* cppTopics = nullptr) : game(cppTopics) {}
    bool Frame(float dt) { return game.Frame(dt); }
private:
    Game3D game;
};

}  // namespace sh2