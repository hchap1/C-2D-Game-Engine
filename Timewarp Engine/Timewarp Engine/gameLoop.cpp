#include <TIMEWARP ENGINE\loadTilemap.h>
#include <iostream>
#include <vector>
#include <TIMEWARP ENGINE\renderer.h>
using namespace std;

int main() {
	int success = rendererInit();
	vector<vector<int>> tilemap = loadLevel(0);
	while (true) {
		render(tilemap, 0.5f, 0.5f);
	}
}