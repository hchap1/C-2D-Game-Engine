#pragma once
#include <vector>
#include <TIMEWARP ENGINE/player.h>
#include <TIMEWARP ENGINE/newRenderer.h>

using namespace std;

void calculatePhysics(Player* player, Renderer* renderer, vector<vector<int>>* tilemap, float deltaTime);