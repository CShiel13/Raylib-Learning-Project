#pragma once
// Small independent save file: never changes the existing Player save format.
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include "LabBank.h"
namespace questlabs {
inline std::vector<std::string> LoadDrafts(const std::string& path="cppquest_lab_drafts.dat") {
    std::vector<std::string> drafts(All().size());
    std::ifstream in(path,std::ios::binary);
    std::string magic;size_t n=0;
    if(!(in>>magic>>n)||magic!="JQLAB2"||n>All().size())return drafts;
    for(size_t j=0;j<n;j++){
        size_t id=0;std::string text;
        if(!(in>>id>>std::quoted(text)))break;
        if(id<drafts.size()&&text.size()<=24000)drafts[id]=std::move(text);
    }
    return drafts;
}
inline bool SaveDrafts(const std::vector<std::string>& drafts,const std::string& path="cppquest_lab_drafts.dat") {
    std::ofstream out(path,std::ios::binary|std::ios::trunc);
    if(!out)return false;
    out<<"JQLAB2 "<<drafts.size()<<'\n';
    for(size_t i=0;i<drafts.size();i++)out<<i<<' '<<std::quoted(drafts[i].substr(0,24000))<<'\n';
    return (bool)out;
}
}
