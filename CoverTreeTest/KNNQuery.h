#pragma once
#include "Object.h"
#include "Space.h"

#include <vector>

using namespace std;
template <typename dist_t>
class KNNQuery {
	Object* query;
	int k;
	int DistCount = 0;
	vector<const Object*>* objectVector;
	vector<dist_t>* distVector;
    Space<dist_t>* space;
public:
	KNNQuery(int _k, Object* _query,  Space<dist_t>* _space)
	{
		space = _space;
		k = _k;
		query = _query;
		objectVector = new vector<const Object*>;
		distVector = new vector<dist_t>;
	}

	int get_k()
	{
		return k;
	}
	vector<double> get_Points()
	{
		vector<double> toReturn;
		for (int i = 0; i < objectVector->size(); i++)
		{
			toReturn.push_back((*objectVector)[i]->get_value());
		}
		return toReturn;
	}

	vector<const Object*> get_Objects()
	{
		return *objectVector;
	}
	vector<dist_t> get_Dists()
	{
		return *distVector;
	}
	int get_distcount()
	{
		return DistCount;
	}
	dist_t DistanceObjLeft(const Object* object)
	{
		DistCount++;
		return space->IndexTimeDistance(query, object);
	}

	void CheckAndAddToResult(const dist_t dist, const Object* object)
	{
		distVector->push_back(dist);
		objectVector->push_back(object);
	}
	void CheckAndAddToResult(const Object* object)
	{
		objectVector->push_back(object);
	}
	void CheckAndAddToResult(const vector<Object*>& bucket)
	{
		objectVector->insert(objectVector->end(), bucket.begin(), bucket.end());
	}
};