#include <GLAD\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <vector>

// Define public variables
GLFWwindow* window;
float tile_vertices[] = {
     0.0f,  0.0f, 0.0f,
     1.0f,  0.0f, 0.0f,
     0.0f,  1.0f, 0.0f
};

// Inital declaration of the window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height);



// Update OPENGL's viewport if the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool getKey(int keycode) {
    if (glfwGetKey(window, keycode) == GLFW_PRESS) {
        return true;
    }
    return false;
}

int init() {
    // Create GLFW context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window using GLFW
    window = glfwCreateWindow(800, 600, "Timewarp", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Tell OPENGL the size of the window
    glViewport(0, 0, 800, 600);

    // Register the window resize callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwTerminate();
    return 0;
}

void render(std::vector<std::vector<int>> tilemap, int playerX, int playerY) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
}