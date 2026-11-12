#pragma once
// AvatarData.h - avatar look + wardrobe catalogue. No raylib here, so Player.h can include it.
//
// Every customisable part is a "field". A field is either a colour palette (skin, dye, outfit colour)
// or a list of items. The player's look is one int per field (the item/colour index).
// Items unlock at a level; items with a price must also be bought with coins.
#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

enum AvField {
    AV_SKIN, AV_HAIR, AV_HAIRC, AV_EYES, AV_MOUTH, AV_FACE, AV_HAT,
    AV_GLASSES, AV_OUTFIT, AV_OUTC, AV_NECK, AV_BACK, AV_PET, AV_BG, AV_FIELDS
};

struct AvItem { const char* name; int level; int price; };   // price 0 = free once the level is reached

inline const char* AvFieldName(int f) {
    static const char* n[AV_FIELDS] = { "Skin", "Hair", "Dye", "Eyes", "Mouth", "Face", "Hat",
                                        "Glasses", "Outfit", "Colour", "Neck", "Back", "Pet", "Scene" };
    return n[f];
}
inline bool AvIsPalette(int f) { return f == AV_SKIN || f == AV_HAIRC || f == AV_OUTC; }
inline int AvPaletteSize(int f) { return f == AV_SKIN ? 6 : 8; }

// Order must match the AvField enum. Palette fields have no items. Keep each list sorted by level.
inline const std::vector<AvItem>& AvItems(int f) {
    static const std::vector<AvItem> t[AV_FIELDS] = {
        /* SKIN    */ {},
        /* HAIR    */ { {"Bald",1,0}, {"Short",1,0}, {"Spiky",1,0}, {"Long",2,0}, {"Bun",3,0}, {"Mohawk",5,40},
                        {"Afro",8,60}, {"Pigtails",11,80}, {"Ponytail",14,100}, {"Flame",25,250} },
        /* HAIRC   */ {},
        /* EYES    */ { {"Dots",1,0}, {"Big",1,0}, {"Happy",1,0}, {"Sleepy",4,0}, {"Star",9,60}, {"Heart",14,100},
                        {"Spiral",18,120}, {"Cyber",26,200} },
        /* MOUTH   */ { {"Smile",1,0}, {"Grin",1,0}, {"Smirk",3,0}, {"Tongue",6,40}, {"Cat",9,60}, {"Fangs",16,100},
                        {"Robot",24,160} },
        /* FACE    */ { {"None",1,0}, {"Blush",2,0}, {"Freckles",4,0}, {"Mustache",8,50}, {"Beard",12,90},
                        {"Eyepatch",15,100}, {"Paint",19,120}, {"Clown nose",22,100} },
        /* HAT     */ { {"None",1,0}, {"Cap",2,0}, {"Beanie",4,30}, {"Cowboy",6,60}, {"Party",8,70}, {"Crown",10,0},
                        {"Top hat",13,120}, {"Wizard",16,150}, {"Pirate",20,200}, {"Halo",24,0}, {"Gold crown",35,500} },
        /* GLASSES */ { {"None",1,0}, {"Round",3,0}, {"Shades",5,40}, {"Hearts",9,70}, {"Monocle",13,90},
                        {"3D",16,100}, {"Goggles",20,150}, {"Visor",28,250} },
        /* OUTFIT  */ { {"Tee",1,0}, {"Hoodie",3,0}, {"Stripes",5,40}, {"Suit",8,100}, {"Armor",13,180},
                        {"Robe",17,220}, {"Hero",21,300}, {"Golden",40,600} },
        /* OUTC    */ {},
        /* NECK    */ { {"None",1,0}, {"Bow tie",4,30}, {"Scarf",5,0}, {"Bandana",7,50}, {"Medal",12,0},
                        {"Chain",19,150}, {"Flower lei",23,120} },
        /* BACK    */ { {"None",1,0}, {"Backpack",5,40}, {"Cape",7,0}, {"Angel wings",25,300},
                        {"Dragon wings",32,400}, {"Jetpack",38,500}, {"Rainbow cape",45,0} },
        /* PET     */ { {"None",1,0}, {"Slime",3,0}, {"Cat",6,50}, {"Dog",10,80}, {"Ghost",15,120},
                        {"Robot",24,250}, {"Baby dragon",33,450} },
        /* BG      */ { {"Classic",1,0}, {"Sunset",3,0}, {"Night",6,40}, {"Meadow",9,60}, {"Ocean",13,100},
                        {"City",17,140}, {"Space",22,200}, {"Aurora",30,350} },
    };
    return t[f];
}
inline const AvItem& AvItemAt(int f, int id) { return AvItems(f)[id]; }
inline int AvCount(int f) { return AvIsPalette(f) ? AvPaletteSize(f) : (int)AvItems(f).size(); }
inline int AvKey(int f, int id) { return f * 100 + id; }     // key stored in Player::owned

struct AvatarLook {
    //           skin hair dye eyes mouth face hat glasses outfit colour neck back pet scene
    int v[AV_FIELDS] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    void Clamp() { for (int i = 0; i < AV_FIELDS; i++) v[i] = std::max(0, std::min(AvCount(i) - 1, v[i])); }
};

// Wearable = palette colour, or level reached (and bought, if it has a price).
inline bool AvUnlocked(int f, int id, int level, const std::set<int>& owned) {
    if (AvIsPalette(f)) return true;
    const AvItem& it = AvItemAt(f, id);
    return level >= it.level && (it.price == 0 || owned.count(AvKey(f, id)) > 0);
}
inline bool AvCanBuy(int f, int id, int level, const std::set<int>& owned) {
    if (AvIsPalette(f)) return false;
    const AvItem& it = AvItemAt(f, id);
    return level >= it.level && it.price > 0 && owned.count(AvKey(f, id)) == 0;
}

// Bonus coins paid on reaching a level (on top of the normal 10). Every 5th level: 20, 40, 60 ... 200.
inline int AvLevelBonus(int level) { return level % 5 == 0 ? 20 * (level / 5) : 0; }

// The next few things the player will unlock, soonest first: {level, "Hat: Wizard"}.
inline std::vector<std::pair<int, std::string>> AvUpcoming(int level, int n) {
    std::vector<std::pair<int, std::string>> out;
    for (int f = 0; f < AV_FIELDS; f++) {
        if (AvIsPalette(f)) continue;
        for (const AvItem& it : AvItems(f))
            if (it.level > level) out.push_back({ it.level, std::string(AvFieldName(f)) + ": " + it.name });
    }
    std::stable_sort(out.begin(), out.end(), [](const std::pair<int, std::string>& a, const std::pair<int, std::string>& b) { return a.first < b.first; });
    if ((int)out.size() > n) out.resize(n);
    return out;
}

// "New: Wizard, Halo" for items whose level falls in (from, to]. Empty if nothing new. Handy for a toast.
inline std::string AvUnlockText(int from, int to) {
    std::vector<std::string> names;
    for (int f = 0; f < AV_FIELDS; f++) {
        if (AvIsPalette(f)) continue;
        for (const AvItem& it : AvItems(f)) if (it.level > from && it.level <= to) names.push_back(it.name);
    }
    if (names.empty()) return "";
    std::string s = "New: ";
    for (size_t i = 0; i < names.size() && i < 3; i++) s += (i ? ", " : "") + names[i];
    if (names.size() > 3) s += "...";
    return s;
}
