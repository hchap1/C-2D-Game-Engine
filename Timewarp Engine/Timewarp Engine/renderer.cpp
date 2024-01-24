#include <GLAD\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <vector>
#include <TIMEWARP ENGINE\renderer.h>
#include <SHADER CLASS\shader.h>

// Define public variables
GLFWwindow* window;
float tileVertices[] = {
     0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
     1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f
};

unsigned int indices[] = {
    0, 1, 2,
    1, 2, 3
};

Shader* basic_shader = nullptr;

unsigned int EBO;

unsigned int VAO;

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

int rendererInit() {
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

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tileVertices), tileVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // index size type normalized stride offsetPointer

    return 0;
}

void render(std::vector<std::vector<int>> tilemap, float playerX, float playerY, Shader basic_shader) {
    glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    basic_shader.setFloat("xOffset", playerX);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
}