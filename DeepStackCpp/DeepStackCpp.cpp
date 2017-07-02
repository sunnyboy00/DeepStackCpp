#include <iostream>
#include <Eigen/Dense>
#include <algorithm>  
#include <time.h>

#include "CustomSettings.h"
#include "arguments.h"

using namespace std;
using Eigen::ArrayXf;
using Eigen::ArrayXXf;
using Eigen::VectorXi;
using Eigen::MatrixXf;
using Eigen::VectorXf;
ArrayXf Test()
{
	ArrayXf b;
	return b;
}

int main(int argc, char **argv)
{
	VectorXf t = bet_sizing;
	cout << t;
	CardArray ar1 = CardArray::Ones(1);
	//CardArray ar = CardArray(1).setOnes();
	//ArrayXXf table(3, 4);
	//table.row(0) = 4;
	//cout << table;
	//int g = table.rows();

	//MatrixXf a(1, 5);
	//auto ar = ArrayXf::LinSpaced(5, 0, 5 -1);
	////cout << ar;
	//cout << ar.row(2);

	////cout << a.col(0);

	//a.row(0) = ar;
	//cout << a(0, 0);
	//cout << a(0, 1);
	//cout << a(0, 2);
	//cout << a(0, 3);
	//cout << a(0, 4);

	//cout << a(1, 0);

	////cout << a.row(0);


	//cout << time(NULL);

	//ArrayXf b(1);
	//b << 1;

	//ArrayXf cg = b * 2;

	//ArrayXf c(b);

}