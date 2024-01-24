#include <TIMEWARP ENGINE\loadTilemap.h>
#include <iostream>
#include <vector>
#include <TIMEWARP ENGINE\renderer.h>
#include <SHADER CLASS\shader.h>
using namespace std;

int main() {
	Shader basic_shader("src/shaders/vertex_shader.txt", "src/shaders/fragment_shader.txt");
	basic_shader.use();
	float playerX = 0.0f;
	int success = rendererInit();
	vector<vector<int>> tilemap = loadLevel(0);
	while (true) {
		playerX += 0.01f;
		render(tilemap, playerX, 0.5f, basic_shader);
	}
}