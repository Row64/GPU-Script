#ifndef SUI_DEMO_ICE_CPP
#define SUI_DEMO_ICE_CPP

#include "c_PipelineManager.h"

namespace AppCore { 
    
    IceSUI::IceSUI( PipelineManager &parent ) :
        SUI_PipelineBase( parent ) {
        Parameters.shader_vert_path = "Shaders/ice.vert.spv";
        Parameters.shader_frag_path = "Shaders/ice.frag.spv";
        TexturePaths = { 
           { "Data/gray_noise.png", vk::SamplerMipmapMode::eNearest, vk::SamplerAddressMode::eClampToEdge},
           { "Data/background.jpg", vk::SamplerMipmapMode::eNearest, vk::SamplerAddressMode::eRepeat}
        };
    }

    IceSUI::~IceSUI() {
    }

    // Prepare methods

    void IceSUI::UpdateUniformBuffer( SUI_PipelineData::DrawResourcesData & drawing_resources ) {
    
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        IceUboData ubo = {};
        ubo.uTime = time;
        ubo.mouseX = (float) Parent.GetInputState().Position.X;
        ubo.mouseY = (float) Parent.GetInputState().Position.Y;
        Parent.GetInputState().Buttons[0].IsPressed ? ubo.mDown = 1: ubo.mDown = 0;
        ubo.dim = glm::vec4( X, Y, Width, Height );
        
        ubo.roughness = AppData.Roughness;
        ubo.isoval = AppData.IsoValue;
        ubo.icecol = AppData.IceColor;
        ubo.refraction = AppData.Refraction;

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

} // end namespace AppCore

#endif // SUI_DEMO_ICE_CPP