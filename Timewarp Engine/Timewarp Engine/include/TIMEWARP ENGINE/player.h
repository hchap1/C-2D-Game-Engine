#pragma once
#include <vector>
#include <TIMEWARP ENGINE/gameState.h>
#include <iostream>
#include <TERMCOLOR/termcolor.hpp>

class Player {
public:
	float x;
	float y;
	Player() {}
	Player(float xPosition, float yPosition) {
		startX = xPosition;
		startY = yPosition;
		x = startX;
		y = startY;
		xVelocity = 0.0f;
		yVelocity = 0.0f;
		isCrouching = false;
	}

	void setStart(float xPosition, float yPosition) {
		startX = x;
		startY = y;
	}

	void goToStart() {
		x = startX;
		y = startY;
		xVelocity = 0.0f;
		yVelocity = 0.0f;
		isCrouching = false;
	}

	GameState* constructGameState() {
		//Package information into GameState for rendering
		cout << "X: " << x << " Y: " << y << endl;
		GameState* newGameState = new GameState(x, y, isCrouching, redButton, greenButton, blueButton);
		return newGameState;
	}

	void injectDataIntoGameState(GameState* gamestate) {
		gamestate->addData(x, y, isCrouching, redButton, greenButton, blueButton);
	}

	void physicsLoop(Renderer* renderer) {
		redButton = false;
		greenButton = false;
		blueButton = false;

		float movementMultiplier = renderer->getDeltaTime() * 5.0f;

		if (renderer->getKeyDown(GLFW_KEY_D)) {
			xVelocity -= movementMultiplier;
		}
		if (renderer->getKeyDown(GLFW_KEY_A)) {
			xVelocity += movementMultiplier;
		}

		x += xVelocity * movementMultiplier;
		y += yVelocity * movementMultiplier;

		xVelocity *= 0.8f;

		
	}


private:
	float startX;
	float startY;

	float xVelocity;
	float yVelocity;

	bool redButton;
	bool greenButton;
	bool blueButton;

	bool isCrouching;
};