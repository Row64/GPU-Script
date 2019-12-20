#ifndef C_PIPELINEMANAGER_H
#define C_PIPELINEMANAGER_H

#include "UILib.h"
#include "c_LayoutManager.h"

namespace AppCore {


    // ************************************************************ //
    // PipelineManager Class                                        //
    // ************************************************************ //
    class PipelineManager : public VkPipelineBase {

    public:

        PipelineManager( std::string const & title, AppDataIO & inAppData );
        ~PipelineManager();
        
        bool Pause = false; 
        
        

        std::string &               GetTitle();
        AppDataIO &                 GetAppData();

        void                        InitPipelines( WindowParameters window ) final override;
        void                        RenderPipelines() final override;

        void                        UpdateUIInputs();

        void                        OnKeyPress( int key ) override final;
    
    private:

        std::string                 Title;
        AppDataIO &                 AppData;
        LayoutManager               LM;

        // Prepare methods

        void                        CreateFrameResources();

        // Drawing methods

        void                        StartFrame( CurrentFrameData & current_frame );
        void                        FinishFrame( CurrentFrameData & current_frame );

        void                        OnWindowSizeChanged_Pre() final override;
        void                        OnWindowSizeChanged_Post() final override;
        void                        LogKeyState(string inStr);
    };


} // end namespace AppCore


#endif  // C_PIPELINEMANAGER_H