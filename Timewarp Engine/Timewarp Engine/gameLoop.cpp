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

bool canDash = true;

int dashEnd = 0;
int gameTime = 0;
int latestTimeReached = 0;
int timeForTimeWarpRefresh = 0;

int dashDirection = 0;

using namespace std;

class gameState {
public:

	gameState() {

	}

	gameState(std::vector<float> pXP, std::vector<float> pYP, std::vector<bool> isCrouching)
		: playerXPositions(pXP), playerYPositions(pYP), crouching(isCrouching) {
	}

	void addPosition(float px, float py, bool isCrouching) {
		playerXPositions.push_back(px);
		playerYPositions.push_back(py);
		crouching.push_back(isCrouching);
	}

	void flipMostRecentPosition() {
		playerXPositions.back() *= -1;
		playerYPositions.back() *= -1;
	}

	std::vector<float> getXData() {
		return playerXPositions;
	}

	std::vector<float> getYData() {
		return playerYPositions;
	}

	std::vector<bool> getCrouching() {
		return crouching;
	}

private:
	std::vector<float> playerXPositions;
	std::vector<float> playerYPositions;
	std::vector<bool> crouching;
};

std::vector<gameState> timeline;

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

	canDash = true;
	bool crouching = false;

	float playerX = -1.0f;
	float playerY = -3.5f;

	int success = rendererInit();
	Shader tile_shader = makeTileShader();
	Shader player_shader = makePlayerShader();
	
	vector<vector<float>> tilemap = loadLevel(0);
	float movementMultiplier, fps;

	//updateTilemap(tilemap);
	glfwSwapInterval(1);

	while (true) {
		gameTime += 1;
		if (gameTime > latestTimeReached) {
			latestTimeReached = gameTime;
		}

		gameState currentGameState;

		//Only create a new gamestate if we are not in the past
		if (gameTime == latestTimeReached) {
			std::vector<float> packagedX;
			std::vector<float> packagedY;
			std::vector<bool> packagedCrouching;

			packagedX.push_back(playerX);
			packagedY.push_back(playerY);
			packagedCrouching.push_back(crouching);

			currentGameState = gameState(packagedX, packagedY, packagedCrouching);
		}

		else {
			timeline[gameTime].addPosition(playerX, playerY, crouching);
			currentGameState = timeline[gameTime];
		}

		std::vector<float> playerSpriteXPositions = currentGameState.getXData();
		std::vector<float> playerSpriteYPositions = currentGameState.getYData();
		std::vector<bool> playerCrouchingVector = currentGameState.getCrouching();
		deltaTime = render(tilemap, playerX, playerY, tile_shader, player_shader, playerSpriteXPositions, playerSpriteYPositions, playerCrouchingVector);

		//Flip most recent position
		//currentGameState.flipMostRecentPosition();
		timeline.push_back(currentGameState);

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
				dashEnd = gameTime + 15;
			}
			if (getKey(GLFW_KEY_T) && gameTime >= timeForTimeWarpRefresh) {
				gameTime -= 200;
				float newX = timeline[gameTime].getXData()[0];
				float newY = timeline[gameTime].getYData()[0];
				dashEnd = 0;
				playerX = newX;
				playerY = newY;
				timeForTimeWarpRefresh = gameTime + 200;
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

		int tempIndexMinus = static_cast<int>((playerX - blockX * 0.49f) * (blocksOnHalfScreenX * -1));
		int tempIndexPlus = static_cast<int>((playerX + blockX * 0.49f) * (blocksOnHalfScreenX * -1));

		//Check for blocks in your lower half for X axis collisions ALWAYS
		if (tilemap[indexOfFirstBlockY - 1][indexOfFirstBlockX - 1] != 0.0f) {
			if (playerXVelocity > 0.0f) {
				float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
				if (targetX - playerX < 0) {
					playerX = targetX;
					playerXVelocity = 0.0f;
					dashEnd = 0;
				}

			}
		}

		if (tilemap[indexOfFirstBlockY - 1][tempIndexMinus] != 0.0f) {
			if (playerXVelocity < 0.0f) {
				float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
				if (targetX - playerX > 0) {
					playerX = targetX;
					playerXVelocity = 0.0f;
					dashEnd = 0;
				}

			}
		}

		//Only check for head collisions if !crouching
		if (!crouching) {
			if (tilemap[indexOfFirstBlockY][indexOfFirstBlockX - 1] != 0.0f) {
				if (playerXVelocity > 0.0f) {
					float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
					if (targetX - playerX < 0) {
						playerX = targetX;
						playerXVelocity = 0.0f;
						dashEnd = 0;
					}

				}
			}

			if (tilemap[indexOfFirstBlockY][tempIndexMinus] != 0.0f) {
				if (playerXVelocity < 0.0f) {
					float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
					if (targetX - playerX > 0) {
						playerX = targetX;
						playerXVelocity = 0.0f;
						dashEnd = 0;
					}

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
		if (getKey(GLFW_KEY_LEFT_SHIFT)) {
			crouching = true;
		}
		else {
			crouching = false;
		}
		if (tilemap[indexOfFirstBlockY][tempIndexMinus] > 0.0f || tilemap[indexOfFirstBlockY][tempIndexPlus] > 0.0f) {
			std::cout << "FORCED CROUCH. X VELOCITY " << playerXVelocity << std::endl;
			crouching = true;
		}

		//When crouching
		if (!crouching) {
			if (tilemap[indexOfFirstBlockY+1][tempIndexMinus] <= 0.0f && tilemap[indexOfFirstBlockY+1][tempIndexPlus] <= 0.0f) {
				if (grounded && getKey(GLFW_KEY_SPACE)) {
					playerYVelocity = blockY * -5.0f;
				}
			}
			else {
				if (playerYVelocity < 0 && false) {
					playerYVelocity = 0;
				}
			}
		}
		else {
			//When not crouching
			if (tilemap[indexOfFirstBlockY][tempIndexMinus] <= 0.0f && tilemap[indexOfFirstBlockY][tempIndexPlus] <= 0.0f) {
				if (playerYVelocity < 0 && false) {
					playerYVelocity = 0;
				}
				if (grounded && getKey(GLFW_KEY_SPACE)) {
					playerYVelocity = blockY * -5.0f;
				}
			}
			else {
				if (playerYVelocity < 0 && false) {
					playerYVelocity = 0;
				}
			}
		}
	}
}