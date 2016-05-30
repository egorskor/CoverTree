#pragma once
#include "Object.h"
#include "Space.h"
#include "KNNQuery.h"
#include <map>
#include <vector>


//ceilf(log(RAND_MAX)/log(2.0));
//ceilf(log(1/1024) / log(2.0));
using namespace std;
template <typename dist_t>
class CoverTreeMethod
{
private:

	class CoverTreeNode {
	private:
		map <int, vector<CoverTreeNode*> > children;
		//Object* object;
		vector<const Object*> objectVector;
	public:
		CoverTreeNode(const Object* _object)
		{
			//object = _object;
			//vector<const Object*>objectVector;
			objectVector.push_back(_object);
		}
		const Object* get_object()
		{
			return objectVector[0];
		}
		vector<const Object*> get_all()
		{
			return objectVector;
		}
		vector<CoverTreeNode*> get_children(int level)
		{
			map<int, vector<CoverTreeNode*>>::iterator it = children.find(level);
			if (it == children.end())
			{
				return vector <CoverTreeNode*>();
			}
			return children.at(level);
		}
		void insert_child(int level, CoverTreeNode* child)
		{
			children[level].push_back(child);
		}

		void insert_clone(Object* clone)
		{
			objectVector.push_back(clone);
		}
	};//CoverTreeNode
	CoverTreeNode* root;
public:
	 int MAX_LEVEL;
	int MIN_LEVEL;
	CoverTreeMethod(long long _max_dist, Space<dist_t>* space,
		const vector<Object*> data)
	{
		MAX_LEVEL = ceilf(log(_max_dist) / log(2.0));
		MIN_LEVEL = MAX_LEVEL - 1;
		if (root == NULL)
		{
			root = new CoverTreeNode(data[0]);
		}
		for (int i = 1; i < data.size(); i++)
		{
			vector<pair<dist_t, CoverTreeNode*>> tmp = { make_pair(space->IndexTimeDistance(data[i],root->get_object()),root) };
			insert(space, data[i], tmp, MAX_LEVEL);
		}
	}

	bool insert(Space<dist_t>* space, Object* p, vector<pair<dist_t, CoverTreeNode*>>& Qi, int level)
	{
		std::vector<std::pair<dist_t, CoverTreeNode*> > Qj;
		dist_t sep = pow(2.0, level);
		dist_t minDist = DBL_MAX;
		std::pair<dist_t, CoverTreeNode*> minQiDist(DBL_MAX, NULL);
		typename  std::vector<std::pair<dist_t, CoverTreeNode*> >::const_iterator it;
		for (it = Qi.begin(); it != Qi.end(); ++it) {
			if (it->first<minQiDist.first) minQiDist = *it;
			if (it->first<minDist) minDist = it->first;
			if (it->first <= sep) Qj.push_back(*it);
			std::vector<CoverTreeNode*> children = it->second->get_children(level);
			typename std::vector<CoverTreeNode*>::const_iterator it2;
			for (it2 = children.begin(); it2 != children.end(); ++it2) {
				dist_t d = space->IndexTimeDistance(p, (*it2)->get_object());
				if (d<minDist) minDist = d;
				if (d <= sep) {
					if (d == 0)
					{
						(*it).second->insert_clone(p);
						return false;
					}
					Qj.push_back(std::make_pair(d, *it2));
				}
			}
		}
		//std::cout << "level: " << level << ", sep: " << sep << ", dist: " << minQDist.first << "\n";
		if (minDist > sep) {
			return true;
		}
		else {
			bool found = insert(space, p, Qj, level - 1);
			//distNodePair minQiDist = distance(p,Qi);
			if (found && minQiDist.first <= sep) {
				if (level - 1<MIN_LEVEL) MIN_LEVEL = level - 1;
				minQiDist.second->insert_child(level,
					new CoverTreeNode(p));
				//std::cout << "parent is ";
				//minQiDist.second->getPoint().print();
				//_numNodes++;
				return false;
			}
			else {
				return found;
			}
		}
	}


	void Search(KNNQuery<dist_t>* query)
	{
		int k = query->get_k();
		//maxDist is the kth nearest known point to p, and also the farthest
		//point from p in the set minNodes defined below.
		double maxDist = query->DistanceObjLeft(root->get_object());
		//minNodes stores the k nearest known points to p.
		std::set<pair<dist_t, CoverTreeNode*>> minNodes;
		minNodes.insert(std::make_pair(maxDist, root));
		std::vector<pair<dist_t, CoverTreeNode*>> Qj(1, std::make_pair(maxDist, root));
		for (int level = MAX_LEVEL; level >= MIN_LEVEL; level--) {
			typename std::vector<pair<dist_t, CoverTreeNode*>>::const_iterator it;
			int size = Qj.size();
			for (int i = 0; i<size; i++) {
				std::vector<CoverTreeNode*> children =
					Qj[i].second->get_children(level);
				typename std::vector<CoverTreeNode*>::const_iterator it2;
				for (it2 = children.begin(); it2 != children.end(); ++it2) {
					double d = query->DistanceObjLeft((*it2)->get_object());
					if (d < maxDist || minNodes.size() < k) {
						minNodes.insert(std::make_pair(d, *it2));
						//--minNodes.end() gives us an iterator to the greatest
						//element of minNodes.
						if (minNodes.size() > k) minNodes.erase(--minNodes.end());
						maxDist = (--minNodes.end())->first;
					}
					Qj.push_back(std::make_pair(d, *it2));
				}
			}
			double sep = maxDist + pow(2.0, level);
			size = Qj.size();
			for (int i = 0; i<size; i++) {
				if (Qj[i].first > sep) {
					//quickly removes an element from a vector w/o preserving order.
					Qj[i] = Qj.back();
					Qj.pop_back();
					size--; i--;
				}
			}
		}
		typename std::set<pair<dist_t, CoverTreeNode*>>::const_iterator it;
		int count = 0;
		for (it = minNodes.begin(); it != minNodes.end(); ++it) {
			const std::vector<const Object *> tmp = (*it).second->get_all();
			typename std::vector<const Object *>::const_iterator it2;
			for (it2 = tmp.begin(); it2 != tmp.end(); it2++)
			{
				if (count > k) break;
				query->CheckAndAddToResult(it->first, it->second->get_object());
				count++;
			}
		}
	}
	void add_element(Space<dist_t>* space, Object * el)
	{
		vector<pair<dist_t, CoverTreeNode*>> tmp = { make_pair(space->IndexTimeDistance(el,root->get_object()),root) };
		insert(space, el, tmp, MAX_LEVEL);
	}
	/*void Search(KNNQuery<dist_t>* query)
	{

	}*/
};//CoverTree
