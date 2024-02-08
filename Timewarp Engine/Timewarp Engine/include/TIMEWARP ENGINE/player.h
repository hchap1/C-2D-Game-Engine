#pragma once
#include <vector>
#include <TIMEWARP ENGINE/gameState.h>
#include <iostream>
#include <TERMCOLOR/termcolor.hpp>
#include <TIMEWARP ENGINE/newRenderer.h>

class Player {
public:
	Player() {}
	Player(float xPosition, float yPosition) {
		startX = xPosition;
		startY = yPosition;
		x = startX;
		y = startY;
		xVelocity = 0.0f;
		yVelocity = 0.0f;
		isCrouching = false;
		onWall = false;
		canWallJump = false;
		canDoubleJump = false;
		canDash = false;
		dashDirection = 0;
		dashing = false;
	}

	bool getOnWall() { return onWall; }
	bool getCanWallJump() { return canWallJump; }
	bool getCanDoubleJump() { return canDoubleJump; }
	bool getCanDash() { return canDash; }
	int getDashDirection() { return dashDirection; }
	int getDashEnd() { return dashEnd; }
	bool getDashing() { return dashing; }

	void setOnWall(bool newOnWall) { onWall = newOnWall; }
	void setCanWallJump(bool newCanWallJump) { canWallJump = newCanWallJump; }
	void setCanDoubleJump(bool newCanDoubleJump) { canDoubleJump = newCanDoubleJump; }
	void setCanDash(bool newCanDash) { canDash = newCanDash; }
	void setDashDirection(int newDashDirection) { dashDirection = newDashDirection; }
	void setDashEnd(int newDashEnd) { dashEnd = newDashEnd; }
	void setDashing(bool newDashing) {}

	void setStart(float xPosition, float yPosition) {
		startX = xPosition;
		startY = yPosition;
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
		GameState* newGameState = new GameState(x, y, isCrouching, redButton, greenButton, blueButton);
		return newGameState;
	}

	void injectDataIntoGameState(GameState* gamestate) {
		gamestate->addData(x, y, isCrouching, redButton, greenButton, blueButton);
	}

	void physicsLoop(Renderer* renderer, float deltaTime) {
		redButton = false;
		greenButton = false;
		blueButton = false;

		float movementMultiplier = deltaTime * 2.0f;

		if (renderer->getKeyDown(GLFW_KEY_D)) {
			xVelocity -= movementMultiplier;
		}
		if (renderer->getKeyDown(GLFW_KEY_A)) {
			xVelocity += movementMultiplier;
		}

		xVelocity *= 0.9f;
		yVelocity += movementMultiplier * 2.0f;
	}

	void applyXVelocity(float deltaTime) { x += xVelocity * deltaTime; }
	void applyYVelocity(float deltaTime) { y += yVelocity * deltaTime; }

	void setXVelocity(float newXVelocity) { xVelocity = newXVelocity; }
	void setYVelocity(float newYVelocity) { yVelocity = newYVelocity; }
	void setX(float newX) { x = newX; }
	void setY(float newY) { y = newY; }

	float getX() { return x; }
	float getY() { return y; }
	float getXVel() { return xVelocity; }
	float getYVel() { return yVelocity; }

	bool getRedButton() { return redButton; }
	bool getGreenButton() { return greenButton; }
	bool getBlueButton() { return blueButton; }

private:
	float x;
	float y;

	float startX;
	float startY;

	float xVelocity;
	float yVelocity;

	bool redButton;
	bool greenButton;
	bool blueButton;

	bool isCrouching;
	bool isDashing;

	bool onWall;
	bool canWallJump;

	bool canDoubleJump;
	bool canDash;

	int dashEnd;
	int dashDirection;
	bool dashing;
};