#pragma once
// Data-only story progression. This header deliberately has no raylib dependency.
#include <array>
#include <cstddef>

namespace sh2 {

enum class Chapter : int {
    Restroom, Overlook, Cemetery, Town, Apartments, Park,
    DataCenter, LegacyMaze, Lakeview, FinalRoom, Count
};

struct ChapterInfo {
    const char* title;
    const char* goal;
    const char* landmark;
    bool indoors;
};

inline constexpr std::array<ChapterInfo, static_cast<std::size_t>(Chapter::Count)> kChapters{{
    {"Restroom", "Leave the restroom and follow the path.", "The open doorway behind you", true},
    {"Overlook", "Follow the trail through the fog.", "The forest path ahead", false},
    {"Cemetery", "Find the road into town.", "The gates beyond the gravestones", false},
    {"Localhost Streets", "Find the abandoned apartment block.", "The far end of the street", false},
    {"Abandoned Apartments", "Locate the access badge and leave the building.", "The door at the end of the hall", true},
    {"Rosewater Park", "Find Mary's old backup notes by the lake.", "The waterfront path", false},
    {"Brookhaven Data Center", "Clear the cable creature and unlock the terminal.", "The locked maintenance exit", true},
    {"Legacy Code Labyrinth", "Collect the restoration key and escape.", "The last hallway", true},
    {"Lakeview Hotel", "Find the original deployment record.", "The far end of the hotel", true},
    {"The Last Commit", "Face the final conflict and choose a branch.", "The glowing terminal", true}
}};

inline const ChapterInfo& ChapterAt(int index) {
    return kChapters[static_cast<std::size_t>(index >= 0 && index < (int)kChapters.size() ? index : 0)];
}

inline bool IsFinalChapter(int index) { return index == (int)Chapter::FinalRoom; }

// No dependencies on gameplay classes. Ending selection is a final, conscious choice.
enum class Ending { Leave, Maria, Water, Rebirth, Dog };
inline constexpr std::array<const char*, 5> kEndingNames{{
    "Leave: accept the truth", "Maria: keep the simulation", "Water: archive everything",
    "Rebirth: restore a backup", "Dog: inspect the admin account"
}};

}  // namespace sh2
