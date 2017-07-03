#pragma once
#include <Eigen/Dense>
#include "game_settings.h"

typedef float mainDataType;

typedef Eigen::Array<float, card_count, 1> CardArray; // Perf: Change to dynamic for a lot of cards to avoid usage of stack!
