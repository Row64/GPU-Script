#ifndef USER_SOLVERS_CPP
#define USER_SOLVERS_CPP

#include "c_PipelineManager.h"

namespace AppCore {

    void ComputeManager::InitSolvers() {
        
        AddCOM<COM_PipelineBase> ( "Basic Compute" );
        AddCOM<COM_Mandelbrot> ( "Mandelbrot" );
    
    }
} // end namespace AppCore

#endif // USER_SOLVERS_CPP