#ifndef TUI_EXAMPLE_LIBRARYUI_H
#define TUI_EXAMPLE_LIBRARYUI_H

#include "TUI_PaneBase.h"

namespace AppCore {

    // ************************************************************ //
    // ExampleLibraryUI Class                                               //
    // ************************************************************ //
    class ExampleLibraryUI : virtual public TUI_PaneBase {
    
    public:

        ExampleLibraryUI( PipelineManager &parent );
        ~ExampleLibraryUI();
        
        
        void                InitDraw();
        void                DrawPane() override final;

    protected:

        // void             StartPane() override final;
        // void             FinishPane() override final;
        void                DrawUI();

    };
    
} // end namespace AppCore

#endif // TUI_EXAMPLE_LIBRARYUI_H