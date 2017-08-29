#include "catch.hpp"
#include "bet_sizing.h"
#include "Node.h"
#include "card_to_string_conversion.h"
#include "TreeBuilderParams.h"
#include "tree_builder.h"
#include "card_tools.h"
#include "cfrd_gadget.h"
#include "arguments.h"
#include "range_generator.h"
#include <string>
#include <iostream>
using namespace std;

const float myEps = 0.01f;

TEST_CASE("range_gen")
{
	card_to_string_conversion converter;
	card_tools tools;
	range_generator generator;

	ArrayXX ranges(gen_batch_size, card_count);
	ArrayX board = converter.string_to_board("");
	generator.set_board(board);
	ArrayXX sum;

	size_t genSize = 1000;
	for (size_t i = 0; i < genSize; i++)
	{
		generator.generate_range(ranges);
		if (sum.size() == 0)
		{
			sum = ranges;
		}
		else
		{
			sum += ranges;
		}
	}

	ArrayX bSum = sum.rowwise().sum();
	bSum /= genSize;

	for (size_t i = 0; i < card_count; i++)
	{
		REQUIRE(bSum(0) == Approx(1).epsilon(myEps));
	}
}