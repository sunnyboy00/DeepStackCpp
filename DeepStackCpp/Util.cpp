#include "Util.h"
#include <fstream>

std::wstring string_to_wstring(const std::string& str) {
	return std::wstring(str.begin(), str.end());
}

void Util::Print(const ArrayXX& dataArg)
{
	IOFormat CleanFmt(3, 0, ", ", "\n", "[", "]");

	ostringstream ss;
	//Eigen::internal::print_matrix(ss, dataArg, CleanFmt);
	ss << dataArg.format(CleanFmt) << endl;
	std::wostringstream result;
	result << string_to_wstring(ss.str());
	OutputDebugString(result.str().c_str());
}

void Util::Print(const MatrixX& dataArg)
{
	IOFormat CleanFmt(3, 0, ", ", "\n", "[", "]");
	ostringstream ss;
	ss << dataArg.format(CleanFmt) << endl;
	std::wostringstream result;
	result << string_to_wstring(ss.str());
	OutputDebugString(result.str().c_str());
}

void Util::Print(const vector<Tf5>& target, int index)
{
	Tf5 tensor = target.at(index);
	Util::Print(tensor);
}

void Util::Print(const std::map<int, Tf5>& target, int index)
{
	Tf5 tensor = target.at(index);
	Util::Print(tensor);
}

void Util::Print(const vector<Tf4>& target, int index)
{
	Tf4 tensor = target.at(index);
	Util::Print(tensor);
}

void Util::Print(const std::map<int, Tf4>& target, int index)
{
	Tf4 tensor = target.at(index);
	Util::Print(tensor);
}

void Util::Print(const vector<Tf3>& target, int index)
{
	Tf3 tensor = target.at(index);
	Util::Print(tensor);
}

void Util::Print(const std::map<int, Tf3>& target, int index)
{
	Tf3 tensor = target.at(index);
	Util::Print(tensor);
}

void Util::Print(const vector<Tf2>& target, int index)
{
	Tf2 tensor = target.at(index);
	Util::Print(tensor);
}

void Util::Print(const std::map<int, Tf2>& target, int index)
{
	Tf2 tensor = target.at(index);
	Util::Print(tensor);
}

void Util::Print(const vector<Tf1>& target, int index)
{
	Tf1 tensor = target.at(index);
	Util::Print(tensor);
}

void Util::Print(const std::map<int, Tf1>& target, int index)
{
	Tf1 tensor = target.at(index);
	Util::Print(tensor);
}


void Util::Print(Tf5& tensor)
{
	std::wostringstream result;
	ostringstream ss;
	ss << "5d: " << tensor.dimension(0) << "x" << tensor.dimension(1) << "x" << tensor.dimension(2) << "x" << tensor.dimension(3) << "x" << tensor.dimension(4) << endl;

	for (DenseIndex d1 = 0; d1 < tensor.dimension(0); d1++)
	{
		for (DenseIndex d2 = 0; d2 < tensor.dimension(1); d2++)
		{
			for (DenseIndex d3 = 0; d3 < tensor.dimension(2); d3++)
			{
				ss << "(" << d1 << "," << d2 << "," << d3 << ")=" << endl;
				for (DenseIndex d4 = 0; d4 < tensor.dimension(3); d4++)
				{
					for (DenseIndex d5 = 0; d5 < tensor.dimension(4); d5++)
					{
						ss << tensor(d1, d2, d3, d4, d5) << "; ";
					}

					ss << endl;
				}
			}
		}
	}

	result << string_to_wstring(ss.str());
	OutputDebugString(result.str().c_str());
}

void Util::Print(Tf4& tensor)
{
	std::wostringstream result;
	ostringstream ss;
	ss << "4d: " << tensor.dimension(0) << "x" << tensor.dimension(1) << "x" << tensor.dimension(2) << "x" << tensor.dimension(3) << endl;

	for (DenseIndex d1 = 0; d1 < tensor.dimension(0); d1++)
	{
		for (DenseIndex d2 = 0; d2 < tensor.dimension(1); d2++)
		{
			ss << "(" << d1 << "," << d2 << ")=" << endl;
			for (DenseIndex d3 = 0; d3 < tensor.dimension(2); d3++)
			{
				for (DenseIndex d4 = 0; d4 < tensor.dimension(3); d4++)
				{
					ss << tensor(d1, d2, d3, d4) << "; ";
				}

				ss << endl;
			}
		}
	}

	result << string_to_wstring(ss.str());
	OutputDebugString(result.str().c_str());
}

void Util::Print(Tf3& tensor)
{
	std::wostringstream result;
	ostringstream ss;
	ss << "3d: " << tensor.dimension(0) << "x" << tensor.dimension(1) << "x" << tensor.dimension(2) << endl;

	for (DenseIndex d1 = 0; d1 < tensor.dimension(0); d1++)
	{
		ss << "(" << d1 << ")=" << endl;
		for (DenseIndex d2 = 0; d2 < tensor.dimension(1); d2++)
		{
			for (DenseIndex d3 = 0; d3 < tensor.dimension(2); d3++)
			{
				ss << tensor(d1, d2, d3) << "; ";
			}

			ss << endl;
		}
	}

	result << string_to_wstring(ss.str());
	OutputDebugString(result.str().c_str());
}

void Util::Print(Tf2& tensor)
{
	std::wostringstream result;
	ostringstream ss;
	ss << "2d: " << tensor.dimension(0) << "x" << tensor.dimension(1) << endl;

	for (DenseIndex d1 = 0; d1 < tensor.dimension(0); d1++)
	{
		for (DenseIndex d2 = 0; d2 < tensor.dimension(1); d2++)
		{
			ss << tensor(d1, d2) << "; ";
		}

		ss << endl;
	}

	result << string_to_wstring(ss.str());
	OutputDebugString(result.str().c_str());
}

//std::string res_str(res.begin(), res.end());;
void Util::Print(Tf1 & tensor)
{
	std::wostringstream result;
	ostringstream ss;

	for (DenseIndex d1 = 0; d1 < tensor.dimension(0); d1++)
	{
		ss << tensor(d1) << "; ";
	}

	result << string_to_wstring(ss.str());
	OutputDebugString(result.str().c_str());
}

void Util::Print(const ArrayX& dataArg)
{
	std::wostringstream result;
	ostringstream ss;

	for (DenseIndex d1 = 0; d1 < dataArg.rows(); d1++)
	{
		ss << dataArg(d1) << "; ";
	}

	result << string_to_wstring(ss.str());
	OutputDebugString(result.str().c_str());
}

