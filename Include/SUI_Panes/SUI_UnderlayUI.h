#ifndef SUI_UNDERLAYUI_H
#define SUI_UNDERLAYUI_H

#include "c_SUI_PipelineBase.h"

namespace AppCore {

    // ************************************************************ //
    // UnderlayUI Class                                             //
    // ************************************************************ //

    

    class UnderlayUI : virtual public SUI_PipelineBase {
    
    public:

        UnderlayUI( PipelineManager &parent );
        virtual ~UnderlayUI();
        
    private:
        
        struct button{      // sprite button
            vec4 dim;       // pixel space dimension minX, minY, maxX, maxY
            vec2 t;         // texture space coordinate for default (no over/down) draw
            vec2 tOver;     // texture space coordinate for over
            vec2 tDown;     // texture space coordinate for down
        };
        
        
        button B1 = {vec4(28,8,214,64),vec2(28,8),vec2(28,78),vec2(28,149)};
        button B2 = {vec4(237,8,421,64),vec2(237,8),vec2(237,78),vec2(237,149)};


        bool            InsideBB(vec2 inPos, vec4 inDim);

        void            SubmitComplete() override final;

        void            ProcessClicks();
        void            OpenFileDialogue();

    };

} // end namespace AppCore

#endif // SUI_UNDERLAYUI_H