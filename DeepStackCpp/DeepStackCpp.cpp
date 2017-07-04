#include <iostream>
#include <Eigen/Dense>
#include <algorithm>  
#include <time.h>
#include <memory>

#include "CustomSettings.h"
#include "arguments.h"

#include <vector>
#include <memory>
#include <iostream>

using namespace std;
//using Eigen::ArrayXf;
//using Eigen::ArrayXXf;
//using Eigen::VectorXi;
using Eigen::MatrixXf;
//using Eigen::VectorXf;
#include <vector>
#include "Node.h"

//MatrixXf Test()
//{
//	MatrixXf a(10000, 10000);
//	return a;
//}

//int main(int argc, char **argv)
//{
//
//
//	
//
//	Node n1;
//	n1.pot = 1;
//	n1.bets(0) = 11;
//
//	Node n2(n1);
//	n2.bets(0) = 22;
//
//	cout << n1.bets(0) << "\n";
//	cout << n2.bets(0) << "\n";
//
//	MatrixXf test = MatrixXf(3, 2);
//
//	test(0, 0) = 111;
//	test(0, 1) = 01;
//
//	MatrixXf test2 = test;
//	test2(0, 0) = 2222;
//
//	cout << test(0, 0) << "\n";
//	cout << test2(0, 0) << "\n";
//	cout << test2(0, 1) << "\n";
//
//	test(0, 0) = 0;
//	test(0, 1) = 01;
//	test(1, 1) = 11;
//	test(1, 0) = 10;
//	test(2, 0) = 20;
//	test(2, 1) = 21;
//	cout << test << "\n";
//	test.conservativeResize(2, 2);
//	cout << test << "\n";
//	//long long s1 = sizeof(test);
// //   cout << s1 << "\n";
//
//	//MatrixXf test2 = MatrixXf(1, 1);
//	//long long s2 = sizeof(test);
//	//cout << s2;
//	//cout << s1;
//	//unique_ptr<VectorXf> out = make_unique<VectorXf>(3);
//	//out(2) = 5;
//	//MatrixXf a(1, 5);
//
//	for (size_t i = 0; i < 2000000; i++)
//	{
//		Test();
//	}
//
//	//VectorXf t = bet_sizing;
//	//cout << a;
//	//CardArray ar1 = CardArray::Ones(1);
//	//CardArray ar = CardArray(1).setOnes();
//	//ArrayXXf table(3, 4);
//	//table.row(0) = 4;
//	//cout << table;
//	//int g = table.rows();
//
//	//MatrixXf a(1, 5);
//	//auto ar = ArrayXf::LinSpaced(5, 0, 5 -1);
//	////cout << ar;
//	//cout << ar.row(2);
//
//	////cout << a.col(0);
//
//	//a.row(0) = ar;
//	//cout << a(0, 0);
//	//cout << a(0, 1);
//	//cout << a(0, 2);
//	//cout << a(0, 3);
//	//cout << a(0, 4);
//
//	//cout << a(1, 0);
//
//	////cout << a.row(0);
//
//
//	//cout << time(NULL);
//
//	//ArrayXf b(1);
//	//b << 1;
//
//	//ArrayXf cg = b * 2;
//
//	//ArrayXf c(b);
//
//}
//
template <class C>
void
display(const C& c)
{
	std::cout << '{';
	if (!c.empty())
		std::cout << *c.front();
	for (auto i = std::next(c.begin()); i != c.end(); ++i)
		std::cout << ", " << **i;
	std::cout << "}\n";
}

int main(int argc, char **argv)
{
	Node node;
	//node.children = vector <unique_ptr<Node>>();
	//vector<unique_ptr<Node>> children = node.children;

	long long i = 0;
	for (auto&& child_node : node.children)
	{
	}


	//typedef std::unique_ptr<int> Ptr;
	//std::vector<Ptr> v;
	//for (int i = 1; i <= 5; ++i)
	//	v.push_back(Ptr(new int(i)));
	//display(v);

	//for (unsigned long long i = 0; i < v.size(); i++)
	//{

	//	 *v[i] += 2;
	//}

	////for (auto i = v.begin(); i != v.end(); ++i)
	////	**i += 2;
	//display(v);
}