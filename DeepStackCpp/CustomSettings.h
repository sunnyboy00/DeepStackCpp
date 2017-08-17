#pragma once
#include <Eigen/Dense>
#include "game_settings.h"
#include <unsupported/Eigen/CXX11/Tensor>

typedef float mainDataType;

#define MyLayoutType Eigen::RowMajor//RowMajor

#define MatrixX Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, MyLayoutType>
#define ArrayXX Eigen::Array<float, Eigen::Dynamic, Eigen::Dynamic, MyLayoutType>
#define ArrayX Eigen::Array<float, Eigen::Dynamic, MyLayoutType>
#define Array2 Eigen::Array<float, 2, MyLayoutType>

#define VectorX Eigen::Matrix<float, Eigen::Dynamic, 1>
#define RowVectorX Eigen::Matrix<float, 1, Eigen::Dynamic>

#define TfN Eigen::Tensor<float, N, MyLayoutType>
#define TfN2 Eigen::Tensor<float, N2, MyLayoutType>
#define Tf5 Eigen::Tensor<float, 5, MyLayoutType>
#define Tf4 Eigen::Tensor<float, 4, MyLayoutType>
#define Tf3 Eigen::Tensor<float, 3, MyLayoutType>
#define Tf2 Eigen::Tensor<float, 2, MyLayoutType>
#define Tf1 Eigen::Tensor<float, 1, MyLayoutType> 

//typedef Eigen::TensorFixedSize<float, Sizes<1>> Tf1;
typedef Eigen::Array<float, card_count, 1> CardArray; // Perf: Change to dynamic for a lot of cards to avoid usage of stack!

#define TbN Eigen::TensorBase<float, N>
#define Tb5 Eigen::TensorBase<float, 5>
#define Tb4 Eigen::TensorBase<float, 4>
#define Tb3 Eigen::TensorBase<float, 3>
#define Tb2 Eigen::TensorBase<float, 2>
#define Tb1 Eigen::TensorBase<float, 1>

#define TmN Eigen::TensorMap<TfN, N>
#define Tm5 Eigen::TensorMap<Tf5, 5>
#define Tm4 Eigen::TensorMap<Tf4, 4>
#define Tm3 Eigen::TensorMap<Tf3, 3>
#define Tm2 Eigen::TensorMap<Tf2, 2>
#define Tm1 Eigen::TensorMap<Tf1, 1>

#define TmAxx Eigen::Map<ArrayXX>
#define TmAx Eigen::Map<ArrayX>

#define CardsT Eigen::TensorFixedSize<float, Eigen::Sizes<card_count>>
#define CfvsT Eigen::TensorFixedSize<float, Eigen::Sizes<players_count, card_count>>

#define ToAmxx_ex(tensor, rows, cols) Eigen::Map<ArrayXX>(tensor.data(), rows, cols)
#define ToAmx_ex(tensor, len) Eigen::Map<ArrayX>(tensor.data(), len)

#define ToAmxx(tensor) Eigen::Map<ArrayXX>(tensor.data(), tensor.dimension(0), tensor.dimension(1))
#define ToAmx(tensor) Eigen::Map<ArrayX>(tensor.data(), tensor.size())
#define ToTmx(target) Tm1(target.data(), target.size())


// Removes tensor second dim
#define Reshape(tensorBase, sizes) tensorBase.reshape(tensorBase, Util::ProcessSizes(tensorBase.size(), sizes))

// Removes tensor first dim
#define RemoveF1D(tensor, offset) tensor.chip(Util::ConvertOffset(tensor, offset, 0), 0)

// Removes tensor first two dims
#define RemoveF2D(tensor, offset1, offset2) tensor.chip(Util::ConvertOffset(tensor, offset1, 0), 0).chip(Util::ConvertOffset(tensor, offset2, 0), 0)

// Removes tensor first three dims
#define RemoveF3D(tensor, offset1, offset2, offset3) tensor.chip(Util::ConvertOffset(tensor, offset1, 0), 0).chip(Util::ConvertOffset(tensor, offset2, 0), 0).chip(Util::ConvertOffset(tensor, offset3, 0), 0)

// Removes tensor first four dims
#define RemoveF4D(tensor, offset1, offset2, offset3, offset4) tensor.chip(Util::ConvertOffset(tensor, offset1, 0), 0).chip(Util::ConvertOffset(tensor, offset2, 0), 0).chip(Util::ConvertOffset(tensor, offset3, 0), 0).chip(Util::ConvertOffset(tensor, offset4, 0), 0)

// Removes tensor second dim
#define Remove2D(tensor, offset) tensor.chip(Util::ConvertOffset(tensor, offset, 1), 1)

// Removes tensor third dim
#define Remove3D(tensor, offset) tensor.chip(Util::ConvertOffset(tensor, offset, 2), 2)

// Removes tensor fourth dim
#define Remove4D(tensor, offset) tensor.chip(Util::ConvertOffset(tensor, offset, 3), 3)

// Removes tensor fifth dim
#define Remove5D(tensor, offset) tensor.chip(Util::ConvertOffset(tensor, offset, 4), 4)

//Coping source tensor to the target tensor when target and source dimensions number are difference
#define CopyDif(target, source) assert(target.size() >= source.size()); memcpy(target.data(), source.data(), source.size() * sizeof(float));