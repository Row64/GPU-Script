#ifndef C_LAYOUTMANAGER_CPP
#define C_LAYOUTMANAGER_CPP

#include "c_PipelineManager.h"

namespace AppCore {

    // ---------- LayoutManager ----------

    LayoutManager::LayoutManager( PipelineManager &parent ) :
        Parent( parent ),
        AppData( parent.GetAppData() ),
        PTM( parent ) {
    }

    LayoutManager::~LayoutManager() {
    }

    void LayoutManager::Init( size_t resource_count ) {
        InitStyles();
        InitLayouts();
        ValidateLayouts();
        PTM.InitPipelines( resource_count );
        UpdateCurrentLayout();
    }

    void LayoutManager::ValidateLayouts() {

        if ( BufferCount < 2 ) {
            std::cout << "Assert: BufferCount must be greater than 1. Please set it in c_LayoutManager.h." << std::endl;
            assert( BufferCount > 1);
        }

        for ( auto & pt : PTM.GetAllPaneTrees() ) {
            int SUICount = (int) PTM.GetSUIVector( pt ).size();
            if ( SUICount < 1 ) {
                std::cout << "Assert: Error in " << pt->Name << " layout. You must have at least 1 SUI pane." << std::endl;
                assert( SUICount >= 1 );
            }
            if ( SUICount + 1 > BufferCount ) {
                std::cout<< "Assert: Error in " << pt->Name << " layout. Not enough command buffers allocated for rendering." << std::endl;
                assert( SUICount + 1 <= BufferCount );
            }
        }
    }

    void LayoutManager::SetCurrentLayout( string inName ) {
        CurrentLayoutName = inName;
        CurrentLayout = PTM.GetPaneTree( inName );
        CurrentSUI = PTM.GetSUIVector( CurrentLayout );
        CurrentTUI = PTM.GetTUIVector( CurrentLayout );
        Parent.Poll = CurrentLayout->Poll;
    }

    void LayoutManager::SetCurrentLayout( int inIndex ) {
        CurrentLayout = PTM.GetPaneTree( inIndex );
        CurrentLayoutName = CurrentLayout->Name;
        CurrentSUI = PTM.GetSUIVector( CurrentLayout );
        CurrentTUI = PTM.GetTUIVector( CurrentLayout );
        Parent.Poll = CurrentLayout->Poll;
    }
    void LayoutManager::UpdateCurrentLayout() {
        array<int, 2> window_size = { (int) UIGetWindowSize().x, (int) UIGetWindowSize().y};
        PTM.GetDimensions( CurrentLayout , &window_size );
        PTM.UpdateDimensions( CurrentLayout );
        PTM.UpdateModes(  CurrentLayout );
        
        // Update Render Pass Settings for SUI
        // No need to update TUI render pass setting, because TUI is always at the end
        int SUICount = (int) CurrentSUI.size();
        for ( int i = 0; i < SUICount; ++i ) {
            CurrentSUI[i]->ReInit( i );
        }
    }

    void LayoutManager::Render( CurrentFrameData & current_frame ) {
        
        // SUI Pipelines
        int SUICount = (int) CurrentSUI.size();
        for ( int i = 0; i < SUICount; ++i ) {
            CurrentSUI[i]->Render( current_frame, *current_frame.FrameResources->CommandBufferList[i], i );
        }

        // TUI pipeline
        PTM.GetTUIPipeline().Render( current_frame, *current_frame.FrameResources->CommandBufferList[SUICount], CurrentTUI );
    }

    void LayoutManager::OnWindowSizeChanged_Pre() {

        UpdateCurrentLayout();

        // SUI pipelines
        int SUICount = (int) CurrentSUI.size();
        for ( int i = 0; i < SUICount; ++i ) {
            CurrentSUI[i]->OnWindowSizeChanged_Pre();
        }

        // TUI pipeline
        PTM.GetTUIPipeline().OnWindowSizeChanged_Pre();

    }

    void LayoutManager::OnWindowSizeChanged_Post() {

        // SUI pipelines
        int SUICount = (int) CurrentSUI.size();
        CurrentSUI[0]->OnWindowSizeChanged_Post();

        // TUI pipeline
        PTM.GetTUIPipeline().OnWindowSizeChanged_Post();   

    }


} // end namespace AppCore

#endif // C_LAYOUTMANAGER_CPP