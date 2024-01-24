#pragma once
#include <vector>

void render(std::vector<std::vector<int>> tilemap, float playerX, float playerY);
bool getKey(int keycode);
int rendererInit();