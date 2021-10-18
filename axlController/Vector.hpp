#pragma once

class Vector3
{
public:
	float_t x, y, z;

	Vector3() { x = y = z = 0; }
	Vector3(float_t x, float_t y, float_t z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3 operator + (Vector3 const &other) { return Vector3(x + other.x, y + other.y, z + other.z); }
	Vector3 operator - (Vector3 const &other) { return Vector3(x - other.x, y - other.y, z - other.z); }
	Vector3 operator * (float_t const &scalar) { return Vector3(x * scalar, y * scalar, z * scalar); }
};