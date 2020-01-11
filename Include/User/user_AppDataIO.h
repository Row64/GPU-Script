#ifndef USER_APPDATAIO_H
#define USER_APPDATAIO_H


#include <string>
using namespace std;

#include <boost/filesystem.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
namespace fs = boost::filesystem;

#ifndef CONVERT_PATH
    #if defined ( __APPLE__)
        #define CONVERT_PATH( pathvar ) pathvar.c_str()
    #elif defined ( _WIN32 )
        #define CONVERT_PATH( pathvar ) pathvar.string().c_str()
    #else
        #define CONVERT_PATH( pathvar ) pathvar.c_str()
    #endif
#endif

namespace AppCore {

    // ************************************************************ //
    // AppDataIO Class                                              //
    //                                                              //
    // Global data and IO communication between windows and panes   //
    // ************************************************************ //
    class AppDataIO {

    public:
    
        AppDataIO( char* inPath ) :
            AppPath( (fs::path) inPath ) {
                RootPath = AppPath.parent_path();
        }

        // Paths
        fs::path                AppPath;
        fs::path                RootPath;

        int                     Current_Tab = 0;

        // Ocean Demo
        float                   OceanSpeed = 1.0f;

        // Ice Demo
        float                   Roughness = 0.5f;
        float                   IsoValue = 0.5f;
        float                   IceColor = 0.12f;
        float                   Refraction = 0.4f;

    };
    
} // end namespace AppCore

#endif // USER_APPDATAIO_H