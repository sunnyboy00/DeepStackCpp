#include "Util.h"
#include <fstream>

std::wstring string_to_wstring(const std::string& str) {
	return std::wstring(str.begin(), str.end());
}

void Util::ToString(const ArrayXXf& dataArg)
{
	IOFormat CleanFmt(3, 0, ", ", "\n", "[", "]");

	ostringstream ss;
	//Eigen::internal::print_matrix(ss, dataArg, CleanFmt);
	ss << dataArg.format(CleanFmt) << endl;
	std::wostringstream result;
	result << string_to_wstring(ss.str());
	OutputDebugString(result.str().c_str());
}

void Util::ToString(const MatrixXf& dataArg)
{
	IOFormat CleanFmt(3, 0, ", ", "\n", "[", "]");
	ostringstream ss;
	ss << dataArg.format(CleanFmt) << endl;
	std::wostringstream result;
	result << string_to_wstring(ss.str());
	OutputDebugString(result.str().c_str());
}

