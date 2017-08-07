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
		template <int N>
		static inline TfN ExpandAs(TfN data, TfN as)
		{
			std::array<ptrdiff_t, N> broadcasts;

			//ToDo: remove during optimization for performance reasons
			for (size_t dim = 0; dim < N; dim++)
			{
				float difDim = (float)as.dimension(dim) / data.dimension(dim);
				assert(difDim >= 1.0);
				assert(ceilf(difDim) == difDim && "The coefficients must be integers");
				broadcasts[dim] = (int)difDim;
			}

			TfN res = data.broadcast(broadcasts);
			return res;
		}

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
		static inline void ResizeAndFill(TfN &target, std::array<DenseIndex, N> const &dims, float value = 0)
		{
			target.resize(dims);
			target.setConstant(value);
		}

		template <int N>
		static inline TfN Transpose(const TfN &target, std::array<DenseIndex, N> const &dims)
		{
			//Eigen::array<DenseIndex, 3> shuffling{ { 1, 2, 0 } };
			TfN output = target.shuffle(dims);
			return output;
		}


		template <int N>
		static inline DenseIndex ConvertOffset(const TfN& target, DenseIndex offset, DenseIndex dim)
		{
			if (offset < 0)
			{
				// ToDo: Check that it works as in torch. -1 should be the last element.
				assert(abs(offset) <= target.dimension(dim));
				return offset + (DenseIndex)target.dimension(dim);
			}

			return offset;
		}

		template <int N>
		static void PreprocessExtents(std::array<std::array<DenseIndex, 2>, N> const & slices, 
			const TfN &target, 
			Eigen::array<DenseIndex, N> &extentsStarts,
			Eigen::array<DenseIndex, N> &extentsLen)
		{
			for (DenseIndex dim = 0; dim < N; dim++)
			{
				extentsStarts[dim] = ConvertOffset(target, slices[dim][0], dim);
				DenseIndex extentEnd = ConvertOffset(target, slices[dim][1], dim); // Inclusive!
				assert(extentEnd >= extentsStarts[dim]);
				extentsLen[dim] = extentEnd - slices[dim][0] + 1; // Extent length +1 because end is inclusive
				assert(extentsLen[dim] > 0);
			}
		}
		// Emulates slices in Torch. Takes array of array of two elements: offset and extent. 
		// Extent may be negative- that means negative index from the end of dimension.
		// If extent is zero - that dimension is not sliced.
		// Returns THE NEW TENSOR. WITH COPY. ToDo: Check all usages for perf optimization
		template <int N>
		static inline TfN Slice(const TfN &target, std::array<std::array<DenseIndex, 2>, N> const &slices)
		{
			Eigen::array<DenseIndex, N> offsets;
			Eigen::array<DenseIndex, N> extentsLen;

			PreprocessExtents(slices, target, offsets, extentsLen);
			return target.slice(offsets, extentsLen);
		}

		// Fill the slice
		template <int N>
		static inline void FillSlice(TfN &target, std::array<std::array<DenseIndex, 2>, N> const &slices, float value)
		{
			Eigen::array<DenseIndex, N> offsets;
			Eigen::array<DenseIndex, N> extentsLen;

			PreprocessExtents(slices, target, offsets, extentsLen);
			target.slice(offsets, extentsLen).setConstant(value);
		}

		////Creates a view with different dimensions of the storage associated with tensor.
		////If one of the dimensions is - 1, the size of that dimension is inferred from the rest of the elements.
		//template <int N>
		//static inline TensorMap<TfN, N> View(TfN &target, std::array<int, N>& sizes)
		//{
		//	int negativeIndex = -1;
		//	int sizeLeft = target.size();
		//	for (int dim = 0; dim < N; dim++)
		//	{
		//		int curDimSize = sizes[dim];
		//		if (curDimSize == -1)
		//		{
		//			assert(negativeIndex == -1);
		//			negativeIndex = curDimSize;
		//		}
		//		else
		//		{
		//			assert(curDimSize > 0);
		//			sizeLeft -= curDimSize;
		//			assert(sizeLeft >= 0);
		//		}
		//	}

		//	if (negativeIndex > 0)
		//	{
		//		sizes[negativeIndex] = sizeLeft;
		//	}


		//	switch (N)
		//	{
		//	case 1:
		//		return TensorMap<TfN, 1>(target.data(), sizes[0]);
		//	case 2:
		//		return TensorMap<TfN, 2>(target.data(), sizes[0], sizes[1]);
		//	case 3:
		//		return TensorMap<TfN, 2>(target.data(), sizes[0], sizes[1], sizes[2]);
		//	case 4:
		//		return TensorMap<TfN, 2>(target.data(), sizes[0], sizes[1], sizes[2], sizes[3]);
		//	case 5:
		//		return TensorMap<TfN, 2>(target.data(), sizes[0], sizes[1], sizes[2], sizes[3], sizes[4]);
		//	default:
		//		throw std::invalid_argument("Invalid tensor size");
		//	};
		//};

		template <int N>
		static inline void ProcessSizes(int targetSize, std::array<int, N>& sizes)
		{
			int negativeIndex = -1;
			int sizeLeft = targetSize;
			for (int dim = 0; dim < N; dim++)
			{
				int curDimSize = sizes[dim];
				if (curDimSize == -1)
				{
					assert(negativeIndex == -1);
					negativeIndex = curDimSize;
				}
				else
				{
					assert(curDimSize > 0);
					sizeLeft -= curDimSize;
					assert(sizeLeft >= 0);
				}
			}

			if (negativeIndex > 0)
			{
				sizes[negativeIndex] = sizeLeft;
			}
		};

		static inline TensorMap<Tf2, 2> View(Tf2 &target, std::array<int, 2>& sizes)
		{
			Util::ProcessSizes((int)target.size(), sizes);
			return TensorMap<Tf2, 2>(target.data(), sizes[0], sizes[1]);
		}

		static inline TensorMap<Tf3, 3> View(Tf3 &target, std::array<int, 3>& sizes)
		{
			Util::ProcessSizes((int)target.size(), sizes);
			return TensorMap<Tf3, 3>(target.data(), sizes[0], sizes[1], sizes[2]);
		}

		static inline TensorMap<Tf4, 4> View(Tf4 &target, std::array<int, 3>& sizes)
		{
			Util::ProcessSizes((int)target.size(), sizes);
			return TensorMap<Tf4, 4>(target.data(), sizes[0], sizes[1], sizes[2], sizes[3]);
		}

		static inline TensorMap<Tf5, 5> View(Tf5 &target, std::array<int, 5>& sizes)
		{
			Util::ProcessSizes((int)target.size(), sizes);
			return TensorMap<Tf5, 5>(target.data(), sizes[0], sizes[1], sizes[2], sizes[3], sizes[4]);
		}

		static Tf1 CardArrayToTensor(CardArray cardArray)
		{
			TensorMap<Tf1> cardMap(cardArray.data(), card_count);
			Tf1 resultTensor = cardMap;
			return resultTensor;
		}

		// Makes 
		//template <int N>
		//static void MultiSliceFill(TfN& target, std::array<DenseIndex, N> const &offsets, std::array<DenseIndex, N> const &slices, float value)
		//{
		//	Eigen::array<DenseIndex, N> offsets;
		//	Eigen::array<DenseIndex, N> extents;

		//	for (DenseIndex dim = 0; dim < N; dim++)
		//	{
		//		std::array<DenseIndex, 2> currentExtent = slices[dim];
		//		DenseIndex offset = currentExtent[0];
		//		DenseIndex extent = currentExtent[1];

		//		if (extent != 0) // Means that we will not slice this dimension
		//		{
		//			offsets[dim] = 0;
		//			extents[dim] = target.dimension(dim);
		//		}
		//		else
		//		{
		//			offsets[dim] = ConvertOffset(target, offset, dim);
		//			extents[dim] = ConvertOffset(target, extent, dim);
		//		}
		//	}

		//	TfN currentRes = target;

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
		////};

		//// Makes 
		//template <int N>
		//static void MultiSliceFill(TfN& target, std::array<DenseIndex, N> const &offsets, std::array<DenseIndex, N> const &dims, float value)
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

		//	TfN currentRes = target;

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
		static inline void Clip(TfN& target, float lowLimin, float maxValue)
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
		static inline void ClipLow(TfN& target, float lowLimin)
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


