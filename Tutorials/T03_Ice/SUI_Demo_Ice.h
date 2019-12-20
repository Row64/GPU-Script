#ifndef SUI_DEMO_ICE_H
#define SUI_DEMO_ICE_H

#include "c_SUI_PipelineBase.h"


namespace AppCore { 

    // ************************************************************ //
    // IceUboData                                                   //
    //                                                              //
    // Struct describing uniform buffer data passed to shader       //
    // ************************************************************ //
    struct IceUboData {

        alignas(4) float uTime;
        alignas(4) float mouseX;
        alignas(4) float mouseY;
        alignas(4) float mDown;
        alignas(16) glm::vec4 color;
        alignas(16) glm::vec4 dim;
        
        alignas(4) float roughness;
        alignas(4) float isoval;
        alignas(4) float icecol;
        alignas(4) float refraction;

    };


    // ************************************************************ //
    // IceSUI Class                                                 //
    // ************************************************************ //
    class IceSUI : virtual public SUI_PipelineBase {

    public:

        IceSUI( PipelineManager &parent );
        virtual ~IceSUI();

    private:

        // Overrided Draw methods
        void            UpdateUniformBuffer( SUI_PipelineData::DrawResourcesData & drawing_resources ) override final;

    };

} // end namespace AppCore

#endif // SUI_DEMO_ICE_H