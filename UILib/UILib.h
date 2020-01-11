#ifndef  UILIB_H
#define  UILIB_H

#if defined( _WIN32 )
    //#define NOMINMAX
    //#define WIN32_LEAN_AND_MEAN     // http://forums.codeguru.com/showthread.php?560037-RESOLVED-VS-2017-and-Windows-h-issue
    #include <windows.h>
    #include <stdexcept>
    #include <algorithm>
#endif

#include <cmath>
#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <array>
#include <map>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
//#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/rotate_vector.hpp>


using namespace std;
using namespace glm;

#include "imgui/imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
    #define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui/imgui_internal.h"
using namespace ImGui;

namespace UILib {
    
    // --------------------------------------------------------------------- //
    // Data Structures                                                       //
    // --------------------------------------------------------------------- //

    struct InputStateData {
        
        struct ButtonData {
            bool IsPressed;
            bool Last;
            int Change;
        } Buttons[2];   
        
        struct KeyData {  // simplified keyboard state
            int Key = 0;
            int Last = 0;
            int Change = 0;
            bool IsPressed = 0;
            vector<array<int,2>> Buffer;
        }Key;
        
        struct PositionData {
            double X;
            double Y;
            struct PositionDeltaData{
                double X;
                double Y;
            } Delta;
        } Position;
        
        float ScrollY = 0;
        bool Shift = false;
        bool Alt = false;
        bool Ctrl = false;
        bool Supr = false; // Super key (windows key or command key on Mac)

        // Constructor
        InputStateData() {
            Buttons[0].IsPressed = false;
            Buttons[1].IsPressed = false;
            Buttons[0].Last = false;
            Buttons[1].Last = false;
            Buttons[0].Change = 0;
            Buttons[1].Change = 0;
            Position.X = 0;
            Position.Y = 0;
            Position.Delta.X = 0;
            Position.Delta.Y = 0;
        }
 
    };
    

    struct DisplayStateData {

        struct WindowSizeData {
            int W;
            int H;
        } WindowSize;

        
        struct FramebufferSizeData {
            int W;
            int H;
        } FramebufferSize;

        // Retina Diplays have different window and frame buffer resoution. Need to apply scale.
        // io.DisplaySize is the same as GLFW Framebuffer size
        struct ScaleFactorData {
            float X;
            float Y;
        } ScaleFactor;

        DisplayStateData() {
            WindowSize.W = 0;
            WindowSize.H = 0;
            FramebufferSize.W = 0;
            FramebufferSize.H = 0;
            ScaleFactor.X = 1.0f;
            ScaleFactor.Y = 1.0f;
        }

    };

    // --------------------------------------------------------------------- //
    // Extern pointers                                                       //
    // --------------------------------------------------------------------- //

    #ifndef UILibIS
    extern InputStateData* UILibIS;         // Current implicit InputStateData pointer
    #endif

    #ifndef UILibDS
        extern DisplayStateData* UILibDS;   // Current implicit DisplayState pointer
    #endif

    #ifndef UILibWindowHandle
        extern void* UILibWindowHandle;     // Current window handle pointer
    #endif


    // --------------------------------------------------------------------- //
    // Declarations                                                          //
    // --------------------------------------------------------------------- //
    



    // UILib core event functions
    void                    UISetInputStatePointer( InputStateData* inMS );
    InputStateData*         UIGetInputState();
    
    
    void                    UISetDisplayStatePointer( DisplayStateData* inDS );
    DisplayStateData*       UIGetDisplayState();
    vec2                    UIGetWindowSize();
    vec2                    UIGetFramebufSize();

    void                    UISetWindowHandlePointer( void* inWin );
    void*                   UIGetWindowHandle();
    

    // Core Drawing Functions
    void                    UIAddTriangle(float inX, float inY, float inRadius, bool inOpen, unsigned int inC);
    void                    UIAddFilledPolyline(vector<vec2> inPnts, unsigned int inC );
    void                    UIAddPolyline(vector<vec2> inPnts, unsigned int inC, float inThickness, bool inClosed );
    void                    UIAddRoundCornerRect(float inX, float inY, float inW, float inH, unsigned int inC, bool inFill, float inThickness, float inRounding, std::string cornerBits);
    void                    UIAddCircle(float inX, float inY, float inRadius, unsigned int inC, bool inFill, float inThickness, int nbSegments);
    void                    UIAddRect(float inX, float inY, float inW, float inH, unsigned int inC, bool inFill, float inThickness);
    void                    UIAddRoundRect(float inX, float inY, float inW, float inH, unsigned int inC, bool inFill, float inThickness, float inRounding);
    void                    UIAddLine(float inX1, float inY1, float inX2, float inY2, unsigned int inC1, float inThickness);
    void                    UIAddGradientRect(float inX, float inY, float inW, float inH, unsigned int inC1, unsigned int inC2, bool inVertical);
    void                    UIAddGradientFade(float inX, float inY, float inW, float inH, unsigned int inC, bool inVertical);
    void                    UIAddBorderRect(float inX, float inY, float inW, float inH, unsigned int inFillC, unsigned int inBorderC, float inThickness);
    void                    UIPlaceRect(float inX, float inY, ImRect inRect, int inXType, int inYType, unsigned int inC);
    void                    UIDrawBB(ImRect inRect, unsigned int inC);
    void                    UIDrawAlphaBB(ImRect inRect, unsigned int inC, float inAlpha);
    void                    UIDrawBBOutline(ImRect inRect, unsigned int inC, float inThickness);
    void                    UIDrawRoundBB(ImRect inRect, unsigned int inC, float inRounding);
    void                    UIAddTextFloat(float inX, float inY, float inW, float inH, float *inVal, vec2 inAlign);
    void                    UIAddTextString(float inX, float inY, float inW, float inH, string inText, int inC, vec2 inAlign);
    void                    UIAddTextWithFont(float inX, float inY, float inW, float inH, string inText, int inC, int inFontIndex, vec2 inAlign);
    void                    UIAddArrowLine(float inX1, float inY1, float inX2, float inY2, unsigned int inC1, float inThickness, float inW_Arrow, float inH_Arrow);


    // Dynamic Widgets - Change Base On Event State
    void                    UIAddSliderH(float inX, float inY, float inW, float inH, float* inVal, float inMin, float inMax);
    void                    UIAddSliderV(float inX, float inY, float inW, float inH, float* inVal, float inMin, float inMax);
    void                    UIAddScrollerV(float inX, float inY, float inW, float inH, float inContentH, float* inVal, int* inState);
    void                    UIAddScrollerH(float inX, float inY, float inW, float inH, float inContentW, float* inVal);
    void                    UIAddTabs(float inX, float inY, float inHeight, vector<string> inStrings, int* inIndex, float inPad, int inFontIndex);

    // Event State
    bool                    GetOverState(ImRect inBB);
    bool                    GetActiveState(bool inOver, ImGuiID inId);
    bool                    MouseDown();
    bool                    DragStart();
    vec2                    GetMousePos();
    vec2                    GetLocalMousePos();
    int                     GetKeyDown();
    float                   GetMouseScroll();

    
    // Helper Functions
    ImU32                   Cl(unsigned int hexValue);
    ImU32                   ClearCl(unsigned int hexValue);
    ImU32                   ClWithAlpha(unsigned int hexValue, float inAlpha);

    ImVec2                  Pos(float x, float y, ImVec2 winPos);
    void                    LogBB(ImRect inBB);
    void                    LogPos(string inTxt, vec2 inPos);
    float                   ClipFloat(float n, float lower, float upper);
    ImRect                  GetGlobalBB(float inX, float inY, float inW, float inH);
    ImRect                  GetCenteredBB(float inX, float inY, float inW, float inH);
    float                   YToGlobal(float inY);
    ImRect                  GetBB(float inX, float inY, float inW, float inH);

} // end namespace ImGui

#endif //  UILIB_H