#pragma once
#include <vector>
#include <string>
#include <numeric>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "SHADER CLASS/shader.h"

using namespace std;

tuple<float*, int> tilemapDecoder(vector<vector<int>> tilemap, int tileTextureSize, int windowWidth, int windowHeight);
class RenderLayer {
public:
    RenderLayer();
    RenderLayer(std::vector<int> attributes, std::string shaderName, std::string textureName);

    void setVertices(float* vertices, unsigned int mode);
    void draw(int numTriangles);

private:
    unsigned int VBO;
    unsigned int VAO;
    unsigned int texture;
    Shader shader;
};

class Renderer {
public:
    Renderer(int width, int height, std::string windowName);
    void updateDisplay();
    void fillScreen(int r, int g, int b);
    bool isRunning();

private:
    GLFWwindow* window;
    int width, height;
};