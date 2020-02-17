#ifndef COM_MANDELBROT_CPP
#define COM_MANDELBROT_CPP

#include "c_PipelineManager.h"

using namespace std;

namespace AppCore {

    COM_Mandelbrot::COM_Mandelbrot( PipelineManager &parent ) :
        COM_PipelineBase( parent ) {
        Parameters.shader_comp_path = "Shaders/COM_Mandelbrot.comp.spv";
        NbWorkgroups = {ceil(WIDTH / float(WORKGROUP_SIZE)), ceil(HEIGHT / float(WORKGROUP_SIZE)), 1};
    }

    COM_Mandelbrot::~COM_Mandelbrot() {
    }

    void COM_Mandelbrot::CreateStorageBuffers() {

        Parameters.StorageBuffers.resize(1);

        // Output buffer
        Parameters.StorageBuffers[0].Size = sizeof(float);
        Parameters.StorageBuffers[0] = Parent.CreateBuffer( Parameters.StorageBuffers[0].Size , vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible );

    }

    void COM_Mandelbrot::ProcessInputs() {
    }


    void COM_Mandelbrot::ProcessOutputs() {

        std::cout << "--- COM_Mandelbrot::SubmitComplete     Mapping back memory" << std::endl;

        void* mappedMemory = NULL;
        // Map the buffer memory, so that we can read from it on the CPU.
        Parent.GetDevice().mapMemory( *Parameters.StorageBuffers[0].Memory, 0, Parameters.StorageBuffers[0].Size, {}, &mappedMemory);
        Pixel* pmappedMemory = (Pixel *)mappedMemory;

        // Done reading, so unmap.
        Parent.GetDevice().unmapMemory( *Parameters.StorageBuffers[0].Memory );

        std::cout << "--- COM_Mandelbrot::SubmitComplete     Finished" << std::endl;
    }

} // end namespace AppCore
#endif // COM_MANDELBROT_CPP