#pragma once

#include <string>

struct C3Vector
{
	float x, y, z;
	C3Vector() = default;
	C3Vector(std::string str)
	{
		size_t next;
		x = stof(str, &next);
		y = stof(str = str.substr(next + 1), &next);
		z = stof(str.substr(next + 1), nullptr);
	}
};
struct C4Vector : C3Vector
{
  float w;
};

struct C44Matrix
{
  C4Vector x;
  C4Vector y;
  C4Vector z;
  C4Vector w;
};

template<typename T>
  struct TSSimpleArray
{
	T* data;
	size_t count;
	size_t capacity;
};
