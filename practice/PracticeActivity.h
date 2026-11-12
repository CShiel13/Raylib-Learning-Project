#pragma once

// Shared screen contract for activities in the Practice Arcade.
enum class PracticeAction { None, Back };

class IPracticeActivity {
public:
    virtual ~IPracticeActivity() = default;
    virtual void Start() = 0;
    virtual PracticeAction Draw(float dt) = 0;
};
