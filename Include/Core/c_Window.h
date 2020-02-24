#ifndef C_WINDOW_H
#define C_WINDOW_H

#if defined( __linux__ )
    #include <algorithm>
    #include <iterator>
#endif
#if defined( _WIN32 )
    //#define NOMINMAX
    #include <windows.h>
    #include <stdexcept>
    #include <algorithm>
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <array>
#include <cmath>

#include "UILib.h"
#include "ch_Tools.h"
#include "user_AppDataIO.h"


#if VK_HEADER_VERSION >= 131 
    #define VULKAN_VERSION_2
#elif VK_HEADER_VERSION >= 68
    #define VULKAN_VERSION_1
#endif


namespace AppCore {

    // ************************************************************ //
    // WindowBase Class                                             //
    // ************************************************************ //

    class WindowBase {

    public:

        WindowBase();
        virtual ~WindowBase();

        // Virtual methods that can be overrided by derived classes
        virtual void OnWindowSizeChanged() = 0;
        
        virtual void OnKeyPress( int key ) = 0;
        
        virtual void RenderPipelines() = 0;

        // Methods to update mouse and display data
        void UpdateMousePosition( double x, double y );
        void UpdateMouseButton( size_t button, bool pressed );
        
        void UpdateWindowSize( int w, int h );
        void UpdateFramebufferSize( int w, int h );
        
        // Methods to access data
        UILib::InputStateData &    GetInputState();
        UILib::DisplayStateData &  GetDisplayState();

        bool                        CanRender;
        bool                        Poll = false;

    protected:

        
        UILib::InputStateData       InputState;
        UILib::DisplayStateData     DisplayState;


    };

    // ************************************************************ //
    // Window Parameters                                            //
    // ************************************************************ //
    struct WindowParameters {
        GLFWwindow         *WindowPtr;

        WindowParameters() :
            WindowPtr() {
        }

    };

    // ************************************************************ //
    // Window Class                                                 //
    // ************************************************************ //

    class Window {
    
    public:

        Window();
        Window( AppDataIO * inAppData );
        ~Window();

        void                Create( const char * title, int width = 500, int height = 500 );
        void                RenderingLoop( WindowBase & project ) const;
        
        WindowParameters    GetParameters() const;

    private:
    
        WindowParameters  Parameters;
        AppDataIO *       AppData;

        // Callback method
        static void WindowResizeEvent(GLFWwindow* window, int width, int height);
        static void FramebufferSizeEvent(GLFWwindow* window, int width, int height);

        static void MouseEvent(GLFWwindow* window, int button, int action, int mods);
        static void MouseMoveEvent(GLFWwindow* window, double xpos, double ypos);

        static void KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

        static void ScrollEvent(GLFWwindow* window, double xoffset, double yoffset);

    };


}   // end namespace AppCore

#endif	// C_WINDOW_H