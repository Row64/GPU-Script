#ifndef C_SUI_PIPELINEBASE_CPP
#define C_SUI_PIPELINEBASE_CPP

#include "c_PipelineManager.h"

namespace AppCore {
    
    SUI_PipelineBase::SUI_PipelineBase( PipelineManager &parent ) :
        Parent( parent ),
        AppData( parent.GetAppData() ),
        TexturePaths(),
        PushConstants() {
    }

    SUI_PipelineBase::~SUI_PipelineBase() {
    }

    void SUI_PipelineBase::Init( size_t resource_count) {

        OnWindowSizeChanged_Post();
        Parameters.DrawingResources.resize( resource_count );

        CreateTexture();
        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateDescriptorSet();
        CreateRenderPasses();
        CreatePipelineLayout();
        CreateGraphicsPipeline();
        
        
    }
    void SUI_PipelineBase::Render( CurrentFrameData & current_frame, vk::CommandBuffer & command_buffer ) {

        // Acquire swap chain image and create frame buffer
        AcquireImage( current_frame );

        UpdateUniformBuffer( Parameters.DrawingResources[current_frame.ResourceIndex] );

        // Record command buffers
        RecordCommandBuffer( current_frame, command_buffer );
        SubmitCommandBuffer( current_frame, command_buffer );

        SubmitComplete();
        
    }
    void SUI_PipelineBase::OnWindowSizeChanged_Pre() {
    }

    void SUI_PipelineBase::OnWindowSizeChanged_Post() {
        
        // Create depth attachment and transition it away from an undefined layout
        vk::ImageSubresourceRange image_subresource_range(
            vk::ImageAspectFlagBits::eDepth,                                // VkImageAspectFlags               aspectMask
            0,                                                              // uint32_t                         baseMipLevel
            1,                                                              // uint32_t                         levelCount
            0,                                                              // uint32_t                         baseArrayLayer
            1                                                               // uint32_t                         layerCount
        );
        for( size_t i = 0; i < Parent.FrameResources.size(); ++i ) {
            Parent.FrameResources[i].get()->DepthAttachment = Parent.CreateImage( Parent.GetSwapChain().Extent.width, Parent.GetSwapChain().Extent.height, Parent.DefaultDepthFormat, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, vk::ImageAspectFlagBits::eDepth );
            Parent.SetImageMemoryBarrier( *Parent.FrameResources[i].get()->DepthAttachment.Handle, image_subresource_range, vk::ImageLayout::eUndefined, vk::AccessFlags( 0 ), vk::PipelineStageFlagBits::eTopOfPipe, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::PipelineStageFlagBits::eEarlyFragmentTests );
        }

        // Pre-transition all swapchain images away from an undefined layout
        {
            vk::ImageSubresourceRange image_subresource_range(
                vk::ImageAspectFlagBits::eColor,                                // VkImageAspectFlags               aspectMask
                0,                                                              // uint32_t                         baseMipLevel
                1,                                                              // uint32_t                         levelCount
                0,                                                              // uint32_t                         baseArrayLayer
                1                                                               // uint32_t                         layerCount
            );
            for( auto & swapchain_image : Parent.GetSwapChain().Images ) {
                Parent.SetImageMemoryBarrier( swapchain_image, image_subresource_range, vk::ImageLayout::eUndefined, vk::AccessFlags( 0 ), vk::PipelineStageFlagBits::eTopOfPipe, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits::eMemoryRead, vk::PipelineStageFlagBits::eBottomOfPipe );
            }
        }
    }

    SUI_PipelineData & SUI_PipelineBase::GetParameters() {
        return Parameters;
    }

    void SUI_PipelineBase::UpdateDimensions( float inX, float inY, float inW, float inH ) {
        X = inX;
        Y = inY;
        Width = inW;
        Height = inH;
    }

    void SUI_PipelineBase::UpdateMode( string inMode ) {
        Mode = inMode;
    }

    // ------------ Protected Methods ------------

    // Init methods

    void SUI_PipelineBase::CreateTexture() {

        int texture_count = (int) TexturePaths.size();

        Parameters.Texture.resize( texture_count );

        for ( int i = 0; i < texture_count; ++i) {

             // Get image data
            fs::path texture_path = AppData.RootPath / TexturePaths[i].path;

            int width = 0, height = 0, data_size = 0;
            std::vector<char> texture_data = Tools::GetImageData( CONVERT_PATH(texture_path), 4, &width, &height, nullptr, &data_size );
            
            Parameters.Texture[i] = Parent.CreateImage( width, height, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, vk::ImageAspectFlagBits::eColor );
            Parameters.Texture[i].Sampler = Parent.CreateSampler( TexturePaths[i].mipmap_mode, TexturePaths[i].address_mode, VK_FALSE );

            // Copy data
            vk::ImageSubresourceRange image_subresource_range(
                vk::ImageAspectFlagBits::eColor,                              // VkImageAspectFlags             aspectMask
                0,                                                            // uint32_t                       baseMipLevel
                1,                                                            // uint32_t                       levelCount
                0,                                                            // uint32_t                       baseArrayLayer
                1                                                             // uint32_t                       layerCount
            );
            Parent.CopyDataToImage( data_size, texture_data.data(), *Parameters.Texture[i].Handle, width, height, image_subresource_range, vk::ImageLayout::eUndefined, vk::AccessFlags( 0 ), vk::PipelineStageFlagBits::eTopOfPipe, vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits::eShaderRead, vk::PipelineStageFlagBits::eFragmentShader );

        }

    }

    void SUI_PipelineBase::CreateVertexBuffer() {

        float coord = 0.5f;
        static const std::vector<VertexData> vertex_data = 
        {   
            { {-coord, -coord, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, 
            { {-coord, coord, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },
            { {coord, -coord, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },
            { {coord, coord, 0.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}  }

        };
        Parameters.VertexBuffer.Size = static_cast<uint32_t>( vertex_data.size() ) * sizeof( VertexData );
        Parameters.VertexBuffer = Parent.CreateBuffer( Parameters.VertexBuffer.Size , vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal );
        Parent.CopyDataToBuffer( Parameters.VertexBuffer.Size , vertex_data.data(), *Parameters.VertexBuffer.Handle, 0, vk::AccessFlags( 0 ), vk::PipelineStageFlagBits::eTopOfPipe, vk::AccessFlagBits::eVertexAttributeRead, vk::PipelineStageFlagBits::eVertexInput );
    }

    void SUI_PipelineBase::CreateIndexBuffer() {
        const std::vector<uint16_t> indices = { 0, 1, 2, 2, 1, 3 };
        Parameters.IndexBuffer.Size = static_cast<uint32_t>( indices.size() ) * sizeof( uint16_t );
        Parameters.IndexBuffer = Parent.CreateBuffer( Parameters.IndexBuffer.Size , vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal );
        Parent.CopyDataToBuffer( Parameters.IndexBuffer.Size , indices.data(), *Parameters.IndexBuffer.Handle, 0, vk::AccessFlags( 0 ), vk::PipelineStageFlagBits::eTopOfPipe, vk::AccessFlagBits::eIndexRead, vk::PipelineStageFlagBits::eVertexInput );
    }

    void SUI_PipelineBase::CreateDescriptorSet() {

        // Create descriptor resources
        std::vector<vk::DescriptorSetLayoutBinding> layout_bindings = {
            {   // shader.vert - u_UniformBuffer, set = 0, binding = 0
                0,                                                          // uint32_t                       binding
                vk::DescriptorType::eUniformBuffer,                         // VkDescriptorType               descriptorType
                1,                                                          // uint32_t                       descriptorCount
                vk::ShaderStageFlagBits::eFragment,                         // VkShaderStageFlags             stageFlags
                nullptr                                                     // const VkSampler               *pImmutableSamplers
            }
        };

        int texture_count = (int) Parameters.Texture.size();
        for ( int i = 0; i < texture_count; ++i) {
            uint32_t binding_index = (uint32_t) i + 1;                      
            layout_bindings.push_back(
                vk::DescriptorSetLayoutBinding(
                    binding_index,                                              // uint32_t                       binding
                    vk::DescriptorType::eCombinedImageSampler,                  // VkDescriptorType               descriptorType
                    1,                                                          // uint32_t                       descriptorCount
                    vk::ShaderStageFlagBits::eFragment,                         // VkShaderStageFlags             stageFlags
                    nullptr                                                     // const VkSampler               *pImmutableSamplers
                )
            );
        }

        std::vector<vk::DescriptorPoolSize> pool_sizes = {
            {
                vk::DescriptorType::eUniformBuffer,                         // VkDescriptorType               type
                1                                                           // uint32_t                       descriptorCount
            }
        };

        if ( texture_count > 0 ) {
            pool_sizes.push_back(
                vk::DescriptorPoolSize(
                    vk::DescriptorType::eCombinedImageSampler,                  // VkDescriptorType               type
                    (uint32_t) texture_count                                    // uint32_t                       descriptorCount
                )
            );
        }

        Parameters.DescriptorSet = Parent.CreateDescriptorResources( layout_bindings, pool_sizes );

        // Update descriptor sets - For Textures Only

        for ( int i = 0; i < texture_count; ++i) {
            std::vector<vk::DescriptorImageInfo> image_infos = {
                {
                    *Parameters.Texture[i].Sampler,                                    // VkSampler                      sampler
                    *Parameters.Texture[i].View,                                       // VkImageView                    imageView
                    vk::ImageLayout::eShaderReadOnlyOptimal                         // VkImageLayout                  imageLayout
                }
            };
            uint32_t binding_index = (uint32_t) i + 1;    
            Parent.UpdateDescriptorSet( *Parameters.DescriptorSet.Handle, vk::DescriptorType::eCombinedImageSampler, binding_index, 0, image_infos );
        }

    }

    void SUI_PipelineBase::CreateRenderPasses() {
        std::vector<RenderPassSubpassData> subpass_descriptions = {
            {
                {},                                                         // std::vector<VkAttachmentReference> const  &InputAttachments
                {                                                           // std::vector<VkAttachmentReference> const  &ColorAttachments
                    {
                        0,                                                        // uint32_t                                   attachment
                        vk::ImageLayout::eColorAttachmentOptimal                  // VkImageLayout                              layout
                    }
                },
                {                                                           // VkAttachmentReference const               &DepthStencilAttachment;
                    1,                                                            // uint32_t                                   attachment
                    vk::ImageLayout::eDepthStencilAttachmentOptimal               // VkImageLayout                              layout
                }
            }
        };

        std::vector<vk::SubpassDependency> dependencies = {
            {
                VK_SUBPASS_EXTERNAL,                                        // uint32_t                       srcSubpass
                0,                                                          // uint32_t                       dstSubpass
                vk::PipelineStageFlagBits::eColorAttachmentOutput,          // VkPipelineStageFlags           srcStageMask
                vk::PipelineStageFlagBits::eColorAttachmentOutput,          // VkPipelineStageFlags           dstStageMask
                vk::AccessFlagBits::eColorAttachmentWrite,                  // VkAccessFlags                  srcAccessMask
                vk::AccessFlagBits::eColorAttachmentWrite,                  // VkAccessFlags                  dstAccessMask
                vk::DependencyFlagBits::eByRegion                           // VkDependencyFlags              dependencyFlags
            },
            {
                0,                                                          // uint32_t                       srcSubpass
                VK_SUBPASS_EXTERNAL,                                        // uint32_t                       dstSubpass
                vk::PipelineStageFlagBits::eColorAttachmentOutput,          // VkPipelineStageFlags           srcStageMask
                vk::PipelineStageFlagBits::eColorAttachmentOutput,          // VkPipelineStageFlags           dstStageMask
                vk::AccessFlagBits::eColorAttachmentWrite,                  // VkAccessFlags                  srcAccessMask
                vk::AccessFlagBits::eColorAttachmentWrite,                  // VkAccessFlags                  dstAccessMask
                vk::DependencyFlagBits::eByRegion                           // VkDependencyFlags              dependencyFlags
            }
        };
        
        // Render pass - from present_src to color_attachment
        {
            std::vector<RenderPassAttachmentData> attachment_descriptions = {
                {
                    Parent.GetSwapChain().Format,                             // VkFormat                       format
                    vk::AttachmentLoadOp::eClear,                             // VkAttachmentLoadOp             loadOp
                    vk::AttachmentStoreOp::eStore,                            // VkAttachmentStoreOp            storeOp
                    vk::ImageLayout::ePresentSrcKHR,                          // VkImageLayout                  initialLayout
                    vk::ImageLayout::eColorAttachmentOptimal                  // VkImageLayout                  finalLayout
                },
                {
                    Parent.DefaultDepthFormat,                                // VkFormat                       format
                    vk::AttachmentLoadOp::eClear,                             // VkAttachmentLoadOp             loadOp
                    vk::AttachmentStoreOp::eStore,                            // VkAttachmentStoreOp            storeOp
                    vk::ImageLayout::eDepthStencilAttachmentOptimal,          // VkImageLayout                  initialLayout
                    vk::ImageLayout::eDepthStencilAttachmentOptimal           // VkImageLayout                  finalLayout
                }
            };
            Parameters.RenderPass = Parent.CreateRenderPass( attachment_descriptions, subpass_descriptions, dependencies );
        }

    }

    void SUI_PipelineBase::CreatePipelineLayout() { 
        std::vector<vk::PushConstantRange> push_constant_ranges {};
        for ( int i = 0; i < PushConstants.size(); ++i ) {
            push_constant_ranges.push_back(
                vk::PushConstantRange(
                    vk::ShaderStageFlagBits::eFragment,                               // VkShaderStageFlags             stageFlags
                    i * sizeof(float),                                                // uint32_t                       offset
                    sizeof(float)                                                     // uint32_t                       size
                )
            );
        };
        Parameters.PipelineLayout = Parent.CreatePipelineLayout( { *Parameters.DescriptorSet.Layout }, push_constant_ranges );
    }


    void SUI_PipelineBase::CreateGraphicsPipeline() {

        fs::path shader_vert_path = AppData.RootPath / Parameters.shader_vert_path;
        fs::path shader_frag_path = AppData.RootPath / Parameters.shader_frag_path;

        vk::UniqueShaderModule vertex_shader_module = Parent.CreateShaderModule( CONVERT_PATH(shader_vert_path) );
        vk::UniqueShaderModule fragment_shader_module = Parent.CreateShaderModule( CONVERT_PATH(shader_frag_path) );

        std::vector<vk::PipelineShaderStageCreateInfo> shader_stage_create_infos = {
            // Vertex shader
            {
            vk::PipelineShaderStageCreateFlags( 0 ),                    // VkPipelineShaderStageCreateFlags               flags
            vk::ShaderStageFlagBits::eVertex,                           // VkShaderStageFlagBits                          stage
            *vertex_shader_module,                                      // VkShaderModule                                 module
            "main"                                                      // const char                                    *pName
            },
            // Fragment shader
            {
            vk::PipelineShaderStageCreateFlags( 0 ),                    // VkPipelineShaderStageCreateFlags               flags
            vk::ShaderStageFlagBits::eFragment,                         // VkShaderStageFlagBits                          stage
            *fragment_shader_module,                                    // VkShaderModule                                 module
            "main"                                                      // const char                                    *pName
            }
        };

        std::vector<vk::VertexInputBindingDescription> vertex_binding_description = {
            {
                0,                                                          // uint32_t                                       binding
                sizeof( VertexData ),                                       // uint32_t                                       stride
                vk::VertexInputRate::eVertex                                // VkVertexInputRate                              inputRate
            }
        };

        std::vector<vk::VertexInputAttributeDescription> vertex_attribute_descriptions = {
            { // shader.vert, location = 0, i_Position
                0,                                                          // uint32_t                                       location
                vertex_binding_description[0].binding,                      // uint32_t                                       binding
                vk::Format::eR32G32B32Sfloat,                               // VkFormat                                       format
                offsetof(struct VertexData, Position)                       // uint32_t                                       offset
            },
            { // shader.vert, location = 1, i_Color
                1,                                                          // uint32_t                                       location
                vertex_binding_description[0].binding,                      // uint32_t                                       binding
                vk::Format::eR32G32B32Sfloat,                               // VkFormat                                       format
                offsetof(struct VertexData, Color)                          // uint32_t                                       offset
            },
            { // shader.vert, location = 2, i_Texcoord
                2,                                                          // uint32_t                                       location
                vertex_binding_description[0].binding,                      // uint32_t                                       binding
                vk::Format::eR32G32Sfloat,                                  // VkFormat                                       format
                offsetof(struct VertexData, Texcoords)                      // uint32_t                                       offset
            }
        };

        vk::PipelineVertexInputStateCreateInfo vertex_input_state_create_info(
            vk::PipelineVertexInputStateCreateFlags( 0 ),                 // VkPipelineVertexInputStateCreateFlags          flags;
            static_cast<uint32_t>(vertex_binding_description.size()),     // uint32_t                                       vertexBindingDescriptionCount
            vertex_binding_description.data(),                            // const VkVertexInputBindingDescription         *pVertexBindingDescriptions
            static_cast<uint32_t>(vertex_attribute_descriptions.size()),  // uint32_t                                       vertexAttributeDescriptionCount
            vertex_attribute_descriptions.data()                          // const VkVertexInputAttributeDescription       *pVertexAttributeDescriptions
        );

        vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_create_info(
            vk::PipelineInputAssemblyStateCreateFlags( 0 ),               // VkPipelineInputAssemblyStateCreateFlags        flags
            vk::PrimitiveTopology::eTriangleList,                         // VkPrimitiveTopology                            topology
            VK_FALSE                                                      // VkBool32                                       primitiveRestartEnable
        );

        vk::PipelineViewportStateCreateInfo viewport_state_create_info(
            vk::PipelineViewportStateCreateFlags( 0 ),                    // VkPipelineViewportStateCreateFlags             flags
            1,                                                            // uint32_t                                       viewportCount
            nullptr,                                                      // const VkViewport                              *pViewports
            1,                                                            // uint32_t                                       scissorCount
            nullptr                                                       // const VkRect2D                                *pScissors
        );

        vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info(
            vk::PipelineRasterizationStateCreateFlags( 0 ),               // VkPipelineRasterizationStateCreateFlags        flags
            VK_FALSE,                                                     // VkBool32                                       depthClampEnable
            VK_FALSE,                                                     // VkBool32                                       rasterizerDiscardEnable
            vk::PolygonMode::eFill,                                       // VkPolygonMode                                  polygonMode
            vk::CullModeFlagBits::eBack,                                  // VkCullModeFlags                                cullMode
            vk::FrontFace::eCounterClockwise,                             // VkFrontFace                                    frontFace
            VK_FALSE,                                                     // VkBool32                                       depthBiasEnable
            0.0f,                                                         // float                                          depthBiasConstantFactor
            0.0f,                                                         // float                                          depthBiasClamp
            0.0f,                                                         // float                                          depthBiasSlopeFactor
            1.0f                                                          // float                                          lineWidth
        );

        vk::PipelineMultisampleStateCreateInfo multisample_state_create_info(
            vk::PipelineMultisampleStateCreateFlags( 0 ),                 // VkPipelineMultisampleStateCreateFlags          flags
            vk::SampleCountFlagBits::e1,                                  // VkSampleCountFlagBits                          rasterizationSamples
            VK_FALSE,                                                     // VkBool32                                       sampleShadingEnable
            1.0f,                                                         // float                                          minSampleShading
            nullptr,                                                      // const VkSampleMask                            *pSampleMask
            VK_FALSE,                                                     // VkBool32                                       alphaToCoverageEnable
            VK_FALSE                                                      // VkBool32                                       alphaToOneEnable
        );

        vk::PipelineColorBlendAttachmentState color_blend_attachment_state(
            VK_FALSE,                                                     // VkBool32                                       blendEnable
            vk::BlendFactor::eSrcAlpha,                                   // VkBlendFactor                                  srcColorBlendFactor
            vk::BlendFactor::eOneMinusSrcAlpha,                           // VkBlendFactor                                  dstColorBlendFactor
            vk::BlendOp::eAdd,                                            // VkBlendOp                                      colorBlendOp
            vk::BlendFactor::eOne,                                        // VkBlendFactor                                  srcAlphaBlendFactor
            vk::BlendFactor::eZero,                                       // VkBlendFactor                                  dstAlphaBlendFactor
            vk::BlendOp::eAdd,                                            // VkBlendOp                                      alphaBlendOp
            vk::ColorComponentFlagBits::eR |                              // VkColorComponentFlags                          colorWriteMask
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA
        );

        vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info(
            vk::PipelineColorBlendStateCreateFlags( 0 ),                  // VkPipelineColorBlendStateCreateFlags           flags
            VK_FALSE,                                                     // VkBool32                                       logicOpEnable
            vk::LogicOp::eCopy,                                           // VkLogicOp                                      logicOp
            1,                                                            // uint32_t                                       attachmentCount
            &color_blend_attachment_state,                                // const VkPipelineColorBlendAttachmentState     *pAttachments
            { 0.0f, 0.0f, 0.0f, 0.0f }                                    // float                                          blendConstants[4]
        );

        vk::PipelineDepthStencilStateCreateInfo depth_stencil_state_create_info(
            vk::PipelineDepthStencilStateCreateFlags( 0 ),                // VkPipelineDepthStencilStateCreateFlags         flags
            VK_TRUE,                                                      // VkBool32                                       depthTestEnable
            VK_TRUE,                                                      // VkBool32                                       depthWriteEnable
            vk::CompareOp::eLessOrEqual                                   // VkCompareOp                                    depthCompareOp
        );

        std::vector<vk::DynamicState> dynamic_states = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor,
        };

        vk::PipelineDynamicStateCreateInfo dynamic_state_create_info(
            vk::PipelineDynamicStateCreateFlags( 0 ),                     // VkPipelineDynamicStateCreateFlags              flags
            static_cast<uint32_t>(dynamic_states.size()),                 // uint32_t                                       dynamicStateCount
            dynamic_states.data()                                         // const VkDynamicState                          *pDynamicStates
        );

        vk::GraphicsPipelineCreateInfo pipeline_create_info(
            vk::PipelineCreateFlags( 0 ),                                 // VkPipelineCreateFlags                          flags
            static_cast<uint32_t>(shader_stage_create_infos.size()),      // uint32_t                                       stageCount
            shader_stage_create_infos.data(),                             // const VkPipelineShaderStageCreateInfo         *pStages
            &vertex_input_state_create_info,                              // const VkPipelineVertexInputStateCreateInfo    *pVertexInputState;
            &input_assembly_state_create_info,                            // const VkPipelineInputAssemblyStateCreateInfo  *pInputAssemblyState
            nullptr,                                                      // const VkPipelineTessellationStateCreateInfo   *pTessellationState
            &viewport_state_create_info,                                  // const VkPipelineViewportStateCreateInfo       *pViewportState
            &rasterization_state_create_info,                             // const VkPipelineRasterizationStateCreateInfo  *pRasterizationState
            &multisample_state_create_info,                               // const VkPipelineMultisampleStateCreateInfo    *pMultisampleState
            &depth_stencil_state_create_info,                             // const VkPipelineDepthStencilStateCreateInfo   *pDepthStencilState
            &color_blend_state_create_info,                               // const VkPipelineColorBlendStateCreateInfo     *pColorBlendState
            &dynamic_state_create_info,                                   // const VkPipelineDynamicStateCreateInfo        *pDynamicState
            *Parameters.PipelineLayout,                                   // VkPipelineLayout                               layout
            *Parameters.RenderPass,                                       // VkRenderPass                                   renderPass
            0,                                                            // uint32_t                                       subpass
            vk::Pipeline(),                                               // VkPipeline                                     basePipelineHandle
            -1                                                            // int32_t                                        basePipelineIndex
        );

        Parameters.GraphicsPipeline = Parent.GetDevice().createGraphicsPipelineUnique( vk::PipelineCache(), pipeline_create_info );

    }
    
    // Render Methods

    void SUI_PipelineBase::AcquireImage( CurrentFrameData & current_frame ) {

        // Acquire swapchain image
        switch (Parent.GetDevice().acquireNextImageKHR( *current_frame.Swapchain->Handle, 3000000000, *current_frame.FrameResources->ImageAvailableSemaphore, vk::Fence(), &current_frame.SwapchainImageIndex )) {
            case vk::Result::eSuccess:
            case vk::Result::eSuboptimalKHR:
                break;
            case vk::Result::eErrorOutOfDateKHR:
                Parent.OnWindowSizeChanged();
                break;
            default:
                throw std::runtime_error( "Could not acquire swapchain image!" );
        }
        // Create a framebuffer for current frame
        current_frame.FrameResources->Framebuffer = Parent.CreateFramebuffer( { *current_frame.Swapchain->ImageViews[current_frame.SwapchainImageIndex], *current_frame.FrameResources->DepthAttachment.View }, current_frame.Swapchain->Extent, *Parameters.RenderPass );
        
    }

    void SUI_PipelineBase::UpdateUniformBuffer( SUI_PipelineData::DrawResourcesData & drawing_resources ) {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferData ubo = {};
        ubo.uTime = time;
        ubo.mouseX = (float) Parent.GetInputState().Position.X;
        ubo.mouseY = (float) Parent.GetInputState().Position.Y;
        Parent.GetInputState().Buttons[0].IsPressed ? ubo.mDown = 1: ubo.mDown = 0;
        ubo.dim = glm::vec4( X, Y, Width, Height );

        drawing_resources.UniformBuffer.Size = sizeof(ubo);
        drawing_resources.UniformBuffer = Parent.CreateBuffer( drawing_resources.UniformBuffer.Size , vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal );
        Parent.CopyDataToBuffer( drawing_resources.UniformBuffer.Size , &ubo, *drawing_resources.UniformBuffer.Handle, 0, vk::AccessFlags( 0 ), vk::PipelineStageFlagBits::eTopOfPipe, vk::AccessFlagBits::eUniformRead, vk::PipelineStageFlagBits::eFragmentShader );

        // Update Ubo Descriptor Set
        std::vector<vk::DescriptorBufferInfo> ubo_infos = {
            {
                *drawing_resources.UniformBuffer.Handle,                               // VkBuffer                       buffer
                0,                                                                     // VkDeviceSize                   offset
                drawing_resources.UniformBuffer.Size                                   // VkDeviceSize                   range
            }
            
        };
        Parent.UpdateDescriptorSet( *Parameters.DescriptorSet.Handle, vk::DescriptorType::eUniformBuffer, 0, 0, {}, ubo_infos );
    }

    void SUI_PipelineBase::RecordCommandBuffer( CurrentFrameData & current_frame, vk::CommandBuffer & command_buffer ) {

        // Specify clear values
        std::vector<vk::ClearValue> clear_values = {
            vk::ClearColorValue( std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 1.0f } ),           // VkClearColorValue                      color
            vk::ClearDepthStencilValue( 1.0f )                                               // VkClearDepthStencilValue               depthStencil
        };

        // Render pass begin info
        auto swapchain_extent = Parent.GetSwapChain().Extent;

        vk::RenderPassBeginInfo render_pass_begin_info(
            *Parameters.RenderPass,                           // VkRenderPass                             renderPass
            *current_frame.FrameResources->Framebuffer,       // VkFramebuffer                            framebuffer
            {                                                 // VkRect2D                                 renderArea
                vk::Offset2D(),                               // VkOffset2D                               offset
                swapchain_extent,                             // VkExtent2D                               extent
            },
            static_cast<uint32_t>(clear_values.size()),       // uint32_t                                 clearValueCount
            clear_values.data()                               // const VkClearValue                     * pClearValues
        );

        // Dynamic viewport and scissor info
        vk::Viewport viewport(
            0,                                                // float                                    x
            0,                                                // float                                    y
            static_cast<float>(swapchain_extent.width),       // float                                    width
            static_cast<float>(swapchain_extent.height),      // float                                    height
            0.0f,                                             // float                                    minDepth
            1.0f                                              // float                                    maxDepth
        );

        vk::Rect2D scissor = SetScissor();
        
        command_buffer.begin( { vk::CommandBufferUsageFlagBits::eOneTimeSubmit } );
        command_buffer.beginRenderPass( render_pass_begin_info, vk::SubpassContents::eInline );
        command_buffer.bindPipeline( vk::PipelineBindPoint::eGraphics, *Parameters.GraphicsPipeline );
        command_buffer.setViewport( 0, { viewport } );
        command_buffer.setScissor( 0, { scissor } );
        command_buffer.bindVertexBuffers( 0, { *Parameters.VertexBuffer.Handle }, { 0 } );
        command_buffer.bindIndexBuffer( *Parameters.IndexBuffer.Handle, 0, vk::IndexType::eUint16 );
        command_buffer.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, *Parameters.PipelineLayout, 0, { *Parameters.DescriptorSet.Handle }, {} );

        for ( int i = 0; i < (int) PushConstants.size(); ++i ) {
            uint32_t push_constant_offset = (uint32_t) i * sizeof(float);
            float * push_constant_ptr = PushConstants[i];
            command_buffer.pushConstants( *Parameters.PipelineLayout, vk::ShaderStageFlagBits::eFragment, push_constant_offset, sizeof(float), push_constant_ptr );
        }

        command_buffer.drawIndexed( 6, 1, 0, 0, 0 );
        command_buffer.endRenderPass();
        command_buffer.end();

    }

    void SUI_PipelineBase::SubmitCommandBuffer( CurrentFrameData & current_frame, vk::CommandBuffer & command_buffer ) {
        vk::PipelineStageFlags wait_dst_stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;  
        vk::SubmitInfo submit_info(
            1,                                                          // uint32_t                       waitSemaphoreCount
            &(*current_frame.FrameResources->ImageAvailableSemaphore),  // const VkSemaphore             *pWaitSemaphores
            &wait_dst_stage_mask,                                       // const VkPipelineStageFlags    *pWaitDstStageMask
            1,                                                          // uint32_t                       commandBufferCount;
            &command_buffer                                             // const VkCommandBuffer         *pCommandBuffers
        );
        Parent.GetGraphicsQueue().Handle.submit( { submit_info }, vk::Fence() );
    }

    void SUI_PipelineBase::SubmitComplete(){
        
    }

    vk::Rect2D SUI_PipelineBase::SetScissor() {
        return vk::Rect2D( { (int32_t) X, (int32_t) Y }, { (uint32_t) Width, (uint32_t) Height } );
    } 

    /* Use this to have the scissor take the full window extent - this would ignore the layout
    vk::Rect2D SUI_PipelineBase::SetScissor() {
        return vk::Rect2D (
            vk::Offset2D(),                                   // VkOffset2D                               offset
            Parent.GetSwapChain().Extent                      // VkExtent2D                               extent
        );
    }
    */

} // end namespace AppCore

#endif // C_SUI_PIPELINEBASE_CPP