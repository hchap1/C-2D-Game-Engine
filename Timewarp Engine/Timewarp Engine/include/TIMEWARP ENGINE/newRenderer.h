#pragma once
#include <vector>
#include <string>
#include <numeric>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "SHADER CLASS/shader.h"
#include "TIMEWARP ENGINE/gameState.h"

using namespace std;

tuple<float*, int> tilemapDecoder(vector<vector<int>> tilemap, int tileTextureSize, int windowWidth, int windowHeight);

class RenderLayer {
public:
    RenderLayer();
    RenderLayer(std::vector<int> attributes, std::string shaderName, std::string textureName);

    void setVertices(float* vertices, int numTriangles, int numFloatsPerTriangle, unsigned int mode);
    void draw(int numTriangles);
    void setFloat(string name, float value);

private:
    unsigned int VBO;
    unsigned int VAO;
    unsigned int texture;
    int step;
    Shader shader;
};

class Renderer {
public:
    Renderer(int width, int height, std::string windowName);
    void updateDisplay();
    void fillScreen(int r, int g, int b);
    bool isRunning();
    float getBlockWidth();
    float getBlockHeight();
    bool getKeyDown(unsigned int keycode);
    float getDeltaTime();

private:
    GLFWwindow* window;
    int width, height;
    float lastFrame;
};

tuple<float*, int> gameStateDecoder(GameState gameState, Renderer renderer);