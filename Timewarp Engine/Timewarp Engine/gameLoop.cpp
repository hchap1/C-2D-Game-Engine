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

bool grounded = false;
bool dashing = false;
int dashEnd = 0;
int gameTime = 0;

int dashDirection = 0;

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

	float playerX = -1.0f;
	float playerY = -3.5f;

	int success = rendererInit();
	Shader basic_shader = makeShader();
	
	vector<vector<float>> tilemap = loadLevel(0);
	float movementMultiplier, fps;

	updateTilemap(tilemap);
	glfwSwapInterval(1);

	while (true) {
		deltaTime = render(tilemap, playerX, playerY, basic_shader);
		gameTime += 1;

		if (dashEnd > gameTime) {
			dashing = true;
		}
		else {
			dashing = false;
		}

		movementMultiplier = deltaTime * 15.0f * blockX;

		if (!dashing) {
			if (getKey(GLFW_KEY_D)) {
				playerXVelocity -= movementMultiplier;
			}
			if (getKey(GLFW_KEY_A)) {
				playerXVelocity += movementMultiplier;
			}
			if (getKey(GLFW_KEY_APOSTROPHE)) {
				dashEnd = gameTime + 20;
			}
		}

		if (playerXVelocity > 0) {
			dashDirection = 1;
		}
		else if (playerXVelocity < 0) {
			dashDirection = -1;
		}

		int px = static_cast<int>(playerX * 10.0f);

		//tilemap[px][0] = 0.3f;
		//updateTilemap(tilemap);
		
		playerX += playerXVelocity * movementMultiplier;
		if (!dashing) { playerXVelocity *= 0.9f; }
		else { (playerXVelocity = blockX * dashDirection * movementMultiplier * 500.0f); }


		fps = 1 / deltaTime;

		int blocksOnHalfScreenX = static_cast<int>(1 / blockX);
		int indexOfFirstBlockX = static_cast<int>(playerX * (blocksOnHalfScreenX * -1));

		int blocksOnHalfScreenY = static_cast<int>(1 / blockY);
		int indexOfFirstBlockY = static_cast<int>(playerY * (blocksOnHalfScreenY * -1));

		if (tilemap[indexOfFirstBlockY - 1][indexOfFirstBlockX - 1] != 0.0f) {
			if (playerXVelocity > 0.0f) {
				float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
				if (targetX - playerX < 0) {
					playerX = targetX;
					playerXVelocity = 0.0f;
				}
				
			}
		}

		int tempIndexMinus = static_cast<int>((playerX - blockX * 0.5f) * (blocksOnHalfScreenX * -1));
		int tempIndexPlus = static_cast<int>((playerX + blockX * 0.5f) * (blocksOnHalfScreenX * -1));

		if (tilemap[indexOfFirstBlockY - 1][tempIndexMinus] != 0.0f) {
			if (playerXVelocity < 0.0f) {
				float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
				if (targetX - playerX > 0) {
					playerX = targetX;
					playerXVelocity = 0.0f;
				}

			}
		}

		playerY += playerYVelocity * movementMultiplier;
		if (!dashing) { playerYVelocity += deltaTime * 15.0f * blockY; }
		if (dashing) { playerYVelocity = 0.0f; }

		grounded = false;

		if (tilemap[indexOfFirstBlockY - 2][tempIndexMinus] != 0.0f || tilemap[indexOfFirstBlockY - 2][tempIndexPlus] != 0.0f) {
			if (playerYVelocity > 0) {
				float targetY = indexOfFirstBlockY * blockY * -1;;
				if (targetY - playerY < blockY / 4) {
					playerY = targetY;
					playerYVelocity = 0.0f;
					if (!dashing) { grounded = true; }
				}
			}
		}	

		if (grounded && getKey(GLFW_KEY_SPACE)) {
			playerYVelocity = blockY * -5.0f;
		}
		
	}
}