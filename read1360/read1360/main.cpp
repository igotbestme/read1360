#include <iostream>
#include "ReadCSV4.hpp"

using namespace std;

#define x 128
#define y 1024
#define z 1360

int main(void)
{
	ReadCSV csv("E:\\wiagData\\total128_1360.csv", x, y, z);//"C:\\software\\C++\\VS2022c\\LoadCSV\\total128_1024_1360.csv"
	vector<vector<vector<int>>> content;
	csv.work(content);


	cout << " size " << sizeof(content) << endl;


	return 0;
}
