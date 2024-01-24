#pragma once
#include <vector>
#include <SHADER CLASS\shader.h>

float render(std::vector<std::vector<float>> tilemap, float playerX, float playerY, Shader basic_shader);
bool getKey(int keycode);
int rendererInit();
Shader makeShader();
void updateTilemap(std::vector<std::vector<float>> tilemap);