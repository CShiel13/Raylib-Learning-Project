#pragma once
// Independent, locally persisted engagement progress; existing Player save format is untouched.
// Date-bound rewards use local calendar days; timed boosts use an absolute Unix expiry.
#include <algorithm>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>

namespace engagement {
inline std::tm LocalTime(std::time_t now) {
    std::tm t{};
#ifdef _WIN32
    localtime_s(&t, &now);
#else
    localtime_r(&now, &t);
#endif
    return t;
}
inline int DayId(std::time_t now) {
    const auto t = LocalTime(now);
    return (t.tm_year + 1900) * 1000 + t.tm_yday;
}
inline int WeekId(std::time_t now) {
    auto t = LocalTime(now);
    t.tm_hour = 12; t.tm_min = t.tm_sec = 0;
    t.tm_mday -= (t.tm_wday + 6) % 7; // local Monday, resilient across year boundaries
    const auto monday = LocalTime(std::mktime(&t));
    return (monday.tm_year + 1900) * 1000 + monday.tm_yday;
}
inline uint32_t Checksum(const std::string& text) {
    uint32_t h = 2166136261u;
    for (unsigned char c : text) { h ^= c; h *= 16777619u; }
    return h; // Typo detection, NOT a cryptographic signature.
}
inline std::string Hex(uint32_t n) {
    std::ostringstream out; out << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << n;
    return out.str();
}

class Rewards {
public:
    explicit Rewards(std::string path = "cppquest_engagement.dat") : path_(std::move(path)) { Load(); Refresh(); Save(); }
    int Tokens() const { return tokens_; }
    int DailyCorrect() const { return dailyCorrect_; }
    int DailyLessons() const { return dailyLessons_; }
    int DailyXp() const { return dailyXp_; }
    int DayStreak() const { return streak_; }
    int WeeklySelfXp() const { return selfXp_; }
    int WeeklyFriendXp() const { return friendXp_; }
    bool PartnerConnected() const { return friendId_ != 0; }
    int WeeklyTarget() const { return 300; }
    bool WeeklyClaimed() const { return weeklyClaimed_; }
    int DailyTarget(int i) const { return i == 0 ? 5 : i == 1 ? 1 : 100; }
    int DailyProgress(int i) const { return i == 0 ? dailyCorrect_ : i == 1 ? dailyLessons_ : dailyXp_; }
    bool DailyClaimed(int i) const { return i >= 0 && i < 3 && (dailyClaimed_ & (1 << i)); }
    bool CanClaimDaily(int i) const {
        return i >= 0 && i < 3 && !DailyClaimed(i) && DailyProgress(i) >= DailyTarget(i);
    }
    bool CanClaimChest(int i) const {
        return i >= 0 && i < 2 && (earnedChests_ & (1 << i)) && !(claimedChests_ & (1 << i));
    }
    bool Active(std::time_t now = std::time(nullptr)) const { return expiry_ > static_cast<int64_t>(now); }
    int SecondsRemaining(std::time_t now = std::time(nullptr)) const {
        return Active(now) ? static_cast<int>(std::min<int64_t>(expiry_ - now, 86400)) : 0;
    }
    int Multiplier(std::time_t now = std::time(nullptr)) const { return Active(now) ? 2 : 1; }
    bool Activate(std::time_t now = std::time(nullptr)) {
        Refresh(now);
        if (!tokens_ || Active(now)) return false;
        --tokens_; expiry_ = static_cast<int64_t>(now) + 15 * 60;
        Save(); return true;
    }
    void Refresh(std::time_t now = std::time(nullptr)) {
        const int day = DayId(now), week = WeekId(now);
        if (day != dayId_) {
            if (dayId_ != 0 && !(dayId_ == DayId(now - 86400) && studied_)) { streak_ = 0; milestoneMask_ = 0; }
            dayId_ = day; dailyCorrect_ = dailyLessons_ = dailyXp_ = 0;
            dailyClaimed_ = 0; earnedChests_ = claimedChests_ = 0; studied_ = false;
        }
        if (week != weekId_) {
            weekId_ = week; selfXp_ = friendXp_ = 0; friendId_ = 0; weeklyClaimed_ = false;
        }
        if (!id_) {
            std::random_device rd;
            id_ = static_cast<uint32_t>(rd()) ^ static_cast<uint32_t>(now);
            if (!id_) id_ = 0xC0FFEE42u;
        }
    }
    void RecordAnswer(bool correct, std::time_t now = std::time(nullptr)) {
        Refresh(now); if (correct) { ++dailyCorrect_; Study(now); } Save();
    }
    void RecordLesson(std::time_t now = std::time(nullptr)) {
        Refresh(now); ++dailyLessons_; Study(now);
        const int hour = LocalTime(now).tm_hour;
        if (hour < 12) earnedChests_ |= 1;
        if (hour >= 18) earnedChests_ |= 2;
        Save();
    }
    void RecordXp(int awarded, std::time_t now = std::time(nullptr)) {
        Refresh(now); if (awarded <= 0) return;
        dailyXp_ += awarded; selfXp_ += awarded; Study(now); Save();
    }
    bool ClaimDaily(int i) {
        Refresh(); if (!CanClaimDaily(i)) return false;
        dailyClaimed_ |= 1 << i; ++tokens_; Save(); return true;
    }
    bool ClaimChest(int i) {
        Refresh(); if (!CanClaimChest(i)) return false;
        claimedChests_ |= 1 << i; ++tokens_; Save(); return true;
    }
    bool CanClaimWeekly() const { return PartnerConnected() && !weeklyClaimed_ && selfXp_ + friendXp_ >= WeeklyTarget(); }
    bool ClaimWeekly() {
        Refresh(); if (!CanClaimWeekly()) return false;
        weeklyClaimed_ = true; tokens_ += 2; Save(); return true;
    }
    std::string ShareCode() {
        Refresh();
        std::string body = "CQ1-" + std::to_string(weekId_) + "-" + Hex(id_) + "-" + std::to_string(selfXp_);
        return body + "-" + Hex(Checksum(body));
    }
    bool Import(const std::string& code) {
        Refresh();
        // Codes only share a sender's cumulative XP, never another person's imported progress.
        std::istringstream in(code);
        std::string ver, wk, who, xp, tag;
        if (!std::getline(in, ver, '-') || !std::getline(in, wk, '-') ||
            !std::getline(in, who, '-') || !std::getline(in, xp, '-') ||
            !std::getline(in, tag, '-') || in.rdbuf()->in_avail() || ver != "CQ1" ||
            wk.empty() || who.size() != 8 || xp.empty() || tag.size() != 8) return false;
        try {
            int week = std::stoi(wk); unsigned long uid = std::stoul(who, nullptr, 16);
            int points = std::stoi(xp);
            if (week != weekId_ || uid == 0 || uid == id_ || points < 0 || points > 1000000 ||
                (friendId_ != 0 && friendId_ != uid)) return false;
            std::string body = "CQ1-" + wk + "-" + who + "-" + xp;
            if (tag != Hex(Checksum(body))) return false;
            friendId_ = static_cast<uint32_t>(uid);
            friendXp_ = std::max(friendXp_, points); Save(); return true;
        } catch (...) { return false; }
    }
    std::string PartnerLabel() const { return friendId_ ? "Partner #" + Hex(friendId_).substr(0, 4) : "No partner yet"; }
    void Save() const {
        std::ofstream f(path_, std::ios::trunc);
        if (!f) return;
        f << "CQENG1 " << id_ << ' ' << dayId_ << ' ' << weekId_ << ' '
          << tokens_ << ' ' << expiry_ << ' ' << streak_ << ' ' << milestoneMask_ << ' '
          << studied_ << ' ' << dailyCorrect_ << ' ' << dailyLessons_ << ' ' << dailyXp_ << ' '
          << dailyClaimed_ << ' ' << earnedChests_ << ' ' << claimedChests_ << ' '
          << selfXp_ << ' ' << friendXp_ << ' ' << friendId_ << ' ' << weeklyClaimed_ << '\n';
    }
private:
    void Load() {
        std::ifstream f(path_); std::string version;
        if (!(f >> version) || version != "CQENG1") return;
        if (!(f >> id_ >> dayId_ >> weekId_ >> tokens_ >> expiry_ >> streak_ >> milestoneMask_
              >> studied_ >> dailyCorrect_ >> dailyLessons_ >> dailyXp_ >> dailyClaimed_
              >> earnedChests_ >> claimedChests_ >> selfXp_ >> friendXp_ >> friendId_ >> weeklyClaimed_)) {
            id_ = 0; dayId_ = weekId_ = tokens_ = streak_ = milestoneMask_ = 0;
            expiry_ = 0; studied_ = false; dailyCorrect_ = dailyLessons_ = dailyXp_ = 0;
            dailyClaimed_ = earnedChests_ = claimedChests_ = selfXp_ = friendXp_ = 0;
            friendId_ = 0; weeklyClaimed_ = false;
        }
        tokens_ = std::max(0, std::min(100, tokens_));
    }
    void Study(std::time_t now) {
        if (studied_) return;
        // A new local study day, not every answer, increments the streak once.
        studied_ = true; ++streak_;
        for (int milestone : {3, 7, 14, 30}) {
            if (streak_ >= milestone && !(milestoneMask_ & (1 << (milestone == 3 ? 0 : milestone == 7 ? 1 : milestone == 14 ? 2 : 3)))) {
                milestoneMask_ |= 1 << (milestone == 3 ? 0 : milestone == 7 ? 1 : milestone == 14 ? 2 : 3);
                ++tokens_;
            }
        }
    }
    std::string path_;
    uint32_t id_ = 0, friendId_ = 0;
    int dayId_ = 0, weekId_ = 0;
    int tokens_ = 0, streak_ = 0, milestoneMask_ = 0;
    int dailyCorrect_ = 0, dailyLessons_ = 0, dailyXp_ = 0;
    int dailyClaimed_ = 0, earnedChests_ = 0, claimedChests_ = 0;
    int selfXp_ = 0, friendXp_ = 0;
    int64_t expiry_ = 0;
    bool studied_ = false, weeklyClaimed_ = false;
};
} // namespace engagement
