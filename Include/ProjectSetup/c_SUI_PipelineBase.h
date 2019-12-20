#ifndef C_SUI_PIPELINEBASE_H
#define C_SUI_PIPELINEBASE_H

#include "c_VkPipelineBase.h"

using namespace std;


namespace AppCore {

    class PipelineManager;

    // ************************************************************ //
    // VertexData                                                   //
    //                                                              //
    // Struct describing data type and format of vertex attributes  //
    // ************************************************************ //
    struct VertexData {

        glm::vec3   Position;             // x, y, z, w
        glm::vec3   Color;                // r, g, b
        glm::vec2   Texcoords;            // u, v
    };

    // ************************************************************ //
    // UniformBufferData                                            //
    //                                                              //
    // Struct describing uniform buffer data passed to shader       //
    // ************************************************************ //
    struct UniformBufferData {
        alignas(4) float uTime;
        alignas(4) float mouseX;
        alignas(4) float mouseY;
        alignas(4) float mDown;
        alignas(16) glm::vec4 color;
        alignas(16) glm::vec4 dim;
    };
    
    // ************************************************************ //
    // TextureMetaData                                              //
    //                                                              //
    // Struct describing texture metadata                           //
    // ************************************************************ //
    struct TextureMetaData {
        std::string                 path;
        vk::SamplerMipmapMode       mipmap_mode = vk::SamplerMipmapMode::eNearest;              // options: eNearest, eLinear
        vk::SamplerAddressMode      address_mode = vk::SamplerAddressMode::eClampToEdge;        // options: eRepeat, eMirroredRepeat, eClampToEdge, eClampToBorder, eMirrorClampToEdge, eMirrorClampToEdgeKHR
    };

    // ************************************************************ //
    // SUI_PipelineData                                             //
    // ************************************************************ //
    struct SUI_PipelineData {

        struct DrawResourcesData {

            BufferParameters                    UniformBuffer;

            DrawResourcesData() :
                UniformBuffer() {
            }

        };

        std::string                                 shader_vert_path = "Shaders/shader.vert.spv";
        std::string                                 shader_frag_path = "Shaders/shader.frag.spv";

        std::vector<ImageParameters>                Texture;
        BufferParameters                            VertexBuffer;
        BufferParameters                            IndexBuffer;
        DescriptorSetParameters                     DescriptorSet;
        vk::UniqueRenderPass                        RenderPass;
        vk::UniquePipelineLayout                    PipelineLayout;
        vk::UniquePipeline                          GraphicsPipeline;
        std::vector<DrawResourcesData>              DrawingResources;           // This needs to be set the same size as FrameResources vector
        

        SUI_PipelineData() :
            Texture(),
            VertexBuffer(),
            IndexBuffer(),
            DescriptorSet(),
            RenderPass(),
            PipelineLayout(),
            GraphicsPipeline(),
            DrawingResources() {
        }
    };


    // ************************************************************ //
    // SUI_PipelineBase Class                                            //
    // ************************************************************ //
    class SUI_PipelineBase {

    public:

        SUI_PipelineBase( PipelineManager &parent );
        virtual ~SUI_PipelineBase();

        void                    Init( size_t resource_count );
        virtual void            Render( CurrentFrameData & current_frame, vk::CommandBuffer & command_buffer );

        virtual void            OnWindowSizeChanged_Pre();
        virtual void            OnWindowSizeChanged_Post();

        SUI_PipelineData &      GetParameters();

        void                    UpdateDimensions( float inX, float inY, float inW, float inH);
        void                    UpdateMode( string inMode );

    protected:

        SUI_PipelineData        Parameters;

        PipelineManager &       Parent;
        float                   X = 0.0f;
        float                   Y = 0.0f;
        float                   Width = 0.0f;
        float                   Height = 0.0f;
        string                  Mode = "";
        AppDataIO &             AppData;
        
        vector<TextureMetaData> TexturePaths;
        vector<float*>          PushConstants;

        // Init methods
        virtual void            CreateTexture();
        virtual void            CreateVertexBuffer();
        virtual void            CreateIndexBuffer();
        virtual void            CreateDescriptorSet();
        virtual void            CreateRenderPasses();
        virtual void            CreatePipelineLayout();
        virtual void            CreateGraphicsPipeline();
        virtual void            SubmitComplete();
        
        // Render methods
        virtual void            AcquireImage( CurrentFrameData & current_frame );
        virtual void            UpdateUniformBuffer( SUI_PipelineData::DrawResourcesData & drawing_resources );
        virtual void            RecordCommandBuffer( CurrentFrameData & current_frame, vk::CommandBuffer & command_buffer );
        virtual void            SubmitCommandBuffer( CurrentFrameData & current_frame, vk::CommandBuffer & command_buffer );

        virtual vk::Rect2D      SetScissor();
        
    };

} // end namespace AppCore

#endif // C_SUI_PIPELINEBASE_H