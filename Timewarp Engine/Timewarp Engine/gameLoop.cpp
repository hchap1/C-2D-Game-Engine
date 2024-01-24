#include <TIMEWARP ENGINE\loadTilemap.h>
#include <iostream>
#include <vector>
#include <TIMEWARP ENGINE\renderer.h>
#include <SHADER CLASS\shader.h>
#include <GLFW\glfw3.h>
#include <chrono>
#include <thread>

using namespace std;

float targetFps = 60.0f;
float msPerFrame;
bool doFrameCap = false;

int main() {
	msPerFrame = 1 / targetFps;

	float deltaTime;
	float playerXVelocity = 0.0f;
	float playerYVelocity = 0.0f;

	float playerX = 0.0f;
	float playerY = 0.0f;

	int success = rendererInit();
	Shader basic_shader = makeShader();
	
	vector<vector<float>> tilemap = loadLevel(0);
	float movementMultiplier, fps;

	updateTilemap(tilemap);

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
		
		playerX += playerXVelocity * movementMultiplier * 1000;

		fps = 1 / deltaTime;
		playerXVelocity /= (fps * 0.1);
		if (fps > targetFps && doFrameCap) {

			int timeDif = static_cast<int>((msPerFrame - deltaTime) * 1000.0f);

			std::chrono::milliseconds durationToWait(timeDif);
			std::this_thread::sleep_for(durationToWait);
		}

		std::cout << "FPS: " << 4000 + fps << std::endl;
		
	}
}