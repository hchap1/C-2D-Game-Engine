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
	RenderLayer tilemapRenderer({ 2, 2, 1 }, "tile", "blocks.png");
	RenderLayer playerRenderer({ 2, 2 }, "player", "player.png");
	 
	//Load tilemap data and store it to the tilemapRenderer
	vector<vector<int>> tilemap = loadTilemap(1);
	int* levelData = loadLevelData(1);
	player.setStart(renderer.getBlockWidth() * levelData[2] * -1.0f, renderer.getBlockHeight() * levelData[3] * -1.0f);
	player.goToStart();
	tuple<float*, int> vertexData = tilemapDecoder(tilemap, 32, 800, 600);
	tilemapRenderer.setVertices(get<0>(vertexData), get<1>(vertexData), 15, GL_STATIC_DRAW);

	glfwSwapInterval(1);
	while (renderer.isRunning()) {
		deltaTime = renderer.getDeltaTime();
		renderer.fillScreen(135, 206, 235);

		//Player physics
		player.physicsLoop(&renderer, deltaTime);
		calculatePhysics(&player, &renderer, &tilemap, deltaTime);

		//Draw tilemap first
		tilemapRenderer.setFloat("cameraX", player.getX());
		tilemapRenderer.setFloat("cameraY", player.getY());
		tilemapRenderer.draw(get<1>(vertexData));

		//Load and then draw player data
		GameState* currentGameState = player.constructGameState(); 
		cout << "PLAYER Y POSITION: " << player.getY() << endl;
		timeline.push_back(currentGameState);
		tuple<float*, int> vertexData = gameStateDecoder(*currentGameState, renderer);
		playerRenderer.setFloat("cameraX", player.getX());
		playerRenderer.setFloat("cameraY", player.getY());
		playerRenderer.setVertices(get<0>(vertexData), get<1>(vertexData), 12, GL_DYNAMIC_DRAW);
		playerRenderer.draw(get<1>(vertexData));

		//Update display
		renderer.updateDisplay();
	}
	
	return 0;
}