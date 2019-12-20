#ifndef SUI_DEMO_PARTICLES_H
#define SUI_DEMO_PARTICLES_H

#include "c_SUI_PipelineBase.h"

namespace AppCore { 

    // ************************************************************ //
    // ParticlesSUI Class                                           //
    // ************************************************************ //
    class ParticlesSUI : virtual public SUI_PipelineBase {

    public:

        ParticlesSUI( PipelineManager &parent );
        virtual ~ParticlesSUI();

    };

}

#endif // SUI_DEMO_PARTICLES_H