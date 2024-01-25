#include <TIMEWARP ENGINE\loadTilemap.h>
#include <iostream>
#include <vector>
#include <TIMEWARP ENGINE\renderer.h>
#include <SHADER CLASS\shader.h>
#include <GLFW\glfw3.h>
#include <chrono>
#include <thread>

using namespace std;

float targetFps = 120.0f;
float msPerFrame;
bool doFrameCap = true;

float blockX;
float blockY;
float width;
float height;

void setBlockSize(float bx, float by, float w, float h) {
	blockX = bx;
	blockY = by;
	width = w;
	height = h;
}

int main() {
	msPerFrame = 1 / targetFps;

	float deltaTime;
	float playerXVelocity = 0.0f;
	float playerYVelocity = 0.0f;

	float playerX = 0;
	float playerY = 0;

	int success = rendererInit();
	Shader basic_shader = makeShader();
	
	vector<vector<float>> tilemap = loadLevel(0);
	float movementMultiplier, fps;

	updateTilemap(tilemap);
	glfwSwapInterval(1);

	while (true) {
		deltaTime = render(tilemap, playerX, playerY, basic_shader);
		movementMultiplier = deltaTime * 2.0f;
		if (getKey(GLFW_KEY_W)) {
			playerY += deltaTime;
		}
		if (getKey(GLFW_KEY_S)) {
			playerY -= deltaTime;
		}
		if (getKey(GLFW_KEY_D)) {
			playerXVelocity -= movementMultiplier;
		}
		if (getKey(GLFW_KEY_A)) {
			playerXVelocity += movementMultiplier;
		}

		int px = static_cast<int>(playerX * 10.0f);

		//tilemap[px][0] = 0.3f;
		//updateTilemap(tilemap);
		
		playerX += playerXVelocity * movementMultiplier;
		playerXVelocity *= 0.9f;

		playerY += playerYVelocity * movementMultiplier;
		//playerYVelocity += deltaTime * 5.0f;

		fps = 1 / deltaTime;

		int blocksOnHalfScreen = static_cast<int>(1 / blockX);
		int indexOfFirstBlock = static_cast<int>(playerX * (blocksOnHalfScreen * 2));

		std::cout << "INDEX: " << indexOfFirstBlock << std::endl;
		
	}
}