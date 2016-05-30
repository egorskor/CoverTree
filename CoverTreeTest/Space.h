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
		if (obj1->get_dimension() != obj2->get_dimension())
		{
			cout << "ERROR: diffrenet objects dimension" << endl;
		}
		dist_t toReturn=0;
		vector<long long> obj1coord = obj1->get_coordinateVector();
		vector<long long> obj2coord = obj2->get_coordinateVector();
		for (int i = 0; i < obj1coord.size(); i++)
		{
			toReturn += pow(obj1coord[i] - obj2coord[i], 2);
		}
		
			return sqrt(toReturn);
	}
};