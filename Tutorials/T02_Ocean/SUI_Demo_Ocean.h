#ifndef SUI_DEMO_OCEAN_H
#define SUI_DEMO_OCEAN_H

#include "c_SUI_PipelineBase.h"

namespace AppCore { 

    // ************************************************************ //
    // OceanSUI Class                                               //
    //                                                              //
    // SUI Pipeline for Ocean Demo                                  //
    // ************************************************************ //
    class OceanSUI : virtual public SUI_PipelineBase {

    public:
        
        OceanSUI( PipelineManager &parent );
        virtual ~OceanSUI();

    };

} // end namespace AppCore

#endif // SUI_DEMO_OCEAN_H