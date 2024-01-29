#pragma once
#include <vector>
#include <SHADER CLASS\shader.h>

float render(std::vector<std::vector<float>> tilemap, float playerX, float playerY, Shader tile_shader, Shader player_shader, std::vector<float> playerSpriteXPositions, std::vector<float> playerSpriteYPositions, std::vector<bool> playerCrouchingVector, bool red);
bool getKey(int keycode);
int rendererInit();
Shader makeTileShader();
Shader makePlayerShader();
void updateTilemap(std::vector<std::vector<float>> tilemap);