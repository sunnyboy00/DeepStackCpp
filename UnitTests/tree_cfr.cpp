#include "catch.hpp"
#include "bet_sizing.h"
#include "Node.h"
#include "card_to_string_conversion.h"
#include "TreeBuilderParams.h"
#include "tree_builder.h"
#include "card_tools.h"
#include "TreeCFR.h"
#include "Util.h"

#include <string>

const float myEps = 0.001f;

TEST_CASE("regrets_calculation_Ks_1200_1200")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P1;
	params.root_node->bets << 1200, 1200;

	tree_builder builder;
	Node& tree = builder.build_tree(params);
	card_tools cradTools;

	ArrayXXf starting_ranges(players_count, card_count);
	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);

	TreeCFR tree_cfr;
	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
	REQUIRE(tree.regrets(0, 0) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 0) == Approx(780).epsilon(myEps));

	REQUIRE(tree.regrets(0, 1) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 1) == Approx(780).epsilon(myEps));

	REQUIRE(tree.regrets(0, 2) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 2) == Approx(0).epsilon(myEps));

	REQUIRE(tree.regrets(0, 3) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 3) == Approx(960).epsilon(myEps));

	REQUIRE(tree.regrets(0, 4) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 4) == Approx(540).epsilon(myEps));

	REQUIRE(tree.regrets(0, 5) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 5) == Approx(540).epsilon(myEps));
}

TEST_CASE("regrets_calculation_100_100_Ks")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("Ks");
	params.root_node->street = 2;
	params.root_node->current_player = P1;
	params.root_node->bets << 100, 100;

	tree_builder builder;
	Node& tree = builder.build_tree(params);
	card_tools cradTools;

	ArrayXXf starting_ranges(players_count, card_count);
	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);

	TreeCFR tree_cfr;
	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
	REQUIRE(tree.regrets(0, 0) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 0) == Approx(9.375).epsilon(myEps));
	REQUIRE(tree.regrets(2, 0) == Approx(16.87).epsilon(myEps));
	REQUIRE(tree.regrets(3, 0) == Approx(106.88).epsilon(myEps));

	REQUIRE(tree.regrets(0, 3) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 3) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(2, 3) == Approx(45).epsilon(myEps));
	REQUIRE(tree.regrets(3, 3) == Approx(285).epsilon(myEps));


	REQUIRE(tree.regrets(0, 5) == Approx(109.38).epsilon(myEps));
	REQUIRE(tree.regrets(1, 5) == Approx(41.875).epsilon(myEps));
	REQUIRE(tree.regrets(2, 5) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(3, 5) == Approx(0).epsilon(myEps));
}

TEST_CASE("regrets_calculation_100_100_2d_street")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("");
	params.root_node->street = 2;
	params.root_node->current_player = P1;
	params.root_node->bets << 100, 100;

	tree_builder builder;
	Node& tree = builder.build_tree(params);
	card_tools cradTools;

	ArrayXXf starting_ranges(players_count, card_count);
	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);

	TreeCFR tree_cfr;
	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
	REQUIRE(tree.regrets(0, 0) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 0) == Approx(4.6875).epsilon(myEps));
	REQUIRE(tree.regrets(2, 0) == Approx(2.3438e+01).epsilon(myEps));
	REQUIRE(tree.regrets(3, 0) == Approx(1.4844e+02).epsilon(myEps));

	REQUIRE(tree.regrets(0, 1) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 1) == Approx(4.6875).epsilon(myEps));
	REQUIRE(tree.regrets(2, 1) == Approx(2.3438e+01).epsilon(myEps));
	REQUIRE(tree.regrets(3, 1) == Approx(1.4844e+02).epsilon(myEps));

	REQUIRE(tree.regrets(0, 2) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 2) == Approx(1.8229e+01).epsilon(myEps));
	REQUIRE(tree.regrets(2, 2) == Approx(7.8125e+00).epsilon(myEps));
	REQUIRE(tree.regrets(3, 2) == Approx(4.9479e+01).epsilon(myEps));

	REQUIRE(tree.regrets(0, 3) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 3) == Approx(1.8229e+01).epsilon(myEps));
	REQUIRE(tree.regrets(2, 3) == Approx(7.8125e+00).epsilon(myEps));
	REQUIRE(tree.regrets(3, 3) == Approx(4.9479e+01).epsilon(myEps));

	REQUIRE(tree.regrets(0, 4) == Approx(2.5521e+01).epsilon(myEps));
	REQUIRE(tree.regrets(1, 4) == Approx(3.1771e+01).epsilon(myEps));
	REQUIRE(tree.regrets(2, 4) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(3, 4) == Approx(0).epsilon(myEps));

	REQUIRE(tree.regrets(0, 5) == Approx(2.5521e+01).epsilon(myEps));
	REQUIRE(tree.regrets(1, 5) == Approx(3.1771e+01).epsilon(myEps));
	REQUIRE(tree.regrets(2, 5) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(3, 5) == Approx(0).epsilon(myEps));
}

TEST_CASE("regrets_calculation_100_100_1_street")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("");
	params.root_node->street = 1;
	params.root_node->current_player = P1;
	params.root_node->bets << 100, 100;

	tree_builder builder;
	Node& tree = builder.build_tree(params);
	card_tools cradTools;

	ArrayXXf starting_ranges(players_count, card_count);
	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);


	TreeCFR tree_cfr;
	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
	REQUIRE(tree.regrets(0, 0) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 0) == Approx(1.7441e+01).epsilon(myEps));
	REQUIRE(tree.regrets(2, 0) == Approx(2.9442e+00).epsilon(myEps));
	REQUIRE(tree.regrets(3, 0) == Approx(1.5231e+02).epsilon(myEps));

	REQUIRE(tree.regrets(0, 1) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 1) == Approx(1.7441e+01).epsilon(myEps));
	REQUIRE(tree.regrets(2, 1) == Approx(2.9442e+00).epsilon(myEps));
	REQUIRE(tree.regrets(3, 1) == Approx(1.5231e+02).epsilon(myEps));

	REQUIRE(tree.regrets(0, 2) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 2) == Approx(1.7687e+01).epsilon(myEps));
	REQUIRE(tree.regrets(2, 2) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(3, 2) == Approx(5.4579e+01).epsilon(myEps));

	REQUIRE(tree.regrets(0, 3) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 3) == Approx(1.7687e+01).epsilon(myEps));
	REQUIRE(tree.regrets(2, 3) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(3, 3) == Approx(5.4579e+01).epsilon(myEps));

	REQUIRE(tree.regrets(0, 4) == Approx(3.1850e+01).epsilon(myEps));
	REQUIRE(tree.regrets(1, 4) == Approx(1.7933e+01).epsilon(myEps));
	REQUIRE(tree.regrets(2, 4) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(3, 4) == Approx(0).epsilon(myEps));

	REQUIRE(tree.regrets(0, 5) == Approx(3.1850e+01).epsilon(myEps));
	REQUIRE(tree.regrets(1, 5) == Approx(1.7933e+01).epsilon(myEps));
	REQUIRE(tree.regrets(2, 5) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(3, 5) == Approx(0).epsilon(myEps));
}

TEST_CASE("regrets_calculation_1200_1200_As_p2")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("As");
	params.root_node->street = 2;
	params.root_node->current_player = P2;
	params.root_node->bets << 1200, 1200;

	tree_builder builder;
	Node& tree = builder.build_tree(params);
	card_tools cradTools;

	ArrayXXf starting_ranges(players_count, card_count);
	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);

	TreeCFR tree_cfr;
	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
	REQUIRE(tree.regrets(0, 0) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 0) == Approx(0).epsilon(myEps));

	REQUIRE(tree.regrets(0, 1) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 1) == Approx(9.6000e+02).epsilon(myEps));

	REQUIRE(tree.regrets(0, 2) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 2) == Approx(6.0000e+02).epsilon(myEps));

	REQUIRE(tree.regrets(0, 3) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 3) == Approx(6.0000e+02).epsilon(myEps));

	REQUIRE(tree.regrets(0, 4) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 4) == Approx(1.2000e+02).epsilon(myEps));

	REQUIRE(tree.regrets(0, 5) == Approx(0).epsilon(myEps));
	REQUIRE(tree.regrets(1, 5) == Approx(1.2000e+02).epsilon(myEps));
}

TEST_CASE("strategy_calculation_100_100_1_street_1_pass")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("");
	params.root_node->street = 1;
	params.root_node->current_player = P1;
	params.root_node->bets << 100, 100;

	tree_builder builder;
	Node& tree = builder.build_tree(params);
	card_tools cradTools;

	ArrayXXf starting_ranges(players_count, card_count);
	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);


	TreeCFR tree_cfr;
	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
	REQUIRE((tree.strategy == 0.25).all());
}

TEST_CASE("strategy_calculation_100_100_1_street_10_pass")
{
	TreeBuilderParams params;
	Node node;
	params.root_node = &node;
	card_to_string_conversion converter;
	params.root_node->board = converter.string_to_board("");
	params.root_node->street = 1;
	params.root_node->current_player = P1;
	params.root_node->bets << 100, 100;

	tree_builder builder;
	Node& tree = builder.build_tree(params);
	card_tools cradTools;

	ArrayXXf starting_ranges(players_count, card_count);
	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);

	TreeCFR tree_cfr;
	tree_cfr.run_cfr(tree, starting_ranges, 10, 0);
	REQUIRE(tree.strategy(0, 0) == Approx(0.0250f).epsilon(myEps));
	REQUIRE(tree.strategy(1, 0) == Approx(0.4365f).epsilon(myEps));
	REQUIRE(tree.strategy(2, 0) == Approx(0.2859f).epsilon(myEps));
	REQUIRE(tree.strategy(3, 0) == Approx(0.2527f).epsilon(myEps));

	REQUIRE(tree.strategy(1, 2) == Approx(0.5030f).epsilon(myEps));
	REQUIRE(tree.strategy(2, 4) == Approx(0.1426).epsilon(myEps));
}