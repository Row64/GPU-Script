#ifndef C_PIPELINEMANAGER_CPP
#define C_PIPELINEMANAGER_CPP


#include "c_PipelineManager.h"

namespace AppCore {

    PipelineManager::PipelineManager( std::string const & title, AppDataIO & inAppData ) :
        Title( title ),
        AppData( inAppData ),
        LM( *this ),
        CM( *this ) {
    }

    PipelineManager::~PipelineManager() {
        if( GetDevice() ) {
            GetDevice().waitIdle();
        }
    }

    std::string & PipelineManager::GetTitle() {
        return Title;
    }

    AppDataIO & PipelineManager::GetAppData() {
        return AppData;
    }

    // Prepare pipelines

    void PipelineManager::InitPipelines( WindowParameters window ) {
        FrameResources.resize( 3 );
        ComputeResources.resize( 3 );

        InitVulkan( window );
        UILib::UISetWindowHandlePointer( window.WindowPtr );
        
        // Set initial display sizes
        int w, h;
        glfwGetWindowSize(window.WindowPtr, &w, &h);
        UpdateWindowSize(w, h);
        UpdateFramebufferSize((int) GetSwapChain().Extent.width, (int) GetSwapChain().Extent.height);

        // Create frame resources and prepare texture and gui pipelines
        CreateFrameResources();

        // Init SUI-Driven and TUI-Driven Pipelines
        LM.Init( FrameResources.size() );

        // Create compute resources
        CreateComputeResources();
        CM.Init();
    }

    void PipelineManager::CreateFrameResources() {
        std::cout << "--- PipelineManager::CreateFrameResources     Creating " << FrameResources.size() << " Frame Resources" << std::endl;

        for( size_t i = 0; i < FrameResources.size(); ++i ) {
            
            FrameResources[i] = std::make_unique<FrameResourcesData>();
            auto frame_resources = FrameResources[i].get();
    
            frame_resources->ImageAvailableSemaphore = CreateSemaphore();
            frame_resources->FinishedRenderingSemaphore = CreateSemaphore();
            #ifdef VULKAN_VERSION_2
                // Only create timeline sempahore if Vulkan Version is 1.2
                frame_resources->TimelineSemaphore = CreateSemaphore( vk::SemaphoreType::eTimeline, (uint64_t) 100 );
            #endif
            frame_resources->Fence = CreateFence( true );
            frame_resources->CommandPool = CreateCommandPool( GetGraphicsQueue().FamilyIndex, vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient );
            frame_resources->CommandBufferList.resize( LM.BufferCount );
            frame_resources->CommandBufferList = std::move( AllocateCommandBuffers( *frame_resources->CommandPool, vk::CommandBufferLevel::ePrimary, LM.BufferCount ) );

        }
    }

    void PipelineManager::CreateComputeResources() {
        std::cout << "--- PipelineManager::CreateComputeResources   Creating " << ComputeResources.size() << " Frame Resources" << std::endl;

        for( size_t i = 0; i < ComputeResources.size(); ++i ) {

            ComputeResources[i] = std::make_unique<ComputeResourcesData>();
            #ifdef VULKAN_VERSION_2
                // Only create timeline sempahore if Vulkan Version is 1.2
                ComputeResources[i]->TimelineSemaphore = CreateSemaphore( vk::SemaphoreType::eTimeline, (uint64_t) 100 );
            #endif
            ComputeResources[i]->Fence = CreateFence( true );
            ComputeResources[i]->CommandPool = CreateCommandPool( GetComputeQueue().FamilyIndex, vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient );
            ComputeResources[i]->CommandBuffer = std::move( AllocateCommandBuffers( *ComputeResources[i]->CommandPool, vk::CommandBufferLevel::ePrimary, 1 )[0] );

        }
    }

    // Syncing GLFW Input with Pipeline Draw Timeline - Simplifies Immediate Mode UI
    void PipelineManager::LogKeyState(string inStr){
        auto *kState = &InputState.Key;
        string KeyVal;
        if(kState->Key == 0){KeyVal = "0";}
        else{KeyVal = (char)kState->Key;}
        string LastVal;
        if(kState->Last == 0){LastVal = "0";}
        else{LastVal = (char)kState->Last;}
        cout << inStr << "\t" << KeyVal << "\t" << LastVal << "\t" << kState->Change << "\t" << kState->IsPressed << "\n";
    }
    void PipelineManager::UpdateUIInputs(){
        
        // update mouse to sync with pipeline
        auto *b1 = &InputState.Buttons[0];
        auto *b2 = &InputState.Buttons[1];
        if(b1->IsPressed == true && b1->Last == false){InputState.Buttons[0].Change = 1;}
        else if(b1->IsPressed == false && b1->Last == true){InputState.Buttons[0].Change = 2;}
        else{InputState.Buttons[0].Change = 0;}
        if(b2->IsPressed == true && b2->Last == false){InputState.Buttons[1].Change = 1;}
        else if(b2->IsPressed == false && b2->Last == true){InputState.Buttons[1].Change = 2;}
        else{InputState.Buttons[1].Change = 0;}
        InputState.Buttons[0].Last = InputState.Buttons[0].IsPressed;
        InputState.Buttons[1].Last = InputState.Buttons[1].IsPressed;
        
        // The simplified key state is synced with the vulkan draw timeline.  It converts events into states that are easy to read when drawing.
        // It collapses/records multi-key press events into the KeyData struct so that only one key can be down at any time
        // Modifier keys work but are limited to Ctrl/Shift/Alt/Super
        // It organizes Key.Change so that every key down (Change=1) will have a matching key up (Change=2) before a new key goes down.
        // example: press "k" down, while it's down press "a" down.  Double down more like sublime:  kkkkkkkkkkkkkaaaaaaaaaaa with a "k" up added before a starts
        // If you want complex multi-key combination events then read them right off of GLFW in c_Window.cpp - Window::KeyEvent
        // Otherwise this is simple and easy to use, inspired by how ImGUI handles key states
        
        auto *kState = &InputState.Key;
        int bufSize = (int)size(kState->Buffer);

        if(bufSize == 0){
            kState->Change = 0;
            kState->Last = kState->Key;
            //LogKeyState("-\t");
        }
        else{
            vector<array<int,2>> newBuffer = {};
            if(kState->IsPressed == 0){
                // take the first key down buffered event (if you have one).  Ignore up events (already up)
                bool recordNew = false;
                for(int i=0;i<bufSize;i++){
                    if(recordNew){ newBuffer.push_back( kState->Buffer[i]);}
                    else if(kState->Buffer[i][0] == 1){
                        kState->Key = kState->Buffer[i][1];
                        kState->Last = 0;
                        kState->Change = 1;
                        kState->IsPressed = 1;
                        recordNew = true;
                        string logStr; logStr =(char) kState->Buffer[i][1];
                        //LogKeyState("DnEvent:" + logStr);
                    }
                }
                if(!recordNew){
                    kState->Last = kState->Key;
                    kState->Change = 0;
                }
                kState->Buffer = newBuffer;
            }
            else if(kState->IsPressed == 1){
                bool recordNew = false;
                for(int i=0;i<bufSize;i++){
                    if(recordNew){newBuffer.push_back( kState->Buffer[i]);}
                    else if(kState->Buffer[i][0] == 1){
                        // if you are already down, close the old key, carry over the new key to make it pressed
                        newBuffer.push_back( kState->Buffer[i]);  // carry over
                        kState->Last = kState->Key;
                        kState->Key = 0;
                        kState->Change = 2;
                        kState->IsPressed = 0;
                        recordNew = true;
                        string logStr; logStr =(char) kState->Buffer[i][1];
                        //LogKeyState("DoubleDnToUp:" + logStr);

                    }
                    else if(kState->Buffer[i][0] == 2){
                        // take the first key up buffered event (if you have one)
                        kState->Last = kState->Key;
                        kState->Key = 0;
                        kState->Change = 2;
                        kState->IsPressed = 0;
                        recordNew = true;
                        string logStr; logStr =(char) kState->Buffer[i][1];
                        //LogKeyState("UpEvent:" + logStr);
                    }
                }
                if(!recordNew){
                    kState->Last = kState->Key;
                    kState->Change = 0;
                }
                kState->Buffer = newBuffer;
            }
        }
    }

    // Drawing

    void PipelineManager::RenderPipelines() {

        CheckActiveComputes();

        if(Pause){
            return;
        }

        UpdateUIInputs();

        // Static allocation of current_frame (this happens only once)
        static CurrentFrameData current_frame = {
            0,                                                        // uint32_t                     ResourceIndex
            static_cast<uint32_t>(FrameResources.size()),             // uint32_t                     ResourceCount
            FrameResources[0].get(),                                  // FrameResourcesData          *FrameResources
            &GetSwapChain(),                                          // const SwapChainParameters   *Swapchain
            0                                                         // uint32_t                     SwapchainImageIndex
        };
        
        // Start Frame
        StartFrame( current_frame );

        // Acquire swap chain image and create frame buffer
        AcquireImage( current_frame );

        // Render Shader-Driven and UILib-Driven Pipelines
        LM.Render( current_frame );

        // Submit graphics queue and present frame
        FinishFrame( current_frame );

        
        // clear mouse change

    }

    void PipelineManager::StartFrame( CurrentFrameData & current_frame ) {
        current_frame.ResourceIndex = (current_frame.ResourceIndex + 1) % current_frame.ResourceCount;
        current_frame.FrameResources = FrameResources[current_frame.ResourceIndex].get();
        if( GetDevice().waitForFences( { *current_frame.FrameResources->Fence }, VK_FALSE, 1000000000 ) != vk::Result::eSuccess ) {
            throw std::runtime_error( "Waiting for a fence takes too long!" );
        }
        GetDevice().resetFences( { *current_frame.FrameResources->Fence } );

    }

    void PipelineManager::AcquireImage( CurrentFrameData & current_frame ) {
        // Acquire swapchain image and signal ImageAvailableSemaphore
        switch (GetDevice().acquireNextImageKHR( *current_frame.Swapchain->Handle, 3000000000, *current_frame.FrameResources->ImageAvailableSemaphore, vk::Fence(), &current_frame.SwapchainImageIndex )) {
            case vk::Result::eSuccess:
            case vk::Result::eSuboptimalKHR:
                break;
            case vk::Result::eErrorOutOfDateKHR:
                OnWindowSizeChanged();
                break;
            default:
                throw std::runtime_error( "Could not acquire swapchain image!" );
        }
    }

    void PipelineManager::FinishFrame( CurrentFrameData & current_frame ) {

        // Present frame

        vk::Result result = vk::Result::eSuccess;
        vk::PresentInfoKHR present_info(
            1,                                                            // uint32_t                     waitSemaphoreCount
            &(*current_frame.FrameResources->FinishedRenderingSemaphore), // const VkSemaphore           *pWaitSemaphores
            1,                                                            // uint32_t                     swapchainCount
            &(*current_frame.Swapchain->Handle),                          // const VkSwapchainKHR        *pSwapchains
            &current_frame.SwapchainImageIndex                            // const uint32_t              *pImageIndices
        );

        switch (GetPresentQueue().Handle.presentKHR( &present_info )) {
            case vk::Result::eSuccess:
                break;
            case vk::Result::eSuboptimalKHR:
            case vk::Result::eErrorOutOfDateKHR:
                OnWindowSizeChanged();
            break;
            default:
                throw std::runtime_error( "Could not present swapchain image!" );
        }
    }

    void PipelineManager::OnWindowSizeChanged_Pre() {
        LM.OnWindowSizeChanged_Pre();
    }

    void PipelineManager::OnWindowSizeChanged_Post() {
        LM.OnWindowSizeChanged_Post();
    }

    void PipelineManager::OnKeyPress( int key ) {
        if (key == 67 ) {
            ExecuteCompute( "Basic Compute" );                // Run compute when key "c" is pressed.
        } else if ( key == 77 ) {
            ExecuteCompute( "Mandelbrot");                    // Run mandelbrot when key "m" is pressed.
        }
    }

    // Computing

    int PipelineManager::ExecuteCompute( string inName ) {
        for( int i = 0; i < (int) ComputeResources.size(); ++i ) {
            auto iter = ACRMap.find( i );
            if ( iter == ACRMap.end() ) {
                if ( CM.Execute(inName, ComputeResources[i].get()) ) {
                    ACRMap.insert( pair<int, int>(i , CM.GetCOMIndex(inName)) );
                    std::cout << "--- PipelineManager::ComputePipelines     '" << inName << "' is using resource " << i << std::endl;
                }
                return i;
            }
        }
        std::cout << "--- PipelineManager::ComputePipelines     '" << inName << "' was not submitted because all resources are busy" << std::endl;
        return -1;      // No compute resources available
    }

    void PipelineManager::CheckActiveComputes() {
        for ( auto iter = ACRMap.cbegin(); iter != ACRMap.cend(); ) {
            if ( GetDevice().waitForFences( { *ComputeResources[iter->first]->Fence }, VK_FALSE, 1000000000 ) == vk::Result::eSuccess ) {
                CM.GetCOM(iter->second)->SubmitComplete();
                ACRMap.erase(iter++);
            } else {
                ++iter;
            }
        }

    }



} // end namespace AppCore


#endif  // C_PIPELINEMANAGER_CPP