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


#ifndef C_VKPIPELINEBASE_H
#define C_VKPIPELINEBASE_H


#include "c_VulkanBase.h"
#include "ch_Tools.h"
#include "c_AppDataIO.h"

namespace AppCore {

    // ************************************************************ //
    // FrameResourcesData                                           //
    //                                                              //
    // Struct containing data used to generate a single frame       //
    // ************************************************************ //
    struct FrameResourcesData {

        ImageParameters                       DepthAttachment;
        vk::UniqueFramebuffer                 Framebuffer;
        vk::UniqueSemaphore                   ImageAvailableSemaphore;
        vk::UniqueSemaphore                   FinishedRenderingSemaphore;
        vk::UniqueFence                       Fence;
        vk::UniqueCommandPool                 CommandPool;
        vector<vk::UniqueCommandBuffer>       CommandBufferList;

        FrameResourcesData() :
            DepthAttachment(),
            Framebuffer(),
            ImageAvailableSemaphore(),
            FinishedRenderingSemaphore(),
            Fence(),
            CommandPool(),
            CommandBufferList() {
        }

        virtual ~FrameResourcesData() {
        }

    };

    // ************************************************************ //
    // CurrentFrameData                                             //
    //                                                              //
    // Struct for managing frame rendering process                  //
    // ************************************************************ //
    struct CurrentFrameData {
        uint32_t                    ResourceIndex;
        uint32_t                    ResourceCount;
        FrameResourcesData         *FrameResources;
        const SwapChainParameters  *Swapchain;
        uint32_t                    SwapchainImageIndex;
    };

    // ************************************************************ //
    // RenderPassAttachmentData                                     //
    //                                                              //
    // Struct simplifying render pass creation                      //
    // ************************************************************ //
    struct RenderPassAttachmentData {
        vk::Format            Format;
        vk::AttachmentLoadOp  LoadOp;
        vk::AttachmentStoreOp StoreOp;
        vk::ImageLayout       InitialLayout;
        vk::ImageLayout       FinalLayout;
    };

    // ************************************************************ //
    // RenderPassSubpassData                                        //
    //                                                              //
    // Struct simplifying render pass creation                      //
    // ************************************************************ //
    struct RenderPassSubpassData {
        std::vector<vk::AttachmentReference> const  InputAttachments;
        std::vector<vk::AttachmentReference> const  ColorAttachments;
        vk::AttachmentReference const               DepthStencilAttachment;
    };

    // ************************************************************ //
    // VkPipelineBase Class                                         //
    //                                                              //
    // Base class for pipelines                                     //
    // ************************************************************ //
    class VkPipelineBase : public VulkanBase {

    public:

        VkPipelineBase();
        virtual ~VkPipelineBase();

        static const vk::Format     DefaultDepthFormat = vk::Format::eD16Unorm;           // VK_FORMAT_D16_UNORM specifies a one-component, 16-bit unsigned normalized format that has a single 16-bit depth component

        virtual void                InitPipelines( WindowParameters window );

        // Resource creation methods
        vk::UniqueShaderModule                CreateShaderModule( char const * filename ) const;
        ImageParameters                       CreateImage( uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::MemoryPropertyFlagBits property, vk::ImageAspectFlags aspect, vk::ImageType image_type = vk::ImageType::e2D, vk::ImageViewType image_view_type = vk::ImageViewType::e2D ) const;
        BufferParameters                      CreateBuffer( uint32_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlagBits memoryProperty ) const;
        DescriptorSetParameters               CreateDescriptorResources( std::vector<vk::DescriptorSetLayoutBinding> const & layout_bindings, std::vector<vk::DescriptorPoolSize> const & pool_sizes ) const;
        vk::UniqueFramebuffer                 CreateFramebuffer( std::vector<vk::ImageView> const & image_views, vk::Extent2D const & extent, vk::RenderPass const & render_pass ) const;
        vk::UniqueSampler                     CreateSampler( vk::SamplerMipmapMode mipmap_mode, vk::SamplerAddressMode address_mode, vk::Bool32 unnormalized_coords ) const;
        vk::UniqueRenderPass                  CreateRenderPass( std::vector<RenderPassAttachmentData> const & attachment_descriptions, std::vector<RenderPassSubpassData> const & subpass_descriptions, std::vector<vk::SubpassDependency> const & dependencies ) const;
        vk::UniquePipelineLayout              CreatePipelineLayout( std::vector<vk::DescriptorSetLayout> const & descriptor_set_layouts, std::vector<vk::PushConstantRange> const & push_constant_ranges ) const;
        vk::UniqueSemaphore                   CreateSemaphore() const;
        vk::UniqueFence                       CreateFence( bool signaled ) const;
        vk::UniqueCommandPool                 CreateCommandPool( uint32_t queue_family_index, vk::CommandPoolCreateFlags flags ) const;
        std::vector<vk::UniqueCommandBuffer>  AllocateCommandBuffers( vk::CommandPool const & pool, vk::CommandBufferLevel level, uint32_t count ) const;

        // Resource tools
        void                UpdateDescriptorSet( vk::DescriptorSet & descriptor_set, vk::DescriptorType descriptor_type, uint32_t binding, uint32_t array_element, std::vector<vk::DescriptorImageInfo> const & image_infos = {}, std::vector<vk::DescriptorBufferInfo> const & buffer_infos = {}, std::vector<vk::BufferView> const & buffer_views = {} ) const;
        void                SetImageMemoryBarrier( vk::Image const & image, vk::ImageSubresourceRange const & image_subresource_range, vk::ImageLayout current_image_layout, vk::AccessFlags current_image_access, vk::PipelineStageFlags generating_stages, vk::ImageLayout new_image_layout, vk::AccessFlags new_image_access, vk::PipelineStageFlags consuming_stages ) const;
        void                CopyDataToImage( uint32_t data_size, void const * data, vk::Image & target_image, uint32_t width, uint32_t height, vk::ImageSubresourceRange const & image_subresource_range, vk::ImageLayout current_image_layout, vk::AccessFlags current_image_access, vk::PipelineStageFlags generating_stages, vk::ImageLayout new_image_layout, vk::AccessFlags new_image_access, vk::PipelineStageFlags consuming_stages ) const;
        void                CopyDataToBuffer( uint32_t data_size, void const * data, vk::Buffer target_buffer, vk::DeviceSize buffer_offset, vk::AccessFlags current_buffer_access, vk::PipelineStageFlags generating_stages, vk::AccessFlags new_buffer_access, vk::PipelineStageFlags consuming_stages ) const;

        std::vector<std::unique_ptr<FrameResourcesData>>  FrameResources;

    private:
    
        void                        ImplCreateImage( uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::UniqueImage & image, vk::ImageType image_type ) const;
        void                        ImplAllocateImageMemory( vk::Image & image, vk::MemoryPropertyFlagBits property, vk::UniqueDeviceMemory & memory ) const;
        void                        ImplCreateImageView( vk::Image & image, vk::Format format, vk::ImageAspectFlags aspect, vk::UniqueImageView & image_view, vk::ImageViewType image_view_type ) const;
        void                        ImplCreateBuffer( uint32_t size, vk::BufferUsageFlags usage, vk::UniqueBuffer & buffer ) const;
        void                        ImplAllocateBufferMemory( vk::Buffer & buffer, vk::MemoryPropertyFlagBits property, vk::UniqueDeviceMemory & memory ) const;
        void                        ImplCreateDescriptorSetLayout( std::vector<vk::DescriptorSetLayoutBinding> const & layout_bindings, vk::UniqueDescriptorSetLayout & set_layout ) const;
        void                        ImplCreateDescriptorPool( std::vector<vk::DescriptorPoolSize> const & pool_sizes, uint32_t max_sets, vk::UniqueDescriptorPool & descriptor_pool ) const;
        void                        ImplAllocateDescriptorSets( std::vector<vk::DescriptorSetLayout> const & descriptor_set_layout, vk::DescriptorPool & descriptor_pool, std::vector<vk::UniqueDescriptorSet> & descriptor_sets ) const;

    };

} // end namespace AppCore


#endif  // C_VKPIPELINEBASE_H