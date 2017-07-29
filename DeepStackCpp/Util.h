#pragma once
#include <unsupported/Eigen/CXX11/Tensor>
#include <Eigen/Dense>
#include <iostream>
#include <sstream>
#include <string>
#include "CustomSettings.h"

using namespace Eigen;
using namespace std;

#define edim(dimSlice) Eigen::array<int, 1> { { dimSlice} }

class Util
{
	public:
		static inline ArrayXXf ExpandAs(ArrayXXf data, ArrayXXf as)
		{
			//ToDo: remove during optimization for performance reasons
			float difCols = (float)as.cols() / data.cols();
			assert(difCols >= 1.0);
			assert(ceilf(difCols) == difCols && "The coefficients must be integers");

			float difRows = (float)as.rows() / data.rows();
			assert(difRows >= 1.0);
			assert(ceilf(difRows) == difRows && "The coefficients must be integers");

			ArrayXXf res = data.replicate((int)difRows, (int)difCols);
			return res;
		}

		static inline ArrayXXf ExpandAs(ArrayXf data, ArrayXXf as)
		{
			return data.replicate(as.rows(), as.cols());
		}

		template <int N>
		static inline void ResizeAndFill(Tensor<float, N> &target, std::array<DenseIndex, N> const &dims, float value = 0)
		{
			target.resize(dims);
			target.setConstant(value);
		}

		template <int N>
		static inline Tensor<float, N> Transpose(const Tensor<float, N> &target, int dim1, int dim2)
		{
			Eigen::array<int, 2> contraction_indices = { dim1, dim2 };
			Eigen::Tensor<int, N> result = A.contract(B, contraction_indices);
			return result;
		}

		//static inline MatrixXf ExpandAs(MatrixXf data, MatrixXf as)
		//{
		//	return data.replicate(as.rows(), as.cols());
		//}

		static inline MatrixXf ExpandAs(VectorXf data, MatrixXf as)
		{
			return data.replicate(as.rows(), as.cols());
		}

		static inline void ClipLow(ArrayXXf& target, float lowLimin)
		{
			target = (target >= lowLimin).select(
				target,
				ArrayXXf::Constant(target.rows(), target.cols(), lowLimin)
			);
		}

		static inline void ClipLow(ArrayXf& target, float lowLimin)
		{
			target = (target >= lowLimin).select(
				target,
				ArrayXf::Constant(target.rows(), target.cols(), lowLimin)
			);
		}

		static inline void Clip(ArrayXXf& target, float lowLimin, float maxValue)
		{
			target = (target >= lowLimin).select(
				target,
				ArrayXXf::Constant(target.rows(), target.cols(), lowLimin)
			);

			target = (target < maxValue).select(
				target,
				ArrayXXf::Constant(target.rows(), target.cols(), maxValue)
			);
		}

		static inline void Clip(CardArray& target, float lowLimin, float maxValue)
		{
			target = (target >= lowLimin).select(
				target,
				CardArray::Constant(target.rows(), target.cols(), lowLimin)
			);

			target = (target < maxValue).select(
				target,
				CardArray::Constant(target.rows(), target.cols(), maxValue)
			);
		}

		//static inline void ClipLow(MatrixXf& target, float lowLimin)
		//{
		//	target = (target.array() >= lowLimin).select(
		//		target,
		//		MatrixXf::Constant(target.rows(), target.cols(), lowLimin)
		//	);
		//}

		static inline void CopyTo(ArrayXXf & target, ArrayXXf & source)
		{
			assert(target.size() >= source.size());
			memcpy(target.data(), source.data(), source.size() * sizeof(float));
		}

		static void ToString(const ArrayXXf& dataArg);

		static void ToString(const MatrixXf& dataArg);

		//static inline TensorMap<Tensor<float, 2>> ToTensor(ArrayXXf & source)
		//{
		//	return TensorMap<Tensor<float, 2>>(source.data(), source.rows(), source.cols());
		//}

		//static inline TensorMap<Tensor<float, 2>> ToTensor(MatrixXf & source)
		//{
		//	return TensorMap<Tensor<float, 2>>(source.data(), source.rows(), source.cols());
		//}

		//// Returns 2d tensor from 3d 
		//static Map<ArrayXXf> TensorToArray2d(Tensor<float, 3>& tensor, int offset, int dim, Tensor<float, 2>& tempVar)
		//{
		//	tempVar = tensor.chip(offset, dim);
		//	Map<ArrayXXf> plCfAr(tempVar.data(), tempVar.dimension(0), tempVar.dimension(1));
		//	return plCfAr;
		//}

		//static inline void CopyTo(MatrixXf & target, MatrixXf & source)
		//{
		//	assert(target.size() >= source.size());
		//	memcpy(target.data(), source.data(), source.size() * sizeof(float));
		//}

};

