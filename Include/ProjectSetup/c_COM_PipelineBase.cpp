#ifndef C_COM_PIPELINEBASE_CPP
#define C_COM_PIPELINEBASE_CPP

#include "c_PipelineManager.h"

namespace AppCore {

    COM_PipelineBase::COM_PipelineBase( PipelineManager &parent ) :
        Parent( parent ),
        AppData( parent.GetAppData() ) {
    }

    COM_PipelineBase::~COM_PipelineBase() {
    }

    COM_PipelineData & COM_PipelineBase::GetParameters() {
        return Parameters;
    }

    void COM_PipelineBase::Init() {

        CreateStorageBuffers();
        CreateDescriptorSet();
        CreatePipelineLayout();
        CreateComputePipeline();
        
    }

    bool COM_PipelineBase::Execute( ComputeResourcesData * compute_resource ) {

        if (IsRunning == true) {
            std::cout << "--- Compute Process '" << Name << "' is already running." << std::endl;
            return false;
        }
        IsRunning = true;

        // Check fence is signaled and reset fences
        if( Parent.GetDevice().waitForFences( { *compute_resource->Fence }, VK_FALSE, 1000000000 ) != vk::Result::eSuccess ) {
            throw std::runtime_error( "Waiting for a fence takes too long!" );
        }
        Parent.GetDevice().resetFences( { *compute_resource->Fence } );

        PreSubmit();
        RecordCommandBuffer( *compute_resource->CommandBuffer );
        SubmitCommandBuffer( *compute_resource, *compute_resource->CommandBuffer);

        return true;
    }

    void COM_PipelineBase::SubmitComplete() {
        ProcessOutputs();
        IsRunning = false;
    }

    // ------------ Protected Methods ------------

    // Init Methods

    void COM_PipelineBase::CreateStorageBuffers() {

        // Note: You need to keep track of which StorageBuffers you are using for input or output.
        // The index of the storage buffer should correspond to the binding value in your compute shader.

        Parameters.StorageBuffers.resize(2);

        // Input buffer
        Parameters.StorageBuffers[0].Size = sizeof(float) * 2;
        Parameters.StorageBuffers[0] = Parent.CreateBuffer( Parameters.StorageBuffers[0].Size , vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eHostVisible );

        // Output buffer
        Parameters.StorageBuffers[1].Size = sizeof(float) * 2;
        Parameters.StorageBuffers[1] = Parent.CreateBuffer( Parameters.StorageBuffers[1].Size , vk::BufferUsageFlagBits::eStorageBuffer , vk::MemoryPropertyFlagBits::eHostVisible );

    }

    void COM_PipelineBase::CreateDescriptorSet() {

        // Create descriptor resources
        std::vector<vk::DescriptorSetLayoutBinding> layout_bindings;

        uint32_t buffer_count = (uint32_t) Parameters.StorageBuffers.size();
        for ( uint32_t i = 0; i < buffer_count; ++i) {                
            layout_bindings.push_back(
                vk::DescriptorSetLayoutBinding(
                    i,                                                          // uint32_t                       binding
                    vk::DescriptorType::eStorageBuffer,                         // VkDescriptorType               descriptorType
                    1,                                                          // uint32_t                       descriptorCount
                    vk::ShaderStageFlagBits::eCompute,                          // VkShaderStageFlags             stageFlags
                    nullptr                                                     // const VkSampler               *pImmutableSamplers
                )
            );
        }

        std::vector<vk::DescriptorPoolSize> pool_sizes = {
            {
                vk::DescriptorType::eStorageBuffer,                         // VkDescriptorType               type
                buffer_count                                                // uint32_t                       descriptorCount
            }
        };

        Parameters.DescriptorSet = Parent.CreateDescriptorResources( layout_bindings, pool_sizes );

        // Update Compute Descriptor Set
        for ( uint32_t i = 0; i < buffer_count; ++i) {
            std::vector<vk::DescriptorBufferInfo> compute_buffer_infos = {
                {
                    *Parameters.StorageBuffers[i].Handle,                               // VkBuffer                       buffer
                    0,                                                                  // VkDeviceSize                   offset
                    Parameters.StorageBuffers[i].Size                                   // VkDeviceSize                   range
                }
                
            };
            Parent.UpdateDescriptorSet( *Parameters.DescriptorSet.Handle, vk::DescriptorType::eStorageBuffer, i, 0, {}, compute_buffer_infos );
        }
    }

    void COM_PipelineBase::CreatePipelineLayout() { 
        Parameters.PipelineLayout = Parent.CreatePipelineLayout( { *Parameters.DescriptorSet.Layout }, {} );
    }

    void COM_PipelineBase::CreateComputePipeline() {

        fs::path shader_comp_path = AppData.RootPath / Parameters.shader_comp_path;

        vk::UniqueShaderModule compute_shader_module = Parent.CreateShaderModule( CONVERT_PATH(shader_comp_path) );

        vk::PipelineShaderStageCreateInfo shader_stage_create_infos(
            vk::PipelineShaderStageCreateFlags( 0 ),                    // VkPipelineShaderStageCreateFlags               flags
            vk::ShaderStageFlagBits::eCompute,                          // VkShaderStageFlagBits                          stage
            *compute_shader_module,                                     // VkShaderModule                                 module
            "main"                                                      // const char                                    *pName
        );

        vk::ComputePipelineCreateInfo pipeline_create_info(
            vk::PipelineCreateFlags( 0 ),                               // VkPipelineCreateFlags                flags
            shader_stage_create_infos,                                  // VkPipelineShaderStageCreateInfo      stage
            *Parameters.PipelineLayout,                                 // VkPipelineLayout                     layout
            vk::Pipeline(),                                             // VkPipeline                           basePipelineHandle
            -1                                                          // int32_t                              basePipelineIndex
        );

        Parameters.ComputePipeline = Parent.GetDevice().createComputePipelineUnique( vk::PipelineCache(), pipeline_create_info );

    }

    // Execute methods
    
    void COM_PipelineBase::PreSubmit() {
        ProcessInputs();
    }

    void COM_PipelineBase::ProcessInputs() {

        // Copy Input Buffer from CPU to GPU
        vector<float> input_data = {7.0f, 10.0f};
        Parent.COM_CopyDataToBuffer( Parameters.StorageBuffers[0].Size, input_data.data(), *Parameters.StorageBuffers[0].Handle, 0, vk::AccessFlags( 0 ), vk::PipelineStageFlagBits::eTopOfPipe, vk::AccessFlagBits::eShaderRead, vk::PipelineStageFlagBits::eComputeShader );

    }

    void COM_PipelineBase::RecordCommandBuffer( vk::CommandBuffer & command_buffer ){
        
        command_buffer.begin( { vk::CommandBufferUsageFlagBits::eOneTimeSubmit } );
        command_buffer.bindPipeline( vk::PipelineBindPoint::eCompute, *Parameters.ComputePipeline );
        command_buffer.bindDescriptorSets( vk::PipelineBindPoint::eCompute, *Parameters.PipelineLayout, 0, { *Parameters.DescriptorSet.Handle }, {} );
        command_buffer.dispatch((uint32_t) NbWorkgroups.x, (uint32_t) NbWorkgroups.y, (uint32_t) NbWorkgroups.z);
        command_buffer.end();

    }

    void COM_PipelineBase::SubmitCommandBuffer( ComputeResourcesData & compute_resource, vk::CommandBuffer & command_buffer ){
        vk::SubmitInfo submit_info(
            0,                                                          // uint32_t                       waitSemaphoreCount
            nullptr,                                                    // const VkSemaphore             *pWaitSemaphores
            nullptr,                                                    // const VkPipelineStageFlags    *pWaitDstStageMask
            1,                                                          // uint32_t                       commandBufferCount;
            &command_buffer                                             // const VkCommandBuffer         *pCommandBuffers
                                                                        // uint32_t                       signalSemaphoreCount
                                                                        // const VkSemaphore             *pSignalSemaphores
        );
        Parent.GetComputeQueue().Handle.submit( { submit_info }, *compute_resource.Fence );
    }

    void COM_PipelineBase::ProcessOutputs() {

        void* mappedMemory = NULL;

        // Map the buffer memory, so that we can read from it on the CPU.
        Parent.GetDevice().mapMemory( *Parameters.StorageBuffers[1].Memory, 0, Parameters.StorageBuffers[1].Size, {}, &mappedMemory);
        float* pmappedMemory = (float *)mappedMemory;

        // The mapped memory is a range of data, so you can access it like an array.
        // In this example there are 2 floats passed in via StorageBuffer[0] and passed out via StorageBuffer[1].
        // We retrieve the StorageBuffer[1] data. 
        // Alternatively, you can pass in 1 storage buffer and have the gpu modify that directly. Rather than
        // having separate input and output buffers.
        vector<float> out_data;
        out_data.push_back(pmappedMemory[0]);
        out_data.push_back(pmappedMemory[1]);
        std::cout << "--- COM_PipelineBase::SubmitComplete()    Output Data: " << out_data[0] << ", " << out_data[1] << std::endl;

        // Done reading, so unmap.
        Parent.GetDevice().unmapMemory( *Parameters.StorageBuffers[1].Memory );

    }

} // end namespace AppCore

#endif // C_COM_PIPELINEBASE_CPP