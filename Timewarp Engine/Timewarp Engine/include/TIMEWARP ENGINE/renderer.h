#pragma once
#include <vector>
#include <SHADER CLASS\shader.h>

float render(float playerX, float playerY, Shader tile_shader, Shader player_shader, std::vector<float> playerSpriteXPositions, std::vector<float> playerSpriteYPositions, std::vector<bool> playerCrouchingVector, bool red, bool green, bool blue);
bool getKey(int keycode);
int rendererInit(bool isGame);
Shader makeTileShader();
Shader makeSimpleTileShader();
Shader makePlayerShader();
Shader makeOutlineShader();
void updateTilemap(std::vector<std::vector<float>> tilemap);
float tilemapRender(float playerX, float playerY, std::vector<std::vector<float>> tilemap, Shader tile_shader, Shader outline_shader);