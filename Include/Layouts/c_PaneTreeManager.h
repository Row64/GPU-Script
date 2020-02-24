#ifndef C_PANETREEMANAGER_H
#define C_PANETREEMANAGER_H

#include "c_SUI_PipelineBase.h"
#include "c_TUI_PipelineBase.h"
#include "c_PaneTree.h"

using namespace std;

namespace AppCore {

    class PipelineManager;

    // ************************************************************ //
    // PaneTreeManager Class                                        //
    //                                                              //
    // Stores and manipulates PaneTrees                             //
    // ************************************************************ //
    class PaneTreeManager {

    public:

        PaneTreeManager( PipelineManager &parent );
        ~PaneTreeManager();

        // Methods acting on Objects
        int                         GetPaneType( string inName );
        SUI_PipelineBase *          GetSUI( string inName );
        TUI_PaneBase *              GetTUI( string inName );        
        
        TUI_PipelineBase &          GetTUIPipeline();
        void                        InitPipelines( size_t resource_count );

        // Methods acting on PaneTrees
        void                        AddPaneTree( string inPTName, bool inPoll, vector<vector<string>> inPTVec );
        PaneTree*                   GetPaneTree( string inPTName );
        PaneTree*                   GetPaneTree( int inIndex );
        vector<PaneTree*> &         GetAllPaneTrees();

        vector<SUI_PipelineBase*>   GetSUIVector( PaneTree *inPT );
        vector<TUI_PaneBase*>       GetTUIVector( PaneTree *inPT );
        void                        GetDimensions(PaneTree *inPT, array<int,2> *inWinSize);
        void                        UpdateDimensions( PaneTree *inPT );
        void                        UpdateModes( PaneTree *inPT ) ;
    private:

        PipelineManager &           Parent;
        AppDataIO &                 AppData;

        map<string, int>            TypeMap;            // TUI = 0, SUI = 1

        map<string, int>            SUIMap;
        vector<SUI_PipelineBase*>   SUIObjects;         // Objects derived from SUI_Pipelinebase

        TUI_PipelineBase            TUIPipeline;
        map<string, int>            TUIMap;
        vector<TUI_PaneBase*>       TUIObjects;         // Objects derived from TUI_PaneBase

        map<string, int>            PTMap;
        vector<PaneTree*>           PTObjects;          // PaneTree objects

        void                        DimensionCrawl(int inIndex, PaneTree *inPT);
        void                        CrawlFloatingPane(PaneNode* inParent, PaneNode * inChild);
        bool                        IsNum(const std::string& s);
        int                         LengthFromPerc(string inPercExp, int inLenght);
        int                         StringToInt(string inStr);
        int                         ReverseNegative(string inCord, int inLength);

    public:

        // Template methods
        template <typename T> void AddSUI( string inName ) {
            auto iter = TypeMap.find( inName );
            if ( iter != TypeMap.end() ) {
                std::cout << "Assert: PaneTreeManager map already has object with name " << iter->first << std::endl;
                assert( iter == TypeMap.end() );
            } else {
                T *obj = new T( Parent );
                SUIObjects.push_back( obj );
                obj->PaneName = inName;
                SUIMap.insert( pair<string, int>( inName, (int) SUIObjects.size() - 1) );
                TypeMap.insert( pair<string, int>( inName, 1 ) );
            }
        }

        template <typename T> void AddTUI( string inName ) {
            auto iter = TypeMap.find( inName );
            if ( iter != TypeMap.end() ) {
                std::cout << "Assert: PaneTreeManager map already has object with name " << iter->first << std::endl;
                assert( iter == TypeMap.end() );
            } else {
                T *obj = new T( Parent );
                TUIObjects.push_back( obj );
                obj->PaneName = inName;
                TUIMap.insert( pair<string, int>( inName, (int) TUIObjects.size() - 1) );
                TypeMap.insert( pair<string, int>( inName, 0 ) );
            }
        }
        
    };

} // end namespace AppCore


#endif /* C_PANETREEMANAGER_H */