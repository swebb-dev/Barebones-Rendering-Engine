#include "../inc/window.h"
#include "../inc/render.h"
#include <stdbool.h>

static GLFWwindow* window;

static double lastTime = 0.0;
static double nowTime = 0.0;
static double deltaTime = 1.0f; // / 60.0f; // Default to 60 FPS

static bool rotating = false;

static double old_xpos = -999.0f, old_ypos = -999.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        handleKey(' ', true);
    } else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        handleKey(' ', false);
    } else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        handleKey('C', true);
    } else if (key == GLFW_KEY_C && action == GLFW_RELEASE) {
        handleKey('C', false);
    } else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        handleKey('W', true);
    } else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        handleKey('S', true);
    } else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        handleKey('W', false);
    } else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        handleKey('S', false);
    } else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        handleKey('A', true);
    } else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        handleKey('D', true);
    } else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        handleKey('A', false);
    } else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
        handleKey('D', false);
    } else {
        return; // Ignore other keys
    } 
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    handleResize(width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    handleScroll(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // Start rotating
            rotating = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else if (action == GLFW_RELEASE) {
            // Stop rotating
            rotating = false;
            old_xpos = -999.0f;
            old_ypos = -999.0f;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (rotating) {
        if (old_xpos != -999.0f && old_ypos != -999.0f) {
            handleMouseMovement(xpos - old_xpos, ypos - old_ypos);
        }
        old_xpos = xpos;
        old_ypos = ypos;
    }
}

int Window_Init(int width, int height, char* title) {
    if (!glfwInit()) {
        // Initialization failed
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // Allow the window to be resizable
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
    glfwSetScrollCallback(window, scroll_callback); // Enable scroll callback for zooming
    glfwSetMouseButtonCallback(window, mouse_button_callback); // Enable mouse button callback for rotation
    glfwSetCursorPosCallback(window, cursor_position_callback); // Enable cursor position callback for rotation
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Handle window resizing
    glfwSetKeyCallback(window, key_callback); // Enable key callback for additional controls (e.g., reset view)

    return 0;
}

void Window_Update(void) {
    /*
    glClear(GL_COLOR_BUFFER_BIT); // Clear the screen
    glfwSwapBuffers(window); // Swap front and back buffers
    glfwPollEvents(); // Check for events (inputs, window close, etc.)

    render(); // Render the scene
    */

    lastTime = glfwGetTime();

    render(deltaTime); // Pass delta time to the render function
    glfwSwapBuffers(window); // Swap front and back buffers

    // glfwPollEvents(); // Check for events (inputs, window close, etc.)
    glfwWaitEvents(); // WI believe this is significantly more efficient than polling and works just as well for this use case

    nowTime = glfwGetTime();
    deltaTime = (float)(nowTime - lastTime) * 10.0f; // Scale delta time for better control

}

int Window_shouldClose(void) {
    return glfwWindowShouldClose(window);
}

void Window_Terminate(void) {
    glfwDestroyWindow(window);
    glfwTerminate();
}