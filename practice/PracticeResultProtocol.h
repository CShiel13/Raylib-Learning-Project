#pragma once
#include <string>
#include <vector>
#include "PracticeTypes.h"

inline bool PracticeGridMatches(const std::vector<unsigned char>& output, const std::vector<unsigned char>& target) {
    return output == target;
}

inline bool ParsePracticeResultRecord(const std::string& record, PracticeRunMode mode, int side, PracticeRunResult& out) {
    std::vector<std::string> fields; size_t fieldStart = 0;
    while (true) { size_t sep = record.find(':', fieldStart); fields.push_back(record.substr(fieldStart, sep == std::string::npos ? sep : sep - fieldStart)); if (sep == std::string::npos) break; fieldStart = sep + 1; }
    if (fields.size() != 8 || fields[0] != "JQRP1") { out.message = "Malformed runner result (header or version)."; return false; }
    auto number = [](const std::string& s, unsigned& n) {
        if (s.empty()) return false; n=0;
        for (unsigned char c : s) { if(c<'0'||c>'9'||n>(0xffffffffu-(c-'0'))/10) return false; n=n*10+(c-'0'); }
        return true;
    };
    unsigned parsedMode=0,bad=0,cellCount=0,textLength=0,overflow=0;
    if(!number(fields[1],parsedMode)||!number(fields[2],bad)||!number(fields[3],cellCount)||!number(fields[4],textLength)||!number(fields[7],overflow)) {
        out.message = "Malformed runner result (invalid numeric field)."; return false;
    }
    size_t expectedCells = mode == PracticeRunMode::GridDrawing ? (size_t)side * side : 0;
    if(parsedMode!=(unsigned)mode||bad>1||overflow>1||cellCount!=expectedCells||textLength>4096||fields[5].size()!=(size_t)cellCount*2||fields[6].size()!=(size_t)textLength*2) {
        out.message = "Malformed runner result (length, mode, or bounds check failed)."; return false;
    }
    auto hexByte=[](const std::string&s,size_t at,unsigned char& value) {
        auto digit=[](char c)->int { if(c>='0'&&c<='9')return c-'0'; if(c>='a'&&c<='f')return c-'a'+10; if(c>='A'&&c<='F')return c-'A'+10; return -1; };
        if(at+1>=s.size())return false; int a=digit(s[at]),b=digit(s[at+1]); if(a<0||b<0)return false; value=(unsigned char)(a*16+b); return true;
    };
    out.cells.resize(cellCount);
    for(size_t i=0;i<cellCount;++i) if(!hexByte(fields[5],i*2,out.cells[i])||out.cells[i]>5) { out.message="Malformed runner result (invalid grid data)."; return false; }
    out.learnerOutput.resize(textLength);
    for(size_t i=0;i<textLength;++i) { unsigned char c=0; if(!hexByte(fields[6],i*2,c)) { out.message="Malformed runner result (invalid output data)."; return false; } out.learnerOutput[i]=(char)c; }
    if(overflow) { out.cells.clear(); out.learnerOutput.clear(); out.message="Learner output exceeded the 4096-byte limit."; return false; }
    if(bad) { out.cells.clear(); out.learnerOutput.clear(); out.message="drawCell used an invalid coordinate or colour."; return false; }
    return true;
}
