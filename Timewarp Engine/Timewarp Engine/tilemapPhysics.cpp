#include <vector>
#include <TIMEWARP ENGINE/player.h>
#include <TIMEWARP ENGINE/newRenderer.h>
#include <TIMEWARP ENGINE/tilemapPhysics.h>

bool collide(int x, int y, Player* player, vector<vector<int>>* tilemap) {
	vector<vector<int>>& physicsTilemap = *tilemap;
	int blockID = physicsTilemap[y][x];
	if (blockID == 0) { return false; }
	if (blockID >= 4 && blockID <= 8) { return false; }
	if (blockID == 9 && player->getRedButton()) { return false; }
	if (blockID == 10 && player->getGreenButton()) { return false; }
	if (blockID == 11 && player->getBlueButton()) { return false; }
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

	 //Calculate the index ([[y][x]) of the blocks on each side of the player
	 int rightSideIndex = static_cast<int>((playerX + blockWidth * 0.5f) * (numberOfBlocksOnHalfScreenX * -1));
	 int leftSideIndex = static_cast<int>((playerX - blockWidth * 0.5f) * (numberOfBlocksOnHalfScreenX * -1));

	 //Change the players Y position, and then do floor related calculations.
	 playerY += playerYVel * deltaTime;

	 //Check the block indices below the players left and right side for collisions.
	 if (collide(leftSideIndex, indexOfMiddleBlockY - 2, player, tilemap) || collide(rightSideIndex, indexOfMiddleBlockY - 2, player, tilemap)) {
		 if (playerYVel > 0.0f) {
			 float targetY = indexOfMiddleBlockY * blockHeight * -1.0f;
			 if (targetY - playerY < 0.0f) {
				 cout << "RESETTING STUFF" << endl;
				 playerY = targetY;
				 playerYVel = 0.0f;
				 grounded = true;
			 }
		 }
	 }

	 playerX += playerXVel * deltaTime;

	 //Left wall @ foot level
	 if (collide(indexOfMiddleBlockX - 1, indexOfMiddleBlockY - 1, player, tilemap)) {
		 player->setOnWall(true);
		 float targetX = indexOfMiddleBlockX * blockWidth * -1 - blockWidth * 0.5f;
		 player->setCanDoubleJump(true);
		 player->setCanDash(true);
		 if (targetX - playerX < blockWidth * 0.05f) { player->setCanWallJump(true); }
		 if (!renderer->getKeyDown(GLFW_KEY_A)) { player->setCanWallJump(false); player->setOnWall(false); }
		 if (!player->getCanDash()) { player->setDashDirection(-1); }
		 if (playerXVel > 0.0f) {
			 if (targetX - playerX < 0) {
				 playerX = targetX;
				 playerXVel = 0.0f;
				 player->setDashEnd(0);
			 }
		 }
	 }

	 //Right wall @ foot level (when moving right)
	 if (collide(rightSideIndex, indexOfMiddleBlockY - 1, player, tilemap)) {
		 player->setOnWall(true);
		 float targetX = indexOfMiddleBlockX * blockWidth * -1 - blockWidth * 0.5f;
		 player->setCanDoubleJump(true);
		 player->setCanDash(true);
		 if (targetX - playerX > blockWidth * -0.05f) { player->setCanWallJump(true); }
		 if (!renderer->getKeyDown(GLFW_KEY_D)) { player->setCanWallJump(false); player->setOnWall(false; }
		 if (!dashing) { dashDirection = 1; }

		 if (playerXVelocity < 0.0f) {

			 if (targetX - playerX > 0) {
				 playerX = targetX;
				 playerXVelocity = 0.0f;
				 dashEnd = 0;
			 }

		 }
	 }

	 //Change the players X position, and then do wall related calculations
	 player->setX(playerX);
	 player->setY(playerY);
	 player->setXVelocity(playerXVel);
	 player->setYVelocity(playerYVel);
}