#include<iostream>
//#include "CoverTree.cpp"
//#include "Space.cpp"
#include <map>
#include <vector>
#include <set>
#include <ctime> 
#include <random>
#include <string>
#include <fstream>

#include "CoverTree.h"
#include "Object.h"
#include "Space.h"
#include "KNNQuery.h"
using namespace std;
using std::string;
/*
class Object
{
	double x;
	double y;
public:
	Object(double _value, double _value2)
	{
		x = _value;
		y=_value2;
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


template <typename dist_t>
class KNNQuery {
	Object* query;
	int k;
	int DistCount=0;
	vector<const Object*>* objectVector;
	vector<dist_t>* distVector;
public:
	KNNQuery(int _k, Object* _query)
	{
		k = _k;
		query = _query;
		objectVector = new vector<const Object*>;
	    distVector= new vector<dist_t>;
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
		return sqrt((pow((query->get_x() - object->get_x()), 2) + pow((query->get_y() - object->get_y()), 2)));
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
template <typename dist_t>
class Space
{
public:
	Space()
	{}
	dist_t IndexTimeDistance(const Object* obj1, const Object* obj2)
	{
		return sqrt((pow((obj1->get_x() - obj2->get_x()),2)+pow((obj1->get_y()- obj2->get_y()),2)));
	}
};
template <typename dist_t>
class CoverTreeMethod
{
private:

	class CoverTreeNode {
	private:
		map<int, vector<CoverTreeNode*>> children;
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
	CoverTreeMethod( Space<dist_t>* space,
		const vector<Object*> data)
	{
		if (root == NULL)
		{
			root = new CoverTreeNode(data[0]);
		}
		long int  size = data.size();
		cout << "0%     10%       20%       30%       40%       50%       60%       70%       80%       90%       100%" << endl;
		for (int i = 1; i < data.size(); i++)
		{
			vector<pair<dist_t, CoverTreeNode*>> tmp = { make_pair(space->IndexTimeDistance(data[i],root->get_object()),root) };
			insert(space, data[i], tmp, MAX_LEVEL);
			if (i % (size / 100) == 0)
			{
				cout << "=";
			}
		}
	}

	bool insert( Space<dist_t>* space, Object* p, vector<pair<dist_t, CoverTreeNode*>>& Qi, int level)
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
				dist_t d = space->IndexTimeDistance(p,(*it2)->get_object());
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
			bool found = insert(space,p, Qj, level - 1);
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
			for (it2 = tmp.begin(); it2 != tmp.end();it2++)
			{
				if (count > k) break;
				query->CheckAndAddToResult(it->first, it->second->get_object());
				count++;
			}
		}
	}

	/*void Search(KNNQuery<dist_t>* query)
	{

	}
};//CoverTree
*/
void test(int N, int k)//N is amount of objects in tree
{
	srand(time(0));
	cout << "Test with " << N << " objects begins" << endl;
	Object* search_object = new Object(rand(),rand());
	Space<double>* fakespace = new Space<double>();
	KNNQuery<double>* query = new KNNQuery<double>(k, search_object, fakespace);//второй аргумент точка, к которой ищем ближайших соседей; первый аргумент - их количество
	vector<Object*> data;
	multiset<pair<double, const Object*>>data_set;//<dist,Object>
	multiset<pair<double, const Object*>>check;
	//vector<Object*> data = {new Object(1), new Object(5), new Object(6),  new Object(6), new Object(7), new Object(46), new Object(3), new Object(2) };
	//data.push_back(search_object); //–аскоментить, чтобы иметь сам объект поиска в дереве
	//data_set.insert(make_pair(fakespace->IndexTimeDistance(search_object, search_object), search_object));
	for (int i = 0; i < N; i++)
	{
		Object* r = new Object(rand(), rand());
		data.push_back(r);
		data_set.insert(make_pair(fakespace->IndexTimeDistance(r, search_object), r));
	}
	multiset<pair<double, const Object*>>::iterator it = data_set.begin();
	for (int i = 0; i < query->get_k(); i++)
	{
		if (it == data_set.end())
		{
			cout << "K is bigger than data.size()" << endl;
			break;
		}
		check.insert(*it);
		it++;
	}
	cout << "Data set created" << endl;

	unsigned int start_tree_creation_time = clock();
	CoverTreeMethod<double> a(fakespace, data);
	unsigned int end_tree_creation_time = clock();
	cout << "Tree creation time: " << end_tree_creation_time - start_tree_creation_time << endl;
	cout << "0%     10%       20%       30%       40%       50%       60%       70%       80%       90%       100%" << endl;
	unsigned int start_search_time = clock();
	a.Search(query);
	unsigned int end_search_time = clock();
	cout << "Search time: " << end_search_time - start_search_time << endl;

	vector<const Object*> result = query->get_Objects();
	vector<double> distResult = query->get_Dists();
	int right_answer_count = 0;
	int i = 0;
	//for (multiset<pair<double, Object*>>::iterator it = check.begin(); i < query->get_k(); it++)//тк элементы могут идти в разном пор€дке - не идеально
	//{
	// if (i >= result.size())
	// {
	//	 cout << "search returned less objects than k" << endl;
	//	 break;
	// }
	// if (it->second == result[i])
	// {
	//	 right_answer_count++;
	// }
	// i++;
	//}
	//vector<const Object*>::iterator it = result.begin(); it != result.end(); it++
	for (i; i < query->get_k(); i++)
	{
		if (i >= result.size())
		{
			cout << "Search returned less objects than k" << endl;
			break;
		}
		multiset<pair<double, const Object*>>::iterator it2 = check.find(make_pair(distResult[i], result[i]));
		if (it2 != check.end())
		{
			right_answer_count++;
		}
	}
	cout << "The number of distance calculations " << query->get_distcount() << endl;
	cout << "Right neighbours " << right_answer_count << endl;
	cout << "From k=" << query->get_k() << endl;
	cout << "Percentage of right answers: " << (float)right_answer_count / i << endl;
}
void test2(int N, int k)
{
	std::default_random_engine generator(time(0));
	std::uniform_int_distribution<> dis(-1000000, 1000000);
	cout << "Test with " << N << " objects begins" << endl;
	Object* search_object = new Object(dis(generator), dis(generator));
	
	Space<double>* fakespace = new Space<double>();
	KNNQuery<double>* query = new KNNQuery<double>(k, search_object, fakespace);//второй аргумент точка, к которой ищем ближайших соседей; первый аргумент - их количество
	vector<Object*> data;
	multiset<pair<double, const Object*>>data_set;//<dist,Object>
	multiset<pair<double, const Object*>>check;
	//vector<Object*> data = {new Object(1), new Object(5), new Object(6),  new Object(6), new Object(7), new Object(46), new Object(3), new Object(2) };
	//data.push_back(search_object);
	//data_set.insert(make_pair(fakespace->IndexTimeDistance(search_object, search_object), search_object));
	for (int i = 0; i < N; i++)
	{
		Object* r = new Object(dis(generator), dis(generator));
		data.push_back(r);
		data_set.insert(make_pair(fakespace->IndexTimeDistance(r, search_object), r));
	}
	multiset<pair<double, const Object*>>::iterator it = data_set.begin();
	for (int i = 0; i < query->get_k(); i++)
	{
		if (it == data_set.end())
		{
			cout << "K is bigger than data.size()" << endl;
			break;
		}
		check.insert(*it);
		it++;
	}
	cout << "Data set created" << endl;

	cout << "Tree creation begin..." << endl;
	unsigned long start_tree_creation_time = clock();
	//CoverTreeMethod<double> a(fakespace, data);//обычный способ

	vector<Object *> rVec = {data[0]}; //этот вариант только дл€ красоты
	CoverTreeMethod<double> a(fakespace, data);
	for (int count=1; count < data.size(); count++)
	{
		a.add_element(fakespace, data[count]);
		if (count % (data.size() / 100) == 0)
		{
			cout << "=";
		}
	}
	unsigned long end_tree_creation_time = clock();
	cout << "Tree creation time: " << end_tree_creation_time - start_tree_creation_time << endl;


	unsigned long start_search_time = clock();
	a.Search(query);
	unsigned long end_search_time = clock();
	cout << "Search time: " << end_search_time - start_search_time << endl;

	vector<const Object*> result = query->get_Objects();
	vector<double> distResult = query->get_Dists();
	int right_answer_count = 0;
	int i = 0;
	for (i; i < query->get_k(); i++)
	{
		if (i >= result.size())
		{
			cout << "Search returned less objects than k" << endl;
			break;
		}
		multiset<pair<double, const Object*>>::iterator it2 = check.find(make_pair(distResult[i], result[i]));
		if (it2 != check.end())
		{
			right_answer_count++;
		}
	}
	cout << "The number of distance calculations " << query->get_distcount() << endl;
	cout << "Right neighbours " << right_answer_count << endl;
	cout << "From k=" << query->get_k() << endl;
	cout << "Percentage of right answers: " << (float)right_answer_count / i *100<<"%"<< endl;
	string filename1 = "testsData/data";
	string filename2 = to_string(N);
	string filename3 = ".txt";
	string fulltime = filename1 + filename2 + filename3;
	ofstream fout(fulltime);
	cout << "Saving data in file "<<fulltime<< endl;
	for (int count = 0; count < data.size(); count++)
	{
		fout << data[count]->get_x() << " " << data[count]->get_y() << endl;;
	}
	fout.close();
	ofstream fout2("answer.txt");
	cout << "Saving answer in file answer.txt" << endl;
	for (auto it = check.begin(); it != check.end();it++)
	{
		fout2 << it->first<< " " << it->second->get_x()<<" "<< it->second->get_x() << endl;
	}
	fout2.close();


}
void save_data(int N, vector<Object *> data)
{
	string filename1 = "testsData/data";
	string filename2 = to_string(N);
	string filename3 = ".txt";
	string fullname = filename1 + filename2 + filename3;
	ofstream fout(fullname);
	cout << "Saving data in file " << fullname << endl;
	for (int count = 0; count < data.size(); count++)
	{
		fout << data[count]->get_x() << " " << data[count]->get_y() << endl;;
	}
	fout.close();
}
void save_result(int N, vector<Object *> check)
{
	string filename1 = "testsData/result";
	string filename2 = to_string(N);
	string filename3 = ".txt";
	string fullname = filename1 + filename2 + filename3;
	ofstream fout(fullname);
	cout << "Saving answer in file answer.txt" << endl;
	fout << "Number of points closer\t"<< "Relative position error\t"<<"Recall\t"<<"Classification accuracy\t" << endl;
	for (auto it = check.begin(); it != check.end(); it++)
	{
		fout << it->first << " " << it->second->get_x() << " " << it->second->get_x() << endl;
	}
	fout.close();
}

void save_answer(int N, multiset<pair<double, const Object*>>check)
{
	string filename1 = "testsData/answer";
	string filename2 = to_string(N);
	string filename3 = ".txt";
	string fullname = filename1 + filename2 + filename3;
	ofstream fout(fullname);
	cout << "Saving answer in file answer.txt" << endl;
	for (auto it = check.begin(); it != check.end(); it++)
	{
		fout << it->first << " " << it->second->get_x() << " " << it->second->get_x() << endl;
	}
	fout.close();
}
int main()
{
	/*Space<double> fakespase;
	double a = sqrt(pow((23-10),2)+pow((19-28),2));
	const Object obj1(23, 19);
	const Object obj2(10, 28);
	double b = fakespase.IndexTimeDistance(&obj1,&obj2);*/
   // test(1000);//argument is amount of objects in tree
	//test(10000);
	//test(1000000);

	test2(1000000, 5);
	system("pause");
	
}