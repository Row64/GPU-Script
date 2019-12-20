#ifndef  UILIB_CPP
#define  UILIB_CPP

#include "UILib.h"
#include <math.h>
#include <cmath>

// --------------------------------------------------------------------- //
// [UILib] Core                                                          //
// --------------------------------------------------------------------- //

namespace UILib {

    #ifndef UILibIS
    InputStateData*   UILibIS = NULL;
    #endif

    #ifndef UILibDS
    DisplayStateData* UILibDS = NULL;
    #endif

    #ifndef UILibWindowHandle
    void* UILibWindowHandle = NULL;
    #endif


    void UISetInputStatePointer( InputStateData* inMS ) {
        UILibIS = inMS;
    }

    InputStateData* UIGetInputState() {
        return UILibIS;
    }
    int GetKeyDown() {
        InputStateData& ins = *UILibIS;
        //return vec2(m.Position.X, m.Position.Y);
        return ins.Key.Key;
    }
    float GetMouseScroll() {  // reading will set to zero so you can get delta next update
        InputStateData& m = *UILibIS;
        float scrollY = m.ScrollY;
        m.ScrollY = 0;
        return scrollY;
    }
    vec2 GetMousePos() {
        InputStateData& m = *UILibIS;
        return vec2(m.Position.X, m.Position.Y);
    }
    vec2 GetLocalMousePos(){
        InputStateData& m = *UILibIS;
        ImVec2 wPos = GImGui->CurrentWindow->Pos;
        return vec2(m.Position.X - wPos.x, m.Position.Y - wPos.y);
    }
    
    void UISetDisplayStatePointer( DisplayStateData* inDS ) {
        UILibDS = inDS;
    }

    DisplayStateData* UIGetDisplayState() {
        return UILibDS;
    }

    vec2 UIGetWindowSize() {
        DisplayStateData& d = *UILibDS;
        return vec2(d.WindowSize.W, d.WindowSize.H);
    }

    vec2 UIGetFramebufSize() {
        DisplayStateData& d = *UILibDS;
        return vec2(d.FramebufferSize.W, d.FramebufferSize.H);
    }

    void UISetWindowHandlePointer( void* inWin ) {
        UILibWindowHandle = inWin;
    }

    void* UIGetWindowHandle() {
        return UILibWindowHandle;
    }
    ImVec2 Pos(float x, float y, ImVec2 winPos){
        // Position to Window Local Space
        return ImVec2(x + winPos.x, y + winPos.y);
    }
    ImU32 Cl(unsigned int hexValue){
        double cr = ((hexValue >> 16) & 0xFF);  // Extract the RR byte
        double cg = ((hexValue >> 8) & 0xFF);   // Extract the GG byte
        double cb = ((hexValue) & 0xFF);        // Extract the BB byte
        ImU32 clr = IM_COL32( cr, cg, cb, 255);
        return clr;
    }
    ImU32 ClearCl(unsigned int hexValue){
        double cr = ((hexValue >> 16) & 0xFF);  // Extract the RR byte
        double cg = ((hexValue >> 8) & 0xFF);   // Extract the GG byte
        double cb = ((hexValue) & 0xFF);        // Extract the BB byte
        ImU32 clr = IM_COL32( cr, cg, cb, 0);
        return clr;
    }
    float YToGlobal(float inY){
        ImVec2 wPos = GImGui->CurrentWindow->Pos;
        return inY - wPos.y;
    }
    ImRect GetCenteredBB(float inX, float inY, float inW, float inH){
        ImVec2 wPos = GImGui->CurrentWindow->Pos;
        float gX = wPos.x + inX;
        float gY = wPos.y + inY;
        ImRect gBB = ImRect(gX-inW*0.5f, gY-inH*0.5f, gX+inW*0.5f, gY+inH*0.5f);
        return gBB;
    }
    ImRect GetGlobalBB(float inX, float inY, float inW, float inH){
        ImVec2 wPos = GImGui->CurrentWindow->Pos;
        float gX = wPos.x + inX;
        float gY = wPos.y + inY;
        ImRect gBB = ImRect(gX,  gY, gX + inW, gY + inH);
        return gBB;
    }
    ImRect GetBB(float inX, float inY, float inW, float inH){
        ImRect gBB = ImRect(inX,  inY, inX + inW, inY + inH);
        return gBB;
    }
    void LogPos(string inTxt, vec2 inPos){
        cout << inTxt << inPos.x << ", " << inPos.y << "\n";
    }
    void LogBB(ImRect inBB){
        cout << "min(" << inBB.Min.x <<", "<< inBB.Min.y <<"), max(" << inBB.Max.x <<", "<< inBB.Max.y << ")\n";
    }
    bool GetOverState(ImRect inBB){
        if (inBB.Contains(GImGui->IO.MousePos)){return true;}
        return false;
        // if you also want to check if the window is active, use this:
        // return ImGui::ItemHoverable(inBB, inId);
    }
    bool MouseDown(){
        InputStateData& m = *UILibIS;
        if(m.Buttons[0].IsPressed == 0){ return false; }
        return true;
    }
    bool GetActiveState(bool inOver, ImGuiID inId){
        
        if(DragStart() && inOver){
            ImGuiWindow* window = GImGui->CurrentWindow;
            ImGui::SetActiveID(inId, window);
            ImGui::SetFocusID(inId, window);
            ImGui::FocusWindow(window);
        }
        if(GImGui->ActiveId == inId ){
            if(MouseDown()){ return true; }
            ImGui::ClearActiveID();
            return false;
        }
        return false;
    }
    bool DragStart(){
        if(GImGui->IO.MouseClicked[0]){ return true; }
        return false;
    }
    void UIDrawRoundBB(ImRect inRect, unsigned int inC, float inRounding){
        
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32 clr = Cl( inC ); 
        drawList->AddRectFilled(inRect.Min, inRect.Max, clr, inRounding, 15);
        //drawList->AddRectFilled(p1, p2, clr, inRounding, 15);
    }
    void UIDrawBBOutline(ImRect inRect, unsigned int inC, float inThickness){
        
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32 clr = Cl( inC ); 
        drawList->AddRect(inRect.Min, inRect.Max, clr, 0, 0, inThickness);

    }
    ImU32 ClWithAlpha(unsigned int hexValue, float inAlpha){
        // inAlpha is 0 to 1
        double cr = ((hexValue >> 16) & 0xFF);  // Extract the RR byte
        double cg = ((hexValue >> 8) & 0xFF);   // Extract the GG byte
        double cb = ((hexValue) & 0xFF);        // Extract the BB byte
        //ImU32 clr = IM_COL32( cr, cg, cb, 0);

        uint alpha = (int)round(inAlpha * 255);
        ImU32 clr = IM_COL32( cr, cg, cb, alpha);

        return clr;
    }
    void UIDrawAlphaBB(ImRect inRect, unsigned int inC, float inAlpha){


        //UIDrawAlphaBB
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        //ImU32 clr = Cl( inC ); 
        ImU32 clr = ClWithAlpha(inC, inAlpha);
        
        drawList->AddRectFilled(inRect.Min, inRect.Max, clr);

    }
    void UIDrawBB(ImRect inRect, unsigned int inC){
        
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32 clr = Cl( inC ); 
        drawList->AddRectFilled(inRect.Min, inRect.Max, clr);

    }
    void UIAddScrollerH(float inX, float inY, float inW, float inH, float inContentW, float* inVal){  
        const ImGuiID id = GImGui->CurrentWindow->GetID("##scrollH");
        const ImRect trackBB = GetGlobalBB(inX, inY, inW, inH);
        float ratio = inW / inContentW;
        float barW = round(inW * ratio);
        float dragW = inW - barW;
        const ImRect dragBB = GetGlobalBB(inX, inY, dragW, inH);

        const bool overTrack = GetOverState(trackBB);
        const bool active = GetActiveState(overTrack, id);
        //int frameClr = active ? 0xFF0000 : overTrack ? 0x00FF00 : 0xFAFBFC;
        int frameClr = 0xF1F0F0;
        UIAddBorderRect( inX, inY, inW, inH, frameClr, 0xC4C4C4, 1);

        static float sliderX = inX + *inVal * dragW ;
        static ImRect barBB = GetGlobalBB(sliderX, inY, barW, inH);
        const bool overBar = GetOverState(barBB);

        static bool dragging = false;
        static float offsetW = 0.0f;
        float mouseX = GetLocalMousePos().x;

        if(overBar && DragStart()){ // Start Dragging the Bar
            dragging = true;
            offsetW = mouseX - sliderX;
        }
        else if(dragging && MouseDown()){ // Currently Dragging the Bar
            sliderX = ClipFloat( mouseX - offsetW, inX, dragW + inX);
            *inVal = (sliderX - inX) / dragW;
        }
        else if(overTrack && DragStart()){ // Click Track Outside Bar
            float xFromVal = *inVal * dragW + inX;
            float pageShift = inW * ratio;
            if(mouseX < sliderX){ sliderX = ClipFloat( xFromVal - pageShift, inX, dragW + inX); } // Page Up
            else{sliderX = ClipFloat( xFromVal + pageShift, inX, dragW + inX); } // Page Down
            *inVal = (sliderX - inX) / dragW;
        }
        else{
            dragging = false;
            sliderX = *inVal * dragW + inX;
        }
        int sliderClr = overBar || dragging ? 0xADACAC : 0xBAB7B8;
        barBB = GetGlobalBB(sliderX, inY+2, barW, inH-4);
        UIDrawRoundBB(barBB, sliderClr, 2);

    }
    void UIAddScrollerV(float inX, float inY, float inW, float inH, float inContentH, float* inVal, int* inState){   
        
        const ImGuiID id = GImGui->CurrentWindow->GetID("##scrollV");
        const ImRect trackBB = GetGlobalBB(inX, inY, inW, inH);
        float ratio = inH / inContentH;
        float barH = round(inH * ratio);
        float dragH = inH - barH;
        const ImRect dragBB = GetGlobalBB(inX, inY, inW, dragH);
        const bool overTrack = GetOverState(trackBB);
        const bool active = GetActiveState(overTrack, id);
        //int frameClr = active ? 0xFF0000 : overTrack ? 0x00FF00 : 0xFAFBFC;
        int frameClr = 0xF1F0F0;
        UIAddBorderRect( inX, inY, inW, inH, frameClr, 0xC4C4C4, 1);
        static float sliderY = inY + *inVal * dragH ;
        static ImRect barBB = GetGlobalBB(inX, sliderY, inW, barH);
        const bool overBar = GetOverState(barBB);
        static bool dragging = false;
        static float offsetH = 0.0f;
        float mouseY = GetLocalMousePos().y;
        
        if(overBar && DragStart()){ // Start Dragging the Bar
            dragging = true;
            offsetH = mouseY - sliderY;
            *inState = 2;
        }
        else if(dragging && MouseDown()){ // Currently Dragging the Bar
            sliderY = ClipFloat( mouseY - offsetH, inY, dragH + inY);
            *inVal = (sliderY - inY) / dragH;
            *inState = 1;
        }
        else if(overTrack && DragStart()){ // Click Track Outside Bar
            float yFromVal = *inVal * dragH + inY;
            float pageShift = inH * ratio;
            if(mouseY < sliderY){ sliderY = ClipFloat( yFromVal - pageShift, inY, dragH + inY); } // Page Up
            else{sliderY = ClipFloat( yFromVal + pageShift, inY, dragH + inY); } // Page Down
            *inVal = (sliderY - inY) / dragH;
        }
        else if(dragging && !MouseDown()){  // Mouse up, stop dragging
            dragging = false;
            sliderY = *inVal * dragH + inY;
            *inState = 3;
        }
        else{
            sliderY = *inVal * dragH + inY;
            *inState = 0;
        }
        barBB = GetGlobalBB(inX + 2, sliderY, inW - 4, barH);
        int sliderClr = overBar || dragging  ? 0xADACAC : 0xBAB7B8;
        UIDrawRoundBB(barBB, sliderClr, 2);

    }
    
    void UIAddSliderH(float inX, float inY, float inW, float inH, float* inVal, float inMin, float inMax){   
        
        string idStr = "##sliderH" + to_string(inX) + to_string(inY);

        const ImGuiID id = GImGui->CurrentWindow->GetID(idStr.c_str() );
        const ImRect trackBB = GetGlobalBB(inX, inY, inW, inH);
        float sliderW = 9.0f;  // circle radius
        ImRect sliderBB = GetBB(1,0,sliderW,inH-2);
        float halfSliderW = sliderW * 0.5f;
        // the area of mouse drag - dragBB is smaller than the length of the slider so the box doesn't go over the edges
        const ImRect dragBB = GetGlobalBB(inX + halfSliderW, inY, inW - sliderW, inH);
        float dragW = dragBB.GetSize().x;  
        const bool overTrack = GetOverState(trackBB);
        const bool active = GetActiveState(overTrack, id);
        int frameClr = active ? 0xFF0000 : overTrack ? 0x00FF00 : 0xFAFBFC;
        float range = inMax - inMin;
        if(active){
            float mouseX = GetMousePos().x;  // global
            //float sPerc = ClipFloat( (mouseX - dragBB.Min.x)/ dragW, 0.0f, 1.0f);
            float sPerc = ClipFloat( (mouseX - dragBB.Min.x)/ dragW, 0.0f, 1.0f);
            *inVal = range * sPerc + inMin;
        }
        float perc = (*inVal - inMin) / range;
        //float grabX = perc * dragW + dragBB.Min.x;
        float grabX = perc * dragW + dragBB.Min.x;
        ImVec2 wPos = GImGui->CurrentWindow->Pos;
        //UIAddBorderRect( inX, inY, inW, inH, frameClr, 0x000000, 1);  // visualize mouse over track
        // UIPlaceRect(grabX - wPos.x, inY + 1 , sliderBB, 1, 0, 0x000000); // visualize mouse grab box
        float midX = grabX - wPos.x;
        float midY = inY + inH * 0.5f;
        float railHeight = 1.2f;  // thickness of the rail the dot slides on
        float railY = inY + (inH - railHeight) * 0.5f;
        float frontRailWidth = perc * dragW;
        unsigned int frontColor = 0x878787;
        if(overTrack || active){frontColor = 0x2EAADC;}
        const ImRect frontRailBB = GetGlobalBB(inX, railY, frontRailWidth, railHeight);
        UIDrawBB(frontRailBB, frontColor);
        const ImRect backRailBB = GetGlobalBB(grabX- wPos.x, railY, dragW - frontRailWidth, railHeight);
        UIDrawBB(backRailBB, 0xD6D6D6);
        unsigned int dotColor = active ? 0x268BAA : overTrack ? 0x2EAADC : 0x878787;
        UIAddCircle(midX, midY, halfSliderW, dotColor, true, 4, 64);

    }
    void UIAddSliderV(float inX, float inY, float inW, float inH, float* inVal, float inMin, float inMax){   
        
        const ImGuiID id = GImGui->CurrentWindow->GetID("##v");
        const ImRect trackBB = GetGlobalBB(inX, inY, inW, inH);
        ImRect sliderBB = GetBB(1,0,inW-2,12);
        float sliderH = sliderBB.GetSize().y;
        float halfSliderH = sliderH * 0.5f;
        const ImRect dragBB = GetGlobalBB(inX, inY + halfSliderH, inW, inH - sliderH);
        float dragH = dragBB.GetSize().y;
        const bool overTrack = GetOverState(trackBB);
        const bool active = GetActiveState(overTrack, id);
        int frameClr = active ? 0xFF0000 : overTrack ? 0x00FF00 : 0xFAFBFC;
        UIAddBorderRect( inX, inY, inW, inH, frameClr, 0xE8E7E8, 1);
        float range = inMax - inMin;
        if(active){
            float mouseY = GetMousePos().y;
            float sPerc = ClipFloat( (mouseY - dragBB.Min.y)/ dragH, 0.0f, 1.0f);
            *inVal = range * sPerc + inMin;
        }
        float perc = (*inVal - inMin) / range;
        float grabY = perc * dragH + dragBB.Min.y;
        ImVec2 wPos = GImGui->CurrentWindow->Pos;
        UIPlaceRect(inX + 1, grabY - wPos.y, sliderBB, 0, 1, 0x000000);

    }

    void UIPlaceRect(float inX, float inY, ImRect inRect, int inXType, int inYType, unsigned int inC){
        
        // inXType:  0=Left, 1=Mid, 2=Right
        // inYType:  0=Top, 1=Mid, 2=Bottom
        ImVec2 rSize = inRect.GetSize();
        float xPos = inX;
        if(inXType == 1){ xPos -= rSize.x * 0.5f; }
        else if(inXType == 2){ xPos -= rSize.x; }
        float yPos = inY;
        if(inYType == 1){ yPos -= rSize.y * 0.5f; }
        else if(inYType == 2){ yPos -= rSize.y; }
        UIAddRect(xPos, yPos, rSize.x, rSize.y, inC, true, 1);

    }
    float ClipFloat(float n, float lower, float upper) {
        return std::max(lower, std::min(n, upper));
    }
    void UIAddBorderRect(float inX, float inY, float inW, float inH, unsigned int inFillC, unsigned int inBorderC, float inThickness){

        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;
        ImVec2 p1 = ImVec2(inX + wPos.x, inY + wPos.y);
        ImVec2 p2 = ImVec2(p1.x + inW, p1.y + inH);
        ImU32 fc = Cl( inFillC ); 
        ImU32 bc = Cl( inBorderC ); 
        drawList->AddRectFilled(p1, p2, fc);
        drawList->AddRect(p1, p2, bc, 0, 0, inThickness);
       
    }
    void UIAddTriangle(float inX, float inY, float inRadius, bool inOpen, unsigned int inC){
        
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;
        float dir = 0 ;
        if(inOpen){dir = 0.5f;}
        float startAngle = (float)(M_PI*2 + M_PI * dir);
        float r1 =  startAngle;
        float r2 = (float)(M_PI*2 * .333333333 + startAngle);
        float r3 = (float)(M_PI*2 * .333333333 * 2 + startAngle);
        ImVec2 p1 = ImVec2( cos(r1)*inRadius + inX + wPos.x, sin(r1)*inRadius + inY + wPos.y);
        ImVec2 p2 = ImVec2( cos(r2)*inRadius + inX + wPos.x, sin(r2)*inRadius + inY + wPos.y);
        ImVec2 p3 = ImVec2( cos(r3)*inRadius + inX + wPos.x, sin(r3)*inRadius + inY + wPos.y);
        ImU32 clr = UILib::Cl( inC );
        drawList->AddTriangleFilled(p1, p2, p3, clr);
    }
    void UIAddFilledPolyline(vector<vec2> inPnts, unsigned int inC ){
        
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;

        size_t n = inPnts.size();
        vector<ImVec2> pList;
        for(int i=0;i<n;i++){
            pList.push_back({inPnts[i].x + wPos.x, inPnts[i].y  + wPos.y});
        }    

        ImVec2* pListArr = &pList[0];
        ImU32 clr = UILib::Cl( inC );

        drawList->AddConvexPolyFilled(pListArr, (int) n, clr);

    }
    void UIAddPolyline(vector<vec2> inPnts, unsigned int inC, float inThickness, bool inClosed ){
        
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;

        size_t n = inPnts.size();
        vector<ImVec2> pList;
        for(int i=0;i<n;i++){
            pList.push_back({inPnts[i].x + wPos.x, inPnts[i].y  + wPos.y});
        }    

        ImVec2* pListArr = &pList[0];
        ImU32 clr = UILib::Cl( inC );

        drawList->AddPolyline(pListArr, (int) n, clr, inClosed, inThickness);

    }
    void UIAddRoundCornerRect(float inX, float inY, float inW, float inH, unsigned int inC, bool inFill, float inThickness, float inRounding, std::string cornerBits){
        
        std::bitset<4> bDef(cornerBits);
        unsigned int cornerFlags = bDef.to_ulong();
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;
        ImU32 clr = UILib::Cl( inC );  
        ImVec2 p1 = ImVec2(inX + wPos.x, inY + wPos.y);
        ImVec2 p2 = ImVec2(p1.x + inW, p1.y + inH);
        if(inFill){
            drawList->AddRectFilled(p1, p2, clr, inRounding, cornerFlags);
        }
        else{
            drawList->AddRect(p1, p2, clr, inRounding, cornerFlags, inThickness);
        }
        

    }
    void UIAddCircle(float inX, float inY, float inRadius, unsigned int inC, bool inFill, float inThickness, int nbSegments){
        
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;
        ImVec2 center = ImVec2(inX + wPos.x, inY + wPos.y);
        ImU32 clr = UILib::Cl( inC );  
        
        if(inFill){
           drawList-> AddCircleFilled(center, inRadius, clr, nbSegments);
        }
        else{
            drawList->AddCircle(center, inRadius, clr, nbSegments, inThickness);
        }
        

    }
    void UIAddRect(float inX, float inY, float inW, float inH, unsigned int inC, bool inFill, float inThickness){

        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;


        ImU32 clr = UILib::Cl( inC );  
        ImVec2 p1 = ImVec2(inX + wPos.x, inY + wPos.y);
        ImVec2 p2 = ImVec2(p1.x + inW, p1.y + inH);
        if(inFill){
            drawList->AddRectFilled(p1, p2, clr);
        }
        else{
            drawList->AddRect(p1, p2, clr, 0, 0, inThickness);
        }
    }
    void UIAddRoundRect(float inX, float inY, float inW, float inH, unsigned int inC, bool inFill, float inThickness, float inRounding){

        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;
        ImU32 clr = UILib::Cl( inC );  
        ImVec2 p1 = ImVec2(inX + wPos.x, inY + wPos.y);
        ImVec2 p2 = ImVec2(p1.x + inW, p1.y + inH);
        if(inFill){
            drawList->AddRectFilled(p1, p2, clr, inRounding, 15);
        }
        else{
            drawList->AddRect(p1, p2, clr, inRounding, 15, inThickness);
        }

    }
    void UIAddTextWithFont(float inX, float inY, float inW, float inH, string inText, int inC, int inFontIndex, vec2 inAlign){

        ImGuiContext& g = *GImGui;
        PushFont(g.IO.Fonts->Fonts[inFontIndex]);
        UIAddTextString(inX, inY, inW, inH, inText, inC, inAlign);
        PopFont();
        
    }
    void UIAddTextString(float inX, float inY, float inW, float inH, string inText, int inC, vec2 inAlign){
        
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImVec2 wPos = window->Pos;
        ImGuiDataType data_type = ImGuiDataType_Float;
        const char* format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
        const char* text_buf = inText.c_str();
        size_t text_size = inText.length();
        const char* value_text_begin = &g.TempBuffer[0];
        ImVec2 min = ImVec2(inX + wPos.x, inY + wPos.y);
        ImVec2 max = ImVec2(min.x + inW, min.y + inH);

        ImU32 Tclr = UILib::Cl( inC );
        PushStyleColor(ImGuiCol_Text, Tclr);

        //ImVec2(0.5f,0.5f)
        ImGui::RenderTextClipped(min, max, text_buf, text_buf + text_size, NULL, ImVec2(inAlign[0], inAlign[1]));
        PopStyleColor(1);

    }
    void UIAddTextFloat(float inX, float inY, float inW, float inH, float *inVal, vec2 inAlign){
        
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImVec2 wPos = window->Pos;
        ImGuiDataType data_type = ImGuiDataType_Float;
        const char* format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
        char value_buf[5];
        const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, inVal, format);
        ImVec2 min = ImVec2(inX + wPos.x, inY + wPos.y);
        ImVec2 max = ImVec2(min.x + inW, min.y + inH);
        ImGui::RenderTextClipped(min, max, value_buf, value_buf_end, NULL, ImVec2(inAlign[0], inAlign[1]));
    }
    void UIAddArrowLine(float inX1, float inY1, float inX2, float inY2, unsigned int inC, float inThickness, float inW_Arrow, float inH_Arrow){
         
        UIAddLine(inX1, inY1, inX2, inY2, inC, inThickness);
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;
        vec2 aVec = vec2(inX2 - inX1, inY2 - inY1);
        vec2 nVec = normalize( aVec );
        vec2 v1 = glm::rotate( nVec, float(0.5f * M_PI) );
        vec2 v2 = glm::rotate( nVec, float(-0.5f * M_PI) );
        vec2 arrowBase = nVec * inH_Arrow * -1.0f ;
        vec2 tip = vec2(inX2  + wPos.x, inY2 + wPos.y);
        vec2 basePoint = tip + arrowBase;
        v1 = normalize(v1) * inW_Arrow * 0.5f ;
        v2 = normalize(v2) * inW_Arrow * 0.5f ;
        vec2 p1 = basePoint + v1;
        vec2 p2 = basePoint + v2;
        ImVec2 imP1 = ImVec2(p1.x, p1.y);
        ImVec2 imP2 = ImVec2(tip.x, tip.y);
        ImVec2 imP3 = ImVec2(p2.x, p2.y);
        ImU32 clr = UILib::Cl( inC );
         // Note: Anti-aliased filling requires points to be in clockwise order.
        drawList->AddTriangleFilled(imP3, imP2, imP1, clr);

    }  
    void UIAddLine(float inX1, float inY1, float inX2, float inY2, unsigned int inC1, float inThickness){
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;
        ImVec2 p1 = UILib::Pos(inX1,inY1, wPos);
        ImVec2 p2 = UILib::Pos(inX2,inY2, wPos);
        ImU32 c1 = UILib::Cl( inC1 );  
        drawList->AddLine( p1, p2, c1, inThickness);
    }
    void UIAddGradientFade(float inX, float inY, float inW, float inH, unsigned int inC, bool inVertical){
        
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;
        ImU32 c1 = UILib::Cl( inC );
        ImU32 c2 = UILib::ClearCl( inC );
        ImVec2 p1 = ImVec2(inX + wPos.x, inY + wPos.y);
        ImVec2 p2 = ImVec2(p1.x + inW, p1.y + inH);
        if(inVertical){
            drawList->AddRectFilledMultiColor(p1, p2, c1, c1, c2, c2);
        }
        else{
            drawList->AddRectFilledMultiColor(p1, p2, c1, c2, c2, c1);
        }
    }
    void UIAddGradientRect(float inX, float inY, float inW, float inH, unsigned int inC1, unsigned int inC2, bool inVertical){
        
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 wPos = window->Pos;
        ImU32 c1 = UILib::Cl( inC1 );  
        ImU32 c2 = UILib::Cl( inC2 );
        ImVec2 p1 = ImVec2(inX + wPos.x, inY + wPos.y);
        ImVec2 p2 = ImVec2(p1.x + inW, p1.y + inH);
        if(inVertical){
            drawList->AddRectFilledMultiColor(p1, p2, c1, c1, c2, c2);
        }
        else{
            drawList->AddRectFilledMultiColor(p1, p2, c1, c2, c2, c1);
        }
    }


} // end namespace UILib


#endif //  UILIB_CPP