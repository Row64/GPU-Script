#ifndef TUI_DEMO_OCEAN_CPP
#define TUI_DEMO_OCEAN_CPP

#include "TUI_Demo_Ocean.h"
#include <math.h>

namespace AppCore { 

    OceanTUI::OceanTUI( AppDataIO & inAppData ) :
       TUI_PaneBase( inAppData ) {
    }
    
    OceanTUI::~OceanTUI() {
    }

    void OceanTUI::DrawPane() {
        
        StartPane();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos( ImVec2(X,Y) );
        ImGui::SetNextWindowSize( ImVec2( Width, Height ) );

        static bool initialized;
        if (!initialized) {InitDraw(); initialized = true;}
        
        ImGui::Begin( "Ocean UI", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
        
        DrawUI();
        ImGui::End();
        FinishPane();

    }

    void OceanTUI::InitDraw(){
        // add draw intialization code you only call once here
    }
    
    void OceanTUI::DrawUI(){
        
        float yVal = 50;
        float textWidth = 40;
        float SideOffset = 100.0;
        float SliderWidth = Width - SideOffset * 2;
        float SliderHeight = 12.0f;
        float SliderX = SideOffset - textWidth * 0.5f;
        UIAddSliderH(SliderX, yVal, SliderWidth, SliderHeight, &AppData.OceanSpeed, 0.0f, 2.0f);
        UIAddTextFloat( SliderX  + SliderWidth, yVal - 9, textWidth, 30, &AppData.OceanSpeed, vec2(0,0));
        UIAddTextWithFont(SliderX,15,130, 44, "Ocean Speed", 0x5FA9E0, 3, vec2(0,.5));
        
    }

} // end namespace AppCore

#endif // TUI_DEMO_OCEAN_CPP