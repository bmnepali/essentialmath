//===============================================================================
// @ IvMain.cpp
// 
// Start up and main event loop
// ------------------------------------------------------------------------------
// Copyright (C) 2008 by Elsevier, Inc. All rights reserved.
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <stdlib.h>

#include <GLFW/glfw3.h>

#include <IvGame.h>
#include <IvDebugger.h>
#include <IvEventHandler.h>
#include <OGL/IvRendererOGL.h>

static void visibilityCallback(GLFWwindow* window, int vis);
static void reshapeCallback(GLFWwindow* window, int w, int h);

static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseCallback(GLFWwindow* window, int button, int action, int mods);

static void error_callback(int error, const char* description)
{
    ERROR_OUT(description << std::endl);
}

static GLFWwindow* window = NULL;

//-------------------------------------------------------------------------------
// @ main()
//-------------------------------------------------------------------------------
// Where it all begins
//-------------------------------------------------------------------------------
int main(int argc, char *argv[])
{   
    gDebugger.DumpToFile( "Debug.txt" );
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
    {
        ERROR_OUT("Error: could not initialize GLFW" << std::endl);
    }
    
    // set up for core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
    
    // set up display
    bool fullscreen = false;
    if (fullscreen)
    {
        window = glfwCreateWindow(640, 480, "Example", glfwGetPrimaryMonitor(), NULL);
    }
    else
    {
        window = glfwCreateWindow(640, 480, "Example", NULL, NULL);
    }
    if (NULL == window) 
    {
        ERROR_OUT("Error: could not create window" << std::endl);
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    
#ifndef PLATFORM_OSX
	GLenum err = glewInit();
    if ( GLEW_OK != err )
    {
        ERROR_OUT("Error: " << glewGetErrorString(err) << std::endl);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // check for version compatibility
    if ( !GLEW_VERSION_2_0 )
    {
        ERROR_OUT("Error: OpenGL version 2.0 is not supported." << std::endl);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // check for ARB compatibility
    if ( !GLEW_ARB_shader_objects || !GLEW_ARB_fragment_shader || !GLEW_ARB_vertex_shader )
    {
        ERROR_OUT("Error: GLSL is not supported." << std::endl);
        return 1;
    }

    DEBUG_OUT("Using GLEW " << glewGetString(GLEW_VERSION));
#endif

    // set up game
    if (!IvGame::Create() || !IvGame::mGame->PreRendererInitialize( argc, argv ))
    {
        IvGame::Destroy();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

	// set up renderer
	if (!IvRendererOGL::Create() || !IvRendererOGL::mRenderer->Initialize( 640, 480 ))
    {
        IvRenderer::Destroy();
        IvGame::Destroy();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // Set the background dark blue
	IvRenderer::mRenderer->SetClearColor( 0.0f, 0.0f, 0.10f, 1.0f );

    // Do post-renderer creation initialization
    if (!IvGame::mGame->PostRendererInitialize())
    {
        IvRenderer::Destroy();
        IvGame::Destroy();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // set up display callbacks
    glfwSetWindowFocusCallback(window, visibilityCallback);
    glfwSetWindowSizeCallback(window, reshapeCallback);

    // set up event handlers
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);

    // run it!
    while (!glfwWindowShouldClose(window))
    {
        // game does everything it needs to do
        IvGame::mGame->Update();
        
        // clear the back buffer
        IvRenderer::mRenderer->ClearBuffers(kColorDepthClear);
        
        // render the world
        if ( IvRenderer::mRenderer->IsActive() )
        {
            IvGame::mGame->Display();
        }
        
        // swap graphics buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    IvGame::Destroy();

    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
    
}   // End of main()


static void
visibilityCallback(GLFWwindow* window, int vis)
{
	IvRenderer::mRenderer->Activate( vis == GL_TRUE );
}


static void
reshapeCallback(GLFWwindow* window, int w, int h)
{
    IvRenderer::mRenderer->Resize( w, h );
}

static void 
keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT) 
    {
        IvGame::mGame->mEventHandler->KeyDown( key );   // Handle key down event
    }
    else 
    {
        IvGame::mGame->mEventHandler->KeyUp( key ); // Handle key up event
    }
    
}

static void 
mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    if ( button == GLFW_MOUSE_BUTTON_LEFT )
    {
        if ( action == GLFW_PRESS )
        {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            IvGame::mGame->mEventHandler->MouseDown( (int)x, (int)y );
        }
        else
        {
            IvGame::mGame->mEventHandler->MouseUp();
        }
    }
}

//-------------------------------------------------------------------------------
// @ GetTime()
//-------------------------------------------------------------------------------
// Get current system time in milliseconds
//-------------------------------------------------------------------------------
unsigned int
GetTime()
{
    return glfwGetTime()*1000.0f;
}


//-------------------------------------------------------------------------------
// @ SetWindowTitle()
//-------------------------------------------------------------------------------
// Set the current window title to the string value
//-------------------------------------------------------------------------------
void
SetWindowTitle( const char* title )
{
    glfwSetWindowTitle( window, title );
}


