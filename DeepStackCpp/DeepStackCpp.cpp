#include <iostream>
#include <Eigen/Dense>
#include <algorithm>  

using namespace std;
using Eigen::ArrayXf;
using Eigen::VectorXi;

int main(int argc, char **argv)
{
	ArrayXf a = ArrayXf::Random(5);
	a = 0;
	a += 2;
	cout << "a =" << endl
		<< a << endl;
}