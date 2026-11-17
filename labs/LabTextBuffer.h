#pragma once
// Input-independent text model for the C++ lab IDE. No raylib dependency;
// editing, undo, navigation and diagnostics can be regression-tested offline.
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace questlabs {

struct LabTextBuffer {
    static constexpr size_t kMaxCodeSize = 24000; // same limit as the WASI judge
    static constexpr size_t kUndoDepth = 100;
    struct Snapshot { std::string text; size_t caret=0, anchor=0; };
    std::string text;
    size_t caret=0, anchor=0;
    bool limitReached=false;
    std::vector<Snapshot> undo, redo;

    static size_t LineStart(const std::string& s, size_t pos) {
        pos=std::min(pos,s.size());
        if(!pos)return 0;
        size_t p=s.rfind('\n',pos-1);
        return p==std::string::npos?0:p+1;
    }
    static size_t LineEnd(const std::string& s, size_t pos) {
        size_t p=s.find('\n',std::min(pos,s.size()));
        return p==std::string::npos?s.size():p;
    }
    static std::string Clean(std::string s) {
        std::string out;out.reserve(s.size());
        for(size_t i=0;i<s.size();++i) {
            const unsigned char c=(unsigned char)s[i];
            if(c=='\r') {if(i+1<s.size() && s[i+1]=='\n')continue;out+='\n';}
            else if(c=='\t')out+="    ";
            else if(c=='\n'||c>=32)out+=(char)c;
        }
        return out;
    }
    void SetText(const std::string& code) {
        text=Clean(code); if(text.size()>kMaxCodeSize)text.resize(kMaxCodeSize);
        caret=anchor=0;limitReached=false;undo.clear();redo.clear();
        // Begin at the first line where learners can type, not at file start.
        const auto todo=text.find("// your code here");
        if(todo!=std::string::npos) {
            const size_t end=LineEnd(text,todo);
            caret=anchor=end<text.size()?end+1: end;
            while(caret<text.size()&&text[caret]==' ')++caret;
            anchor=caret;
        }
    }
    std::string Text() const {return text;}
    std::pair<size_t,size_t> SelectedRange() const {return {std::min(anchor,caret),std::max(anchor,caret)};}
    bool Selected() const {return anchor!=caret;}
    std::string SelectedText() const {const auto r=SelectedRange();return text.substr(r.first,r.second-r.first);}
    void Move(size_t pos,bool extend=false) {caret=std::min(pos,text.size());if(!extend)anchor=caret;}
    void SelectAll(){anchor=0;caret=text.size();}
    std::pair<int,int> RowCol() const {
        int row=1;for(size_t i=0;i<caret;i++)if(text[i]=='\n')++row;
        return {row,(int)(caret-LineStart(text,caret))+1};
    }
    size_t PositionAt(int row,int column=1) const {
        size_t p=0;while(row>1&&p<text.size()){
            const auto n=text.find('\n',p);if(n==std::string::npos)return text.size();p=n+1;--row;
        }
        return std::min(p+(size_t)std::max(0,column-1),LineEnd(text,p));
    }
    int LineCount() const {return (int)std::count(text.begin(),text.end(),'\n')+1;}
    void Remember(){
        if(undo.size()>=kUndoDepth)undo.erase(undo.begin());
        undo.push_back({text,caret,anchor});redo.clear();limitReached=false;
    }
    bool Undo() {
        if(undo.empty())return false;
        redo.push_back({text,caret,anchor});auto old=std::move(undo.back());undo.pop_back();
        text=std::move(old.text);caret=old.caret;anchor=old.anchor;limitReached=false;return true;
    }
    bool Redo() {
        if(redo.empty())return false;
        undo.push_back({text,caret,anchor});auto next=std::move(redo.back());redo.pop_back();
        text=std::move(next.text);caret=next.caret;anchor=next.anchor;limitReached=false;return true;
    }
    bool ReplaceSelection(const std::string& raw) {
        const std::string value=Clean(raw);
        auto r=SelectedRange();
        if(text.size()-(r.second-r.first)+value.size()>kMaxCodeSize){limitReached=true;return false;}
        if(value.empty()&&r.first==r.second)return false;
        Remember();text.replace(r.first,r.second-r.first,value);caret=anchor=r.first+value.size();return true;
    }
    bool Insert(const std::string& s){return ReplaceSelection(s);}
    bool RemoveRange(size_t a,size_t b){
        if(a>=b||b>text.size())return false;
        Remember();text.erase(a,b-a);caret=anchor=a;return true;
    }
    bool Backspace(){
        if(Selected())return ReplaceSelection("");
        if(caret==0)return false;
        // Delete both halves of a fresh empty pair together.
        char l=text[caret-1],r=caret<text.size()?text[caret]:'\0';
        if((l=='('&&r==')')||(l=='['&&r==']')||(l=='{'&&r=='}')||(l=='"'&&r=='"'))
            return RemoveRange(caret-1,caret+1);
        return RemoveRange(caret-1,caret);
    }
    bool DeleteForward(){
        if(Selected())return ReplaceSelection("");
        if(caret==text.size())return false;
        return RemoveRange(caret,caret+1);
    }
    bool NewLine(){
        const auto r=SelectedRange();size_t start=LineStart(text,r.first),n=0;
        while(start+n<text.size() && text[start+n]==' ' && start+n<r.first)n++;
        size_t trim=r.first;while(trim>start && text[trim-1]==' ')--trim;
        const bool brace=trim>start && text[trim-1]=='{';
        const bool betweenBraces=brace&&!Selected()&&r.first<text.size()&&text[r.first]=='}';
        std::string insert="\n"+std::string(n+(brace?4:0),' ');
        if(betweenBraces)insert+="\n"+std::string(n,' ');
        if(!Insert(insert))return false;
        if(betweenBraces)caret=anchor=r.first+1+n+4;
        return true;
    }
    bool Type(char ch) {
        if(ch=='\n')return NewLine();
        if((unsigned char)ch<32)return false;
        if(!Selected()) {
            const char next=caret<text.size()?text[caret]:'\0';
            if((ch==')'||ch==']'||ch=='}'||ch=='"')&&next==ch){Move(caret+1);return true;}
            // Typing } on an indented blank line outdents one level.
            if(ch=='}'){
                const size_t start=LineStart(text,caret);
                if(caret>=start+4 && text.substr(start,caret-start)==std::string(caret-start,' ')){
                    anchor=caret-4;ReplaceSelection("");
                }
            }
        }
        const char close=ch=='('?')':ch=='['?']':ch=='{'?'}':ch=='"'?'"':'\0';
        if(close) {
            const auto r=SelectedRange();const std::string existing=SelectedText();
            if(!ReplaceSelection(std::string(1,ch)+existing+close))return false;
            caret=anchor=r.first+1+existing.size();return true;
        }
        return Insert(std::string(1,ch));
    }
    void MoveHorizontal(int direction,bool select=false,bool byWord=false){
        size_t p=caret;
        if(direction<0){
            if(p>0)--p;
            if(byWord) {while(p>0&&std::isspace((unsigned char)text[p]))--p;
                while(p>0&&(std::isalnum((unsigned char)text[p-1])||text[p-1]=='_'))--p;}
        }else if(p<text.size()){
            if(byWord) {while(p<text.size()&&(std::isalnum((unsigned char)text[p])||text[p]=='_'))++p;
                while(p<text.size()&&std::isspace((unsigned char)text[p])&&text[p]!='\n')++p;}
            else ++p;
        }
        Move(p,select);
    }
    void MoveVertical(int direction,bool select=false){
        const size_t start=LineStart(text,caret);
        const size_t column=caret-start;
        if(direction<0){if(start==0)return;const size_t previous=LineStart(text,start-1);Move(std::min(previous+column,start-1),select);}
        else{size_t next=LineEnd(text,caret);if(next==text.size())return;++next;Move(std::min(next+column,LineEnd(text,next)),select);}
    }
    void MoveHome(bool select=false){Move(LineStart(text,caret),select);}
    void MoveEnd(bool select=false){Move(LineEnd(text,caret),select);}
    bool Indent(bool outdent=false){
        const auto r=SelectedRange();size_t start=LineStart(text,r.first);
        size_t end=LineEnd(text,r.second);
        // Selection ending exactly at the start of the next line should not indent that line.
        if(Selected()&&r.second==LineStart(text,r.second)&&r.second>r.first)end=r.second-1;
        std::string newText=text;size_t pos=start;
        size_t left=r.first,right=r.second;bool changed=false;
        while(pos<=end&&pos<newText.size()+1){
            size_t amount=0;
            if(outdent){while(amount<4&&pos+amount<newText.size()&&newText[pos+amount]==' ')++amount;}
            if(outdent&&amount){newText.erase(pos,amount);changed=true;
                left=left>pos?left-std::min(amount,left-pos):left;
                right=right>pos?right-std::min(amount,right-pos):right;
                end-=std::min(amount,end-pos+1);
            }else if(!outdent){newText.insert(pos,4,' ');changed=true;
                if(left>=pos)left+=4;
                if(right>=pos)right+=4;
                end+=4;pos+=4;
            }
            const size_t newline=newText.find('\n',pos);if(newline==std::string::npos||newline>=end)break;
            pos=newline+1;
        }
        if(!changed||newText.size()>kMaxCodeSize){limitReached=newText.size()>kMaxCodeSize;return false;}
        Remember();text=std::move(newText);
        if(Selected()){anchor=left;caret=right;}else {caret=anchor=right;}
        return true;
    }
    bool ToggleComment(){
        const auto r=SelectedRange();
        const size_t begin=LineStart(text,r.first);
        const size_t last=(Selected()&&r.second>r.first&&r.second==LineStart(text,r.second))
                          ?r.second-1:r.second;
        const size_t end=LineEnd(text,last);
        const std::string original=text.substr(begin,end-begin);
        std::vector<std::string> lines;
        size_t at=0;
        while(at<=original.size()){
            const size_t next=original.find('\n',at);
            lines.push_back(original.substr(at,next==std::string::npos?std::string::npos:next-at));
            if(next==std::string::npos)break;
            at=next+1;
        }
        bool allComment=true;
        for(const auto& line:lines){
            const auto pos=line.find_first_not_of(' ');
            if(pos==std::string::npos)continue;
            if(line.compare(pos,2,"//")!=0){allComment=false;break;}
        }
        std::string replacement;
        for(size_t i=0;i<lines.size();++i){
            std::string line=lines[i];
            const auto pos=line.find_first_not_of(' ');
            if(pos!=std::string::npos){
                if(allComment)line.erase(pos,line.compare(pos,3,"// ")==0?3:2);
                else line.insert(pos,"// ");
            }
            if(i)replacement+='\n';
            replacement+=line;
        }
        if(text.size()-(end-begin)+replacement.size()>kMaxCodeSize){limitReached=true;return false;}
        Remember();text.replace(begin,end-begin,replacement);
        if(r.first!=r.second){anchor=begin;caret=begin+replacement.size();}
        else {caret=anchor=std::min(text.size(),begin+std::min(r.first-begin,replacement.size()));}
        return true;
    }
    bool FindNext(const std::string& query){
        if(query.empty())return false;
        size_t found=text.find(query,std::min(caret,text.size()));
        if(found==std::string::npos)found=text.find(query);
        if(found==std::string::npos)return false;
        anchor=found;caret=found+query.size();return true;
    }
    // Find the mate of a bracket adjacent to caret; unlike auto-indent this is visual-only.
    std::pair<size_t,size_t> BracketPair() const {
        if(text.empty())return {std::string::npos,std::string::npos};
        size_t pos=caret, n=text.size();
        auto opening=[](char c){return c=='('||c=='['||c=='{';};
        auto closing=[](char c){return c==')'||c==']'||c=='}';};
        if(pos>=n||(!opening(text[pos])&&!closing(text[pos]))) {
            if(pos==0)return {std::string::npos,std::string::npos};
            --pos;
            if(!opening(text[pos])&&!closing(text[pos]))return {std::string::npos,std::string::npos};
        }
        char c=text[pos],match=c=='('?')':c=='['?']':c=='{'?'}':c==')'?'(':c==']'?'[':'{';
        const bool forward=opening(c);int depth=0;
        if(forward){for(size_t i=pos;i<n;++i){if(text[i]==c)++depth;else if(text[i]==match&&--depth==0)return {pos,i};}}
        else {for(size_t i=pos+1;i-->0;){if(text[i]==c)++depth;else if(text[i]==match&&--depth==0)return {i,pos};if(i==0)break;}}
        return {std::string::npos,std::string::npos};
    }
};

// clang uses #line 1 "Your lab solution" in the test harness.
// Return the first learner source line referred to by a compiler diagnostic.
inline int LabErrorLine(const std::string& message){
    const std::string label="Your lab solution:";
    auto at=message.find(label);
    if(at==std::string::npos)return 0;
    at+=label.size();int result=0;
    if(at>=message.size()||!std::isdigit((unsigned char)message[at]))return 0;
    while(at<message.size()&&std::isdigit((unsigned char)message[at])){
        result=result*10+(message[at++]-'0');if(result>100000)return 0;
    }
    return at<message.size()&&message[at]==':'?result:0;
}
} // namespace questlabs
