//#define NOMINMAX
//
//#include "catch.hpp"
//#include "bet_sizing.h"
//#include "Node.h"
//#include "card_to_string_conversion.h"
//#include "TreeBuilderParams.h"
//#include "tree_builder.h"
//#include "card_tools.h"
//#include "TreeCFR.h"
//#include "Util.h"
//#include "tree_values.h"
//
//#include <string>
//
//const float myEps = 0.001f;
//
//TEST_CASE("tree_values_Ks_1200_1200_range")
//{
//	TreeBuilderParams params;
//	Node node;
//	params.root_node = &node;
//	card_to_string_conversion converter;
//	params.root_node->board = converter.string_to_board("Ks");
//	params.root_node->street = 2;
//	params.root_node->current_player = P1;
//	params.root_node->bets << 1200, 1200;
//
//	tree_builder builder;
//	Node& tree = builder.build_tree(params);
//	card_tools cradTools;
//
//	ArrayXXf starting_ranges(players_count, card_count);
//	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
//	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);
//
//	TreeCFR tree_cfr;
//	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
//
//	tree_values tv;
//	tv._fill_ranges_dfs(tree, starting_ranges);
//
//	REQUIRE(tree.children[0]->ranges_absolute(0, 0) == Approx(0.1).epsilon(myEps));
//	REQUIRE(tree.children[0]->ranges_absolute(1, 0) == Approx(0.2).epsilon(myEps));
//
//	REQUIRE(tree.children[0]->ranges_absolute(0, 2) == Approx(0.0).epsilon(myEps));
//	REQUIRE(tree.children[0]->ranges_absolute(1, 2) == Approx(0.0).epsilon(myEps));
//
//	REQUIRE(tree.children[0]->ranges_absolute(0, 4) == Approx(0.1).epsilon(myEps));
//	REQUIRE(tree.children[0]->ranges_absolute(1, 4) == Approx(0.2).epsilon(myEps));
//
//	REQUIRE(tree.children[0]->ranges_absolute(0, 5) == Approx(0.1).epsilon(myEps));
//	REQUIRE(tree.children[0]->ranges_absolute(1, 5) == Approx(0.2).epsilon(myEps));
//}
//
//TEST_CASE("tree_values_Ks_100_100_range")
//{
//	TreeBuilderParams params;
//	Node node;
//	params.root_node = &node;
//	card_to_string_conversion converter;
//	params.root_node->board = converter.string_to_board("Ks");
//	params.root_node->street = 2;
//	params.root_node->current_player = P1;
//	params.root_node->bets << 100, 100;
//
//	tree_builder builder;
//	Node& tree = builder.build_tree(params);
//	card_tools cradTools;
//
//	ArrayXXf starting_ranges(players_count, card_count);
//	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
//	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);
//
//	TreeCFR tree_cfr;
//	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
//
//	tree_values tv;
//	tv._fill_ranges_dfs(tree, starting_ranges);
//
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(0, 0) == Approx(0.05).epsilon(myEps));
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(1, 0) == Approx(0.1).epsilon(myEps));
//
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(0, 1) == Approx(0.05).epsilon(myEps));
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(1, 1) == Approx(0.1).epsilon(myEps));
//
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(0, 2) == Approx(0.0).epsilon(myEps));
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(1, 2) == Approx(0.0).epsilon(myEps));
//
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(0, 5) == Approx(0.05).epsilon(myEps));
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(1, 5) == Approx(0.1).epsilon(myEps));
//}
//
//TEST_CASE("tree_values_100_100_range")
//{
//	TreeBuilderParams params;
//	Node node;
//	params.root_node = &node;
//	card_to_string_conversion converter;
//	params.root_node->board = converter.string_to_board("");
//	params.root_node->street = 1;
//	params.root_node->current_player = P1;
//	params.root_node->bets << 100, 100;
//
//	tree_builder builder;
//	Node& tree = builder.build_tree(params);
//	card_tools cradTools;
//
//	ArrayXXf starting_ranges(players_count, card_count);
//	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
//	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);
//
//	TreeCFR tree_cfr;
//	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
//
//	tree_values tv;
//	tv._fill_ranges_dfs(tree, starting_ranges);
//
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(0, 0) == Approx(0.01 * 4.1667).epsilon(myEps));
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(1, 0) == Approx(0.01 * 8.3333).epsilon(myEps));
//
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(0, 1) == Approx(0.01 * 4.1667).epsilon(myEps));
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(1, 1) == Approx(0.01 * 8.3333).epsilon(myEps));
//
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(0, 2) == Approx(0.01 * 4.1667).epsilon(myEps));
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(1, 2) == Approx(0.01 * 8.3333).epsilon(myEps));
//
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(0, 5) == Approx(0.01 * 4.1667).epsilon(myEps));
//	REQUIRE(tree.children[3]->children[0]->ranges_absolute(1, 5) == Approx(0.01 * 8.3333).epsilon(myEps));
//}
//
//TEST_CASE("exploitability_100_100_1_pass")
//{
//	TreeBuilderParams params;
//	Node node;
//	params.root_node = &node;
//	card_to_string_conversion converter;
//	params.root_node->board = converter.string_to_board("");
//	params.root_node->street = 1;
//	params.root_node->current_player = P1;
//	params.root_node->bets << 100, 100;
//
//	tree_builder builder;
//	Node& tree = builder.build_tree(params);
//	card_tools cradTools;
//
//	ArrayXXf starting_ranges(players_count, card_count);
//	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
//	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);
//
//	TreeCFR tree_cfr;
//	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
//
//	tree_values tv;
//	tv.compute_values(tree, &starting_ranges);
//	REQUIRE(tree.exploitability == Approx(175.52).epsilon(myEps));
//}
//
//TEST_CASE("exploitability_As_1200_1200_P2")
//{
//	TreeBuilderParams params;
//	Node node;
//	params.root_node = &node;
//	card_to_string_conversion converter;
//	params.root_node->board = converter.string_to_board("As");
//	params.root_node->street = 2;
//	params.root_node->current_player = P2;
//	params.root_node->bets << 1200, 1200;
//
//	tree_builder builder;
//	Node& tree = builder.build_tree(params);
//	card_tools cradTools;
//
//	ArrayXXf starting_ranges(players_count, card_count);
//	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
//	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);
//
//	TreeCFR tree_cfr;
//	tree_cfr.run_cfr(tree, starting_ranges, 1, 0);
//
//	tree_values tv;
//	tv.compute_values(tree, &starting_ranges);
//	REQUIRE(tree.exploitability == Approx(240).epsilon(myEps));
//}
//
//TEST_CASE("exploitability_100_100_2_pass")
//{
//	TreeBuilderParams params;
//	Node node;
//	params.root_node = &node;
//	card_to_string_conversion converter;
//	params.root_node->board = converter.string_to_board("Ks");
//	params.root_node->street = 2;
//	params.root_node->current_player = P2;
//	params.root_node->bets << 1200, 1200;
//
//	tree_builder builder;
//	Node& tree = builder.build_tree(params);
//	card_tools cradTools;
//
//	ArrayXXf starting_ranges(players_count, card_count);
//	starting_ranges.row(0) = cradTools.get_uniform_range(params.root_node->board);
//	starting_ranges.row(1) = cradTools.get_uniform_range(params.root_node->board);
//
//	TreeCFR tree_cfr;
//	tree_cfr.run_cfr(tree, starting_ranges, 2, 0);
//
//	tree_values tv;
//	tv.compute_values(tree, &starting_ranges);
//	REQUIRE(tree.exploitability == Approx(120.0).epsilon(myEps));
//}