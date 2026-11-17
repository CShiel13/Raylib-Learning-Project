#pragma once
// Shared two-column destination layout for Practice, Bonus Games and Other Languages.
// Keeps titles, descriptions, metadata, click targets and spacing identical in each hub.
#include <algorithm>
#include <string>
#include "Widgets.h"
#include "Scroller.h"

namespace HubMenu {
constexpr float Left = 35.0f;
constexpr float Right = 567.0f;
constexpr float CardWidth = 498.0f;
constexpr float FirstCardY = 190.0f;
constexpr float RowPitch = 163.0f;
constexpr float CardHeight = 147.0f;

inline Rectangle CardRect(int index) {
    return { index % 2 ? Right : Left, FirstCardY + (index / 2) * RowPitch, CardWidth, CardHeight };
}
inline float ScrollHeight(int cards) {
    const float bottom = cards > 0 ? CardRect(cards - 1).y + CardHeight + 25.0f : 700.0f;
    return std::max(626.0f, bottom - 74.0f);
}
inline void Heading(const std::string& heading, const std::string& explanation) {
    Txt(heading, Left, 95, 34, kText);
    Txt(Fit(explanation, 1030, 16), Left, 142, 16, kMuted);
}
// Entire card is interactive: consistent with the main-menu destination tiles.
// glyph replaces the generic icon for language course initials (J, Py, C#, JS).
inline bool Card(Rectangle r, const std::string& title, const std::string& description,
                 const std::string& detail, const std::string& action,
                 Color accent, int icon, const std::string& glyph = "") {
    const bool hovered = CheckCollisionPointRec(Mouse(), r);
    const Color edge = hovered ? accent : kBorder;
    Card3D(r, 19, hovered ? Mix(kSurface, accent, 0.09f) : kSurface, edge, 5);
    Box({r.x + 19, r.y + 18, 64, 64}, 17, accent);
    if (glyph.empty()) Icon(icon, r.x + 51, r.y + 50, 17, OnColor(accent));
    else TxtC(glyph, r.x + 51, r.y + 34, glyph.size() > 2 ? 21 : 26, OnColor(accent));
    const float tx = r.x + 101;
    const float textWidth = r.width - 121;
    Txt(Fit(title, textWidth, 22), tx, r.y + 13, 22, kText);
    Txt(Fit(description, textWidth, 15), tx, r.y + 49, 15, kMuted);
    Txt(Fit(detail, textWidth, 13), tx, r.y + 77, 13, accent);
    DrawLineEx({tx, r.y + 105}, {r.x + r.width - 20, r.y + 105}, 1, kBorder);
    Txt(action + "  >", tx, r.y + 115, 16, hovered ? kText : accent);
    return hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}
} // namespace HubMenu
