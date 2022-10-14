#define ARR_SIZE 5
constant float4 colors[ARR_SIZE] = { (float4)(1.0, 0.0, 0.0, 1.0), (float4)(0.0, 1.0, 0.0, 1.0), (float4)(0.0, 0.0, 1.0, 1.0), (float4)(1.0, 1.0, 1.0, 1.0), (float4)(1.0, 1.0, 0.0, 1.0) };

typedef struct Circle
{
	float xPos;
	float yPos;
	int radius;
	int xVelocity;
	int yVelocity;
} Circle;

float calcDistSquared(int x1, int y1, int x2, int y2)
{
	float dx = x1 - x2;
	float dy = y1 - y2;
	return dx * dx + dy * dy;
}

__kernel void META(__global struct Circle* Circs, int numCircs, __write_only image2d_t output)
{
	const int2 pos = { get_global_id(0), get_global_id(1) };

	float sum = 0.0f;
	for (int i = 0; i < numCircs; i++)
	{
		struct Circle c = Circs[i];
		float d2 = calcDistSquared(pos.x, pos.y, c.xPos, c.yPos);
		float cont = (c.radius * c.radius) / d2;
		sum += cont;
	}
	float4 color = (float4)(0.0,0.0,0.0,0.0);

	if(sum > 1.0)
	{
		uint index = 0;
		float mindist = calcDistSquared(pos.x, pos.y, Circs[0].xPos, Circs[0].yPos);
		
		for(int i = 1; i < numCircs; i++)
		{
			struct Circle c = Circs[i];
			float d2 = calcDistSquared(pos.x, pos.y, c.xPos, c.yPos);
			if(d2 < mindist)
			{
				index = i;
				mindist = d2;
			}
		}
		color = (float4)colors[index];
	}

	write_imagef(output, (int2)(pos.x, pos.y), color);
}