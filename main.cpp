
#include "c_PipelineManager.h"
#include "user_AppDataIO.h"

#include <boost/filesystem.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
namespace fs = boost::filesystem;

// http://www.cplusplus.com/forum/windows/127216/
#include <iostream>
#include <fstream>

class RedirectStdOutput {
    public:
        RedirectStdOutput(std::ofstream& file)
            : _psbuf{file.rdbuf()}, _backup{std::cout.rdbuf()}
        {
            std::cout.rdbuf(_psbuf);
        }

        ~RedirectStdOutput() {
            std::cout.rdbuf(_backup);
        }

    private:
        std::streambuf* _psbuf;
        std::streambuf* _backup;
};

int main( int argc, char **argv ) {

    fs::path log_path = ((fs::path) argv[0]).parent_path() / "Log.txt";
	std::ofstream file (log_path.string().c_str());
    
    // Uncomment the next line for cmd and error logging to go into into to Log.txt
    // RedirectStdOutput rso (file);
    

    AppCore::AppDataIO app_data ( argv[0] );
    AppCore::Window window(&app_data);
	AppCore::PipelineManager sample ("GPU Script Example App", app_data);
    

try {
	// Window creation
    window.Create( "GPU Script Example App", 1000, 800 );

	// Vulkan preparations and initialization
	sample.InitPipelines( window.GetParameters() );

	// Rendering loop - diplays window
	window.RenderingLoop( sample );

} catch( std::exception &exception ) {

	std::cout << exception.what() << std::endl;

	return -1;
}

return 0;
}
