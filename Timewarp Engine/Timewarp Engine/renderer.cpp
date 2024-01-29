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

int globalScreenWidth = 800;
int globalScreenHeight = 600;

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

unsigned int VBO, VAO, EBO;
unsigned int PVBO, PVAO, PEBO;
unsigned int playerTexture;

unsigned int generateTexture(std::string filePath) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    if (isPixelArt) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); }
    else { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); }
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
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

int rendererInit() {
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

    playerTexture = generateTexture("src/textures/player.png");
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

Shader makePlayerShader() {
    Shader basic_shader("src/shaders/player_vertex_shader.txt", "src/shaders/player_fragment_shader.txt");
    basic_shader.use();
    return basic_shader;
}

std::pair<float*, int> flatten2DVector(const std::vector<std::vector<float>>& inputVector) {
    // Calculate the total size needed for the flat array
    size_t totalSize = 0;
    for (const auto& row : inputVector) {
        totalSize += row.size() * 36 + 36;
    }

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float xMult = 100.0f / width;
    float yMult = 100.0f / height;

    setBlockSize(xMult, yMult, width, height);
    blockWidth = xMult;
    blockHeight = yMult;

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

int updatePlayerPositions(std::vector<float> pXP, std::vector<float> pYP, std::vector<bool> pCV) {
    float pHBX = blockWidth / 2;
    float nHBX = pHBX * -1;
    float nFBY = blockHeight * -1;
    float pFBY = blockHeight;
    float vertices[] = { nHBX, -pFBY, 0.0f, 0.0f, //bottom left
                    nHBX,  pFBY, 0.0f, 1.0f, //top left
                     pHBX, -pFBY, 1.0f, 0.0f, //bottom right
                    nHBX,  pFBY, 0.0f, 1.0f, //top left
                     pHBX, -pFBY, 1.0f, 0.0f, //bottom right
                     pHBX,  pFBY, 1.0f, 1.0f  //top right
    };
    glBindVertexArray(PVAO);
    glBindBuffer(GL_ARRAY_BUFFER, PVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    return 2;
}

float render(std::vector<std::vector<float>> tilemap, float playerX, float playerY, Shader tile_shader, Shader player_shader, std::vector<float> playerSpriteXPositions, std::vector<float> playerSpriteYPositions, std::vector<bool> playerCrouchingVector) {

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, blockTexture);

    updateTilemap(tilemap);
    
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;

    tile_shader.use();
    tile_shader.setFloat("cameraX", playerX);
    tile_shader.setFloat("cameraY", playerY);

    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, triangleCount * sizeof(float));

    glBindVertexArray(PVAO);
    player_shader.use();
    player_shader.setFloat("cameraX", playerX);
    player_shader.setFloat("cameraY", playerY);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    int num_triangles = updatePlayerPositions(playerSpriteXPositions, playerSpriteYPositions, playerCrouchingVector);
    glDrawArrays(GL_TRIANGLES, 0, num_triangles * sizeof(float));

    glfwSwapBuffers(window);
    glfwPollEvents();

    return deltaTime;
}