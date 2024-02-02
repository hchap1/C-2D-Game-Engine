#include <TIMEWARP ENGINE\loadTilemap.h>
#include <iostream>
#include <vector>
#include <TIMEWARP ENGINE\renderer.h>
#include <SHADER CLASS\shader.h>
#include <GLFW\glfw3.h>

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
bool canDoubleJump = true;
bool canWallJump = true;
bool canDash = true;

int LID;

int dashEnd = 0;
int nextDash = 0;
int gameTime = 0;
int latestTimeReached = 0;
int timeForTimeWarpRefresh = 0;

float jumpHeight = -7.0f;
float startX, startY;
float playerX, playerY;
float playerXVelocity;
float playerYVelocity;
class gameState {
public:

	gameState() {
		playerXPositions = {};
		playerYPositions = {};
		crouching = {};
		redButton = false;
		greenButton = false;
		blueButton = false;
	}

	gameState(std::vector<float> pXP, std::vector<float> pYP, std::vector<bool> isCrouching,
		bool redButtonPressed, bool greenButtonPressed, bool blueButtonPressed)
		: playerXPositions(pXP), playerYPositions(pYP), crouching(isCrouching), redButton(redButtonPressed),
		greenButton(greenButtonPressed), blueButton(blueButtonPressed) {
	}

	void addPosition(float px, float py, bool isCrouching, bool redButtonPressed,
		bool greenButtonPressed, bool blueButtonPressed) {
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
	bool getGreenButton() {
		return greenButton;
	}
	bool getBlueButton() {
		return blueButton;
	}

private:
	std::vector<float> playerXPositions;
	std::vector<float> playerYPositions;
	std::vector<bool> crouching;

	bool redButton;
	bool greenButton;
	bool blueButton;
};


vector<vector<float>> tilemap;
std::vector<gameState> timeline;

float colorMultiplier[3] = { 1.0f, 1.0f, 1.0f };
/*
map<std::string, float> blockIDs = {
	"RedButton":0.4f
}
*/
int dashDirection = 0;

bool spaceDown = false;
bool spaceTracker = false;

bool apostropheDown = false;
bool apostropheTracker = false;

using namespace std;

bool doCollide(float blockID, bool redButtonIsPressed, bool greenButtonIsPressed, bool blueButtonIsPressed) {
	if (blockID == 0.0f || (blockID >= 0.4f && blockID <= 0.9f)) {
		return false;
	}
	if ((blockID == 1.0f && redButtonIsPressed) || (blockID == 1.2f && greenButtonIsPressed) || (blockID == 1.4f && blueButtonIsPressed)) {
		return false;
	}
	if (blockID == 2.0f) { return false; }
	return true;
}

void die(Shader tile_shader, Shader player_shader, Shader parallax_shader);

void parseSpecialBlocks(float blockID, Shader tile_shader, Shader player_shader, Shader parallax_shader) {
	if (blockID == 0.4f || blockID == 0.5f) { red = true; }
	if (blockID == 0.6f || blockID == 0.7f) { green = true; }
	if (blockID == 0.8f || blockID == 0.9f) { blue = true; }
	if (blockID >= 1.6f && blockID <= 1.9f) { die(tile_shader, player_shader, parallax_shader); }
	if (blockID == 2.0f) {
		tilemap = loadLevel(LID + 1);
		updateTilemap(tilemap);
		int* levelData = loadLevelData(LID);
		startX = blockX * levelData[2] * -1;
		startY = blockY * levelData[3] * -1;

		colorMultiplier[0] = 1.0f;
		colorMultiplier[1] = 1.0f;
		colorMultiplier[2] = 1.0f;

		playerX = startX;
		playerY = startY;
		playerXVelocity = 0.0f;
		playerYVelocity = 0.0f;
		timeline.clear();
		gameTime = 0;
		latestTimeReached = 0;
		dashEnd = 0;
		dashing = false;
		nextDash = 0;

	}
}

void setBlockSize(float bx, float by, float w, float h) {
	blockX = bx;

	blockY = by;
	width = w;
	height = h;
}

void die(Shader tile_shader, Shader player_shader, Shader parallax_shader) {
	colorMultiplier[0] = 0.9f;
	colorMultiplier[1] = 0.5f;
	colorMultiplier[2] = 0.5f;
	int index = gameTime;
	int speed = 1;
	int count = 0;
	while (index > 10) {
		index -= speed;
		if (getKey(GLFW_KEY_SPACE)) {
			index -= 3;
		}
		gameState currentGameState = timeline[index];
		float pX = currentGameState.getXData().back();
		float pY = currentGameState.getYData().back();
		bool cRed = currentGameState.getRedButton();
		bool cGreen = currentGameState.getGreenButton();
		bool cBlue = currentGameState.getBlueButton();

		render(playerX, playerY, tile_shader, player_shader, currentGameState.getXData(), currentGameState.getYData(), currentGameState.getCrouching(), cRed, cGreen, cBlue, colorMultiplier, parallax_shader);
	}

	colorMultiplier[0] = 1.0f;
	colorMultiplier[1] = 1.0f;
	colorMultiplier[2] = 1.0f;

	playerX = startX;
	playerY = startY;
	playerXVelocity = 0.0f;
	playerYVelocity = 0.0f;
	timeline.clear();
	gameTime = 0;
	latestTimeReached = 0;
	dashEnd = 0;
	dashing = false;
	nextDash = 0;
}

int gameMain(int levelID) {
	LID = levelID;
	msPerFrame = 1 / targetFps;

	float deltaTime;
	playerXVelocity = 0.0f;
	playerYVelocity = 0.0f;

	canDash = true;
	bool crouching = false;

	playerX = -1.0f;
	playerY = -3.0f;

	int success = rendererInit(true);
	Shader tile_shader = makeTileShader();
	Shader player_shader = makePlayerShader();
	Shader parallax_shader = makeParallaxShader();
	
	tilemap = loadLevel(levelID);
	float movementMultiplier, fps;

	updateTilemap(tilemap);
	glfwSwapInterval(1);

	int* levelData = loadLevelData(levelID);
	startX = blockX * levelData[2] * -1;
	startY = blockY * levelData[3] * -1;
	playerX = startX;
	playerY = startY;

	float blockType, blockType2;

	while (true) {
		spaceDown = false;
		if (getKey(GLFW_KEY_SPACE)) {
			if (!spaceTracker) {
				spaceTracker = true;
				spaceDown = true;
			}
		
		}
		else { spaceTracker = false; }

		apostropheDown = false;
		if (getKey(GLFW_KEY_APOSTROPHE)) {
			if (!apostropheTracker) {
				apostropheTracker = true;
				apostropheDown = true;
			}

		}
		else { apostropheTracker = false; }

		gameTime += 1;
		if (gameTime > latestTimeReached) { latestTimeReached = gameTime; }

		gameState currentGameState;

		//Only create a new gamestate if we are not in the past. Otherwise modify and then read an old one.
		if (gameTime == latestTimeReached) {
			std::vector<float> packagedX;
			std::vector<float> packagedY;
			std::vector<bool> packagedCrouching;

			packagedX.push_back(playerX);
			packagedY.push_back(playerY);
			packagedCrouching.push_back(crouching);

			currentGameState = gameState(packagedX, packagedY, packagedCrouching, red, green, blue);
		}

		else {
			timeline[gameTime].addPosition(playerX, playerY, crouching, red, green, blue);
			currentGameState = timeline[gameTime];
		}

		//Prepare data from gamestate for rendering.
		std::vector<float> playerSpriteXPositions = currentGameState.getXData();
		std::vector<float> playerSpriteYPositions = currentGameState.getYData();
		std::vector<bool> playerCrouchingVector = currentGameState.getCrouching();
		bool redButtonIsPressed = currentGameState.getRedButton();
		bool greenButtonIsPressed = currentGameState.getGreenButton();
		bool blueButtonIsPressed = currentGameState.getBlueButton();

		red = redButtonIsPressed;
		green = greenButtonIsPressed;
		blue = blueButtonIsPressed;

		//Not used as of now
		red = false;
		green = false;
		blue = false;

		//GL render function [see renderer.cpp and .h]. Updates buffers, draws triangles.
		deltaTime = render(playerX, playerY, tile_shader, player_shader, 
			playerSpriteXPositions, playerSpriteYPositions, playerCrouchingVector,
			redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed, colorMultiplier,
			parallax_shader);
	
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
				dashDirection = -1;
			}
			if (getKey(GLFW_KEY_A)) {
				playerXVelocity += movementMultiplier;
				dashDirection = 1;
			}
		}

		//Once we start moving, always keep track of which direction we are facing.

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

		//Offset the Y index by 0.5 of the size of a block [For jumping collisions].
		int indexOfBlockAboveHead = static_cast<int>((playerY - blockY * 0.5f) * (blocksOnHalfScreenY * -1));

		//Potentially offset the indices of the blocks in the middle of the screen if the player covers more than one block.
		int tempIndexMinus = static_cast<int>((playerX - blockX * 0.49f) * (blocksOnHalfScreenX * -1));
		int tempIndexPlus = static_cast<int>((playerX + blockX * 0.49f) * (blocksOnHalfScreenX * -1));
		int tempIndexPlusBig = static_cast<int>((playerX + blockX * 0.5f) * (blocksOnHalfScreenX * -1));
		int tempIndexMinusBig = static_cast<int>((playerX - blockX * 0.5f) * (blocksOnHalfScreenX * -1));

		//For obstacle detection, crouch forcing, ect. Gives more clearance on either side of player.
		int tempIndexMinusSmall = static_cast<int>((playerX - blockX * 0.45f) * (blocksOnHalfScreenX * -1));
		int tempIndexPlusSmall = static_cast<int>((playerX + blockX * 0.45f) * (blocksOnHalfScreenX * -1));


		//Change player Y by y velocity. Only do gravity if !dashing.
		playerY += playerYVelocity * movementMultiplier;
		if (!dashing) { playerYVelocity += deltaTime * 15.0f * blockY; } //Gravity
		if (dashing) { playerYVelocity = 0.0f; }

		//Start as if we are not on the ground.
		grounded = false;


		//Do collide returns true if the block should be collided with. 
		//Ground collision [set player position to standing on top of a block
		//if the player collided with it whilst moving downwards.


		//First et the block type [float] of the block below your 'right' foot and below your 'left' foot for spike testing.
		blockType = tilemap[indexOfFirstBlockY - 2][tempIndexMinusSmall];
		blockType2 = tilemap[indexOfFirstBlockY - 2][tempIndexPlusSmall];

		float targetY = indexOfFirstBlockY * blockY * -1;

		if (targetY - playerY < blockY / 8 && !dashing) {
			if (((blockType >= 1.6f && blockType <= 1.9f) || blockType == 0.0f) && ((blockType2 >= 1.6f && blockType2 <= 1.9f) || blockType2 == 0.0f)) {
				if (!(blockType + blockType2 == 0.0f)) { die(tile_shader, player_shader, parallax_shader); }
			}
		}


		//Get the block type [float] of the block below your 'right' foot and below your 'left' foot.
		blockType = tilemap[indexOfFirstBlockY - 2][tempIndexMinus];
		blockType2 = tilemap[indexOfFirstBlockY - 2][tempIndexPlus];

		if (doCollide(blockType, redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed) || doCollide(blockType2, redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed)) {
			if (playerYVelocity > 0) {
				if (targetY - playerY < blockY / 8) {
					playerY = targetY;
					playerYVelocity = 0.0f;
					if (!dashing) { grounded = true; }
				}
			}
		}

		//Redefine block types for the blocks above your head to stop the player from
		//jumping through them. Also allows force crouch.

		blockType = tilemap[indexOfBlockAboveHead+1][tempIndexMinus];
		blockType2 = tilemap[indexOfBlockAboveHead+1][tempIndexPlus];

		//tilemap[indexOfFirstBlockY][tempIndexPlus] = 0.4f;
		//updateTilemap(tilemap);
		//Blocks closer to your head.
		if (doCollide(blockType, redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed) || doCollide(blockType2, redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed)) {
			if (playerYVelocity < 0) {
				float targetY = (indexOfFirstBlockY)*blockY * -1;
				
				if (targetY - playerY < blockY / 2) {
					playerY = targetY;
					playerYVelocity = 0.0f;
					if (!dashing) { grounded = true; }
				}
			}
		}
		if (grounded) {
			canWallJump = false;
			canDoubleJump = true;
		}
		//Crouch whenever the users wants to as of right now. Restrictions can be added later.
		if (getKey(GLFW_KEY_LEFT_SHIFT)) { crouching = true; }
		else { crouching = false; }
		if (tilemap[indexOfFirstBlockY][tempIndexMinusBig] > 0.0f && false || tilemap[indexOfFirstBlockY][indexOfFirstBlockX-1] > 0.0f && false) {
			crouching = true;
		}

		//Allow the player to jump if they are grounded and below a non-collidable block.
		if (!crouching) {
			if (tilemap[indexOfFirstBlockY + 1][tempIndexMinus] <= 0.0f && tilemap[indexOfFirstBlockY + 1][tempIndexPlus] <= 0.0f) {
				if (grounded && spaceDown) {
					playerYVelocity = blockY * jumpHeight;
				}
			}
		}







		//Move on to x calculations. The Y was done first because we know any incurred collisions were a result of y changes.
		bool onWall = false;
		playerX += playerXVelocity * movementMultiplier;
		if (!dashing) { playerXVelocity *= 0.9f; }
		else { (playerXVelocity = blockX * dashDirection * movementMultiplier * 300.0f); }
		
		blockType = tilemap[indexOfFirstBlockY - 1][tempIndexMinus];
		parseSpecialBlocks(blockType, tile_shader, player_shader, parallax_shader);
		blockType = tilemap[indexOfFirstBlockY - 1][tempIndexPlus];
		parseSpecialBlocks(blockType, tile_shader, player_shader, parallax_shader);

		//Left wall @ foot level
		blockType = tilemap[indexOfFirstBlockY - 1][indexOfFirstBlockX - 1];
		if (doCollide(blockType, redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed)) {
			onWall = true;
			float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
			canDoubleJump = true;
			canDash = true;
			if (targetX - playerX < blockX * 0.05f) {
				canWallJump = true;
			}
			if (!getKey(GLFW_KEY_A)) { canWallJump = false; onWall = false; }
			if (!dashing) { dashDirection = -1; }

			if (playerXVelocity > 0.0f) {

				if (targetX - playerX < 0) {
					parseSpecialBlocks(blockType, tile_shader, player_shader, parallax_shader);
					playerX = targetX;
					playerXVelocity = 0.0f;
					dashEnd = 0;
				}

			}
		}

		//Check for button presses.

		//Right wall @ foot level (when moving right)
		blockType = tilemap[indexOfFirstBlockY - 1][tempIndexMinusBig];
		if (doCollide(blockType, redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed)) {
			onWall = true;
			canDoubleJump = true;
			canDash = true;
			float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
			if (targetX - playerX > blockX * -0.05f) {
				canWallJump = true;
			}
			if (!getKey(GLFW_KEY_D)) { canWallJump = false; onWall = false; }
			if (!dashing) { dashDirection = 1; }

			if (playerXVelocity < 0.0f) {

				if (targetX - playerX > 0) {
					playerX = targetX;
					playerXVelocity = 0.0f;
					dashEnd = 0;
				}

			}
		}

		blockType = tilemap[indexOfFirstBlockY - 1][indexOfFirstBlockX - 1];
		//parseSpecialBlocks(blockType, tile_shader, player_shader, parallax_shader);

		

		//Only check for head collisions if !crouching
		if (!crouching) {
			blockType = tilemap[indexOfBlockAboveHead][tempIndexMinus];
			parseSpecialBlocks(blockType, tile_shader, player_shader, parallax_shader);
			blockType = tilemap[indexOfBlockAboveHead][tempIndexPlus];
			parseSpecialBlocks(blockType, tile_shader, player_shader, parallax_shader);
			//Left wall @ head level
			//blockType = tilemap[indexOfFirstBlockY][indexOfFirstBlockX - 1];
			blockType = tilemap[indexOfBlockAboveHead][indexOfFirstBlockX - 1];
			if (doCollide(blockType, redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed)) {
				onWall = true;
				float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
				float targetY = indexOfBlockAboveHead * blockY * -1 - blockY;
				canDoubleJump = true;
				canDash = true;
				if (targetX - playerX < blockX * 0.05f) {
					canWallJump = true;
				}
				if (!getKey(GLFW_KEY_A)) { canWallJump = false; onWall = false; }
				if (!dashing) { dashDirection = -1; }

				if (playerXVelocity > 0.0f) {

					if (targetX - playerX < 0) {
						playerX = targetX;
						playerXVelocity = 0.0f;
						dashEnd = 0;
					}

				}
			}
			//Right wall @ head level
			//blockType = tilemap[indexOfFirstBlockY][tempIndexMinusBig];
			blockType = tilemap[indexOfBlockAboveHead][tempIndexMinusBig];
			if (doCollide(blockType, redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed)) {
				onWall = true;
				canDoubleJump = true;
				canDash = true;
				float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
				float targetY = indexOfBlockAboveHead * blockY * -1 - blockY;
				if (targetX - playerX > blockX * -0.05f) {
					canWallJump = true;
				}
				if (!getKey(GLFW_KEY_D)) { canWallJump = false; onWall = false; }
				if (!dashing) { dashDirection = 1; }

				if (playerXVelocity < 0.0f) {

					if (targetX - playerX > 0) {
						playerX = targetX;
						playerXVelocity = 0.0f;
						dashEnd = 0;
					}

				}
			}

			//Check in middle of player (fixes some wall-noclip glitches).
			blockType = tilemap[indexOfFirstBlockY][indexOfFirstBlockX - 1];
			if (doCollide(blockType, redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed)) {
				onWall = true;
				float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
				if (playerXVelocity > 0.0f) {
					if (targetX - playerX < 0) {
						playerX = targetX;
						playerXVelocity = 0.0f;
						dashEnd = 0;
					}
				}
			}
			blockType = tilemap[indexOfBlockAboveHead][tempIndexMinusBig];
			if (doCollide(blockType, redButtonIsPressed, greenButtonIsPressed, blueButtonIsPressed)) {
				onWall = true;
				float targetX = indexOfFirstBlockX * blockX * -1 - blockX * 0.5f;
				if (playerXVelocity < 0.0f) {
					if (targetX - playerX > 0) {
						playerX = targetX;
						playerXVelocity = 0.0f;
						dashEnd = 0;
					}
				}
			}
		}









		if (!onWall) { canWallJump = false; }

		if (canDoubleJump && spaceDown && !grounded && !canWallJump) {
			playerYVelocity = blockY * jumpHeight;
			canDoubleJump = false;
			canDash = true;
		}

		if (canWallJump) {
			if (spaceDown) {
				canWallJump = false;
				playerYVelocity = blockY * jumpHeight;
				playerXVelocity = blockX * 5.0f * dashDirection;
			}
		}

		if (grounded) {
			canDash = true;
		}

		if (canWallJump && playerYVelocity > 0) {
			playerYVelocity *= 0.8f;
		}

		if (apostropheDown && canDash && gameTime >= nextDash && !dashing) {
			dashEnd = gameTime + 20;
			nextDash = dashEnd + 30;
			canDash = false;
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
				bool cGreen = currentGameState.getGreenButton();
				bool cBlue = currentGameState.getBlueButton();
				
				render(playerX, playerY, tile_shader, player_shader, currentGameState.getXData(), currentGameState.getYData(), 
					currentGameState.getCrouching(), cRed, cGreen, cBlue, colorMultiplier, parallax_shader);
			}
			gameTime = index;
			gameState currentGameState = timeline[gameTime];
			playerX = currentGameState.getXData().front();
			playerY = currentGameState.getYData().front();
			playerXVelocity = 0.0f;
			playerYVelocity = 0.0f;
			dashEnd = gameTime;
			nextDash = gameTime;
		}

		std::cout << "FPS: " << fps << std::endl;
	}
}