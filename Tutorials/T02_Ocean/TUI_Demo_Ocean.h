#ifndef TUI_DEMO_OCEAN_H
#define TUI_DEMO_OCEAN_H

#include "TUI_PaneBase.h"

namespace AppCore { 
    
    // ************************************************************ //
    // OceanTUI Class                                               //
    //                                                              //
    // TUI Pane for Ocean Demo                                      //
    // ************************************************************ //
    class OceanTUI : virtual public TUI_PaneBase {
    
    public:

        OceanTUI( PipelineManager &parent );
        ~OceanTUI();
        
        void                DrawPane() override final;
        
    private:
    
        void                InitDraw();
        void                DrawUI();

    };

} // end namespace AppCore

#endif // TUI_DEMO_OCEAN_H