#ifndef SUI_UNDERLAYUI_CPP
#define SUI_UNDERLAYUI_CPP

#include "c_PipelineManager.h"
#include <nfd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace glm;

namespace AppCore {

    UnderlayUI::UnderlayUI( PipelineManager &parent ) :
        SUI_PipelineBase( parent ) {
        Parameters.shader_vert_path = "Shaders/SUI_Underlay_UI.vert.spv";
        Parameters.shader_frag_path = "Shaders/SUI_Underlay_UI.frag.spv";
        TexturePaths = {
            {"Data/Example_ShaderUI_01.png"}
        };
    }

    UnderlayUI::~UnderlayUI() {
    }

    void UnderlayUI::SubmitComplete(){

        ProcessClicks();

    }
    
    void UnderlayUI::OpenFileDialogue(){
        
        Parent.Pause = true; // need to stop Vulkan drawing while opening a File Dialogue
                        
        nfdchar_t *outPath = NULL;
        nfdresult_t result = NFD_OpenDialog( NULL, NULL, &outPath );
        // Other things you can call with this Native File dialogue:
        // NFD_PickFolder, NFD_OpenDialogMultiple, NFD_SaveDialog
        
        if ( result == NFD_OKAY ) {
            puts("Success!");
            puts(outPath);
            free(outPath);
            Parent.Pause = false;
        }
        else if ( result == NFD_CANCEL ) {
            puts("User pressed cancel.");
                Parent.Pause = false;
        }
        else {
            printf("Error: %s\n", NFD_GetError() );
                Parent.Pause = false;
        }
        
    }
    
    bool UnderlayUI::InsideBB(vec2 inPos, vec4 inDim){
        if(inPos.x > inDim[0] && inPos.x < inDim[2] && inPos.y > inDim[1] && inPos.y < inDim[3]){
            return true;
        }
        return false;
    }

    void UnderlayUI::ProcessClicks(){
        
        if(Parent.GetInputState().Buttons[0].Change == 1){ 
            vec2 m = vec2(Parent.GetInputState().Position.X - X, Parent.GetInputState().Position.Y - Y);
            if(InsideBB(m, B1.dim)){
                cout << "clicked button 1:\n";
            }
            else if(InsideBB(m, B2.dim)){
                cout << "clicked button 2:\n";
                OpenFileDialogue();
            }

        }

    }


} // end namespace AppCore

#endif // SUI_UNDERLAYUI_CPP