#ifndef USER_LAYOUTS_CPP
#define USER_LAYOUTS_CPP

#include "c_PipelineManager.h"

namespace AppCore {


    void LayoutManager::InitLayouts() {

        PTM.AddSUI<UnderlayUI> ( "UnderUI" );
        PTM.AddTUI<ExampleLibraryUI> ( "ExampleLib" );

        // PTM.AddSUI<ParticlesSUI> ( "ParticlesShader" );

        // PTM.AddSUI<OceanSUI> ( "OceanShader" );
        // PTM.AddTUI<OceanTUI> ( "OceanSlider" );

        // PTM.AddSUI<IceSUI> ( "IceShader" );
        // PTM.AddTUI<IceTUI> ( "IceSliders" );
        
        
        // =============PaneTreeName====LazyUpdate===Name=============ParentName======WidthExp=====HeightExp===Split=======Mode====FloatX==FloatY===  
        PTM.AddPaneTree( "Basic Example", 0,    {   {"Main",           "",              "*",        "*",        "y",        "",     "",     ""     },
                                                    {"UnderUI",        "Main",          "",        "140",       "",         "",     "",     ""     },
                                                    {"ExampleLib",     "Main",          "*",        "*",        "",         "",     "",     ""     }
        });

        
        /*
        // =============PaneTreeName====LazyUpdate===Name=============ParentName======WidthExp=====HeightExp===Split=======Mode====FloatX==FloatY=== 
        PTM.AddPaneTree( "Particles Example", 1,{   {"Main",            "",              "*",        "*",        "y",        "",     "",     ""     },
                                                    {"ParticlesShader", "Main",          "*",        "*",        "y",        "",     "",     ""     }
        });
        */

        /* 
        // =============PaneTreeName====LazyUpdate===Name=============ParentName======WidthExp=====HeightExp===Split=======Mode====FloatX==FloatY=== 
        PTM.AddPaneTree( "Ocean Example",  1,    {  {"Main",            "",              "*",        "*",        "y",        "",     "",     ""     },
                                                    {"OceanSlider",     "Main",          "",         "90",       "",        "",     "",     ""     },
                                                    {"OceanShader",     "Main",          "*",        "*",        "",        "",     "",     ""     }
        });
        */

        /* 
        // =============PaneTreeName====LazyUpdate===Name=============ParentName======WidthExp=====HeightExp===Split=======Mode====FloatX==FloatY=== 
        PTM.AddPaneTree( "Ice Example", 1,      {   {"Main",            "",              "*",        "*",        "y",        "",     "",     ""     },
                                                    {"IceShader",       "Main",          "*",        "*",        "",         "",     "",     ""     },
                                                    {"IceSliders",      "Main",          "300",      "120",       "",        "",    "-20",  "-20"   }
        });
        */

        SetCurrentLayout( "Basic Example" );      // Or you can set by index of Pane Tree:   SetCurrentLayout( 0 )
    }
    
    void LayoutManager::InitStyles() {
        
        // Set default style elements here
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsLight();
        
        // Load fonts needed
        fs::path font_path = AppData.RootPath / "Data/FreeSans.ttf";
        io.Fonts->AddFontFromFileTTF(CONVERT_PATH(font_path), 15 );    // size 15
        io.Fonts->AddFontFromFileTTF(CONVERT_PATH(font_path), 18 );    // size 18
        io.Fonts->AddFontFromFileTTF(CONVERT_PATH(font_path), 20 );    // size 20

        fs::path boldItalic_path = AppData.RootPath / "Data/FreeSansBoldOblique.ttf";
        io.Fonts->AddFontFromFileTTF(CONVERT_PATH(boldItalic_path), 25 );    // size 20

    }
} // end namespace AppCore

#endif // USER_LAYOUTS_CPP