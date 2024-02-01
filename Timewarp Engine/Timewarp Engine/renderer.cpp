#define STB_IMAGE_IMPLEMENTATION
#include <GLAD\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <vector>
#include <TIMEWARP ENGINE\renderer.h>
#include <SHADER CLASS\shader.h>
#include <utility>
#include <STB\stb_image.h>
#include <TIMEWARP ENGINE\gameLoop.h>
#include <TIMEWARP ENGINE\levelEditor.h>

int globalScreenWidth = 1000;
int globalScreenHeight = 800;

float blockWidth;
float blockHeight;

bool isPixelArt = true;

unsigned int blockTexture;

// Define public variables
GLFWwindow* window;
std::vector<std::vector<float>> cachedTilemap;
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

unsigned int triangleCount = 0;

float currentTime, deltaTime, lastFrame;

unsigned int VBO, VAO;
unsigned int PVBO, PVAO;
unsigned int BVBO, BVAO;
unsigned int playerTexture;
unsigned int background0Texture;
unsigned int background1Texture;
unsigned int background2Texture;

void setBackgroundRGB(float r, float b, float g) {
    glClearColor(r / 255, g / 255, b / 255, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

unsigned int generateTexture(std::string filePath) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    if (isPixelArt) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); }
    else { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); }
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);  
    std::cout << "DONE LOADING TEXTURE: " << filePath << std::endl;
    return texture;
}

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

int rendererInit(bool isGame) {
    // Create GLFW context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window using GLFW
    window = glfwCreateWindow(globalScreenWidth, globalScreenHeight, "Timewarp", NULL, NULL);
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
    glViewport(0, 0, globalScreenWidth, globalScreenHeight);

    // Register the window resize callback  
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Rendering setup for TILES
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tileVertices), tileVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // index size type normalized stride offsetPointer

    stbi_set_flip_vertically_on_load(true);
    blockTexture = generateTexture("src/textures/blocks.png");

    //Rendering setup for PLAYER[s]
    glGenVertexArrays(1, &PVAO);
    glGenBuffers(1, &PVBO);
    glBindVertexArray(PVAO);
    glBindBuffer(GL_ARRAY_BUFFER, PVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tileVertices), tileVertices, GL_DYNAMIC_DRAW);
    //Vertice example: x, y, tx, ty [z is hardcoded to 1]
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); //texture coords
    glEnableVertexAttribArray(1);
    // index size type normalized stride offsetPointer
    stbi_set_flip_vertically_on_load(false);
    if (isGame) { 
        playerTexture = generateTexture("src/textures/player.png"); 
        stbi_set_flip_vertically_on_load(true);
        background0Texture = generateTexture("src/textures/0.png");
        glGenVertexArrays(1, &BVAO);
        glGenBuffers(1, &BVBO);
        glBindVertexArray(BVAO);
        glBindBuffer(GL_ARRAY_BUFFER, BVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(tileVertices), tileVertices, GL_DYNAMIC_DRAW);
        //Vertice example: x, y, tx, ty [z is hardcoded to 1]
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); //position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float))); //texture coords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); //backgroundID
        glEnableVertexAttribArray(2);
        float sizeX = 100.0f / globalScreenWidth * 60.0f;
        float sizeY = 100.0f / globalScreenHeight * 15.0f;

        std::cout << "SIZE X: " << globalScreenWidth << std::endl;

        float backgroundVertices[] = {
            -sizeX, -sizeY, 0.0f, 0.25f, 2.0f,
             sizeX, -sizeY, 1.0f, 0.25f, 2.0f,
            -sizeX,  sizeY, 0.0f, 0.5f, 2.0f,
             sizeX,  sizeY, 1.0f, 0.5f, 2.0f,
             sizeX, -sizeY, 1.0f, 0.25f, 2.0f,
            -sizeX,  sizeY, 0.0f, 0.5f, 2.0f,

            -sizeX, -sizeY, 0.0f, 0.5f, 1.0f,
             sizeX, -sizeY, 1.0f, 0.5f, 1.0f,
            -sizeX,  sizeY, 0.0f, 0.75f, 1.0f,
             sizeX,  sizeY, 1.0f, 0.75f, 1.0f,
             sizeX, -sizeY, 1.0f, 0.5f, 1.0f,
            -sizeX,  sizeY, 0.0f, 0.75f, 1.0f,
            
            -sizeX, -sizeY, 0.0f, 0.75f, 0.0f,
             sizeX, -sizeY, 1.0f, 0.75f, 0.0f,
            -sizeX,  sizeY, 0.0f, 1.0f, 0.0f,
             sizeX,  sizeY, 1.0f, 1.0f, 0.0f,
             sizeX, -sizeY, 1.0f, 0.75f, 0.0f,
            -sizeX,  sizeY, 0.0f, 1.0f, 0.0f         
            
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
    }
    else { 
        int tw, th;
        glfwGetWindowSize(window, &tw, &th);
        float w = 50.0f / tw;
        float h = 50.0f / th;
        float outlineVertices[24] = {
            -w, -h, 0, 0,
             w, -h, 1, 0,
            -w,  h, 0, 1,
            -w,  h, 0, 1,
             w,  h, 1, 1,
             w, -h, 1, 0
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(outlineVertices), outlineVertices, GL_STATIC_DRAW);
        playerTexture = generateTexture("src/textures/outline.png"); }
    glBindVertexArray(VAO);

    deltaTime = 0.01f;
    lastFrame = glfwGetTime();

    return 0;
}

Shader makeTileShader() {
    Shader basic_shader("src/shaders/tile_vertex_shader.txt", "src/shaders/tile_fragment_shader.txt");
    basic_shader.use();
    return basic_shader;
}

Shader makeSimpleTileShader() {
    Shader basic_shader("src/shaders/simple_tile_vertex_shader.txt", "src/shaders/simple_tile_fragment_shader.txt");
    basic_shader.use();
    return basic_shader;
}

Shader makePlayerShader() {
    Shader basic_shader("src/shaders/player_vertex_shader.txt", "src/shaders/player_fragment_shader.txt");
    basic_shader.use();
    return basic_shader;
}

Shader makeOutlineShader() {
    Shader basic_shader("src/shaders/outline_vertex_shader.txt", "src/shaders/outline_fragment_shader.txt");
    basic_shader.use();
    return basic_shader;
}

Shader makeParallaxShader() {
    Shader basic_shader("src/shaders/parallax_vertex_shader.txt", "src/shaders/parallax_fragment_shader.txt");
    basic_shader.use();
    return basic_shader;
}

std::pair<float*, int> flatten2DVector(const std::vector<std::vector<float>>& inputVector) {
    // Calculate the total size needed for the flat array
    size_t totalSize = 0;
    for (const auto& row : inputVector) {
        totalSize += row.size() * 36 * sizeof(float);
    }

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float xMult = 100.0f / width;
    float yMult = 100.0f / height;

    setBlockSize(xMult, yMult, width, height);
    blockWidth = xMult;
    blockHeight = yMult;

    setBlockAndWindowSize(xMult, yMult, width, height);

    // Allocate a dynamic array
    float* flattenedArray = new float[totalSize];

    // Copy data from the 2D vector to the flat array
    size_t index = 0;
    int x = 0;
    int y = 0;
    int numOfTriangles = 0;
    for (const auto& row : inputVector) {
        for (const auto& element : row) {
            if (element != 0.0f) {
                numOfTriangles += 2;
                //Top right
                flattenedArray[index++] = (x + 1.0f) * xMult;
                flattenedArray[index++] = (y + 1.0f) * yMult;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = element * 10;

                //Bottom right
                flattenedArray[index++] = (x + 1.0f) * xMult;
                flattenedArray[index++] = y * yMult;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = 0.0f;
                flattenedArray[index++] = element * 10;

                //Bottom left
                flattenedArray[index++] = x * xMult;
                flattenedArray[index++] = y * yMult;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = 0.0f;
                flattenedArray[index++] = 0.0f;
                flattenedArray[index++] = element * 10;

                //Top right
                flattenedArray[index++] = (x + 1.0f) * xMult;
                flattenedArray[index++] = (y + 1.0f) * yMult;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = element * 10;

                //Top left
                flattenedArray[index++] = x * xMult;
                flattenedArray[index++] = (y + 1.0f) * yMult;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = 0.0f;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = element * 10;

                //Bottom left
                flattenedArray[index++] = x * xMult;
                flattenedArray[index++] = y * yMult;
                flattenedArray[index++] = 1.0f;
                flattenedArray[index++] = 0.0f;
                flattenedArray[index++] = 0.0f;
                flattenedArray[index++] = element * 10;
            }
            x++;
        }
        y++;
        x = 0;
    }
    return { flattenedArray, numOfTriangles };
}


void updateTilemap(std::vector<std::vector<float>> tilemap) {
    cachedTilemap = tilemap;
    auto result = flatten2DVector(tilemap);

    float* vertices = result.first;
    int numOfTriangles = result.second;

    size_t totalSize = numOfTriangles * 18 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, totalSize, vertices, GL_DYNAMIC_DRAW);
    triangleCount = numOfTriangles;
    delete[] vertices;
}

    int updatePlayerPositions(std::vector<float> pXP, std::vector<float> pYP, std::vector<bool> pCV, float playerX, float playerY) {

        int playersToRender = pXP.size();
        size_t totalSize = playersToRender * 24 * sizeof(float);
        float* vertices = new float[totalSize];

        float pHBX, nHBX, pFBY, nFBY, xPos, yPos, top, bot;
        bool isCrouching;
        int numTriangles = 0;
        int index = 0;

        for (int i = 0; i < playersToRender; i++){
            xPos = pXP[i];
            yPos = pYP[i];
            isCrouching = pCV[i];

            pHBX = blockWidth / 2 + xPos;
            nHBX = blockWidth / -2 + xPos;
            nFBY = blockHeight * -1 + yPos;
            pFBY = blockHeight + yPos;

            bot = 0.0f;
            top = 0.5f;

            if (isCrouching) { top += 0.25f; bot += 0.5f; }

            numTriangles += 2;

            if (isCrouching) { nFBY += blockHeight; }

            //Bottom left
            vertices[index++] = nHBX;
            vertices[index++] = nFBY;
            vertices[index++] = 0.0f;
            vertices[index++] = bot;
            //Top left
            vertices[index++] = nHBX;
            vertices[index++] = pFBY;
            vertices[index++] = 0.0f;
            vertices[index++] = top;
            //Bottom right
            vertices[index++] = pHBX;
            vertices[index++] = nFBY;
            vertices[index++] = 1.0f;
            vertices[index++] = bot;
            //Top left
            vertices[index++] = nHBX;
            vertices[index++] = pFBY;
            vertices[index++] = 0.0f;
            vertices[index++] = top;
            //Bottom right
            vertices[index++] = pHBX;
            vertices[index++] = nFBY;
            vertices[index++] = 1.0f;
            vertices[index++] = bot;
            //Top right
            vertices[index++] = pHBX;
            vertices[index++] = pFBY;
            vertices[index++] = 1.0f;
            vertices[index++] = top;
        };
        glBindVertexArray(PVAO);
        glBindBuffer(GL_ARRAY_BUFFER, PVBO);
        glBufferData(GL_ARRAY_BUFFER, totalSize, vertices, GL_DYNAMIC_DRAW);
        delete[] vertices;
        return numTriangles;
    }

float render(float playerX, float playerY, 
    Shader tile_shader, Shader player_shader, std::vector<float> playerSpriteXPositions, 
    std::vector<float> playerSpriteYPositions, std::vector<bool> playerCrouchingVector,
    bool red, bool green, bool blue, float* colorMultiplier, Shader parallax_shader) {

    setBackgroundRGB(100 * colorMultiplier[0], 175 * colorMultiplier[1], 205 * colorMultiplier[2]);

    glBindVertexArray(BVAO);
    glBindTexture(GL_TEXTURE_2D, background0Texture);
    parallax_shader.use();
    parallax_shader.setFloat("cameraX", playerX);
    parallax_shader.setFloat("cameraY", playerY);
    glDrawArrays(GL_TRIANGLES, 0, 6 * sizeof(float));

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, blockTexture);
    
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;

    tile_shader.use();
    tile_shader.setFloat("cameraX", playerX);
    tile_shader.setFloat("cameraY", playerY);
    tile_shader.setBool("red", red);
    tile_shader.setBool("green", green);
    tile_shader.setBool("blue", blue);
    tile_shader.setFloatVec3("colorMultiplier", colorMultiplier);

    glDrawArrays(GL_TRIANGLES, 0, triangleCount * sizeof(float));

    glBindVertexArray(PVAO);
    player_shader.use();
    player_shader.setFloat("cameraX", playerX);
    player_shader.setFloat("cameraY", playerY);

    glBindTexture(GL_TEXTURE_2D, playerTexture);
    int numTriangles = updatePlayerPositions(playerSpriteXPositions, playerSpriteYPositions, playerCrouchingVector, playerX, playerY);
    glDrawArrays(GL_TRIANGLES, 0, numTriangles * 3);

    glfwSwapBuffers(window);
    glfwPollEvents();

    return deltaTime;
}

float tilemapRender(float playerX, float playerY, std::vector<std::vector<float>> tilemap, Shader tile_shader, Shader outline_shader) {
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, blockTexture);
    tile_shader.use();
    tile_shader.setFloat("cameraX", playerX);
    tile_shader.setFloat("cameraY", playerY);
    tile_shader.setBool("red", false);
    tile_shader.setBool("green", false);
    tile_shader.setBool("blue", false);
    setBackgroundRGB(100, 175, 205);
    glDrawArrays(GL_TRIANGLES, 0, triangleCount * sizeof(float));
    
    glBindVertexArray(PVAO);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    outline_shader.use();
    glDrawArrays(GL_TRIANGLES, 0, 2 * sizeof(float));

    glfwSwapBuffers(window);
    glfwPollEvents();
    return deltaTime;
}