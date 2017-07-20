#pragma once
#include <unsupported/Eigen/CXX11/Tensor>
#include <Eigen/Dense>

using namespace Eigen;

#define edim(dimSlice) Eigen::array<int, 1> { { dimSlice} }

class Util
{
	public:
		static inline ArrayXXf ExpandAs(ArrayXXf data, ArrayXXf as);

		static inline ArrayXXf ExpandAs(ArrayXf data, ArrayXXf as);

		static inline MatrixXf ExpandAs(MatrixXf data, MatrixXf as);

		static inline MatrixXf ExpandAs(VectorXf data, MatrixXf as);

		static inline void ClipLow(ArrayXXf& target, float lowLimin);

		static inline void ClipLow(MatrixXf& target, float lowLimin);

		static inline void Util::ClipLow(ArrayXf& target, float lowLimin);

		static inline void CopyTo(MatrixXf& target, MatrixXf& source);

		static inline void CopyTo(ArrayXXf& target, ArrayXXf& source);

};
