#pragma once
typedef struct Circle
{
	float xPos;
	float yPos;
	int radius;
	int xVelocity;
	int yVelocity;
} Circle;

void updateCircle(Circle& c, const int screenWidth, const int screenHeight, const double dT);