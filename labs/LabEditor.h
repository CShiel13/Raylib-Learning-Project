#pragma once
// Labs-only editor: familiar IDE shortcuts, selection, undo/redo, find,
// auto-indent, horizontal scrolling and diagnostic markers. Other editors
// deliberately remain untouched.
#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>
#include <vector>
#include "LabTextBuffer.h"
#include "../ui/UI.h"

namespace questlabs {
struct LabEditor {
    LabTextBuffer doc;
    float scrollY=0, scrollX=0;
    int fontSize=16, errorLine=0;
    bool focused=true, findOpen=false, showKeys=false;
    bool wantRun=false,wantSave=false;
    std::string search;
    static constexpr float gutter=45, padding=10;

    void SetText(const std::string& text){doc.SetText(text);scrollY=scrollX=0;focused=true;findOpen=false;showKeys=false;errorLine=0;search.clear();wantRun=wantSave=false;}
    std::string Text() const{return doc.Text();}
    bool CloseFind(){if(!findOpen)return false;findOpen=false;focused=true;return true;}
    bool Undo(){focused=true;return doc.Undo();}
    bool Redo(){focused=true;return doc.Redo();}
    void FontLarger(){fontSize=std::min(20,fontSize+1);}
    void FontSmaller(){fontSize=std::max(13,fontSize-1);}
    float LineHeight() const{return fontSize*1.5f+3;}
    Rectangle Body(Rectangle full) const {return {full.x,full.y+(findOpen?34.0f:0.0f),full.width,full.height-(findOpen?34.0f:0.0f)};}
    void EnsureCaret(Rectangle area){
        auto pos=doc.RowCol();float lh=LineHeight();
        const float top=(pos.first-1)*lh, bottom=top+lh;
        if(top<scrollY)scrollY=top;
        if(bottom>scrollY+area.height-2*padding)scrollY=bottom-(area.height-2*padding);
        const size_t lineStart=LabTextBuffer::LineStart(doc.text,doc.caret);
        const float px=TxtW(doc.text.substr(lineStart,doc.caret-lineStart),(float)fontSize,true);
        const float available=area.width-gutter-30;
        if(px-scrollX<0)scrollX=std::max(0.0f,px-8);
        if(px-scrollX>available)scrollX=std::max(0.0f,px-available+10);
        const float limit=std::max(0.0f,doc.LineCount()*lh+2*padding-area.height);
        scrollY=std::max(0.0f,std::min(limit,scrollY));
    }
    void Update(Rectangle full){
        wantRun=wantSave=false;
        const bool ctrl=IsKeyDown(KEY_LEFT_CONTROL)||IsKeyDown(KEY_RIGHT_CONTROL);
        const bool shift=IsKeyDown(KEY_LEFT_SHIFT)||IsKeyDown(KEY_RIGHT_SHIFT);
        const auto repeat=[](int key){return IsKeyPressed(key)||IsKeyPressedRepeat(key);};
        Vector2 m=Mouse();
        Rectangle body=Body(full);
        bool hover=CheckCollisionPointRec(m,full);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            focused=CheckCollisionPointRec(m,body);
            if(focused){
                const float lh=LineHeight();
                int row=(int)std::floor((m.y-body.y-padding+scrollY)/lh)+1;
                row=std::max(1,std::min(doc.LineCount(),row));
                const size_t begin=doc.PositionAt(row,1),end=LabTextBuffer::LineEnd(doc.text,begin);
                const float offset=std::max(0.0f,m.x-(body.x+gutter+padding)+scrollX);
                size_t column=0;
                while(begin+column<end){
                    const float middle=TxtW(doc.text.substr(begin,column+1),(float)fontSize,true);
                    if(middle>offset+TxtW("m",(float)fontSize,true)*0.3f)break;
                    ++column;
                }
                doc.Move(begin+column,shift);
            }
        }
        const float wheel=GetMouseWheelMove();
        if(hover&&wheel!=0){
            if(ctrl){if(wheel>0)FontLarger();else FontSmaller();}
            else if(shift)scrollX=std::max(0.0f,scrollX-wheel*45);
            else scrollY-=wheel*LineHeight()*3;
        }
        // These work anywhere on the lab screen, even after clicking a button.
        if(ctrl&&IsKeyPressed(KEY_ENTER))wantRun=true;
        if(ctrl&&IsKeyPressed(KEY_S))wantSave=true;
        if(ctrl&&IsKeyPressed(KEY_F)){findOpen=true;search.clear();focused=false;}
        if(findOpen){
            if(IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_KP_ENTER)){doc.FindNext(search);findOpen=false;focused=true;}
            if(repeat(KEY_BACKSPACE)&&!search.empty())search.pop_back();
            int c=GetCharPressed();while(c>0){if(!ctrl&&c>=32&&c<=126&&search.size()<64)search+=(char)c;c=GetCharPressed();}
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)&&!CheckCollisionPointRec(m,{full.x,full.y,full.width,34})){
                // Clicking code closes Find; the click position becomes the caret next time.
                findOpen=false;
            }
            EnsureCaret(Body(full));return;
        }
        if(!focused){int c=GetCharPressed();while(c>0)c=GetCharPressed();return;}
        bool moved=false;
        if(ctrl){
            if(IsKeyPressed(KEY_Z)){moved=shift?doc.Redo():doc.Undo();}
            else if(IsKeyPressed(KEY_Y)){moved=doc.Redo();}
            else if(IsKeyPressed(KEY_A)){doc.SelectAll();moved=true;}
            else if(IsKeyPressed(KEY_C)){if(doc.Selected()){const auto s=doc.SelectedText();SetClipboardText(s.c_str());}}
            else if(IsKeyPressed(KEY_X)){if(doc.Selected()){const auto s=doc.SelectedText();SetClipboardText(s.c_str());moved=doc.ReplaceSelection("");}}
            else if(IsKeyPressed(KEY_V)){const char* c=GetClipboardText();if(c)moved=doc.Insert(c);}
            else if(IsKeyPressed(KEY_SLASH)){moved=doc.ToggleComment();}
            else if(repeat(KEY_LEFT)){doc.MoveHorizontal(-1,shift,true);moved=true;}
            else if(repeat(KEY_RIGHT)){doc.MoveHorizontal(1,shift,true);moved=true;}
            else if(repeat(KEY_HOME)){doc.Move(0,shift);moved=true;}
            else if(repeat(KEY_END)){doc.Move(doc.text.size(),shift);moved=true;}
        }
        int c=GetCharPressed();while(c>0){if(!ctrl && c>=32&&c<=126)moved=doc.Type((char)c)||moved;c=GetCharPressed();}
        if(!ctrl){
            if(repeat(KEY_ENTER)||repeat(KEY_KP_ENTER))moved=doc.NewLine()||moved;
            if(repeat(KEY_TAB))moved=(shift?doc.Indent(true):doc.Selected()?doc.Indent(false):doc.Insert("    "))||moved;
            if(repeat(KEY_BACKSPACE))moved=doc.Backspace()||moved;
            if(repeat(KEY_DELETE))moved=doc.DeleteForward()||moved;
            if(repeat(KEY_LEFT)){doc.MoveHorizontal(-1,shift);moved=true;}
            if(repeat(KEY_RIGHT)){doc.MoveHorizontal(1,shift);moved=true;}
        }
        if(repeat(KEY_UP)){doc.MoveVertical(-1,shift);moved=true;}
        if(repeat(KEY_DOWN)){doc.MoveVertical(1,shift);moved=true;}
        if(!ctrl){
            if(repeat(KEY_HOME)){doc.MoveHome(shift);moved=true;}
            if(repeat(KEY_END)){doc.MoveEnd(shift);moved=true;}
        }
        if(repeat(KEY_PAGE_UP)){for(int i=0;i<10;i++)doc.MoveVertical(-1,shift);moved=true;}
        if(repeat(KEY_PAGE_DOWN)){for(int i=0;i<10;i++)doc.MoveVertical(1,shift);moved=true;}
        if(moved)EnsureCaret(body);
        const float limit=std::max(0.0f,doc.LineCount()*LineHeight()+2*padding-body.height);
        scrollY=std::max(0.0f,std::min(limit,scrollY));
    }
    static Color TokenColor(const std::string& token,bool function){
        static const char* keywords[]={"auto","bool","break","case","catch","char","const","constexpr","continue","default","delete","do","double","else","enum","explicit","false","float","for","if","inline","int","long","namespace","new","nullptr","return","short","signed","sizeof","static","struct","switch","template","this","throw","true","try","typedef","typename","unsigned","using","void","volatile","while"};
        for(auto kw:keywords)if(token==kw)return kViolet;
        if(token=="std"||token=="size_t")return kSky;
        return function?kSky:Color{205,214,235,255};
    }
    void Draw(Rectangle full,float time) const {
        Inset(full,10,Color{10,14,24,255},kBorder);
        if(findOpen){
            Box({full.x+6,full.y+5,full.width-12,29},7,kSurface2);
            Txt("Find:",full.x+16,full.y+9,13,kMuted);
            Txt(Fit(search.empty()?"Type to search...":search,full.width-185,13,true),full.x+71,full.y+9,13,search.empty()?kMuted:kText,true);
            TxtR("Enter next",full.x+full.width-18,full.y+9,13,kSky);
        }
        const Rectangle area=Body(full);
        ScissorOn(area.x+2,area.y+2,area.width-4,area.height-4);
        DrawRectangle((int)area.x+2,(int)area.y+2,(int)gutter,(int)area.height-4,Al(Color{245,249,255,255},0.035f));
        const float lh=LineHeight();
        const auto selection=doc.SelectedRange();
        const auto brackets=doc.BracketPair();
        const auto current=doc.RowCol();
        size_t start=0;int row=1;
        while(start<=doc.text.size()){
            size_t end=LabTextBuffer::LineEnd(doc.text,start);
            const float y=area.y+padding+(row-1)*lh-scrollY;
            if(y+lh>=area.y && y<area.y+area.height){
                const bool active=row==current.first;
                if(active)DrawRectangle((int)(area.x+gutter+2),(int)y-1,(int)(area.width-gutter-5),(int)lh,Al(kSky,focused?0.09f:0.035f));
                if(errorLine==row){
                    DrawRectangle((int)(area.x+gutter-3),(int)y-1,3,(int)lh,kCoral);
                    DrawRectangle((int)(area.x+gutter+2),(int)y-1,(int)(area.width-gutter-5),(int)lh,Al(kCoral,0.08f));
                }
                TxtR(std::to_string(row),area.x+gutter-10,y+2,13,errorLine==row?kCoral:active?kText:kLock,true);
                const std::string line=doc.text.substr(start,end-start);
                const float x0=area.x+gutter+padding-scrollX;
                // Indentation guides are subtle enough not to compete with text.
                int spaces=0;while(spaces<(int)line.size()&&line[spaces]==' ')++spaces;
                for(int indent=4;indent<=spaces;indent+=4){
                    const float gx=x0+TxtW(line.substr(0,(size_t)indent),(float)fontSize,true)-4;
                    DrawLineEx({gx,y},{gx,y+lh-2},1,Al(kMuted,0.16f));
                }
                if(selection.second>start&&selection.first<end){
                    const size_t a=std::max(selection.first,start)-start, b=std::min(selection.second,end)-start;
                    float ax=x0+TxtW(line.substr(0,a),(float)fontSize,true);
                    float bx=x0+TxtW(line.substr(0,b),(float)fontSize,true);
                    DrawRectangle((int)ax,(int)y-1,(int)std::max(1.0f,bx-ax),(int)lh,Al(kSky,0.32f));
                }
                // A selected newline also receives a marker on an otherwise empty line.
                if(selection.first<=end&&selection.second>end&&end<doc.text.size()){
                    float nx=x0+TxtW(line,(float)fontSize,true);
                    DrawRectangle((int)nx,(int)y-1,7,(int)lh,Al(kSky,0.32f));
                }
                for(size_t k=0;k<line.size();){
                    size_t st=k;Color c={205,214,235,255};
                    if(line[k]=='/'&&k+1<line.size()&&line[k+1]=='/') {k=line.size();c=kMuted;}
                    else if(line[k]=='"'||line[k]=='\''){
                        const char quote=line[k++];
                        while(k<line.size()) {if(line[k]=='\\'){k=std::min(k+2,line.size());continue;} if(line[k++]==quote)break;}
                        c=kAmber;
                    }else if(std::isalpha((unsigned char)line[k])||line[k]=='_'){
                        while(k<line.size()&&(std::isalnum((unsigned char)line[k])||line[k]=='_'))++k;
                        c=TokenColor(line.substr(st,k-st),k<line.size()&&line[k]=='(');
                    }else if(std::isdigit((unsigned char)line[k])){
                        while(k<line.size()&&(std::isalnum((unsigned char)line[k])||line[k]=='.'))++k;
                        c=kMint;
                    }else ++k;
                    const float tx=x0+TxtW(line.substr(0,st),(float)fontSize,true);
                    if(tx<area.x+area.width&&tx+TxtW(line.substr(st,k-st),(float)fontSize,true)>area.x+gutter)
                        Txt(line.substr(st,k-st),tx,y,fontSize,c,true);
                }
                // Matching braces are outlined, not replaced with a distracting color.
                for(const auto match:{brackets.first,brackets.second}) {
                    if(match!=std::string::npos&&match>=start&&match<end){
                        const float x=x0+TxtW(line.substr(0,match-start),(float)fontSize,true);
                        DrawRectangleLinesEx({x-1,y-1,TxtW(line.substr(match-start,1),(float)fontSize,true)+2,lh-2},1.5f,kAmber);
                    }
                }
                if(active&&focused&&(int)(time*2)%2==0){
                    const float x=x0+TxtW(line.substr(0,doc.caret-start),(float)fontSize,true);
                    DrawRectangle((int)x,(int)y-1,2,(int)lh-2,kText);
                }
            }
            if(end==doc.text.size())break;
            start=end+1;++row;
        }
        EndScissorMode();
        const float total=doc.LineCount()*lh+2*padding;
        if(total>area.height){
            const float thumb=std::max(25.0f,area.height*area.height/total);
            const float y=area.y+(scrollY/std::max(1.0f,total-area.height))*(area.height-thumb);
            Box({area.x+area.width-5,y,3,thumb},2,Al(kMuted,0.55f));
        }
        if(doc.limitReached){
            Box({area.x+9,area.y+area.height-31,312,25},7,Al(kAmber,0.94f));
            Txt("24 KB code limit - shorten the solution",area.x+16,area.y+area.height-27,12,kBg);
        }
        if(showKeys){
            const Rectangle pop{area.x+36,area.y+area.height-150,area.width-72,132};
            BoxB(pop,9,kSurface2,kSky,1);
            Txt("EDITOR SHORTCUTS",pop.x+12,pop.y+9,14,kText);
            Txt("Ctrl+Enter Run  |  Ctrl+S Save  |  Ctrl+F Find",pop.x+12,pop.y+34,13,kMuted);
            Txt("Ctrl+Z/Y Undo/Redo  |  Ctrl+/ Comment",pop.x+12,pop.y+57,13,kMuted);
            Txt("Tab / Shift+Tab Indent  |  Ctrl+A/C/V Select/Copy/Paste",pop.x+12,pop.y+80,12,kMuted);
            Txt("Ctrl+mouse wheel Zoom  |  Shift+wheel Scroll sideways",pop.x+12,pop.y+104,12,kMuted);
        }
    }
};
} // namespace questlabs
