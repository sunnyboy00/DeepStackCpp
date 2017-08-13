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

		////Coping source tensor to the target tensor when target and source dimensions number are difference
		//template <int N1>
		//template <int N2>
		//static inline void CopyDif(Tensor<float, N1> & target, const Tensor<float, N2>& source)
		//{
		//	assert(target.size() >= source.size());
		//	memcpy(target.data(), source.data(), source.size() * sizeof(float));
		//}

		//Coping source tensor to the target tensor
		template <int N>
		static inline void Copy(TfN& target, const TfN& source)
		{
			assert(target.size() >= source.size());
			memcpy(target.data(), source.data(), source.size() * sizeof(float));
		}

		// Coping source tensor map to the target tensor
		template <int N>
		static inline void Copy(TfN& target, const TmN& source)
		{
			assert(target.size() >= source.size());
			memcpy(target.data(), source.data(), source.size() * sizeof(float));
		}

		static inline void Copy(Tf2 & target, Tf2 & source)
		{
			assert(target.size() >= source.size());
			memcpy(target.data(), source.data(), source.size() * sizeof(float));
		}

		template <int N>
		static inline TfN ExpandAs(TfN& data, const TfN& as)
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

		template <int N>
		static inline TfN ExpandAs(TmN& data, const TfN& as)
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

		//// Expands one tensor as other one
		//template<typename Derived, typename OtherDerived>
		//static inline TensorBase<Derived> ExpandAs(TensorBase<Derived>& data, const TensorBase<OtherDerived>& as)
		//{
		//	std::array<ptrdiff_t, data.NumDimensions> broadcasts;
		//	
		//	//ToDo: remove during optimization for performance reasons
		//	for (size_t dim = 0; dim < data.NumDimensions; dim++)
		//	{
		//		float difDim = (float)as.dimension(dim) / data.dimension(dim);
		//		assert(difDim >= 1.0);
		//		assert(ceilf(difDim) == difDim && "The coefficients must be integers");
		//		broadcasts[dim] = (int)difDim;
		//	}

		//	TensorBase<Derived> res = data.broadcast(broadcasts);
		//	return res;
		//}

		// Expands one array as other one
		static inline Tf2 ExpandAs(Tf2 data, Tf2 as)
		{
			//ToDo: remove during optimization for performance reasons
			float difCols = (float)as.dimension(1) / data.dimension(1);
			assert(difCols >= 1.0);
			assert(ceilf(difCols) == difCols && "The coefficients must be integers");

			float difRows = (float)as.dimension(0) / data.dimension(0);
			assert(difRows >= 1.0);
			assert(ceilf(difRows) == difRows && "The coefficients must be integers");

			Tf2 res = data.replicate((int)difRows, (int)difCols);
			return res;
		}

		// Expands one array as other one
		static inline Tf2 ExpandAs(Tf1 data, Tf2 as)
		{
			return data.replicate(as.dimension(0), as.dimension(1));
		}

		// Resizes the tensor and fills it with some value
		template <int N>
		static inline void ResizeAndFill(TfN &target, std::array<DenseIndex, N> const &dims, float value = 0)
		{
			target.resize(dims);
			target.setConstant(value);
		}

		// Transpose (or shuffle) tensor dims
		template <int N>
		static inline TfN Transpose(const TfN &target, std::array<DenseIndex, N> const &dims)
		{
			TfN output = target.shuffle(dims);
			return output;
		}

		// Converters negative indexes offsets to positive zero indexes.
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

		// Converters if negative indexes of {fromOffsetDN, toInclusiveOffsetD1N} are negative to positive zero based indexes.
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
		// Example Util::Slice(target, {{ {fromOffsetD1, toInclusiveOffsetD1}, {fromOffsetD2, toInclusiveOffsetD12}, ... {fromOffsetDN, toInclusiveOffsetD1N} }})
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

		template <int N>
		static inline void CopyToSlice(TfN &target, std::array<std::array<DenseIndex, 2>, N> const &slices, TfN source)
		{
			Eigen::array<DenseIndex, N> offsets;
			Eigen::array<DenseIndex, N> extentsLen;

			PreprocessExtents(slices, target, offsets, extentsLen);
			target.slice(offsets, extentsLen) = source;
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

		static Tf5 NoReductionSum(Tf5 target, int dim)
		{
			assert(dim < target.NumDimensions);
			std::array<DenseIndex, 1> dims = { dim };
			Tf4 res = target.sum(dims);
			std::array<DenseIndex, 5> new_dims{ { target.dimension(0), target.dimension(1), target.dimension(2), target.dimension(3), target.dimension(4) } };
			new_dims[dim] = 1;
			return res.reshape(new_dims);
		}

		static Tf4 NoReductionSum(Tf4 target, int dim)
		{
			assert(dim < target.NumDimensions);
			std::array<DenseIndex, 1> dims = { dim };
			Tf3 res = target.sum(dims);
			std::array<DenseIndex, 4> new_dims{ { target.dimension(0), target.dimension(1), target.dimension(2), target.dimension(3)} };
			new_dims[dim] = 1;
			return res.reshape(new_dims);
		}

		static Tf3 NoReductionSum(Tf3 target, int dim)
		{
			assert(dim < target.NumDimensions);
			std::array<DenseIndex, 1> dims = { dim };
			Tf2 res = target.sum(dims);
			std::array<DenseIndex, 3> new_dims{ { target.dimension(0), target.dimension(1), target.dimension(2)} };
			new_dims[dim] = 1;
			return res.reshape(new_dims);
		}

		static Tf2 NoReductionSum(Tf2 target, int dim)
		{
			assert(dim < target.NumDimensions);
			std::array<DenseIndex, 1> dims = { dim };
			Tf1 res = target.sum(dims);
			std::array<DenseIndex, 2> new_dims{ { target.dimension(0), target.dimension(1) } };
			new_dims[dim] = 1;
			return res.reshape(new_dims);
		}

		// Calculates and replaces one -1 dimension of tensor from sizes basing on the targetSize.
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
					negativeIndex = dim;
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

		static inline Tm2 View(Tf2 &source, std::array<int, 2>& sizes)
		{
			assert(source.size() == sizes[0] * sizes[1]);
			Util::ProcessSizes((int)source.size(), sizes);
			return Tm2(source.data(), sizes[0], sizes[1]);
		}

		static inline Tm3 View(Tf3 &source, std::array<int, 3>& sizes)
		{
			Util::ProcessSizes((int)source.size(), sizes);
			return Tm3(source.data(), sizes[0], sizes[1], sizes[2]);
		}

		static inline Tm4 View(Tf4 &source, std::array<int, 3>& sizes)
		{
			Util::ProcessSizes((int)source.size(), sizes);
			return Tm4(source.data(), sizes[0], sizes[1], sizes[2], sizes[3]);
		}

		static inline Tm5 View(Tf5 &source, std::array<int, 5>& sizes)
		{
			Util::ProcessSizes((int)source.size(), sizes);
			return Tm5(source.data(), sizes[0], sizes[1], sizes[2], sizes[3], sizes[4]);
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

		static inline MatrixXf ExpandAs(Tf1 data, MatrixXf as)
		{
			return data.replicate(as.dimension(0), as.dimension(1));
		}

		static inline void ClipLow(Tf2& target, float lowLimin)
		{
			target = (target >= lowLimin).select(
				target,
				Tf2::Constant(target.dimension(0), target.dimension(1), lowLimin)
			);
		}

		static inline void ClipLow(Tf1& target, float lowLimin)
		{
			target = (target >= lowLimin).select(
				target,
				Tf1::Constant(target.dimension(0), target.dimension(1), lowLimin)
			);
		}

		static inline void Clip(Tf2& target, float lowLimin, float maxValue)
		{
			target = (target >= lowLimin).select(
				target,
				Tf2::Constant(target.dimension(0), target.dimension(1), lowLimin)
			);

			target = (target < maxValue).select(
				target,
				Tf2::Constant(target.dimension(0), target.dimension(1), maxValue)
			);
		}

		static inline void Clip(CardArray& target, float lowLimin, float maxValue)
		{
			target = (target >= lowLimin).select(
				target,
				CardArray::Constant(target.dimension(0), target.dimension(1), lowLimin)
			);

			target = (target < maxValue).select(
				target,
				CardArray::Constant(target.dimension(0), target.dimension(1), maxValue)
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
		//		MatrixXf::Constant(target.dimension(0), target.dimension(1), lowLimin)
		//	);
		//}

		static void ToString(const Tf2& dataArg);

		static void ToString(const MatrixXf& dataArg);

		//static inline TensorMap<Tensor<float, 2>> ToTensor(Tf2 & source)
		//{
		//	return TensorMap<Tensor<float, 2>>(source.data(), source.dimension(0), source.dimension(1));
		//}

		//static inline TensorMap<Tensor<float, 2>> ToTensor(MatrixXf & source)
		//{
		//	return TensorMap<Tensor<float, 2>>(source.data(), source.dimension(0), source.dimension(1));
		//}

		//// Returns 2d tensor from 3d 
		//static Map<Tf2> TensorToArray2d(Tensor<float, 3>& tensor, int offset, int dim, Tensor<float, 2>& tempVar)
		//{
		//	tempVar = tensor.chip(offset, dim);
		//	Map<Tf2> plCfAr(tempVar.data(), tempVar.dimension(0), tempVar.dimension(1));
		//	return plCfAr;
		//}

		//static inline void CopyTo(MatrixXf & target, MatrixXf & source)
		//{
		//	assert(target.size() >= source.size());
		//	memcpy(target.data(), source.data(), source.size() * sizeof(float));
		//}

};


