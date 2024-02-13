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
		canDash = true;
		dashDirection = 0;
		isDashing = false;
		isCrouching = false;
		isGrounded = false;
		spacePressed = 0;
		apostrophePressed = 0;
		redButton = false;
		greenButton = false;
		blueButton = false;
	}

	bool getOnWall() { return onWall; }
	bool getCanWallJump() { return canWallJump; }
	bool getCanDoubleJump() { return canDoubleJump; }
	bool getCanDash() { return canDash; }
	int getDashDirection() { return dashDirection; }
	int getDashEnd() { return dashEnd; }
	bool getDashing() { return isDashing; }
	bool getCrouching() { return isCrouching; }
	bool setGrounded() { return isGrounded; }

	void setOnWall(bool newOnWall) { onWall = newOnWall; }
	void setCanWallJump(bool newCanWallJump) { canWallJump = newCanWallJump; }
	void setCanDoubleJump(bool newCanDoubleJump) { canDoubleJump = newCanDoubleJump; }
	void setCanDash(bool newCanDash) { canDash = newCanDash; }
	void setDashDirection(int newDashDirection) { dashDirection = newDashDirection; }
	void setDashEnd(int newDashEnd) { dashEnd = newDashEnd; }
	void setDashing(bool newDashing) { isDashing = newDashing; }
	void setCrouching(bool newCrouching) { isCrouching = newCrouching; }
	void setGrounded(bool newGrounded) { isGrounded = newGrounded; }
	void setRedButton(bool newRedButton) { redButton = newRedButton; }
	void setGreenButton(bool newGreenButton) { greenButton = newGreenButton; }
	void setBlueButton(bool newBlueButton) { blueButton = newBlueButton; }

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

	void physicsLoop(Renderer* renderer, float deltaTime, int gameTime, vector<bool> buttonStates) {
		if (buttonStates.size() == 1) {
			redButton = false;
			greenButton = false;
			blueButton = false;
		}

		else {
			redButton = buttonStates[0];
			greenButton = buttonStates[1];
			blueButton = buttonStates[2];
		}

		if (renderer->getKeyDown(GLFW_KEY_SPACE)) {	spacePressed += 1; }
		else { spacePressed = 0; }
		if (renderer->getKeyDown(GLFW_KEY_APOSTROPHE)) { apostrophePressed += 1; }
		else { apostrophePressed = 0; }

		float movementMultiplier = deltaTime * 4.0f;

		if (renderer->getKeyDown(GLFW_KEY_D)) {
			xVelocity -= movementMultiplier;
			if (!onWall) { dashDirection = -1; }
		}
		if (renderer->getKeyDown(GLFW_KEY_A)) {
			xVelocity += movementMultiplier;
			if (!onWall) { dashDirection = 1; }
		}

		xVelocity *= 0.9f;
		yVelocity += movementMultiplier;

		if (spacePressed == 1 && isGrounded && !isDashing) {
			yVelocity = renderer->getBlockHeight() * -12.0f;
		}

		if (spacePressed == 1 && onWall && canWallJump && !isGrounded && !isDashing) {
			yVelocity = renderer->getBlockHeight() * -12.0f;
			xVelocity = dashDirection * movementMultiplier * 17.0f;
		}

		if (spacePressed == 1 && canDoubleJump && !isGrounded && !canWallJump && !isDashing) {
			yVelocity = renderer->getBlockHeight() * -12.0f;
			canDoubleJump = false;
			canDash = true;
		}

		if (apostrophePressed == 1 && canDash && !isDashing && dashEnd < gameTime) {
			dashEnd = gameTime + 20;
			canDash = false;
		}
		if (dashEnd > gameTime) {
			isDashing = true;
		}
		else {
			isDashing = false;
		}
		if (onWall && yVelocity > 0.0f) {
			yVelocity *= 0.75f;
		}
		if (renderer->getKeyDown(GLFW_KEY_LEFT_SHIFT)) {
			isCrouching = true;
		}
		else { isCrouching = false; }
		onWall = false;
		canWallJump = false;
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

	bool isGrounded;

	//Key management
	int spacePressed;
	int apostrophePressed;
};