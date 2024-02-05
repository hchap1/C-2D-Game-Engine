#define STB_IMAGE_IMPLEMENTATION
#include <TIMEWARP ENGINE/loadTilemap.h>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <tuple>
#include <TIMEWARP ENGINE/newRenderer.h>

using namespace std;

int main() {

	Renderer renderer(800, 600, "Timewarp");
	RenderLayer tilemapRenderer({ 2, 2, 1 }, "tilemap", "blocks.png");

	vector<vector<int>> tilemap = loadTilemap(0);
	tuple<float*, int> vertexData = tilemapDecoder(tilemap, 32, 800, 600);
	tilemapRenderer.setVertices(get<0>(vertexData), GL_STATIC_DRAW);

	while (renderer.isRunning()) {
		renderer.fillScreen(135, 206, 235);
		tilemapRenderer.draw(get<1>(vertexData));
		renderer.updateDisplay();
	}
	
	return 0;
}