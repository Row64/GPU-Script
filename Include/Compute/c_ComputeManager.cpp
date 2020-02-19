#ifndef C_COMPUTEMANAGER_CPP
#define C_COMPUTEMANAGER_CPP

#include "c_PipelineManager.h"

using namespace std;

namespace AppCore {

    ComputeManager::ComputeManager( PipelineManager &parent ) :
        Parent( parent ),
        AppData( parent.GetAppData() ) {
    }

    ComputeManager::~ComputeManager() {
        for ( int i = 0; i < COMObjects.size(); ++i ) {
            COMObjects[i]->~COM_PipelineBase();
        }
    }

    void ComputeManager::Init() {
        InitSolvers();
        InitPipelines();
    }

    bool ComputeManager::Execute( string inName, ComputeResourcesData * compute_resource ) {
        int index = GetCOMIndex(inName);
        COM_PipelineBase * current_COM = COMObjects[index];
        return current_COM->Execute( compute_resource );
    }

    COM_PipelineBase * ComputeManager::GetCOM( string inName ) {
        auto iter = COMMap.find( inName );
        if ( iter == COMMap.end() ) {
            std::cout << "SUI Object not found: " << inName << std::endl;
            assert( iter != COMMap.end() );
        }
        return COMObjects[iter->second];
    }

    COM_PipelineBase * ComputeManager::GetCOM( int inIndex ) {
        return COMObjects[inIndex];
    }

    int ComputeManager::GetCOMIndex( string inName ) {
        auto iter = COMMap.find( inName );
        if ( iter == COMMap.end() ) {
            std::cout << "SUI Object not found: " << inName << std::endl;
            assert( iter != COMMap.end() );
        }
        return iter->second;
    }

    // Private 

    void ComputeManager::InitPipelines( ) {
        for ( int i = 0; i < COMObjects.size(); ++i ) {
            COMObjects[i]->Init();
        }
    }

} // end namespace AppCore
#endif // C_COMPUTEMANAGER_CPP