#include "Circle.h"
void updateCircle(Circle& c, const int screenWidth, const int screenHeight, const double dT)
{
	if (c.xPos + c.radius > screenWidth && c.xVelocity > 0)
		c.xVelocity *= -1;
	else if (c.xPos - c.radius < 0 && c.xVelocity < 0)
		c.xVelocity *= -1;
	if (c.yPos + c.radius > screenHeight && c.yVelocity > 0)
		c.yVelocity *= -1;
	else if (c.yPos - c.radius < 0 && c.yVelocity < 0)
		c.yVelocity *= -1;
	
	c.xPos += c.xVelocity * dT;
	c.yPos += c.yVelocity * dT;
}