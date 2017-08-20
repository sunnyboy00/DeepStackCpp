#pragma once
#include <unsupported/Eigen/CXX11/Tensor>
#include <Eigen/Dense>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "CustomSettings.h"

using namespace Eigen;
using namespace std;

#define edim(dimSlice) Eigen::array<int, 1> { { dimSlice} }

class Util
{
	public:

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

		template <typename Derived>
		static inline void Copy(ArrayBase<Derived> & target, ArrayBase<Derived> & source)
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

		static void Print(Tf5& tensor);

		static void Print(Tf4& tensor);

		static void Print(Tf3& tensor);

		static void Print(Tf2& tensor);

		static void Print(Tf1& tensor);

		static void Print(const vector<Tf5>& target, int index);
		static void Print(const std::map<int, Tf5>& target, int index);

		static void Print(const vector<Tf4>& target, int index);
		static void Print(const std::map<int, Tf4>& target, int index);

		static void Print(const vector<Tf3>& target, int index);
		static void Print(const std::map<int, Tf3>& target, int index);

		static void Print(const vector<Tf2>& target, int index);
		static void Print(const std::map<int, Tf2>& target, int index);

		static void Print(const vector<Tf1>& target, int index);
		static void Print(const std::map<int, Tf1>& target, int index);
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
		static inline ArrayXX ExpandAs(ArrayXX data, ArrayXX as)
		{
			//ToDo: remove during optimization for performance reasons
			float difCols = (float)as.cols() / data.cols();
			assert(difCols >= 1.0);
			assert(ceilf(difCols) == difCols && "The coefficients must be integers");

			float difRows = (float)as.rows() / data.rows();
			assert(difRows >= 1.0);
			assert(ceilf(difRows) == difRows && "The coefficients must be integers");

			ArrayXX res = data.replicate((int)difRows, (int)difCols);
			return res;
		}

		// Expands one array as other one
		static inline ArrayXX ExpandAs(ArrayX data, ArrayXX as)
		{
			return data.replicate(as.rows(), as.cols());
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

		//template <int N>
		//static inline void CopyToSlice(TfN &target, std::array<std::array<DenseIndex, 2>, N> const &slices, TfN source)
		//{
		//	Eigen::array<DenseIndex, N> offsets;
		//	Eigen::array<DenseIndex, N> extentsLen;

		//	PreprocessExtents(slices, target, offsets, extentsLen);
		//	target.slice(offsets, extentsLen) = source;
		//}

		template <int N, int N2>
		static void CopyToSlice(TfN &target, std::array<std::array<DenseIndex, 2>, N> const &slices, TfN2 source)
		{
			Eigen::array<DenseIndex, N> offsets;
			Eigen::array<DenseIndex, N> extentsLen;

			Util::PreprocessExtents(slices, target, offsets, extentsLen);
			auto slicedTarget = target.slice(offsets, extentsLen);

#ifdef _DEBUG
			size_t size = 1;
			for (size_t i = 0; i < N; i++)
			{
				size *= extentsLen[i];
			}
			assert(size == source.size());
#endif

			Eigen::array<Eigen::DenseIndex, N> dims;
			for (size_t i = 0; i < N; i++)
			{
				dims[i] = extentsLen[i];
			}

			slicedTarget = source.reshape(dims);
		}

		template <int N>
		static inline TfN NotReduceSum(const TfN& tensor, int dim)
		{
			const std::array<int, 1> sumDims = { dim };
			std::array<DenseIndex, N> dims = tensor.dimensions();
			dims[dim] = 1;
			return tensor.sum(sumDims).reshape(dims);
		}

		// Calculates and replaces one -1 dimension of tensor from sizes basing on the targetSize.
		template <int N>
		static inline void ProcessSizes(int targetSize, std::array<int, N>& sizes)
		{
			int negativeIndex = -1;
			int sizeUsed = 1;
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
					sizeUsed  *= curDimSize; 
					assert(sizeUsed <= targetSize);
				}
			}

			if (negativeIndex > 0)
			{ 
				float difKoef = (float)targetSize / sizeUsed;
				assert(ceilf(difKoef) == difKoef && "The coefficients must be integers");
				sizes[negativeIndex] = (int)difKoef;
			}
		};

		static inline Tm2 View(Tf2 &source, std::array<int, 2>& sizes)
		{
			Util::ProcessSizes((int)source.size(), sizes);
			assert(source.size() == sizes[0] * sizes[1]);
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

		static inline void Process2dSizes(size_t size, int& rows, int& cols)
		{
			assert(rows < 0 || cols < 0);
			if (rows == -1)
			{
				float newRows = (float)cols / size;
				assert(ceilf(newRows) == newRows && "The coefficients must be integers");
				rows = (int)newRows;
			}
			else if (cols == -1)
			{
				float newCols = (float)cols / size;
				assert(ceilf(newCols) == newCols && "The coefficients must be integers");
				cols = (int)newCols;
			}

			assert(rows * cols == size);
		}

		static inline TmAxx ArXXView(Tf5 &source, int rows, int cols)
		{
			Process2dSizes(source.size(), rows, cols);
			return TmAxx(source.data(), rows, cols);
		}

		static inline TmAxx ArXXView(Tf4 &source, int rows, int cols)
		{
			Process2dSizes(source.size(), rows, cols);
			return TmAxx(source.data(), rows, cols);
		}

		static inline TmAxx ArXXView(Tf3 &source, int rows, int cols)
		{
			Process2dSizes(source.size(), rows, cols);
			return TmAxx(source.data(), rows, cols);
		}

		static inline TmAxx ArXXView(Tf2 &source, int rows, int cols)
		{
			Process2dSizes(source.size(), rows, cols);
			return TmAxx(source.data(), rows, cols);
		}

		static Tf1 CardArrayToTensor(CardArray cardArray)
		{
			TensorMap<Tf1> cardMap(cardArray.data(), card_count);
			Tf1 resultTensor = cardMap;
			return resultTensor;
		}

		static inline MatrixX ExpandAs(VectorX data, MatrixX as)
		{
			return data.replicate(as.rows(), as.cols());
		}

		static inline void ClipLow(ArrayXX& target, float lowLimin)
		{
			target = (target >= lowLimin).select(
				target,
				ArrayXX::Constant(target.rows(), target.cols(), lowLimin)
			);
		}

		template <int N>
		static inline void ClipLow(TfN& target, float lowLimin)
		{
			target = target.cwiseMax(lowLimin);
		}

		static inline void ClipLow(ArrayX& target, float lowLimin)
		{
			target = target.cwiseMax(lowLimin);

		}

		static inline void Clip(ArrayX& target, float lowLimin, float maxValue)
		{
			target = target.cwiseMax(lowLimin).cwiseMin(maxValue);
		}

		static inline void Clip(ArrayXX& target, float lowLimin, float maxValue)
		{
			target = target.cwiseMax(lowLimin).cwiseMin(maxValue);
		}

		static inline void Clip(CardArray& target, float lowLimin, float maxValue)
		{
			target = target.cwiseMax(lowLimin).cwiseMin(maxValue);
		}

		template <int N>
		static inline void Clip(TfN& target, float lowLimin, float maxValue)
		{
			target = target.cwiseMax(lowLimin).cwiseMin(maxValue);
		}

		static void Print(const ArrayXX& dataArg);

		static void Print(const MatrixX& dataArg);

		static void Print(const ArrayX& dataArg);
};



