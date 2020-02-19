#ifndef COM_MANDELBROT_H
#define COM_MANDELBROT_H

#include "c_COM_PipelineBase.h"

using namespace std;

const int WIDTH = 3200; // Size of rendered mandelbrot set.
const int HEIGHT = 2400; // Size of renderered mandelbrot set.
const int WORKGROUP_SIZE = 32; // Workgroup size in compute shader.

namespace AppCore {

    // ************************************************************ //
    // Pixel Struct                                                 //
    // The pixels of the rendered mandelbrot set are in this format //
    // ************************************************************ //
    struct Pixel {
        float r, g, b, a;
    };

    // ************************************************************ //
    // COM_Mandelbrot Class                                         //
    // ************************************************************ //

    class COM_Mandelbrot : virtual public COM_PipelineBase {
    
    public:

        COM_Mandelbrot( PipelineManager &parent );
        virtual ~COM_Mandelbrot();

    protected:

        void            CreateStorageBuffers() override final;
        void            ProcessInputs()  override final;
        void            ProcessOutputs()  override final;

    };

} // end namespace AppCore

#endif // COM_MANDELBROT_H