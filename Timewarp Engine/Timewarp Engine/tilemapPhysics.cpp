#include <vector>
#include <TIMEWARP ENGINE/player.h>
#include <TIMEWARP ENGINE/newRenderer.h>
#include <TIMEWARP ENGINE/tilemapPhysics.h>

bool collide(int x, int y, Player* player, vector<vector<int>>* tilemap) {
	//Find BLOCKID
	vector<vector<int>>& physicsTilemap = *tilemap;
	int blockID = physicsTilemap[y][x];

	//Check if it is a special block
	if (blockID == 4) { player->setRedButton(true); cout << colour::red << "BUTTON PRESSED!" << endl; }
	if (blockID == 6) { player->setGreenButton(true); cout << colour::green << "BUTTON PRESSED!" << endl;	}
	if (blockID == 8) { player->setBlueButton(true); cout << colour::green << "BUTTON PRESSED!" << endl;	}

	//Check if the block should collide
	if (blockID == 0) { return false; }
	if (blockID >= 4 && blockID <= 9) { return false; }
	if (blockID == 10 && player->getRedButton()) { return false; }
	if (blockID == 12 && player->getGreenButton()) { return false; }
	if (blockID == 14 && player->getBlueButton()) { return false; }
	return true;
}

void calculatePhysics(Player* player, Renderer* renderer, vector<vector<int>>* tilemap, float deltaTime) {
	 //Initialize temporary variables
	 float blockWidth = renderer->getBlockWidth();
	 float blockHeight = renderer->getBlockHeight();
	 float playerX = player->getX();
	 float playerY = player->getY();
	 float playerXVel = player->getXVel();
	 float playerYVel = player->getYVel();

	 bool grounded = false;

	 //Calculate the starting point for physics engine
	 int numberOfBlocksOnHalfScreenX = static_cast<int>(1 / blockWidth);
	 int numberOfBlocksOnHalfScreenY = static_cast<int>(1 / blockHeight);
	 int indexOfMiddleBlockX = static_cast<int>(playerX * (numberOfBlocksOnHalfScreenX * -1.0f));
	 int indexOfMiddleBlockY = static_cast<int>(playerY * (numberOfBlocksOnHalfScreenY * -1.0f));
	 int indexOfBlockInHeadY = static_cast<int>((playerY - blockHeight * 0.5f) * (numberOfBlocksOnHalfScreenY * -1.0f));

	 //Calculate the index ([[y][x]) of the blocks on each side of the player
	 int leftSideIndex = static_cast<int>((playerX + blockWidth * 0.51f) * (numberOfBlocksOnHalfScreenX * -1));
	 int leftSideIndexWithClearance = static_cast<int>((playerX + blockWidth * 0.49f) * (numberOfBlocksOnHalfScreenX * -1));
	 int rightSideIndex = static_cast<int>((playerX - blockWidth * 0.5f) * (numberOfBlocksOnHalfScreenX * -1));
	 int rightSideIndexWithClearance = static_cast<int>((playerX - blockWidth * 0.49f) * (numberOfBlocksOnHalfScreenX * -1));

	 playerX += playerXVel * deltaTime;

	 //Left wall @ foot level
	 if (collide(leftSideIndex, indexOfMiddleBlockY - 1, player, tilemap)) {

		 float targetX = indexOfMiddleBlockX * blockWidth * -1 - blockWidth * 0.5f;
		 player->setCanDoubleJump(true);
		 player->setCanDash(true);
		 if (targetX - playerX < 0.0f) { player->setCanWallJump(true); }
		 if (!renderer->getKeyDown(GLFW_KEY_A)) { player->setCanWallJump(false); false; }
		 if (!player->getDashing()) { player->setDashDirection(-1); }
		 if (playerXVel > 0.0f) {
			 if (targetX - playerX < 0) {
				 //player->setOnWall(true);
				 playerX = targetX;
				 playerXVel = 0.0f;
				 player->setDashEnd(0);
			 }
		 }
	 }

	 //Right wall @ foot level (when moving right)
	 if (collide(rightSideIndex, indexOfMiddleBlockY - 1, player, tilemap)) {
		 
		 float targetX = indexOfMiddleBlockX * blockWidth * -1 - blockWidth * 0.5f;
		 player->setCanDoubleJump(true);
		 player->setCanDash(true);
		 if (targetX - playerX > 0.0f) { player->setCanWallJump(true); }
		 if (!renderer->getKeyDown(GLFW_KEY_D)) { player->setCanWallJump(false); false; }
		 if (!player->getDashing()) { player->setDashDirection(1); }
		 if (playerXVel < 0.0f) {
			 if (targetX - playerX > 0) {
				 //player->setOnWall(true);
				 playerX = targetX;
				 playerXVel = 0.0f;
				 player->setDashEnd(0);
			 }
		 }
	 }

	 //Do the same but for the blocks above (head level) if crouching
	 if (!player->getCrouching()) {
		 //Left wall @ head level
		 if (collide(leftSideIndex, indexOfBlockInHeadY, player, tilemap)) {
			 //player->setOnWall(true);
			 float targetX = indexOfMiddleBlockX * blockWidth * -1 - blockWidth * 0.5f;
			 player->setCanDoubleJump(true);
			 player->setCanDash(true);
			 if (targetX - playerX < 0.0f) { player->setCanWallJump(true); }
			 if (!renderer->getKeyDown(GLFW_KEY_A)) { player->setCanWallJump(false); false; }
			 if (!player->getDashing()) { player->setDashDirection(-1); }
			 if (playerXVel > 0.0f) {
				 if (targetX - playerX < 0) {
					 playerX = targetX;
					 playerXVel = 0.0f;
					 player->setDashEnd(0);
				 }
			 }
		 }

		 //Right wall @ head level (when moving right)
		 if (collide(rightSideIndex, indexOfBlockInHeadY, player, tilemap)) {
			 //player->setOnWall(true);
			 float targetX = indexOfMiddleBlockX * blockWidth * -1 - blockWidth * 0.5f;
			 player->setCanDoubleJump(true);
			 player->setCanDash(true);
			 if (targetX - playerX > 0.0f) { player->setCanWallJump(true); }
			 if (!renderer->getKeyDown(GLFW_KEY_D)) { player->setCanWallJump(false); false; }
			 if (!player->getDashing()) { player->setDashDirection(1); }
			 if (playerXVel < 0.0f) {
				 if (targetX - playerX > 0) {
					 playerX = targetX;
					 playerXVel = 0.0f;
					 player->setDashEnd(0);
				 }
			 }
		 }

		 //Left wall @ head level
		 if (collide(leftSideIndex, indexOfMiddleBlockY, player, tilemap)) {
			 player->setOnWall(true);
			 float targetX = indexOfMiddleBlockX * blockWidth * -1 - blockWidth * 0.5f;
			 player->setCanDoubleJump(true);
			 player->setCanDash(true);
			 if (targetX - playerX < 0.0f) { player->setCanWallJump(true); }
			 if (!renderer->getKeyDown(GLFW_KEY_A)) { player->setCanWallJump(false); player->setOnWall(false); }
			 if (!player->getDashing()) { player->setDashDirection(-1); }
			 if (playerXVel > 0.0f) {
				 if (targetX - playerX < 0) {
					 playerX = targetX;
					 playerXVel = 0.0f;
					 player->setDashEnd(0);
				 }
			 }
		 }

		 //Right wall @ head level (when moving right)
		 if (collide(rightSideIndex, indexOfMiddleBlockY, player, tilemap)) {
			 player->setOnWall(true);
			 float targetX = indexOfMiddleBlockX * blockWidth * -1 - blockWidth * 0.5f;
			 player->setCanDoubleJump(true);
			 player->setCanDash(true);
			 if (targetX - playerX > 0.0f) { player->setCanWallJump(true); }
			 if (!renderer->getKeyDown(GLFW_KEY_D)) { player->setCanWallJump(false); player->setOnWall(false); }
			 if (!player->getDashing()) { player->setDashDirection(1); }
			 if (playerXVel < 0.0f) {
				 if (targetX - playerX > 0) {
					 playerX = targetX;
					 playerXVel = 0.0f;
					 player->setDashEnd(0);
				 }
			 }
		 }
	 }

	 //Only do Y axis movement if !dashing
	 if (player->getDashing()) { cout << "DASHING!"; }
	 if (!player->getDashing()) {
		 //Change the players Y position, and then do floor related calculations.
		 playerY += playerYVel * deltaTime;

		 //Check the block indices below the players left and right side for collisions.
		 if (collide(leftSideIndexWithClearance, indexOfMiddleBlockY - 2, player, tilemap) || collide(rightSideIndexWithClearance, indexOfMiddleBlockY - 2, player, tilemap)) {
			 if (playerYVel > 0.0f) {
				 float targetY = indexOfMiddleBlockY * blockHeight * -1.0f;
				 if (targetY - playerY < 0.0f) {
					 playerY = targetY;
					 playerYVel = 0.0f;
					 grounded = true;
				 }
			 }
		 }

		 //Make sure the player cannot clip through blocks above their head.
		 if (collide(leftSideIndexWithClearance, indexOfBlockInHeadY + 1, player, tilemap)) {
			 if (playerYVel < 0.0f) {
				 float targetY = indexOfMiddleBlockY * blockHeight * -1.0f;
				 float targetX = indexOfMiddleBlockX * blockWidth * -1.0f;
				 if (targetY - playerY > blockHeight && targetX - playerX > 0) {
					 playerY = targetY - blockHeight;
					 playerYVel = 0.0f;
				 }
			 }
		 }

		 if (collide(rightSideIndexWithClearance, indexOfBlockInHeadY + 1, player, tilemap)) {
			 if (playerYVel < 0.0f) {
				 float targetY = indexOfMiddleBlockY * blockHeight * -1.0f;
				 float targetX = indexOfMiddleBlockX * blockWidth * -1.0f;
				 if (targetY - playerY > blockHeight) {//&& targetX - playerX < 0) {
					 playerY = targetY - blockHeight;
					 playerYVel = 0.0f;
				 }
			 }
		 }

		 if (collide(rightSideIndexWithClearance, indexOfBlockInHeadY + 1, player, tilemap)) {
			 if (playerYVel < 0.0f) {
				 float targetY = indexOfMiddleBlockY * blockHeight * -1.0f;
				 float targetX = indexOfMiddleBlockX * blockWidth * -1.0f;
				 if (targetY - playerY > blockHeight) {//&& targetX - playerX < 0) {
					 playerY = targetY - blockHeight;
					 playerYVel = 0.0f;
				 }
			 }
		 }
	 }

	 //Change the players X position, and then do wall related calculations
	 player->setGrounded(grounded);
	 if (grounded || player->getOnWall()) { player->setCanDoubleJump(true); player->setCanDash(true); }
	 player->setX(playerX);
	 player->setY(playerY);
	 if (!player->getDashing()) { player->setXVelocity(playerXVel); }
	 else { player->setXVelocity(player->getDashDirection() * blockWidth * 15.0f); playerYVel = 0.0f; }
	 player->setYVelocity(playerYVel);
}