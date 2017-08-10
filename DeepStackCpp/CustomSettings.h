#pragma once
#include <Eigen/Dense>
#include "game_settings.h"
#include <unsupported/Eigen/CXX11/Tensor>

typedef float mainDataType;

//typedef Eigen::TensorFixedSize<float, Sizes<1>> Tf1;
typedef Eigen::Array<float, card_count, 1> CardArray; // Perf: Change to dynamic for a lot of cards to avoid usage of stack!

#define TensorLayoutType Eigen::RowMajor//RowMajor
#define TfN Eigen::Tensor<float, N, TensorLayoutType>
#define Tf5 Eigen::Tensor<float, 5, TensorLayoutType>
#define Tf4 Eigen::Tensor<float, 4, TensorLayoutType>
#define Tf3 Eigen::Tensor<float, 3, TensorLayoutType>
#define Tf2 Eigen::Tensor<float, 2, TensorLayoutType>
#define Tf1 Eigen::Tensor<float, 1, TensorLayoutType>

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

#define TmAxx Eigen::Map<ArrayXXf>
#define TmAxx Eigen::Map<ArrayXXf>

#define CardsT Eigen::TensorFixedSize<float, Eigen::Sizes<card_count>>
#define CfvsT Eigen::TensorFixedSize<float, Eigen::Sizes<players_count, card_count>>



#define ToTmxx(tensor, rows, cols) Eigen::Map<ArrayXXf>(tensor.data(), rows, cols)
#define ToTmx(tensor, len) Eigen::Map<ArrayXf>(tensor.data(), len)

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