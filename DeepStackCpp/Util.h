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
		static inline Tensor<float, N> Transpose(const Tensor<float, N> &target, std::array<DenseIndex, N> const &dims)
		{
			//Eigen::array<DenseIndex, 3> shuffling{ { 1, 2, 0 } };
			Tensor<float, N> output = target.shuffle(dims);
			return output;
		}


		template <int N>
		static inline int ConvertOffset(Tensor<float, N>& target, int offset, int dim)
		{
			if (offset < 0)
			{
				// ToDo: Check that it works as in torch. -1 should be the last element.
				assert(abs(offset) < target.dimension(dim));
				return offset + (int)target.dimension(dim);
			}

			return offset;
		}

		template <int N>
		static inline Tensor<float, N> Slice(const Tensor<float, N> &target, std::array<std::array<DenseIndex, 2>, N> const &offsets)
		{
			Eigen::array<int, N> offsets[N];
			Eigen::array<int, N> extents[N];

			for (size_t dim = 0; dim < length; dim++)
			{
				std::array<DenseIndex, 2> currentExtent = offsets[dim];
				DenseIndex offset = currentExtent[0];
				DenseIndex extent = currentExtent[1];

				if (offset == 0 && extent == 0) // Means that we will not slice this dimension
				{
					offsets[dim] = 0;
					extents[dim] = target.dimension(dim);
				}
				else
				{
					assert(offset > 0);
					offsets[dim] = ConvertOffset(target, offset, dim);
					extents[dim] = ConvertOffset(target, extent, dim);
				}
			}

			Tensor<float, N> output = target.slice(offsets, extents);
			return output;
		}

		//static inline MatrixXf ExpandAs(MatrixXf data, MatrixXf as)
		//{
		//	return data.replicate(as.rows(), as.cols());
		//}

		static Tf1 CardArrayToTensor(CardArray cardArray)
		{
			TensorMap<Eigen::Tensor<float, 1>> cardMap(cardArray.data(), card_count);
			Tf1 resultTensor = cardMap;
			return resultTensor;
		}

		//template <int N>
		//static void MultiSliceFill(Tensor<float, N>& target, std::array<DenseIndex, N> const &offsets, std::array<DenseIndex, N> const &dims, float value)
		//{
		//	std::array<int, N> convertedOffsets;
		//	std::array<int, N> convertedDims;

		//	for (size_t i = 0; i < offsets.size(); i++)
		//	{
		//		if (dims[i] < 0)
		//		{
		//			assert(abs(dims[i]) < target.dimension(i));
		//			convertedOffsets[i] = dims[i] + target.dimension(i);
		//		}
		//		else
		//		{
		//			convertedOffsets[i] = dims[i];
		//		}
		//	}

		//	Tensor<float, N> currentRes = target;

		//	bool inited = false;

		//	int missedAxes = 0;

		//	vector<void*> op;

		//	for (size_t i = 0; i < dims.size(); i++)
		//	{
		//		if (dims[i] == 0)
		//		{
		//			Tensor<float, N - 1> currentRes = target.chip(0, 0);
		//			//Tensor<float, N - 1> currentRes = target.chip(convertedOffsets[i], missedAxes);


		//			//if (!inited)
		//			//{
		//			//	currentRes = target.chip(convertedOffsets[i], missedAxes);
		//			//	inited = true;
		//			//}
		//			//else
		//			//{
		//			//	currentRes = currentRes.chip(convertedOffsets[i], missedAxes);
		//			//}
		//		}
		//		else
		//		{
		//			assert(dims[i] == 1);
		//			missedAxes++;
		//		}
		//	}

		//	currentRes.setConstant(value);
		//};

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

		template <int N>
		static inline void Clip(Tensor<float, N>& target, float lowLimin, float maxValue)
		{
			target = (target >= lowLimin).select(
				target,
				target.setConstant(lowLimin)
			);

			target = (target < maxValue).select(
				target,
				target.setConstant(maxValue)
			);
		}

		template <int N>
		static inline void ClipLow(Tensor<float, N>& target, float lowLimin)
		{
			target = (target >= lowLimin).select(
				target,
				target.setConstant(lowLimin)
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


