#ifndef TUI_EXAMPLE_LIBRARYUI_CPP
#define TUI_EXAMPLE_LIBRARYUI_CPP

#include "TUI_Example_LibraryUI.h"

// Include\Panes\p_l_Example_LibraryUI.cpp

namespace AppCore {
    
    ExampleLibraryUI::ExampleLibraryUI( PipelineManager &parent ) :
       TUI_PaneBase( parent ) {
    }
    
    ExampleLibraryUI::~ExampleLibraryUI() {
    }

    void ExampleLibraryUI::DrawPane() {
        
        StartPane();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos( ImVec2(X,Y) );
        ImGui::SetNextWindowSize( ImVec2( Width, Height ) );
        
        ImGui::Begin( "ExampleLibraryUI", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

        static bool initialized;
        if (!initialized) {InitDraw(); initialized = true;}

        DrawUI();
        ImGui::End();
        FinishPane();
        
    }
    void ExampleLibraryUI::InitDraw(){
        // add draw intialization code you only call once here
    }

    void ExampleLibraryUI::DrawUI(){
        
        // draw rounded red button 
        // we can use either the drawing functions like UIAddRoundRect or the bounding box drawing 
        // functions like UIDrawRoundBB.  In this case use the BB functions, then reuse them for mouse detection
        
        // given local window cordinates, this will return a global cord bounding box
        ImRect buttonBB = GetGlobalBB(35,30,172, 44);
        bool overButton = GetOverState(buttonBB);  // check if mouse is over the button

        // uses immediate mode UI - basically a big if tree that checks per-frame state instead of events
        if(overButton){
            UIDrawRoundBB(buttonBB, 0xC82E53, 8);
            if(MouseDown()){ // mouse down will be true for only one frame on start of down
                cout << "click START PROCESS" << "\n";
            }
        }
        else{
            UIDrawRoundBB(buttonBB, 0xBC002D, 8);
        }

        // add text on our button, specifying the second (larger size) Arial font imported
        UIAddTextWithFont(34,30,172, 44, "START PROCESS", 0xFFFFFF, 2, vec2(.5,.5));

        // add diagram line with arrow and text
        UIAddArrowLine(375, 52, 235, 52, 0x000000, 1.5, 12, 18);
        UIAddTextWithFont(330,30,400, 44, "Button made in Triangle Draw Libray", 0x000000, 2, vec2(.5,.5));

        

    }



} // end namespace AppCore

#endif // TUI_EXAMPLE_LIBRARYUI_CPP