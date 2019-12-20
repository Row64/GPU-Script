#ifndef SUI_DEMO_OCEAN_CPP
#define SUI_DEMO_OCEAN_CPP

#include "c_PipelineManager.h"

namespace AppCore { 
    
    OceanSUI::OceanSUI( PipelineManager &parent ) :
        SUI_PipelineBase( parent ) {
        Parameters.shader_vert_path = "Shaders/ocean.vert.spv";
        Parameters.shader_frag_path = "Shaders/ocean.frag.spv";
        PushConstants = { &AppData.OceanSpeed };                    // Must be float type
    }

    OceanSUI::~OceanSUI() {
    }


} // end namespace AppCore

#endif // SUI_DEMO_OCEAN_CPP