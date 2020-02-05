#ifndef C_LAYOUTMANAGER_H
#define C_LAYOUTMANAGER_H

#include "UILib.h"
#include "user_Includes.h"
#include "c_PaneTreeManager.h"

using namespace std;

namespace AppCore {

    // ************************************************************ //
    // LayoutManager Class                                          //
    //                                                              //
    // Holds all Layouts used                                       //
    // ************************************************************ //
    class LayoutManager {

    public:

        LayoutManager( PipelineManager &parent );
        ~LayoutManager();

        void                    Init( size_t resource_count );
        void                    ValidateLayouts();
        void                    SetCurrentLayout( string inName );
        void                    SetCurrentLayout( int inIndex );
        void                    UpdateCurrentLayout();

        int                     BufferCount = 3;    // Set max number of command buffers needed.
        void                    InitStyles();
        void                    InitLayouts();      // Add your layouts here
        
        void                    Render( CurrentFrameData & current_frame );
        void                    OnWindowSizeChanged_Pre();
        void                    OnWindowSizeChanged_Post();

    private:

        PipelineManager &           Parent;
        AppDataIO &                 AppData;

        string                      CurrentLayoutName;
        PaneTree                    *CurrentLayout;
        vector<SUI_PipelineBase*>   CurrentSUI;
        vector<TUI_PaneBase*>       CurrentTUI;

        PaneTreeManager         PTM;

    };

} // end namespace AppCore

#endif // C_LAYOUTMANAGER_H