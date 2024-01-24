#include <TIMEWARP ENGINE\loadTilemap.h>
#include <iostream>
#include <vector>
#include <TIMEWARP ENGINE\renderer.h>
#include <SHADER CLASS\shader.h>
#include <GLFW\glfw3.h>
using namespace std;

int main() {
	float deltaTime;
	float playerX = 0.0f;
	float playerY = 0.0f;
	int success = rendererInit();
	Shader basic_shader = makeShader();
	
	vector<vector<float>> tilemap = loadLevel(0);

	updateTilemap(tilemap);

	while (true) {
		deltaTime = render(tilemap, playerX, playerY, basic_shader);
		if (getKey(GLFW_KEY_W)) {
			playerY += deltaTime;
		}
		if (getKey(GLFW_KEY_S)) {
			playerY -= deltaTime;
		}
		if (getKey(GLFW_KEY_D)) {
			playerX += deltaTime;
		}
		if (getKey(GLFW_KEY_A)) {
			playerX -= deltaTime;
		}
	}
}