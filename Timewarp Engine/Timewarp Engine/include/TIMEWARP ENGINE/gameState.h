#pragma once
#include <vector>
using namespace std;

class GameState {
public:
	GameState(){}
	GameState(float playerX, float playerY, bool isCrouching, bool red, bool green, bool blue) {
		xPositions.push_back(playerX);
		yPositions.push_back(playerY);
		crouchStates.push_back(isCrouching);
		redButton = red;
		greenButton = green;
		blueButton = blue;
	}

	void addData(float playerX, float playerY, bool isCrouching, bool red, bool green, bool blue) {
		xPositions.push_back(playerX);
		yPositions.push_back(playerY);
		crouchStates.push_back(isCrouching);
		if (red) { redButton = red; }
		if (green) { greenButton = green; }
		if (blue) { blueButton = blue; }
	}

	vector<float> getXPositions() { return xPositions; }
	vector<float> getYPositions() { return yPositions; }
	vector<bool> getCrouchStates() { return crouchStates; }
	vector<bool> getButtonStates() {
		vector<bool> buttonStates = { redButton, greenButton, blueButton };
		return buttonStates;
	}
	bool getRedButton() { return redButton; }
	bool getGreenButton() { return greenButton; }
	bool getBlueButton() { return blueButton; }

private:
	vector<float> xPositions;
	vector<float> yPositions;
	vector<bool> crouchStates;

	bool redButton;
	bool greenButton;
	bool blueButton;
};