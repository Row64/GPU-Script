#ifndef TUI_DEMO_ICE_H
#define TUI_DEMO_TUI_DEMO_ICE_HOCEAN_H

#include "TUI_PaneBase.h"

namespace AppCore { 
    
    // ************************************************************ //
    // IceTUI Class                                                 //
    //                                                              //
    // TUI Pane for Ice Demo                                        //
    // ************************************************************ //
    class IceTUI : virtual public TUI_PaneBase {
    
    public:

        IceTUI( PipelineManager &parent );
        ~IceTUI();
        
        void                DrawPane() override final;
        bool                Draggable = false;
        float               TitleBarHeight = 10.0f;
    
    private:

        void                InitDraw();
        void                DrawUI();

    };

} // end namespace AppCore

#endif // TUI_DEMO_ICE_H