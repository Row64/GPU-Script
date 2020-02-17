#ifndef COM_MANDELBROT_CPP
#define COM_MANDELBROT_CPP

#include "c_PipelineManager.h"
#include "lodepng.h"

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
        Parameters.StorageBuffers[0].Size = sizeof(Pixel) * WIDTH * HEIGHT;
        Parameters.StorageBuffers[0] = Parent.CreateBuffer( Parameters.StorageBuffers[0].Size , vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible );

    }

    void COM_Mandelbrot::ProcessInputs() {
    }


    void COM_Mandelbrot::ProcessOutputs() {

        // Code modified from Vulkan Minimal Compute example
        // https://github.com/Erkaman/vulkan_minimal_compute
        // Copyright (c) 2017 Eric Arnebäck
        // The MIT License (MIT)

        std::cout << "--- COM_Mandelbrot::SubmitComplete     Mapping back memory" << std::endl;

        void* mappedMemory = NULL;
        // Map the buffer memory, so that we can read from it on the CPU.
        Parent.GetDevice().mapMemory( *Parameters.StorageBuffers[0].Memory, 0, Parameters.StorageBuffers[0].Size, {}, &mappedMemory);
        Pixel* pmappedMemory = (Pixel *)mappedMemory;

        // Get the color data from the buffer, and cast it to bytes.
        // We save the data to a vector.
        std::vector<unsigned char> image;
        image.reserve(WIDTH * HEIGHT * 4);
        for (int i = 0; i < WIDTH*HEIGHT; i += 1) {
            image.emplace_back((unsigned char)(255.0f * (pmappedMemory[i].r)));
            image.emplace_back((unsigned char)(255.0f * (pmappedMemory[i].g)));
            image.emplace_back((unsigned char)(255.0f * (pmappedMemory[i].b)));
            image.emplace_back((unsigned char)(255.0f * (pmappedMemory[i].a)));
        }

        // Done reading, so unmap.
        Parent.GetDevice().unmapMemory( *Parameters.StorageBuffers[0].Memory );

        // Now we save the acquired color data to a .png.
        fs::path save_path = AppData.RootPath / "mandelbrot.png";
        unsigned error = lodepng::encode(CONVERT_PATH(save_path), image, WIDTH, HEIGHT);
        if (error) printf("encoder error %d: %s", error, lodepng_error_text(error));

        std::cout << "--- COM_Mandelbrot::SubmitComplete     Finished" << std::endl;
    }

} // end namespace AppCore
#endif // COM_MANDELBROT_CPP