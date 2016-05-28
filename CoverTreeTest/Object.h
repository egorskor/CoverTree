#pragma once

class Object
{
	double x;
	double y;
public:
	Object(double _value, double _value2)
	{
		x = _value;
		y = _value2;
	}

	double get_x() const
	{
		return x;
	}
	double get_y() const
	{
		return y;
	}
};
