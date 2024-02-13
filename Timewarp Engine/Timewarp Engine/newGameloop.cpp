#define STB_IMAGE_IMPLEMENTATION
#include <TIMEWARP ENGINE/loadTilemap.h>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <tuple>
#include <TIMEWARP ENGINE/newRenderer.h>
#include <TIMEWARP ENGINE/player.h>
#include <TIMEWARP ENGINE/tilemapPhysics.h>

using namespace std;

int main() {
	//Initialize variables
	vector<GameState*> timeline;
	int gameTime = 0;
	int latestGameTime = 0;
	float deltaTime;

	//Initialize player object
	Player player(0.0f, 0.0f);

	//Initialize rendering pipeline
	Renderer renderer(800, 600, "Timewarp");
	RenderLayer tilemapRenderer({ 2, 2, 1 }, "tile", "blocks.png", false);
	RenderLayer playerRenderer({ 2, 2 }, "player", "player.png", true);
	 
	//Load tilemap data and store it to the tilemapRenderer
	vector<vector<int>> tilemap = loadTilemap(1);
	int* levelData = loadLevelData(1);
	player.setStart(renderer.getBlockWidth() * levelData[2] * -1.0f, renderer.getBlockHeight() * levelData[3] * -1.0f);
	player.goToStart();
	tuple<float*, int> vertexData = tilemapDecoder(tilemap, 32, 800, 600);
	int numberOfTriangles = get<1>(vertexData);
	tilemapRenderer.setVertices(get<0>(vertexData), get<1>(vertexData), 15, GL_STATIC_DRAW);

	glfwSwapInterval(1);
	while (renderer.isRunning()) {
		if (gameTime > latestGameTime) { latestGameTime = gameTime; }

		deltaTime = renderer.getDeltaTime();
		renderer.fillScreen(135, 206, 235);

		//Player physics
		if (gameTime == latestGameTime) { player.physicsLoop(&renderer, deltaTime, gameTime, {false}); }
		else { player.physicsLoop(&renderer, deltaTime, gameTime, timeline[gameTime]->getButtonStates()); }
		cout << "DID PLAYER LOOP" << endl;
		calculatePhysics(&player, &renderer, &tilemap, deltaTime);
		cout << "DID PHYSICS LOOP" << endl;

		//Draw tilemap first
		tilemapRenderer.setFloat("cameraX", player.getX());
		tilemapRenderer.setFloat("cameraY", player.getY());
		tilemapRenderer.draw(get<1>(vertexData));

		GameState* currentGameState;

		//Load and then draw player data
		if (latestGameTime == gameTime) {
			currentGameState = player.constructGameState();
			timeline.push_back(currentGameState);
		}
		else {
			currentGameState = timeline[gameTime];
			player.injectDataIntoGameState(currentGameState);
		}
		tuple<float*, int> vertexData = gameStateDecoder(*currentGameState, renderer);
		playerRenderer.setFloat("cameraX", player.getX());
		playerRenderer.setFloat("cameraY", player.getY());
		playerRenderer.setVertices(get<0>(vertexData), get<1>(vertexData), 12, GL_DYNAMIC_DRAW);
		playerRenderer.draw(get<1>(vertexData));

		//Update display
		gameTime++;
		renderer.updateDisplay();

		if (renderer.getKeyDown(GLFW_KEY_T)) { 
			GameState* rewindGameState;
			while (renderer.getKeyDown(GLFW_KEY_T)) {
				gameTime--;
				renderer.fillScreen(135, 206, 235);
				rewindGameState = timeline[gameTime];

				player.setX(rewindGameState->getXPositions().back());
				player.setY(rewindGameState->getYPositions().back());

				float playerX = player.getX();
				float playerY = player.getY();

				//Draw tilemap first
				tilemapRenderer.setFloat("cameraX", playerX);
				tilemapRenderer.setFloat("cameraY", playerY);
				tilemapRenderer.draw(numberOfTriangles);

				//Then draw the player
				tuple<float*, int> vertexData = gameStateDecoder(*rewindGameState, renderer);
				playerRenderer.setFloat("cameraX", playerX);
				playerRenderer.setFloat("cameraY", playerY);
				playerRenderer.setVertices(get<0>(vertexData), get<1>(vertexData), 12, GL_DYNAMIC_DRAW);
				playerRenderer.draw(get<1>(vertexData));
				
				renderer.getDeltaTime();
				renderer.updateDisplay();
			}
			rewindGameState = timeline[gameTime];
			player.setX(rewindGameState->getXPositions().back());
			player.setY(rewindGameState->getYPositions().back());
			player.setXVelocity(0.0f);
			player.setYVelocity(0.0f);

		}
	}
	
	return 0;
}