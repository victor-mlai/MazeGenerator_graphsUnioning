#pragma once

struct Coord2D
{
	int x = 0, y = 0;

	// distance^2
	int distance2(const Coord2D& other) const
	{
		const int dx = (other.x - x);
		const int dy = (other.y - y);
		return dx * dx + dy * dy;
	}

	Coord2D operator+(const Coord2D& other) const
	{
		return Coord2D{ x + other.x, y + other.y };
	}

	Coord2D operator/(int devider) const
	{
		return Coord2D{ x / devider, y / devider };
	}

	bool operator==(const Coord2D& other) const
	{
		return (x == other.x) && (y == other.y);
	}

	bool operator!=(const Coord2D& other) const
	{
		return !(*this == other);
	}
};
