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

const float myEps = 0.05f;

TEST_CASE("range_gen_empty_board")
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

	ArrayX bSum = sum.colwise().sum();
	bSum /= (genSize * gen_batch_size);

	REQUIRE(bSum(0) == Approx(0.19).epsilon(myEps));
	REQUIRE(bSum(1) == Approx(0.12).epsilon(myEps));
	REQUIRE(bSum(2) == Approx(0.19).epsilon(myEps));
	REQUIRE(bSum(3) == Approx(0.19).epsilon(myEps));
	REQUIRE(bSum(4) == Approx(0.12).epsilon(myEps));
	REQUIRE(bSum(5) == Approx(0.19).epsilon(myEps));
}

TEST_CASE("range_gen_Qs")
{
	card_to_string_conversion converter;
	card_tools tools;
	range_generator generator;

	ArrayXX ranges(gen_batch_size, card_count);
	ArrayX board = converter.string_to_board("Qs");
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

	ArrayX bSum = sum.colwise().sum();
	bSum /= (genSize * gen_batch_size);
	card_to_string_conversion conv;
	int card = conv.string_to_card("Qs");

	REQUIRE(bSum(0) == Approx(0.22).epsilon(myEps));
	REQUIRE(bSum(1) == Approx(0.18).epsilon(myEps));
	REQUIRE(bSum(2) == Approx(0.18).epsilon(myEps));
	REQUIRE(bSum(3) == Approx(0.18).epsilon(myEps));
	assert(card == 4);
	REQUIRE(bSum(4) == Approx(0).epsilon(myEps));
	REQUIRE(bSum(5) == Approx(0.22).epsilon(myEps));
}