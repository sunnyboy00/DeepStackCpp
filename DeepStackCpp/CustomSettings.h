#pragma once
#include <Eigen/Dense>
#include "game_settings.h"
#include <unsupported/Eigen/CXX11/Tensor>

typedef float mainDataType;


//typedef Eigen::TensorFixedSize<float, Sizes<1>> Tf1;
typedef Eigen::Array<float, card_count, 1> CardArray; // Perf: Change to dynamic for a lot of cards to avoid usage of stack!

#define TfN Tensor<float, N>
#define Tf5 Tensor<float, 5>
#define Tf4 Tensor<float, 4>
#define Tf3 Tensor<float, 3>
#define Tf2 Tensor<float, 2>
#define Tf1 Tensor<float, 1>

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