////////////////////////////////////////////////////////////////////////////////
// Copyright 2018 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
////////////////////////////////////////////////////////////////////////////////

// Modifications Copyright (C) 2019 Dillon Ridge Computing
// Under the MIT License in LICENSE.txt


#ifndef C_VKPIPELINEBASE_CPP
#define C_VKPIPELINEBASE_CPP

#include "c_VkPipelineBase.h"

namespace AppCore {

    VkPipelineBase::VkPipelineBase() :
        FrameResources() {
    }

    VkPipelineBase::~VkPipelineBase() {
        if( GetDevice() ) {
            GetDevice().waitIdle();
        }
    }

    void VkPipelineBase::InitPipelines( WindowParameters window ) {
        InitVulkan( window );
    }


    // Resource creation methods

    vk::UniqueShaderModule VkPipelineBase::CreateShaderModule( char const * filename ) const {

        const std::vector<char> code = Tools::GetBinaryFileContents( filename );

        vk::ShaderModuleCreateInfo shader_module_create_info(
            vk::ShaderModuleCreateFlags( 0 ),                     // VkShaderModuleCreateFlags      flags
            code.size(),                                          // size_t                         codeSize
            reinterpret_cast<const uint32_t*>(code.data())        // const uint32_t                *pCode
        );

        return GetDevice().createShaderModuleUnique( shader_module_create_info );
    }

    ImageParameters VkPipelineBase::CreateImage( uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::MemoryPropertyFlagBits property, vk::ImageAspectFlags aspect, vk::ImageType image_type, vk::ImageViewType image_view_type ) const {

        vk::UniqueImage tmp_image;
        ImplCreateImage( width, height, format, usage, tmp_image, image_type );

        vk::UniqueDeviceMemory tmp_memory;
        ImplAllocateImageMemory( *tmp_image, property, tmp_memory );

        GetDevice().bindImageMemory( *tmp_image, *tmp_memory, vk::DeviceSize( 0 ) );

        vk::UniqueImageView tmp_view;
        ImplCreateImageView( *tmp_image, format, aspect, tmp_view, image_view_type );

        ImageParameters image;
        image.Handle = std::move( tmp_image );
        image.Memory = std::move( tmp_memory );
        image.View   = std::move( tmp_view );

        return std::move( image );
    }

    BufferParameters VkPipelineBase::CreateBuffer( uint32_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlagBits memoryProperty ) const {

        vk::UniqueBuffer tmp_buffer;
        ImplCreateBuffer( size, usage, tmp_buffer );

        vk::UniqueDeviceMemory tmp_memory;
        ImplAllocateBufferMemory( *tmp_buffer, memoryProperty, tmp_memory );

        GetDevice().bindBufferMemory( *tmp_buffer, *tmp_memory, vk::DeviceSize( 0 ) );

        BufferParameters buffer;
        buffer.Size = size;
        buffer.Handle = std::move( tmp_buffer );
        buffer.Memory = std::move( tmp_memory );

        return std::move( buffer );
    }

    DescriptorSetParameters VkPipelineBase::CreateDescriptorResources( std::vector<vk::DescriptorSetLayoutBinding> const & layout_bindings, std::vector<vk::DescriptorPoolSize> const & pool_sizes ) const {

        vk::UniqueDescriptorSetLayout tmp_layout;
        ImplCreateDescriptorSetLayout( layout_bindings, tmp_layout );

        vk::UniqueDescriptorPool tmp_pool;
        ImplCreateDescriptorPool( pool_sizes, 1, tmp_pool );

        std::vector<vk::UniqueDescriptorSet> tmp_sets;
        ImplAllocateDescriptorSets( { *tmp_layout }, *tmp_pool, tmp_sets );

        DescriptorSetParameters descriptor_set;
        descriptor_set.Layout = std::move( tmp_layout );
        descriptor_set.Pool = std::move( tmp_pool );
        descriptor_set.Handle = std::move( tmp_sets[0] );

        return std::move( descriptor_set );
    }

    vk::UniqueFramebuffer VkPipelineBase::CreateFramebuffer( std::vector<vk::ImageView> const & image_views, vk::Extent2D const & extent, vk::RenderPass const & render_pass ) const {
        
        vk::FramebufferCreateInfo framebuffer_create_info(
            vk::FramebufferCreateFlags( 0 ),                  // VkFramebufferCreateFlags       flags
            render_pass,                                      // VkRenderPass                   renderPass
            static_cast<uint32_t>(image_views.size()),        // uint32_t                       attachmentCount
            image_views.data(),                               // const VkImageView             *pAttachments
            extent.width,                                     // uint32_t                       width
            extent.height,                                    // uint32_t                       height
            1                                                 // uint32_t                       layers
        );

        return GetDevice().createFramebufferUnique( framebuffer_create_info );
    }

    vk::UniqueSampler VkPipelineBase::CreateSampler( vk::SamplerMipmapMode mipmap_mode, vk::SamplerAddressMode address_mode, vk::Bool32 unnormalized_coords ) const {

        vk::SamplerCreateInfo sampler_create_info(
                vk::SamplerCreateFlags( 0 ),                  // VkSamplerCreateFlags       flags
                vk::Filter::eLinear,                          // VkFilter                   magFilter
                vk::Filter::eLinear,                          // VkFilter                   minFilter
                mipmap_mode,                                  // VkSamplerMipmapMode        mipmapMode
                address_mode,                                 // VkSamplerAddressMode       addressModeU
                address_mode,                                 // VkSamplerAddressMode       addressModeV
                address_mode,                                 // VkSamplerAddressMode       addressModeW
                0.0f,                                         // float                      mipLodBias
                VK_FALSE,                                     // VkBool32                   anisotropyEnable
                1.0f,                                         // float                      maxAnisotropy
                VK_FALSE,                                     // VkBool32                   compareEnable
                vk::CompareOp::eAlways,                       // VkCompareOp                compareOp
                0.0f,                                         // float                      minLod
                0.0f,                                         // float                      maxLod
                vk::BorderColor::eFloatOpaqueBlack,           // VkBorderColor              borderColor
                unnormalized_coords                           // VkBool32                   unnormalizedCoordinates
        );
        return GetDevice().createSamplerUnique( sampler_create_info );

    }

    vk::UniqueRenderPass VkPipelineBase::CreateRenderPass( std::vector<RenderPassAttachmentData> const & attachment_descriptions, std::vector<RenderPassSubpassData> const & subpass_descriptions, std::vector<vk::SubpassDependency> const & dependencies ) const {

        std::vector<vk::AttachmentDescription> tmp_descriptions;

        for( auto & attachment : attachment_descriptions ) {
            tmp_descriptions.emplace_back(
                vk::AttachmentDescriptionFlags( 0 ),                    // VkAttachmentDescriptionFlags     flags
                attachment.Format,                                      // VkFormat                         format
                vk::SampleCountFlagBits::e1,                            // VkSampleCountFlagBits            samples
                attachment.LoadOp,                                      // VkAttachmentLoadOp               loadOp
                attachment.StoreOp,                                     // VkAttachmentStoreOp              storeOp
                vk::AttachmentLoadOp::eDontCare,                        // VkAttachmentLoadOp               stencilLoadOp
                vk::AttachmentStoreOp::eDontCare,                       // VkAttachmentStoreOp              stencilStoreOp
                attachment.InitialLayout,                               // VkImageLayout                    initialLayout
                attachment.FinalLayout                                  // VkImageLayout                    finalLayout
            );
        }

        std::vector<vk::SubpassDescription> tmp_subpasses;
        for( auto & subpass : subpass_descriptions ) {
            tmp_subpasses.emplace_back(
                vk::SubpassDescriptionFlags( 0 ),                       // VkSubpassDescriptionFlags        flags
                vk::PipelineBindPoint::eGraphics,                       // VkPipelineBindPoint              pipelineBindPoint
                static_cast<uint32_t>(subpass.InputAttachments.size()), // uint32_t                         inputAttachmentCount
                subpass.InputAttachments.data(),                        // const VkAttachmentReference     *pInputAttachments
                static_cast<uint32_t>(subpass.ColorAttachments.size()), // uint32_t                         colorAttachmentCount
                subpass.ColorAttachments.data(),                        // const VkAttachmentReference     *pColorAttachments
                nullptr,                                                // const VkAttachmentReference     *pResolveAttachments
                &subpass.DepthStencilAttachment                         // const VkAttachmentReference     *pDepthStencilAttachment
            );
        }

        vk::RenderPassCreateInfo render_pass_create_info(
            vk::RenderPassCreateFlags( 0 ),                               // VkRenderPassCreateFlags        flags
            static_cast<uint32_t>(tmp_descriptions.size()),               // uint32_t                       attachmentCount
            tmp_descriptions.data(),                                      // const VkAttachmentDescription *pAttachments
            static_cast<uint32_t>(tmp_subpasses.size()),                  // uint32_t                       subpassCount
            tmp_subpasses.data(),                                         // const VkSubpassDescription    *pSubpasses
            static_cast<uint32_t>(dependencies.size()),                   // uint32_t                       dependencyCount
            dependencies.data()                                           // const VkSubpassDependency     *pDependencies
        );

        return GetDevice().createRenderPassUnique( render_pass_create_info );
    }

    vk::UniquePipelineLayout VkPipelineBase::CreatePipelineLayout( std::vector<vk::DescriptorSetLayout> const & descriptor_set_layouts, std::vector<vk::PushConstantRange> const & push_constant_ranges ) const {

        std::cout << "--- VkPipelineBase::CreatePipelineLayout: " << std::endl;
        std::cout << "\t" << descriptor_set_layouts.data() << std::endl;
        
        vk::PipelineLayoutCreateInfo layout_create_info(
            vk::PipelineLayoutCreateFlags( 0 ),                   // VkPipelineLayoutCreateFlags    flags
            static_cast<uint32_t>(descriptor_set_layouts.size()), // uint32_t                       setLayoutCount
            descriptor_set_layouts.data(),                        // const VkDescriptorSetLayout   *pSetLayouts
            static_cast<uint32_t>(push_constant_ranges.size()),   // uint32_t                       pushConstantRangeCount
            push_constant_ranges.data()                           // const VkPushConstantRange     *pPushConstantRanges
        );

        return GetDevice().createPipelineLayoutUnique( layout_create_info );
        }

    vk::UniqueSemaphore VkPipelineBase::CreateSemaphore() const {
        return GetDevice().createSemaphoreUnique( {} );
    }

    vk::UniqueFence VkPipelineBase::CreateFence( bool signaled) const {
        return GetDevice().createFenceUnique( { signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags( 0u ) } );
    }

    vk::UniqueCommandPool VkPipelineBase::CreateCommandPool( uint32_t queue_family_index, vk::CommandPoolCreateFlags flags )  const {
        return GetDevice().createCommandPoolUnique( { flags, queue_family_index } );
    }

    std::vector<vk::UniqueCommandBuffer> VkPipelineBase::AllocateCommandBuffers( vk::CommandPool const & pool, vk::CommandBufferLevel level, uint32_t count ) const {

        vk::CommandBufferAllocateInfo command_buffer_allocate_info(
            pool,                                             // VkCommandPool                  commandPool
            level,                                            // VkCommandBufferLevel           level
            count                                             // uint32_t                       bufferCount
        );

        return GetDevice().allocateCommandBuffersUnique( command_buffer_allocate_info );
    }


    // Resource tools

    void VkPipelineBase::UpdateDescriptorSet( vk::DescriptorSet & descriptor_set, vk::DescriptorType descriptor_type, uint32_t binding, uint32_t array_element, std::vector<vk::DescriptorImageInfo> const & image_infos, std::vector<vk::DescriptorBufferInfo> const & buffer_infos, std::vector<vk::BufferView> const & buffer_views ) const {
       
        vk::WriteDescriptorSet descriptor_writes(
            descriptor_set,                                       // VkDescriptorSet                dstSet
            binding,                                              // uint32_t                       dstBinding
            array_element,                                        // uint32_t                       dstArrayElement
            1,                                                    // uint32_t                       descriptorCount
            descriptor_type,                                      // VkDescriptorType               descriptorType
            image_infos.size() ? image_infos.data() : nullptr,    // const VkDescriptorImageInfo   *pImageInfo
            buffer_infos.size() ? buffer_infos.data() : nullptr,  // const VkDescriptorBufferInfo  *pBufferInfo
            buffer_views.size() ? buffer_views.data() : nullptr   // const VkBufferView            *pTexelBufferView
        );

        GetDevice().updateDescriptorSets( 1, &descriptor_writes, 0, nullptr );
    }

    void VkPipelineBase::SetImageMemoryBarrier( vk::Image const & image, vk::ImageSubresourceRange const & image_subresource_range, vk::ImageLayout current_image_layout, vk::AccessFlags current_image_access, vk::PipelineStageFlags generating_stages, vk::ImageLayout new_image_layout, vk::AccessFlags new_image_access, vk::PipelineStageFlags consuming_stages ) const {
        
        // Allocate temporary command buffer from a temporary command pool
        vk::UniqueCommandPool command_pool = CreateCommandPool( GetGraphicsQueue().FamilyIndex, vk::CommandPoolCreateFlagBits::eTransient );
        vk::UniqueCommandBuffer command_buffer = std::move( AllocateCommandBuffers( *command_pool, vk::CommandBufferLevel::ePrimary, 1 )[0] );

        // Record command buffer which copies data from the staging buffer to the destination buffer
        command_buffer->begin( { vk::CommandBufferUsageFlagBits::eOneTimeSubmit } );
        vk::ImageMemoryBarrier image_memory_barrier(
            current_image_access,                         // VkAccessFlags                          srcAccessMask
            new_image_access,                             // VkAccessFlags                          dstAccessMask
            current_image_layout,                         // VkImageLayout                          oldLayout
            new_image_layout,                             // VkImageLayout                          newLayout
            VK_QUEUE_FAMILY_IGNORED,                      // uint32_t                               srcQueueFamilyIndex
            VK_QUEUE_FAMILY_IGNORED,                      // uint32_t                               dstQueueFamilyIndex
            image,                                        // VkImage                                image
            image_subresource_range                       // VkImageSubresourceRange                subresourceRange
        );
        command_buffer->pipelineBarrier( generating_stages, consuming_stages, vk::DependencyFlags( 0 ), {}, {}, { image_memory_barrier } );
        command_buffer->end();

        // Submit
        auto fence = CreateFence( false );
        vk::SubmitInfo submit_info(
            0,                                            // uint32_t                               waitSemaphoreCount
            nullptr,                                      // const VkSemaphore                     *pWaitSemaphores
            nullptr,                                      // const VkPipelineStageFlags            *pWaitDstStageMask
            1,                                            // uint32_t                               commandBufferCount
            &(*command_buffer),                           // const VkCommandBuffer                 *pCommandBuffers
            0,                                            // uint32_t                               signalSemaphoreCount
            nullptr                                       // const VkSemaphore                     *pSignalSemaphores
        );
        GetGraphicsQueue().Handle.submit( { submit_info }, *fence );
        GetDevice().waitForFences( { *fence }, VK_FALSE, 3000000000 );
    }

    void VkPipelineBase::CopyDataToImage( uint32_t data_size, void const * data, vk::Image & target_image, uint32_t width, uint32_t height, vk::ImageSubresourceRange const & image_subresource_range, vk::ImageLayout current_image_layout, vk::AccessFlags current_image_access, vk::PipelineStageFlags generating_stages, vk::ImageLayout new_image_layout, vk::AccessFlags new_image_access, vk::PipelineStageFlags consuming_stages ) const {
    

        // Create staging buffer and map it's memory to copy data from the CPU

        StagingBufferParameters staging_buffer;
        staging_buffer.Buffer = CreateBuffer( data_size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible );
        staging_buffer.Pointer = GetDevice().mapMemory( *staging_buffer.Buffer.Memory, 0, data_size );

        std::memcpy( staging_buffer.Pointer, data, data_size );

        vk::MappedMemoryRange memory_range(
            *staging_buffer.Buffer.Memory,              // VkDeviceMemory                         memory
            0,                                          // VkDeviceSize                           offset
            VK_WHOLE_SIZE                               // VkDeviceSize                           size
        );

        GetDevice().flushMappedMemoryRanges( { memory_range } );
        GetDevice().unmapMemory( *staging_buffer.Buffer.Memory );


        // Allocate temporary command buffer from a temporary command pool

        vk::UniqueCommandPool command_pool = CreateCommandPool( GetGraphicsQueue().FamilyIndex, vk::CommandPoolCreateFlagBits::eTransient );
        vk::UniqueCommandBuffer command_buffer = std::move( AllocateCommandBuffers( *command_pool, vk::CommandBufferLevel::ePrimary, 1 )[0] );


        // Record command buffer which copies data from the staging buffer to the destination buffer

        command_buffer->begin( { vk::CommandBufferUsageFlagBits::eOneTimeSubmit } );

        vk::ImageMemoryBarrier pre_transfer_image_memory_barrier(
            current_image_access,                         // VkAccessFlags                          srcAccessMask
            vk::AccessFlagBits::eTransferWrite,           // VkAccessFlags                          dstAccessMask
            current_image_layout,                         // VkImageLayout                          oldLayout
            vk::ImageLayout::eTransferDstOptimal,         // VkImageLayout                          newLayout
            VK_QUEUE_FAMILY_IGNORED,                      // uint32_t                               srcQueueFamilyIndex
            VK_QUEUE_FAMILY_IGNORED,                      // uint32_t                               dstQueueFamilyIndex
            target_image,                                 // VkImage                                image
            image_subresource_range                       // VkImageSubresourceRange                subresourceRange
        );
        command_buffer->pipelineBarrier( generating_stages, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags( 0 ), {}, {}, { pre_transfer_image_memory_barrier } );

        std::vector<vk::BufferImageCopy> buffer_image_copy;
        buffer_image_copy.reserve( image_subresource_range.levelCount );

        for( uint32_t i = image_subresource_range.baseMipLevel; i < image_subresource_range.baseMipLevel + image_subresource_range.levelCount; ++i ) {
            buffer_image_copy.emplace_back(
                0,                                          // VkDeviceSize                           bufferOffset
                0,                                          // uint32_t                               bufferRowLength
                0,                                          // uint32_t                               bufferImageHeight
                vk::ImageSubresourceLayers(                 // VkImageSubresourceLayers               imageSubresource
                image_subresource_range.aspectMask,         // VkImageAspectFlags                     aspectMask
                i,                                          // uint32_t                               mipLevel
                image_subresource_range.baseArrayLayer,     // uint32_t                               baseArrayLayer
                image_subresource_range.layerCount          // uint32_t                               layerCount
                ),
                vk::Offset3D(),                             // VkOffset3D                             imageOffset
                vk::Extent3D(                               // VkExtent3D                             imageExtent
                width,                                      // uint32_t                               width
                height,                                     // uint32_t                               height
                1                                           // uint32_t                               depth
                )
            );
        }
        command_buffer->copyBufferToImage( *staging_buffer.Buffer.Handle, target_image, vk::ImageLayout::eTransferDstOptimal, buffer_image_copy );

        vk::ImageMemoryBarrier post_transfer_image_memory_barrier(
            vk::AccessFlagBits::eTransferWrite,           // VkAccessFlags                          srcAccessMask
            new_image_access,                             // VkAccessFlags                          dstAccessMask
            vk::ImageLayout::eTransferDstOptimal,         // VkImageLayout                          oldLayout
            new_image_layout,                             // VkImageLayout                          newLayout
            VK_QUEUE_FAMILY_IGNORED,                      // uint32_t                               srcQueueFamilyIndex
            VK_QUEUE_FAMILY_IGNORED,                      // uint32_t                               dstQueueFamilyIndex
            target_image,                                 // VkImage                                image
            image_subresource_range                       // VkImageSubresourceRange                subresourceRange
        );

        command_buffer->pipelineBarrier( vk::PipelineStageFlagBits::eTransfer, consuming_stages, vk::DependencyFlags( 0 ), {}, {}, { post_transfer_image_memory_barrier } );
        command_buffer->end();
    
        // Submit

        vk::UniqueFence fence = CreateFence( false );
        vk::SubmitInfo submit_info(
            0,                                            // uint32_t                               waitSemaphoreCount
            nullptr,                                      // const VkSemaphore                     *pWaitSemaphores
            nullptr,                                      // const VkPipelineStageFlags            *pWaitDstStageMask
            1,                                            // uint32_t                               commandBufferCount
            &(*command_buffer)                            // const VkCommandBuffer                 *pCommandBuffers
        );
        GetGraphicsQueue().Handle.submit( { submit_info }, *fence );
        GetDevice().waitForFences( { *fence }, VK_FALSE, 3000000000 );

    }


    void VkPipelineBase::CopyDataToBuffer( uint32_t data_size, void const * data, vk::Buffer target_buffer, vk::DeviceSize buffer_offset, vk::AccessFlags current_buffer_access, vk::PipelineStageFlags generating_stages, vk::AccessFlags new_buffer_access, vk::PipelineStageFlags consuming_stages ) const {
        
        
        // Create staging buffer and map it's memory to copy data from the CPU
        
        StagingBufferParameters staging_buffer;
        
        staging_buffer.Buffer = CreateBuffer( data_size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible );
        staging_buffer.Pointer = GetDevice().mapMemory( *staging_buffer.Buffer.Memory, 0, data_size );

        std::memcpy( staging_buffer.Pointer, data, data_size );

        vk::MappedMemoryRange memory_range(
            *staging_buffer.Buffer.Memory,              // VkDeviceMemory                         memory
            0,                                          // VkDeviceSize                           offset
            VK_WHOLE_SIZE                               // VkDeviceSize                           size
        );
        GetDevice().flushMappedMemoryRanges( { memory_range } );
        GetDevice().unmapMemory( *staging_buffer.Buffer.Memory );
        

        // Allocate temporary command buffer from a temporary command pool

        vk::UniqueCommandPool command_pool = CreateCommandPool( GetGraphicsQueue().FamilyIndex, vk::CommandPoolCreateFlagBits::eTransient );
        vk::UniqueCommandBuffer command_buffer = std::move( AllocateCommandBuffers( *command_pool, vk::CommandBufferLevel::ePrimary, 1 )[0] );


        // Record command buffer which copies data from the staging buffer to the destination buffer

        command_buffer->begin( { vk::CommandBufferUsageFlagBits::eOneTimeSubmit } );

        vk::BufferMemoryBarrier pre_transfer_buffer_memory_barrier(
            current_buffer_access,                        // VkAccessFlags                          srcAccessMask
            vk::AccessFlagBits::eTransferWrite,           // VkAccessFlags                          dstAccessMask
            VK_QUEUE_FAMILY_IGNORED,                      // uint32_t                               srcQueueFamilyIndex
            VK_QUEUE_FAMILY_IGNORED,                      // uint32_t                               dstQueueFamilyIndex
            target_buffer,                                // VkBuffer                               buffer
            buffer_offset,                                // VkDeviceSize                           offset
            data_size                                     // VkDeviceSize                           size
        );
        command_buffer->pipelineBarrier( generating_stages, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags( 0 ), {}, { pre_transfer_buffer_memory_barrier }, {} );

        vk::BufferCopy buffer_copy_region(
            0,                                            // VkDeviceSize                           srcOffset
            buffer_offset,                                // VkDeviceSize                           dstOffset
            data_size                                     // VkDeviceSize                           size
        );
        command_buffer->copyBuffer( *staging_buffer.Buffer.Handle, target_buffer, { buffer_copy_region } );

        vk::BufferMemoryBarrier post_transfer_buffer_memory_barrier(
            vk::AccessFlagBits::eTransferWrite,           // VkAccessFlags                          srcAccessMask
            new_buffer_access,                            // VkAccessFlags                          dstAccessMask
            VK_QUEUE_FAMILY_IGNORED,                      // uint32_t                               srcQueueFamilyIndex
            VK_QUEUE_FAMILY_IGNORED,                      // uint32_t                               dstQueueFamilyIndex
            target_buffer,                                // VkBuffer                               buffer
            buffer_offset,                                // VkDeviceSize                           offset
            data_size                                     // VkDeviceSize                           size
        );
        command_buffer->pipelineBarrier( vk::PipelineStageFlagBits::eTransfer, consuming_stages, vk::DependencyFlags( 0 ), {}, { post_transfer_buffer_memory_barrier }, {} );
        command_buffer->end();


        // Submit
  
        vk::UniqueFence fence = CreateFence( false );
        vk::SubmitInfo submit_info(
            0,                                            // uint32_t                               waitSemaphoreCount
            nullptr,                                      // const VkSemaphore                     *pWaitSemaphores
            nullptr,                                      // const VkPipelineStageFlags            *pWaitDstStageMask
            1,                                            // uint32_t                               commandBufferCount
            &(*command_buffer)                            // const VkCommandBuffer                 *pCommandBuffers
        );
        GetGraphicsQueue().Handle.submit( { submit_info }, *fence );
        GetDevice().waitForFences( { *fence }, VK_FALSE, 3000000000 );

    }


    // Private implementation functions

    void VkPipelineBase::ImplCreateImage( uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::UniqueImage & image, vk::ImageType image_type ) const {
        vk::ImageCreateInfo image_create_info(
            vk::ImageCreateFlags( 0 ),                        // VkImageCreateFlags             flags
            image_type, //vk::ImageType::e2D,                               // VkImageType                    imageType
            format,                                           // VkFormat                       format
            {                                                 // VkExtent3D                     extent
                width,                                            // uint32_t                       width
                height,                                           // uint32_t                       height
                1                                                 // uint32_t                       depth
            },
            1,                                                // uint32_t                       mipLevels
            1,                                                // uint32_t                       arrayLayers
            vk::SampleCountFlagBits::e1,                      // VkSampleCountFlagBits          samples
            vk::ImageTiling::eOptimal,                        // VkImageTiling                  tiling
            usage                                             // VkImageUsageFlags              usage
        );
        image = GetDevice().createImageUnique( image_create_info );
    }

    void VkPipelineBase::ImplAllocateImageMemory( vk::Image & image, vk::MemoryPropertyFlagBits property, vk::UniqueDeviceMemory & memory ) const {
        vk::MemoryRequirements image_memory_requirements = GetDevice().getImageMemoryRequirements( image );
        vk::PhysicalDeviceMemoryProperties memory_properties = GetPhysicalDevice().getMemoryProperties();

        for( uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i ) {
            if( (image_memory_requirements.memoryTypeBits & (1 << i)) && ((memory_properties.memoryTypes[i].propertyFlags & property) == property) ) {
                try {
                    memory = GetDevice().allocateMemoryUnique( { image_memory_requirements.size, i } );
                    return;
                } catch( ... ) {
                    // Iterate over all supported memory types; only if none of them could be used, throw exception
                }
            }
        }
        throw std::runtime_error( "Could not allocate a memory for an image!" );
    }

    void VkPipelineBase::ImplCreateImageView( vk::Image & image, vk::Format format, vk::ImageAspectFlags aspect, vk::UniqueImageView & image_view, vk::ImageViewType image_view_type ) const {
        vk::ImageViewCreateInfo image_view_create_info(
            vk::ImageViewCreateFlags( 0 ),                    // VkImageViewCreateFlags         flags
            image,                                            // VkImage                        image
            image_view_type, //vk::ImageViewType::e2D,                           // VkImageViewType                viewType
            format,                                           // VkFormat                       format
            vk::ComponentMapping(),                           // VkComponentMapping             components
            {                                                 // VkImageSubresourceRange        subresourceRange
                aspect,                                           // VkImageAspectFlags             aspectMask
                0,                                                // uint32_t                       baseMipLevel
                1,                                                // uint32_t                       levelCount
                0,                                                // uint32_t                       baseArrayLayer
                1                                                 // uint32_t                       layerCount
            }
        );
        image_view = GetDevice().createImageViewUnique( image_view_create_info );
    }

    void VkPipelineBase::ImplCreateBuffer( uint32_t size, vk::BufferUsageFlags usage, vk::UniqueBuffer & buffer ) const {
        vk::BufferCreateInfo buffer_create_info(
            vk::BufferCreateFlags( 0 ),                           // VkBufferCreateFlags            flags
            size,                                                 // VkDeviceSize                   size
            usage                                                 // VkBufferUsageFlags             usage
        );
        buffer = GetDevice().createBufferUnique( buffer_create_info );
    }

    void VkPipelineBase::ImplAllocateBufferMemory( vk::Buffer & buffer, vk::MemoryPropertyFlagBits property, vk::UniqueDeviceMemory & memory ) const {
        vk::MemoryRequirements buffer_memory_requirements = GetDevice().getBufferMemoryRequirements( buffer );
        vk::PhysicalDeviceMemoryProperties memory_properties = GetPhysicalDevice().getMemoryProperties();
        for( uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i ) {
            if( (buffer_memory_requirements.memoryTypeBits & (1 << i)) && ((memory_properties.memoryTypes[i].propertyFlags & property) == property) ) {
                try {
                    memory = GetDevice().allocateMemoryUnique( { buffer_memory_requirements.size, i } );
                    return;
                } catch( ... ) {
                    // Iterate over all supported memory types; only if none of them could be used, throw exception
                }
            }
        }
        throw std::runtime_error( "Could not allocate a memory for a buffer!" );
    }

    void VkPipelineBase::ImplCreateDescriptorSetLayout( std::vector<vk::DescriptorSetLayoutBinding> const & layout_bindings, vk::UniqueDescriptorSetLayout & set_layout ) const {
        vk::DescriptorSetLayoutCreateInfo descriptor_set_layout_create_info( 
            vk::DescriptorSetLayoutCreateFlags( 0 ),              // VkDescriptorSetLayoutCreateFlags     flags
            static_cast<uint32_t>(layout_bindings.size()),        // uint32_t                             bindingCount
            layout_bindings.data()                                // const VkDescriptorSetLayoutBinding  *pBindings
        );
        set_layout = GetDevice().createDescriptorSetLayoutUnique( descriptor_set_layout_create_info );
    }

    void VkPipelineBase::ImplCreateDescriptorPool( std::vector<vk::DescriptorPoolSize> const & pool_sizes, uint32_t max_sets, vk::UniqueDescriptorPool & descriptor_pool ) const {
        vk::DescriptorPoolCreateInfo descriptor_pool_create_info(
            vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, // VkDescriptorPoolCreateFlags          flags
            max_sets,                                             // uint32_t                             maxSets
            static_cast<uint32_t>(pool_sizes.size()),             // uint32_t                             poolSizeCount
            pool_sizes.data()                                     // const VkDescriptorPoolSize          *pPoolSizes
        );
        descriptor_pool = GetDevice().createDescriptorPoolUnique( descriptor_pool_create_info );
    }

    void VkPipelineBase::ImplAllocateDescriptorSets( std::vector<vk::DescriptorSetLayout> const & descriptor_set_layout, vk::DescriptorPool & descriptor_pool, std::vector<vk::UniqueDescriptorSet> & descriptor_sets ) const {
        vk::DescriptorSetAllocateInfo descriptor_set_allocate_info(
            descriptor_pool,                                      // VkDescriptorPool                     descriptorPool
            static_cast<uint32_t>(descriptor_set_layout.size()),  // uint32_t                             descriptorSetCount
            descriptor_set_layout.data()                          // const VkDescriptorSetLayout   *pSetLayouts
        );
        descriptor_sets = GetDevice().allocateDescriptorSetsUnique( descriptor_set_allocate_info );
    }


} // end namespace AppCore


#endif  // C_VKPIPELINEBASE_CPP