#pragma once
#include "imgui-SFML.h"

template <typename T>
class Vec2
{
public:
	T x = 0;
	T y = 0;

	Vec2() = default;
	Vec2(T xin, T yin):x(xin), y(yin)
	{
	}

	Vec2(const sf::Vector2<T>& vec) :x(vec.x), y(vec.y)
	{
	}

	operator sf::Vector2<T>()
	{
		return sf::Vector2<T>(x, y);
	}

	Vec2 operator + (const Vec2& rhs) const
	{
		//TODO
		return Vec2(x + rhs.x, y + rhs.y);
	}

	Vec2 operator - (const Vec2& rhs) const
	{
		//TODO
		return Vec2(x - rhs.x, y - rhs.y);
	}

	Vec2 operator / (const T val) const
	{
		//TODO
		return Vec2(x / val, y / val);
	}

	Vec2 operator * (const T val) const
	{
		//TODO
		return Vec2(x * val, y * val);
	}

	bool operator == (const Vec2& rhs) const
	{
		//TODO
		return x==rhs.x && y==rhs.y;
	}

	bool operator != (const Vec2& rhs) const
	{
		//TODO
		return x!=rhs.x || y!=rhs.y;
	}

	void operator += (const Vec2& rhs)
	{
		//TODO
		x+=rhs.x;
		y+=rhs.y;
	}

	void operator -= (const Vec2& rhs)
	{
		//TODO
		x-=rhs.x;
		y-=rhs.y;
	}

	void operator *= (const T val)
	{
		//TODO
		x*=val;
		y*=val;
	}

	void operator /=(const T val)
	{
		//TODO
		x/=val;
		y/=val;
	}

	float dist(const Vec2& rhs) const
	{
		//TODO
		return sqrt((x-rhs.x)*(x-rhs.x)+(y-rhs.y)*(y-rhs.y));
	}

	float length() const
	{
		//TODO
		return sqrt(x*x+y*y);
	}
	
	
};

using Vec2f = Vec2<float>;