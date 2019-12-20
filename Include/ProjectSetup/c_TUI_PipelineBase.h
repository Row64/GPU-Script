#ifndef C_TUI_PIPELINEBASE_H
#define C_TUI_PIPELINEBASE_H

#include "c_VkPipelineBase.h"
#include "TUI_PaneBase.h"

using namespace std;

namespace AppCore {

    class PipelineManager;

    // ************************************************************ //
    // TUI_PipelineData                                             //
    // ************************************************************ //
    struct TUI_PipelineData {

        struct DrawResourcesData {
            StagingBufferParameters             VertexBuffer;
            StagingBufferParameters             IndexBuffer;

            DrawResourcesData() :
                VertexBuffer(),
                IndexBuffer() {
            }
        };

        ImGuiContext*                               Context;
        ImageParameters                             Image;
        DescriptorSetParameters                     DescriptorSet;
        vk::UniqueRenderPass                        RenderPass;
        vk::UniquePipelineLayout                    PipelineLayout;
        vk::UniquePipeline                          GraphicsPipeline;
        std::vector<DrawResourcesData>              DrawingResources;           // This needs to be set the same size as FrameResources vector
        

        TUI_PipelineData() :
            Context( ImGui::CreateContext() ),
            Image(),
            DescriptorSet(),
            RenderPass(),
            PipelineLayout(),
            GraphicsPipeline(),
            DrawingResources() {
        }
    };


    // ************************************************************ //
    // TUI_PipelineBase Class                                       //
    // ************************************************************ //
    class TUI_PipelineBase {

    public:

        TUI_PipelineBase( PipelineManager &parent );
        virtual ~TUI_PipelineBase();


        void                    Init( size_t resource_count );
        virtual void            Render( CurrentFrameData & current_frame, vk::CommandBuffer & command_buffer, vector<TUI_PaneBase*> inPaneList );

        virtual void            OnWindowSizeChanged_Pre();
        virtual void            OnWindowSizeChanged_Post();

        TUI_PipelineData &      GetParameters();

    protected:

        TUI_PipelineData        Parameters;

        PipelineManager &       Parent;
        AppDataIO &             AppData;
        
        // Init methods
        virtual void            InitPanes();
        virtual void            CreateTexture();
        virtual void            CreateDescriptorSet();
        virtual void            CreateRenderPasses();
        virtual void            CreatePipelineLayout();
        virtual void            CreateGraphicsPipeline();

        // Render methods
        virtual void            DrawAllPanes( vector<TUI_PaneBase*> inPaneList );
        virtual void            RecordCommandBuffer( CurrentFrameData & current_frame, vk::CommandBuffer & command_buffer );
        virtual void            DrawFrameData( vk::CommandBuffer & command_buffer, TUI_PipelineData::DrawResourcesData & drawing_resources ); 
        virtual void            SubmitCommandBuffer( CurrentFrameData & current_frame, vk::CommandBuffer & command_buffer );
        
    };

} // end namespace AppCore

#endif // C_TUI_PIPELINEBASE_H