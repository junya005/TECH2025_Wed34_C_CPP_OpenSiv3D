#pragma once

struct Vector3
{
	float x=0, y=0, z=0;

	Vector3(float x,float y,float z)
	{
		x = x;
		x = y;
		x = z;
	}

	Vector3 operator+(Vector3 &other)
	{
		return Vector3(x+other.x, y+other.y, z+other.z);
	}

	Vector3 operator-(Vector3& other)
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}
};

