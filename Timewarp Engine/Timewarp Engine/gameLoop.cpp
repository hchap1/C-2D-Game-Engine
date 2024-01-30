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

bool red;
bool blue;
bool green;

bool grounded = false;
bool dashing = false;

bool canDash = true;

int dashEnd = 0;
int gameTime = 0;
int latestTimeReached = 0;
int timeForTimeWarpRefresh = 0;

int dashDirection = 0;

using namespace std;

bool doCollide(float blockID, bool redButtonIsPressed) {
	if (blockID == 0.0f || blockID == 0.4f || blockID == 0.5f || (blockID == 1.0f && redButtonIsPressed)) {
		return false;
	}
	return true;
}

void parseButton(float blockID) {
	if (blockID == 0.4f || blockID == 0.5f) {
		red = true;
	}
}

class gameState {
public:

	gameState() {
		playerXPositions = {};
		playerYPositions = {};
		crouching = {};
		redButton = false;
	}

	gameState(std::vector<float> pXP, std::vector<float> pYP, std::vector<bool> isCrouching, bool redButtonPressed)
		: playerXPositions(pXP), playerYPositions(pYP), crouching(isCrouching), redButton(redButtonPressed) {
	}

	void addPosition(float px, float py, bool isCrouching, bool redButtonPressed) {
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

	bool getRedButton() {
		return redButton;
	}

private:
	std::vector<float> playerXPositions;
	std::vector<float> playerYPositions;
	std::vector<bool> crouching;

	bool redButton;
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

		//Only create a new gamestate if we are not in the past. Otherwise modify and then read an old one.
		if (gameTime == latestTimeReached) {
			std::vector<float> packagedX;
			std::vector<float> packagedY;
			std::vector<bool> packagedCrouching;

			packagedX.push_back(playerX);
			packagedY.push_back(playerY);
			packagedCrouching.push_back(crouching);

			currentGameState = gameState(packagedX, packagedY, packagedCrouching, red);
		}

		else {
			timeline[gameTime].addPosition(playerX, playerY, crouching, red);
			currentGameState = timeline[gameTime];
		}

		//Prepare data from gamestate for rendering.
		std::vector<float> playerSpriteXPositions = currentGameState.getXData();
		std::vector<float> playerSpriteYPositions = currentGameState.getYData();
		std::vector<bool> playerCrouchingVector = currentGameState.getCrouching();
		bool redButtonIsPressed = currentGameState.getRedButton();
		red = redButtonIsPressed;

		red = false;

		//GL render function [see renderer.cpp and .h]. Updates buffers, draws triangles.
		deltaTime = render(tilemap, playerX, playerY, tile_shader, player_shader, 
			playerSpriteXPositions, playerSpriteYPositions, playerCrouchingVector,
			redButtonIsPressed);
	
		//Add our current game state to the timeline. [If this is a new gameframe].
		if (gameTime == latestTimeReached) { timeline.push_back(currentGameState); }
		
		//Timer-based dash system.
		if (dashEnd > gameTime) {
			dashing = true;
		}
		else {
			dashing = false;
		}
		//Saves performing multiple of the same operation later on.
		movementMultiplier = deltaTime * 15.0f * blockX;

		//Dashing exclusive inputs [movement, dash].
		//Jump is at the bottom, when we know if we are grounded or not.
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
		}

		//Once we start moving, always keep track of which direction we are facing.
		if (playerXVelocity > 0) {
			dashDirection = 1;
		}
		else if (playerXVelocity < 0) {
			dashDirection = -1;
		}

		//If our x velocity is small enough, just set it to zero.
		if (playerXVelocity < 0.01f && playerXVelocity > -0.01f) { playerXVelocity = 0.0f; }

		//Deltatime is in seconds. E.g. 0.016 ~ 60fps.
		fps = 1 / deltaTime;

		//Calculate the closest X to the player [index].
		int blocksOnHalfScreenX = static_cast<int>(1 / blockX);
		int indexOfFirstBlockX = static_cast<int>(playerX * (blocksOnHalfScreenX * -1));

		//Calculate the closest Y to the player [index].
		int blocksOnHalfScreenY = static_cast<int>(1 / blockY);
		int indexOfFirstBlockY = static_cast<int>(playerY * (blocksOnHalfScreenY * -1));

		//Potentially offset the indices of the blocks in the middle of the screen if the player covers more than one block.
		int tempIndexMinus = static_cast<int>((playerX - blockX * 0.49f) * (blocksOnHalfScreenX * -1));
		int tempIndexPlus = static_cast<int>((playerX + blockX * 0.49f) * (blocksOnHalfScreenX * -1));
		int tempIndexMinusBig = static_cast<int>((playerX - blockX * 0.5f) * (blocksOnHalfScreenX * -1));

		//For obstacle detection, crouch forcing, ect. Gives more clearance on either side of player.
		int tempIndexMinusSmall = static_cast<int>((playerX - blockX * 0.45f) * (blocksOnHalfScreenX * -1));
		int tempIndexPlusSmall = static_cast<int>((playerX + blockX * 0.45f) * (blocksOnHalfScreenX * -1));

		playerY += playerYVelocity * movementMultiplier;
		if (!dashing) { playerYVelocity += deltaTime * 15.0f * blockY; }
		if (dashing) { playerYVelocity = 0.0f; }

		grounded = false;

		float blockType = tilemap[indexOfFirstBlockY - 2][tempIndexMinus];
		float blockType2 = tilemap[indexOfFirstBlockY - 2][tempIndexPlus];

		if (doCollide(blockType, redButtonIsPressed) || doCollide(blockType2, red)) {
			if (playerYVelocity > 0) {
				float targetY = indexOfFirstBlockY * blockY * -1;;
				if (targetY - playerY < blockY / 4) {
					playerY = targetY;
					playerYVelocity = 0.0f;
					if (!dashing) { grounded = true; }
				}
			}
		}

		bool crouchedBecauseOfHeadhitter = false;
		blockType = tilemap[indexOfFirstBlockY + 1][tempIndexMinus];
		blockType2 = tilemap[indexOfFirstBlockY + 1][tempIndexPlus];
		
		if (doCollide(blockType, redButtonIsPressed) || doCollide(blockType2, red)) {
			if (playerYVelocity < 0) {
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
		else if (!crouchedBecauseOfHeadhitter) {
			crouching = false;
		}
		if (tilemap[indexOfFirstBlockY][tempIndexMinusSmall] > 0.0f || tilemap[indexOfFirstBlockY][tempIndexPlusSmall] > 0.0f) {
			crouching = true;
			std::cout << "forced crouch. XPOS: " << playerX << std::endl;
		}

		//When crouching
		if (!crouching) {
			if (tilemap[indexOfFirstBlockY + 1][tempIndexMinus] <= 0.0f && tilemap[indexOfFirstBlockY + 1][tempIndexPlus] <= 0.0f) {
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
			}
			else {
				if (playerYVelocity < 0 && false) {
					playerYVelocity = 0;
				}
			}
		}

		playerX += playerXVelocity * movementMultiplier;
		if (!dashing) { playerXVelocity *= 0.9f; }
		else { (playerXVelocity = blockX * dashDirection * movementMultiplier * 500.0f); }

		//Check for blocks in your lower half for X axis collisions ALWAYS
		blockType = tilemap[indexOfFirstBlockY - 1][indexOfFirstBlockX - 1];
		//parseButton(blockType);
		if (doCollide(blockType, redButtonIsPressed)) {
			if (playerXVelocity > 0.0f) {
				float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
				if (targetX - playerX < 0) {
					playerX = targetX;
					playerXVelocity = 0.0f;
					dashEnd = 0;
				}

			}
		}

		//Check for button presses
		blockType = tilemap[indexOfFirstBlockY - 1][tempIndexMinus];
		parseButton(blockType);
		blockType = tilemap[indexOfFirstBlockY - 1][tempIndexPlus];
		parseButton(blockType);

		blockType = tilemap[indexOfFirstBlockY - 1][tempIndexMinusBig];
		//parseButton(blockType);
		if (doCollide(blockType, redButtonIsPressed)) {
			std::cout << "COLLIDED" << std::endl;
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
			//Left wall @ head level
			blockType = tilemap[indexOfFirstBlockY][indexOfFirstBlockX - 1];
			if (doCollide(blockType, redButtonIsPressed)) {
				if (playerXVelocity > 0.0f) {
					float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
					if (targetX - playerX < 0) {
						playerX = targetX;
						playerXVelocity = 0.0f;
						dashEnd = 0;
					}

				}
			}
			//Right wall @ head level
			blockType = tilemap[indexOfFirstBlockY][tempIndexMinusBig];
			if (doCollide(blockType, red)) {
				if (playerXVelocity > 0.0f) {
					float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
					if (targetX - playerX > 0) {
						playerX = targetX;
						playerXVelocity = 0.0f;
						dashEnd = 0;
					}

				}
			}
		}

		if (getKey(GLFW_KEY_T)) {
			int index = gameTime;
			int speed = 1;
			int count = 0;
			while (getKey(GLFW_KEY_T)) {
				count++;
				index -= speed;
				if (count > 60 && speed < 2) {
					speed++;
				}
				if (count > 80 && speed < 3) {
					speed++;
				}
				if (count > 100 && speed < 4) {
					speed++;
				}
				if (count > 120 && speed < 5) {
					speed++;
				}
				gameState currentGameState = timeline[index];
				float pX = currentGameState.getXData().back();
				float pY = currentGameState.getYData().back();
				bool cRed = currentGameState.getRedButton();
				
				render(tilemap, pX, pY, tile_shader, player_shader, currentGameState.getXData(), currentGameState.getYData(), currentGameState.getCrouching(), cRed);
			}
			gameTime = index;
			gameState currentGameState = timeline[gameTime];
			playerX = currentGameState.getXData().front();
			playerY = currentGameState.getYData().front();
			playerXVelocity = 0.0f;
			playerYVelocity = 0.0f;
		}
	}
}