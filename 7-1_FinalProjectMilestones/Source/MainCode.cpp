///////////////////////////////////////////////////////////////////////////////
// maincode.cpp
//
// Application entry point. Initializes the OpenGL environment, creates the
// application window, loads shaders and scene resources, and executes the
// main rendering loop.
//
// CS-330 Computational Graphics and Visualization
// Southern New Hampshire University
// CS330Content2026 Framework
// Release: 2026 C4
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdlib>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneManager.h"
#include "ViewManager.h"
#include "ShapeMeshes.h"
#include "ShaderManager.h"

// Application-wide objects used during initialization and rendering.
namespace
{
    constexpr const char* WINDOW_TITLE = "OpenGL Sample";

    GLFWwindow* g_Window = nullptr;

    std::unique_ptr<SceneManager> g_SceneManager;
    std::unique_ptr<ShaderManager> g_ShaderManager;
    std::unique_ptr<ViewManager> g_ViewManager;
}

// Initializes GLFW and configures the OpenGL context.
bool InitializeGLFW();

// Initializes GLEW after a valid OpenGL context has been created.
bool InitializeGLEW();

/***********************************************************
 *  main()
 *
 *  Creates the application window, initializes OpenGL
 *  resources, and runs the rendering loop until the user
 *  closes the application.
 ***********************************************************/

int main()
{
    if (!InitializeGLFW())
    {
        return EXIT_FAILURE;
    }

    g_ShaderManager = std::make_unique<ShaderManager>();
    g_ViewManager = std::make_unique<ViewManager>(g_ShaderManager.get());

    g_Window = g_ViewManager->CreateDisplayWindow(WINDOW_TITLE);

    if (!InitializeGLEW())
    {
        return EXIT_FAILURE;
    }

    g_ShaderManager->LoadShaders(
        "shaders/vertexShader.glsl",
        "shaders/fragmentShader.glsl");
    g_ShaderManager->use();

    g_SceneManager = std::make_unique<SceneManager>(g_ShaderManager.get());
    g_SceneManager->PrepareScene();

    // Enable depth testing so fragments are rendered based on distance.
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(g_Window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        g_ViewManager->PrepareSceneView();
        g_SceneManager->RenderScene();

        glfwSwapBuffers(g_Window);
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}

/***********************************************************
 *  InitializeGLFW()
 *
 *  Initializes GLFW and configures the OpenGL context
 *  attributes required by the application.
 *
 *  Returns:
 *      true  - GLFW initialized successfully
 *      false - GLFW initialization failed
 ***********************************************************/

bool InitializeGLFW()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed.\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return true;
}

/***********************************************************
 *  InitializeGLEW()
 *
 *  Initializes the GLEW extension loader and verifies that
 *  OpenGL functions required by the application are
 *  available.
 *
 *  Returns:
 *      true  - GLEW initialized successfully
 *      false - GLEW initialization failed
 ***********************************************************/

bool InitializeGLEW()
{
    GLenum result = glewInit();

    if (result != GLEW_OK)
    {
        std::cerr << "GLEW initialization failed: "
            << glewGetErrorString(result)
            << '\n';
        return false;
    }

    std::cout << "OpenGL initialized successfully\n";
    std::cout << "OpenGL Version: "
        << glGetString(GL_VERSION)
        << '\n';

    return true;
}