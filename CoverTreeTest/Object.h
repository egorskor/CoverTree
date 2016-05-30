#pragma once
#include <vector>
using namespace std;
class Object
{
	/*double x;
	double y;*/
	int dimension;
	vector<long long> coordinateVector;
public:
	Object(vector<long long>* _coordinateVector)
	{
		coordinateVector = *_coordinateVector;
		dimension = _coordinateVector->size();
	}
	Object(vector<long long> _coordinateVector)
	{
		coordinateVector = _coordinateVector;
		dimension = _coordinateVector.size();
	}
	int get_dimension() const
	{
		return dimension;
	}
	vector<long long> get_coordinateVector() const
	{
		return coordinateVector;
	}
};
