#ifndef TUI_DEMO_ICE_CPP
#define TUI_DEMO_ICE_CPP

#include "TUI_Demo_Ice.h"

namespace AppCore { 

    IceTUI::IceTUI( AppDataIO & inAppData ) :
       TUI_PaneBase( inAppData ) {
    }
    
    IceTUI::~IceTUI() {
    }

    void IceTUI::DrawPane() {
        
        StartPane();
        ImGuiIO& io = ImGui::GetIO();

        static bool initialized;
        if (!initialized) {InitDraw(); initialized = true;}

        if (Mode.find('D') != std::string::npos){                   // draggable
            Draggable = true;
            ImGui::SetNextWindowPos( ImVec2(X,Y), ImGuiCond_Once );
            ImGui::SetNextWindowSize( ImVec2( Width, Height ), ImGuiCond_Once );
            ImGui::Begin( "Ice Sliders");
            ImGuiContext& g = *GImGui;
            Width = g.CurrentWindow->Size.x;
            Height = g.CurrentWindow->Size.y;
        }
        else{
            Draggable = false;
            ImGui::SetNextWindowPos( ImVec2(X,Y) );                 // not draggable
            ImGui::SetNextWindowSize( ImVec2( Width, Height ) );
            ImGui::Begin( "Ice Sliders", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize );
        }
        
        DrawUI();
        
        ImGui::End();
        FinishPane();

    }

    void IceTUI::InitDraw(){
        // add draw intialization code you only call once here
    }
    
    void IceTUI::DrawUI(){    
        
        float WinHeight = Height;
        if(Draggable){WinHeight = Height - TitleBarHeight;}

        float ySpacer = 9.0f;
        float xSpacer = 8.0f;
        float labelWidth = 55.0f;
        float valWidth = 40.0f;
        float sliderHeight = 14.0f;

        float yStep = (WinHeight - ySpacer * 2)/ 4.0f;
        string names[4] = { "roughness", "expansion", "color", "refraction" }; 
        float yPos = ySpacer + sliderHeight * 0.5f;
        if(Draggable){ yPos += TitleBarHeight;}

        float * values[4] = { &AppData.Roughness, &AppData.IsoValue, &AppData.IceColor, &AppData.Refraction };
        float sliderX = xSpacer + labelWidth + 6;
        float sliderWidth = Width - xSpacer * 2 - valWidth - labelWidth;
        float valX = sliderX + sliderWidth + 5;
         
        for(int i =0;i<4;i++){
            UIAddTextString( xSpacer, yPos - 2, labelWidth, sliderHeight, names[i], 0x424242, vec2(0,0));
            UIAddSliderH(sliderX, yPos, sliderWidth, sliderHeight, values[i], 0.0f, 1.0f);
            UIAddTextFloat( valX, yPos - 2, 40, sliderHeight, values[i], vec2(0,0));
            yPos += yStep;

        }

    }

} // end namespace AppCore

#endif // TUI_DEMO_ICE_CPP