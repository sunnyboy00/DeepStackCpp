#include <iostream>
#include <Eigen/Dense>
#include <algorithm>    

using Eigen::Vector3i;

int main(int argc, char **argv)
{
	Vector3i m;
	m << 1, 2, 3;
	//m.setZero();

	Eigen::Vector4i s;

	memcpy(s.data(), m.data(), m.size() * sizeof(int));
	std::cout << s;
}