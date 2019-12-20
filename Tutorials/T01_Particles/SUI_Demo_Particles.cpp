#ifndef SUI_DEMO_PARTICLES_CPP
#define SUI_DEMO_PARTICLES_CPP

#include "c_PipelineManager.h"

namespace AppCore { 
    
    ParticlesSUI::ParticlesSUI( PipelineManager &parent ) :
        SUI_PipelineBase( parent ) {
        Parameters.shader_vert_path = "Shaders/particles.vert.spv";
        Parameters.shader_frag_path = "Shaders/particles.frag.spv";
    }

    ParticlesSUI::~ParticlesSUI() {
    }

}

#endif // SUI_DEMO_PARTICLES_CPP