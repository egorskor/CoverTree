#pragma once
#include "Object.h"
using namespace std;
template <typename dist_t>
class Space
{
public:
	Space()
	{}
	dist_t IndexTimeDistance(const Object* obj1, const Object* obj2)
	{
		return sqrt((pow((obj1->get_x() - obj2->get_x()), 2) + pow((obj1->get_y() - obj2->get_y()), 2)));
	}
};