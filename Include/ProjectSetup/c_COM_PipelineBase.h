#ifndef C_COM_PIPELINEBASE_H
#define C_COM_PIPELINEBASE_H

#include "c_VkPipelineBase.h"

using namespace std;


namespace AppCore {

    class PipelineManager;

    // ************************************************************ //
    // COM_PipelineData                                             //
    // ************************************************************ //
    struct COM_PipelineData {

        std::string                                 shader_comp_path = "Shaders/shader.comp.spv";

        vector<BufferParameters>                    StorageBuffers;
    
        DescriptorSetParameters                     DescriptorSet;
        vk::UniquePipelineLayout                    PipelineLayout;
        vk::UniquePipeline                          ComputePipeline;
        

        COM_PipelineData() :
            StorageBuffers(),
            DescriptorSet(),
            PipelineLayout(),
            ComputePipeline(){
        }
    };


    // ************************************************************ //
    // COM_PipelineBase Class                                            //
    // ************************************************************ //
    class COM_PipelineBase {

    public:

        COM_PipelineBase( PipelineManager &parent );
        virtual ~COM_PipelineBase();
        
        void                    Init();
        virtual bool            Execute( ComputeResourcesData * compute_resource );
        virtual void            PreSubmit();
        virtual void            SubmitComplete();
        
        COM_PipelineData &      GetParameters();
        std::string             Name = "";
        bool                    IsRunning = false;

    protected:

        COM_PipelineData        Parameters;
        PipelineManager &       Parent;
        AppDataIO &             AppData;
        glm::vec3               NbWorkgroups = {2, 1, 1};
        

        // Init methods
        virtual void            CreateStorageBuffers();
        virtual void            CreateDescriptorSet();
        virtual void            CreatePipelineLayout();
        virtual void            CreateComputePipeline();

        // Execute methods
        virtual void            ProcessInputs();
        virtual void            RecordCommandBuffer( vk::CommandBuffer & command_buffer );
        virtual void            SubmitCommandBuffer( ComputeResourcesData & compute_resource , vk::CommandBuffer & command_buffer );
        virtual void            ProcessOutputs();
        
        

    };

} // end namespace AppCore

#endif // C_COM_PIPELINEBASE_H