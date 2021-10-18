#pragma once

class Vector3
{
public:
	float x, y, z;

	Vector3() { x = y = z = 0; }
	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3 operator + (Vector3 const &other) { return Vector3(x + other.x, y + other.y, z + other.z); }
	Vector3 operator - (Vector3 const &other) { return Vector3(x - other.x, y - other.y, z - other.z); }
	Vector3 operator * (float const &scalar) { return Vector3(x * scalar, y * scalar, z * scalar); }
};