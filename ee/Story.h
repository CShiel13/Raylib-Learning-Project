#pragma once
// Story beats adapted and condensed from the project's user-supplied parody script.
// This is playable scene dialogue, not a verbatim implementation of every page.
#include <string>
#include <vector>

namespace sh2 {
struct Line { std::string who, text; };

inline std::vector<Line> ApartmentEntry() {
    return {{"JAMES", "This apartment building looks like it has not passed CI in years."},
            {"EDDIE", "I didn't push to main! The production database was like that when I got here!"},
            {"JAMES", "I'm James. Full-stack developer. Have you seen Mary?"},
            {"EDDIE", "No. I saw some legacy code moving through the hallway. I ran."},
            {"JAMES", "The Wi-Fi is unsecured. You should commit and get out of here."},
            {"EDDIE", "A recruiter brought me here. A very persistent recruiter."}};
}
inline std::vector<Line> ApartmentAngela() {
    return {{"ANGELA", "Oh. It's you. The guy who doesn't use TypeScript."},
            {"JAMES", "I brought a rubber duck. Does that count as support?"},
            {"ANGELA", "My mother wrote the original documentation. I can't find it anywhere."},
            {"JAMES", "I found a commit signed by Mary. Her account has been gone three years."},
            {"ANGELA", "Maybe the repository remembers more than we do."},
            {"ANGELA", "I hope you find what you're looking for. I need to keep searching."}};
}
inline std::vector<Line> ParkEntry() {
    return {{"MARIA", "You look like you've been debugging for three days."},
            {"JAMES", "Mary? No... You're using semicolons."},
            {"MARIA", "I'm Maria. I use Arch, by the way."},
            {"JAMES", "I got a pull request from my wife's account. She said she'd wait in our special repository."},
            {"MARIA", "Wasn't there a hotel? Lakeview? You two used to pair program there."},
            {"JAMES", "That was another place. I need to get there."},
            {"MARIA", "I'll come. I've seen the dependency vulnerabilities out here."}};
}
inline std::vector<Line> DataCenterEntry() {
    return {{"LAURA", "What are you doing here? This is a restricted server!"},
            {"JAMES", "Laura! How do you know Mary's name?"},
            {"LAURA", "I knew her. She left me a letter, on actual paper."},
            {"JAMES", "Documentation! Can I see it?"},
            {"LAURA", "It's in the maintenance room. Go on in..."},
            {"", "A heavy door closes. Somewhere beyond it, cables begin to move."},
            {"LAURA", "I changed the root password! Maybe learn to read the README!"}};
}
inline std::vector<Line> MazeEntry() {
    return {{"", "The corridor branches like a dependency graph nobody understands."},
            {"EDDIE", "I committed straight to main. On a Friday. They never forgave me."},
            {"JAMES", "You can't keep blaming everyone else for your changes."},
            {"EDDIE", "I didn't write the old code. I only copied it."},
            {"JAMES", "Somebody has to take ownership."},
            {"", "A tape drive hums behind a locked gate. The original backup is close."}};
}
inline std::vector<Line> HotelEntry() {
    return {{"", "Lakeview Hotel. Dust coats the terminal where two people once worked side by side."},
            {"JAMES", "Our special place. It still feels like she could be here."},
            {"ANGELA", "For me it's always like this: a deployment that never finishes."},
            {"JAMES", "I can't fix every build. But I can stop pretending the logs aren't there."},
            {"", "A damaged recording points to the last commit... and a locked room upstairs."}};
}
inline std::vector<Line> HotelTruth() {
    return {{"MARY", "James... you promised you would deploy me here again."},
            {"JAMES", "I kept rebuilding the same mistake. I thought if I found your code I could undo it."},
            {"MARY", "A restore isn't the same as a person. You know that."},
            {"JAMES", "I need to see the final commit for myself."}};
}
inline std::vector<Line> FinalEntry() {
    return {{"MARIA", "Welcome back, James. I've been running here all along."},
            {"JAMES", "You're not Mary. You're a copy of what I wanted to remember."},
            {"MARIA", "Then why did you keep this branch alive?"},
            {"", "The last merge conflict takes shape in the fog."}};
}
inline std::vector<Line> EndingDialogue(Ending ending) {
    switch (ending) {
        case Ending::Leave:
            return {{"JAMES", "I can't change that last commit. But I can stop hiding from it."},
                    {"MARY", "Go on, James. And try to document what you learned."},
                    {"", "The fog thins. The road out of localhost is still open."}};
        case Ending::Maria:
            return {{"MARIA", "We could start a fresh repository. No history, no blame."},
                    {"JAMES", "Maybe... but the warnings are still there."},
                    {"", "A new process starts. The same error quietly returns in its logs."}};
        case Ending::Water:
            return {{"JAMES", "I have to let this version go. Even the broken parts."},
                    {"", "One by one, the old branches become archived read-only memories."},
                    {"MARY", "Our special repository will always be part of you."}};
        case Ending::Rebirth:
            return {{"JAMES", "There is a backup. One that predates everything."},
                    {"", "The restoration begins. There is no guarantee what will wake up."},
                    {"JAMES", "Maybe history cannot be rewritten... but the build is running."}};
        case Ending::Dog:
            return {{"JAMES", "So it was you! You wrote the original codebase?"},
                    {"", "A small dog in reading glasses presses Enter with one paw."},
                    {"", "Commit: Fixed everything. Woof."},
                    {"JAMES", "Coffee? You've earned it."}};
    }
    return {};
}
}  // namespace sh2
