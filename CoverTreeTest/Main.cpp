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
		vector<long long> tmp = data[count]->get_coordinateVector();
		for (int count2 = 0; count2<data[count]->get_dimension(); count2++)
		{
			fout << tmp[count2] << " ";
		}
		fout << endl;
	}
	fout.close();
}

void save_result(int N, int k, int dimension, int dispersion, int distComp)
{
	/*string filename1 = "testsData/result";
	string filename2 = to_string(N);
	string filename3 = ".txt";
	string fullname = filename1 + filename2 + filename3;*/
	fstream fout("dimensiontests.txt",ios::app);
	cout << "Saving answer in file dimensiontests.txt" << endl;
	//fout << "Number of points closer\t" << "Relative position error\t" << "Recall\t" << "Classification accuracy\t" << endl;
    fout << N<<"\t"<<k << "\t" <<dimension << "\t" <<dispersion << "\t" <<distComp << endl;

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
		fout << it->first << " ";
		vector<long long> tmp = it->second->get_coordinateVector();
		for (int count2 = 0; count2<it->second->get_dimension(); count2++)
		{
			fout << tmp[count2] << " ";
		}
		fout << endl;
	}
	fout.close();
}
long long count_max_dist(Space<double>* space,int dimension, int dispertion)
{
	vector<long long> coordVec;
	for (int count = 0; count < dimension; count++)
	{
		coordVec.push_back(dispertion);
	}
	vector<long long> coordVec2;
	for (int count = 0; count < dimension; count++)
	{
		coordVec2.push_back(-dispertion);
	}
	return space->IndexTimeDistance(new Object(coordVec), new Object(coordVec2));
}
void test2(int N, int k, int dimension, int dispersion)
{
	std::default_random_engine generator(time(0));
	std::uniform_int_distribution<> dis(-dispersion, dispersion);
	cout << "Test with " << N << " objects; K=" << k << "; dimension=" << dimension << "; dispersion="<< dispersion << endl;
	
	vector<long long> SOcoordVec;
	for (int count = 0; count < dimension; count++)
	{
		SOcoordVec.push_back(dis(generator));
	}
	Object* search_object = new Object(SOcoordVec);
	
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
		vector<long long> coordVec;
		for (int count = 0; count < dimension; count++)
		{
			coordVec.push_back(dis(generator));
		}
		Object* r = new Object(coordVec);
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
	//CoverTreeMethod<double> a(count_max_dist(new Space<double>, dimension, dispersion),fakespace, data);//обычный способ
	cout << "0%        10%       20%       30%       40%       50%       60%       70%       80%       90%       100%" << endl;
	vector<Object *> rVec = {data[0]}; //этот вариант только для красоты
	CoverTreeMethod<double> a(count_max_dist(new Space<double>, dimension, dispersion),fakespace, rVec);//count_max_dist работает только для евклидовых метрик
	for (int count=1; count < data.size(); count++)
	{
		a.add_element(fakespace, data[count]);
		if (count % (data.size() / 100) == 0)
		{
			cout << "=";
		}
	}
	unsigned long end_tree_creation_time = clock();
	cout << endl << "Tree creation time: " << end_tree_creation_time - start_tree_creation_time << endl;


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
	
	//save_data(N, data);
	//save_answer(N, check);
	save_result(N, k, dimension, dispersion, query->get_distcount());
}
/*
void float_test(int N, int k, int dimension, int dispersion)
{
	std::default_random_engine generator(time(0));
	std::uniform_real_distribution<> dis(-dispersion, dispersion);
	cout << "Test with " << N << " objects; K=" << k << "; dimension=" << dimension << "; dispersion=" << dispersion << endl;

	vector<double> SOcoordVec;
	for (int count = 0; count < dimension; count++)
	{
		SOcoordVec.push_back(dis(generator));
	}
	Object* search_object = new Object(SOcoordVec);

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
		vector<long long> coordVec;
		for (int count = 0; count < dimension; count++)
		{
			coordVec.push_back(dis(generator));
		}
		Object* r = new Object(coordVec);
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
	//CoverTreeMethod<double> a(count_max_dist(new Space<double>, dimension, dispersion),fakespace, data);//обычный способ
	cout << "0%        10%       20%       30%       40%       50%       60%       70%       80%       90%       100%" << endl;
	vector<Object *> rVec = { data[0] }; //этот вариант только для красоты
	CoverTreeMethod<double> a(count_max_dist(new Space<double>, dimension, dispersion), fakespace, rVec);//count_max_dist работает только для евклидовых метрик
	for (int count = 1; count < data.size(); count++)
	{
		a.add_element(fakespace, data[count]);
		if (count % (data.size() / 100) == 0)
		{
			cout << "=";
		}
	}
	unsigned long end_tree_creation_time = clock();
	cout << endl << "Tree creation time: " << end_tree_creation_time - start_tree_creation_time << endl;


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
	cout << "Percentage of right answers: " << (float)right_answer_count / i * 100 << "%" << endl;

	//save_data(N, data);
	//save_answer(N, check);
	save_result(N, k, dimension, dispersion, query->get_distcount());
}*/
int main()
{
	/*
	Space<double> fakespase;
	vector<double> _coordinateVector = { 0,0,0 };
	vector<double> _coordinateVector2 = { 10,5,33 };
	const Object* obj1=new  Object(_coordinateVector);
	const Object* obj2=new   Object(_coordinateVector2);
	double b = fakespase.IndexTimeDistance(obj1,obj2);
   // test(1000);//argument is amount of objects in tree
	//test(10000);
	//test(1000000);
	*/

	/*
	//2 - dimension tests
	for (int i = 0; i < 5; i++)//тесты на 1000 точек
	{
		test2(1000, 5, 2, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000 точек
	{
		test2(1000, 5, 2, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000 точек
	{
		test2(1000, 5, 2, 100000);
	}
	//3- dimension
	for (int i = 0; i < 5; i++)//тесты на 1000 точек
	{
		test2(1000, 5, 3, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000 точек
	{
		test2(1000, 5, 3, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000 точек
	{
		test2(1000, 5, 3, 100000);
	}
	//4-dimension
	for (int i = 0; i < 5; i++)//тесты на 1000 точек
	{
		test2(1000, 5, 3, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000 точек
	{
		test2(1000, 5, 3, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000 точек
	{
		test2(1000, 5, 3, 100000);
	}

	

	//2-dimension
	for (int i = 0; i < 5; i++)//тесты на 5000 точек
	{
		test2(5000, 5, 2, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 5000 точек
	{
		test2(5000, 5, 2, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 5000 точек
	{
		test2(5000, 5, 2, 100000);
	}
	//3-dimension
	for (int i = 0; i < 5; i++)//тесты на 5000 точек
	{
		test2(5000, 5, 3, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 5000 точек
	{
		test2(5000, 5, 3, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 5000 точек
	{
		test2(5000, 5, 3, 100000);
	}
	//4-dimension
	for (int i = 0; i < 5; i++)//тесты на 5000 точек
	{
		test2(5000, 5, 4, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 5000 точек
	{
		test2(5000, 5, 4, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 5000 точек
	{
		test2(5000, 5, 4, 100000);
	}



	//2-dimension
	for (int i = 0; i < 5; i++)//тесты на 10000 точек
	{
		test2(10000, 5, 2, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 10000 точек
	{
		test2(10000, 5, 2, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 10000 точек
	{
		test2(10000, 5, 2, 100000);
	}
	//3-dimension
	for (int i = 0; i < 5; i++)//тесты на 10000 точек
	{
		test2(10000, 5, 3, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 10000 точек
	{
		test2(10000, 5, 3, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 10000 точек
	{
		test2(10000, 5, 3, 100000);
	}
	//4-dimension
	for (int i = 0; i < 5; i++)//тесты на 10000 точек
	{
		test2(10000, 5, 4, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 10000 точек
	{
		test2(10000, 5, 4, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 10000 точек
	{
		test2(10000, 5, 4, 100000);
	}
	

	*/
/*
for (int count = 500; count < 10000; count += 100)
{
	test2(100000, 5, 2, count);
}
for (int count = 10000; count < 1000000;count+=10000)
{ */
	test2(1000, 5, 2, 1000);
//}


/*
for (int i = 0; i < 5; i++)//тесты на 20000 точек
{
	test2(20000, 5, 2, 1000);
}
for (int i = 0; i < 5; i++)//тесты на 20000 точек
{
	test2(20000, 5, 2, 10000);
}
for (int i = 0; i < 5; i++)//тесты на 20000 точек
{
	test2(20000, 5, 2, 100000);
}*/
	/*
	//2-dimension
	for (int i = 0; i < 5; i++)//тесты на 100000 точек
	{
		test2(100000, 5, 2, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 100000 точек
	{
		test2(100000, 5, 2, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 100000 точек
	{
		test2(100000, 5, 2, 100000);
	}
	//3-dimension
	for (int i = 0; i < 5; i++)//тесты на 100000 точек
	{
		test2(100000, 5, 3, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 100000 точек
	{
		test2(100000, 5, 3, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 100000 точек
	{
		test2(100000, 5, 3, 100000);
	}
	//4-dimension
	for (int i = 0; i < 5; i++)//тесты на 100000 точек
	{
		test2(100000, 5, 4, 1000);
	}
	for (int i = 0; i < 5; i++)//тесты на 100000 точек
	{
		test2(100000, 5, 4, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 100000 точек
	{
		test2(100000, 5, 4, 100000);
	}


	//2-dimension
	for (int i = 0; i < 5; i++)//тесты на 1000000 точек
	{
		test2(1000000, 5, 2, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000000 точек
	{
		test2(1000000, 5, 2, 100000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000000 точек
	{
		test2(1000000, 5, 2, 1000000);
	}
	

	//3-dimension
	for (int i = 0; i < 5; i++)//тесты на 1000000 точек
	{
		test2(1000000, 5, 3, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000000 точек
	{
		test2(1000000, 5, 3, 100000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000000 точек
	{
		test2(1000000, 5, 3, 1000000);
	}
	//4-dimension
	for (int i = 0; i < 5; i++)//тесты на 1000000 точек
	{
		test2(1000000, 5, 4, 10000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000000 точек
	{
		test2(1000000, 5, 4, 100000);
	}
	for (int i = 0; i < 5; i++)//тесты на 1000000 точек
	{
		test2(1000000, 5, 4, 1000000);
	}
	*/
	//test2(1000000, 5,2,1000000);
	system("pause");
	
}