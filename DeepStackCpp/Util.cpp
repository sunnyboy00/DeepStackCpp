#include "Util.h"

ArrayXXf Util::ExpandAs(ArrayXXf data, ArrayXXf as)
{
	return data.replicate(as.rows(), as.cols());
}

ArrayXXf Util::ExpandAs(ArrayXf data, ArrayXXf as)
{
	return data.replicate(as.rows(), as.cols());
}

MatrixXf Util::ExpandAs(MatrixXf data, MatrixXf as)
{
	return data.replicate(as.rows(), as.cols());
}

MatrixXf Util::ExpandAs(VectorXf data, MatrixXf as)
{
	return data.replicate(as.rows(), as.cols());
}

void Util::ClipLow(ArrayXXf& target, float lowLimin)
{
	target = (target >= lowLimin).select(
		target,
		ArrayXXf::Constant(target.rows(), target.cols(), lowLimin)
	);
}

void Util::ClipLow(ArrayXf& target, float lowLimin)
{
	target = (target >= lowLimin).select(
		target,
		ArrayXf::Constant(target.rows(), target.cols(), lowLimin)
	);
}

void Util::ClipLow(MatrixXf& target, float lowLimin)
{
	target = (target.array() >= lowLimin).select(
		target,
		MatrixXf::Constant(target.rows(), target.cols(), lowLimin)
	);
}

void Util::CopyTo(ArrayXXf & target, ArrayXXf & source)
{
	assert(target.size() >= source.size());
	memcpy(target.data(), source.data(), source.size() * sizeof(float));
}

void Util::CopyTo(ArrayXXf & target, ArrayXXf & source)
{
	assert(target.size() >= source.size());
	memcpy(target.data(), source.data(), source.size() * sizeof(float));
}

void Util::CopyTo(MatrixXf & target, MatrixXf & source)
{
	assert(target.size() >= source.size());
	memcpy(target.data(), source.data(), source.size() * sizeof(float));
}
