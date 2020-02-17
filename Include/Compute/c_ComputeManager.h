#ifndef C_COMPUTEMANAGER_H
#define C_COMPUTEMANAGER_H

#include "c_COM_PipelineBase.h"
#include "user_Includes.h"

using namespace std;

namespace AppCore {

    // ************************************************************ //
    // ComputeManager Class                                         //
    //                                                              //
    // Holds all Layouts used                                       //
    // ************************************************************ //
    class ComputeManager {

    public:

        ComputeManager( PipelineManager &parent );
        ~ComputeManager();

        void                    Init();
        bool                    Execute( string inName, ComputeResourcesData * compute_resource );

        COM_PipelineBase *      GetCOM( string inName );
        COM_PipelineBase *      GetCOM( int inIndex );
        int                     GetCOMIndex( string inName );

    private:

        PipelineManager &               Parent;
        AppDataIO &                     AppData;

        map<string, int>                COMMap;
        vector<COM_PipelineBase*>       COMObjects;         // Objects derived from COM_PipelineBase
        
        void                    InitSolvers();              // This is in user_Solvers.cpp
        void                    InitPipelines();
    
        // Template methods
        template <typename T> void AddCOM( string inName ) {
            auto iter = COMMap.find( inName );
            if ( iter != COMMap.end() ) {
                std::cout << "Assert: COM map already has object with name " << iter->first << std::endl;
                assert( iter == COMMap.end() );
            } else {
                T *obj = new T( Parent );
                obj->Name = inName;
                COMObjects.push_back( obj );
                COMMap.insert( pair<string, int>( inName, (int) COMObjects.size() - 1) );
            }
        }

    };

} // end namespace AppCore
#endif // C_COMPUTEMANAGER_H