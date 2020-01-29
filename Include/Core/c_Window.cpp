#ifndef C_WINDOW_CPP
#define C_WINDOW_CPP

#include "c_Window.h"

using namespace std;
namespace AppCore {

    // ------- WindowBase ------- //
    
    WindowBase::WindowBase() :
        CanRender( false ) {
    }

    WindowBase::~WindowBase() {
    }

    // Mouse

    void WindowBase::UpdateMousePosition( double x, double y ) {
        InputState.Position.Delta.X = x - InputState.Position.X;
        InputState.Position.Delta.Y = y - InputState.Position.Y;
        InputState.Position.X = x;
        InputState.Position.Y = y;
    }
    void WindowBase::UpdateMouseButton( size_t button, bool pressed ) {
        if( button < 2 ) {
            InputState.Buttons[button].IsPressed = pressed;
        }
        if ( CanRender ) {
            RenderPipelines();
        }
    }
    
    // Display 

    void WindowBase::UpdateWindowSize( int w, int h ) {
        DisplayState.WindowSize.W = w;
        DisplayState.WindowSize.H = h;
    }

    
    void WindowBase::UpdateFramebufferSize( int w, int h ) {
        DisplayState.FramebufferSize.W = w;
        DisplayState.FramebufferSize.H = h;
        DisplayState.ScaleFactor.X = (float) DisplayState.FramebufferSize.W / (float) DisplayState.WindowSize.W;
        DisplayState.ScaleFactor.Y = (float) DisplayState.FramebufferSize.H / (float) DisplayState.WindowSize.H;
    }

    // Access methods

    UILib::InputStateData & WindowBase::GetInputState() {
        return InputState;
    }
    UILib::DisplayStateData & WindowBase::GetDisplayState() {
        return DisplayState;
    }


    // ------- Window ------- //


    Window::Window() :
        Parameters() {
    }

    Window::~Window() {
        glfwDestroyWindow(Parameters.WindowPtr);
        glfwTerminate();
    }

    WindowParameters Window::GetParameters() const {
        return Parameters;
    }

    void Window::Create( const char *title, int width, int height ) {

        if( !glfwInit() ) {
            glfwTerminate();
            return;
        };

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);

        Parameters.WindowPtr = glfwCreateWindow(width,height,title, NULL, NULL);
        if( !Parameters.WindowPtr ) {
            printf("Could not create Window \n");
            return;
        }

    }

    void Window::RenderingLoop( WindowBase &project ) const {

        
        // Set User Pointer
        glfwSetWindowUserPointer(Parameters.WindowPtr, reinterpret_cast<void *>(&project));

        //GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        //glfwSetCursor(Parameters.WindowPtr, cursor);
        
        // Set Callbacks
        glfwSetWindowSizeCallback(Parameters.WindowPtr, &WindowResizeEvent);

        glfwSetMouseButtonCallback(Parameters.WindowPtr, &MouseEvent);
        glfwSetCursorPosCallback(Parameters.WindowPtr, &MouseMoveEvent);
        glfwSetKeyCallback(Parameters.WindowPtr, &KeyEvent);
        glfwSetScrollCallback(Parameters.WindowPtr, &ScrollEvent);


        int w, h;
        glfwGetWindowSize(Parameters.WindowPtr, &w, &h);
        project.UpdateWindowSize(w, h);

        int fw, fh;
        glfwGetFramebufferSize(Parameters.WindowPtr, &fw, &fh);
        project.UpdateFramebufferSize( fw, fh);

        while(!glfwWindowShouldClose(Parameters.WindowPtr)) {
            
            if( project.Poll == true ) { glfwPollEvents(); }
            else { glfwWaitEvents(); }
            
            // Render
            if( project.CanRender ) {
                try {
                    project.RenderPipelines();
                } catch (int except) {
                    std::cout << "RenderingLoop Exception: " << except << std::endl;
                    break;
                }
            } else {
                std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
            }

        }
    }

    // Window callback functions

    void Window::WindowResizeEvent(GLFWwindow* window, int width, int height) {
        int fwidth, fheight;
        glfwGetFramebufferSize(window, &fwidth, &fheight);

        WindowBase *project = reinterpret_cast<WindowBase*>(glfwGetWindowUserPointer(window));
        project->UpdateWindowSize( width, height );
        project->UpdateFramebufferSize( fwidth, fheight );
        project->OnWindowSizeChanged();
        if ( project->CanRender ) {
            project->RenderPipelines();
        }
    }
    void Window::ScrollEvent(GLFWwindow* window, double xoffset, double yoffset){
        WindowBase *project = reinterpret_cast<WindowBase*>(glfwGetWindowUserPointer(window));
        UILib::InputStateData& ISD = project->GetInputState();
        ISD.ScrollY = (float)yoffset;
    }
    void Window::MouseEvent(GLFWwindow* window, int button, int action, int mods) {   
        WindowBase *project = reinterpret_cast<WindowBase*>(glfwGetWindowUserPointer(window));
        project->UpdateMouseButton( (size_t) button, action );
    }

    void Window::MouseMoveEvent(GLFWwindow* window, double xpos, double ypos) {
        WindowBase *project = reinterpret_cast<WindowBase*>(glfwGetWindowUserPointer(window));
        project->UpdateMousePosition( xpos, ypos);
        //std::cout << "--- Window::Mouse Position    " << xpos << ", " << ypos << std::endl;
    }
    void Window::KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods){
        WindowBase *project = reinterpret_cast<WindowBase*>(glfwGetWindowUserPointer(window));
        
        UILib::InputStateData& ISD = project->GetInputState();
        vector<array<int,2>> *kBuffer = &ISD.Key.Buffer;

        if (action == GLFW_PRESS){
            //std::cout << "---  DOWN Key  " << key << std::endl;
            // std::cout << "---  DOWN Key  " << (char) key << std::endl;
            project->OnKeyPress(key);
            kBuffer->push_back({1, key});
        }
        else if (action == GLFW_REPEAT){
            //std::cout << "---  REPEAT Key  " << key << std::endl;
        }
        else if (action == GLFW_RELEASE ){
            //std::cout << "---  RELEASE  Key  " << key << std::endl;
            kBuffer->push_back({2, key});

        }
        
        if (mods & GLFW_MOD_SHIFT) {ISD.Shift = true;}else{ISD.Shift = false;}
        if (mods & GLFW_MOD_CONTROL) {ISD.Ctrl = true;}else{ISD.Ctrl = false;}
        if (mods & GLFW_MOD_ALT) {ISD.Alt = true;}else{ISD.Alt = false;}
        if (mods & GLFW_MOD_SUPER) {ISD.Supr = true;}else{ISD.Supr = false;}

    }

}   // end namespace AppCore


#endif	// C_WINDOW_CPP